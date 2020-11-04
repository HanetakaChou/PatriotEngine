// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// VirtualProcessor.h
//
// Source file containing the VirtualProcessor declaration.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

namespace Concurrency
{
namespace details
{
    //
    // virtualized hardware thread
    //
    /// <summary>
    ///     A virtual processor is an abstraction of a hardware thread.  However, there may very well be more than one
    ///     virtual processor per hardware thread.  The <c>SchedulerPolicy</c> key <c>TargetOversubscriptionFactor</c> determines
    ///     the number of virtual processor per hardware thread, scheduler wide.
    /// </summary>
    /// <remarks>
    ///     Virtual processors may be created and destroyed at any time, since resource management (RM) may give or take 
    ///     away hardware threads.  But as such batches of <c>TargetOversubscriptionFactor</c> virtual processors are created or
    ///     destroyed simultaneously.
    /// </remarks>
    class VirtualProcessor
    {
    public:

        /// <summary>
        ///     Constructs a virtual processor.
        /// </summary>
        VirtualProcessor();

        /// <summary>
        ///     Destroys a virtual processor
        /// </summary>
        virtual ~VirtualProcessor();

        /// <summary>
        ///     Returns a scheduler unique identifier for the virtual processor.
        /// </summary>
        unsigned int GetId() const { return m_id; }

        /// <summary>
        ///     Rambling -- searching foreign nodes for work. When work is found, update state accordingly.
        /// </summary>
        void UpdateRamblingState(bool rambling, SchedulingRing *pCurrentRing);

        /// <summary>
        ///     Attempts to claim exclusive ownership of the virtual processor by resetting the the available flag.
        /// </summary>
        /// <returns>
        ///     True if it was able to claim the virtual processor, false otherwise.
        /// </returns>
        bool ClaimExclusiveOwnership();

        /// <summary>
        ///     Makes a virtual processor available for scheduling work.
        /// </summary>
        void MakeAvailable();

        /// <summary>
        ///     Returns a pointer to the internal context that is executing on this virtual processor.
        /// </summary>
        IExecutionContext * GetExecutingContext() { return m_pExecutingContext; }

        /// <summary>
        ///     Returns a pointer to the owning node for the virtual processor.
        /// </summary>
        SchedulingNode * GetOwningNode() { return m_pOwningNode; }

        /// <summary>
        ///     Returns a pointer to the owning ring for the virtual processor.
        /// </summary>
        SchedulingRing * GetOwningRing() { return m_pOwningRing; }

        /// <summary>
        ///     Returns a pointer to the owning root for the virtual processor.
        /// </summary>
        IVirtualProcessorRoot * GetOwningRoot() { return m_pOwningRoot; }

        /// <summary>
        ///     Returns a pointer to the suballocator for the virtual processor.
        /// </summary>
        SubAllocator * GetCurrentSubAllocator();

        /// <summary>
        ///     Returns true if the virtual processor is marked as available, false otherwise.
        /// </summary>
        bool IsAvailable() { return (m_fAvailable == TRUE); }

        /// <summary>
        ///     Returns true if the virtual processor is marked for retirement, false otherwise.
        /// </summary>
        bool IsMarkedForRetirement() { return m_fMarkedForRetirement; }

        /// <summary>
        ///     Activates a virtual processor with the context provided.
        /// </summary>
        void Activate(IExecutionContext *pContext);

        /// <summary>
        ///     Temporarily deactivates a virtual processor.
        /// <summary>
        /// <returns>
        ///     An indication of which side the awakening occured from (true -- we activated it, false -- the RM awoke it).
        /// </returns>
        bool Deactivate(IExecutionContext *pContext);

        /// <summary>
        ///     Invokes the underlying virtual processor root to ensure all tasks are visible.
        /// </summary>
        void EnsureAllTasksVisible(IExecutionContext * pContext);

#if _UMSTRACE
        void Trace(int traceEvt, void *pCtx, void *pVproc, ULONG_PTR data)
        {
            m_traceBuffer.Trace(traceEvt, pCtx, pVproc, data);
        }
#endif // _UMSTRACE

