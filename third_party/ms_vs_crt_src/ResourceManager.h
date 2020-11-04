// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// ResourceManager.h
//
// Implementation of IResourceManager.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once 

namespace Concurrency
{
namespace details
{
    class ResourceManager : public Concurrency::IResourceManager
    {
    public:
        /// <summary>
        ///     Increments the reference count on a resource manager.
        /// </summary>
        /// <returns>
        ///     Returns the resulting reference count.
        /// </returns>
        virtual unsigned int Reference();

        /// <summary>
        ///     Decrements the reference count on a resource manager.
        /// </summary>
        /// <returns>
        ///     Returns the resulting reference count.
        /// </returns>
        virtual unsigned int Release();

        /// <summary>
        ///     Associates an IScheduler with the ISchedulerProxy that represents the part
        ///     of IResourceManager associated with the IScheduler.
        /// </summary>
        /// <param name="pScheduler">
        ///     The scheduler to be associated.
        /// </param>
        /// <param name="version">
        ///     The version of the RM<->Scheduler communication channel that is being utilized.
        /// </param>
        virtual ISchedulerProxy * RegisterScheduler(IScheduler *pScheduler, unsigned int version);

        /// <summary>
        ///     Creates an instance of the resource manager.
        /// </summary>
        static ResourceManager* CreateSingleton();

        /// <summary>
        ///     Returns the OS version.
        /// </summary>
        static IResourceManager::OSVersion Version();

        /// <summary>
        ///     Returns whether we must work around a Win7 RTM UMS bug.
        /// </summary>
        static bool RequireUMSWorkaround()
        {
            return s_fRequireUMSWorkaround;
        }

        /// <summary>
        ///     Returns the number of nodes (processor packages or NUMA nodes)
        /// </summary>
        static unsigned int GetNodeCount();

        /// <summary>
        ///     Returns the number of cores.
        /// </summary>
        static unsigned int GetCoreCount();

        /// <summary>
        ///     Returns a pointer to the manager of factories for thread proxies.
        /// </summary>
        ThreadProxyFactoryManager * GetThreadProxyFactoryManager() { return &m_threadProxyFactoryManager; }

        /// <summary>
        ///     These APIs return unique identifiers for use by schedulers, execution contexts and thread proxies.
        /// </summary>
        static unsigned int GetSchedulerId() { return InterlockedIncrement(&s_schedulerIdCount); }
        static unsigned int GetExecutionContextId() { return InterlockedIncrement(&s_executionContextIdCount); }
        static unsigned int GetThreadProxyId() { return InterlockedIncrement(&s_threadProxyIdCount); }

        /// <summary>
        ///     Creates UMS background threads.
        /// </summary>
        void CreateUMSBackgroundThreads();

        /// <summary>
        ///     Returns an instance of the UMS background poller thread.
        /// </summary>
        UMSBackgroundPoller *GetUMSBackgroundPoller() { return m_pUMSPoller; }

        /// <summary>
        ///     Returns an instance of transmogrificator.
        /// </summary>
        Transmogrificator *GetTransmogrificator() { return m_pTransmogrificator; }

        /// <summary>
        ///     The main allocation routine that allocates cores for a newly created scheduler proxy.
        /// </summary>
        ExecutionResource * PerformAllocation(SchedulerProxy *pSchedulerProxy, bool doExternalThreadAllocation);

        /// <summary>
        ///     This API registers the current thread with the resource manager, associating it with this scheduler proxy,
        ///     and returns an instance of IExecutionResource back to the scheduler, for bookkeeping and maintenance.
        /// </summary>
        ExecutionResource * SubscribeCurrentThread(SchedulerProxy *pSchedulerProxy);

        /// <summary>
        ///     The allocation routine that allocates a single core for a newly registered external thread.
        /// </summary>
        ExecutionResource * PerformExternalThreadAllocation(SchedulerProxy *pSchedulerProxy);

        /// <summary>
        ///     Removes an execution resource that was created for an external thread.
        /// </summary>
        void RemoveExecutionResource(ExecutionResource * pExecutionResource);

        /// <summary>
        ///     Called in order to notify the resource manager that the given scheduler is shutting down.  This
        ///     will cause the resource manager to immediately reclaim all resources granted to the scheduler.
        /// </summary>
        void Shutdown(SchedulerProxy *pProxy);

