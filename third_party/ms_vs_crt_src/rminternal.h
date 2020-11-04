// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// rminternal.h
//
// Main internal header file for ConcRT's Resource Manager.
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

namespace Concurrency
{
namespace details
{
    // The lowest two bits are used to determine the type of the pointer stored in the
    // execution resource TLS slot (created in the RM).
    static const size_t TlsResourceBitMask = 0x3;
    static const size_t TlsResourceInResource = 0x0;
    static const size_t TlsResourceInProxy = 0x1;
    static const size_t TlsResourceInUMSProxy = 0x2;

    // The RM has an array of processor nodes and cores representing the hardware topology on the machine.
    // Every scheduler that asks for an allocation, gets it's own copy of this array of nodes/cores once they
    // have been granted an allocation of cores by the RM, that is stored in the corresponding scheduler proxy.

    /// <summary>
    ///     An abstraction of a hardware affinity that understands how to deal with affinity on both Win7 and pre-Win7
    ///     platforms.
    /// </summary>
    struct HardwareAffinity
    {
    public:

        /// <summary>
        ///     Construct an empty affinity.
        /// </summary>
        HardwareAffinity()
        {
            memset(&m_affinity, 0, sizeof(m_affinity));
            m_affinity.Group = 0;
            m_affinity.Mask = 0;
        }

        /// <summary>
        ///     Construct a hardware affinity from a given thread.
        /// </summary>
        HardwareAffinity(HANDLE hThread);

        /// <summary>
        ///    Construct a Win7 understood affinity.
        /// </summary>
        HardwareAffinity(USHORT processorGroup, ULONG_PTR affinityMask) 
        {
            memset(&m_affinity, 0, sizeof(m_affinity));
            m_affinity.Group = processorGroup;
            m_affinity.Mask = affinityMask;
        }

        /// <summary>
        ///     Construct a pre-Win7 understood affinity.
        /// </summary>
        HardwareAffinity(DWORD_PTR affinityMask)
        {
            memset(&m_affinity, 0, sizeof(m_affinity));
            m_affinity.Group = 0;
            m_affinity.Mask = affinityMask;
        }

        /// <summary>
        ///     Copy construct an affinity.
        /// </summary>
        HardwareAffinity(const HardwareAffinity &src)
        {
            memcpy(&m_affinity, &src.m_affinity, sizeof(m_affinity));
        }

        /// <summary>
        ///     Compare two affinities
        /// </summary>
        bool operator==(const HardwareAffinity &rhs)
        {
            return (rhs.m_affinity.Group == m_affinity.Group && rhs.m_affinity.Mask == m_affinity.Mask);
        }

        /// <summary>
        ///     Compare two affinities
        /// </summary>
        bool operator!=(const HardwareAffinity &rhs)
        {
            return !operator==(rhs);
        }

        /// <summary>
        ///     Copy an affinity.
        /// </summary>
        HardwareAffinity& operator=(const HardwareAffinity &rhs)
        {
            m_affinity.Group = rhs.m_affinity.Group;
            m_affinity.Mask = rhs.m_affinity.Mask;
            return *this;
        }

        /// <summary>
        ///     Applies this hardware affinity to a thread.
        /// </summary>
        /// <param name="hThread">
        ///     The thread handle to which to apply this affinity.
        /// </param>
        void ApplyTo(HANDLE hThread);

        /// <summary>
        ///     Initializes the SetThreadGroupAffinity function for Win 7 and higher.
        /// </summary>
        static void InitializeSetThreadGroupAffinityFn();

    private:

        // A pointer to a kernel32 function that exists only on Win7 and higher
        typedef BOOL (WINAPI *PFnSetThreadGroupAffinity)(HANDLE, PGROUP_AFFINITY, PGROUP_AFFINITY);
        static PFnSetThreadGroupAffinity s_pfnSetThreadGroupAffinity;

        // A pointer to a kernel32 function that exists only on Win7 and higher
        typedef BOOL (WINAPI *PFnGetThreadGroupAffinity)(HANDLE, PGROUP_AFFINITY);
        static PFnGetThreadGroupAffinity s_pfnGetThreadGroupAffinity;

        GROUP_AFFINITY m_affinity;

    };