        /// <summary>
        ///     Returns the default destination of a SwitchTo(NULL).  There is none on a default virtual processor.
        /// </summary>
        virtual IExecutionContext *GetDefaultDestination()
        {
            return NULL;
        }

        /// <summary>
        ///     Performs a search for work for the given virtual processor.
        /// </summary>
        bool SearchForWork(WorkItem *pWorkItem, ScheduleGroupBase *pOriginGroup)
        {
            return m_searchCtx.Search(pWorkItem, pOriginGroup);
        }

        /// <summary>
        ///     Performs a search for work for the given virtual processor only allowing certain types of work to be found.
        /// </summary>
        bool SearchForWork(WorkItem *pWorkItem, ScheduleGroupBase *pOriginGroup, WorkItem::WorkItemType allowableTypes)
        {
            return m_searchCtx.Search(pWorkItem, pOriginGroup, allowableTypes);
        }

        /// <summary>
        ///     Performs a yielding search for work for the given virtual processor.
        /// </summary>
        bool SearchForWorkInYield(WorkItem *pWork, ScheduleGroupBase *pOriginGroup)
        {
            return m_searchCtx.YieldingSearch(pWork, pOriginGroup);
        }

        /// <summary>
        ///     Performs a yielding search for work for the given virtual processor only allowing certain types of work to be found.
        /// </summary>
        bool SearchForWorkInYield(WorkItem *pWorkItem, ScheduleGroupBase *pOriginGroup, WorkItem::WorkItemType allowableTypes)
        {
            return m_searchCtx.YieldingSearch(pWorkItem, pOriginGroup, allowableTypes);
        }

        /// <summary>
        ///     Stub called in SFW before we search for runnable contexts.
        /// </summary>
        /// <returns>
        ///     A context which should be run.
        /// </returns>
        virtual InternalContextBase *PreRunnableSearch()
        {
            return NULL;
        }

        /// <summary>
        ///     Called when the context running atop this virtual processor has reached a safe point.
        /// </summary>
        /// <returns>
        ///     An indication of whether the caller should perform a commit.
        /// </returns>
        bool SafePoint();

        /// <summary>
        ///     Hides or unhides a virtual processor.  This may only be called for oversubscribed virtual processors in certain places!
        /// </summary>
        void Hide(bool fHidden)
        {
            m_fHidden = fHidden;
        }

        /// <summary>
        ///     Returns whether an oversubscribed virtual processor is "hidden" currently.
        /// </summary>
        bool IsHidden() const
        {
            return m_fHidden;
        }

        /// <summary>
        ///     Notifies the virtual processor that its activation is being throttled or unthrottled.
        /// </summary>
        void ThrottleActivation(bool fThrottled)
        {
            m_fThrottled = fThrottled;
        }

        /// <summary>
        ///     Answers the instantaneous query as to whether the virtual processor is throttled.  This information is stale the moment it is returned.
        /// </summary>
        bool IsThrottled() const
        {
            return (m_fThrottled);
        }

    protected:
        //
        // protected data
        //

        // Indicates whether vproc is available to perform work.
        volatile LONG m_fAvailable;

        // Local caching of realized chores/contexts
        StructuredWorkStealingQueue<InternalContextBase, _HyperNonReentrantLock> m_localRunnableContexts;

        // The search context which keeps track of where this virtual processor is in a search-for-work regardless of algorithm.
        WorkSearchContext m_searchCtx;

        // Owning scheduling node -- immutable
        SchedulingNode *m_pOwningNode;

        // Current scheduling node
        SchedulingRing *m_pCurrentRing;

        // Owning ring -- may change
        SchedulingRing *m_pOwningRing;

        // Owning virtual processor root
        IVirtualProcessorRoot *m_pOwningRoot;