        /// <summary>
        ///     Called by a scheduler in order make an initial request for an allocation of virtual processors.  The request
        ///     is driven by policies within the scheduler queried via the IScheduler::GetPolicy method.  If the request
        ///     can be satisfied via the rules of allocation, it is communicated to the scheduler as a call to
        ///     IScheduler::AddVirtualProcessors.
        /// </summary>
        /// <param name="pProxy">
        ///     The scheduler proxy that is making the allocation request.
        /// </param>
        /// <param name="doSubscribeCurrentThread">
        ///     Whether to subscribe the current thread and account for it during resource allocation.
        /// </param>
        /// <returns>
        ///     The IExecutionResource instance representing current thread if doSubscribeCurrentThread was true; NULL otherwise.
        /// </returns>
        IExecutionResource * RequestInitialVirtualProcessors(SchedulerProxy *pProxy, bool doSubscribeCurrentThread);

        /// <summary>
        ///     Debug CRT test hook to create artificial topologies. With the retail CRT, this API simply returns.
        /// </summary>
        virtual void CreateNodeTopology(unsigned int nodeCount, unsigned int *pCoreCount, unsigned int **pNodeDistance, unsigned int *pProcessorGroups);

        /// <summary>
        ///     The API returns after ensuring that all store buffers on processors that are running threads from this process,
        ///     are flushed. It does this by either calling a Win32 API that explictly does this on versions of Windows that
        ///     support the functionality, or by changing the protection on a page that is locked into the working set, forcing
        ///     a TB flush on all processors in question.
        /// </summary>
        void FlushStoreBuffers();

        /// <summary>
        ///     Returns the TLS slot where information about an execution resource is supposed to be stored.
        /// </summary>
        /// <remarks>
        ///     Used to check whether SubscribeCurrentThread already has an execution resource on which it is running.
        /// </remarks>
        DWORD GetExecutionResourceTls() const
        {
            return m_threadProxyFactoryManager.GetExecutionResourceTls();
        }

        /// <summary>
        ///     Decrements the use count on a particular global core. Used for removal of external threads.
        /// </summary>
        void DecrementCoreUseCount(unsigned int nodeId, unsigned int coreIndex)
        {
            // NOTE: Caller is responsible for holding the RM lock!
            GlobalCore * pGlobalCore = &(m_pGlobalNodes[nodeId].m_pCores[coreIndex]);
            pGlobalCore->m_useCount--;
        }

        /// <summary>
        ///     Returns the current thread's node id and core id (relative to that node).
        /// </summary>
        unsigned int GetCurrentNodeAndCore(unsigned int * pCore);

        /// <summary>
        ///     Returns the global subscription level of the underlying core.
        /// </summary>
        unsigned int CurrentSubscriptionLevel(unsigned int nodeId, unsigned int coreIndex);

        /// <summary>
        ///     Returns true if there are any schedulers in the RM that need notifications sent, false, otherwise.
        /// </summary>
        bool SchedulersNeedNotifications()
        {
            return (m_numSchedulersNeedingNotifications > 0);
        }

        /// <summary>
        ///     Returns the number of schedulers that need core busy/idle notifications.
        /// </summary>
        unsigned int GetNumSchedulersForNotifications()
        {
            return m_numSchedulersNeedingNotifications;
        }

        /// <summary>
        ///     Wakes up the dynamic RM worker from a wait state.
        /// </summary>
        void WakeupDynamicRMWorker()
        {
            SetEvent(m_hDynamicRMEvent);
        }

        /// <summary>
        ///     Sends NotifyResourcesExternallyIdle/NotifyResourcesExternallyBusy notifications to the schedulers that
        ///     qualify for them, to let them know that the hardware resources allocated to them are in use or out of use
        ///     by other schedulers that share those resources.
        /// </summary>
        /// <param name ="pNewlyAllocatedProxy">
        ///     The newly allocated scheduler proxy, if one was just allocated.
        /// </param>
        void SendResourceNotifications(SchedulerProxy * pNewlyAllocatedProxy = NULL);

        /// <summary>
        ///     Inform the resource manager that termination is happening.
        /// </summary>
        static void SetTerminating()
        {
            InterlockedExchange(&s_terminating, 1);
        }

