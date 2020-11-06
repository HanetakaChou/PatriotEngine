// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// UMSThreadScheduler.h
//
// Source file containing the implementation for a UMS thread based concrt scheduler 
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#pragma once

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    DWORD UMSThreadScheduler::t_dwSchedulingContextIndex;

    /// <summary>
    ///     Creates a UMS thread based scheduler
    /// </summary>
    UMSThreadScheduler::UMSThreadScheduler(__in const ::Concurrency::SchedulerPolicy& policy) :
        SchedulerBase(policy),
        m_pCompletionList(NULL),
        m_pendingRequests(0)
    {
    }

    /// <summary>
    ///     Creates a UMS thread based scheduler
    /// </summary>
    UMSThreadScheduler* UMSThreadScheduler::Create(__in const ::Concurrency::SchedulerPolicy& policy)
    {
        return new UMSThreadScheduler(policy);
    }

    /// <summary>
    ///     Creates a UMS thread based virtual processor.
    /// </summary>
    VirtualProcessor* UMSThreadScheduler::CreateVirtualProcessor(SchedulingNode *pOwningNode, IVirtualProcessorRoot* pOwningRoot)
    {
        return new UMSThreadVirtualProcessor(pOwningNode, pOwningRoot);
    }

    ///<summary>
    ///     Creates a new thread internal context and returns it to the base scheduler.
    ///</summary>
    InternalContextBase *UMSThreadScheduler::CreateInternalContext()
    {
        return new UMSThreadInternalContext(this);
    }

    /// <summary>
    ///     Destroys a UMS thread based scheduler
    /// </summary>
    UMSThreadScheduler::~UMSThreadScheduler()
    {
    }

    /// <summary>
    ///     Starts up new virtual processor if one is found. The virtual processor is assigned a context
    ///     that starts its search for work in the schedule group specified.
    ///     In UMS scheduler, a vproc could be deactivated waiting for resources such as reserved contexts.
    ///     They need to be excluded when activating a new virtual processor. New work needs to activate
    ///     a vproc that is not waiting for resources. This is required to honour user specified min
    ///     concurrency level.
    /// </summary>
    void UMSThreadScheduler::StartupNewVirtualProcessor(ScheduleGroupBase *pGroup)
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

        UMSThreadVirtualProcessor *pVirtualProcessor = FindNewVirtualProcessor();

        if (pVirtualProcessor != NULL)
        {
           // Found a vproc to activate
           ActivateVirtualProcessor(pVirtualProcessor, pGroup);
        }

        ContextBase::StaticExitHyperCriticalRegion();
    }

    /// <summary>
    ///     Find an idle virtual processor that is not waiting for resources (such as reserved contexts)
    ///     claim the vproc and return it.
    /// </summary>
    UMSThreadVirtualProcessor * UMSThreadScheduler::FindNewVirtualProcessor()
    {
        for (int node = 0; node < m_nodeCount; ++node)
        {
            SchedulingNode * pNode = m_nodes[node];
            int idx;

            if (pNode != NULL)
            {
                UMSThreadVirtualProcessor *pVirtualProcessor = 
                   static_cast<UMSThreadVirtualProcessor *>(pNode->GetFirstVirtualProcessor(&idx));

                while (pVirtualProcessor != NULL)
                {
                   // Skip vprocs that are waiting for resources.
                   if (!pVirtualProcessor->IsWaitingForReservedContext() && pVirtualProcessor->ClaimExclusiveOwnership())
                   {
                       // Found a vproc
                       return pVirtualProcessor;
                   }
                   
                   pVirtualProcessor = static_cast<UMSThreadVirtualProcessor *>(pNode->GetNextVirtualProcessor(&idx));
                }
            }
        }

        return NULL;
    }

    ///<summary>
    ///     Notification after a virtual processor goes from INACTIVE to ACTIVE or ACTIVE to INACTIVE
    ///     For UMS we need to ensure that there is atleast 1 active vproc.
    ///</summary>
    /// <param value="fActive"> 
    ///     True if a virtual processor is going from INACTIVE to ACTIVE, and false if it is going from ACTIVE to INACTIVE.
    /// </param>
    /// <param value="activeCount"> 
    ///     Active virtual processor count after the transition
    /// </param>
    void UMSThreadScheduler::VirtualProcessorActiveNotification(bool fActive, LONG activeCount)
    {
        // We need to maintain at least 1 active virtual processor as long
        // as there is work. Since we cannot easily determine if there are blocked UMS context,
        // do not allow active vproc count to go from 1 to 0.
        if (activeCount == 0)
        {
            // If we are the last active virtual processor, we should be in a hyper cricital region
            CORE_ASSERT(!fActive);
            StartupIdleVirtualProcessor(GetNextSchedulingRing()->GetAnonymousScheduleGroup());
        }
    }

    ///<summary>
    ///     Determines if there is pending work such as blocked context/unstarted chores etc in the
    ///     scheduler. If there is no pending work, the scheduler will attempt to shutdown. 
    ///     For UMS, look for pending requests from vprocs for reserved contexts
    ///</summary>
    bool UMSThreadScheduler::HasWorkPending()
    {
        return (m_pendingRequests > 0) || SchedulerBase::HasWorkPending();
    }

    /// <summary>
    ///     Replenishes the list of reserved contexts. Reserved contexts are just
    ///     a cache of internal contexts that are associated with a thread proxy.
    /// </summary>
    void UMSThreadScheduler::ReplenishReservedContexts()
    {
        ASSERT(m_pendingRequests > 0);

        // The number of pending requests could be much bigger than what is
        // really required. This is because a vproc could have a search loop
        // that attempts to get a SFW context a few times before waiting on 
        // the list event. So we will create a fixed number of contexts and
        // set the event. The number is bounded by the number of active vprocs
        // the scheduler.
        long numContextToCreate = GetNumberOfActiveVirtualProcessors() + 1;

        // Limit the number of contexts created at a time. Allow a few of
        // the vprocs to do useful work while we create more contexts.
        if (numContextToCreate > 4)
        {
            numContextToCreate = 4;
        }

        // m_numReservedContexts doesn't need to be very accurate. In the worst
        // case the background thread would need to run an extra iteration.
        while (m_numReservedContexts < numContextToCreate)
        {
            InternalContextBase * pContext = GetInternalContext();

            // Add to the reserved context list
            _InterlockedIncrement(&m_numReservedContexts);
            m_reservedContexts.Push(pContext);
        }

        // Allow the background thread to be signalled again
        _InterlockedExchange(&m_pendingRequests, 0);

        // Indicate that the reserved list has been Replenished. Note that all
        // the context could have been consumed before we set this event. Anyone
        // waiting on the event would do a GetReservedContext which would return NULL.
        // This would wake up the background thread for Replenishing...
        SignalReservedContextsAvailable();
    }

    /// <summary>
    ///     Releases the list of reserved contexts to the idle pool. The thread proxy
    ///     is released before returning the contexts to the idle pool
    /// </summary>
    void UMSThreadScheduler::ReleaseReservedContexts()
    {
        InternalContextBase *pContext = m_reservedContexts.Pop();

        while (pContext != NULL)
        {
            GetSchedulerProxy()->UnbindContext(pContext);
            ReleaseInternalContext(pContext);

            pContext = m_reservedContexts.Pop();
        }

        m_numReservedContexts = 0;
        ASSERT(m_pendingRequests == 0);
    }

    /// <summary>
    ///     Attempts to get an internal context for execution. If an internal
    ///     context is obtained the routine prepares it for execution by 
    ///     associating a schedule group with it. Note that this routine can
    ///     return NULL. 
    /// </summary>
    InternalContextBase * UMSThreadScheduler::GetReservedContext()
    {
        InternalContextBase *pContext = m_reservedContexts.Pop();

        if (pContext != NULL)
        {
            _InterlockedDecrement(&m_numReservedContexts);

            pContext->PrepareForUse(GetAnonymousScheduleGroup(), NULL, false);
        }
        else
        {
            if (_InterlockedIncrement(&m_pendingRequests) == 1)
            {
                // Wake up the background thread
                SetEvent(m_hCreateContext);
            }
        }

        return pContext;
    }

    /// <summary>
    ///     Release reserved contexts to idle pool and cancel all internal contexts
    /// </summary>
    void UMSThreadScheduler::CancelAllContexts()
    {
        // We need to be in a hypercritical region (this code path shall not rely
        // on another UT to be scheduled).
        ContextBase *pContext = FastCurrentContext();
        bool fExitHyperCriticalRegion = false;

        if ((pContext != NULL) && (!pContext->IsExternal()))
        {
            fExitHyperCriticalRegion = true;
            pContext->EnterHyperCriticalRegion();
        }

        // Sweep ensures that there are no pending requests
        CORE_ASSERT(m_pendingRequests == 0);

        ReleaseReservedContexts();

        if (fExitHyperCriticalRegion)
        {
            pContext->ExitHyperCriticalRegion();
        }

        // Defer to the base class to cancel the remaining contexts
        SchedulerBase::CancelAllContexts();
    }

    /// <summary>
    ///     Create a background thread for UT creation
    /// </summary>
    void UMSThreadScheduler::InitializeSchedulerEventHandlers()
    {
        // We need to create background thread to create reserved contexts. The 
        // external context exit handler thread could be reused. For now, we have a 
        // dedicated background thread for UMS.

        m_numReservedContexts = 0;
        m_pendingRequests = 0;
        m_fCancelContextCreationHandler = false;

        // Auto reset event.
        m_hCreateContext = CreateEventW(NULL, FALSE, FALSE, NULL);

        // Event handlers take an internal reference on the scheduler which is released when they exit.
        IncrementInternalContextCount();

        HANDLE threadHandle = LoadLibraryAndCreateThread(NULL,
                                           DEFAULTCONTEXTSTACKSIZE,
                                           BackgroundContextCreationProc,
                                           this,
                                           0,
                                           NULL);

        CloseHandle(threadHandle);

        // Allow base class to register any handlers if required.
        SchedulerBase::InitializeSchedulerEventHandlers();
    }

    /// <summary>
    ///     Main thread procedure for the background threads
    /// </summary>
    DWORD CALLBACK UMSThreadScheduler::BackgroundContextCreationProc(LPVOID lpParameter)
    {
        UMSThreadScheduler * pScheduler = reinterpret_cast<UMSThreadScheduler*>(lpParameter);
        pScheduler->WaitForContextCreationRequest();
        FreeLibraryAndDestroyThread(0);
        return 0;
    }

    /// <summary>
    ///     Wait loop for reserved context creation
    /// </summary>
    void UMSThreadScheduler::WaitForContextCreationRequest()
    {
        ULONG lastReplenishTime = 0;
    
        for (;;)
        {
            WaitForSingleObject(m_hCreateContext, INFINITE);

            if (m_fCancelContextCreationHandler)
            {
                ASSERT(m_pendingRequests == 0);
                ASSERT(m_numReservedContexts == 0);
                break;
            }

            //
            // In order to prevent an explosion of threads in the case where we perform an operation like:
            //
            // parallel_for*(large_range) 
            // {
            //     latent_blocking_operation();
            // }
            //
            // We will throttle the creation of threads here.  Note that this is only a mitigation and eventually the throttling
            // should become pervasive throughout the scheduler.  No matter the level of throttling here, if we hit a system cap on the number
            // of threads, this will still throw.
            //
            ULONG delay = ThrottlingTime(4);
            ULONG curTime = GetTickCount();
            ULONG delta = curTime - lastReplenishTime; // okay with wrap

            if (delay > 0 && delta < delay)
            {
                Sleep(delay - delta);
            }
            lastReplenishTime = curTime;

            ReplenishReservedContexts();
        }

        CloseHandle(m_hCreateContext);

        // NOTE: Decrementing the internal context context count could finalize the scheduler - it is not safe to touch
        // *this* after this point.
        DecrementInternalContextCount();
    }

    /// <summary>
    ///     Destroy the UT creation background thread
    /// </summary>
    void UMSThreadScheduler::DestroySchedulerEventHandlers()
    {
        // Cancel the UT creation thread
        m_fCancelContextCreationHandler = true;
        SetEvent(m_hCreateContext);
 
        // Allow base class to destroy any handlers it has
        SchedulerBase::DestroySchedulerEventHandlers();
    }

    /// <summary>
    ///     Signal threads waiting for reserved contexts to become
    ///     available.
    /// </summary>
    void UMSThreadScheduler::SignalReservedContextsAvailable()
    {
        bool activated = false;

        // UMS virtual processors attempt to create SFW context when
        // the executing context is UMS blocked (non-cooperative blocking).
        // If we are unable to create one at that time, the vproc will
        // deactivate. They need to be woken up when the background thread
        // manages to create SFW contexts. All such vprocs need to be woken
        // up even if we do not have enough reserved contexts. Their requests
        // would fail and wake up the background thread...
 
        // Activate vprocs that are waiting for reserved contexts
        for (int node = 0; node < m_nodeCount; ++node)
        {
            SchedulingNode * pNode = m_nodes[node];
 
            if (pNode != NULL)
            {
                if (ActivateAllReservedVirtualProcessors(pNode))
                {
                    activated = true;
                }
            }
        }
 
        // At least 1 vproc needs to be activated since pending requests blocks
        // scheduler shutdown. Activate a vproc after clearing the pending request
        // count to restart scheduler shutdown.
        if (!activated)
        {
            StartupIdleVirtualProcessor(GetAnonymousScheduleGroup());
        }
    }

    /// <summary>
    ///     Activate all vprocs that were waiting for reserved cotnexts to become
    ///     available.
    /// </summary>
    bool UMSThreadScheduler::ActivateAllReservedVirtualProcessors(SchedulingNode * pNode)
    {
        CORE_ASSERT(pNode != NULL);
 
        bool activated = false;
        int idx;
        UMSThreadVirtualProcessor *pVirtualProcessor = static_cast<UMSThreadVirtualProcessor *>(pNode->GetFirstVirtualProcessor(&idx));
 
        while (pVirtualProcessor != NULL)
        {
            if (pVirtualProcessor->IsWaitingForReservedContext() && pVirtualProcessor->ClaimExclusiveOwnership())
            {
                pVirtualProcessor->StartupWorkerContext(GetAnonymousScheduleGroup());
                activated = true;
            }
            
            pVirtualProcessor = static_cast<UMSThreadVirtualProcessor *>(pNode->GetNextVirtualProcessor(&idx));
        }
 
        return activated;
    }

    /// <summary>
    ///     Called in order to move the completion list to the runnables lists.
    /// </summary>
    /// <param name="pBias">
    ///     Bias any awakening of virtual processors to the scheduling node that
    ///     pBias belongs to.
    /// </param>
    /// <returns>
    ///     Whether there was anything on the completion list when queried.
    /// </returns>
    bool UMSThreadScheduler::MoveCompletionListToRunnables(VirtualProcessor *pBias)
    {
        bool fHadItems = false;

        //
        // This *ABSOLUTELY* must be in a hyper-critical region.  Deadlock can ensue if not.  Anything outward from this
        // must follow the set of rules governing a hyper-critical region.
        //
        ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();
        if (pCurrentContext != NULL)
            pCurrentContext->EnterHyperCriticalRegion();

        IUMSUnblockNotification *pUnblock = m_pCompletionList->GetUnblockNotifications();
        while (pUnblock != NULL)
        {
            fHadItems = true;

            IUMSUnblockNotification *pNext = pUnblock->GetNextUnblockNotification();
            UMSThreadInternalContext *pContext = static_cast<UMSThreadInternalContext *> (pUnblock->GetContext());

            VCMTRACE(MTRACE_EVT_PULLEDFROMCOMPLETION, pContext, (pCurrentContext && !pCurrentContext->IsExternal()) ? static_cast<InternalContextBase *>(pCurrentContext)->m_pVirtualProcessor : NULL, pCurrentContext);

#if defined(_DEBUG)
            pContext->SetDebugBits(CTX_DEBUGBIT_PULLEDFROMCOMPLETIONLIST);
#endif // _DEBUG

            //
            // In order to know what to do with this particular item, we need to know *why* it blocked.  If the primary hasn't gotten to telling us that,
            // we must spin.
            //
            UMSThreadInternalContext::BlockingType blockingType = pContext->SpinOnAndReturnBlockingType();
            CORE_ASSERT(blockingType != UMSThreadInternalContext::BlockingNone);

            //
            // Make a determination of where this item goes.  There are several cases here:
            //
            // - It might have UMS blocked during a normal critical region (e.g.: the main dispatch loop blocked on the heap lock or some
            //   other similar object).  If the context was inside a critical region, we have special determinations to make.
            //
            // - It might just be a runnable.
            //
            switch(blockingType)
            {
                case UMSThreadInternalContext::BlockingCritical:
                    //
                    // This is the single special context allowed to be "inside a critical region" on the virtual processor.  Signal the virtual
                    // processor specially.
                    //
                    VCMTRACE(MTRACE_EVT_CRITICALNOTIFY, pContext, (pCurrentContext && !pCurrentContext->IsExternal()) ? static_cast<InternalContextBase *>(pCurrentContext)->m_pVirtualProcessor : NULL, pCurrentContext);
#if defined(_DEBUG)
                    pContext->SetDebugBits(CTX_DEBUGBIT_CRITICALNOTIFY);
#endif // _DEBUG
                    pContext->m_pLastVirtualProcessor->CriticalNotify();
                    break;

                case UMSThreadInternalContext::BlockingNormal:
                    //
                    // If it's a normal runnable, it just goes on the runnables list.  We pass along the bias to indicate which virtual processor
                    // (or owning node) we prefer to awaken due to the addition of runnables.
                    //
                    pContext->AddToRunnables(pBias);
                    break;
            }

            pUnblock = pNext;
            
        }

        if (pCurrentContext != NULL)
            pCurrentContext->ExitHyperCriticalRegion();

        return fHadItems;

    }

    /// <summary>
    ///     Static initialization common to UMS schedulers.
    /// </summary>
    void UMSThreadScheduler::OneShotStaticConstruction()
    {
        t_dwSchedulingContextIndex = TlsAlloc();
        if (t_dwSchedulingContextIndex == TLS_OUT_OF_INDEXES)
        {
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
        }
    }

    /// <summary>
    ///     Static destruction common to UMS schedulers.
    /// </summary>
    void UMSThreadScheduler::OneShotStaticDestruction()
    {
        TlsFree(t_dwSchedulingContextIndex);
        t_dwSchedulingContextIndex = 0;
    }

} // namespace details
} // namespace Concurrency