        // Sub allocator
        SubAllocator *m_pSubAllocator;

        // Flag that specifies if the virtual processor is working on a ring that is different from it's owning
        // ring.
        volatile LONG m_fRambling;

        // The index that this Virtual Processor is at in its list array
        int m_listArrayIndex;

        // Statistics data counters
        unsigned int m_enqueuedTaskCounter;
        unsigned int m_dequeuedTaskCounter;

        // Statistics data checkpoints
        unsigned int m_enqueuedTaskCheckpoint;
        unsigned int m_dequeuedTaskCheckpoint;

        // Internal context that is affinitized to and executing on this virtual processor
        // This is always an InternalContextBase except on UMS in very special circumstances.
        IExecutionContext * m_pExecutingContext;

        _HyperNonReentrantLock m_lock;

        // Unique identifier for vprocs within a scheduler.
        unsigned int m_id;

        // Flag specifying whether this is a virtual processor created as a result of a call to Oversubscribe.
        bool m_fOversubscribed;

        // Flag that is set when the virtual processor should remove itself from the scheduler at a yield point,
        // i.e, either when the context executing on it calls Block or Yield, or when it is in the dispatch loop
        // looking for work.
        bool m_fMarkedForRetirement;

        // Whether this virtual processor has reached a safe point in the code
        // Used to demark when all virtual processors have reached safe points and a list array deletion
        // can occur
        bool m_fReachedSafePoint;

        // A hidden virtual processor is one that was created for Oversubscribe but whose activation is throttled.
        bool m_fHidden;

        /// <summary>
        ///     Causes the virtual processor to remove itself from the scheduler. This is used either when oversubscription
        ///     ends or when the resource manager asks the vproc to retire.
        /// </summary>
        virtual void Retire();

        // The internal context that caused this virtual processor to be created, if this is an oversubscribed vproc.
        InternalContextBase * m_pOversubscribingContext;

        /// <summary>
        ///     Affinitizes an internal context to the virtual processor.
        /// </summary>
        /// <param name="pContext">
        ///     The internal context to affinitize.
        /// </param>
        virtual void Affinitize(InternalContextBase *pContext);

        /// <summary>
        ///     Returns a type-cast of pContext to an InternalContextBase or NULL if it is not.
        /// </summary>
        virtual InternalContextBase *ToInternalContext(IExecutionContext *pContext);

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
        virtual void Initialize(SchedulingNode *pOwningNode, IVirtualProcessorRoot *pOwningRoot);

    private:
        friend class SchedulerBase;
        friend class ContextBase;
        friend class InternalContextBase;
        friend class ThreadInternalContext;
        friend class UMSThreadInternalContext;
        friend class UMSSchedulingContext;
        friend class ScheduleGroup;
        friend class FairScheduleGroup;
        friend class CacheLocalScheduleGroup;
        friend class SchedulingNode;
        friend class WorkSearchContext;
        template <class T> friend class ListArray;
        template<class T, class Counter> friend class List;

        // Indication of throttling.
        bool m_fThrottled;

        // Links for throttling
        VirtualProcessor *m_pNext;
        VirtualProcessor *m_pPrev;

        // Intrusive links for list array.
        SLIST_ENTRY m_listArrayFreeLink;

        // The safe point marker.
        SafePointMarker m_safePointMarker;

#if _UMSTRACE
        _TraceBuffer m_traceBuffer;
#endif // _UMSTRACE

        /// <summary>
        ///     Start a worker context executing on this.virtual processor.
        /// </summary>
        virtual void StartupWorkerContext(ScheduleGroupBase* pGroup);

        /// <summary>
        ///     Oversubscribes the virtual processor by creating a new virtual processor root affinitized to the same
        ///     execution resource as that of the current root
        /// </summary>
        /// <returns>
        ///     A virtual processor that oversubscribes this one.
        /// </returns>
        virtual VirtualProcessor * Oversubscribe();

