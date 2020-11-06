// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// workqueue.cpp
//
// Work stealing queues pair implementation.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Constructs a new work queue.
    /// </summary>
    WorkQueue::WorkQueue()
        : m_structuredQueue(&m_lock),
        m_unstructuredQueue(&m_lock),
        m_pOwningContext(NULL),
        m_detachmentState(QUEUE_ATTACHED)
    {
        m_detachment.m_listArrayIndex = 0;
        m_detachment.m_pObject = this;
        m_id = SchedulerBase::GetNewWorkQueueId();
    }

    /// <summary>
    ///     Steal a chore from the work stealing queue for unrealized parallelism.
    /// </summary>
    _UnrealizedChore *WorkQueue::UnlockedSteal()
    {
        if (IsEmpty())
            return NULL;

        //
        // Make certain we do not steal from a canceled context.  The exception handling will properly unwind this.  This avoids a degree on infighting
        // on cancellation.  When a subtree which has stolen chores is canceled, the stealing threads are canceled, unwind, and immediately become available
        // for stealing.  They can easily pick up a region of the tree where exception handling has not unwound to the cancellation point and the exception
        // handlers wind up in a little battle with the stealing threads (the exception handlers shooting down contexts and the contexts stealing different
        // pieces of the tree, wash, rinse, repeat).  In order to avoid *some* of the contention, we simply will refuse to steal from any context which was 
        // shot down as part of a subtree cancellation.
        //
        // Further, as an optimization, we do temporarily suspend stealing from a context which has an inflight cancellation -- even if the stealing happens
        // in a different piece of the subtree than being canceled.  This allows us to avoid yet more infighting during a cancellation.  It should be a rare
        // enough event that the suspension of stealing from that context during the in flight cancel shouldn't hurt performance.  Note that this is the difference
        // between the pCheckContext->IsCanceled() and a pCheckContext->IsCanceledContext() below.  The latter would only check fully canceled contexts.  The
        // former does the suspension on contexts with in flight cancellations.
        //

        _UnrealizedChore *pResult = NULL; 
        ContextBase *pOwningContext = m_pOwningContext;
        if (pOwningContext != NULL && pOwningContext->IsCanceled())
        {
            //
            // There is a scenario that we need to be extraordinarily careful of here.  Normally, we could just ignore the steal.  Unfortunately,
            // it's entirely possible that a detached queue was reused on a context (A) and then A became cancelled while we waited on a task
            // collection that had chores in the detached region.  This would lead to deadlock.  We need to allow stealing from the region of
            // the WSQ which was detached.  
            //
            // m_unstructuredQueue may contains tasks from task collections that are not being canceled.  However, we cannot quickly detect the difference.
            // Thus before arbitrary stealing from m_unstructuredQueue, we need to finish cancelation of the task collections that need to be canceled, which is why
            // pOwningContext->IsCanceled() is true.  Cancellation is over when pOwningContext->IsCanceled() is false and then uninhibited stealing from m_unstructuredQueue
            // may occur again.  However, in the meantime, stealing threads may go idle because no work can be found and if m_pOwningContext exits w/o waiting,
            // there may be work left to execute, but all vprocs are idle.  This 'deadlock' is prevented by calling NotifyWorkSkipped() that assures an associated vproc will 
            // not go idle without verifying there is no work left by executing this function again.
            //
            if (m_unstructuredQueue.MarkedForDetachment())
                pResult = (_UnrealizedChore *) m_unstructuredQueue.UnlockedSteal(true);
            if (pResult != NULL)
                pResult->_SetDetached(true);
            else
                static_cast<InternalContextBase *>(SchedulerBase::FastCurrentContext())->NotifyWorkSkipped();
        }
        else
        {
            pResult = (_UnrealizedChore *) m_structuredQueue.UnlockedSteal();

            //
            // Structured does not need to deal with detachment and cancellation references, simply return the value.
            //
            if (pResult != NULL)
                return pResult;

            pResult = (_UnrealizedChore *) m_unstructuredQueue.UnlockedSteal(false);

            //
            // If we stole and there is an owning context and it's not a detached steal (which always goes on the task collection list), keep the owning context
            // alive until the wrapper can add the task to the appropriate cancellation list.
            //  NOTE:  pResult is unstructured.
            //
            if (pResult != NULL)
            {
                if (m_pOwningContext != NULL 
                    && reinterpret_cast <ContextBase *> (pResult->_OwningCollection()->_OwningContext()) == m_pOwningContext)
                {
                    m_pOwningContext->ReferenceForCancellation();
                    pResult->_SetDetached(false);
                } else
                    pResult->_SetDetached(true);
            }
        }

        return pResult;
    }

    /// <summary>
    ///     Attempts to steal an unrealized chore from the unstructured work stealing queue.
    /// </summary>
    /// <param name="fSuccessfullyAcquiredLock">
    ///     The try lock was successfully acquired.
    /// </param>
    /// <returns>
    ///     An unrealized chore stolen from the work stealing queues or NULL if no such chore can be stolen
    /// </returns>
    _UnrealizedChore *WorkQueue::TryToSteal(bool& fSuccessfullyAcquiredLock)
    {
        _UnrealizedChore *pResult = NULL;
        if (m_lock._TryAcquire())
        {
            __try
            {
                pResult = UnlockedSteal();
            }
            __finally
            {
                m_lock._Release();
            }
            fSuccessfullyAcquiredLock = true;
        }
        else
            fSuccessfullyAcquiredLock = false;
        
        return pResult;
    }

    /// <summary>
    ///     Steal a chore from both work stealing queues.
    /// </summary>
    _UnrealizedChore* WorkQueue::Steal()
    {
        _CriticalNonReentrantLock::_Scoped_lock lockHolder(m_lock);
        return UnlockedSteal();
    }

    /// <summary>
    ///     Sweeps the unstructured work stealing queue for items matching a predicate and potentially removes them
    ///     based on the result of a callback.
    /// </summary>
    /// <param name="pPredicate">
    ///     The predicate for things to call pSweepFn on.
    /// </param>
    /// <param name="pData">
    ///     The data for the predicate callback
    /// </param>
    /// <param name="pSweepFn">
    ///     The sweep function
    /// </param>
    void WorkQueue::SweepUnstructured(WorkStealingQueue<_UnrealizedChore>::SweepPredicate pPredicate,
                                      void *pData,
                                      WorkStealingQueue<_UnrealizedChore>::SweepFunction pSweepFn
                                      )
    {
        m_unstructuredQueue.Sweep(pPredicate, pData, pSweepFn);
    }

    /// <summary>
    ///     Causes a detached work queue to release its reference on the inpassed schedule group and remove itself from that schedule group's
    ///     list of work queues at the next available safe point.
    /// </summary>
    void WorkQueue::RetireAtSafePoint(ScheduleGroupBase *pScheduleGroup)
    {
        m_pDetachedScheduleGroup = pScheduleGroup;
        m_detachmentSafePoint.InvokeAtNextSafePoint(reinterpret_cast<SafePointInvocation::InvocationFunction>(&WorkQueue::StaticRetire),
                                                    this,
                                                    pScheduleGroup->GetScheduler());
    }

    /// <summary>
    ///     Causes a detached work queue to redetach due to roll-back of retirement at the next available safe point.
    /// </summary>
    void WorkQueue::RedetachFromScheduleGroupAtSafePoint(ScheduleGroupBase *pScheduleGroup)
    {
        m_pDetachedScheduleGroup = pScheduleGroup;
        m_detachmentSafePoint.InvokeAtNextSafePoint(reinterpret_cast<SafePointInvocation::InvocationFunction>(&WorkQueue::StaticRedetachFromScheduleGroup),
                                                    this,
                                                    pScheduleGroup->GetScheduler());
    }

    /// <summary>
    ///     Retires the detached work queue.
    /// </summary>
    void WorkQueue::StaticRetire(WorkQueue *pQueue)
    {
        pQueue->m_pDetachedScheduleGroup->RetireDetachedQueue(pQueue);
    }

    /// <summary>
    ///     Places the work queue back in a detached state on roll back.
    /// </summary>
    void WorkQueue::StaticRedetachFromScheduleGroup(WorkQueue *pQueue)
    {
        pQueue->m_pDetachedScheduleGroup->RedetachQueue(pQueue);
    }

} // namespace details
} // namespace Concurrency
