// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// threadscheduler.h
//
// Header file containing the metaphor for a thread based concrt scheduler 
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#pragma once

namespace Concurrency
{
namespace details
{
    class ThreadScheduler : public SchedulerBase
    {
    public:

        /// <summary>
        ///     Creates a thread based scheduler
        /// </summary>
        ThreadScheduler(__in const Concurrency::SchedulerPolicy& pPolicy);

        /// <summary>
        ///     Creates a thread based scheduler
        /// </summary>
        static ThreadScheduler* Create(__in const ::Concurrency::SchedulerPolicy& pPolicy);

        /// <summary>
        ///     Create the correct flavor of virtual processor
        /// </summary>
        virtual VirtualProcessor *CreateVirtualProcessor(SchedulingNode *pOwningNode, IVirtualProcessorRoot *pOwningRoot);

        /// <summary>
        ///     Destroys a thread based scheduler
        /// </summary>
        virtual ~ThreadScheduler();

        /// <summary>
        ///     Returns whether we should throttle oversubscribers.
        /// </summary>
        virtual bool ShouldThrottleOversubscriber() const
        {
            return true;
        }

        /// <summary>
        ///     Performs the throttling of an oversubscribed virtual processor.
        /// </summary>
        virtual void ThrottleOversubscriber(VirtualProcessor *pVProc);

        /// <summary>
        ///     Removes a virtual processor from the throttling list.  After this call returns, the virtual processor is guaranteed
        ///     not to be activated by the throttler thread.
        /// </summary>
        virtual void RemoveThrottleOversubscriber(VirtualProcessor *pVProc);

    protected:

        ///<summary>
        ///     Creates a new thread internal context and returns it to the base scheduler.
        ///</summary>
        virtual InternalContextBase *CreateInternalContext();

        ///<summary>
        ///     Determines if there is pending work such as blocked context/unstarted chores etc in the
        ///     scheduler. If there is no pending work, the scheduler will attempt to shutdown.
        ///</summary>
        virtual bool HasWorkPending();

        ///<summary>
        ///     Initialize scheduler event handlers/background threads.  The thread scheduler
        ///     manages throttling of oversubscribed virtual processors through a background
        ///     thread.
        ///</summary>
        virtual void InitializeSchedulerEventHandlers();

        ///<summary>
        ///     Destroy scheduler event handlers/background threads.  The thread scheduler
        ///     manages throttling of oversubscribed virtual processors through a background
        ///     thread.
        ///</summary>
        virtual void DestroySchedulerEventHandlers();

    private:

        // Hide the assignment operator and copy constructor.
        ThreadScheduler const &operator =(ThreadScheduler const &);  // no assign op
        ThreadScheduler(ThreadScheduler const &);                    // no copy ctor

        /// <summary>
        ///     Manages oversubscribers.
        /// </summary>
        void ManageOversubscribers();

        /// <summary>
        ///     Entry point for background oversubscribe manager thread
        /// </summary>
        static DWORD CALLBACK BackgroundOversubscribeManagerProc(LPVOID lpParameter);

        //
        // Throttling management for oversubscribed virtual processors:
        //
        bool m_fCancelOversubscribeManager;
        SafeRWList<VirtualProcessor> m_throttledOversubscribers;
        HANDLE m_hOversubscribeManagerSignal;
        volatile LONG m_throttledOversubscriberCount;

    };
} // namespace details
} // namespace Concurrency
