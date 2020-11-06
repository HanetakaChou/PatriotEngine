// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// UMSBackgroundPoller.h
//
// A background thread responsible for polling for contexts which had a failed ExecuteUmsThread and inject them on our
// abstraction of a completion list.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     An entry in the UMS background poller.  Anything wanting to be inserted must have an intrusive copy of this
    ///     data structure.
    /// </summary>
    struct UMSBackgroundPollerEntry
    {
        // The link within
        ListEntry m_link;

        // A timestamp at the start of polling.
        __int64 m_pollStart;

        // The number of times that the poller
        LONG m_pollCount;
    };

    /// <summary>
    ///     The object responsible for polling for completions of threads which when executed were suspended or otherwise occupied.
    ///     The UMS system will never return these to the completion list and threads such as these must be polled.  The RM polls
    ///     in a central location to isolate these types of issues from the scheduler.  This is that central location.
    /// </summary>
    class UMSBackgroundPoller
    {
    public:

        /// <summary>
        ///     Constructs a new UMS background poller.
        /// </summary>
        /// <param name="pollingPeriod">
        ///     How often (in milliseconds) the background thread should poll for awakenings.
        /// </param>
        UMSBackgroundPoller(int pollingPeriod);

        /// <summary>
        ///     Destructs a UMS background poller.
        /// </summary>
        ~UMSBackgroundPoller();

        /// <summary>
        ///     Causes the background poller thread to poll for waking of pThreadProxy and insert it into the completion
        ///     list of pSchedulerProxy.
        /// </summary>
        /// <param name="pThreadProxy">
        ///     The thread proxy to poll for awakening of.
        /// </param>
        /// <param name="pSchedulerProxy">
        ///     The scheduler proxy to insert pThreadProxy into the completion list of when awake.
        /// </param>
        void InsertProxyInCompletionWhenAwake(UMSThreadProxy *pThreadProxy, UMSSchedulerProxy *pSchedulerProxy);

        /// <summary>
        ///     Retires the UMS background poller.
        /// </summary>
        void Retire();

    private:

        /// <summary>
        ///     Wakes up and polls periodically the list of proxies that are required and inserts them into the appropriate transfer
        ///     lists if they are awake.
        /// </summary>
        void StartPolling();

        /// <summary>
        ///     The main thread function for the background poller.
        /// </summary>
        static DWORD CALLBACK BackgroundPollerMain(LPVOID lpParameter)
        {
            (reinterpret_cast<UMSBackgroundPoller *> (lpParameter))->StartPolling();
            FreeLibraryAndDestroyThread(0);
            return 0;
        }

        // Handle to the background poller thread.
        HANDLE m_hThread;

        // The TID of the background poller thread.
        DWORD m_tid;

        // Notification that there are pollers to poll.  The background poller stays asleep otherwise.
        HANDLE m_hNotification;

        // How often we poll (in mS)
        int m_pollingPeriod;

        // The count of pollers.
        volatile LONG m_pollCount;

        // Flag to indicate whether the poller should be retired
        volatile bool m_fCanceled;

        // The thread proxies to poll
        SafeRWList<ListEntry> m_pollProxies;
    };

} // namespace details
} // namespace Concurrency