        /// <summary>
        ///     Queries the resource manager to determine if termination is happening.
        /// </summary>
        static bool IsTerminating()
        {
            return (s_terminating != 0);
        }

    private:

        // State that controls what the dynamic RM worker thread does.
        enum DynamicRMWorkerState
        {
            Standby,
            LoadBalance,
            Exit
        };

#if defined(CONCRT_TRACING)

        struct GlobalCoreData
        {
            unsigned char m_nodeIndex;
            unsigned char m_coreIndex;
            unsigned char m_useCount;
            unsigned char m_idleSchedulers;
        };

        // Maintains a trace for every core removed during preprocessing.
        struct PreProcessingTraceData
        {
            SchedulerProxy * m_pProxy;
            unsigned char m_nodeIndex;
            unsigned char m_coreIndex;
            bool m_fMarkedAsOwned : 1;
            bool m_fBorrowedCoreRemoved : 1;
            bool m_fSharedCoreRemoved : 1;
            bool m_fIdleCore : 1;
        };

        // Maintains a trace for each core allocation
        struct DynamicAllocationTraceData
        {
            SchedulerProxy * m_pGiver; // this is null if the core was unused or idle
            SchedulerProxy * m_pReceiver;
            unsigned char m_round;
            unsigned char m_nodeIndex;
            unsigned char m_coreIndex;
            bool m_fUnusedCoreMigration : 1;
            bool m_fIdleCoreSharing : 1;
            bool m_fBorrowedByGiver : 1;
            bool m_fIdleOnGiver : 1;
        };

#endif

        //
        // Private data
        //

        // The singleton resource manager instance.
        static ResourceManager *s_pResourceManager;

        // A lock that protects setting of the singleton instance.
        static _StaticLock s_lock;

        // The total number of hardware threads on the system.
        static unsigned int s_physicalProcessorCount;

        // The number of processor packages on the system.
        static unsigned int s_packageCount;

        // The number of NUMA nodes on the system.
        static unsigned int s_nodeCount;

        // Operating system characteristics.
        static bool s_fNativeX64;
        static bool s_fRequireUMSWorkaround;
        static IResourceManager::OSVersion s_version;

        // Termination indicator
        static volatile LONG s_terminating;

        // Static counters to generate unique identifiers.
        static volatile LONG s_schedulerIdCount;
        static volatile LONG s_executionContextIdCount;
        static volatile LONG s_threadProxyIdCount;

        // Variables used to obtain information about the topology of the system.
        static DWORD s_logicalProcessorInformationLength;
        static PSYSTEM_LOGICAL_PROCESSOR_INFORMATION s_pSysInfo;

        // Constants that are used as parameters to the ReleaseSchedulerResources API
        static const unsigned int ReleaseCoresUptoMin = -1;
        static const unsigned int ReleaseOnlyBorrowedCores = -2;

        // RM instance reference count
        volatile LONG m_referenceCount;

        // Number of schedulers currently using resources granted by the RM.
        unsigned int m_numSchedulers;

        // The maximum number of schedulers the dynamic RM worker has resources to handle, at any time. This can
        // change during the lifetime of the process.
        unsigned int m_maxSchedulers;

        // Number of schedulers in the RM that need resource notifications.
        unsigned int m_numSchedulersNeedingNotifications;

        // The number of processor packages on the system. This is a mirror of s_nodeCount above.
        unsigned int m_nodeCount;

        // A field used during core migration to keep track of the number of idle cores - cores such that 
        // all schedulers they are assigned to are not using them.
        unsigned int m_dynamicIdleCoresAvailable;

        // Keeps track of the allocation round during dynamic core migration. Used for tracing.
        unsigned int m_allocationRound;

        // A state variable that is used to control how the dynamic worker behaves.
        volatile DynamicRMWorkerState m_dynamicRMWorkerState;

        // A lock that protects resource allocation and deallocation.
        _NonReentrantBlockingLock m_lock;

        // The global allocation map for all schedulers - array of processor nodes.
        GlobalNode * m_pGlobalNodes;

        // An array that is used to represent sorted nodes during allocation.
        unsigned int* m_pSortedNodeOrder;

        // Handle to the dynamic RM worker thread.
        HANDLE m_hDynamicRMThreadHandle;

