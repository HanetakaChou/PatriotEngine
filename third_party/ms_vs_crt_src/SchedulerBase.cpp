// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// SchedulerBase.cpp
//
// Implementation file of the metaphor for a concrt scheduler 
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
/// <summary>
///     Creates a scheduler that only manages internal contexts. Implicitly calls Reference.
///     If Attach is called, the scheduler is no longer anonymous because it is also managing the external
///     context where Attach was called.  To destroy an anonymous scheduler, Release needs to be called.
/// </summary>
/// <param name="policy">
///     [in] A const reference to the scheduler policy.
/// </param>
/// <returns>
///     A pointer to the new scheduler
/// </returns>
Scheduler* Scheduler::Create(__in const SchedulerPolicy& policy)
{
    ::Concurrency::details::SchedulerBase *pScheduler = ::Concurrency::details::SchedulerBase::Create(policy);
    pScheduler->Reference();
    return pScheduler;
}

/// <summary>
///     Allows a user defined policy to be used to create the default scheduler. It is only valid to call this API when no default
///     scheduler exists. Once a default policy is set, it remains in effect until the next valid callto the API.
/// </summary>
/// <param name="_Policy">
///     [in] The policy to be set as the default. The runtime will make a copy of the policy for its use, and the user
///     is responsible for the lifetime of the policy that is passed in.
/// </param>
void Scheduler::SetDefaultSchedulerPolicy(__in const SchedulerPolicy & _Policy)
{
    ::Concurrency::details::SchedulerBase::SetDefaultSchedulerPolicy(_Policy);
}

/// <summary>
///     Resets the default scheduler policy, and the next time a default scheduler is created, it will use the runtime's default policy settings.
/// </summary>
void Scheduler::ResetDefaultSchedulerPolicy()
{
    ::Concurrency::details::SchedulerBase::ResetDefaultSchedulerPolicy();
}

//
// Internal bit mask definitions for the shutdown gate.
//
#define SHUTDOWN_INITIATED_FLAG                 0x80000000
#define SUSPEND_GATE_FLAG                       0x40000000
#define SHUTDOWN_COMPLETED_FLAG                 0x20000000
#define GATE_COUNT_MASK                         0x1FFFFFFF
#define GATE_FLAGS_MASK                         0xE0000000

namespace details
{
    // Template specializations for types used by the scheduler.
    template <>
    inline unsigned int Hash<HANDLE, ExternalContextBase*>::HashValue(const HANDLE& key, int size)
    {
        ASSERT(size > 0);
        // Handle values are a multiple of 4.
        return (unsigned int) ((size_t)key/4) % size;
    }

    // The default scheduler lock protects access to both the default scheduler as well as the
    // default scheduler policy.
    SchedulerBase *SchedulerBase::s_pDefaultScheduler = NULL;
    SchedulerPolicy *SchedulerBase::s_pDefaultSchedulerPolicy = NULL;

    LONG SchedulerBase::s_initializedCount = 0;
    LONG SchedulerBase::s_oneShotInitializationState = ONESHOT_NOT_INITIALIZED;
    volatile LONG SchedulerBase::s_workQueueIdCounter = 0;
    DWORD SchedulerBase::t_dwContextIndex;

    // Number of suballocators for use by external contexts that are active in the process.
    volatile LONG SchedulerBase::s_numExternalAllocators = 0;

    // The max number of external contexts that could have suballocators at any given time.
    const int SchedulerBase::s_maxExternalAllocators = 32;

    // The maximum depth of the free pool of allocators.
    const int SchedulerBase::s_allocatorFreePoolLimit = 16;

    /// <summary>
    ///     Constructor for SchedulerBase.
    /// </summary>
    SchedulerBase::SchedulerBase(__in const ::Concurrency::SchedulerPolicy& policy) :
        m_policy(policy),
        m_refCount(0),
        m_attachCount(0),
        m_id(-1),
        m_boundContextCount(0),
        m_internalContextCountPlusOne(1),
        m_nextSchedulingRingIndex(0),
        m_contextIdCounter(-1),
        m_scheduleGroupIdCounter(-1),
        m_vprocShutdownGate(0),
        m_activeVProcCount(0),
        m_initialReference(0),
        m_enqueuedTaskCounter(0),
        m_dequeuedTaskCounter(0),
        m_enqueuedTaskCheckpoint(0),
        m_dequeuedTaskCheckpoint(0),
        m_pResourceManager(NULL),
        m_pSchedulerResourceManagement(NULL),
        m_externalThreadStatistics(NULL, 256, ListArray<ExternalStatistics>::DeletionThresholdInfinite),
        m_safePointDataVersion(0),
        m_safePointCommitVersion(0),
        m_safePointPendingVersion(0)
    {
        m_schedulerKind             = (::Concurrency::SchedulerType) policy.GetPolicyValue(::Concurrency::SchedulerKind);
        m_localContextCacheSize     = (unsigned short) policy.GetPolicyValue(::Concurrency::LocalContextCacheSize);
        m_schedulingProtocol        = (::Concurrency::SchedulingProtocolType) policy.GetPolicyValue(::Concurrency::SchedulingProtocol);

        //
        // This is a count before which we will **NOT** perform any throttling.  In the event of repeated latent blocking, we will reach
        // this number of threads rapidly.  By default, we choose this number to be 4x the number of cores.  If a client has specified a
        // MinConcurrency value that implies a greater number of vprocs than this, we will adjust the throttling limit upwards to 
        // MinConcurrency.  This may result in poorer overall throttling performance; however -- one would expect that most clients aren't
        // requesting > 4x oversubscription.
        //
        m_threadsBeforeThrottling   = max(::Concurrency::GetProcessorCount() * 4, policy.GetPolicyValue(::Concurrency::MinConcurrency));

        // Allocate a TLS slot to track statistics for threads alien to this scheduler
        m_dwExternalStatisticsIndex = TlsAlloc();
        if (m_dwExternalStatisticsIndex == TLS_OUT_OF_INDEXES)
        {
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
        }

    }

    SchedulerBase::~SchedulerBase()
    {
        Cleanup();
    }

    void SchedulerBase::Cleanup()
    {
        for (int idx = 0; idx < m_nodeCount; ++idx)
            delete m_nodes[idx];

        for (int idx = 0; idx < m_nodeCount; ++idx)
            delete m_rings[idx];

        delete [] m_nodes;
        delete [] m_rings;

        // Cleanup a TLS slot and allow a reuse
        TlsFree(m_dwExternalStatisticsIndex); 
        m_dwExternalStatisticsIndex = 0;

        m_pResourceManager->Release();
        SchedulerBase::StaticDestruction();
    }

    // race is fine -- only for inputting work
    SchedulingRing *SchedulerBase::GetNextSchedulingRing()
    {
        SchedulingRing *pRing = m_rings[m_nextSchedulingRingIndex];
        ASSERT(pRing != NULL);
        m_nextSchedulingRingIndex = GetNextValidSchedulingRingIndex(m_nextSchedulingRingIndex);
        return pRing;
    }

    int SchedulerBase::GetValidSchedulingRingIndex(int idx)
    {
        ASSERT(idx >= 0 && idx <= m_nodeCount);
        if (m_rings[idx] == NULL)
            return GetNextValidSchedulingRingIndex(idx);
        return idx;
    }

    int SchedulerBase::GetNextValidSchedulingRingIndex(int idx)
    {
        ASSERT(idx >= 0 && idx <= m_nodeCount);
        do
        {
            idx = (idx+1) % m_nodeCount;
        } while (m_rings[idx] == NULL);
        return idx;
    }

    SchedulingRing *SchedulerBase::GetNextSchedulingRing(const SchedulingRing *pOwningRing, SchedulingRing *pCurrentRing)
    {
        ASSERT(pCurrentRing != NULL && pOwningRing != NULL);

        SchedulingRing *pRing = m_rings[GetNextValidSchedulingRingIndex(pCurrentRing->Id())];
        ASSERT(pRing != NULL);
        if (pRing == pOwningRing)
            pRing = NULL;
        return pRing;
    }

    /// <summary>
    ///     Creates a scheduler instance
    /// </summary>
    /// <param name="policy">
    ///     [in] A const reference to the scheduler policy.
    /// </param>
    /// <returns>
    ///     A pointer to the new scheduler An exception is thrown if an error occurs.
    /// </returns>
    __ecount(1) SchedulerBase* SchedulerBase::Create(__in const SchedulerPolicy& policy)
    {
        SchedulerBase *pScheduler = CreateWithoutInitializing(policy);
        // Obtain hardware threads, initialize virtual processors, etc.
        pScheduler->Initialize();

        return pScheduler;
    }

    /// <summary>
    ///     Creates a scheduler instance
    /// </summary>
    /// <param name="policy">
    ///     [in] A const pointer to the scheduler policy.
    /// </param>
    /// <returns>
    ///     A pointer to the new scheduler An exception is thrown if an error occurs.
    /// </returns>
    __ecount(1) SchedulerBase* SchedulerBase::CreateWithoutInitializing(__in const SchedulerPolicy& policy)
    {
        policy._ValidateConcRTPolicy();
        CheckStaticConstruction();

        ::Concurrency::SchedulerType schedulerKind = (::Concurrency::SchedulerType) policy.GetPolicyValue(::Concurrency::SchedulerKind);
        SchedulerBase *pScheduler = NULL;

        if (schedulerKind == ::Concurrency::ThreadScheduler)
        {
            pScheduler = ThreadScheduler::Create(policy);
        }
        else
        {
            ASSERT(schedulerKind == ::Concurrency::UmsThreadDefault);
            pScheduler = UMSThreadScheduler::Create(policy);
        }

        ASSERT(pScheduler != NULL);

        return pScheduler;
    }

    /// <summary>
    ///     Generates a unique identifier for a context.
    /// </summary>
    unsigned int SchedulerBase::GetNewContextId()
    {
        return (unsigned int) InterlockedIncrement(&m_contextIdCounter);
    }

    /// <summary>
    ///     Generates a unique identifier for a schedule group.
    /// </summary>
    unsigned int SchedulerBase::GetNewScheduleGroupId()
    {
        return (unsigned int) InterlockedIncrement(&m_scheduleGroupIdCounter);
    }

    /// <summary>
    ///     Generates a unique identifier for a work queue (across scheduler instances in the process).
    /// </summary>
    unsigned int SchedulerBase::GetNewWorkQueueId()
    {
        return (unsigned int) InterlockedIncrement(&s_workQueueIdCounter);
    }

    /// <summary>
    ///     Anything which requires a one shot pattern of initialization with no destruction until termination goes here.
    /// </summary>
    void SchedulerBase::OneShotStaticConstruction()
    {
        _SpinCount::_Initialize();

        //
        // The TLS indicies must be one-shot as they are used outside the domain of guaranteed scheduler presence.  We cannot free them
        // until process-exit/CRT-unload or we'll have races with scheduler teardown/creation and outside APIs which require the TLS indicies.
        //

        t_dwContextIndex = TlsAlloc();
        if (t_dwContextIndex == TLS_OUT_OF_INDEXES)
        {
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
        }

        UMSThreadScheduler::OneShotStaticConstruction();
    }

    /// <summary>
    ///     Anything which requires a pattern of demand initialization upon first scheduler creation and destruction upon last
    ///     scheduler destruction goes here.
    /// </summary>
    void SchedulerBase::StaticConstruction()
    {
    }

    /// <summary>
    ///     Called to ensure static construction is performed upon creation of a scheduler.
    /// </summary>
    void SchedulerBase::CheckStaticConstruction()
    {
        _StaticLock::_Scoped_lock lockHolder(s_schedulerLock);

        if (InterlockedIncrement(&s_initializedCount) == 1) 
        {
            //
            // all static initialization here
            //
            StaticConstruction();

            if ((s_oneShotInitializationState & ONESHOT_INITIALIZED_FLAG) == 0)
            {
                OneShotStaticConstruction();

                //
                // This both guarantees a full fence and protects against simultaneous manipulation of the reference count stored within the lower
                // 31 bits of s_oneShotInitializationState.
                //
                InterlockedOr(&s_oneShotInitializationState, ONESHOT_INITIALIZED_FLAG);
            }
        }
    }

    /// <summary>
    ///     Performs one shot static destruction (at unload/process exit).
    /// </summary>
    void SchedulerBase::OneShotStaticDestruction()
    {
        UMSThreadScheduler::OneShotStaticDestruction();
        TlsFree(t_dwContextIndex); 
        t_dwContextIndex = 0;
    }

    /// <summary>
    ///     Called at unload/process exit to perform cleanup of one-shot initialization items.
    /// </summary>
    void SchedulerBase::CheckOneShotStaticDestruction()
    {
        //
        // This might happen at unload time and does not need to be governed by lock.  Further, at the time of calling in such circumstance,
        // all static and globals should already have destructed -- it would be bad form to touch s_schedulerLock even if it is presently
        // a wrapper around a POD type.  Note that a background thread might come through here but would never get past the InterlockedDecrement
        // unless we were at unload time.
        //
        LONG val = InterlockedDecrement(&s_oneShotInitializationState);
        if (val == ONESHOT_INITIALIZED_FLAG) // ref==0
        {
            //
            // Here, we are at unload time.
            //
            OneShotStaticDestruction();

            val = InterlockedAnd(&s_oneShotInitializationState, ~ONESHOT_INITIALIZED_FLAG);
            ASSERT(val == ONESHOT_INITIALIZED_FLAG);
        }
    }