    /// <summary>
    ///     Base class for description of a core or hardware thread.
    /// </summary>
    struct ProcessorCore
    {
        enum CoreState
        {
            // The core available for allocation
            Available = 0,

            // The core is allocated to a scheduler
            Allocated,

            // When cores are freed up from other schedulers, this will be the state they set their copy of the
            // core to, while setting the global copy to Available. This will enable them to track the cores they
            // have relinquished. If the new scheduler is not allocated this core, it will revert back to Allocated
            // for the scheduler proxy it came from.
            Stolen,

            // The core is reserved for a possible allocation
            Reserved,

            // A core is considered idle during dynamic core migration if the scheduler(s) that core is assigned
            // to, have all vprocs de-activated.
            Idle
        };

        // 'Available' means available for assignment to a scheduler during the allocation calculation.
        CoreState m_coreState;

        // The processor number in Win7 {group, processor number} id scheme.
        BYTE m_processorNumber;
    };

    /// <summary>
    ///     Representation of a processor core within the RM's global map of execution resources. Information in this struct
    ///     represents a systemwide view of the underlying hardware thread.
    /// </summary>
    struct GlobalCore : public ProcessorCore
    {
        // The number of schedulers that this core is assigned to.
        unsigned int m_useCount;

        // Used to send notifications to qualifying schedulers regarding external subscription level changes.
        LONG m_currentSubscriptionLevel;
        LONG m_previousSubscriptionLevel;

        // This field is used during core migration to represent the number of schedulers that this core has been allocated
        // to, that have also deactivated all virtual processors on the core, i.e. the number of schedulers that are 'idle'
        // with respect to this core. When this is equal to the use count, the core is considered 'idle'.
        unsigned int m_idleSchedulers;
    };

    /// <summary>
    ///     Representation of a processor core within a scheduler proxy's local map of execution resources. Information in this struct
    ///     represents the schedulers utilization of the underlying hardware thread.
    /// </summary>
    struct SchedulerCore : public ProcessorCore
    {
        // When virtual processor roots are created for a scheduler proxy, or external threads are subscribed, the corresponding
        // execution resources are inserted into this list.
        List<ExecutionResource, CollectionTypes::Count> m_resources;

        // This field represents the number of activated virtual processors and subscribed threads that a scheduler has
        // on this core at any time. When a virtual processor root is deactivated, or when a thread subscription is released
        // the count is decremented. The core is considered to be 'idle' in the scheduler it belongs to, if this value is 0.
        volatile LONG m_subscriptionLevel;

        // Used to send notifications to qualifying schedulers regarding external subscription level changes.
        LONG m_currentSubscriptionLevel;
        LONG m_previousSubscriptionLevel;

        // The number of threads that were assigned to this core through initial allocation or core migration.
        // Note that this is not necessarily equal to the number of roots in the m_resources list, since the list
        // includes oversubscribed vproc roots as well.
        unsigned int m_numAssignedThreads;

        // The total number of threads (running on vprocs and external) that require this core to be fixed.
        unsigned int m_numFixedThreads;

        // The number of external threads that run on this core.
        unsigned int m_numExternalThreads;

        // This is set to true for a scheduler proxy's core during static allocation or core migration if the subscription
        // level on the core is found to be 0 when the Dynamic RM worker is executing. The subscription value can change
        // as soon as it is captured, but the captured value is what is used for successive computation.
        bool m_fIdleDuringDRM;

        // This is set to true for a scheduler proxy's core during core migration, if this is an borrowed core.
        // An borrowed core is a core that is assigned to one or more different schedulers, but was found to be idle.
        // The RM temporarily assigns idle resources to schedulers that need them.
        bool m_fBorrowed;

        // This variable is set to true when a borrowed core is converted to a fixed core. When the core is unfixed,
        // it is marked borrowed again.
        bool m_fPreviouslyBorrowed;

        /// <summary>
        ///     Returns whether this core is fixed, i.e., cannot be removed by the RM.
        /// </summary>
        bool IsFixed()
        {
            return m_numFixedThreads > 0;
        }

        /// <summary>
        ///     Returns whether this core is idle, i.e., its subscription level was 0 at the time it was retreived by the RM.
        ///     Note that this state could change, but once we capture it, we consider it idle until the next time it is captured.
        /// </summary>
        bool IsIdle()
        {
            return m_fIdleDuringDRM;
        }