        // An event that is used to control the dynamic RM worker thread.
        HANDLE m_hDynamicRMEvent;

        // Data used for static and/or dynamic allocation.
        AllocationData ** m_ppProxyData;
        DynamicAllocationData ** m_ppGivingProxies;
        DynamicAllocationData ** m_ppReceivingProxies;

        // Lists to hold scheduler proxies.
        List<SchedulerProxy, CollectionTypes::Count> m_schedulers;

        // The UMS poller -- right now the thread which polls periodically for UMS completions of "odd threads" which don't show up on the completion
        // list.
        UMSBackgroundPoller *m_pUMSPoller;

        // The transmogrificator.  The object responsible for the transmogrification of UTs.
        Transmogrificator *m_pTransmogrificator;

        // A collection of thread proxy factories
        ThreadProxyFactoryManager m_threadProxyFactoryManager;

        // A pointer to a kernel32 function that exists only on versions of windows that are 6000 and higher.
        typedef void (WINAPI *PFnFlushProcessWriteBuffers)(void);
        PFnFlushProcessWriteBuffers m_pfnFlushProcessWriteBuffers;

        // A pointer to a kernel32 function that exists only on versions of windows that are 6100 and higher.
        typedef VOID (WINAPI *PFnGetCurrentProcessorNumberEx)(PPROCESSOR_NUMBER);
        static PFnGetCurrentProcessorNumberEx s_pfnGetCurrentProcessorNumberEx;

        // A pointer to a page that is used to flush write buffers on versions of Windows < 6000.
        char* m_pPageVirtualProtect;

#if defined(CONCRT_TRACING)

        // Captures the initial global state during the DRM phase.
        GlobalCoreData * m_drmInitialState;
        unsigned int m_numTotalCores;

        // Maintains a trace for every core removed during preprocessing.
        PreProcessingTraceData m_preProcessTraces[100];
        unsigned int m_preProcessTraceIndex;

        // Maintains a trace for each core allocation
        DynamicAllocationTraceData m_dynAllocationTraces[100];
        unsigned int m_dynAllocationTraceIndex;
#endif

        //
        // Private methods
        //

        // Private constructor.
        ResourceManager();

        // Private destructor.
        ~ResourceManager();

        /// <summary>
        ///     Initializes static information such as os version, node and core counts.
        /// </summary>
        static void InitializeSystemInformation(bool fSaveTopologyInfo = false);

        /// <summary>
        ///     Initialize function pointers that are only present on specific versions of the OS (Win7 or higher).
        /// <summary>
        static void InitializeSystemFunctionPointers();

        /// <summary>
        ///     Creates a scheduler proxy for an IScheduler that registers with the RM.
        /// </summary>
        SchedulerProxy* CreateSchedulerProxy(IScheduler *pScheduler);

        /// <summary>
        ///     Increments the reference count to RM but does not allow a 0 to 1 transition.
        /// </summary>
        /// <returns>
        ///     True if the RM was referenced; false, if the reference count was 0.
        /// </returns>
        bool SafeReference();

        /// <summary>
        ///     Creates a structure of nodes and cores based on the machine topology.
        /// </summary>
        void DetermineTopology();

        /// <summary>
        ///     Instructs schedulers to release borrowed cores. Then tries to allocate available cores for the new scheduler.
        /// </summary>
        unsigned int ReleaseBorrowedCores(SchedulerProxy * pNewProxy, unsigned int request);

        /// <summary>
        ///     Tries to redistribute cores allocated to all schedulers, proportional to each schedulers value for 'DesiredHardwareThreads',
        ///     and allocates any freed cores to the new scheduler.
        /// </summary>
        unsigned int RedistributeCoresAmongAll(SchedulerProxy* pSchedulerProxy, unsigned int allocated, unsigned int minimum, unsigned int desired);

        /// <summary>
        ///     Instructs all other schedulers to give up cores until they are at min. Then tries to allocate available cores
        ///     for the new scheduler.
        /// </summary>
        unsigned int ReduceOthersToMin(SchedulerProxy * pNewProxy, unsigned int request);

        /// <summary>
        ///     Allocates cores to the new scheduler at higher use counts - this is use only to satisify MinHWThreads.
        /// </summary>
        unsigned int AllocateAtHigherUseCounts(SchedulerProxy * pSchedulerProxy, unsigned int request);