    void SchedulerBase::StaticDestruction()
    {
        _StaticLock::_Scoped_lock lockHolder(s_schedulerLock);

        if (InterlockedDecrement(&s_initializedCount) == 0) 
        {
            //
            // all static destruction here
            //

            // We have exclusive access to the free pool, and therefore can use unsafe APIs.
            SubAllocator* pAllocator = s_subAllocatorFreePool.Pop();
            while (pAllocator != NULL)
            {
                delete pAllocator;
                pAllocator = s_subAllocatorFreePool.Pop();
            }
        }
    }

    /// <summary>
    ///     Initialize variables and request execution resources from the Resource Manager.
    /// </summary>
    void SchedulerBase::Initialize()
    {
        m_virtualProcessorAvailableCount = 0;
        m_virtualProcessorCount = 0;
        m_nodeCount = 0;

        // A SchedulerResourceManagement instance implements the interfaces required for communication with
        // the Resource Manager.
        m_pSchedulerResourceManagement = new SchedulerResourceManagement(this);
        m_pResourceManager = Concurrency::CreateResourceManager();

        m_id = Concurrency::GetSchedulerId();

        // Get the number of nodes on the machine so we can create a fixed array for scheduling nodes and
        // scheduling rings - obviating the need for locking these collections when we traverse them.
        m_maxNodes = GetProcessorNodeCount();

        m_rings = new SchedulingRing*[m_maxNodes];
        m_nodes = new SchedulingNode*[m_maxNodes];
        memset(m_rings, 0, sizeof(SchedulingRing*) * m_maxNodes);
        memset(m_nodes, 0, sizeof(SchedulingNode*) * m_maxNodes);

        // The RequestInitialVirtualProcessors API will invoke a scheduler callback to add new virtual processors to
        // the scheduler during the course of the API call. If this API succeeds, we can assume that scheduling
        // nodes have been populated with virtual processors representing resources allocated by the RM based on
        // values specified in the scheduler's policy.
        m_pSchedulerProxy = m_pResourceManager->RegisterScheduler(m_pSchedulerResourceManagement, CONCRT_RM_VERSION_1);
        m_pSchedulerProxy->RequestInitialVirtualProcessors(false);

        m_nextSchedulingRingIndex = GetValidSchedulingRingIndex(0);

        m_hSchedulerShutdownSync = CreateSemaphoreW(NULL, 0, 0x7FFFFFFF, NULL);
        if (m_hSchedulerShutdownSync == NULL)
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError())); // the RM process should probably die here

        m_pExternalContextTable = new Hash<HANDLE, ExternalContextBase*>();

        InitializeSchedulerEventHandlers();

        TraceSchedulerEvent(CONCRT_EVENT_START, TRACE_LEVEL_INFORMATION, m_id);
    }

    /// <summary>
    ///     Create a context from the default scheduler (possibly create the default too).
    /// </summary>
    ContextBase* SchedulerBase::CreateContextFromDefaultScheduler()
    {
        // If the context TLS value is NULL, the current thread is not attached to a scheduler. Find the
        // default scheduler and attach to it.

        SchedulerBase* pDefaultScheduler = GetDefaultScheduler();
        // Creating an external context on the current thread attaches the scheduler.
        ContextBase *pContext = pDefaultScheduler->AttachExternalContext(false);

        ASSERT((ContextBase*) TlsGetValue(t_dwContextIndex) == pContext);

        // GetDefaultScheduler takes a reference which is safe to release after the attach.
        pDefaultScheduler->Release();

        return pContext;
    }

    /// <summary>
    ///     Returns the ConcRT context attached to the current OS execution context. If one does not exist NULL is returned
    /// </summary>
    ContextBase *SchedulerBase::SafeFastCurrentContext()
    {
        return IsOneShotInitialized() ? (ContextBase*) TlsGetValue(t_dwContextIndex) : NULL;
    }

    /// <summary>
    ///     Returns the ConcRT context attached to the current OS execution context. If one does not exist NULL is returned
    ///     This is only callable if you know a-priori that all statics have been initialized.
    /// </summary>
    ContextBase *SchedulerBase::FastCurrentContext()
    {
        CORE_ASSERT(IsOneShotInitialized());
        return (ContextBase*) TlsGetValue(t_dwContextIndex);
    }

    /// <summary>
    ///     Returns a pointer to the ConcRT scheduler attached to the current thread. If one does not exist, it creates
    ///     a context and attaches it to the default scheduler.
    /// </summary>
    SchedulerBase* SchedulerBase::CurrentScheduler()
    {
        return CurrentContext()->GetScheduler();
    }

    /// <summary>
    ///     Returns a pointer to the current scheduler, if the current thread is attached to a ConcRT scheduler, null otherwise.
    ///     This is only callable if you know a-priori that all statics have been initialized.
    /// </summary>
    SchedulerBase *SchedulerBase::FastCurrentScheduler()
    {
        ContextBase * pContext = FastCurrentContext();
        return (pContext != NULL) ? pContext->GetScheduler() : NULL;
    }

    /// <summary>
    ///     Returns a pointer to the current scheduler, if the current thread is attached to a ConcRT scheduler, null otherwise.
    /// </summary>
    SchedulerBase *SchedulerBase::SafeFastCurrentScheduler()
    {
        ContextBase * pContext = SafeFastCurrentContext();
        return (pContext != NULL) ? pContext->GetScheduler() : NULL;
    }

    /// <summary>
    ///     Returns a pointer to the default scheduler. Creates one if it doesn't exist and tries to make it the default.
    ///     NOTE: The API takes an reference on the scheduler which must be released by the caller appropriately.
    /// </summary>
    SchedulerBase *SchedulerBase::GetDefaultScheduler()
    {
        // Acquire the lock in order to take a safe reference on the default scheduler.
        _StaticLock::_Scoped_lock _lock(s_defaultSchedulerLock);

        // If the default scheduler is non-null, try to reference it safely. If the reference fails,
        // we've encountered a scheduler that is in the middle of finalization => the thread finalizing
        // the scheduler will attempt to clear the value under write mode.
        if ((s_pDefaultScheduler == NULL) || !s_pDefaultScheduler->SafeReference())
        {
            SchedulerPolicy policy(0);

            // Note that the default scheduler policy is protected by the default scheduler lock.
            SchedulerPolicy * pDefaultPolicy = s_pDefaultSchedulerPolicy;
            if (pDefaultPolicy != NULL)
            {
                policy = *pDefaultPolicy;
            }

            // Either the default scheduler was null, or we found a scheduler that was in the middle of being finalized.
            // Create a scheduler and set it as the default.
            s_pDefaultScheduler = SchedulerBase::CreateWithoutInitializing(policy);

            // Obtain hardware threads, initialize virtual processors, etc.
            s_pDefaultScheduler->Initialize();

            // Create returns a scheduler with a reference count of 0. We need to reference the scheduler before releasing the lock.
            // to prevent a different thread from assuming this scheduler is shutting down because the ref count is 0.
            // The caller is responsible for decrementing it after attaching to the scheduler.
            s_pDefaultScheduler->Reference();
        }

        // We're holding on to a reference, so it is safe to return this scheduler.
        ASSERT(s_pDefaultScheduler != NULL);
        return s_pDefaultScheduler;
    }

    /// <summary>
    ///     Allows a user defined policy to be used to create the default scheduler. It is only valid to call this API when no default
    ///     scheduler exists, unless the parameter is NULL. Once a default policy is set, it remains in effect until the next valid call
    ///     to the API.
    /// </summary>
    /// <param name="_Policy">
    ///     [in] A pointer to the policy to be set as the default. The runtime will make a copy of the policy
    ///     for its use, and the user is responsible for the lifetime of the policy that is passed in. An argument of NULL will reset
    ///     the default scheduler policy, and the next time a default scheduler is created, it will use the runtime’s default policy settings.
    /// </param>
    void SchedulerBase::SetDefaultSchedulerPolicy(__in const SchedulerPolicy & _Policy)
    {
        _Policy._ValidateConcRTPolicy();

        bool fSetDefault = false;

        if (s_pDefaultScheduler == NULL)
        {
            // We can only set a non-null default policy if the default scheduler does not exist.

            _StaticLock::_Scoped_lock _lock(s_defaultSchedulerLock);

            // It's possible the default scheduler exists but is on its way out, i.e. its ref count is 0, and a different thread is about
            // acquire the write lock and set the value to null. We ignore this case, and allow the API to fail.
            if (s_pDefaultScheduler == NULL)
            {
                delete s_pDefaultSchedulerPolicy;
                s_pDefaultSchedulerPolicy = new SchedulerPolicy(_Policy);
                fSetDefault  = true;
            }
        }

        if (!fSetDefault)
        {
            throw default_scheduler_exists();
        }
    }

    /// <summary>
    ///     Resets the default scheduler policy, and the next time a default scheduler is created, it will use the runtime's default policy settings.
    /// </summary>
    void SchedulerBase::ResetDefaultSchedulerPolicy()
    {
        _StaticLock::_Scoped_lock _lock(s_defaultSchedulerLock);

        if (s_pDefaultSchedulerPolicy != NULL)
        {
            delete s_pDefaultSchedulerPolicy;
            s_pDefaultSchedulerPolicy = NULL;
        }
    }

    /// <summary>
    ///     Increments the reference count to the scheduler but does not allow a 0 to 1 transition. This API should
    ///     be used to safely access a scheduler when the scheduler is not 'owned' by the caller.
    /// </summary>
    /// <returns>
    ///     True if the scheduler was referenced, false, if the reference count was 0.
    /// </returns>
    bool SchedulerBase::SafeReference()
    {
        return SafeInterlockedIncrement(&m_refCount);
    }

    /// <summary>
    ///     Starts up an available virtual processor if one is found. The virtual processor is assigned a context
    ///     that starts its search for work in the schedule group specified.
    /// </summary>
    void SchedulerBase::StartupIdleVirtualProcessor(ScheduleGroupBase *pGroup, VirtualProcessor *pBias)
    {
        //
        // We **MUST** be in a hyper-critical region during this period.  There is an interesting scenario on UMS that makes this so:
        //
        //     - [VP A] can't find work and is in its search for work loop
        //     - [VP A] makes itself available 
        //     - [VP B] running context alpha adds a new work item and does a StartupIdleVirtualProcessor
        //     - [VP B] does a FindAvailableVirtualProcessor and claims VP A
        //     - [VP B] page faults / blocks
        //     - [VP A] finds context alpha in its final SFW pass
        //     - [VP A] tries to claim ownership of its virtual processor
        //     - [VP A] can't claim exclusive ownership because context alpha already did
        //     - [VP A] calls deactivate to wait for the corresponding activation.
        //     - [VP A] deadlocks with context alpha.  Since it is about to execute alpha, no one else can grab it.  Similarly,
        //              it's waiting on an activate which will only come from context alpha.
        //
        // Since this code runs on the primary anyway during completion list moves, hyper-crit should be safe.  This does mean that
        // this code must be extraordinarily careful about what it calls / does.  There can be NO MEMORY ALLOCATION or other arbitrary
        // Win32 calls in the UMS variant of this path.
        //
        ContextBase::StaticEnterHyperCriticalRegion();

        // The callers of this API MUST check that that the available virtual processor count in the scheduler
        // is non-zero before calling the API. We avoid putting that check here since it would evaluate to false
        // most of the time, and it saves the function call overhead on fast paths (chore push)
        VirtualProcessor *pVirtualProcessor = FindAvailableVirtualProcessor(pBias);

        if (pVirtualProcessor != NULL)
        {
            ActivateVirtualProcessor(pVirtualProcessor, pGroup);
        }

        ContextBase::StaticExitHyperCriticalRegion();
    }

    /// <summary>
    ///     Activate the given virtual processor
    /// </summary>
    void SchedulerBase::ActivateVirtualProcessor(VirtualProcessor *pVirtualProcessor, ScheduleGroupBase *pGroup)
    {
        // Initialize to a value of true, if this a virtual processor that doesn't have a context attached,
        // it has already been 'activated' previously.
        bool activated = true;
        //
        // Notify the scheduler that we're about to activate a new virtual processor. Do it only if this is
        // truly a new virtual processor and not the one that is sitting in the Dispatch loop waiting for
        // work to come in.
        //
        if (pVirtualProcessor->GetExecutingContext() == NULL)
        {
            activated = VirtualProcessorActive(true);
        }

        // If this is not a brand new virtual processor (i.e. an internal context is attached), we do nothing special to
        // synchronize with scheduler shutdown here. The shutdown code that cancels contexts will synchronize with us,
        // making sure a virtual processor is not activated twice.
        if (activated)
        {
            TRACE(TRACE_SCHEDULER, L"SchedulerBase::FindAvailableVirtualProcessor(vpid=%d,available=%d)",
                  pVirtualProcessor->GetId(), (int)pVirtualProcessor->IsAvailable());

            pVirtualProcessor->StartupWorkerContext(pGroup);
        }
        else
        {
            // If the scheduler has already shutdown, it is unable to activate new virtual processors.
            // The shutdown path synchronizes with VirtualProcessorActive, and fails the call if this is the case.
            // Restore the virtual processor availability - this may not be required, but there is no harm in doing
            // it here, and this will prevent errors in the future if we assume that all !available virtual processors
            // have worker contexts attached to them.
            pVirtualProcessor->MakeAvailable();
        }
    }

    /// <summary>
    ///     Looks for an available virtual processor in the scheduler, and returns it.
    /// </summary>
    VirtualProcessor* SchedulerBase::FindAvailableVirtualProcessor(VirtualProcessor *pBias)
    {
        // Direct or indirect callers of this API MUST check that that the available virtual processor count in the scheduler
        // is non-zero before calling the API. We avoid putting that check here since it would evaluate to false
        // most of the time, and it saves the function call overhead on fast paths (chore push)
        VirtualProcessor *pVProc = NULL;
        SchedulingNode *pBiasNode = NULL;

        //
        // Bias first towards the given virtual processor, secondly to its node, and thirdly to everyone else in order.
        //
        if (pBias != NULL)
        {
            pBiasNode = pBias->GetOwningNode();
            VirtualProcessor *pVProc = pBiasNode->FindAvailableVirtualProcessor(pBias);
            if (pVProc != NULL)
                return pVProc;
        }

        for (int idx = 0; idx < m_nodeCount; ++idx)
        {
            SchedulingNode *pNode = m_nodes[idx];
            if (pNode != NULL && pNode != pBiasNode)
            {
                // Perform a quick check of the processor count to avoid the function call overhead
                // on some fast paths (chore push operations) on a system with many nodes.
                if (pNode->m_virtualProcessorAvailableCount > 0)
                {
                    pVProc = pNode->FindAvailableVirtualProcessor();
                    if (pVProc != NULL)
                    {
                        break;
                    }
                }
            }
        }

        return pVProc;
    }

    /// <summary>
    ///     Steals a local runnable contexts from nodes in the scheduler other than the skip node provided.
    /// </summary>
    InternalContextBase *SchedulerBase::StealForeignLocalRunnableContext(SchedulingNode *pSkipNode)
    {
        ASSERT(pSkipNode != NULL);

        for (int i = 0; i < m_nodeCount; ++i)
        {
            if (m_nodes[i] != NULL && m_nodes[i] != pSkipNode)
            {
                ASSERT(m_nodes[i]->m_id == i);
                InternalContextBase *pContext = m_nodes[i]->StealLocalRunnableContext();
                if (pContext != NULL)
                    return pContext;
            }
        }

        return NULL;
    }

    /// <summary>
    ///     Determines how long in milliseconds until the next set of threads is allowed to be created.
    /// </summary>
    ULONG SchedulerBase::ThrottlingTime(ULONG stepWidth)
    {
        ULONG boundContextCount = GetNumberOfBoundContexts();
        if (boundContextCount < m_threadsBeforeThrottling)
            return 0;

        ULONG overage = (boundContextCount - m_threadsBeforeThrottling);

        //
        // We can instantly shoot up to m_threadsBeforeThrottling.  For all below notes, K is the stair-step width.  Note that we are 
        // hard limited to 8K threads on Win7 UMS currently.  This should have hefty slope to reach the thousands especially since this is per-scheduler
        // and we can have multiple schedulers in the process!
        //
        // After that, the next 100 threads will take approximately (1) seconds to create. // 100 threads
        //           , the next 200 threads will take approximately (8) seconds to create. // 300 threads
        //           , the next 300 threads will take approximately (20) seconds to create. // 600 threads
        //           , the next 900 threads will take approximately (6.5) minutes to create. // 1.5K threads (2.5m: 600-1000)
        //           , the next 1000 threads will take approximately (20) minutes to create. // 2.5K threads
        //           , the next 1500 threads will take approximately (1.5) hours to create. // 4K threads
        //           , the next 4000 threads will take approximately (12) hours to create. // 8K threads
        //           , we run out of resources.  Hopefully, we aren't repeatedly waiting on operations with multi-hour latency in a parallel loop.
        //
        ULONG delay = 0;

        if (overage < 100)
            delay = 5 + (overage / 10);
        else if (overage < 300)
            delay = 15 + 0 + (overage / 8);
        else if (overage < 600)
            delay = 53 + 7 + (overage / 5);
        else if (overage < 1500)
            delay = 180 + 0 + (overage / 4);
        else if (overage < 2500)
            delay = 555 + 0 + (overage / 3);
        else if (overage < 4000)
            delay = 1388 + 1112 + (overage / 3);
        else 
            delay = 3833 + 4367 + (overage / 2);

        return (delay * stepWidth);
    }

    /// <summary>
    ///     Acquires a new internal context of the appropriate type and returns it.  This can come from either
    ///     a free list within the scheduler, or be one newly allocated from the heap.
    /// </summary>
    InternalContextBase *SchedulerBase::GetInternalContext()
    {
        InternalContextBase *pContext = m_internalContextPool.Pop();

        if (pContext == NULL)
        {
            pContext = CreateInternalContext();
            AddContext(pContext);


            // The internal reference count on the scheduler *must* be incremented by the creator of the context.
            // The reference count will be released when the context is canceled. If the context is executing on
            // a thread proxy at the time it is canceled, it will decrement its own reference count before
            // leaving the dispatch loop for the final time. If it is on the idle pool - the thread/context
            // that cancels it is responsible for decrementing the reference count.
            IncrementInternalContextCount();
        }

        //
        // IMPORTANT NOTE: It is possible that there is a thread proxy still in the process of executing this
        // context's dispatch loop. This is because on going idle, contexts add themselves to the idle pool,
        // and proceed to leave their dispatch loops - they could be picked up and re-initialized before they
        // have actually left the routine.
        //
        // We must be careful *not* to re-initialize any variables the context uses after the point at which it
        // adds itself to the idle list, until the time it has left the dispatch routine.
        //
        CORE_ASSERT(pContext != NULL);

        //
        // Note also that there are other fields which need to be touched until m_blockedState is set.  When we reuse a context, we must spin until
        // that bit is set.  Newly created contexts are started blocked.
        //
        pContext->SpinUntilBlocked();

        // Context shall be marked as idle.
        CORE_ASSERT(pContext->IsIdle());

        // Bind a thread proxy to the context
        pContext->m_pThreadProxy = NULL;

        InterlockedIncrement(&m_boundContextCount);
        GetSchedulerProxy()->BindContext(pContext);
        //__faststorefence();

        CMTRACE(MTRACE_EVT_CONTEXT_ACQUIRED, pContext, NULL, NULL);

#if defined(_DEBUG)
        pContext->NotifyAcquired();
#endif // _DEBUG

        pContext->ClearCriticalRegion();

        return pContext;
    }

    /// <summary>
    ///     Enqueues a context into m_allContexts
    /// </summary>
    void SchedulerBase::AddContext(InternalContextBase * pContext)
    {
        ContextNode *pNode = new ContextNode(pContext);
        m_allContexts.Push(pNode);
    }

    ///<summary>
    ///     Releases an internal context of the appropriate to the scheduler's idle pool.
    ///</summary>
    void SchedulerBase::ReleaseInternalContext(InternalContextBase *pContext)
    {
#if defined(_DEBUG)
        pContext->m_fEverRecycled = true;
        pContext->SetDebugBits(CTX_DEBUGBIT_RELEASED);
#endif // _DEBUG

        InterlockedDecrement(&m_boundContextCount);

        // Context shall be marked as idle.
        CORE_ASSERT(pContext->IsIdle());

        // We keep all contexts created by the scheduler. Deleting/canceling these contexts would required us to
        // remove it from the list of 'all contexts' (m_allContexts), which we use during finalization to detect
        // blocked contexts, and would require an additional level of synchronization there. Since idle contexts
        // do not have backing thread proxies, this is not a problem.
        m_internalContextPool.Push(pContext);
    }

    /// <summary>
    ///     Gets a realized chore from the idle pool, creating a new one if the idle pool is empty.
    /// </summary>
    RealizedChore * SchedulerBase::GetRealizedChore(TaskProc pFunction, void * pParameters)
    {
        RealizedChore * pChore = m_realizedChorePool.Pop();

        if (pChore == NULL)
        {
            pChore = new RealizedChore(pFunction, pParameters);
        }
        else
        {
            pChore->Initialize(pFunction, pParameters);
        }
        return pChore;
    }

    ///<summary>
    ///     Releases an external context of the to the scheduler's idle pool, destroying it if the idle pool is full.
    ///</summary>
    void SchedulerBase::ReleaseRealizedChore(RealizedChore * pChore)
    {
        // Try to maintain the max size of the free pool somewhere close to num vprocs * 32. It is
        // not an exact upper limit.
        if (m_realizedChorePool.Count() < (m_virtualProcessorCount << 5))
        {
            m_realizedChorePool.Push(pChore);
        }
        else
        {
            delete pChore;
        }
    }

    /// <summary>
    ///     References the anonymous schedule group, creating it if it doesn't exists, and returns a pointer to it.
    /// </summary>
    ScheduleGroupBase* SchedulerBase::GetAnonymousScheduleGroup()
    {
        ContextBase *pContext = FastCurrentContext();
        if (pContext == NULL || pContext->IsExternal() || pContext->GetScheduler() != this)
        {
            return GetNextSchedulingRing()->GetAnonymousScheduleGroup();
        }
        else
        {
            // The current context is an internal context on the 'this' scheduler.
            InternalContextBase * pInternalContext = static_cast<InternalContextBase*> (pContext);
            return pInternalContext->GetScheduleGroup()->GetSchedulingRing()->GetAnonymousScheduleGroup();
        }
    }

    /// <returns>
    ///     Returns a copy of the policy this scheduler is using.  No error state.
    /// </returns>
    SchedulerPolicy SchedulerBase::GetPolicy() const
    {
        return m_policy;
    }

    /// <summary>
    ///     Increments a reference count to this scheduler to manage lifetimes over composition.
    ///     This reference count is known as the scheduler reference count.
    /// </summary>
    /// <returns>
    ///     The resulting reference count is returned.  No error state.
    /// </returns>
    unsigned int SchedulerBase::Reference()
    {
        ASSERT(m_internalContextCountPlusOne > 0);
        LONG val = InterlockedIncrement(&m_refCount);
        if (val == 1)
        {
            //
            // This could be an initial reference upon the scheduler from a creation path or it could be
            // the case that an unblocked context from a scheduler decided to attach the scheduler somewhere.
            // In the second case, we need to resurrect the scheduler and halt the shutdown attempt.
            //
            if (m_initialReference > 0)
            {
                //
                // This should only come from an **INTERNAL** context on this scheduler; otherwise, the client is
                // being very bad and racing with shutdown for a nice big crash.
                //
                ContextBase* pCurrentContext = SchedulerBase::FastCurrentContext();

                if ((pCurrentContext == NULL ) || (pCurrentContext->IsExternal()) || (pCurrentContext->GetScheduler() != this))
                {
                    throw improper_scheduler_reference();
                }

                Resurrect();
            }
            else
            {
                InterlockedExchange(&m_initialReference, 1);
            }

        }
        return (unsigned int)val;
    }
    
    /// <summary>
    ///     Decrements this scheduler’s reference count to manage lifetimes over composition.
    ///     A scheduler starts the shutdown protocol when the scheduler reference count goes to zero.
    /// <summary>
    unsigned int SchedulerBase::Release()
    {
        LONG val = InterlockedDecrement(&m_refCount);
        if (val == 0)
        {
            PhaseOneShutdown();
        }

        return (unsigned int)val;
    }
    
    /// <summary>
    ///     Causes the OS event object ‘eventObject’ to be set when the scheduler shuts down and destroys itself.
    /// </summary>
    /// <param name="eventObject">
    ///     [in] A valid event object.
    /// </param>
    void SchedulerBase::RegisterShutdownEvent(__in HANDLE eventObject)
    {
        if (eventObject == NULL || eventObject == INVALID_HANDLE_VALUE)
        {
            throw std::invalid_argument("eventObject");
        }

        HANDLE hEvent = NULL;

        if (!DuplicateHandle(GetCurrentProcess(),
                             eventObject,
                             GetCurrentProcess(),
                             &hEvent,
                             0,
                             FALSE,
                             DUPLICATE_SAME_ACCESS))
        {
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
        }
        else
        {
            WaitNode *pNode = new WaitNode;
            pNode->m_hEvent = hEvent;
            m_finalEvents.AddTail(pNode);
        }
    }

    /// <summary>
    ///     Attaches this scheduler to the calling thread. Implicitly calls Reference.  After this function is called,
    ///     the calling thread is then managed by the scheduler and the scheduler becomes the current scheduler.
    /// </summary>
    void SchedulerBase::Attach()
    {
        const SchedulerBase* pCurrentScheduler = FastCurrentScheduler();

        // A context is not allowed to re-attach to its current scheduler.
        if (pCurrentScheduler == this)
            throw improper_scheduler_attach();

        // Attaching to the scheduler involves incrementing reference counts on the scheduler and creating a context data structure 
        // corresponding to the new scheduler for the current thread.
        ASSERT(m_internalContextCountPlusOne > 0);

        // External context creation will reference the scheduler, create the context data structure and store the context and
        // scheduler values in TLS.
        AttachExternalContext(true);

        TraceSchedulerEvent(CONCRT_EVENT_ATTACH, TRACE_LEVEL_INFORMATION, m_id);
    }

    ///<summary>
    ///     Detaches the current scheduler from the calling thread and restores the previously attached scheduler as the
    ///     current scheduler.  Implicitly calls Release().  After this function is called, the calling thread is then managed
    ///     by the scheduler that was previously activated via Create() or Attach().
    ///</summary>
    void SchedulerBase::Detach()
    {
        ContextBase* pContext = SchedulerBase::FastCurrentContext();

        ASSERT(pContext != NULL);
        if (!pContext->IsExternal())
        {
            throw improper_scheduler_detach();
        }

        ExternalContextBase* pExternalContext = static_cast<ExternalContextBase*>(pContext);

        if (!pExternalContext->WasExplicitlyAttached())
        {
            // Only contexts that attached explicitly either via the attach api, or the current scheduler creation api
            // are allowed to detach.

            throw improper_scheduler_detach();
        }

        unsigned int schedulerId = m_id;

        // External context detachment will release references on the scheduler, and remove the context from TLS on explicit detaches.
        DetachExternalContext(pExternalContext, true);

        // It is *UNSAFE* to touch 'this', after the DetachExternalContext call, since the caller does not have a reference on the 
        // scheduler and it could get deleted in the meantime.
        TraceSchedulerEvent(CONCRT_EVENT_DETACH, TRACE_LEVEL_INFORMATION, schedulerId);
    }

    /// <summary>
    ///     Creates an external context and attaches it to the calling thread. Called when a thread attaches to a scheduler.
    /// </summary>
    ExternalContextBase * SchedulerBase::AttachExternalContext(bool explicitAttach)
    {
        ContextBase * pCurrentContext = SchedulerBase::FastCurrentContext();

        if (pCurrentContext != NULL)
        {
            ASSERT(explicitAttach);

            if (pCurrentContext->m_pScheduler == this)
            {
                // A context is not allowed to re-attach to its own scheduler.
                throw improper_scheduler_attach();
            }
            // Check if this is an internal context. If so, it is presumably consuming a virtual processor
            // on a different scheduler (the parent scheduler). Since a new ConcRT context will be
            // associated with the OS context during this API, the parent scheduler should get its virtual processor back.
            if (!pCurrentContext->IsExternal())
            {
                //
                // If the underlying previous context was a UMS thread, the LeaveScheduler call will result in a
                // SwitchTo(..., Nesting) to the RM which will immediately transmogrify the UMS thread into a "virtual"-thread.
                // From the perspective of the scheduler, it will behave identically to a thread.  There's more overhead in this,
                // but the functionality is identical.
                //
                static_cast<InternalContextBase *>(pCurrentContext)->LeaveScheduler();
            }
            // We also clear out the context/scheduler TLS values here. Since we've saved the current context,
            // we remember it as the parent context of the new context we're about to create. The reason for
            // clearing TLS, is that if code that executes between this point and the point at which the new
            // TLS values are setup, relies on the current context in TLS, it might behave undeterministically.
            // e.g. Creating an external context looks at the anyonymous schedule group - since the current
            // context is an internal context on the previous scheduler, we don't get what we expect..
            pCurrentContext->ClearContextTls();
        }
        // Take reference counts on the scheduler.
        ReferenceForAttach();

        ExternalContextBase * pContext = GetExternalContext(explicitAttach);

        // Save the new context into the TLS slot reserved for the same.
        pContext->PushContextToTls(pCurrentContext);

        // The thread continues to run. It does not need to wait to be scheduled on a virtual processor.
        return pContext;
    }

    /// <summary>
    ///     Detaches an external context from the scheduler.
    /// </summary>
    /// <param name="pContext">
    ///     The external context being detached.
    /// </param>
    /// <param name="explicitDetach">
    ///     Whether this was the result of an explicit detach or the thread exiting.
    /// </param>
    void SchedulerBase::DetachExternalContext(ExternalContextBase * pContext, bool explicitDetach)
    {
        // External context destruction will remove the values from TLS and cleanup the context data structure
        ContextBase * pParentContext = NULL;

        if (explicitDetach)
            pParentContext = pContext->PopContextFromTls();

        ReleaseExternalContext(pContext);

        // Release reference counts on the scheduler.
        ReleaseForDetach();

        // The this pointer is *cannot be dereferenced* after the Release call.

        if ((pParentContext != NULL) && (!pParentContext->IsExternal()))
        {
            ASSERT(explicitDetach);
            // An internal parent context must be rescheduled on its scheduler.
            static_cast<InternalContextBase *>(pParentContext)->RejoinScheduler();
        }
    }

    /// <summary>
    ///     Gets an external context from the idle pool, creating a new one if the idle pool is empty
    /// </summary>
    ExternalContextBase * SchedulerBase::GetExternalContext(bool explicitAttach)
    {
        ExternalContextBase * pContext = m_externalContextPool.Pop();

        if (pContext == NULL)
        {
            pContext = new ExternalContextBase(this, explicitAttach);
        }
        else
        {
            pContext->PrepareForUse(explicitAttach);
        }
        return pContext;
    }

    ///<summary>
    ///     Releases an external context of the to the scheduler's idle pool, destroying it if the idle pool is full.
    ///</summary>
    void SchedulerBase::ReleaseExternalContext(ExternalContextBase * pContext)
    {
        // Try to maintain the max size of the free pool somewhere close to m_virtualProcessorCount. It is
        // not an exact upper limit.
        if (m_externalContextPool.Count() < m_virtualProcessorCount)
        {
            pContext->RemoveFromUse();
            m_externalContextPool.Push(pContext);
        }
        else
        {
            _InternalDeleteHelper<ExternalContextBase>(pContext);
        }
    }

    /// <summary>
    ///     Create a schedule group within this scheduler.
    /// </summary>
    /// <returns>
    ///     A reference to a new ScheduleGroup.
    /// </returns>
    ScheduleGroup* SchedulerBase::CreateScheduleGroup()
    {
        // CreateScheduleGroup references the schedule group for the caller.
        ScheduleGroup *pScheduleGroup = GetNextSchedulingRing()->CreateScheduleGroup();
        return pScheduleGroup;
    }

    /// <summary>
    ///     Create a light-weight task (cf. Task below) within this scheduler in an implementation dependent schedule group.
    /// </summary>
    /// <param name="proc">
    ///     [in] A pointer to the main function of a task.
    /// </param>
    /// <param name="data">
    ///     [in] A void pointer to the data that will be passed in to the task.
    /// </param>
    void SchedulerBase::ScheduleTask(__in TaskProc proc, __in void *data)
    {
        ScheduleGroupBase *pGroup = NULL;

        //
        // If we are currently executing in the context of a particular schedule group, the work should go there instead of in an anonymous schedule group.
        // We should not easily create cross-group dependencies.
        //
        // Note that it is entirely possible that the current context is on a DIFFERENT scheduler (e.g.: we're scheduling work on scheduler 2 within a thread
        // bound to scheduler 1).  In this case, we do **NOT** want to look at the current group -- we just pick an **appropriate** anonymous group within
        // scheduler 1.
        //
        ContextBase *pCurrentContext = FastCurrentContext();
        if (pCurrentContext != NULL && pCurrentContext->GetScheduler() == this)
        {
            pGroup = pCurrentContext->GetScheduleGroup();
            ASSERT(pGroup != NULL);
        }
        else
        {
            //
            // This task does not have an associated schedule group, assign it to the anonymous
            // schedule group for the scheduler
            //
            pGroup = GetAnonymousScheduleGroup();
        }

        pGroup->ScheduleTask(proc, data);
    }

    /// <summary>
    ///     Initialize event handlers or background threads
    /// </summary>
    void SchedulerBase::InitializeSchedulerEventHandlers()
    {
        // All handlers are created on demand.
    }

    /// <summary>
    ///     Destroy event handlers or background threads
    /// </summary>
    void SchedulerBase::DestroySchedulerEventHandlers()
    {
        {
            _NonReentrantBlockingLock::_Scoped_lock lock(m_backgroundLock);

            ContextExitEventHandler *pHandler = m_allHandlers.First();
            ContextExitEventHandler *pNextHandler = NULL;
            while (pHandler != NULL)
            {
                // We need to snap the next handler up front, because as soon as we set the event below, the background thread
                // could wake up and delete pHandler.
                pNextHandler = m_allHandlers.Next(pHandler);

                pHandler->m_fCanceled = true;
                SetEvent(pHandler->m_hWakeEventHandler);
                pHandler = pNextHandler;
            }
        }
    }

    /// <summary>
    ///     Thread handles for external contexts that are implicitly attached to the scheduler are used to track
    ///     thread exit. A background thread (or threads) waits on a maximum of MAXIMUM_WAIT_OBJECTS -1 thread handles
    ///     and responds to thread exit by releasing references on the scheduler and freeing other memory resources
    ///     associated with the external contexts.
    /// </summary>
    /// <param name="pContext">
    ///     The external context whose handle is to be tracked.
    /// </param>
    void SchedulerBase::RegisterForExitTracking(ExternalContextBase* pContext)
    {
        ContextExitEventHandler* pHandler = NULL;
        bool newHandler = false;

        ASSERT(!pContext->WasExplicitlyAttached());

        {
            _NonReentrantBlockingLock::_Scoped_lock lock(m_backgroundLock);

            // Add the mapping for this context handle to the hash table 
            ASSERT(!m_pExternalContextTable->Exists(pContext->GetPhysicalContext()));
            m_pExternalContextTable->Insert(pContext->GetPhysicalContext(), pContext);

            // Check the head of the list of available handlers, if one is not available, create a new handler.
            ListEntry* listEntry = m_availableHandlers.First();

            if (listEntry == NULL)
            {
                // Create a new handler
                pHandler = new ContextExitEventHandler();

                pHandler->m_fCanceled = false;

                // Event handlers take an internal reference on the scheduler which is released when they exit.
                IncrementInternalContextCount();
                pHandler->m_pScheduler = this;

                pHandler->m_hWakeEventHandler = CreateEventW(NULL, FALSE, FALSE, NULL);
                memset(pHandler->m_waitHandleArray, 0, 64 * sizeof(HANDLE));
                pHandler->m_waitHandleArray[0] = pHandler->m_hWakeEventHandler;

                // A new event handler is created when the scheduler tries to register a context handle but there
                // are no available slots. The handle is passed in to avoid the extra notification.
                pHandler->m_waitHandleArray[1] = pContext->GetPhysicalContext();
                pHandler->m_handleCount = 2;
                pHandler->m_newHandleCount = 2;
                m_allHandlers.AddTail(pHandler);

                // Add it to the list of handlers with available slots for context registration.
                m_availableHandlers.AddHead(&pHandler->m_availableChain);

                // The event handler is fully initialized at this point. The background thread for it is created
                // after releasing the lock.
                newHandler = true;
            }
            else
            {
                pHandler = CONTAINING_RECORD(listEntry, ContextExitEventHandler, m_availableChain);
                // There should be space for one more
                ASSERT(pHandler->m_newHandleCount < MAXIMUM_WAIT_OBJECTS);
                pHandler->m_waitHandleArray[pHandler->m_newHandleCount++] = pContext->GetPhysicalContext();

                if (pHandler->m_newHandleCount == MAXIMUM_WAIT_OBJECTS)
                {
                    // The handler has no more slots - remove it from ok.the list of available handlers.
                    m_availableHandlers.Remove(listEntry);
                }

                // Wake the event handler for the first new event added to the array - avoid the SetEvent call
                // in cases where two or more threads added handles before the thread had a chance to wake up.
                if ((pHandler->m_newHandleCount - pHandler->m_handleCount) == 1)
                {
                    SetEvent(pHandler->m_hWakeEventHandler);
                }
            }
        }

        if (newHandler)
        {
            ASSERT (pHandler != NULL);
            HANDLE threadHandle = LoadLibraryAndCreateThread(NULL,
                                               DEFAULTCONTEXTSTACKSIZE,
                                               BackgroundThreadProc,
                                               pHandler,
                                               0,
                                               NULL);

            CloseHandle(threadHandle);
        }
    }

    /// <summary>
    ///     Main thread procedure for the background threads
    /// </summary>
    DWORD CALLBACK SchedulerBase::BackgroundThreadProc(LPVOID lpParameter)
    {
        ContextExitEventHandler* pHandler = reinterpret_cast<ContextExitEventHandler *>(lpParameter);
        pHandler->m_pScheduler->WaitForSchedulerEvents(pHandler);
        delete pHandler;
        FreeLibraryAndDestroyThread(0);
        return 0;
    }

    /// <summary>
    ///     Wait loop for scheduler events (as indicated by the handler)
    /// </summary>
   void SchedulerBase::WaitForSchedulerEvents(ContextExitEventHandler * pHandler)
   {
        //
        // The thread starts out by waiting on the wait handles. The first of these is an event controlled 
        // by the scheduler. This event is signalled when the scheduler encounters a situation where it 
        // needs to break this thread out of its wait state and do some processing. An example of such 
        // processing is adding a new wait handle to the array. The remaining wait handles are handles to 
        // external context - these are signalled when the thread exits.
        //
        // A reference to the scheduler was taken on behalf of the this thread (by the thread that created the
        // event handler, so it can safely access the scheduler memory here. This reference is released by the 
        // background thread itself before it exits.
        //
        for (;;)
        {
            ASSERT(pHandler->m_handleCount <= MAXIMUM_WAIT_OBJECTS);
            DWORD waitResult = WaitForMultipleObjects(pHandler->m_handleCount, pHandler->m_waitHandleArray, false, INFINITE);

            if (waitResult == WAIT_FAILED)
            {
                throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
            }
            
            if (pHandler->m_fCanceled)
            {
                break;
            }

            int waitHandleIndex = waitResult - WAIT_OBJECT_0;
            ProcessContextExitEvents(pHandler, waitHandleIndex);
        }

        CloseHandle(pHandler->m_hWakeEventHandler);
        // There's no need to remove the event handers from the lists.

        // NOTE: Decrementing the internal context context count could finalize the scheduler - it is not safe to touch
        // *this* after this point.
        DecrementInternalContextCount();
    }

    /// <summary>
    ///     Processes external context exit events for external contexts that get implicitly attached to schedulers
    /// </summary>
    void SchedulerBase::ProcessContextExitEvents(ContextExitEventHandler* pHandler, int waitHandleIndex)
    {
        ExternalContextBase *pContext = NULL;
        HANDLE threadHandle = INVALID_HANDLE_VALUE;

        {
            _NonReentrantBlockingLock::_Scoped_lock lock(m_backgroundLock);

            if (waitHandleIndex == 0)
            {
                // A new thread handle or handles need to be added to the wait array. The new handle count 
                // variable is updated under the lock by the code that creates the external context.
                pHandler->m_handleCount = pHandler->m_newHandleCount;
                ASSERT(pHandler->m_handleCount <= MAXIMUM_WAIT_OBJECTS);
            }
            else
            {
                // An external context we were waiting on has exited.

                // Handles could've been added to the array before we acquired the lock, so take that into account
                // as well. This may lead to a spurious wakeup, since the event handle has been signaled but there
                // are no more thread handles to add, but it is rare.
                pHandler->m_handleCount = pHandler->m_newHandleCount = pHandler->m_newHandleCount - 1;
                ASSERT((pHandler->m_handleCount > 0) && (pHandler->m_handleCount < MAXIMUM_WAIT_OBJECTS));

                ASSERT(waitHandleIndex < MAXIMUM_WAIT_OBJECTS);
                threadHandle = pHandler->m_waitHandleArray[waitHandleIndex];

                // Move the last handle in the array into the place of the handle that just exited.
                if ((pHandler->m_handleCount > 1) && (waitHandleIndex < pHandler->m_handleCount))
                {
                    pHandler->m_waitHandleArray[waitHandleIndex] = pHandler->m_waitHandleArray[pHandler->m_handleCount];
                }

                if (pHandler->m_handleCount == MAXIMUM_WAIT_OBJECTS - 1)
                {
                    // This handler is now available for adding more thread handles to.
                    m_availableHandlers.AddHead(&pHandler->m_availableChain);
                }
                else if (pHandler->m_handleCount == 1 && m_availableHandlers.Count() > 1)
                {
                    // This handler is not waiting on thread handles anymore, and there is at least one available handler
                    // to service future requests. We can cancel this handler and let its thread exit.
                    m_availableHandlers.Remove(&pHandler->m_availableChain);
                    m_allHandlers.Remove(pHandler);

                    // The thread will exit once it returns from this function and finds that its canceled flag is set to true.
                    // We still have its reference on the scheduler for the remainder of this function.
                    pHandler->m_fCanceled = true;
                    SetEvent(pHandler->m_hWakeEventHandler);
                }

                ASSERT(pContext == NULL);
                bool retVal = m_pExternalContextTable->FindAndDelete(threadHandle, &pContext);

                ASSERT(retVal && (pContext != NULL));

                // destroy the context after releasing the lock
            }
        }

        if (pContext != NULL)
        {
            DetachExternalContext(pContext, false);
            pContext = NULL;
        }
    }

    //  Finalization:
    //
    // The active vproc count and idle vproc count are encoded into the vproc shutdown gate, in addition to the SHUTDOWN and SUSPEND flags.
    //
    //                 SHUTDOWN_INITIATED_FLAG (0x80000000)
    //                   |
    //                   |
    //                   |         <----- 29 bit gate count ------>
    //                 +---+---+---+------------------------------+
    //                 | i | g | c | encoded active/idle count    |
    //                 +---+---+---+------------------------------+
    //                       |   |
    //                       |   |
    //                       |  SHUTDOWN_COMPLETED_FLAG (0x20000000)
    //                       |
    //                    SUSPEND_GATE_FLAG (0x40000000)
    //
    // The gate count is 29 bits that encodes both the active and idle vproc counts. NOTE: If we have more than half a billion vprocs,
    // this needs to change.  Somehow, I'm doubting that, but it's a hard limit of the way this gate functions.
    //
    // Once the SHUTDOWN_INITIATED bit on the shutdown gate is set (all external references to the scheduler are released),
    // the scheduler proceeds into a 'sweep' phase iff all active virtual processors in the scheduler are idle (gate count is 0). The
    // sweep suspends all virtual processors and looks for blocked contexts in the scheduler. If any are found, finalization is rolled
    // back and virtual processors are unsuspended and allowed to proceed. In addition, virtual processor transitions from active to idle
    // and active to inactive, that bring the gate count to 0, also trigger a sweep and can lead to finalization.
    //
    // A virtual processor notifies the scheduler that it is ACTIVE when it is started up by the scheduler in response to incoming work.
    // From the ACTIVE state it could transition to IDLE and back to ACTIVE several times before either the scheduler shuts down or the 
    // in the individual virtual processor is asked to retire. At that point it moves from ACTIVE to INACTIVE.
    //
    //                VirtualProcessorActive(true)              VirtualProcessorIdle(true)
    //                                     |                         |
    //                                     |                         |
    //                                  ------------>        ------------->
    //                         INACTIVE               ACTIVE                IDLE
    //                                  <------------        <-------------
    //                                     |                         |
    //                                     |                         |
    //               VirtualProcessorActive(false)              VirtualProcessorIdle(false)
    //                                                           
    //
    // The figure below shows how a single virtual processor affects the gate count value(GC) as it changes state.
    //
    //                                                    
    //                   VirtualProcessorActive(false)    VirtualProcessorActive(true)
    //                                         |             |
    //                                         |             |
    //                                       <--------  --------->
    //                                    GC-1        GC        GC+1
    //                                       <--------  --------->
    //                                         |              |
    //                                         |              |
    //                     VirtualProcessorIdle(true)       VirtualProcessorIdle(false)
    //
    // Virtual processors should never be able to produce a gate count of -1 if they transition through valid states.
    // Therefore the increment/decrement operations can be done simply using InterlockedIncrement/InterlockedDecrement 
    // without fear of underflowing the 29 bit count into the flag area. (This is all assuming there are not > 1/2 billion
    // vprocs, of course).
    //
    // When the number of active vprocs is equal to the number of idle vprocs the gate count is 0.

    ///<summary>
    ///     Called to initiate shutdown of the scheduler.  This may directly proceed to phase two of shutdown (actively
    ///     shutting down internal contexts) or it may wait for additional events (e.g.: all work to complete) before
    ///     proceeding to phase two.
    ///</summary>
