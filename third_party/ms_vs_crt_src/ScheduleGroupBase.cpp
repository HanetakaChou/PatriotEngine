// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// ScheduleGroupBase.cpp
//
// Implementation file for ScheduleGroupBase.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{

    /// <summary>
    ///     Constructs a schedule group with an initial reference count of 1.
    /// </summary>
    ScheduleGroupBase::ScheduleGroupBase(SchedulingRing *pRing) 
        : m_pRing(pRing),
          m_refCount(1),
          m_workQueues(pRing->m_pScheduler, 256, 64),
          m_detachedWorkQueues(pRing->m_pScheduler, 256, ListArray< ListArrayInlineLink<WorkQueue> >::DeletionThresholdInfinite) // No deletion
    {
        m_pScheduler = m_pRing->m_pScheduler;
        m_id = m_pScheduler->GetNewScheduleGroupId();
    }

    /// <summary>
    ///     Adds runnable context to the schedule group. This is usually a previously blocked context that
    ///     was subsequently unblocked, but it could also be an internal context executing chores on behalf
    ///     of an external context.
    /// </summary>
    void ScheduleGroupBase::AddRunnableContext(InternalContextBase* pContext, VirtualProcessor *pBias)
    {
        ASSERT(pContext->GetScheduleGroup() == this);
        //
        // If the current context does not belong to this group, the caller is not guaranteed to have a reference to the
        // schedule group. We call CrossGroupRunnable() to make sure that scheduler and schedule group are kept around long
        // enough, that we can attempt to startup the virtual processor without fear of the scheduler being finalized, or the
        // schedule group being destroyed.
        //
        ContextBase* pCurrentContext = SchedulerBase::FastCurrentContext();

        if ((pCurrentContext == NULL) || (pCurrentContext->GetScheduleGroup() != this))
        {
            // Set this flag to allow the calling thread to use 'this' safely once the context is pushed onto runnables.
            // Note that this call does not need a fence because it is fenced by push to the runnable contexts collection.
            pContext->CrossGroupRunnable(TRUE);
        }

        // Add it to the actual collection.
        AddToRunnablesCollection(pContext);

        if (m_pScheduler->m_virtualProcessorAvailableCount > 0)
        {
            m_pScheduler->StartupIdleVirtualProcessor(this, pBias);
        }

        // Reset the flag, if it was set, since we're done with touching scheduler/context data.
        // This flag is not fenced. This means the reader could end up spinning a little longer until the data is
        // propagated by the cache coherency mechanism.
        pContext->CrossGroupRunnable(FALSE);
        // NOTE: It is not safe to touch 'this' after this point, if this was a cross group runnable.
    }

    /// <summary>
    ///     Steals an unrealized chore from a workqueue in the schedule group.
    /// </summary>
    _UnrealizedChore* ScheduleGroupBase::StealUnrealizedChore() 
    {
        //
        // When we fail to steal from a work queue that's detached, it's an indication that the work queue
        // is finally empty and can be retired.
        //

        _UnrealizedChore *pChore;

        bool killEmptyQueues = false;
        int maxIndex = m_workQueues.MaxIndex();
        if (maxIndex > 0)
        {
            int skippedCount = 0;
            const int maxSkippedCount = 16;
            int skippedState[maxSkippedCount];
            bool fEntered = false;

            for (int j = 0; j < maxIndex; j++)
            {
                WorkQueue *pQueue = m_workQueues[j];
                if (pQueue != NULL)
                {
                    if ( !pQueue->IsEmpty())
                    {
                        if ((pChore = pQueue->TryToSteal(fEntered)) != NULL)
                            return pChore;
                        else if ( !fEntered)
                        {
                            if (skippedCount < maxSkippedCount-1)
                            {
                                skippedState[skippedCount++] = j;
                                continue;
                            }
                            else if ((pChore = pQueue->Steal()) != NULL)
                                return pChore;
                        }

                        killEmptyQueues |= (pQueue->IsDetached() && pQueue->IsEmpty());
                    }
                    else
                        killEmptyQueues |= pQueue->IsDetached();
                }
            }

            if (skippedCount > 0)
            {
                for (int j = 0; j < skippedCount; j++)
                {
                    WorkQueue *pQueue = m_workQueues[skippedState[j]];
                    if (pQueue != NULL)
                    {
                        if ( !pQueue->IsEmpty() && (pChore = pQueue->Steal()) != NULL)
                            return pChore;
                        else
                            killEmptyQueues |= (pQueue->IsDetached() && pQueue->IsEmpty());
                    }
                }
            }
        }

        int numDetachedArrays = m_detachedWorkQueues.MaxIndex();
        if (numDetachedArrays > 0 && killEmptyQueues)
        {
            for (int i = 0; i < m_workQueues.MaxIndex(); i++)
            {
                WorkQueue *pQueue = m_workQueues[i];
                if (pQueue != NULL)
                {
                    if (pQueue->IsDetached() && pQueue->IsUnstructuredEmpty()) 
                    {
                        SafelyDeleteDetachedWorkQueue(pQueue);
                    }
                }
            }
        }

        return NULL;
    }

    /// <summary>
    ///     Returns true if the group has any realized chores.
    ///     This is used during scheduler finalization when only one thread is active in the scheduler.
    ///     At any other time, this information is stale since new work could get added to the scheduler.
    /// </summary>
    bool ScheduleGroupBase::HasRealizedChores() const
    {
        return !m_realizedChores.Empty();
    }

    /// <summary>
    ///     Returns true if any of the workqueues in the schedule group has unrealized chores.
    ///     This is used during scheduler finalization when only one thread is active in the scheduler.
    ///     At any other time, this information is stale since new work could get added to the scheduler.
    /// </summary>
    bool ScheduleGroupBase::HasUnrealizedChores()
    {
        for (int i = 0; i < m_workQueues.MaxIndex(); i++)
        {
            WorkQueue *pQueue = m_workQueues[i];
            if (pQueue != NULL)
            {
                if (!pQueue->IsStructuredEmpty() || !pQueue->IsUnstructuredEmpty())
                {
                    return true;
                }
                else if (pQueue->IsDetached())
                {
                    SafelyDeleteDetachedWorkQueue(pQueue);
                }
            }
        }

        return false;
    }

    /// <summary>
    ///     Called to safely delete a detached work queue -- this is lock free and utilizes safe points to perform
    ///     the deletion and dereference.  It can be called during the normal SFW or during the finalization sweep
    ///     safely.
    /// </summary>
    bool ScheduleGroupBase::SafelyDeleteDetachedWorkQueue(WorkQueue *pQueue)
    {
        //
        // The way in which we resolve race conditions between this and queue reattachment is by who is able to remove the
        // element from the detached list array.  We cannot kill the work queue until it's gone out of that list array.
        //
        if (m_detachedWorkQueues.Remove(&pQueue->m_detachment, false))
        {
            //
            // There's always the possibility of a very subtle race where we check IsDetached and IsUnstructuredEmpty and then
            // are preempted, the queue is reattached, work is added, and it's detached again in the same spot with work.  We
            // cannot free the queue in such circumstance.  Only if it is empty AFTER removal from m_detachedWorkQueues are
            // we safe.
            //
            if (pQueue->IsUnstructuredEmpty())
            {
                //
                // Each detached work queue holds a reference on the group.  It is referenced
                // in ScheduleGroupBase::DetachActiveWorkQueue().  Since we are removing this
                // empty work queue, we need to release the reference.
                //
                // There's an unfortunate reality here -- this work queue might be the LAST thing holding reference onto
                // the schedule group.  It's entirely possible that someone just stole and hasn't yet gotten to the point
                // where a reference is added to the schedule group.  If we arbitrarily release this reference, we might delete
                // (or reuse) an active schedule group.  This could cause all sorts of problems.
                //
                // Instead of trying to release that reference here, we will wait until the next safe point to do so.  We 
                // are guaranteed no one is in the middle of stealing from this schedule group at that time.
                //
                // Note that this means that the stealer **MUST** stay within a critical region until after the WorkItem::TransferReferences
                // call.
                //
                pQueue->RetireAtSafePoint(this);
                return true;
            }
            else
            {
                CORE_ASSERT(!m_pScheduler->InFinalizationSweep());

                //
                // The queue is not empty and we need to roll back.  Since we never removed the queue from m_workQueues, the work will 
                // still be found by the scheduler without undue futzing around sleep states.  The queue must, however, be placed
                // back in m_detachedWorkQueues in a detached state.
                //
                // There's an unfortunate reality here too -- the slot used for the queue within the detached queues list might already
                // be gone.  Adding back to the detached queues might trigger a heap allocation.  Given that this might be in SFW, a heap allocation
                // triggering UMS would be bad.  Hence -- if we need to roll back (unlikely), we must do this at a safe point.
                //
                pQueue->RedetachFromScheduleGroupAtSafePoint(this);
            }
        }

        return false;
    }

    /// <summary>
    ///     Creates a realized (non workstealing) chore in the schedule group. Used to schedule light-weight
    ///     tasks and agents.
    /// </summary>
    void ScheduleGroupBase::ScheduleTask(__in TaskProc proc, void* data)
    {
        if (proc == NULL)
        {
            throw std::invalid_argument("proc");
        }

        RealizedChore *pChore = m_pScheduler->GetRealizedChore(proc, data);
        TRACE(TRACE_SCHEDULER, L"ScheduleGroupBase::ScheduleTask(sgroup=%d,ring=0x%p,chore=0x%p)\n", Id(), m_pRing, pChore);

        // Every task takes a reference on its schedule group. This is to ensure a schedule group has a ref count > 0 if
        // no contexts are working on it, but queued tasks are present. The reference count is transferred to the context
        // that eventually executes the task.
        InternalReference();

        m_realizedChores.Enqueue(pChore);

        ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();

        if (pCurrentContext == NULL || pCurrentContext->GetScheduler() != m_pScheduler)
        {
            //
            // This is a thread that is in no way tracked in ConcRT (no context assigned to it) or it is a context foreign to
            // this scheduler, so we cannot have statistics directly associated with its context. Instead, there is an entry in
            // the TLS section PER scheduler that points to the external statistics mapping. From that information, we can know
            // whether we have seen this thread before and whether it was ever scheduling tasks on the current scheduler.
            //
            ExternalStatistics * externalStatistics = (ExternalStatistics *) TlsGetValue(m_pScheduler->m_dwExternalStatisticsIndex);

            if (externalStatistics == NULL)
            {
                //
                // This is the first piece of statistical data for this thread on this scheduler, so
                // create a statistics class, add it to the list array of statistics on this scheduler and
                // save it in the TLS slot reserved for statistics on this scheduler.
                //
                externalStatistics = new ExternalStatistics();
                m_pScheduler->AddExternalStatistics(externalStatistics);
                TlsSetValue(m_pScheduler->m_dwExternalStatisticsIndex, externalStatistics);
            }
            else
            {
                //
                // We already have some statistical data for this thread on this scheduler.
                //
                ASSERT(m_pScheduler->m_externalThreadStatistics.MaxIndex() > 0);
            }

            ASSERT(externalStatistics != NULL);
            externalStatistics->IncrementEnqueuedTaskCounter();
        }
        else if (pCurrentContext->IsExternal())
        {
            static_cast<ExternalContextBase *>(pCurrentContext)->IncrementEnqueuedTaskCounter();
        }
        else
        {
            static_cast<InternalContextBase *>(pCurrentContext)->IncrementEnqueuedTaskCounter();
        }

        // In most cases this if check will fail. To avoid the function call overhead in the common case, we check
        // for virtual processors beforehand.
        if (m_pScheduler->m_virtualProcessorAvailableCount > 0)
        {
            m_pScheduler->StartupNewVirtualProcessor(this);
        }

    }

    /// <summary>
    ///     Places a work queue in the detached queue.  This will cause the work queue to remain eligible for stealing
    ///     while the queue can be detached from a context.  The work queue will be recycled and handed back to a 
    ///     context executing within the schedule group that needs a queue.  If the queue is not recycled, it will be
    ///     abandoned and freed when it becomes empty (a steal on it while in detached mode fails).
    /// </summary>
    void ScheduleGroupBase::DetachActiveWorkQueue(WorkQueue *pWorkQueue)
    {
        InternalReference();

        //
        // Note: there is a distinct lack of relative atomicity between the flag set and the queue add.  The worst thing that
        //       happens here is that we ask the list array to remove an element at an invalid index.  It is prepared to handle
        //       that anyway.
        //
        pWorkQueue->SetDetached(true);
        m_detachedWorkQueues.Add(&pWorkQueue->m_detachment);
    }

    /// <summary>
    ///     Called by a work queue in order to roll back an attempted kill that could not be committed due to reuse. 
    /// </summary>
    void ScheduleGroupBase::RedetachQueue(WorkQueue *pWorkQueue)
    {
        //
        // Roll back by reinserting into m_detachedWorkQueues.  We detect the error before setting detached state to false or releasing
        // reference, so this is the only operation which needs to happen.  It just cannot happen during the steal due to the fact that
        // there is a **SLIGHT** chance that the call will perform a heap allocation.
        //
        m_detachedWorkQueues.Add(&pWorkQueue->m_detachment);
    }

    /// <summary>
    ///     Attempts to acquire a detached work queue from the schedule group.  If such a work queue is found, it
    ///     is removed from detached queue and returned.  This allows recycling of work queues that are detached
    ///     yet still have unstructured work.
    ///</summary>
    WorkQueue *ScheduleGroupBase::GetDetachedWorkQueue()
    {
        int maxIdx = m_detachedWorkQueues.MaxIndex();
        for (int i = 0; i < maxIdx; i++)
        {
            ListArrayInlineLink<WorkQueue> *pLink = m_detachedWorkQueues[i];

            //
            // No code below this may dereference pLink unless it is removed from the list array.  There is no guarantee
            // of safety as this can be called from an external context or multiple internal contexts.
            //
            if (pLink != NULL && m_detachedWorkQueues.Remove(pLink, i, false))
            {
                WorkQueue *pWorkQueue = pLink->m_pObject;

                pWorkQueue->SetDetached(false);

                //
                // This removed detached work queue incremented the reference count
                // in ScheduleGroupBase::DetachActiveWorkQueue().  Release it now.
                // 
                // This is safe because we are inside the schedule group getting a work queue.  This means that there is already
                // some context with a reference on the schedule group and it won't disappear out from underneath us by removing
                // the detach reference.
                //
                InternalRelease();

                return pWorkQueue;
            }
        }

        return NULL;
    }

    /// <summary>
    ///     Called by a work queue in order to retire itself at a safe point.
    /// </summary>
    void ScheduleGroupBase::RetireDetachedQueue(WorkQueue *pWorkQueue)
    {
        VERIFY(m_workQueues.Remove(pWorkQueue));

        //
        // This removed detached work queue incremented the reference count
        // in ScheduleGroupBase::DetachActiveWorkQueue().  Release it now.
        // 
        InternalRelease();
    }

    RealizedChore * ScheduleGroupBase::GetRealizedChore()
    {
        if (m_realizedChores.Empty())
            return NULL;

        RealizedChore *pChore = m_realizedChores.Dequeue();
        TRACE(TRACE_SCHEDULER, L"ScheduleGroup::GetRealizedChore(sgroup=%d,ring=0x%p,chore=0x%p)\n", Id(), m_pRing, pChore);
        return pChore;
    }

    /// <summary>
    ///     Gets an internal context from either the idle pool or a newly allocated one and prepares it for
    ///     exection. A NULL return value from the routine is considered fatal (out of memory). This is the
    ///     API that should be used to obtain an internal context for exection. The context is associated
    //      with this schedule group.
    /// </summary>
    InternalContextBase * ScheduleGroupBase::GetInternalContext(_Chore *pChore, bool choreStolen)
    {
        // Get an internal context from the idle pool
        InternalContextBase* pContext = m_pScheduler->GetInternalContext();

        // Associate it with this schedule group
        ASSERT(pContext != NULL);
        pContext->PrepareForUse(this, pChore, choreStolen);

        return pContext;
    }

    /// <summary>
    ///     Releases an internal context after execution into the idle pool. If the idle pool
    ///     is full, it could be freed.
    /// </summary>
    void ScheduleGroupBase::ReleaseInternalContext(InternalContextBase *pContext)
    {
        pContext->RemoveFromUse();
        m_pScheduler->ReleaseInternalContext(pContext);
    }

} // namespace details
} // namespace Concurrency