        /// <summary>
        ///     Worker routine that does the actual core allocation, using the supplied use count. It tries to
        ///     pack allocated cores onto nodes by preferring nodes where more free cores are available.
        /// </summary>
        unsigned int AllocateCores(SchedulerProxy * pSchedulerProxy, unsigned int request, unsigned int useCount);

        /// <summary>
        ///     Instructs a scheduler proxy to free up a fixed number of resources. This is only a temporary release of resources. The
        ///     use count on the global core is decremented and the scheduler proxy remembers the core as temporarily released. At a later
        ///     point, the release is either confirmed or rolled back, depending on whether the released core was used to satisfy a
        ///     different scheduler's allocation.
        /// </summary>
        /// <param name="pSchedulerProxy">
        ///     The scheduler proxy that needs to free up resources.
        /// </param>
        /// <param name="numberToFree">
        ///     The number of resources to free. This parameter can have a couple of special values:
        ///         ReleaseCoresUptoMin - scheduler should release all cores above it's minimum. Preference is giving to releasing borrowed cores.
        ///         ReleaseOnlyBorrowedCores - scheduler should release all its borrowed cores.
        /// </param>
        /// <param name="borrowedCoresReleased">
        ///     Tells whether a previous call to this API was made for the scheduler that successfully released borrowed cores. If this is true
        ///     the scheduler should only have owned cores.
        /// </param>
        bool ReleaseSchedulerResources(SchedulerProxy *pSchedulerProxy, unsigned int numberToFree, bool borrowedCoresReleased = false);

        /// <summary>
        ///     Called to claim back any previously released cores that were not allocated to a different scheduler. If released
        ///     cores were allocated (stolen), the proxy needs to notify its scheduler to give up the related virtual processor
        ///     roots.
        /// <summary>
        void RestoreSchedulerResources(SchedulerProxy * pSchedulerProxy);

        /// <summary>
        ///     Called to allocate a single resource from the current scheduler proxy to accomodate an external thread during
        ///     thread subscription. This function is called only if there are cores above minimum that this proxy can replace
        ///     with the external thread.
        /// <summary>
        unsigned int ReleaseSchedulerResourceAboveMin(SchedulerProxy * pSchedulerProxy);

        /// <summary>
        ///     Starts up the dynamic RM worker thread if it is on standby, or creates a thread if one is not already created.
        ///     The worker thread wakes up at fixed intervals and load balances resources among schedulers, until it it put on standby.
        /// </summary>
        void CreateDynamicRMWorker();

        /// <summary>
        ///     Routine that performs dynamic resource management among existing schedulers at fixed time intervals.
        /// </summary>
        void DynamicResourceManager();

        /// <summary>
        ///     Performs a dynamic resource allocation based on feedback from hill climbing.
        /// </summary>
        void DoCoreMigration();

        /// <summary>
        ///     When the number of schedulers in the RM goes from 2 to 1, this routine is invoked to make sure the remaining scheduler
        ///     has its desired number of cores, before putting the dynamic RM worker on standby. It is also called when there is just
        ///     one scheduler with external subscribed threads that it removes -> there is a chance that this move may allow us to allocate
        ///     more vprocs.
        /// </summary>
        bool DistributeCoresToSurvivingScheduler();

        /// <summary>
        ///     This API is called by the dynamic RM worker thread when it starts up, and right after its state changed to
        ///     LoadBalance after being on Standby for a while. We need to find the existing schedulers, and discard the
        ///     statistics they have collected so far if any. Either we've never collected statistics for this scheduler before,
        ///     or too much/too little time has passed since we last collected statistics, and this information cannot be trusted.
        /// </summary>
        void DiscardExistingSchedulerStatistics();

        /// <summary>
        ///     Ensures that the memory buffers needed for static and dynamic RM are of the right size, and initializes them.
        /// </summary>
        void InitializeRMBuffers();

        /// <summary>
        ///     Populates data needed for allocation (static or dynamic).
        /// </summary>
        void PopulateCommonAllocationData(unsigned int index, SchedulerProxy * pSchedulerProxy, AllocationData * pAllocationData);