#pragma warning(disable: 4702) // unreachable code
    void SchedulerBase::PhaseOneShutdown()
    {
        // If this scheduler is the default scheduler, it should be cleared here, since we do not want any external contexts attaching 
        // to it via the CurrentScheduler interface. In the meantime, all calls to GetDefaultScheduler will perform a safe reference which
        // will fail.
        if (s_pDefaultScheduler == this) 
        {
            _StaticLock::_Scoped_lock lockHolder(s_defaultSchedulerLock);

            if (s_pDefaultScheduler == this)
            {
                s_pDefaultScheduler = NULL;
            }
        }

        ASSERT(m_internalContextCountPlusOne >= 1);

        // Check if the internal reference count is greater than 1. This is an optimistic check - it could go down to 1 immediately
        // after the check - this just means we will attempt to shutdown virtual processors even though there may be none left by the
        // time we try. Note, that with our implementation today this will not happen, if virtual processors were started up previously,
        // the contexts that executed on them retain their internal references on the scheduler, keeping m_internalContextCountPlusOne > 1,
        // until a finalization sweep completes successfully. However, even if the implementation changes, this optimistic check, at worst,
        // causes us to do slightly more work here. Could it increase after the check if we found it to be 1? Not without a bug
        // in user code, and we're not hardening ourself against these. (The external facing reference count is 0 at this point, and we
        // have the only reference to the scheduler.)
        //
        // The reason we don't decrement the value m_internalContextCountPlusOne, and check if the result is non-zero, to detect that
        // there are contexts/virtual processors out there, is that the moment we do that, the scheduler could disappear out
        // from under us. Consider the case where we decremented the internal count from 5 to 4, then proceeded to set the SHUTDOWN_INITIATED_FLAG
        // bit, and noticed that the gate count was 0. Now there is nothing stopping a virtual processor from becoming active
        // while the SHUTDOWN_INITIATED_FLAG bit is set. If it quickly became inactive (bringing the Gate count to zero once again),
        // we could have two callers simultaneously executing AttemptSchedulerSweep. One would win, and go on to the sweep and finalize
        // the scheduler, the other could be left touching garbage memory. This is why decrementing the ref count m_internalContextCountPlusOne
        // is the last thing we should do in this function - this keeps the scheduler alive as long as we're executing PhaseOneShutdown.
        //

        if (m_internalContextCountPlusOne > 1)
        {
            //
            // Signal a start of shutdown.
            //
            // At this point, we cannot finalize the scheduler.  Instead, we need to wait for all work to complete
            // before performing such finalization.  This means any realized chores, blocked contexts, etc...  We do
            // this by waiting for all virtual processors to become idle (no more unblocked work) and then suspending
            // all the virtual processors (they're idle anyway) and sweeping the scheduler looking for blocked contexts.
            // If none are found, the scheduler can be finalized.
            //
            LONG oldVal = m_vprocShutdownGate;
            LONG xchgVal = 0;

            for(;;)
            {
                xchgVal = InterlockedCompareExchange(&m_vprocShutdownGate, oldVal | SHUTDOWN_INITIATED_FLAG, oldVal);
                if (xchgVal == oldVal)
                    break;

                oldVal = xchgVal;
            }

            ASSERT((oldVal & GATE_FLAGS_MASK) == 0);

            if ((oldVal & GATE_COUNT_MASK) == 0)
            {
                AttemptSchedulerSweep();
            }
        }

        // There is an 'extra' internal context count per scheduler that must be released by every call to PhaseOneShutdown on the scheduler.
        // Resurrecting a scheduler must resurrect this count as well.
        DecrementInternalContextCount();
    }
