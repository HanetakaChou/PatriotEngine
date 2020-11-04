// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// SearchAlgorithms.h
//
// Header file containing definitions for all scheduling algorithms.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Concurrency
{
namespace details
{

    /// <summary>
    ///     Variant type representing a work item returned from a search.
    /// </summary>
    class WorkItem
    {
    public:

        /// <summary>
        ///     The type of work item.
        /// </summary>
        enum WorkItemType
        {
            WorkItemTypeNone = 0x0,
            WorkItemTypeContext = 0x1,
            WorkItemTypeRealizedChore = 0x2,
            WorkItemTypeUnrealizedChore = 0x4
        };

        /// <summary>
        ///     Default constructor for a work item.
        /// </summary>
        WorkItem() :
            m_type(WorkItemTypeNone),
            m_pItem(NULL)
        {
        }

        /// <summary>
        ///     Constructs a work item from an internal context.
        /// </summary>
        WorkItem(InternalContextBase *pContext);

        /// <summary>
        ///     Constructs a work item from a realized chore.
        /// </summary>
        WorkItem(RealizedChore *pRealizedChore, ScheduleGroupBase *pGroup) :
            m_type(WorkItemTypeRealizedChore),
            m_pGroup(pGroup),
            m_pRealizedChore(pRealizedChore)
        {
        }

        /// <summary>
        ///    Constructs a work item from an unrealized chore.
        /// </summary>
        WorkItem(_UnrealizedChore *pUnrealizedChore, ScheduleGroupBase *pGroup) :
            m_type(WorkItemTypeUnrealizedChore),
            m_pGroup(pGroup),
            m_pUnrealizedChore(pUnrealizedChore)
        {
        }

        /// <summary>
        ///     Transfers reference counts as necessary to inline the given work item on the given context.  This may
        ///     only be called on a work item that can be inlined (e.g.: an unbound one).
        /// </summary>
        /// <param name="pContext">
        ///     The context that is attempting to inline the work item.
        /// </param>
        void TransferReferences(InternalContextBase *pContext);

        /// <summary>
        ///     Binds the work item to a context and returns the context.  This may or may not allocate a new context.  Note that
        ///     act of binding which performs a context allocation will transfer a single count of work to the counter of the new
        ///     context.
        /// </summary>
        InternalContextBase *Bind();

        /// <summary>
        ///     Invokes the work item.
        /// </summary>
        void Invoke();

        /// <summary>
        ///     Accessor for type.
        /// </summary>
        WorkItemType GetType() const
        {
            return m_type;
        }

        /// <summary>
        ///     Returns the work item.
        /// </summary>
        void *GetItem() const
        {
            return m_pItem;
        }

        /// <summary>
        ///     Returns whether the work item is a context or not.
        /// </summary>
        bool IsContext() const
        {
            return (m_type == WorkItemTypeContext);
        }

        /// <summary>
        ///     Accessor for a context.
        /// </summary>
        InternalContextBase *GetContext() const
        {
            CORE_ASSERT(m_type == WorkItemTypeContext);
            return m_pContext;
        }

        /// <summary>
        ///     Accessor for a realized chore.
        /// </summary>
        RealizedChore *GetRealizedChore() const
        {
            CORE_ASSERT(m_type == WorkItemTypeRealizedChore);
            return m_pRealizedChore;
        }

        /// <summary>
        ///     Accessor for an unrealized chore.
        /// </summary>
        _UnrealizedChore *GetUnrealizedChore() const
        {
            CORE_ASSERT(m_type == WorkItemTypeUnrealizedChore);
            return m_pUnrealizedChore;
        }

        /// <summary>
        ///     Accessor for the schedule group.
        /// </summary>
        ScheduleGroupBase *GetScheduleGroup() const
        {
            return m_pGroup;
        }

    private:

        // The type of work item
        WorkItemType m_type;

        // The schedule group that the work item was found in.
        ScheduleGroupBase *m_pGroup; 

        // The work item itself
        union
        {
            void *m_pItem;
            InternalContextBase *m_pContext;
            RealizedChore *m_pRealizedChore;
            _UnrealizedChore *m_pUnrealizedChore;
        };

    };

    /// <summary>
    ///     A class which tracks iterator state for a search-for-work.  This is generic in terms of search algorithm.
    /// </summary>
    class WorkSearchContext
    {
    public:

        enum Algorithm
        {
            AlgorithmNotSet = 0,
            AlgorithmCacheLocal,
            AlgorithmFair
        };

        /// <summary>
        ///     Constructs a work search context that will be reset later.
        /// </summary>
        WorkSearchContext() : m_pSearchFn(NULL), m_pSearchYieldFn(NULL), m_pVirtualProcessor(NULL), m_pScheduler(NULL)
        {
        }

        /// <summary>
        ///     Constructs a work search context (an iterator position for a search algorithm).
        /// </summary>
        WorkSearchContext(VirtualProcessor *pVirtualProcessor, Algorithm algorithm)
        {
            Reset(pVirtualProcessor, algorithm);
        }

        /// <summary>
        ///     Resets the work search context to utilize the specified algorithm at the starting iterator position.
        /// </summary>
        /// <param name="pVirtualProcessor">
        ///     The virtual processor binding the searching.
        /// </param>
        /// <param name="algorithm">
        ///     What algorithm to reset the iterator with.
        /// </param>
        void Reset(VirtualProcessor *pVirtualProcessor, Algorithm algorithm);

        /// <summary>
        ///     Searches from the last iterator position according to the set algorithm.  This can return any type of work
        ///     item (context, realized chore, or unrealized chore)
        /// </summary>
        /// <param name="pWorkItem">
        ///     Upon successful return, the resulting work item is placed here along with information about what group it was found in, etc...
        /// </param>
        /// <param name="pOriginGroup">
        ///     The schedule group of the context which is performing the search.
        /// </param>
        /// <param name="allowableTypes">
        ///     What type of work items are allowed to be returned.
        /// </param>
        /// <returns>
        ///     An indication of whether a work item was found or not.
        /// </returns>
        bool Search(WorkItem *pWorkItem, 
                    ScheduleGroupBase *pOriginGroup,
                    ULONG allowableTypes = WorkItem::WorkItemTypeContext | WorkItem::WorkItemTypeRealizedChore | WorkItem::WorkItemTypeUnrealizedChore)
        {
            return (this->*m_pSearchFn)(pWorkItem, pOriginGroup, allowableTypes);
        }


        /// <summary>
        ///     Searches from the last iterator position according to the set algorithm for a yield.  This can return any type of
        ///     work item (context, realized chore, or unrealized chore)
        /// </summary>
        bool YieldingSearch(WorkItem *pWorkItem, 
                                ScheduleGroupBase *pOriginGroup,
                                ULONG allowableTypes = WorkItem::WorkItemTypeContext | WorkItem::WorkItemTypeRealizedChore)
        {
            return (this->*m_pSearchYieldFn)(pWorkItem, pOriginGroup, allowableTypes);
        }
                                
    private:

        // **************************************************
        // Common:
        //

        //
        // NOTE: The m_pCurrentRing and m_pStartingRing are flagged as volatile because it's possible that something blocks inside
        // the search algorithm and the UMS primary invokes a search for runnables during that point.  In that case, the routine must
        // be fully reentrant and the original search may have m_pCurrentRing/m_pStartingRing overwritten by the primary.  This is not a problem
        // if the search code takes care.  It snaps these values at the top of a loop and utilizes them during the search locally.
        //

        // The current scheduling ring.
        SchedulingRing * volatile m_pCurrentRing;

        // The ring at which we started a search
        SchedulingRing * volatile m_pStartingRing;

        // The virtual processor binding the search.
        VirtualProcessor *m_pVirtualProcessor;

        // The scheduler
        SchedulerBase *m_pScheduler;

        // How many times work has been found in the local bias (LRCs, current schedule group) since the last local bias reset
        ULONG m_localBias;

        // How many times work has been found in the local bias OR current scheduling ring since the last ring bias reset.
        ULONG m_ringBias;

        // The search function to utilize.
        bool (WorkSearchContext::*m_pSearchFn)(WorkItem *pWorkItem, 
                                               ScheduleGroupBase *pOriginGroup, 
                                               ULONG allowableTypes);

        // The search function to utilize for yielding.
        bool (WorkSearchContext::*m_pSearchYieldFn)(WorkItem *pWorkItem, 
                                                    ScheduleGroupBase *pOriginGroup, 
                                                    ULONG allowableTypes);

        // An indication of whether we are to perform a one time LRC scan upon a forced ring change.
        bool m_fPerformOneTimeLRCScan;
        bool m_fPerformedOneTimeLRCScan;

        /// <summary>
        ///     Performs a pre-search for any "special" contexts (e.g.: the UMS SUT)
        /// </summary>
        bool PreSearch(WorkItem *pWorkItem);

        /// <summary>
        ///     Steals a local runanble from a virtual processor within the specified node.  Note that this allows a given virtual processor
        ///     to be skipped.
        /// </summary>
        bool StealLocalRunnable(WorkItem *pWorkItem, SchedulingNode *pNode, VirtualProcessor *pSkipVirtualProcessor);

        /// <summary>
        ///     Steals a local runnable from a virtual processor of any scheduling node other than the specified local node.
        /// </summary>
        bool StealForeignLocalRunnable(WorkItem *pWorkItem, SchedulingNode *pLocalNode);

        /// <summary>
        ///     Gets a local runnable context from the specified virtual processor.
        /// </summary>
        bool GetLocalRunnable(WorkItem *pWorkItem, VirtualProcessor *pVirtualProcessor, bool fSteal);

        /// <summary>
        ///     Gets a runnable from the specified group.
        /// </summary>
        bool GetRunnableContext(WorkItem *pWorkItem, ScheduleGroupBase *pGroup);

        /// <summary>
        ///     Called on any biased work.
        /// </summary>
        void Bias()
        {
            m_localBias++;
            m_ringBias++;
        }

        /// <summary>
        ///     Resets the local bias counter but not the ring bias counter.
        /// </summary>
        void ResetLocalBias()
        {
            m_localBias = 0;
        }

        /// <summary>
        ///     Resets all bias counters.
        /// </summary>
        void ResetBias()
        {
            m_localBias = m_ringBias = 0;
            m_fPerformOneTimeLRCScan = false;
            m_fPerformedOneTimeLRCScan = false;
        }

        /// <summary>
        ///     Returns the current stage of local bias.
        /// </summary>
        int LocalBiasStage()
        {
            if (m_localBias < 101)
                return 0; // (fwd) Normal --> LRC LIFO
            else if (m_localBias < 127)
                return 1; // (fwd) Flip LRC --> LRC FIFO
            else if (m_localBias < 151)
                return 2; // (fwd) Skip LRC --> runnables
            else
                return 3; // (fwd) Skip SG --> inject change
        }

        /// <summary>
        ///     Returns whether we drop the bias to the current scheduling ring.  This is the final stage of dropping bias.  After
        ///     this, bias should be reset and we can go back to being fully cache local on another scheduling ring.
        /// </summary>
        bool DropRingBias()
        {
            bool fDrop = (m_ringBias > 307);
            if (fDrop && !m_fPerformedOneTimeLRCScan)
                m_fPerformOneTimeLRCScan = true;

            return fDrop;
        }

        /// <summary>
        ///     Informs us whether we perform a one time LRC scan upon entering a new ring via a forced transition.
        /// </summary>
        bool PerformOneTimeLRCScan()
        {
            if (m_fPerformOneTimeLRCScan)
            {
                m_fPerformOneTimeLRCScan = false;
                m_fPerformedOneTimeLRCScan = true;
                return true;
            }

            return false;
        }


        // **************************************************
        // Cache Local Algorithm:
        //

        /// <summary>
        ///     Performs a cache local search for runnables in the specified ring.
        /// </summary>
        bool SearchCacheLocal_Runnables(WorkItem *pWorkItem, SchedulingRing *pRing, ScheduleGroupBase *pOriginGroup);

        /// <summary>
        ///     Performs a cache local search for realized chores in the specified ring.
        /// </summary>
        bool SearchCacheLocal_Realized(WorkItem *pWorkItem, SchedulingRing *pRing, SchedulingRing *pStartingRing, ScheduleGroupBase *pOriginGroup);

        /// <summary>
        ///     Performs a cache local search for unrealized chores in the specified ring.
        /// </summary>
        bool SearchCacheLocal_Unrealized(WorkItem *pWorkItem, SchedulingRing *pRing, SchedulingRing *pStartingRing, ScheduleGroupBase *pOriginGroup);

        /// <summary>
        ///     Performs a cache local search for work.
        /// </summary>
        bool SearchCacheLocal(WorkItem *pWorkItem, ScheduleGroupBase *pOriginGroup, ULONG allowableTypes);

        /// <summary>
        ///     Performs a cache local search for work in the yielding case.
        /// </summary>
        bool SearchCacheLocalYield(WorkItem *pWorkItem, ScheduleGroupBase *pOriginGroup, ULONG allowableTypes);

        // **************************************************
        // Fair Algorithm:
        //

        /// <summary>
        ///     Performs a fair search for runnables in the specified ring.
        /// </summary>
        bool SearchFair_Runnables(WorkItem *pWorkItem, SchedulingRing *pRing);

        /// <summary>
        ///     Performs a fair search for realized chores in the specified ring.
        /// </summary>
        bool SearchFair_Realized(WorkItem *pWorkItem, SchedulingRing *pRing);

        /// <summary>
        ///     Performs a fair search for unrealized chores in the specified ring.
        /// </summary>
        bool SearchFair_Unrealized(WorkItem *pWorkItem, SchedulingRing *pRing);

        /// <summary>
        ///     Performs a fair search for work.
        /// </summary>
        bool SearchFair(WorkItem *pWorkItem, ScheduleGroupBase *pOriginGroup, ULONG allowableTypes);

        /// <summary>
        ///     Performs a fair search for work in the yielding case.
        /// </summary>
        bool SearchFairYield(WorkItem *pWorkItem, ScheduleGroupBase *pOriginGroup, ULONG allowableTypes); 

    };

}
}

