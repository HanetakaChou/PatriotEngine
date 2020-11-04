// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// SchedulingNode.h
//
// Source file containing the SchedulingNode declaration.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Concurrency
{
namespace details
{
    /// <summary>
    /// A scheduling node corresponds to a NUMA node or a processor package; containing one or more virtual processor groups.
    /// </summary>
    class SchedulingNode
    {
    public:

        /// <summary>
        ///     Constructs a scheduling node.
        /// </summary>
        SchedulingNode(SchedulingRing *pRing);

        /// <summary>
        ///     Destroys a scheduling node.
        /// </summary>
        ~SchedulingNode();

        /// <summary>
        ///     Creates and adds a new virtual processor in the node to associated with the root provided.
        ///     NOTE: For non-oversubscribed vprocs this API is currently will only work for intial allocation.
        /// </summary>
        /// <param name="pOwningRoot">
        ///     The virtual processor root to create the virtual processor with.
        /// </param>
        /// <param name="fOversubscribed">
        ///     True if this is an oversubscribed virtual processor.
        /// </param>
        /// <returns>
        ///     The newly created virtual processor.
        /// </returns>
        VirtualProcessor* AddVirtualProcessor(IVirtualProcessorRoot *pOwningRoot, bool fOversubscribed = false);

        /// <summary>
        ///     Returns the scheduler associated with the node.
        /// </summary>
        SchedulerBase * GetScheduler() { return m_pScheduler; }

        /// <summary>
        ///     Returns the scheduling ring associated with the node.
        /// </summary>
        SchedulingRing * GetSchedulingRing() { return m_pRing; }

        /// <summary>
        ///     Find the virtual processor in this node that matches the root provided.
        /// </summary>
        /// <param name="pRoot">
        ///     The virtual processor root to match.
        /// </param>
        VirtualProcessor* FindMatchingVirtualProcessor(IVirtualProcessorRoot* pRoot);

        /// <summary>
        ///     Returns the ID of the scheduling node.
        /// </summary>
        int Id() const
        {
            return m_id;
        }

        /// <summary>
        ///     Returns the first virtual processor.
        /// </summary>
        /// <param name="pIdx">
        ///     The iterator position of the returned virtual processor will be placed here.  This can only be 
        ///     utilized as the pIdx parameter or the idxStart parameter of a GetNextVirtualProcessor.
        /// </param>
        VirtualProcessor *GetFirstVirtualProcessor(int *pIdx)
        {
            *pIdx = 0;
            return GetNextVirtualProcessor(pIdx, -1);
        }

        /// <summary>
        ///     Returns the next virtual processor in an iteration.
        /// </summary>
        VirtualProcessor *GetNextVirtualProcessor(int *pIdx, int idxStart = 0)
        {
            int base = *pIdx + (idxStart == -1 ? 0 : 1);
            int size = m_virtualProcessors.MaxIndex();
            for (int i = 0; i < size; i++)
            {
                int index = (i + base) % size;
                if (index == idxStart)
                    return NULL;

                VirtualProcessor *pVProc = m_virtualProcessors[index];
                if (pVProc != NULL)
                {
                    *pIdx = index;
                    return pVProc;
                }
            }

            return NULL;
        }

    private:
        friend class SchedulerBase;
        friend class VirtualProcessor;
        friend class UMSThreadVirtualProcessor;
        friend class InternalContextBase;
        friend class FairScheduleGroup;
        template <typename T> friend class ListArray;

        // Owning scheduler
        SchedulerBase *m_pScheduler;

        // Owning ring
        SchedulingRing * const m_pRing;

        volatile LONG m_virtualProcessorAvailableCount;
        volatile LONG m_virtualProcessorCount;
        volatile LONG m_ramblingCount; // rambling -- searching foreign nodes for work

        int m_id;

        // Virtual processors owned by this node.
        ListArray<VirtualProcessor> m_virtualProcessors;

        InternalContextBase *StealLocalRunnableContext(VirtualProcessor* pSkipVirtualProcessor = NULL);

        /// <summary>
        ///     Find an available virtual processor in the scheduling node.
        /// </summary>
        VirtualProcessor* FindAvailableVirtualProcessor(VirtualProcessor *pBias = NULL);

        /// When all vprocs are rambling (searching foreign nodes for work), find the ring with the most vprocs working on it and set that to
        /// the owning ring for all vprocs.
        void CheckForNewOwningRing();

        void Cleanup();

        // Prevent warning about generated assignment operator & copy constructors.
        SchedulingNode(const SchedulingNode&);
        void operator=(const SchedulingNode&);
    };
} // namespace details
} // namespace Concurrency