#pragma warning(default: 4702)

    /// <summary>
    ///     Invoked when the Gate Count goes to zero as a result of virtual processor state transitions, while the
    ///     scheduler has been marked for shutdown. It proceeds to sweep the scheduler if it can set the suspend flag
    ///     on the shutdown gate while the gate count is still 0 and the scheduler is marked for shutdown.
    /// </summary>
    void SchedulerBase::AttemptSchedulerSweep()
    {
        //
        // If we're actively in a shutdown semantic, start a handshake to ensure that no virtual processor
        // comes out of idle until we've swept the scheduler.
        //
        LONG oldVal = SHUTDOWN_INITIATED_FLAG; // Gate Count was found to be 0 and suspend bit cannot be set.

        LONG xchgVal = InterlockedCompareExchange(&m_vprocShutdownGate, oldVal | SUSPEND_GATE_FLAG, oldVal);
        if (xchgVal == oldVal)
        {
            //
            // At this point no vprocs can become active even if they found work - they will stall on
            // the suspend gate when they make the transition from idle -> active.
            //
            SweepSchedulerForFinalize();
            //
            // NOTE: After this point, the *this* pointer is no longer valid and must not be dereferenced.
            //
        }
        else
        {
            //
            // One of three things could've happened here:
            //   - One of the idle vprocs became active before we could set the suspend gate flag
            //   - The scheduler was resurrected and vproc resurrecting the scheduler managed to become idle again.
            //   - There was a concurrent call to AttemptSchedulerSweep (see comments in PhaseOneShutdown) which
            //     succeeded in setting the suspend bit before we did, and probably completed finalization as well.
            //
            ASSERT(((xchgVal & GATE_COUNT_MASK) != 0) || ((xchgVal & GATE_FLAGS_MASK) == 0) ||
                   ((xchgVal & GATE_FLAGS_MASK) == (SHUTDOWN_INITIATED_FLAG | SUSPEND_GATE_FLAG)) ||
                   ((xchgVal & GATE_FLAGS_MASK) == (SHUTDOWN_INITIATED_FLAG | SHUTDOWN_COMPLETED_FLAG)));
        }
    }

    /// <summary>
    ///     Called during scheduler finalization, to check if any chores exist in the scheduler.
    /// </summary>
    /// <returns>
    ///     A boolean value indicating whether any unstarted chores (realized or unrealized) were found.
    /// </returns>
    bool SchedulerBase::FoundUnstartedChores()
    {
        ASSERT((m_vprocShutdownGate & SHUTDOWN_INITIATED_FLAG) != 0);
        ASSERT((m_vprocShutdownGate & SUSPEND_GATE_FLAG) != 0);

        bool choresFound = false;

        for (int idx = 0; idx < m_nodeCount; ++idx)
        {
            SchedulingRing *pRing = m_rings[idx];
            // For each ring, go through all schedule groups

            if (pRing != NULL)
            {
                for (int i = 0; i < pRing->m_scheduleGroups.MaxIndex(); i++)
                {
                    ScheduleGroupBase *pGroup = pRing->m_scheduleGroups[i];
                    if (pGroup != NULL)
                    {
                        if (pGroup->HasRealizedChores() || pGroup->HasUnrealizedChores())
                        {
                            choresFound = true;
                            break;
                        }
                    }

                }
                if (choresFound)
                    break;
            }
        }
        return choresFound;
    }

    /// <summary>
    ///     Called during scheduler finalization, to check if any blocked contexts exist in the scheduler.
    /// </summary>
    /// <returns>
    ///     A boolean value indicating whether any blocked contexts were found.
    /// </returns>
    bool SchedulerBase::FoundBlockedContexts()
    {
        ASSERT((m_vprocShutdownGate & SHUTDOWN_INITIATED_FLAG) != 0);
        ASSERT((m_vprocShutdownGate & SUSPEND_GATE_FLAG) != 0);

        bool blockedContextsFound = false;

        //
        // No new contexts shall be added to the m_allContexts list at this point. Since
        // all the vprocs are guaranteed to be idle, there should be no one executing on
        // a vproc (even though there could be contexts that are blocked). Any background
        // thread in the scheduler that adds a context needs to synchronize with sweep to
        // ensure that contexts are not added while the sweep is in progress. For example,
        // the UT creation thread in UMS fails the sweep if there are pending requests to
        // add new reserved contexts.
        //
        ContextNode *pNode = m_allContexts.Unsafe_Top();
        while (pNode != NULL)
        {
            //
            // Work consists of active contexts that are blocked.
            //
            if ((!pNode->m_pContext->IsIdle()) && pNode->m_pContext->IsBlocked())
            {
                ASSERT(pNode->m_pContext->IsBlocked());
                blockedContextsFound = true;
                break;
            }

            pNode = LockFreePushStack<ContextNode>::Next(pNode);
        }

        return blockedContextsFound;
    }

    ///<summary>
    ///     Determines if there is pending work such as blocked context/unstarted chores etc in the
    ///     scheduler. If there is no pending work, the scheduler will attempt to shutdown. 
    ///</summary>
    bool SchedulerBase::HasWorkPending()
    {
        return FoundBlockedContexts() || FoundUnstartedChores();
    }

    ///<summary>
    ///     Cancel all the internal contexts.
    ///</summary>
    void SchedulerBase::CancelAllContexts()
    {
        ContextNode *pNode = m_allContexts.Unsafe_Top();
        while ( pNode != NULL)
        {
            pNode->m_pContext->Cancel();
            pNode = LockFreePushStack<ContextNode>::Next(pNode);
        }
    }

    ///<summary>
    ///     Once all virtual processors are idle, the scheduler calls this routine which performs a full sweep through all
    ///     schedule groups looking for work.  If work is found (even a blocked context), the scheduler backs off finalization;
    ///     otherwise, it proceeds with phase two of shutdown, which cancels all contexts running in their dispatch loops,
    ///     as well as any background threads that exist.
    ///</summary>
    void SchedulerBase::SweepSchedulerForFinalize()
    {
        ASSERT((m_vprocShutdownGate & SHUTDOWN_INITIATED_FLAG) != 0);
        ASSERT((m_vprocShutdownGate & SUSPEND_GATE_FLAG) != 0);

        ContextBase *pContext = FastCurrentContext();
        bool fExternal = true;

        if ((pContext != NULL) && (!pContext->IsExternal()))
        {
            fExternal = false;
            pContext->EnterCriticalRegion();
        }

        //
        // Once we get in the sweep, and the SUSPEND_GATE_FLAG is set, no virtual processor can start searching for work,
        // without being gated by us. At this point we expect all virtual processors (except this one, if this is a context
        // running on a virtual processor) to be deactivated via (vproot->Deactivate). Even if work was added and one of
        // these was activated again (via vproot->Activate), they would have to reduce the idle count (via VirtualProcessorIdle(false)),
        // before they actually searched the scheduler queues for work. If they did, they would be suspended on the shutdown
        // gate semaphore. We need to make one final pass for blocked contexts and chores before deciding whether the scheduler
        // can be finalized. If either is found, we need to roll back.
        //
        if (!HasWorkPending())
        {
            //
            // If there are no blocked contexts and no chores, we can safely proceed to PhaseTwoShutdown.
            //
            PhaseTwoShutdown();
        }
        else
        {
            //
            // There's work, we need to let everyone go and try again on the next active to idle transition, or
            // the next active to inactive transition.
            //
            LONG xchgVal = m_vprocShutdownGate;
            LONG oldVal = xchgVal;

            do
            {
                oldVal = xchgVal;
                xchgVal = InterlockedCompareExchange(&m_vprocShutdownGate, oldVal & ~SUSPEND_GATE_FLAG, oldVal);
            }
            while (xchgVal != oldVal);

            //
            // Some virtual processors may have tried to transition from idle to active and been suspended by the gate.
            // The number of these vprocs should be the value of the gate count at the time we clear the suspend flag.
            // The flag was set at a time the gate count was 0, and from there, it could only have transitioned to a
            // positive value. (+1 for each vproc that tried to go idle -> active and was suspended on the gate).
            // This is how many virtual processors we need to wake up from suspend.
            //
            ReleaseSuspendedVirtualProcessors(xchgVal & GATE_COUNT_MASK);
        }

        if (!fExternal) pContext->ExitCriticalRegion();
    }

    /// <summary>
    ///     Releases virtual processors that were suspended on the shutdown gate, while trying to go from IDLE to
    ///     ACTIVE when the finalization sweep was in progress.
    /// </summary>
    /// <param name="releaseCount">
    ///     Number of virtual processors that need to be released.
    /// </param>
    void SchedulerBase::ReleaseSuspendedVirtualProcessors(LONG releaseCount)
    {
        if (releaseCount > 0)
        {
            ReleaseSemaphore(m_hSchedulerShutdownSync, releaseCount, NULL);
        }
    }

    ///<summary>
    ///     Called when a virtual processor becomes active (before it does) or becomes inactive (before it does).
    ///</summary>
    ///<param value="fActive"> 
    ///     True if a virtual processor is going from INACTIVE to ACTIVE, and false if it is going from ACTIVE to INACTIVE.
    ///</param>
    ///<returns>
    ///     For activation, the function returns true if the virtual processor was successfully activated, and false
    ///     if it could not be activated because the scheduler was shutting down. For inactivation, it always returns true.
    ///</returns>
    bool SchedulerBase::VirtualProcessorActive(bool fActive)
    {
        //
        // It is possible for a virtual processor to become active while the finalization sweep is in progress. A thread external
        // to the scheduler could unblock a scheduler context, and activate a virtual processor, if one is found to be available.
        //
        // However, it is not possible for a virtual processor to become inactive during the sweep. This is because the active 
        // and idle vproc counts were equal at the time the bit was set (gate count was 0), and there are no external contexts
        // attached to the scheduler. To become inactive, a virtual processor would first have to go idle -> active, and would
        // block on the suspend gate.
        //

        //
        // A virtual processor increments the shutdown gate count on going from inactive to active, and decrements it
        // on going from active to inactive.
        // It also updates the active count - the gate count is used to synchronize with finalization, but the active count
        // is needed when the finalization code needs to suspend virtual processors.
        //
        if (fActive)
        {
            // We need to spin while the suspend bit is set, if the scheduler is in the sweep phase. If the scheduler finds a blocked
            // context, it will rollback finalization and allow us to proceed. If it enters phase two shutdown, it will set a bit
            // indicating that shutdown is complete and we should fail this call. We are guaranteed that the scheduler will not 
            // be deleted, in that we are safe in accessing its data members during this call, since every path that calls this API
            // and does not already have a reference on the scheduler, employs a different means of synchronization to ensure the
            // scheduler stays around. An example of this is the AddRunnableContext API in CacheLocalScheduleGroup.
            //
            // We also need to increment the shutdown gate while ensuring that the suspend bit is not set, therefore we use a 
            // compare exchange instead of an interlocked increment.
            LONG xchgVal = m_vprocShutdownGate;
            LONG oldVal = xchgVal;

            do
            {
                oldVal = xchgVal;

                if ((oldVal & SUSPEND_GATE_FLAG) != 0)
                {
                    // If the suspend bit was set in the meantime, we need to spin again until it is unset. Same logic as above applies;
                    // a blocked context exists, and the scheduler will roll back finalization.
                    oldVal = SpinUntilBitsReset(&m_vprocShutdownGate, SUSPEND_GATE_FLAG);
                }
                if ((oldVal & SHUTDOWN_COMPLETED_FLAG) != 0)
                {
                    // Scheduler has shutdown and we cannot activate virtual processors anymore.
                    return false;
                }

                xchgVal = InterlockedCompareExchange(&m_vprocShutdownGate, oldVal + 1, oldVal);
            }
            while (xchgVal != oldVal);

            ASSERT((oldVal & SUSPEND_GATE_FLAG) == 0);
            ASSERT((oldVal & SHUTDOWN_COMPLETED_FLAG) == 0);

            LONG activeCount = InterlockedIncrement(&m_activeVProcCount);
            VirtualProcessorActiveNotification(fActive, activeCount);
        }
        else
        {
            ASSERT((m_vprocShutdownGate & SUSPEND_GATE_FLAG) == 0);
            ASSERT((m_activeVProcCount != 0) && ((m_vprocShutdownGate & GATE_COUNT_MASK) != 0));

            LONG activeCount = InterlockedDecrement(&m_activeVProcCount);
            VirtualProcessorActiveNotification(fActive, activeCount);

            LONG val = InterlockedDecrement(&m_vprocShutdownGate);
            //
            // The act of going inactive could potentially make the active and idle vproc counts equal and should try initiate finalization
            // if the shutdown initiated flag is set.
            //
            if (((val & GATE_COUNT_MASK) == 0) && ((val & SHUTDOWN_INITIATED_FLAG) != 0))
            {
                // The suspend flag should not bet set.
                ASSERT(val == SHUTDOWN_INITIATED_FLAG);
                AttemptSchedulerSweep();
            }
        }

        return true;
    }

    ///<summary>
    ///     Internal contexts call the scheduler right before they deactivate their virtual processors and sleep indefinitely
    ///     until more work enters the scheduler, in order to allow things that happen on scheduler idle to happen (e.g.: sweeping
    ///     for phase two shutdown).
    ///
    ///     They also call the scheduler when they transition out of idle before starting to search the scheduler for work, if
    ///     they underlying virtual processor was re-activated as a result of new work entering. This may halt scheduler shutdown
    ///     or it may coordinate with scheduler shutdown depending on the current phase of shutdown.
    ///
    ///     This call *MUST* be made from a scheduler critical region.
    ///</summary>
    void SchedulerBase::VirtualProcessorIdle(bool fIdle)
    {
        //
        // There shall be no blocking operations during SchedulerSweep. If they do and we are forced to make a scheduling decision (such as
        // in UMS), it is a recipe for deadlock. (Scheduling decision => activate idle virtual processor => wait for sweep to complete => deadlock).
        // We will enter a hypercritical region to enforce it. Any blocking operation that require another thread to be scheduled will immediately
        // deadlock.
        //
        ContextBase::StaticEnterHyperCriticalRegion();

        //
        // A virtual processor decrements the shutdown gate count when it goes from active to idle and increments it
        // when it goes from idle to active.
        //
        if (fIdle)
        {
            ASSERT((m_vprocShutdownGate & SUSPEND_GATE_FLAG) == 0);
            ASSERT((m_vprocShutdownGate & GATE_COUNT_MASK) != 0);
            LONG val = InterlockedDecrement(&m_vprocShutdownGate);
            //
            // The act of going idle could potentially make the active and idle vproc counts equal and should try initiate finalization
            // if the shutdown in progres bit flag is set.
            //
            if (((val & GATE_COUNT_MASK) == 0) && ((val & SHUTDOWN_INITIATED_FLAG) != 0))
            {
                // The suspend flag should not bet set.
                ASSERT(val == SHUTDOWN_INITIATED_FLAG);
                AttemptSchedulerSweep();
            }
        }
        else
        {
            LONG val = InterlockedIncrement(&m_vprocShutdownGate);
            //
            // If a virtual processor is trying to go from idle to active while a scheduler sweep is in progress, it must be
            // forcefully suspended until the scheduler has decided whether it needs to rollback or continue with finalization.
            //
            if ((val & SUSPEND_GATE_FLAG) != 0)
            {
                //
                // For UMS, this will trigger return to primary.  We're in a critical section, so we won't
                // be able to observe it.
                //
                WaitForSingleObject(m_hSchedulerShutdownSync, INFINITE);
            }
        }

        ContextBase::StaticExitHyperCriticalRegion();
    }

    ///<summary>
    ///     Called to perform a resurrection of the scheduler.  When the scheduler reference count has fallen to zero,
    ///     it's possible there's still work on the scheduler and that one of those work items will perform an action
    ///     leading to additional reference.  Such bringing of the reference count from zero to non-zero is only legal
    ///     on an *INTERNAL* context and immediately halts shutdown.
    ///</summary>
    void SchedulerBase::Resurrect()
    {
        //
        // If we got here, someone is going to flag shutdown triggering a whole slew of stuff.  We need to ensure
        // that that guy progresses to the point where the shutdown_initiated_flag gets set and *THEN* clear it.  Hence,
        // spin for a while waiting for the thread which released to finish setting the flag.  Subsequently,
        // we clear it.  The original thread will not sweep (as there's a non idle vproc by definition if we
        // get here).
        //

        LONG val = SpinUntilBitsSet(&m_vprocShutdownGate, SHUTDOWN_INITIATED_FLAG);
        ASSERT((val & SHUTDOWN_INITIATED_FLAG) != 0);

        while(true)
        {
            LONG xchgVal = InterlockedCompareExchange(&m_vprocShutdownGate, val & ~SHUTDOWN_INITIATED_FLAG, val);
            if (xchgVal == val)
                break;

            val = xchgVal;
        }

        //
        // As this had to have happened from an internal context, that fact alone should guarantee that we weren't in the
        // middle of a sweep or moving forward finalization.  Further, it also makes this guaranteed safe -- our context
        // still holds one reference on the scheduler.  This cannot have dropped to zero yet.
        //
        IncrementInternalContextCount();
    }

    ///<summary>
    ///     Actively informs all internal contexts to exit and breaks them out of their dispatch loops.  When the last
    ///     internal context dies, finalization will occur and we move to SchedulerBase::Finalize().
    ///</summary>
    void SchedulerBase::PhaseTwoShutdown()
    {
        ContextBase * pContext = SchedulerBase::FastCurrentContext();
        bool fExternal = (pContext == NULL || pContext->IsExternal() || (pContext->GetScheduler() != this));

        // If this is not an internal context belonging to this scheduler, we need to take a reference here to be safe.
        // In the course of phase two shutdown, we may end up canceling all contexts, which may bring the reference
        // count to zero and finalize the scheduler while we're still releasing a lock or something.
        if (fExternal)
            IncrementInternalContextCount();

        //
        // Fully commit all safe points.
        //
        CommitToVersion(0);

        //
        // Cancel all contexts. For contexts that are running atop virtual processors, we must activate the virtual processors,
        // so that the contexts can exit their dispatch loops. For the remaining contexts, we must perform a certain amount of
        // cleanup, such as decrementing the reference counts they hold on the scheduler.
        //
        // Contexts *must* be canceled before the suspend bit is reset. This is so that any calls to VirtualProcessorIdle(false)
        // by contexts in their dispatch loops are blocked on the suspend gate until *after* the context is canceled. That way,
        // when they are un-suspended, they will immediately exit their dispatch loop, as we intend for them to. Calls to
        // VirtualProcessorIdle(false) could result as a race between adding a work item to the scheduler (and subsequently
        // trying to activate a virtual processor), and the scheduler shutting down due to a different virtual processor finding
        // and executing that work item and subsequently going idle.
        //
        CancelAllContexts();

        //
        // PhaseTwoShutdown is executed when the scheduler has confirmed that no work, in the form of blocked contexts, exists, and that
        // all active virtual processors are idle. However, it is possible that a foregin thread is trying to activate a virtual processor
        // in this scheduler. This could be due to a race while adding runnable contexts. We need to mark the scheduler such that the
        // vproc addition apis can fail gracefully. Atomically clear the suspend bit and set the shutdown completed bit.
        //
        // This needs to be done in a loop, since changes to the gate count from calls to VirtualProcessorIdle(false) are possible.
        //
        LONG xchgVal = m_vprocShutdownGate;
        LONG oldVal = 0;

        do
        {
            oldVal = xchgVal;
            ASSERT((oldVal & SHUTDOWN_INITIATED_FLAG) != 0);
            ASSERT((oldVal & SUSPEND_GATE_FLAG) != 0);

            xchgVal = InterlockedCompareExchange(&m_vprocShutdownGate, (oldVal | SHUTDOWN_COMPLETED_FLAG) & ~SUSPEND_GATE_FLAG, oldVal);
        }
        while (xchgVal != oldVal);

        //
        // Some virtual processors may have tried to transition from idle to active and been suspended by the gate.
        // The number of these vprocs should be the value of the gate count at the time we clear the suspend flag.
        // The flag was set at a time the gate count was 0, and from there, it could only have transitioned to a
        // positive value. (+1 for each vproc that tried to go idle -> active and was suspended on the gate).
        // This is how many virtual processors we need to wake up from suspend.
        //
        ReleaseSuspendedVirtualProcessors(xchgVal & GATE_COUNT_MASK);

        //
        // Cancel all background event handlers
        //
        DestroySchedulerEventHandlers();

        if (fExternal) DecrementInternalContextCount();
    }

    /// <summary>
    ///     Returns true if the scheduler has gone past a certain point in PhaseTwoShutdown (when it sets the shutdown completed flag).
    ///     This function is mainly used for debug asserts.
    /// </summary>
    bool SchedulerBase::HasCompletedShutdown()
    {
        LONG shutdownFlags = SHUTDOWN_INITIATED_FLAG | SHUTDOWN_COMPLETED_FLAG;
        return ((m_vprocShutdownGate & shutdownFlags) == shutdownFlags);
    }

    /// <summary>
    ///     Returns true if the scheduler is in the finalization sweep, i.e, the SUSPEND_GATE_FLAG is set.
    ///     This function is mainly used for debug asserts.
    /// </summary>
    bool SchedulerBase::InFinalizationSweep()
    {
        LONG shutdownFlags = SHUTDOWN_INITIATED_FLAG | SUSPEND_GATE_FLAG;
        return ((m_vprocShutdownGate & shutdownFlags) == shutdownFlags);
    }

    /// <summary>
    ///     Called to finalize the scheduler.
    /// </summary>
    void SchedulerBase::Finalize()
    {

        // The scheduler resources should be given back to RM before setting the
        // shutdown events. Waiters should be able to create a new scheduler and
        // get the resources this scheduler released.
        //
        // Note that this should happen prior to deleting contexts.  We might be
        // on a UMS thread finalizing which might need to perform lock validation
        // of other UMS related things which require m_pContext not to be toasted
        // underneath the UMS thread.
        //
        m_pSchedulerProxy->Shutdown();
        delete m_pSchedulerResourceManagement;

        CloseHandle(m_hSchedulerShutdownSync);

        delete m_pExternalContextTable;

        // Delete all the internal contexts
        ContextNode *pNode = m_allContexts.Flush();
        while ( pNode != NULL)
        {
            ContextNode *pNextNode = LockFreePushStack<ContextNode>::Next(pNode);
            _InternalDeleteHelper<InternalContextBase>(pNode->m_pContext);
            delete pNode;
            pNode = pNextNode;
        }

        ExternalContextBase *pContext = m_externalContextPool.Flush();
        while (pContext != NULL)
        {
            ExternalContextBase *pNextContext = LockFreeStack<ExternalContextBase>::Next(pContext);
            _InternalDeleteHelper<ExternalContextBase>(pContext);
            pContext = pNextContext;
        }

        RealizedChore *pChore = m_realizedChorePool.Flush();
        while (pChore != NULL)
        {
            RealizedChore *pNextChore = LockFreeStack<RealizedChore>::Next(pChore);
            delete pChore;
            pChore = pNextChore;
        }

        // Trace event to signal scheduler shutdown
        TraceSchedulerEvent(CONCRT_EVENT_END, TRACE_LEVEL_INFORMATION, m_id);

        // Signal threads waiting on scheduler shutdown
        while ( !m_finalEvents.Empty())
        {
            WaitNode *pNode = m_finalEvents.RemoveHead();
            SetEvent(pNode->m_hEvent);
            CloseHandle(pNode->m_hEvent);
            delete pNode;
        }

        delete this;
    }

    ///<summary>
    ///     Increments the reference counts required by a scheduler attach.
    ///</summary>
    void SchedulerBase::ReferenceForAttach()
    {
        InterlockedIncrement(&m_attachCount);
        Reference();
    }

    ///<summary>
    ///     Decrements the reference counts incremented for scheduler attach.
    ///</summary>
    void SchedulerBase::ReleaseForDetach()
    {
        InterlockedDecrement(&m_attachCount);
        Release();
    }

    ///<summary>
    ///     Internal contexts call this when created and utilized inside this scheduler.
    ///</summary>
    void SchedulerBase::IncrementInternalContextCount()
    {
        InterlockedIncrement(&m_internalContextCountPlusOne);
    }

    ///<summary>
    ///     Internal contexts call this function in order to notify that they are out of dispatch.  The last internal context
    ///     to call this will trigger scheduler finalization.
    ///</summary>
    void SchedulerBase::DecrementInternalContextCount()
    {
        LONG val = InterlockedDecrement(&m_internalContextCountPlusOne);
        ASSERT(val >= 0);
        if (val == 0) Finalize();
    }

    /// <summary>
    ///     Send a scheduler ETW event.
    /// </summary>
    void SchedulerBase::ThrowSchedulerEvent(ConcRT_EventType eventType, UCHAR level, unsigned int schedulerId)
    {
        if (g_pEtw != NULL)
        {
            CONCRT_TRACE_EVENT_HEADER_COMMON concrtHeader = {0};

            concrtHeader.header.Size = sizeof concrtHeader;
            concrtHeader.header.Flags = WNODE_FLAG_TRACED_GUID;
            concrtHeader.header.Class.Type = (UCHAR)eventType;
            concrtHeader.header.Class.Level = level;
            concrtHeader.header.Guid = SchedulerEventGuid;

            concrtHeader.SchedulerID = schedulerId;

            g_pEtw->Trace(g_ConcRTSessionHandle, &concrtHeader.header);
        }
    }

    /// <summary>
    ///     Called when the RM adds virtual processors to the scheduler. This is either at scheduler creation time,
    ///     or when RM adds a core due to core migration. This API is NOT used to add oversubscribed virtual processors.
    /// </summary>
    void SchedulerBase::AddVirtualProcessor(Concurrency::IVirtualProcessorRoot *pVProcRoot)
    {
        ASSERT(m_nodeCount <= m_maxNodes);

        // IMPORTANT: This API is called for each virtual processor added at the time of scheduler creation, and
        // later when dynamic RM adds cores to this scheduler. We do not need to synchronize between concurrent
        // invocations of this API, as the RM guarantees for now that only one thread is calling this API at a time.
        int nodeId = pVProcRoot->GetNodeId();

        ASSERT(nodeId >= 0 && nodeId < m_maxNodes);
        if (m_nodes[nodeId] == NULL)
        {
            SchedulingRing *pRing = new SchedulingRing(this, nodeId);
            SchedulingNode *pNode = new SchedulingNode(pRing);
            pRing->SetOwningNode(pNode);

            m_rings[nodeId] = pRing;
            m_nodes[nodeId] = pNode;

            if (nodeId >= m_nodeCount)
            {
                m_nodeCount = nodeId + 1;
            }
        }

        m_nodes[nodeId]->AddVirtualProcessor(pVProcRoot);

        // This count should be incremented after adding the virtual processor. The SchedulingNode::AddVirtualProcessor
        // called aboveAPI tests if the count is 0 to infer that this virtual processor is the first one added to the
        // scheduler during the initial thread request.
        InterlockedIncrement(&m_virtualProcessorCount);

        // The total count on the scheduler is not incremented for oversubscribed virtual processors. (adding an oversubscribed
        // virtual processor doesn't go through this code path). Querying for the number of virtual processors assigned to a scheduler
        // does not take into account oversubscribed virtual processors, since technically these virtual processors are in place
        // to compensate for other virtual processors that may be blocked, and therefore are not available to perform work for
        // the scheduler.
    }

    /// <summary>
    ///     Called when RM decides to take a virtual processor away from a scheduler. The scheduler must mark the
    ///     virtual processor for retirement, and respond with IVirtualProcessorRoot::Remove when the virtual processor
    ///     root is ready for destruction.
    /// </summary>
    void SchedulerBase::RemoveVirtualProcessor(IVirtualProcessorRoot *pVProcRoot)
    {
        VirtualProcessor * pVirtualProcessor = m_nodes[pVProcRoot->GetNodeId()]->FindMatchingVirtualProcessor(pVProcRoot);

        while (pVirtualProcessor == NULL)
        {
            // If the virtual processor was not found the first time around, it must because it is an oversubscribed virtual processor
            // and we are racing with the call to Oversubscribe(true). Once the virtual processor root has been created in the RM
            // (for the oversubscribed vproc), we can receive a RemoveVirtualProcessor call for that root at any time. Only the thread
            // scheduler creates oversubscribed vprocs.
            ASSERT(m_policy.GetPolicyValue(::Concurrency::SchedulerKind) == ::Concurrency::ThreadScheduler);

            _SpinWaitBackoffNone spinWait;
            while (spinWait._SpinOnce())
            {
                // _YieldProcessor is called inside _SpinOnce
            }

            SwitchToThread();
            pVirtualProcessor = m_nodes[pVProcRoot->GetNodeId()]->FindMatchingVirtualProcessor(pVProcRoot);
            ASSERT(pVirtualProcessor == NULL || pVirtualProcessor->m_fOversubscribed);
        }

        if (pVirtualProcessor->m_fOversubscribed)
        {
            // We must sychronize with a potential RemoveVirtualProcessor for this virtual processor due to the RM taking the underlying
            // core away. The winner of the interlocked exchange gets to retire the virtual processor.
            ASSERT(pVirtualProcessor->m_pOversubscribingContext != NULL);
            pVirtualProcessor = pVirtualProcessor->m_pOversubscribingContext->GetAndResetOversubscribedVProc(pVirtualProcessor);

            ASSERT(pVirtualProcessor == NULL || pVirtualProcessor->GetOwningRoot() == pVProcRoot);
            // Even if we lose the race, we are safe to touch the virtual processor here, since the context retiring the virtual processor
            // is guaranteed to not get past the call to the RM (in VirtualProcessor::Retire), that removes the virtual processor.
        }

        if (pVirtualProcessor != NULL)
        {
            pVirtualProcessor->MarkForRetirement();
        }
    }

    ///<summary>
    ///     Collect all the statistical information about this scheduler, which include arrival and completion
    ///     rates, from which the total number of tasks is deduced.
    ///</summary>
    void SchedulerBase::Statistics(unsigned int *pTaskCompletionRate, unsigned int *pTaskArrivalRate, unsigned int *pNumberOfTasksEnqueued)
    {
        //
        // Collect all the virtual processor statistics. All internal contexts own a virtual processor when they
        // run a task, so they also own the statistical information at that point and are free to update it without
        // racing with other internal contexts (it is done without interlocked operation). Now we simply collect that
        // information that aggregated on each virtual processor and add it to our total. We do it non-interlocked
        // fully aware that the numbers might be slightly off, for example due to store-buffer not being flushed.
        //

        for (int index = 0; index < m_nodeCount; index++)
        {
            SchedulingNode * pNode = m_nodes[index];

            if (pNode != NULL)
            {
                for (int i = 0; i < pNode->m_virtualProcessors.MaxIndex(); i++)
                {
                    VirtualProcessor * pVirtualProcessor = pNode->m_virtualProcessors[i];

                    //
                    // We collect the data and reset it so that next time around we would get the numbers as of
                    // last update. This allows us to get the rate of change and avoid overflow in most cases.
                    //
                    if (pVirtualProcessor != NULL)
                    {
                        unsigned int arrivalRate = pVirtualProcessor->GetEnqueuedTaskCount();
                        unsigned int completionRate = pVirtualProcessor->GetDequeuedTaskCount();

                        *pTaskArrivalRate += arrivalRate;
                        *pTaskCompletionRate += completionRate;
                        *pNumberOfTasksEnqueued += (arrivalRate - completionRate);
                    }
                }
            }
        }

        //
        // Collect data from the retired virtual processors, saved on the scheduler itself. Note that there is
        // a race here between virtual processor retiring and statistics being collected. Since we do not lock
        // any structure that we are reading from we can either count the statistics twice, or miss them completely.
        // This will cause a spike in statistics, but hopefully it will be rare enough that after collecting
        // several datapoints it can be discarded.
        //
        {
            unsigned int arrivalRate = GetEnqueuedTaskCount();
            unsigned int completionRate = GetDequeuedTaskCount();

            *pTaskArrivalRate += arrivalRate;
            *pTaskCompletionRate += completionRate;
            *pNumberOfTasksEnqueued += (arrivalRate - completionRate);
        }

        //
        // Collect all the external context and free thread statistics. All external contexts and alien threads (not
        // bound to our scheduler) are registered in a list array that is kept on a scheduler. They own a slot in the
        // list array of external statistics, and they update that particular external statistics. Because they own a unique
        // ExternalStatistics class there is no contention or races. Now we simply collect that information per external context
        // or free thread and add it to our total. Again, we are fully aware that numbers might not be fully accurate.
        //
        for (int i = 0; i < m_externalThreadStatistics.MaxIndex(); i++)
        {
            ExternalStatistics * externalStatistics = m_externalThreadStatistics[i];

            //
            // We collect the data and reset it so that next time around we would get the numbers as of
            // last update. This allows us to get the rate of change.
            //
            if (externalStatistics != NULL)
            {
                unsigned int arrivalRate = externalStatistics->GetEnqueuedTaskCount();
                unsigned int completionRate = externalStatistics->GetDequeuedTaskCount();

                *pTaskArrivalRate += arrivalRate;
                *pTaskCompletionRate += completionRate;
                *pNumberOfTasksEnqueued += (arrivalRate - completionRate);

                // If external statistics class is no longer useful, remove it. Note that we could have left the external
                // statistics alone because when scheduler finalizes it destroys the ListArray<ExternalStatistics>, which
                // will also destruct all external statistics stored in it. However, this way we allow for slot reuse in
                // the ListArray in case we have a huge amount of external context joining and leaving.
                if (!externalStatistics->IsActive())
                {
                    // We can safely remove this statistics from our list. To see why look at comments in IsActive().
                    m_externalThreadStatistics.Remove(externalStatistics, false);
                    delete externalStatistics;
                }
            }
        }
    }

    /// <summary>
    ///     Returns a suballocator from the pool of suballocators in the process, or creates a new one. The RM only allows
    ///     a fixed number of allocators for external contexts in the process, whereas every virtual processor that requests
    ///     an allocator will get one.
    /// </summary>
    /// <param name="fExternalAllocator">
    ///     Specifies whether the allocator is being requested for an external context. If this is 'true' the RM will return
    ///     NULL if it has reached its limit of suballocators for external contexts. If this is 'false', the caller is requesting
    ///     the suballocator for a virtual processor, and the RM *must* allocate one (resources permitting).
    /// </param>
    SubAllocator* SchedulerBase::GetSubAllocator(bool fExternalAllocator)
    {
        if (fExternalAllocator)
        {
            if (s_numExternalAllocators >= s_maxExternalAllocators)
            {
                return NULL;
            }
            InterlockedIncrement(&s_numExternalAllocators);
        }

        SubAllocator* pAllocator = s_subAllocatorFreePool.Pop();
        if (pAllocator == NULL)
        {
            pAllocator = new SubAllocator();
        }

        ASSERT(pAllocator != NULL);
        pAllocator->SetExternalAllocatorFlag(fExternalAllocator);

        return pAllocator;
    }

    /// <summary>
    ///     Returns a suballocator back to the pool in the RM.
    ///     The RM caches a fixed number of suballocators and will destroy the rest.
    /// </summary>
    void SchedulerBase::ReturnSubAllocator(SubAllocator* pAllocator)
    {
        if (pAllocator->IsExternalAllocator())
        {
            LONG retVal = InterlockedDecrement(&s_numExternalAllocators);
            ASSERT(retVal >= 0);
        }

        if (s_subAllocatorFreePool.Count() < s_allocatorFreePoolLimit)
        {
            s_subAllocatorFreePool.Push(pAllocator);
        }
        else
        {
            delete pAllocator;
        }
    }

    /// <summary>
    ///     Called to perform a commit of safe-point registrations up to **AND INCLUDING** a particular version.
    /// </summary>
    /// <param name="commitVersion">
    ///     The data version that we commit to.  A version of zero indicates a full commit.
    /// </param>
    void SchedulerBase::CommitToVersion(ULONG commitVersion)
    {
        //
        // For UMS, this has to be lock free (more accurately, UMS trigger free -- meaning no blocking or yielding operations).  
        // We store this as a pure-spin-lock (hyper critical) protected queue.  There should be very low contention on this lock.
        //

        SQueue<SafePointInvocation> pCommits;

        m_safePointInvocations.Acquire();
        for(;;)
        {
            SafePointInvocation *pCur = m_safePointInvocations.Current();
            
            //
            // We do not attempt to commit across the wrap-around boundary.  We commit up to the boundary and then recommit afterward.  This prevents
            // wrap-around issues.
            //
            if (pCur != NULL && (commitVersion == 0 || (IsVisibleVersion(pCur->m_safePointVersion) && pCur->m_safePointVersion <= commitVersion)))
            {
                pCur = m_safePointInvocations.SQueue<SafePointInvocation>::Dequeue();
                pCommits.Enqueue(pCur);
            }
            else
            {
                break;
            }
        }
        m_safePointInvocations.Release();

        //
        // Perform every safe point invocation.
        // These must be invoked in the order they were enqueued to m_safePointInvocations.  There is an ordering constraint because
        // ListArrays of workqueues are in ScheduleGroups, which are in ListArrays themselves.  Deleting a workqueue after deleting
        // its enclosing ScheduleGroup will cause an AV.
        //
        while (!pCommits.Empty())
        {
            SafePointInvocation *pCur = pCommits.Dequeue();
            pCur->Invoke();
        }
    }

    /// <summary>
    ///     Returns the commit version for safe points within the scheduler.
    /// </summary>
    ULONG SchedulerBase::ComputeSafePointCommitVersion()
    {
        bool fVersioned = false;
        ULONG commitVersion = 0;

        for (int index = 0; index < m_nodeCount; index++)
        {
            SchedulingNode *pNode = m_nodes[index];
            
            if (pNode != NULL)
            {
                for (int i = 0; i < pNode->m_virtualProcessors.MaxIndex(); i++)
                {
                    VirtualProcessor *pVirtualProcessor = pNode->m_virtualProcessors[i];
                    if (pVirtualProcessor != NULL)
                    {
                        ULONG localCommit = ObservedVersion(pVirtualProcessor->m_safePointMarker.m_lastObservedVersion);

                        if (fVersioned)
                        {
                            //
                            // We can only commit to the lowest version that every virtual processor has observed.
                            //
                            if (commitVersion > localCommit)
                                commitVersion = localCommit;
                        }
                        else
                        {
                            commitVersion = localCommit;
                            fVersioned = true;
                        }
                    }
                }
            }
        }

        return commitVersion;
    }

    /// <summary>
    ///     Updates and returns the pending version for safe point commits.
    ///     If there are no commits pending, 0 is returned.
    /// </summary>
    ULONG SchedulerBase::UpdatePendingVersion()
    {
        ULONG commitVersion = ComputeSafePointCommitVersion();

        if (commitVersion <= m_safePointPendingVersion)
        {
            // It has either been comitted or is pending in some vproc.
            return 0;
        }

        if (m_safePointPendingVersion == m_safePointCommitVersion)
        {
            // Update pending version. This routine is called with the lock
            // held. This compare and set operation needs to be atomic.
            m_safePointPendingVersion = commitVersion;
            return commitVersion;
        }
        else
        {
            // Just update the pending version. The vproc that snapped the list
            // will notice this update and resnap the new entries
            m_safePointPendingVersion = commitVersion;
            return 0;
        }
    }

    /// <summary>
    ///     Updates the commit version to the given version and returns
    ///     the pending commit version. If there are no commits pending, 0 is returned.
    /// </summary>
    /// <param name="commitVersion">
    ///     The version up to which safe points have been committed.
    /// </param>
    ULONG SchedulerBase::UpdateCommitVersion(ULONG commitVersion)
    {
        ASSERT(commitVersion != 0);

        if (commitVersion == ULONG_MAX)
        {
            // Handle overflow
            ASSERT(commitVersion == m_safePointPendingVersion);
            m_safePointCommitVersion = 0;

            // Handle safepoints above the wrap around point
            m_safePointPendingVersion = ComputeSafePointCommitVersion();
        }
        else
        {
            // Update the committed version
            m_safePointCommitVersion = commitVersion;
        }

        if (m_safePointCommitVersion != m_safePointPendingVersion)
        {
            // Found pending commits
            ASSERT(m_safePointPendingVersion > m_safePointCommitVersion);
            return m_safePointPendingVersion;
        }

        return 0;
    }

    /// <summary>
    ///     Called to make a determination of what version of data we can commit up to.  This is the minimum data version that all virtual
    ///     processors have observed.
    /// </summary>
    void SchedulerBase::CommitSafePoints()
    {
        ULONG commitVersion = 0;

        // Update the version we are about to commit.
        m_safePointInvocations.Acquire();
        commitVersion = UpdatePendingVersion();
        m_safePointInvocations.Release();


        // Note that a commitVersion of 0 here indicates that there are no safe points
        // to commit.
        while (commitVersion != 0)
        {
            CommitToVersion(commitVersion);

            // Publish the committed version and check for any pending commits
            m_safePointInvocations.Acquire();
            commitVersion = UpdateCommitVersion(commitVersion);
            m_safePointInvocations.Release();
        }
    }

    /// <summary>
    ///     Called when a particular virtual processor reaches a safe point.  This function does very little unless there has
    ///     been a change in the version number of the safe point.
    /// </summary>
    /// <param name="pMarker">
    ///     The safe point marker for a given virtual processor.  This is the virtual processor reaching a safe point.
    /// </param>
    /// <returns>
    ///     An indication of whether a commit should take place.  If this is true, the caller should call CommitSafePoints when possible.
    ///     Note that this is a return value so that things like UMS virtual processors can exit critical regions before performing
    ///     the commit (to avoid, for instance, heap locks in critical regions).
    /// </returns>
    bool SchedulerBase::MarkSafePoint(SafePointMarker *pMarker)
    {
        //
        // If there has been no change in observation version, there's nothing to mark off and nothing to worry about.  Only if there
        // has been a change need we go down the path of checking how far we can commit.
        //
        if (pMarker->m_lastObservedVersion != m_safePointDataVersion)
        {
            pMarker->m_lastObservedVersion = m_safePointDataVersion;
            return true;
        }
        
        return false;
    }

    /// <summary>
    ///     The routine is used to trigger a safe point commit on all the vprocs by
    ///     updating the data version. This routine shall not trigger synchronous UMS blocking
    /// </summary>
    void SchedulerBase::TriggerCommitSafePoints(SafePointMarker *pMarker)
    {
        m_safePointInvocations.Acquire();

        //
        // We check for a change with the lock held to avoid triggering
        // a commit check on all vprocs unnecessarily.
        //
        if (pMarker->m_lastObservedVersion != m_safePointDataVersion)
        {
            //
            // Publishing a new data version would cause all vprocs
            // to attempt a commit
            //
            PublishNewDataVersion();
        }

       m_safePointInvocations.Release();
    }

    /// <summary>
    ///     Called to register an object to invoke upon reaching the next safe point after this call.
    /// </summary>
    /// <param name="pInvocation">
    ///     The invocation object which contains information about what to call.
    /// </param>
    void SchedulerBase::InvokeOnSafePoint(SafePointInvocation *pInvocation)
    {
        //
        // *READ THIS CAREFULLY*:
        //
        // Due to the places in which this is likely to be invoked, this routine must not trigger UMS.  That means it must be lock-free (or at least
        // block/yield free).  Doing any UMS trigger operation will wreak havoc on the UMS scheduler. 
        //
        // Note that in order to vastly simplify this code, I am utilizing a pure spin lock protected queue.  There should be low enough contention
        // on this that it should not matter and there are potential truly lock-free algorithms which might be used for more efficiency (though hugely
        // more complex).
        //
        
        //
        // Note that we assume that everything is enqueued in data version order (FIFO) in order to simplify the code and prevent having to always
        // scan the entire list.  In order to guarantee this, the increment and the list addition must be atomic with repsect to each other.  Right
        // now, this is guarded via the spinlock on this list.
        //
        m_safePointInvocations.Acquire();

        pInvocation->m_safePointVersion = PublishNewDataVersion();
        m_safePointInvocations.SQueue<SafePointInvocation>::Enqueue(pInvocation);

        m_safePointInvocations.Release();

        //
        // pInvocation is must not be dereferenced after this point:
        //

    }

    /// <summary>
    ///     Registers a particular function to be called with particular data when a given scheduler reaches the next safe point
    ///     after the call is made.  This is an intrusive invocation with the current SafePointInvocation class incuring no heap
    ///     allocations.
    /// </summary>
    /// <param name="pInvocationFunction">
    ///     The function which will be invoked at the next safe point
    /// </param>
    /// <param name="pData">
    ///     User specified data.
    /// </param>
    /// <param name="pScheduler">
    ///     The scheduler on which to wait for a safe point to invoke pInvocationFunction.
    /// </param>
    void SafePointInvocation::InvokeAtNextSafePoint(InvocationFunction pInvocationFunction, void *pData, SchedulerBase *pScheduler)
    {
        m_pInvocation = pInvocationFunction;
        m_pData = pData;

        pScheduler->InvokeOnSafePoint(this);
    }

} // namespace details
} // namespace Concurrency

