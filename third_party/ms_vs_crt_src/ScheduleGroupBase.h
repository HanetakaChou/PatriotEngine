// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// ScheduleGroupBase.h
//
// Header file containing ScheduleGroup related declarations.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

namespace Concurrency
{
namespace details
{

    class ScheduleGroupBase : public ScheduleGroup
    {

    public:

        //
        // Public Methods
        //

        /// <summary>
        ///     Constructs a schedule group
        /// </summary>
        ScheduleGroupBase(SchedulingRing *pRing);

        /// <summary>
        ///     Virtual destructor
        /// </summary>
        virtual ~ScheduleGroupBase() 
        {
            // There shall be no work queues  (detached or otherwise) when a schedule group
            // is deleted. This assumption is made in our safe point mechanism. If one
            // of the workqueues in a schedule group requests a safe point invocation after
            // the one for schedule group deletion, the workqueues would be deleted before 
            // its callback is invoked.
            ASSERT(m_workQueues.IsEmptyAtSafePoint());
            ASSERT(m_detachedWorkQueues.IsEmptyAtSafePoint());
        }

        /// <summary>
        ///     Returns a unique identifier to the schedule group.
        /// </summary>
        unsigned int Id() const
        {
            return m_id;
        }

        /// <summary>
        ///     Increments the reference count of a schedule group.  A reference count is held for
        ///       - every unstarted or incomplete realized chore that is part of the schedule group
        ///       - every context that is executing a chore that was stolen from an unrealized chore queue
        ///         within the schedule group
        ///       - every external context attached to the scheduler instance, IFF this is the anonymous
        ///         schedule group for the scheduler instance,
        ///       - an external caller, IFF this schedule group was created using one of the public task
        ///         creation APIs.
        /// </summary>
        /// <returns>
        ///     Returns the resulting reference count.
        /// </returns>
        virtual unsigned int Reference()
        {
            return (unsigned int)InternalReference();
        }

        /// <summary>
        ///     Decrements the reference count of a schedule group.  Used for composition.
        /// </summary>
        /// <returns>
        ///     Returns the resulting reference count.
        /// </returns>
        virtual unsigned int Release()
        {
            return (unsigned int)InternalRelease();
        }

        /// <summary>
        ///     Schedules a realized (non workstealing) chore in the schedule group. Used to schedule light-weight
        ///     tasks and agents.
        /// </summary>
        void ScheduleTask(__in TaskProc proc, void* data);

        /// <summary>
        ///     Returns the scheduling ring the group belongs to.
        /// </summary>
        SchedulingRing * GetSchedulingRing() { return m_pRing; }

        /// <summary>
        ///     Returns a pointer to the scheduler this group belongs to.
        /// </summary>
        SchedulerBase * GetScheduler() { return m_pScheduler; }

        /// <summary>
        ///     Called by a work queue in order to retire itself at a safe point.
        /// </summary>
        void RetireDetachedQueue(WorkQueue *pWorkQueue);

        /// <summary>
        ///     Called by a work queue in order to roll back an attempted kill that could not be committed due to reuse. 
        /// </summary>
        void RedetachQueue(WorkQueue *pWorkQueue);

    protected:
        friend class SchedulerBase;
        friend class ContextBase;
        friend class ExternalContextBase;
        friend class InternalContextBase;
        friend class ThreadInternalContext;
        friend class SchedulingNode;
        friend class SchedulingRing;
        friend class VirtualProcessor;
        friend class UMSSchedulingContext;
        friend class WorkItem;
        friend class WorkSearchContext;
        template <class T> friend class ListArray;

        enum {
            CacheLocalScheduling = 1,
            FairScheduling = 2,
            AnonymousScheduleGroup = 4
        };

        //
        // Private data
        //

        // Each schedule group has three stores of work. It has a collection of runnable contexts (in the derived classes),
        // a FIFO queue of realized chores and a list of workqueues that hold unrealized chores.

        // A queue of realized chores.
        SafeSQueue<RealizedChore, _HyperNonReentrantLock> m_realizedChores;

        // A list array of all unrealized chore queues that are owned by contexts in this schedule group, 
        // protected by a r/w lock.
        ListArray<WorkQueue> m_workQueues;

        // Owning ring
        SchedulingRing *m_pRing;

        // Owning scheduler
        SchedulerBase *m_pScheduler;

        // The index that this schedule group is at in its containing ListArray
        int m_listArrayIndex;

        // Reference count for the schedule group
        volatile long m_refCount;

        // Unique identifier
        unsigned int m_id;

        // flag indicating schedule group kind
        BYTE m_kind;

