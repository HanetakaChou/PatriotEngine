// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// Context.h
//
// Header file containing the metaphor for an execution context/stack/thread.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#pragma once

// Defines used for context blocking (m_blockedState):
// * Possible blocked states
#define CONTEXT_NOT_BLOCKED         0x0
#define CONTEXT_BLOCKED             0x1
#define CONTEXT_UMS_SYNC_BLOCKED    0x2
#define CONTEXT_UMS_ASYNC_BLOCKED   0x4

// * Useful bit-masks
#define CONTEXT_SYNC_BLOCKED        (CONTEXT_BLOCKED | CONTEXT_UMS_SYNC_BLOCKED)
#define CONTEXT_UMS_BLOCKED         (CONTEXT_UMS_SYNC_BLOCKED | CONTEXT_UMS_ASYNC_BLOCKED)

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Implements the base class for a ConcRT execution context.
    /// </summary>
    class ContextBase : public Context
    {
    public:
        //
        // Public Methods
        //

        /// <summary>
        ///     Constructor
        /// </summary>
        ContextBase(SchedulerBase *pScheduler, bool fIsExternal);

        /// <summary>
        ///     Returns a unique identifier to the context.
        /// </summary>
        virtual unsigned int GetId() const;

        /// <summary>
        ///     Returns an identifier to the virtual processor the context is currently executing on, if any.
        /// </summary>
        virtual unsigned int GetVirtualProcessorId() const =0;

        /// <summary>
        ///     Returns an identifier to the schedule group the context is currently working on, if any.
        /// <summary>
        virtual unsigned int GetScheduleGroupId() const;

        /// <summary>
        ///     Returns the reference count of the underlying schedule group, which is equivalent
        ///     to the number of contexts performing work on the schedule group.
        /// <summary>
        unsigned int ScheduleGroupRefCount() const;

        /// <summary>
        ///     Causes the context to block, yielding the virtual processor to another context.
        /// </summary>
        virtual void Block() =0;

        /// <summary>
        ///     Unblocks the context and makes it runnable.
        /// </summary>
        virtual void Unblock() =0;

        /// <summary>
        ///     Determines whether or not the context is synchronously blocked at this given time.
        /// </summary>
        /// <returns>
        ///     Whether context is in synchronous block state.
        /// </returns>
        virtual bool IsSynchronouslyBlocked() const =0;

        /// <summary>
        ///     Returns whether the context is blocked or not.  Note that this definition of blocked is "blocked and requires
        ///     eventual reexecution -- e.g.: finalization will call this during the sweep phase).
        /// </summary>
        bool IsBlocked() const
        {
            return (m_blockedState != CONTEXT_NOT_BLOCKED);
        }

        /// <summary>
        ///     Yields execution to a different runnable context, and puts this context on a runnables collection.
        ///     If no context is found to yield to, the context continues to run.
        /// </summary>
        virtual void Yield() =0;

        /// <summary>
        ///     Yields execution to a different runnable context, and puts this context on a runnables collection.
        ///     If no context is found to yield to, the context continues to run.
        ///     
        ///     This is intended for spin loops.
        /// </summary>
        virtual void SpinYield() =0;

        /// <summary>
        ///     See comments for Concurrency::Context::Oversubscribe.
        /// </summary>
        virtual void Oversubscribe(bool beginOversubscription) =0;

        /// <summary>
        ///     Cleans up the Context.
        /// </summary>
        void Cleanup();

        /// <summary>
        ///     Allocates a block of memory of the size specified.
        /// </summary>
        /// <param name="numBytes">
        ///     Number of bytes to allocate.
        /// </param>
        /// <returns>
        ///     A pointer to newly allocated memory.
        /// </returns>
        virtual void* Alloc(size_t numBytes) =0;

        /// <summary>
        ///     Frees a block of memory previously allocated by the Alloc API.
        /// </summary>
        /// <param name="pAllocation">
        ///     A pointer to an allocation previously allocated by Alloc.
        /// </param>
        virtual void Free(void* pAllocation) =0;

        /// <summary>
        ///     Enters a critical region of the scheduler.  Calling this guarantees that the virtual processor on which this context lives
        ///     is guaranteed to be stable throughout the critical region.  For some context types, this is virtually a NOP.  For others 
        ///     (UMS), this makes it appear that blocking on the context actually blocks the UMS thread instead of triggering return to 
        ///     primary.  Note that critical regions suppress asynchronous blocking but not synchronous blocking.
        /// </summary>
        virtual int EnterCriticalRegionHelper() 
        {
            CORE_ASSERT(Context::CurrentContext() == this);
            return ++m_criticalRegionCount;
        }

        int EnterCriticalRegion();

        /// <summary>
        ///    Static version of EnterCriticalRegion. 
        /// </summary>
        static void StaticEnterCriticalRegion();

        /// <summary>
        ///     Enters a hyper-critical region of the scheduler.  Calling this guarantees not only the conditions of a critical region but it
        ///     guarantees that synchronous blocking is suppressed as well.  This allows for lock sharing between the primary and hyper-critical
        ///     regions running on UTs.  No lock sharing can occur between the inside of this region type and the outside of this region type
        ///     on a UT.
        /// </summary>
        virtual int EnterHyperCriticalRegionHelper()
        {
            m_criticalRegionCount++;
            return ++m_hyperCriticalRegionCount;
        }

        int EnterHyperCriticalRegion();

        /// <summary>
        ///    Static version of EnterHyperCriticalRegion.
        /// </summary>
        static void StaticEnterHyperCriticalRegion();

        /// <summary>
        ///     Exits a critical region of the scheduler.
        /// </summary>
        virtual int ExitCriticalRegionHelper() 
        {
            CORE_ASSERT(m_criticalRegionCount > 0);
            CORE_ASSERT(Context::CurrentContext() == this);
            return --m_criticalRegionCount;
        }

        int ExitCriticalRegion();

        /// <summary>
        ///    Static version of ExitCriticalRegion.
        /// </summary>
        static void StaticExitCriticalRegion();

        /// <summary>
        ///     Exits a hyper-critical region of the scheduler.
        /// </summary>
        virtual int ExitHyperCriticalRegionHelper()
        {
            CORE_ASSERT(m_hyperCriticalRegionCount > 0);
            CORE_ASSERT(m_criticalRegionCount > 0);
            m_criticalRegionCount--;
            return --m_hyperCriticalRegionCount;
        }

        int ExitHyperCriticalRegion();

        /// <summary>
        ///    Static version of ExitHyperCriticalRegion.
        /// </summary>
        static void StaticExitHyperCriticalRegion();

        /// <summary>
        ///     Checks if a context is in a critical region.  This is only safe from either the current context or from a UMS primary which
        ///     has woken due to a given context blocking.
        /// </summary>
        virtual CriticalRegionType GetCriticalRegionType() const
        {
            if (m_hyperCriticalRegionCount > 0)
                return InsideHyperCriticalRegion;
            if (m_criticalRegionCount > 0)
                return InsideCriticalRegion;
            return OutsideCriticalRegion;
        }

        /// <summary>
        ///     Since critical region counts are turned off for thread schedulers, this method is used
        ///     where the return value is expected to be true.  For a thread scheduler, it always returns true.
        ///     For a ums scheduler it returns (GetCriticalRegionType() != OutsideCriticalRegion).
        ///     IsInsideContextLevelCriticalRegion only checks (ContextBase::GetCriticalRegionType() != OutsideCriticalRegion).
        /// </summary>
        bool IsInsideCriticalRegion() const;

        /// <summary>
        ///     Static version of GetCriticalRegionType.
        /// </summary>
        static CriticalRegionType StaticGetCriticalRegionType();

        /// <summary>
        ///     Set critical region counts to zero
        /// </summary>
        void ClearCriticalRegion()
        {
            m_hyperCriticalRegionCount = m_criticalRegionCount = 0;
        }

#if defined(_DEBUG)
        /// <summary>
        ///     Tells the context it's shutting down a virtual processor and normal lock validations don't apply.
        /// </summary>
        void SetShutdownValidations()
        {
            m_fShutdownValidations = true;
        }

        /// <summary>
        ///     Re-enable normal lock validations
        /// </summary>
        void ClearShutdownValidations()
        {
            m_fShutdownValidations = false;
        }

        /// <summary>
        ///     Returns whether or not the context is in a "shutting down a virtual processor" mode where normal lock validations don't apply.
        /// </summary>
        bool IsShutdownValidations() const
        {
            return m_fShutdownValidations;
        }
#endif // _DEBUG

        /// <summary>
        ///     Wrapper for m_pWorkQueue for use in unstructured task collections 
        ///     that performs delay construction as well as insertion into schedule group.
        /// </summary>
        WorkQueue *GetWorkQueue()
        {
            // want inlining 
            if (m_pWorkQueue == NULL)
                CreateWorkQueue();
            return m_pWorkQueue;
        }
        
        /// <summary>
        ///     Wrapper for m_pWorkQueue for use in structured task collections 
        ///     that performs delay construction as well as insertion into schedule group.
        /// </summary>
        WorkQueue *GetStructuredWorkQueue()
        {
            // want inlining 
            if (m_pWorkQueue == NULL)
                CreateStructuredWorkQueue();
            return m_pWorkQueue;
        }

        /// <summary>
        ///     Create a workqueue for use in unstructured task collections.
        /// </summary>
        void CreateWorkQueue();

        /// <summary>
        ///     Create a workqueue for use in structured task collections.
        /// </summary>
        void CreateStructuredWorkQueue();

        /// <summary>
        ///     Returns a unique identifier for the work queue associated with this context.  Note that this should only be used
        ///     for binding (e.g.: task collection binding)
        /// </summary>
        unsigned int GetWorkQueueIdentity()
        {
            return GetWorkQueue()->Id();
        }

        /// <summary>
        ///     Pushes an unrealized chore onto the work stealing queue for structured parallelism.
        /// </summary>
        /// <param name="pChore">
        ///     The chore to push onto the structured work stealing queue.
        /// </param>
        void PushStructured(_UnrealizedChore *pChore);

        /// <summary>
        ///     Pushes an unrealized chore onto the work stealing queue for unstructured parallelism.
        /// </summary>
        /// <param name="pChore">
        ///     The chore to push onto the unstructured work stealing queue.
        /// </param>
        int PushUnstructured(_UnrealizedChore *pChore);

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
        void SweepUnstructured(WorkStealingQueue<_UnrealizedChore>::SweepPredicate pPredicate,
                               void *pData,
                               WorkStealingQueue<_UnrealizedChore>::SweepFunction pSweepFn
                               );

        /// <summary>
        ///     Pops the topmost chore from the work stealing queue for structured parallelism.  Failure
        ///     to pop typically indicates stealing.
        /// </summary>
        /// <returns>
        ///     An unrealized chore from the structured work stealing queue or NULL if none is present.
        /// </returns>
        _UnrealizedChore *PopStructured();

        /// <summary>
        ///     Attempts to pop the chore specified by a cookie value from the unstructured work stealing queue.  Failure
        ///     to pop typically indicates stealing.
        /// </summary>
        /// <param name="cookie">
        ///     A cookie returned from PushUnstructured indicating the chore to attempt to pop from
        ///     the unstructured work stealing queue.
        /// </param>
        /// <returns>
        ///     The specified unrealized chore (as indicated by cookie) or NULL if it could not be popped from
        ///     the work stealing queue.
        /// </returns>
        _UnrealizedChore *TryPopUnstructured(int cookie);

        /// <summary>
        ///     Returns the scheduler the specified context is associated with.
        /// </summary>
        SchedulerBase *GetScheduler() const;

        /// <summary>
        ///     Returns the schedule group the specified context is associated with.
        /// </summary>
        ScheduleGroupBase *GetScheduleGroup() const;

        /// <summary>
        ///     Tells whether the context is an external context
        /// <summary>
        bool IsExternal() const { return m_fIsExternal; }

        /// <summary>
        ///     Gets the indirect alias.
        /// </summary>
        _TaskCollection *GetIndirectAlias() const;

        /// <summary>
        ///     Sets the indirect alias.
        /// </summary>
        void SetIndirectAlias(_TaskCollection *pAlias);

        /// <summary>
        ///     Returns whether a task collection executing on this context was canceled.
        /// </summary>
        bool IsCanceled() const
        {
            return (m_canceledCount > 0);
        }

        /// <summary>
        ///     Returns whether the entire context was canceled due to a steal.
        /// </summary>
        bool IsCanceledContext() const
        {
            return (m_canceledContext != 0);
        }

        /// <summary>
        ///     Called in order to indicate that a cancellation is happening for a structured task collection associated with this thread.
        /// </summary>
        void PendingCancel()
        {
            InterlockedIncrement(&m_pendingCancellations);
        }

        /// <summary>
        ///     Called when a pending cancel completes.
        /// </summary>
        void PendingCancelComplete()
        {
            ASSERT(m_pendingCancellations > 0);
            InterlockedDecrement(&m_pendingCancellations);
        }

        /// <summary>
        ///     Returns whether the context has a pending cancellation.
        /// </summary>
        bool IsPendingCancellation() const
        {
            return (m_pendingCancellations > 0);
        }

        /// <summary>
        ///     Returns whether there is any cancellation on the context (pending or committed)
        /// </summary>
        bool HasAnyCancellation() const
        {
            return (m_pendingCancellations + m_canceledCount > 0);
        }

        /// <summary>
        ///     Called in order to indicate that a collection executing on this context was canceled.  This will often cause cancellation
        ///     and unwinding of the entire context (up to the point where we get to the canceled collection.
        /// </summary>
        void CancelCollection(int inliningDepth);

        /// <summary>
        ///     Called in order to indicate that we're blowing away the entire context.  It's stolen from a collection which was canceled.
        /// </summary>
        void CancelEntireContext()
        {
            InterlockedExchange(&m_canceledContext, TRUE);
            CancelCollection(0);
        }

        /// <summary>
        ///     When a cancellation bubbles up to the collection being canceled, this function is called in order to stop propagation of
        ///     the cancellation further up the work tree.
        /// </summary>
        bool CollectionCancelComplete(int inliningDepth);

        /// <summary>
        ///     Completely clears the cancel count.  This should be called when a root stolen chore completes on a context.
        /// </summary>
        void ClearCancel()
        {
            m_minCancellationDepth = -1;
            m_canceledCount = 0;
            m_canceledContext = 0;
        }

        /// <summary>
        ///     Returns the task collection executing atop a stolen context.
        /// </summary>
        _TaskCollectionBase *GetRootCollection()
        {
            return m_pRootCollection;
        }

        /// <summary>
        ///     Sets the task collection executing atop a stolen context.  Note that this also sets the executing collection since the root
        ///     collection is executing at the time it is set.
        /// </summary>
        void SetRootCollection(_TaskCollectionBase *pRootCollection)
        {
            m_pRootCollection = pRootCollection;
            m_pExecutingCollection = pRootCollection;
        }

        /// <summary>
        ///     Gets the task collection currently executing atop the context.
        /// </summary>
        _TaskCollectionBase *GetExecutingCollection()
        {
            return m_pExecutingCollection;
        }

        /// <summary>
        ///     Sets the task collection currently executing atop the context.
        /// </summary>
        void SetExecutingCollection(_TaskCollectionBase *pExecutingCollection)
        {
            m_pExecutingCollection = pExecutingCollection;
        }

        /// <summary>
        ///     Places a reference on the context preventing it from being destroyed until such time as the stealer is added to the chain
        ///     via AddStealer.  Note that the operation of AddStealer should happen rapidly as it will *BLOCK* cleanup of the context.
        /// </summary>
        void ReferenceForCancellation();

        /// <summary>
        ///     Removes a reference on the context which was preventing it from being destroyed.
        /// </summary>
        void DereferenceForCancellation();

        /// <summary>
        ///     Adds a stealing context.  Removes a reference.
        /// </summary>
        void AddStealer(ContextBase *pStealer, bool fDereferenceForCancellation);

        /// <summary>
        ///     Removes a stealing context.
        /// </summary>
        void RemoveStealer(ContextBase *pStealer);

        /// <summary>
        ///     Called by a stolen chore to flag the context as running a chore for which the steal is chained to a task collection instead
        ///     of the context.
        /// </summary>
        void NotifyTaskCollectionChainedStealer()
        {
            m_fContextChainedStealer = false;
        }

        /// <summary>
        ///     Returns whether the given context's steal is chained to the context (true) or some task collection (false)
        /// </summary>
        bool IsContextChainedStealer() const
        {
            return m_fContextChainedStealer;
        }

        /// <summary>
        ///     Called on both internal and external contexts, either when the are put into an idle pool to
        ///     be recycled, or when they are ready to be deleted. The API moves the contexts that are in
        ///     the list of 'stealers' (used for cancellation) to lists in the task collections from which
        ///     those contexts have stolen chores.
        /// </summary>
        void DetachStealers();

        /// <summary>
        ///     Gets an arbitrary alias out of the context's alias table.
        /// </summary>
        _TaskCollection *GetArbitraryAlias(_TaskCollection *pCollection)
        {
            Hash<_TaskCollection*, _TaskCollection*>::ListNode *pNode = m_aliasTable.Find(pCollection, NULL);
            _TaskCollection *pAlias = (pNode != NULL ? pNode->m_value : NULL);
            if (pAlias != NULL && pAlias->_IsStaleAlias())
            {
                m_aliasTable.Delete(pAlias->_OriginalCollection());
                delete pAlias;
                pAlias = NULL;
            }
            return pAlias;
        }

        /// <summary>
        ///     Adds an arbitrary alias (direct or indirect) to the alias table.
        /// </summary>
        void AddArbitraryAlias(_TaskCollection *pOriginCollection, _TaskCollection *pAliasCollection)
        {
            SweepAliasTable();
            m_aliasTable.Insert(pOriginCollection, pAliasCollection);
        }

        /// <summary>
        ///     Sweeps the alias table for stale entries.  Anything considered stale is deleted.
        /// </summary>
        void SweepAliasTable();

        /// <summary>
        ///     Clears the alias table.
        /// </summary>
        void ClearAliasTable();

        /// <summary>
        ///     Cancel everything stolen from pCollection outward from this context.
        /// </summary>
        void CancelStealers(_TaskCollectionBase *pCollection);

        /// <summary>
        ///     Returns the highest inlining depth (tree wise) of a canceled task collection.  Note that it will return -1
        ///     if there is no in-progress cancellation on the context.
        /// </summary>
        int MinimumCancellationDepth() const
        {
            //
            // If the entire context is canceled, the minimum depth is reported to be zero so as to be less than all inlining depths
            // for the purposes of checking cancellation.  Note that even if the top collection has inlining depth of zero, it does not matter
            // since it **IS** the top collection.
            //
            return IsCanceledContext() ? 0 : m_minCancellationDepth;
        }

        // An enumerated type that tells the type of the underlying execution context.
        enum ContextKind
        {
            ExternalContext,
            ThreadContext,
            UMSThreadContext,
            UMSThreadSchedulingUT
        };

        /// <summary>
        ///     Returns the type of context
        /// </summary>
        virtual ContextKind GetContextKind() const = 0;

#if _DEBUG
        // _DEBUG helper
        virtual DWORD GetThreadId() const = 0;
#endif
    
    protected:
        class ScopedCriticalRegion
        {
        public:
            ScopedCriticalRegion(ContextBase* pCB) : m_pCB(pCB)
            {
                m_pCB->EnterCriticalRegion();
            }
            
            ~ScopedCriticalRegion()
            {
                m_pCB->ExitCriticalRegion();
            }

        private:
            const ScopedCriticalRegion& operator=(const ScopedCriticalRegion&); //no assigment operator
            ContextBase* m_pCB;
        };

        // 
        // Protected data members
        //

        // Entry for freelist
        SLIST_ENTRY m_slNext; 

        // Unique identifier
        unsigned int m_id;

        // Critical region counter.
        DWORD m_criticalRegionCount; 

        // Hyper-critical region counter.
        DWORD m_hyperCriticalRegionCount;

        // Oversubscription count - the number of outstanding Oversubscribe(true) calls on this context.
        DWORD m_oversubscribeCount; 

        // The schedule group the context is associated with.
        ScheduleGroupBase *m_pGroup; 

        // The scheduler instance the context belongs to.
        SchedulerBase *m_pScheduler; 

        // Workqueue for unrealized chores.
        WorkQueue *m_pWorkQueue; 

        // Link to implement the stack of parent contexts for nested schedulers.
        ContextBase *m_pParentContext; 

        // Flag indicating whether the context is blocked.
        volatile LONG m_blockedState;

        // Memory fence to assist Block/Unblock.
        volatile LONG m_contextSwitchingFence; 

        // Tracks the task collection from which this context stole (if it's a context executing a stolen chore).
        _TaskCollectionBase *m_pRootCollection; 

        // Tracks the task collection currently executing (used to maintain parent/child relationships).
        _TaskCollectionBase *m_pExecutingCollection;  

        // The thread id for the thread backing the context.
        DWORD m_threadId;

        //
        // Protected methods
        //

        /// <summary>
        ///     Clean up the work queue for this Context.
        /// </summary>
        void ReleaseWorkQueue();

        /// <summary>
        ///     Sets the 'this' context into the tls slot as the current context. This is used by internal contexts in
        ///     their dispatch loops.
        /// </summary>
        void SetAsCurrentTls();

        ///<summary>Send a context ETW event</summary>
        void TraceContextEvent(ConcRT_EventType eventType, UCHAR level, DWORD schedulerId, DWORD contextId)
        {
            if (g_TraceOn && level <= g_EnableLevel)
                ThrowContextEvent(eventType, level, schedulerId, contextId);
        }

        static void ThrowContextEvent(ConcRT_EventType eventType, UCHAR level, DWORD schedulerId, DWORD contextId);

    private:

        //
        // Friend declarations
        //
        friend class SchedulerBase;
        friend class ThreadScheduler;
        friend class UMSThreadScheduler;
        friend class InternalContextBase;
        friend class SchedulingRing;
        friend class VirtualProcessor;
        friend class ScheduleGroupBase;
        friend class UMSThreadVirtualProcessor;
        friend class ScheduleGroup;
        friend class FairScheduleGroup;
        friend class CacheLocalScheduleGroup;
        friend class _UnrealizedChore;
        friend class _TaskCollection;
        friend class _StructuredTaskCollection;
        friend class UMSSchedulingContext;
        template <class T> friend class LockFreeStack;

        //
        // Private data
        //

        // Used in finalization to distinguish between blocked and free-list contexts
        LONG m_sweeperMarker;

        // Flag indicating context kind.
        bool m_fIsExternal;

        // Keeps track as to whether this context is chained to a context (true) or a schedule group (false) for the purposes of stealing/cancellation.
        bool m_fContextChainedStealer;

        // Indicates that normal lock validations should not be performed -- the context is shutting down a virtual processor.
        bool m_fShutdownValidations;

        // Tracks all contexts which stole from any collection on *this* context.
        SafeRWList<ListEntry> m_stealers;
        // Link for contexts added to m_stealers
        ListEntry m_stealChain;

        // Reference count of things waiting to be added to the steal chain of this context.
        volatile LONG m_cancellationRefCount;
        // The inlining depth of a canceled task collection.
        volatile LONG m_minCancellationDepth; 
        // The number of task collections running on this context which have been canceled.
        volatile LONG m_canceledCount;
        // An indication that the context was shot down as it stole from a canceled collection.
        volatile LONG m_canceledContext; 
        // An indication that there is a pending cancellation of a structured collection on this thread.
        volatile LONG m_pendingCancellations;
        // The indirect alias for this context. This allows an unstructured task collection to carry into a stolen chore and be
        // utilized there without any cross threaded semantics within the task collection.
        _TaskCollection *m_pIndirectAlias; 
        // The table of aliases for this context.  This allows transitive indirect aliases as well as direct aliases (which
        // are not presently implemented).
        Hash<_TaskCollection*, _TaskCollection*> m_aliasTable; 
        
        //
        // Private member functions
        //

        /// <summary>
        ///     When schedulers are nested on the same stack context, the nested scheduler creates a new external context that overrides 
        ///     the previous context. PopContextFromTls will restore the previous context by setting the TLS value appropriately.
        /// </summary>
        ContextBase* PopContextFromTls();

        /// <summary>
        ///     When schedulers are nested on the same stack context, the nested scheduler creates a new external context that overrides 
        ///     the previous context. PushContextToTls will remember the parent context and set the new context into TLS.
        /// </summary>
        void PushContextToTls(ContextBase* pParentContext);

        /// <summary>
        ///     Context TLS is cleared during nesting on internal contexts before the external context TLS is correctly setup. If not,
        ///     code that executes between the clear and setting the new TLS could get confused.
        /// </summary>
        void ClearContextTls();
    };
} // namespace details
} // namespace Concurrency