        /// <summary>
        ///     Captures data needed for static allocation, for all existing schedulers. This includes determining which
        ///     cores on a scheduler are idle.
        ///     A number of preprocessing steps are are also preformed before we are ready to allocate cores for a new scheduler.
        ///
        ///     - If a borrowed core is now in use by the other scheduler(s) that own that core, it is taken away.
        ///     - If the scheduler with the borrowed core is now the only scheduler using the core, it is not considered borrowed any more.
        /// </summary>
        void SetupStaticAllocationData(SchedulerProxy * pNewProxy, bool fNeedsExternalThreadAllocation);

        /// <summary>
        ///     Captures data needed for dynamic allocation for all existing schedulers. This includes gathering statistics
        ///     and invoking a per scheduler hill climbing instance, to get a suggested future allocation. Also, determines how may
        ///     idle cores a scheduler has.
        /// </summary>
        void PopulateDynamicAllocationData();

        /// <summary>
        ///     Resets state that was set on the global cores during static or dynamic allocation.
        /// </summary>
        void ResetGlobalState();

        /// <summary>
        ///     Toggles the idle state on a core and updates tracking counts.
        /// </summary>
        void ToggleRMIdleState(SchedulerNode * pAllocatedNode, SchedulerCore * pAllocatedCore,
                                GlobalNode * pGlobalNode, GlobalCore * pGlobalCore, AllocationData * pDRMData);

        /// <summary>
        ///     A number of preprocessing steps are preformed before we are ready to migrate cores. They include handling of borrowed, idle,
        ///     and shared cores, as follows:
        ///
        ///     - If a borrowed core is now in use by the other scheduler(s) that own that core, it is taken away.
        ///     - If the scheduler with the borrowed core is now the only scheduler using the core, it is not considered borrowed anymore.
        ///     - If hill climbing has suggested an allocation increase for a scheduler that has idle cores, or an allocation decrease that
        ///         does not take away all its idle cores, the RM overrides it, setting the suggested allocation for that scheduler to
        ///         max(minCores, allocatedCores - idleCores).
        ///
        ///       The new value of suggested allocation is used for the following:
        ///     - If the suggested allocation is less than the current allocation for a scheduler that has shared cores (cores oversubscribed
        ///         with a different scheduler), those cores are taken away here, since we want to minimize sharing.
        /// </summary>
        void PreProcessDynamicAllocationData();

        /// <summary>
        ///     Preprocessing steps for borrowed cores - both static and dynamic allocation start out with a call to this API.
        /// </summary>
        void HandleBorrowedCores(SchedulerProxy * pSchedulerProxy, AllocationData * pAllocationData);

        /// <summary>
        ///     Preprocessing steps for shared cores - this is used during dynamic core migration.
        /// </summary>
        void HandleSharedCores(SchedulerProxy * pSchedulerProxy, DynamicAllocationData * pAllocationData);

        /// <summary>
        ///     This routine increases the suggested allocation to desired, for schedulers with the following characteristics:
        ///     1) Hill climbing has *not* recommended an allocation decrease.
        ///     2) They are using all the cores allocated to them (no idle cores).
        ///     In the second round of core migration, we try to satisfy these schedulers' desired allocation.
        /// </summary>
        void IncreaseFullyLoadedSchedulerAllocations();

        /// <summary>
        ///     Decides on the number of additional cores to give a set of schedulers, given what the schedulers need and what is available.
        /// </summary>
        unsigned int AdjustDynamicAllocation(unsigned int coresAvailable, unsigned int coresNeeded, unsigned int numReceivers);

        /// <summary>
        ///     Initializes receiving proxy data in preparation for core transfer. Calcuates the number of partially filled nodes
        ///     for schedulers that are receiving cores, and sorts the receiving proxy data in increasing order of partial nodes.
        /// </summary>
        /// <returns>
        ///     Number of recivers that still need cores (allocation field of the receiving proxy data > 0).
        /// </returns>
        unsigned int PrepareReceiversForCoreTransfer(unsigned int numReceivers);

        /// <summary>
        ///     Assigns one core at a time to a partially filled node on a receiving scheduler proxy, if possible
        /// </summary>
        bool FindCoreForPartiallyFilledNode(unsigned int& unusedCoreQuota,
                                            unsigned int& usedCoreQuota,
                                            DynamicAllocationData * pReceivingProxyData,
                                            unsigned int numGivers);