        /// <summary>
        ///     Marks the the virtual processor such that it removes itself from the scheduler, once the context it is executing
        ///     reaches a safe yield point. Alternatively, if the context has not started executing yet, it can be retired immediately.
        /// </summary>
        void MarkForRetirement();

#if _UMSTRACE
        void TraceSearchedLocalRunnables();
#endif // _UMSTRACE

        /// <summary>
        ///     Steals a context from the local runnables queue of this virtual processor.
        /// </summary>
        InternalContextBase *StealLocalRunnableContext()
        {
#if _UMSTRACE
            TraceSearchedLocalRunnables();
#endif // _UMSTRACE

            InternalContextBase *pContext = m_localRunnableContexts.Steal();
            if (pContext != NULL)
            {
#if defined(_DEBUG)
                Concurrency::details::SetContextDebugBits(pContext, CTX_DEBUGBIT_STOLENFROMLOCALRUNNABLECONTEXTS);
#endif // _DEBUG
            }

            return pContext;
        }

        /// <summary>
        ///     Pops a runnable context from the local runnables queue of the vproc, if it can find one.
        /// </summary>
        InternalContextBase *GetLocalRunnableContext()
        {
#if _UMSTRACE
            TraceSearchedLocalRunnables();
#endif // _UMSTRACE

            if (m_localRunnableContexts.Count() > 0) // Is this check worthwhile?  Yes, I believe.  We'd take a fence to check otherwise.
            {
                InternalContextBase *pContext = m_localRunnableContexts.Pop();
#if defined(_DEBUG)
                Concurrency::details::SetContextDebugBits(pContext, CTX_DEBUGBIT_POPPEDFROMLOCALRUNNABLECONTEXTS);
#endif // _DEBUG
                return pContext;
            }
            return NULL;
        }

        /// <summary>
        ///     Resets the count of work coming in.
        /// </summary>
        /// <remarks>
        ///     This function is using modulo 2 behavior of unsigned ints to avoid overflow and
        ///     reset problems. For more detail look at ExternalStatistics class in externalcontextbase.h.
        /// </remarks>
        /// <returns>
        ///     Previous value of the counter.
        /// </returns>
        unsigned int GetEnqueuedTaskCount()
        {
            unsigned int currentValue = m_enqueuedTaskCounter;
            unsigned int retVal = currentValue - m_enqueuedTaskCheckpoint;

            // Update the checkpoint value with the current value
            m_enqueuedTaskCheckpoint = currentValue;

            ASSERT(retVal < INT_MAX);
            return retVal;
        }

        /// <summary>
        ///     Resets the count of work being done.
        /// </summary>
        /// <remarks>
        ///     This function is using modulo 2 behavior of unsigned ints to avoid overflow and
        ///     reset problems. For more detail look at the ExternalStatistics class in externalcontextbase.h.
        /// </remarks>
        /// <returns>
        ///     Previous value of the counter.
        /// </returns>
        unsigned int GetDequeuedTaskCount()
        {
            unsigned int currentValue = m_dequeuedTaskCounter;
            unsigned int retVal = currentValue - m_dequeuedTaskCheckpoint;

            // Update the checkpoint value with the current value
            m_dequeuedTaskCheckpoint = currentValue;

            ASSERT(retVal < INT_MAX);
            return retVal;
        }

        /// <summary>
        ///     Send a virtual processor ETW event
        /// </summary>
        void TraceVirtualProcessorEvent(ConcRT_EventType eventType, UCHAR level, DWORD schedulerId, DWORD vprocId)
        {
            if (g_TraceOn && level <= g_EnableLevel)
                ThrowVirtualProcessorEvent(eventType, level, schedulerId, vprocId);
        }

        /// <summary>
        ///     Send a virtual processor ETW event
        /// </summary>
        static void ThrowVirtualProcessorEvent(ConcRT_EventType eventType, UCHAR level, DWORD schedulerId, DWORD vprocId);
    };
} // namespace details
} // namespace Concurrency