        /// <summary>
        ///     Returns whether this core is borrowed, i.e., it was temporarily lent to this scheduler due to the owning
        ///     scheduler being idle on this core.
        /// </summary>
        bool IsBorrowed()
        {
            return m_fBorrowed;
        }
    };

    /// <summary>
    ///     Base class for the description of a processor package or NUMA node.
    /// </summary>
    struct ProcessorNode
    {
        // affinity mask of node
        ULONG_PTR m_nodeAffinity;

        // total number of cores in the node
        unsigned int m_coreCount;

        // number of cores allocated to the scheduler proxy (this field is only applicable to a scheduler proxy's nodes)
        unsigned int m_allocatedCores;

        // A scratch field used during allocation. The allocation routine works by looking at cores with m_useCount=0,
        // grabs all it can, then looks at m_useCount=1, then m_useCount=2, etc... During an allocation attempt at a particular
        // use count, cores that are allocated at previous use counts are stored in m_allocatedCores, and cores available at the
        // current use count are stored in m_availableForAllocation. A subset of those may be allocated to the scheduler proxy,
        // and that number is added to m_allocatedCores before moving on.
        unsigned int m_availableForAllocation;

        // The group number in Win7 {group, mask} id scheme
        unsigned int m_processorGroup;

        // The node id which maps to a scheduler node id.
        unsigned int m_id;
    };

    /// <summary>
    ///     Representation of a processor node within a scheduler proxy's local map of execution resources. Information in this struct
    ///     represents the schedulers utilization of the underlying node.
    /// </summary>
    struct SchedulerNode : public ProcessorNode
    {
        // The number of allocated cores that are borrowed. An borrowed core is a core that is assigned to
        // one or more different schedulers, but was found to be idle. The RM temporarily assigns idle resources to
        // schedulers that need them.
        unsigned int m_numBorrowedCores;

        // The number of cores on this node that are considered fixed. Fixed cores cannot be removed by the RM during static/dynamic allocation.
        unsigned int m_numFixedCores;

        // The number of cores in this node for the scheduler in question that were found to be idle during the dynamic RM phase. This is
        // a scratch field, and the value is stale outside of dynamic RM phases.
        unsigned int m_numDRMIdle;

        // The number of borrowed cores in this node for the scheduler in question that were found to be idle during the dynamic RM phase.
        // This is a scratch field, and the value is stale outside of dynamic RM phases.
        unsigned int m_numDRMBorrowedIdle;

        // The array of cores in this node.
        SchedulerCore * m_pCores;

        /// <summary>
        ///     Returns the number of cores that were found to be idle.
        /// </summary>
        unsigned int GetNumIdleCores()
        {
            return m_numDRMIdle;
        }

        /// <summary>
        ///     Returns the number of allocated cores in this node that are fixed - cannot be removed by dynamic RM.
        /// </summary>
        unsigned int GetNumFixedCores()
        {
            return m_numFixedCores;
        }

        /// <summary>
        ///     Returns the number of movable cores within this node.
        /// </summary>
        unsigned int GetNumMigratableCores()
        {
            return m_allocatedCores - m_numFixedCores;
        }

        /// <summary>
        ///     Returns the number of owned cores - cores that are not borrowed from a different scheduler.
        /// </summary>
        unsigned int GetNumOwnedCores()
        {
            return m_allocatedCores - m_numBorrowedCores;
        }

        /// <summary>
        ///     Returns the number of non-borrowed, non-fixed cores.
        unsigned int GetNumOwnedMigratableCores()
        {
            return m_allocatedCores - m_numBorrowedCores - m_numFixedCores;
        }

        /// <summary>
        ///     Returns the number of borrowed cores - cores that were temporarily lent to this scheduler since the scheduler(s) they
        ///     were assigned to, were not using them.
        /// </summary>
        unsigned int GetNumBorrowedCores()
        {
            return m_numBorrowedCores;
        }

        /// <summary>
        ///     Returns the number of borrowed cores that are idle.
        /// </summary>
        unsigned int GetNumBorrowedIdleCores()
        {
            return m_numDRMBorrowedIdle;
        }

