// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// VirtualProcessor.cpp
//
// Source file containing the VirtualProcessor implementation.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Constructs a virtual processor.
    /// </summary>
    VirtualProcessor::VirtualProcessor()
        : m_localRunnableContexts(&m_lock),
          m_fThrottled(false)
    {
        // Derived classes should use Initialize(...) to init the virtual processor
    }

    /// <summary>
    ///     Initializes the virtual processor. This API is called by the constructor, and when a virtual processor is to
    ///     be re-initialized, when it is pulled of the free pool in the list array.
    /// </summary>
    /// <param name="pOwningNode">
    ///     The owning schedule node for this virtual processor
    /// </param>
    /// <param name="pOwningRoot"> 
    ///     The owning IVirtualProcessorRoot
    /// </param>
    void VirtualProcessor::Initialize(SchedulingNode *pOwningNode, IVirtualProcessorRoot *pOwningRoot)
    {
        //
        // A recycled virtual processor should be removed from throttling before being reused.
        //
        ASSERT(!IsThrottled());

        m_pOwningNode = pOwningNode;
        m_pOwningRing = pOwningNode->GetSchedulingRing();
        m_pCurrentRing = pOwningNode->GetSchedulingRing();
        m_pOwningRoot = pOwningRoot;
        m_fMarkedForRetirement = false;
        m_fRambling = false;
        m_fOversubscribed = false;
        m_fAvailable = TRUE;
        m_fHidden = false;
        m_enqueuedTaskCounter = 0;
        m_dequeuedTaskCounter = 0;
        m_enqueuedTaskCheckpoint = 0;
        m_dequeuedTaskCheckpoint = 0;
        m_pExecutingContext = NULL;
        m_pOversubscribingContext = NULL;
        m_safePointMarker.Reset();
        m_pSubAllocator = NULL;

        if (m_pOwningNode->GetScheduler()->GetSchedulingProtocol() == ::Concurrency::EnhanceScheduleGroupLocality)
            m_searchCtx.Reset(this, WorkSearchContext::AlgorithmCacheLocal);
        else
            m_searchCtx.Reset(this, WorkSearchContext::AlgorithmFair);

        // A virtual procesor has the same id as its associated virtual processor root. The roots have process unique ids.
        m_id = pOwningRoot->GetId();

        TraceVirtualProcessorEvent(CONCRT_EVENT_START, TRACE_LEVEL_INFORMATION, m_pOwningNode->m_pScheduler->Id(), m_id);
    }

    /// <summary>
    ///     Destroys a virtual processor
    /// </summary>
    VirtualProcessor::~VirtualProcessor()
    {
        ASSERT(m_localRunnableContexts.Count() == 0);

        if (m_pSubAllocator != NULL)
        {
            SchedulerBase::ReturnSubAllocator(m_pSubAllocator);
            m_pSubAllocator = NULL;
        }
    }

    /// <summary>
    ///     Activates a virtual processor with the context provided.
    /// </summary>
    void VirtualProcessor::Activate(IExecutionContext * pContext)
    {
        VMTRACE(MTRACE_EVT_ACTIVATE, ToInternalContext(pContext), this, SchedulerBase::FastCurrentContext());
#if _UMSTRACE
        ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();
        CMTRACE(MTRACE_EVT_ACTIVATE, (pCurrentContext && !pCurrentContext->IsExternal()) ? static_cast<InternalContextBase *>(pCurrentContext) : NULL, this, pContext);
        CMTRACE(MTRACE_EVT_ACTIVATED, ToInternalContext(pContext), this, pCurrentContext);
#endif // _UMSTRACE
        m_pOwningRoot->Activate(pContext);
    }

    /// <summary>
    ///     Temporarily deactivates a virtual processor.
    /// <summary>
    /// <returns>
    ///     An indication of which side the awakening occured from (true -- we activated it, false -- the RM awoke it).
    /// </returns>
    bool VirtualProcessor::Deactivate(IExecutionContext * pContext)
    {
        VMTRACE(MTRACE_EVT_DEACTIVATE, ToInternalContext(pContext), this, false);
#if _UMSTRACE
        ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();
        CMTRACE(MTRACE_EVT_DEACTIVATE, (pCurrentContext && !pCurrentContext->IsExternal()) ? static_cast<InternalContextBase *>(pCurrentContext) : NULL, this, pContext);
#endif // _UMSTRACE

        return m_pOwningRoot->Deactivate(pContext);
    }

    /// <summary>
    ///     Invokes the underlying virtual processor root to ensure all tasks are visible
    /// </summary>
    void VirtualProcessor::EnsureAllTasksVisible(IExecutionContext * pContext)
    {
        VMTRACE(MTRACE_EVT_DEACTIVATE, ToInternalContext(pContext), this, true);
#if _UMSTRACE
        ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();
        CMTRACE(MTRACE_EVT_DEACTIVATE, (pCurrentContext && !pCurrentContext->IsExternal()) ? static_cast<InternalContextBase *>(pCurrentContext) : NULL, this, pContext);
#endif // _UMSTRACE

        m_pOwningRoot->EnsureAllTasksVisible(pContext);
    }

    /// <summary>
    ///     Start a worker context executing on this.virtual processor.
    /// </summary>
    void VirtualProcessor::StartupWorkerContext(ScheduleGroupBase* pGroup)
    {
        TRACE(TRACE_SCHEDULER, L"VirtualProcessor::StartupWorkerContext");

        // This virtual processor might already have a context attached to it from the Dispatch loop, where it is waiting for work
        // to come in. If this is the case, there is no need to spin off another internal context to do the work.
        if (m_pExecutingContext == NULL)
        {
            InternalContextBase * pContext = pGroup->GetInternalContext();
            ASSERT(pContext != NULL);

            Affinitize(pContext);
            ASSERT(m_pExecutingContext == pContext);
        }
        else
        {
            ASSERT(!ToInternalContext(m_pExecutingContext) || ToInternalContext(m_pExecutingContext)->m_pVirtualProcessor == this);
        }
        m_pOwningRoot->Activate(m_pExecutingContext);
    }

    /// <summary>
    ///     Affinitizes an internal context to the virtual processor.
    /// </summary>
    /// <param name="pContext">
    ///     The internal context to affinitize.
    /// </param>
    void VirtualProcessor::Affinitize(InternalContextBase *pContext)
    {
        //
        // Wait until the context is firmly blocked, if it has started. This is essential to prevent vproc orphanage
        // if the context we're switching to is IN THE PROCESS of switching out to a different one. An example of how this
        // could happen:
        //
        // 1] ctxA is running on vp1. It is in the process of blocking, and wants to switch to ctxB. This means ctxA needs to
        // affintize ctxB to its own vproc, vp1.
        //
        // 2] At the exact same time, ctxA is unblocked by ctxY and put onto a runnables collection in its scheduler. Meanwhile, ctxZ
        // executing on vp2, has also decided to block. It picks ctxA off the runnables collection, and proceeds to switch to it.
        // This means that ctxZ needs to affinitize ctxA to ITS vproc vp2.
        //
        // 3] Now, if ctxZ affintizes ctxA to vp2 BEFORE ctxA has had a chance to affintize ctxB to vp1, ctxB gets mistakenly
        // affintized to vp2, and vp1 is orphaned.
        //
        // In order to prevent this, ctxZ MUST wait until AFTER ctxA has finished its affinitization. This is indicated via the 
        // blocked flag. ctxA will set its blocked flag to 1, after it has finished affintizing ctxB to vp1, at which point it is
        // safe for ctxZ to modify ctxA's vproc and change it from vp1 to vp2.
        //
        pContext->SpinUntilBlocked();
        pContext->PrepareToRun(this);

        VCMTRACE(MTRACE_EVT_AFFINITIZED, pContext, this, NULL);

#if defined(_DEBUG)
        pContext->ClearDebugBits();
        pContext->SetDebugBits(CTX_DEBUGBIT_AFFINITIZED);
#endif // _DEBUG

        // Make sure there is a two-way mapping between a virual processor and the affinitized context attached to it.
        // The pContext-> side of this mapping was established in PrepareToRun.
        m_pExecutingContext = pContext;

        //
        // If we were unable to update the statistical information because internal context was not
        // affinitized to a virtual processor, then do it now when the affinitization is done.
        //
        if (pContext->m_fHasDequeuedTask)
        {
            m_dequeuedTaskCounter++;
            pContext->m_fHasDequeuedTask = false;
        }
    }

    /// <summary>
    ///     Marks the the virtual processor such that it removes itself from the scheduler once the context it is executing
    ///     reaches a safe yield point. Alternatively, if the context has not started executing yet, it can be retired immediately.
    /// </summary>
    void VirtualProcessor::MarkForRetirement()
    {
        if (ClaimExclusiveOwnership())
        {
            // If there is a context attached to this virtual processor but we were able to claim it for
            // retirement then we have to unblock this context and send it for retirement. Otherwise, if
            // there was no context attached we can simply retire the virtual processor.
            if (m_pExecutingContext != NULL)
            {
                m_fMarkedForRetirement = true;
                m_pOwningRoot->Activate(m_pExecutingContext);
            }
            else
            {
                Retire();
            }
        }
        else
        {
            // Instruct the virtual processor to exit at a yield point - when the context it is executing enters the scheduler
            // from user code.
            m_fMarkedForRetirement = true;
        }
    }

    /// <summary>
    ///     Attempts to claim exclusive ownership of the virtual processor by resetting the the available flag.
    /// </summary>
    /// <returns>
    ///     True if it was able to claim the virtual processor, false otherwise.
    /// </returns>
    bool VirtualProcessor::ClaimExclusiveOwnership()
    {
        if ((m_fAvailable == TRUE) && (InterlockedExchange(&m_fAvailable, FALSE) == TRUE))
        {
#if _UMSTRACE
            ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();
            VCMTRACE(MTRACE_EVT_CLAIMEDOWNERSHIP, (pCurrentContext && !pCurrentContext->IsExternal()) ? static_cast<InternalContextBase *>(pCurrentContext) : NULL, this, SchedulerBase::FastCurrentContext());
#endif // _UMSTRACE

            if (!IsHidden())
            {
                InterlockedDecrement(&m_pOwningNode->m_pScheduler->m_virtualProcessorAvailableCount);
                InterlockedDecrement(&m_pOwningNode->m_virtualProcessorAvailableCount);
            }

            return true;
        }
        return false;
    }

    /// <summary>
    ///     Makes a virtual processor available for scheduling work.
    /// </summary>
    void VirtualProcessor::MakeAvailable()
    {
        ASSERT(m_fAvailable == FALSE);

#if _UMSTRACE
        ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();
        VCMTRACE(MTRACE_EVT_MADEAVAILABLE, (pCurrentContext && !pCurrentContext->IsExternal()) ? static_cast<InternalContextBase *>(pCurrentContext) : NULL, this, NULL);
#endif // _UMSTRACE

        InterlockedIncrement(&m_pOwningNode->m_pScheduler->m_virtualProcessorAvailableCount);
        InterlockedIncrement(&m_pOwningNode->m_virtualProcessorAvailableCount);
        InterlockedExchange(&m_fAvailable, TRUE);
    }

    /// <summary>
    ///     Oversubscribes the virtual processor by creating a new virtual processor root affinitized to the same
    ///     execution resource as that of the current root
    /// </summary>
    /// <returns>
    ///     A virtual processor that oversubscribes this one.
    /// </returns>
    VirtualProcessor * VirtualProcessor::Oversubscribe()
    {
        IVirtualProcessorRoot *pOversubscriberRoot = GetOwningNode()->GetScheduler()->GetSchedulerProxy()->CreateOversubscriber(m_pOwningRoot);
        ASSERT(pOversubscriberRoot != NULL);

        return m_pOwningNode->AddVirtualProcessor(pOversubscriberRoot, true);
    }

    /// <summary>
    ///     Causes the virtual processor to remove itself from the scheduler. This is used either when oversubscription
    ///     ends or when the resource manager asks the vproc to retire.
    /// </summary>
    void VirtualProcessor::Retire()
    {
        //
        // If this virtual processor is throttled, it's on a list in the background thread.  Remove it from that list so we do not have the background thread
        // attempting to wake a reused virtual processor after this goes on the free list!
        //
        if (IsThrottled())
            GetOwningNode()->GetScheduler()->RemoveThrottleOversubscriber(this);

        if (m_fRambling)
            UpdateRamblingState(false, NULL);

        // Virtual processor available counts are already decremented by this point. We need to decrement the total counts
        // on both the node and the scheduler. Oversubscribed vprocs do not contribute to the total vproc count on the scheduler.
        InterlockedDecrement(&m_pOwningNode->m_virtualProcessorCount);
        if (!m_fOversubscribed)
        {
            InterlockedDecrement(&m_pOwningNode->m_pScheduler->m_virtualProcessorCount);
        }

        // Since virtual processor is going away we'd like to preserve its counts
        m_pOwningNode->GetScheduler()->SaveRetiredVirtualProcessorStatistics(this);

        // If this is a virtual processor currently associated with an executing context, it's important to assert there that
        // the scheduler is not shutting down. We want to make sure that all virtual processor root removals (for executing virtual
        // processors) occur before the scheduler shuts down. This will ensure that all IVirtualProcessorRoot::Remove calls
        // that can originate from a scheduler's internal contexts instance are received the RM before the ISchedulerProxy::Shutdown call,
        // which asks the RM to release all resources and destroy the remaining virtual processor roots allocated to the scheduler.
        // RM should not receive Remove calls for roots that are already destroyed.
        ASSERT(m_pExecutingContext == NULL || ToInternalContext(m_pExecutingContext) == SchedulerBase::FastCurrentContext());
        ASSERT(m_pExecutingContext == NULL || (!m_pOwningNode->GetScheduler()->InFinalizationSweep() && !m_pOwningNode->GetScheduler()->HasCompletedShutdown()));

        m_pExecutingContext = NULL;

        // Check if there are contexts in the Local Runnables Collection and put them into the collection of runnables in their
        // respective schedule groups.
        InternalContextBase *pContext = GetLocalRunnableContext();
        while (pContext != NULL)
        {
            pContext->GetScheduleGroup()->AddRunnableContext(pContext);
            pContext = GetLocalRunnableContext();
        }

        // Send an IScheduler pointer into to Remove. Scheduler does derive from IScheduler, and therefore we need
        // an extra level of indirection.
        m_pOwningRoot->Remove(m_pOwningNode->GetScheduler()->GetIScheduler());
        m_pOwningRoot = NULL;

        TraceVirtualProcessorEvent(CONCRT_EVENT_END, TRACE_LEVEL_INFORMATION, m_pOwningNode->m_pScheduler->Id(), m_id);

        if (m_pSubAllocator != NULL)
        {
            SchedulerBase::ReturnSubAllocator(m_pSubAllocator);
            m_pSubAllocator = NULL;
        }

        // Removing this VirtualProcessor from the ListArray will move it to a pool for reuse
        // This must be done at the end of this function, otherwise, this virtual processor itself could be
        // pulled out of the list array for reuse and stomped over before retirement is complete.
        m_pOwningNode->m_virtualProcessors.Remove(this);
        // *DO NOT* touch 'this' after removing it from the list array.
    }

    /// <summary>
    ///     Returns a pointer to the suballocator for the virtual processor.
    /// </summary>
    SubAllocator * VirtualProcessor::GetCurrentSubAllocator()
    {
        if (m_pSubAllocator == NULL)
        {
            m_pSubAllocator = SchedulerBase::GetSubAllocator(false);
        }
        return m_pSubAllocator;
    }

    /// <summary>
    ///     Rambling -- searching foreign nodes for work. When work is found, update state accordingly.
    /// </summary>
    void VirtualProcessor::UpdateRamblingState(bool rambling, SchedulingRing *pCurrentRing)
    {
        //
        // One vproc triggers owning ring change for the whole node.
        // Other vproc has m_pCurrentRing change out from under it.
        //
        if (m_pCurrentRing != pCurrentRing)
        {
            if (rambling)
            {
                //
                // searching foreign rings
                //
                if ( !m_fRambling)
                {
#if _UMSTRACE
                    InternalContextBase *pCurrentContext = static_cast<InternalContextBase *>(SchedulerBase::FastCurrentContext());
                    CMTRACE(MTRACE_EVT_UPDATERAMBLING_RING, pCurrentContext, this, pCurrentRing);
#endif // _UMSTRACE
	
                    //
                    // Searching a foreign ring for the first time
                    //

                    m_pCurrentRing = pCurrentRing;

                    m_fRambling = TRUE;
                    
                    //
                    // Check to see if owning ring needs to be changed.
                    // When all vprocs in owning node are rambling, then we change the owning ring
                    // to be the one on which the maximal number of the owning node's vprocs
                    // are working.
                    //
                    if (InterlockedIncrement(&m_pOwningNode->m_ramblingCount) == m_pOwningNode->m_virtualProcessorCount)
                        m_pOwningNode->CheckForNewOwningRing();
                }
            }
            else if (m_fRambling)
            {
                // There is a by-design race here.
                //
                // The idea is to have 2 mechanisms to change owning ring.  
                // 1) SchedulingNode::CheckForNewOwningRing sees if all vprocs in the current node
                // are executing on some different ring than the node's associated ring (viz. they
                // are all rombling) and if so, changes the owning ring of all vprocs on the node
                // to the ring that has the most vprocs (on the node) executing on it.
                // 2) When a vproc again finds work on its node's associated ring, provide stickiness 
                // by resetting the owning and current ring to the node's associated ring.
                //
                // However, another path (CheckForNewOwningRing) could concurrently change the
                // owning and current ring.  This race is acceptable, because this vproc has found work
                // on its node's associated ring.
                //

                //
                // Reset back to node's associated ring (m_pOwningNode->GetSchedulingRing()).
                //
                m_pOwningRing = m_pCurrentRing = m_pOwningNode->GetSchedulingRing();

                // m_fRambling is set to false on multiple paths. 
                // Decrement the count only if this path successfully sets it to false
                if (InterlockedExchange(&m_fRambling, FALSE) == TRUE)
                    InterlockedDecrement(&m_pOwningNode->m_ramblingCount);
            }
        }
    }

    /// <summary>
    ///     Send a virtual processor ETW event.
    /// </summary>
    void VirtualProcessor::ThrowVirtualProcessorEvent(ConcRT_EventType eventType, UCHAR level, DWORD schedulerId, DWORD vprocId)
    {
        if (g_pEtw != NULL)
        {
            CONCRT_TRACE_EVENT_HEADER_COMMON concrtHeader = {0};

            concrtHeader.header.Size = sizeof concrtHeader;
            concrtHeader.header.Flags = WNODE_FLAG_TRACED_GUID;
            concrtHeader.header.Class.Type = (UCHAR)eventType;
            concrtHeader.header.Class.Level = level;
            concrtHeader.header.Guid = VirtualProcessorEventGuid;

            concrtHeader.SchedulerID = schedulerId;
            concrtHeader.VirtualProcessorID = vprocId;

            g_pEtw->Trace(g_ConcRTSessionHandle, &concrtHeader.header);
        }
    }

#if _UMSTRACE
    void VirtualProcessor::TraceSearchedLocalRunnables()
    {
        ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();
        CMTRACE(MTRACE_EVT_SEARCHEDLOCALRUNNABLES, (pCurrentContext && !pCurrentContext->IsExternal()) ? static_cast<InternalContextBase *>(pCurrentContext) : NULL, this, NULL);
    }
#endif // UMSTRACE

    /// <summary>
    ///     Returns a type-cast of pContext to an InternalContextBase or NULL if it is not.
    /// </summary>
    InternalContextBase *VirtualProcessor::ToInternalContext(IExecutionContext *pContext)
    {
        return static_cast<InternalContextBase *>(pContext);
    }

    /// <summary>
    ///     Called when the context running atop this virtual processor has reached a safe point.
    /// </summary>
    /// <returns>
    ///     An indication of whether the caller should perform a commit.
    /// </returns>
    bool VirtualProcessor::SafePoint()
    {
        return GetOwningNode()->GetScheduler()->MarkSafePoint(&m_safePointMarker);
    }

} // namespace details
} // namespace Concurrency
