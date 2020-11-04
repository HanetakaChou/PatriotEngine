// ==++==
//
//	Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// UmsThreadScheduler.h
//
// Header file containing the metaphor for a UMS thread based concrt scheduler 
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#pragma once

namespace Concurrency
{
namespace details
{
    class UMSThreadScheduler : public SchedulerBase
    {
    public:

        /// <summary>
        ///     Creates a UMS thread based scheduler
        /// </summary>
        UMSThreadScheduler(__in const Concurrency::SchedulerPolicy& policy);

        /// <summary>
        ///     Creates a UMS thread based scheduler
        /// </summary>
        static UMSThreadScheduler* Create(__in const ::Concurrency::SchedulerPolicy& policy);

        // create the correct flavour of vproc
        virtual VirtualProcessor *CreateVirtualProcessor(SchedulingNode *pOwningNode, IVirtualProcessorRoot *pOwningRoot);

        /// <summary>
        ///     Destroys a UMS thread based scheduler
        /// </summary>
        virtual ~UMSThreadScheduler();

        /// <summary>
        ///     Start up an new virtual processor in the scheduler. New virtual processor refers
        ///     to any vproc that either has never been activated or has been deactivated due to lack
        ///     of work (wait for work).
        /// </summary>
        virtual void StartupNewVirtualProcessor(ScheduleGroupBase *pGroup);

        /// <summary>
        ///     Called in order to notify the scheduler of the particular completion list created in the RM for it.
        /// </summary>
        /// <param name="pCompletionList">
        ///     The completion list created in the RM for this scheduler.
        /// </param>
        void SetCompletionList(IUMSCompletionList *pCompletionList)
        {
            m_pCompletionList = pCompletionList;
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
        bool MoveCompletionListToRunnables(VirtualProcessor *pBias = NULL);

        /// <summary>
        ///     Static initialization common to UMS schedulers.
        /// </summary>
        static void OneShotStaticConstruction();

        /// <summary>
        ///     Static destruction common to UMS schedulers.
        /// </summary>
        static void OneShotStaticDestruction();

        /// <summary>
        ///     Returns the current scheduling context.
        /// </summary>
        static UMSSchedulingContext* FastCurrentSchedulingContext()
        {
            return reinterpret_cast<UMSSchedulingContext *>(TlsGetValue(UMSThreadScheduler::t_dwSchedulingContextIndex));
        }

    protected:

        ///<summary>
        ///     Creates a new thread internal context and returns it to the base scheduler.
        ///</summary>
        virtual InternalContextBase *CreateInternalContext();

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
        virtual void VirtualProcessorActiveNotification(bool fActive, LONG activeCount);

        ///<summary>
        ///     Determines if there is pending work such as blocked context/unstarted chores etc in the
        ///     scheduler. If there is no pending work, the scheduler will attempt to shutdown.
        ///</summary>
        virtual bool HasWorkPending();

        ///<summary>
        ///     Initialize scheduler event handlers/background threads. UMS scheduler creates
        ///     a background thread to create UTs
        ///</summary>
        virtual void InitializeSchedulerEventHandlers();

        ///<summary>
        ///     Destroy scheduler event handlers/background threads. UMS scheduler destroys
        ///     the UT creation background thread.
        ///</summary>
        virtual void DestroySchedulerEventHandlers();

        ///<summary>
        ///     Cancel all the inernal contexts. UMS Scheduler releases all the reserved
        ///     contexts into the idle pool.
        ///</summary>
        virtual void CancelAllContexts();

    private:

        friend class UMSSchedulingContext;

        // Keeps track of the current scheduling context.
        static DWORD t_dwSchedulingContextIndex;

        // The completion list.
        IUMSCompletionList *m_pCompletionList;

        // flag to cancel the background thread that creates UTs
        bool m_fCancelContextCreationHandler;

        // Event to signal the creation of UTs
        HANDLE m_hCreateContext;

        // Number of pending requests for reserved contexts
        volatile long m_pendingRequests;

        // Number of reserved contexts in the list
        volatile long m_numReservedContexts;

        // List of reserved contexts
        LockFreeStack<InternalContextBase> m_reservedContexts;

        // Private methods

        // Find a new vproc (idle vproc not waiting for resources)
        UMSThreadVirtualProcessor * FindNewVirtualProcessor();

        // Replenish the reserved context list
        void ReplenishReservedContexts();

        // Remove contexts from the reserved context list and return to idle pool
        void ReleaseReservedContexts();

        // Get a reserved context. This routine is allowed to return NULL
        InternalContextBase * GetReservedContext();

        // Wait loop for context creation
        void WaitForContextCreationRequest();

        // Entry point for context creation background thread
        static DWORD CALLBACK BackgroundContextCreationProc(LPVOID lpParameter);

        // Activate all virtual processors waiting on a reserved context
        bool ActivateAllReservedVirtualProcessors(SchedulingNode * pNode);

        // Wake up all threads waiting for reserved contexts
        void SignalReservedContextsAvailable();

        // Hide the assignment operator and copy constructor.
        UMSThreadScheduler const &operator =(UMSThreadScheduler const &);  // no assign op
        UMSThreadScheduler(UMSThreadScheduler const &);                    // no copy ctor
    };
} // namespace details
} // namespace Concurrency