        /// <summary>
        ///     Attempts to assign cores to a receiver on a single empty node, taking cores from multiple givers if necessary.
        /// </summary>
        unsigned int FindBestFitExclusiveAllocation(unsigned int& unusedCoreQuota,
                                                    unsigned int& usedCoreQuota,
                                                    DynamicAllocationData * pReceivingProxyData,
                                                    unsigned int remainingReceivers,
                                                    unsigned int numGivers);

        /// <summary>
        ///     Distributes unused cores and cores from scheduler proxies that are willing to give up cores to scheduler proxies that
        ///     need cores.
        /// </summary>
        void DistributeExclusiveCores(unsigned int totalCoresNeeded,
                                      unsigned int unusedCoreQuota,
                                      unsigned int usedCoreQuota,
                                      unsigned int numReceivers,
                                      unsigned int numGivers);

        /// <summary>
        ///     Attempts to assign cores to a receiver on a single empty node, using idle cores.
        /// </summary>
        unsigned int FindBestFitIdleAllocation(unsigned int idleCoresAvailable, DynamicAllocationData * pReceivingProxyData, unsigned int remainingReceivers);

        /// <summary>
        ///     Distributes idle cores to scheduler proxies that need cores.
        /// </summary>
        void DistributeIdleCores(unsigned int totalCoresAvailable, unsigned int numReceivers);

        /// <summary>
        ///     Assigns a fixed number of unused or idle cores to a scheduler from a given node.
        /// </summary>
        void DynamicAssignCores(SchedulerProxy * pReceivingProxy, unsigned int nodeIndex, unsigned int numCoresToMigrate, bool fIdle);

        /// <summary>
        ///     Transfers a fixed number of cores from one scheduler to another.
        /// </summary>
        void DynamicMigrateCores(DynamicAllocationData * pGivingProxyData, SchedulerProxy * pReceivingProxy, unsigned int nodeIndex, unsigned int numCoresToMigrate);

#if defined (CONCRT_TRACING)

        /// <summary>
        ///     Captures the initial state of the global map at the beginning of core migration, each cycle.
        /// </summary>
        void TraceInitialDRMState();

        /// <summary>
        ///     Captures data relating to an action during DRM preprocessing.
        /// </summary>
        void TracePreProcessingAction(SchedulerProxy * pProxy, unsigned int nodeIndex, unsigned int coreIndex,
                                      bool fMarkedAsOwned, bool fBorrowedCoreRemoved, bool fSharedCoreRemoved, bool fIdleCore);

        /// <summary>
        ///     Captures data relating to an action during DRM core migration.
        /// </summary>
        void TraceCoreMigrationAction(SchedulerProxy * pGiver, SchedulerProxy * pReceiver, unsigned int round, unsigned int nodeIndex,
                                      unsigned int coreIndex, bool fUnusedCoreMigration, bool fIdleCoreSharing, bool fBorrowedByGiver,
                                      bool fIdleOnGiver);

        /// <summary>
        ///     Dumps allocations, for existing scheduler proxies, and a newly allocated scheduler proxy, if specified.
        /// </summary>
        void DumpAllocations(SchedulerProxy *pSchedulerProxy = NULL);

        /// <summary>
        ///     Dumps the state of the global nodes in the RM - system wide view of resource allocation.
        /// </summary>
        void DumpGlobalNodes();

#endif
        /// <summary>
        ///     Performs borrowed core validation. A core can be borrowed by only one scheduler at a time.
        /// </summary>
        void ValidateBorrowedCores();

        /// <summary>
        ///     Performs state validations during dynamic core migration.
        /// </summary>
        void ValidateDRMSchedulerState();

        /// <summary>
        ///     Main thread procedure for the dynamic RM worker thread.
        /// </summary>
        /// <param name="lpParameter">
        ///     Resource manager pointer passed to the worker thread.
        /// </param>
        /// <returns>
        ///     Status on thread exit.
        /// </returns>
        static DWORD CALLBACK DynamicRMThreadProc(LPVOID lpParameter);

        /// <summary>
        ///     Given a set of scaled floating point allocations that add up to nSum, rounds them to integers.
        /// </summary>
        static void RoundUpScaledAllocations(AllocationData ** ppData, unsigned int count, unsigned int nSum);
    };
} // namespace details
} // namespace Concurrency