        /// <summary>
        ///     Returns the number of borrowed cores that are not idle.
        /// </summary>
        unsigned int GetNumBorrowedInUseCores()
        {
            ASSERT(m_numBorrowedCores >= m_numDRMBorrowedIdle);
            return (m_numBorrowedCores - m_numDRMBorrowedIdle);
        }

        /// <summary>
        ///     Deallocates memory allocated by the node.
        /// </summary>
        void Cleanup(void)
        {
            delete [] m_pCores;
        }
    };

    /// <summary>
    ///     Representation of a processor node within the RM's global map of execution resources. Information in this struct
    ///     represents a systemwide view of the underlying node.
    /// </summary>
    struct GlobalNode : public ProcessorNode
    {
        // A scratch field used during dynamic RM allocation, on the RM's global copy of nodes. Idle cores represents the number
        // of cores on this node that are idle and can temporarily be assigned to another scheduler that needs cores.
        unsigned int m_idleCores;

        // The array of cores in this node.
        GlobalCore * m_pCores;

        // An array of indices for sorting cores.
        unsigned int * m_pSortedCoreOrder;

        /// <summary>
        ///     Initializes a processor node.
        /// </summary>
        void Initialize(USHORT id, USHORT processorGroup, ULONG_PTR affinityMask)
        {
            m_id = id;
            m_processorGroup = processorGroup;
            m_nodeAffinity = affinityMask;

            m_coreCount = NumberOfBitsSet(affinityMask);
            m_allocatedCores = m_availableForAllocation = 0;

            m_pCores = new GlobalCore[m_coreCount];
            memset(m_pCores, 0, m_coreCount * sizeof(GlobalCore));

            m_pSortedCoreOrder = new unsigned int[m_coreCount];
            for (unsigned int i = 0; i < m_coreCount; ++i)
            {
                m_pSortedCoreOrder[i] = i;
            }

            for (unsigned int i = 0, j = 0; j < m_coreCount; ++i)
            {
                ASSERT(i < sizeof(ULONG_PTR) * 8);

                // Check if the LSB of the affinity mask is set.
                if ((affinityMask & 1) != 0)
                {
                    // Bit 0 of the affinity mask corresponds to processor number 0, bit 1 to processor number 1, etc...
                    m_pCores[j++].m_processorNumber = (BYTE) i;
                }
                // Right shift the affinity by 1.
                affinityMask >>= 1;
            }
        }

        /// <summary>
        ///     Creates a scheduler node from a global processor node. Used to create a representation of the node for
        ///     a scheduler proxy when allocation is complete. Also resets the original node so it is ready for the
        ///     next allocation attempt.
        /// </summary>
        void CloneAndReset(SchedulerNode * pNewNode)
        {
            ASSERT(pNewNode != NULL);
            // Copy the base class portion of the node, which is shared.
            memcpy(pNewNode, this, sizeof(ProcessorNode));

            pNewNode->m_pCores = new SchedulerCore[m_coreCount];
            memset(pNewNode->m_pCores, 0, m_coreCount * sizeof(SchedulerCore));

            // Since we're using a memset to initialize the elements of each SchedulerCore, add an assert
            // in case the implementation of List changes such that zeroing it out is not a 'good initial state'.
            ASSERT(pNewNode->m_pCores[0].m_resources.Empty());

            for (unsigned int i = 0; i < m_coreCount; ++i)
            {
                // Copy the base class portion of the core.
                memcpy(&pNewNode->m_pCores[i], &m_pCores[i], sizeof(ProcessorCore));
                // Reset the core state on the cores of the original node structure ('this')
                // once a deep copy has been made.
                m_pCores[i].m_coreState = ProcessorCore::Available;
                m_pCores[i].m_idleSchedulers = 0;
            }
            m_allocatedCores = 0;
            ASSERT(m_availableForAllocation == 0);
        }

        /// <summary>
        ///     Copies a processor node. Used to create a copy of the node from a scheduler proxy when allocation
        ///     is invoked in the "update" mode.
        /// </summary>
        void Copy(SchedulerNode * pCopyFromNode)
        {
            for (unsigned int i = 0; i < m_coreCount; ++i)
            {
                ASSERT(m_pCores[i].m_coreState == ProcessorCore::Available);
                m_pCores[i].m_coreState = pCopyFromNode->m_pCores[i].m_coreState;
            }
            m_allocatedCores = pCopyFromNode->m_allocatedCores;
        }