        // A list array of work queues which still contain work within the schedule group but have become detached
        // from their parent context (e.g.: a chore queues unstrucured work and does not wait upon it before
        // exiting).  This is the first level "free list".  Any context needing a work queue can grab one from
        // here assuming it's executing the same schedule group.
        ListArray< ListArrayInlineLink<WorkQueue> > m_detachedWorkQueues;

        //
        // Private methods
        //

        /// <summary>
        ///     Adds runnable context to the schedule group. This is usually a previously blocked context that
        ///     was subsequently unblocked, but it could also be an internal context executing chores on behalf
        ///     of an external context.
        /// </summary>
        void AddRunnableContext(InternalContextBase *pContext, VirtualProcessor *pBias = NULL);

        /// <summary>
        ///     Puts a runnable context into the runnables collection in the schedule group.
        /// </summary>
        virtual void AddToRunnablesCollection(InternalContextBase *pContext) =0;

        virtual InternalContextBase *GetRunnableContext() = 0;

        /// <summary>
        ///     Returns true if the group has any realized chores.
        ///     This is used during scheduler finalization when only one thread is active in the scheduler.
        ///     At any other time, this information is stale since new work could get added to the scheduler.
        /// </summary>
        bool HasRealizedChores() const;

        /// <summary>
        ///     Returns true if any of the workqueues in the schedule group has unrealized chores.
        ///     This is used during scheduler finalization when only one thread is active in the scheduler.
        ///     At any other time, this information is stale since new work could get added to the scheduler.
        /// </summary>
        bool HasUnrealizedChores();

        /// <summary>
        ///     Returns a realized chore if one exists in the queue.
        /// </summary>
        RealizedChore *GetRealizedChore();

        /// <summary>
        ///     Acquires an internal context for execution
        /// </summary>
        InternalContextBase* GetInternalContext(_Chore *pChore = NULL, bool choreStolen = false);

        /// <summary>
        ///     Releases an internal context after execution
        /// </summary>
        void ReleaseInternalContext(InternalContextBase *pContext);

        bool IsFairScheduleGroup() const { return (m_kind & FairScheduling) != 0; }

        /// <summary>
        ///     Steals an unrealized chore from a workqueue in the schedule group.
        /// </summary>
        _UnrealizedChore* StealUnrealizedChore();

        /// <summary>
        ///     Non-virtual function that increments the reference count of a schedule group.
        /// </summary>
        LONG ScheduleGroupBase::InternalReference()
        {
            if ((m_kind & AnonymousScheduleGroup) == 0)
            {
                ASSERT(m_refCount >= 0);
                TRACE(TRACE_SCHEDULER, L"ScheduleGroupBase::InternalReference(rc=%d)\n", m_refCount+1);
                return InterlockedIncrement(&m_refCount);
            }
            return 0;
        }

        /// <summary>
        ///     Non-virtual function that decrements the reference count of a schedule group.
        /// </summary>
        LONG ScheduleGroupBase::InternalRelease()
        {
            if ((m_kind & AnonymousScheduleGroup) == 0)
            {
                ASSERT(m_refCount > 0);
                TRACE(TRACE_SCHEDULER, L"ScheduleGroupBase::InternalRelease(rc=%d)\n", m_refCount-1);
                LONG newValue = InterlockedDecrement(&m_refCount);
                if (newValue == 0)
                    m_pRing->RemoveScheduleGroup(this);
                return newValue;
            }
            return 0;
        }

        /// <summary>
        ///     Attempts to acquire a detached work queue from the schedule group.  If such a work queue is found, it
        ///     is removed from detached queue and returned.  This allows recycling of work queues that are detached
        ///     yet still have unstructured work.
        ///</summary>
        WorkQueue *GetDetachedWorkQueue();

        /// <summary>
        ///     Places a work queue in the detached queue.  This will cause the work queue to remain eligible for stealing 
        ///     while the queue can be detached from a context.  The work queue will be recycled and handed back to a 
        ///     context executing within the schedule group that needs
        ///     a queue.  If the queue is not recycled, it will be abandoned and freed when it becomes empty (a steal on it
        ///     while in detached mode fails).
        /// </summary>
        void DetachActiveWorkQueue(WorkQueue *pWorkQueue);

        /// <summary>
        ///     Called to safely delete a detached work queue -- this is lock free and utilizes safe points to perform
        ///     the deletion and dereference.  It can be called during the normal SFW or during the finalization sweep
        ///     safely.
        /// </summary>
        bool SafelyDeleteDetachedWorkQueue(WorkQueue *pQueue);

    private:

        // Intrusive links for list array.
        SLIST_ENTRY m_listArrayFreeLink;

    };
} // namespace details
} // namespace Concurrency
