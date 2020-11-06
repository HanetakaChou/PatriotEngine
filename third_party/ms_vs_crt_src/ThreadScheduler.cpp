// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// ThreadScheduler.h
//
// Source file containing the implementation for a thread based concrt scheduler 
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#pragma once

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Creates a thread based scheduler
    /// </summary>
    ThreadScheduler::ThreadScheduler(__in const ::Concurrency::SchedulerPolicy& policy) :
        SchedulerBase(policy)
    {
    }

    /// <summary>
    ///     Creates a thread based scheduler
    /// </summary>
    ThreadScheduler* ThreadScheduler::Create(__in const ::Concurrency::SchedulerPolicy& policy)
    {
        return new ThreadScheduler(policy);
    }

    /// <summary>
    ///     Creates a thread based virtual processor.
    /// </summary>
    VirtualProcessor* ThreadScheduler::CreateVirtualProcessor(SchedulingNode *pOwningNode, IVirtualProcessorRoot* pOwningRoot)
    {
        return new ThreadVirtualProcessor(pOwningNode, pOwningRoot);
    }

    ///<summary>
    ///     Returns a newly created thread internal context to the base scheduler.
    ///</summary>
    InternalContextBase *ThreadScheduler::CreateInternalContext()
    {
        return new ThreadInternalContext(this);
    }

    /// <summary>
    ///     Destroys a thread based scheduler
    /// </summary>
    ThreadScheduler::~ThreadScheduler()
    {
    }

    /// <summary>
    ///     Performs the throttling of an oversubscribed virtual processor.
    /// </summary>
    void ThreadScheduler::ThrottleOversubscriber(VirtualProcessor *pVProc)
    {
        SafeRWList<VirtualProcessor>::_Scoped_lock lockHolder(m_throttledOversubscribers);
        if (!pVProc->IsThrottled())
        {
            pVProc->ThrottleActivation(true);
            m_throttledOversubscribers.UnlockedAddTail(pVProc);
            if (InterlockedIncrement(&m_throttledOversubscriberCount) == 1)
                SetEvent(m_hOversubscribeManagerSignal);
        }
    }

    /// <summary>
    ///     Removes a virtual processor from the throttling list.  After this call returns, the virtual processor is guaranteed
    ///     not to be activated by the throttler thread.
    /// </summary>
    void ThreadScheduler::RemoveThrottleOversubscriber(VirtualProcessor *pVProc)
    {
        SafeRWList<VirtualProcessor>::_Scoped_lock lockHolder(m_throttledOversubscribers);
        if (pVProc->IsThrottled())
        {
            pVProc->ThrottleActivation(false);
            m_throttledOversubscribers.UnlockedRemove(pVProc);
        }
    }

    /// <summary>
    ///     Manages oversubscribers.
    /// </summary>
    void ThreadScheduler::ManageOversubscribers()
    {
        ULONG lastReplenishTime = 0;
        LONG val = 0;
    
        for (;;)
        {
            //
            // In order to prevent an explosion of threads in the case where we perform an operation like:
            //
            // queue_repeated_lwt:
            //
            // {
            //     Context::Oversubscribe(true);
            //     latent_blocking_operation();
            //     Context::Oversubscribe(false);
            // }
            //
            // We will throttle the creation of threads here.  Note that this is only a mitigation and eventually the throttling
            // should become pervasive throughout the scheduler.  No matter the level of throttling here, if we hit a system cap on the number
            // of threads, this will still throw.
            //
            ULONG delay = ThrottlingTime(1);
            ULONG curTime = GetTickCount();
            ULONG delta = curTime - lastReplenishTime; // okay with wrap
            delay = (delta < delay) ? delay - delta : 0;

            DWORD result = WaitForSingleObject(m_hOversubscribeManagerSignal, val == 0 ? INFINITE : delay);

            if (m_fCancelOversubscribeManager)
            {
                ASSERT(m_throttledOversubscriberCount == 0);
                break;
            }

            if (result == WAIT_TIMEOUT)
            {
                lastReplenishTime = curTime;

                VirtualProcessor *pVProc = NULL;

                {
                    //
                    // Guarantee that a return from RemoveThrottledOversubscriber() indicates we will never activate here until the given vproc
                    // is put back on the list.  The lock on the list governs this.
                    //
                    SafeRWList<VirtualProcessor>::_Scoped_lock lockHolder(m_throttledOversubscribers);

                    pVProc = m_throttledOversubscribers.UnlockedRemoveHead();
                    if (pVProc != NULL)
                    {
                        //
                        // Retirement should have claimed **AND** removed us from the throttling list.  Oversubscribe(false) will retire.  We should never get here
                        // for a recycled virtual processor.
                        //
                        ASSERT(pVProc->IsThrottled());
                        pVProc->ThrottleActivation(false);

                        if (pVProc->ClaimExclusiveOwnership())
                        {
                            pVProc->Hide(false);
                            ActivateVirtualProcessor(pVProc, GetAnonymousScheduleGroup());
                        }
                    }
                }

                val = InterlockedDecrement(&m_throttledOversubscriberCount);

                //
                // Outstanding requests block finalization.  If there was an outstanding request and we didn't activate the virtual processor, we
                // need to attempt to finalize.  Otherwise, we risk never finalizing the scheduler in rare races.
                //
                if (val == 0)
                {
                    AttemptSchedulerSweep();
                }

            }
            else
                val = 1;

            ASSERT(val >= 0);
        }

        CloseHandle(m_hOversubscribeManagerSignal);

        // NOTE: Decrementing the internal context context count could finalize the scheduler - it is not safe to touch
        // *this* after this point.
        DecrementInternalContextCount();
    }

    /// <summary>
    ///     Entry point for background oversubscribe manager thread
    /// </summary>
    DWORD CALLBACK ThreadScheduler::BackgroundOversubscribeManagerProc(LPVOID lpParameter)
    {
        ThreadScheduler * pScheduler = reinterpret_cast<ThreadScheduler*>(lpParameter);
        pScheduler->ManageOversubscribers();
        FreeLibraryAndDestroyThread(0);
        return 0;
    }

    ///<summary>
    ///     Determines if there is pending work such as blocked context/unstarted chores etc in the
    ///     scheduler. If there is no pending work, the scheduler will attempt to shutdown. 
    ///     For thread scheduler, look for pending requests from vprocs for throttled activation.
    ///</summary>
    bool ThreadScheduler::HasWorkPending()
    {
        return (m_throttledOversubscriberCount > 0) || SchedulerBase::HasWorkPending();
    }

    ///<summary>
    ///     Initialize scheduler event handlers/background threads.  The thread scheduler
    ///     manages throttling of oversubscribed virtual processors through a background
    ///     thread.
    ///</summary>
    void ThreadScheduler::InitializeSchedulerEventHandlers()
    {
        //
        // We need to create a background thread to manage awakening of oversubscribed virtual processors.
        //

        m_throttledOversubscriberCount = 0;
        m_fCancelOversubscribeManager = false;

        // Auto reset event.
        m_hOversubscribeManagerSignal = CreateEventW(NULL, FALSE, FALSE, NULL);

        // Event handlers take an internal reference on the scheduler which is released when they exit.
        IncrementInternalContextCount();

        HANDLE threadHandle = LoadLibraryAndCreateThread(NULL,
                                           DEFAULTCONTEXTSTACKSIZE,
                                           BackgroundOversubscribeManagerProc,
                                           this,
                                           0,
                                           NULL);

        CloseHandle(threadHandle);

        // Allow base class to register any handlers if required.
        SchedulerBase::InitializeSchedulerEventHandlers();
    }

    ///<summary>
    ///     Destroy scheduler event handlers/background threads.  The thread scheduler
    ///     manages throttling of oversubscribed virtual processors through a background
    ///     thread.
    ///</summary>
    void ThreadScheduler::DestroySchedulerEventHandlers()
    {
        //
        // Cancel the background oversubscribe manager.
        //
        m_fCancelOversubscribeManager = true;
        SetEvent(m_hOversubscribeManagerSignal);
 
        // Allow base class to destroy any handlers it has
        SchedulerBase::DestroySchedulerEventHandlers();
    }

} // namespace details
} // namespace Concurrency