        /// <summary>
        ///     Copies the allocation changes in a processor node to the schedulers node after an allocation increase, and resets it.
        /// </summary>
        void CopyAndReset(SchedulerNode * pCopyToNode)
        {
#if defined(_DEBUG)
            unsigned int numChanged = 0;
#endif
            for (unsigned int i = 0; i < m_coreCount; ++i)
            {
#if defined(_DEBUG)
                if (m_pCores[i].m_coreState != pCopyToNode->m_pCores[i].m_coreState)
                {
                    ASSERT(m_pCores[i].m_coreState == ProcessorCore::Allocated);
                    ASSERT(pCopyToNode->m_pCores[i].m_coreState == ProcessorCore::Available);
                    ASSERT(++numChanged <= 1);
                }
#endif
                pCopyToNode->m_pCores[i].m_coreState = m_pCores[i].m_coreState;
                m_pCores[i].m_coreState = ProcessorCore::Available;
                m_pCores[i].m_idleSchedulers = 0;
            }
            ASSERT(pCopyToNode->m_allocatedCores <= m_allocatedCores);
            pCopyToNode->m_allocatedCores = m_allocatedCores;
            m_allocatedCores = 0;
            ASSERT(m_availableForAllocation == 0);
        }

        /// <summary>
        ///     Deallocates memory allocated by the node.
        /// </summary>
        void Cleanup(void)
        {
            delete [] m_pCores;
            delete [] m_pSortedCoreOrder;
        }
    };
    /// <summary>
    ///     Used to store information during static and dynamic allocation.
    /// </summary>
    struct AllocationData
    {
        // Index into an array of schedulers - used for sorting, etc.
        unsigned int m_index;

        // Additional allocation to give to a scheduler after proportional allocation decisions are made.
        unsigned int m_allocation;

        // Used to hold a scaled allocation value during proportional allocation.
        double m_scaledAllocation;

        // The scheduler proxy this allocation data is for.
        SchedulerProxy *m_pProxy;

        // Number of idle cores in a scheduler proxy during static allocation or dynamic core migration.
        unsigned int m_numIdleCores;

        // Number of idle cores in a scheduler proxy during static allocation or dynamic core migration that are also borrowed. During core
        // migration these cores are the first to go.
        unsigned int m_numBorrowedIdleCores;
    };

    struct StaticAllocationData : public AllocationData
    {
        // A field used during static allocation to decide on an allocation proportional to each scheduler's desired value.
        double m_adjustedDesired;

        // Tells if a thread subscription is a part of this static allocation request.
        bool m_fNeedsExternalThreadAllocation;
    };

    struct DynamicAllocationData : public AllocationData
    {
        // This variable is toggled back in forth during dynamic migration to instruct the RM whether or not
        // an exact fit allocation should be attempted - i.e. if a node has 3 available cores, but this scheduler proxy
        // needs only 2, keep searching in case a later node is found with 2 available cores.
        bool m_fExactFitAllocation;

        // Fully loaded is set to true when a scheduler is using all the cores that are allocated to it (no cores are idle)
        // AND it has less than its desired number of cores.
        bool m_fFullyLoaded;

        // Number suggested as an appropriate allocation for the scheduler proxy, by the hill climbing instance.
        unsigned int m_suggestedAllocation;

#if defined(CONCRT_TRACING)
        unsigned int m_originalSuggestedAllocation;
#endif

        union
        {
            // Struct used for a receiving proxy.
            struct
            {
                // Number of nodes in the scheduler proxy that are partially allocated.
                unsigned int m_numPartiallyFilledNodes;

                // As we go through dynamic allocation, the starting node index moves along the array of sorted nodes,
                // in a scheduling proxy that is receiving cores.
                unsigned int m_startingNodeIndex;
            };
            // Struct used for a giving proxy.
            struct
            {
                // Maximum number of borrowed idle cores this scheduler can give up.
                unsigned int m_borrowedIdleCoresToMigrate;

                // Maximum number of borrowed in-use cores this scheduler can give up.
                unsigned int m_borrowedInUseCoresToMigrate;

                // Maximum number of owned cores this scheduler can give up.
                unsigned int m_ownedCoresToMigrate;
            };
        };
    };
} // namespace details
} // namespace Concurrency
