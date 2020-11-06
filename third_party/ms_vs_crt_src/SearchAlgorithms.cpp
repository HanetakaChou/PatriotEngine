// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// SearchAlgorithms.cpp
//
// Implementation file containing all scheduling algorithms.
//
// **PLEASE NOTE**:
//
//     Any search algorithm in here must be fully reentrant.  On UMS schedulers, the UMS primary will invoke these routines
//     to perform a search for work.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    //*************************************************************************** 
    //
    // General:
    //

    /// <summary>
    ///     Constructs a work item from an internal context.
    /// </summary>
    WorkItem::WorkItem(InternalContextBase *pContext) :
        m_type(WorkItemTypeContext),
        m_pGroup(pContext->GetScheduleGroup()),
        m_pContext(pContext)
    {
    }

    /// <summary>
    ///     Binds the work item to a context and returns the context.  This may or may not allocate a new context.  Note that
    ///     act of binding which performs a context allocation will transfer a single count of work to the counter of the new
    ///     context.
    /// </summary>
    InternalContextBase *WorkItem::Bind()
    {
        switch(m_type)
        {
            case WorkItemTypeUnrealizedChore:
                m_pContext = m_pGroup->GetInternalContext(m_pUnrealizedChore, true);
                m_pContext->SaveDequeuedTask();
                break;
            case WorkItemTypeRealizedChore:
                m_pContext = m_pGroup->GetInternalContext(m_pRealizedChore);
                m_pContext->SaveDequeuedTask();
                break;
            case WorkItemTypeContext:
                break;
        }

        m_type = WorkItemTypeContext;
        return m_pContext;
    }

    /// <summary>
    ///     Invokes the work item.
    /// </summary>
    void WorkItem::Invoke()
    {
        CORE_ASSERT(m_type == WorkItemTypeRealizedChore || m_type == WorkItemTypeUnrealizedChore);
        switch(m_type)
        {
            case WorkItemTypeUnrealizedChore:
                m_pUnrealizedChore->_Invoke();
                break;
            case WorkItemTypeRealizedChore:
                m_pRealizedChore->Invoke();
                m_pGroup->GetScheduler()->ReleaseRealizedChore(m_pRealizedChore);
                break;
        }
    }

    /// <summary>
    ///     Transfers reference counts as necessary to inline the given work item on the given context.  This may
    ///     only be called on a work item that can be inlined (e.g.: an unbound one).
    /// </summary>
    /// <param name="pContext">
    ///     The context that is attempting to inline the work item.
    /// </param>
    void WorkItem::TransferReferences(InternalContextBase *pContext)
    {
        ASSERT(m_type == WorkItemTypeRealizedChore || m_type == WorkItemTypeUnrealizedChore);

        ScheduleGroupBase *pGroup = pContext->GetScheduleGroup();
        if (m_type == WorkItemTypeRealizedChore)
        {
            if (pGroup != m_pGroup)
                pContext->SwapScheduleGroup(m_pGroup, false);
            else
                //
                // If newGroup is the same as the existing group, we need to release a reference since both, the context,
                // and the realized chore, have a reference on the schedule group, and we only need to hold one reference.
                //
                pGroup->InternalRelease();

        }
        else if (pGroup != m_pGroup)
        {
            pContext->SwapScheduleGroup(m_pGroup, true);
        }
    }

    /// <summary>
    ///     Resets the work search context to utilize the specified algorithm at the starting iterator position.
    /// </summary>
    /// <param name="pVirtualProcessor">
    ///     The virtual processor binding the searching.
    /// </param>
    /// <param name="algorithm">
    ///     The algorithm to reset the iterator with.
    /// </param>
    void WorkSearchContext::Reset(VirtualProcessor *pVirtualProcessor, Algorithm algorithm)
    {
        m_pCurrentRing = NULL;
        m_localBias = 0;
        m_ringBias = 0;
        m_fPerformOneTimeLRCScan = false;
        m_fPerformedOneTimeLRCScan = false;
        m_pVirtualProcessor = pVirtualProcessor;
        m_pScheduler = pVirtualProcessor->GetOwningNode()->GetScheduler();

        switch(algorithm)
        {
            case AlgorithmCacheLocal:
                m_pSearchFn = &WorkSearchContext::SearchCacheLocal;
                m_pSearchYieldFn = &WorkSearchContext::SearchCacheLocalYield;
                break;
            case AlgorithmFair:
                m_pSearchFn = &WorkSearchContext::SearchFair;
                m_pSearchYieldFn = &WorkSearchContext::SearchFairYield;
                break;
            default:
                ASSERT(false);
        }
    }

    /// <summary>
    ///     Steals a local runnable from a virtual processor within the specified node.  Note that this allows a given virtual processor
    ///     to be skipped.
    /// </summary>
    bool WorkSearchContext::StealLocalRunnable(WorkItem *pWorkItem, SchedulingNode *pNode, VirtualProcessor *pSkipVirtualProcessor)
    {
        int idx;
        VirtualProcessor *pVProc = pNode->GetFirstVirtualProcessor(&idx);
        while (pVProc != NULL)
        {
            if (pVProc != pSkipVirtualProcessor)
            {
                InternalContextBase *pContext = pVProc->StealLocalRunnableContext();
                if (pContext != NULL)
                {
                    *pWorkItem = WorkItem(pContext);
                    return true;
                }
            }
            
            pVProc = pNode->GetNextVirtualProcessor(&idx);
        }

        return false;
    }

    /// <summary>
    ///     Steals a local runnable from a virtual processor of any scheduling node other than the specified local node.
    /// </summary>
    bool WorkSearchContext::StealForeignLocalRunnable(WorkItem *pWorkItem, SchedulingNode *pLocalNode)
    {
        int idx;
        SchedulingNode *pNode = m_pScheduler->GetFirstSchedulingNode(&idx);
        while (pNode != NULL)
        {
            if (pNode != pLocalNode)
            {
                if (StealLocalRunnable(pWorkItem, pNode, NULL))
                    return true;
            }

            pNode = m_pScheduler->GetNextSchedulingNode(&idx);
        }

        return false;

    }

    /// <summary>
    ///     Performs a pre-search for any "special" contexts (e.g.: the UMS SUT)
    /// </summary>
    bool WorkSearchContext::PreSearch(WorkItem *pWorkItem)
    {
        InternalContextBase *pContext = m_pVirtualProcessor->PreRunnableSearch();
        if (pContext != NULL)
        {
            *pWorkItem = WorkItem(pContext);
            return true;
        }

        return false;
    }

    /// <summary>
    ///     Gets a local runnable context from the specified virtual processor.
    /// </summary>
    bool WorkSearchContext::GetLocalRunnable(WorkItem *pWorkItem, VirtualProcessor *pVirtualProcessor, bool fSteal)
    {
        InternalContextBase *pContext = fSteal ? pVirtualProcessor->StealLocalRunnableContext() : pVirtualProcessor->GetLocalRunnableContext();
        if (pContext != NULL)
        {
            *pWorkItem = WorkItem(pContext);
            return true;
        }

        return false;
    }

    /// <summary>
    ///     Gets a runnable from the specified group.
    /// </summary>
    bool WorkSearchContext::GetRunnableContext(WorkItem *pWorkItem, ScheduleGroupBase *pGroup)
    {
        InternalContextBase *pContext = pGroup->GetRunnableContext();
        if (pContext != NULL)
        {
            *pWorkItem = WorkItem(pContext);
            return true;
        }

        return false;
    }

    //*************************************************************************** 
    //
    // Fair Searches
    //

    /// <summary>
    ///     Performs a fair search for runnables in the specified ring.
    /// </summary>
    bool WorkSearchContext::SearchFair_Runnables(WorkItem *pWorkItem, SchedulingRing *pRing)
    {
        int idx;
        ScheduleGroupBase *pGroup = pRing->GetPseudoRRScheduleGroup(&idx);

        int idxStart = idx;

        while (pGroup != NULL)
        {
            InternalContextBase *pContext = pGroup->GetRunnableContext();
            if (pContext != NULL)
            {
                pRing->SetPseudoRRScheduleGroupNext(idx);
                *pWorkItem = WorkItem(pContext);
                return true;
            }

            pGroup = pRing->GetNextScheduleGroup(&idx, idxStart);
        }

        return false;

    }

    /// <summary>
    ///     Performs a fair search for realized chores in the specified ring.
    /// </summary>
    bool WorkSearchContext::SearchFair_Realized(WorkItem *pWorkItem, SchedulingRing *pRing)
    {
        int idx;
        ScheduleGroupBase *pGroup = pRing->GetPseudoRRScheduleGroup(&idx);

        int idxStart = idx;

        while (pGroup != NULL)
        {
            RealizedChore *pRealizedChore = pGroup->GetRealizedChore();
            if (pRealizedChore != NULL)
            {
                pRing->SetPseudoRRScheduleGroupNext(idx);
                *pWorkItem = WorkItem(pRealizedChore, pGroup);
                return true;
            }

            pGroup = pRing->GetNextScheduleGroup(&idx, idxStart);
        }

        return false;

    }

    /// <summary>
    ///     Performs a fair search for unrealized chores in the specified ring.
    /// </summary>
    bool WorkSearchContext::SearchFair_Unrealized(WorkItem *pWorkItem, SchedulingRing *pRing)
    {
        int idx;
        ScheduleGroupBase *pGroup = pRing->GetPseudoRRScheduleGroup(&idx);

        int idxStart = idx;
        
        while (pGroup != NULL)
        {
            _UnrealizedChore *pUnrealizedChore = pGroup->StealUnrealizedChore();
            if (pUnrealizedChore != NULL)
            {
                pRing->SetPseudoRRScheduleGroupNext(idx);
                *pWorkItem = WorkItem(pUnrealizedChore, pGroup);
                return true;
            }

            pGroup = pRing->GetNextScheduleGroup(&idx, idxStart);
        }

        return false;

    }

    /// <summary>
    ///     Performs a fair search for work.
    /// </summary>
    bool WorkSearchContext::SearchFair(WorkItem *pWorkItem, ScheduleGroupBase *pOriginGroup, ULONG allowableTypes)
    {
        bool fFound = false;

        //
        // Do any up-front searching required for special circumstances (e.g.: UMS schedulers)
        //
        if (PreSearch(pWorkItem))
            return true;

        //
        // The fair search essentially round robins among scheduling rings and groups within a ring.
        // If you consider the search space as follows:
        //
        //               SR      SR     SR     SR
        // Contexts      ---------------------->
        // Realized      ---------------------->
        // Unrealized    ---------------------->
        //
        // fair scheduling will make horizontal slices through the search space to find work.
        //
        // Each entry in the above matrix can be viewed as:
        //
        // SG -> SG -> SG -> SG
        //
        // However, after finding work in a particular ring, fair will move onto the next ring in round-robin fashion.
        //

        //
        // At the top of each search, reset to the next ring in the round robin index.  This is simply the starting point for this search.
        //
        SchedulingRing *pCurrentRing;

        pCurrentRing = m_pCurrentRing = m_pScheduler->GetNextSchedulingRing();

        if (allowableTypes & WorkItem::WorkItemTypeContext)
        {
            SchedulingRing *pRing = pCurrentRing;
            while (pRing != NULL)
            {
                fFound = SearchFair_Runnables(pWorkItem, pRing);
                if (fFound)
                    break;
                
                pRing = m_pScheduler->GetNextSchedulingRing(pCurrentRing, pRing);
            }

            if (!fFound)
                fFound = StealForeignLocalRunnable(pWorkItem, m_pVirtualProcessor->GetOwningNode());
        }

        if (!fFound && (allowableTypes & WorkItem::WorkItemTypeRealizedChore))
        {
            SchedulingRing *pRing = pCurrentRing;
            while (pRing != NULL)
            {
                fFound = SearchFair_Realized(pWorkItem, pRing);
                if (fFound)
                    break;

                pRing = m_pScheduler->GetNextSchedulingRing(pCurrentRing, pRing);
            }
        }

        if (!fFound && (allowableTypes & WorkItem::WorkItemTypeUnrealizedChore))
        {
            SchedulingRing *pRing = pCurrentRing;
            while (pRing != NULL)
            {
                fFound = SearchFair_Unrealized(pWorkItem, pRing);
                if (fFound)
                    break;

                pRing = m_pScheduler->GetNextSchedulingRing(pCurrentRing, pRing);
            }
        }

        return fFound;
    }

    /// <summary>
    ///     Performs a fair search for work in the yielding case.
    /// </summary>
    bool WorkSearchContext::SearchFairYield(WorkItem *pWorkItem, ScheduleGroupBase *pOriginGroup, ULONG allowableTypes)
    {
        //
        // The yielding case slices identically to the regular case excepting that the search is done in a pseudo-reverse order
        //
        bool fFound = false;

        //
        // Do any up-front searching required for special circumstances (e.g.: UMS schedulers)
        //
        if (PreSearch(pWorkItem))
            return true;

        //
        // At the top of each search, reset to the next ring in the round robin index.  This is simply the starting point for this search.
        //
        SchedulingRing *pCurrentRing;

        pCurrentRing = m_pCurrentRing = m_pScheduler->GetNextSchedulingRing();

        if (allowableTypes & WorkItem::WorkItemTypeUnrealizedChore)
        {
            SchedulingRing *pRing = pCurrentRing;
            while (pRing != NULL)
            {
                fFound = SearchFair_Unrealized(pWorkItem, pRing);
                if (fFound)
                    break;

                pRing = m_pScheduler->GetNextSchedulingRing(pCurrentRing, pRing);
            }
        }


        if (!fFound && (allowableTypes & WorkItem::WorkItemTypeRealizedChore))
        {
            SchedulingRing *pRing = pCurrentRing;
            while (pRing != NULL)
            {
                fFound = SearchFair_Realized(pWorkItem, pRing);
                if (fFound)
                    break;

                pRing = m_pScheduler->GetNextSchedulingRing(pCurrentRing, pRing);
            }
        }

        if (!fFound && (allowableTypes & WorkItem::WorkItemTypeContext))
        {
            SchedulingRing *pRing = pCurrentRing;
            while (pRing != NULL)
            {
                fFound = SearchFair_Runnables(pWorkItem, pRing);
                if (fFound)
                    break;
                
                pRing = m_pScheduler->GetNextSchedulingRing(pCurrentRing, pRing);
            }

            if (!fFound)
                fFound = StealForeignLocalRunnable(pWorkItem, m_pVirtualProcessor->GetOwningNode());
        }

        return fFound;

    }

    //*************************************************************************** 
    //
    // Cache Local Searches
    //

    /// <summary>
    ///     Performs a cache local search for runnables in the specified ring.
    /// </summary>
    bool WorkSearchContext::SearchCacheLocal_Runnables(WorkItem *pWorkItem, SchedulingRing *pRing, ScheduleGroupBase *pOriginGroup)
    {
        int idx;
        ScheduleGroupBase *pGroup = pRing->GetPseudoRRScheduleGroup(&idx);

        int idxStart = idx;

        while (pGroup != NULL)
        {
            if (pGroup != pOriginGroup)
            {
                InternalContextBase *pContext = pGroup->GetRunnableContext();
                if (pContext != NULL)
                {
                    pRing->SetPseudoRRScheduleGroupNext(idx);
                    *pWorkItem = WorkItem(pContext);
                    return true;
                }
            }

            pGroup = pRing->GetNextScheduleGroup(&idx, idxStart);
        }

        return false;

    }

    /// <summary>
    ///     Performs a cache local search for realized chores in the specified ring.
    /// </summary>
    bool WorkSearchContext::SearchCacheLocal_Realized(WorkItem *pWorkItem, SchedulingRing *pRing, SchedulingRing *pStartingRing, ScheduleGroupBase *pOriginGroup)
    {
        //
        // At the outset of every search, bias to the origin group.
        //
        if (pRing == pStartingRing && pOriginGroup != NULL)
        {
            RealizedChore *pRealizedChore = pOriginGroup->GetRealizedChore();
            if (pRealizedChore != NULL)
            {
                *pWorkItem = WorkItem(pRealizedChore, pOriginGroup);
                return true;
            }
        }

        int idx;
        ScheduleGroupBase *pGroup = pRing->GetPseudoRRScheduleGroup(&idx);

        int idxStart = idx;

        while (pGroup != NULL)
        {
            if (pGroup != pOriginGroup)
            {
                RealizedChore *pRealizedChore = pGroup->GetRealizedChore();
                if (pRealizedChore != NULL)
                {
                    pRing->SetPseudoRRScheduleGroupNext(idx);
                    *pWorkItem = WorkItem(pRealizedChore, pGroup);
                    return true;
                }
            }

            pGroup = pRing->GetNextScheduleGroup(&idx, idxStart);
        }

        return false;

    }

    /// <summary>
    ///     Performs a cache local search for unrealized chores in the specified ring.
    /// </summary>
    bool WorkSearchContext::SearchCacheLocal_Unrealized(WorkItem *pWorkItem, SchedulingRing *pRing, SchedulingRing *pStartingRing, ScheduleGroupBase *pOriginGroup)
    {
        //
        // At the outset of every search, bias to the origin group.
        //
        if (pRing == pStartingRing && pOriginGroup != NULL)
        {
            _UnrealizedChore *pUnrealizedChore = pOriginGroup->StealUnrealizedChore();
            if (pUnrealizedChore != NULL)
            {
                *pWorkItem = WorkItem(pUnrealizedChore, pOriginGroup);
                return true;
            }
        }

        int idx;
        ScheduleGroupBase *pGroup = pRing->GetPseudoRRScheduleGroup(&idx);

        int idxStart = idx;

        while (pGroup != NULL)
        {
            if (pGroup != pOriginGroup)
            {
                _UnrealizedChore *pUnrealizedChore = pGroup->StealUnrealizedChore();
                if (pUnrealizedChore != NULL)
                {
                    pRing->SetPseudoRRScheduleGroupNext(idx);
                    *pWorkItem = WorkItem(pUnrealizedChore, pGroup);
                    return true;
                }
            }

            pGroup = pRing->GetNextScheduleGroup(&idx, idxStart);
        }

        return false;

    }


    /// <summary>
    ///     Performs a cache local search for work.
    /// </summary>
    bool WorkSearchContext::SearchCacheLocal(WorkItem *pWorkItem, ScheduleGroupBase *pOriginGroup, ULONG allowableTypes)
    {
        //
        // The cache local search biases work to the current schedule group, current scheduling ring, current scheduling node before looking elsewhere.
        // If you consider the search space as follows:
        //
        //               SR      SR     SR     SR
        // Contexts      |       |      |      | 
        // Realized      |       |      |      |
        // Unrealized    v       v      v      v
        //
        // cache local will make vertical slices through the search space to find work.
        //
        // Each entry in the above matrix can be viewed as:
        //
        // SG -> SG -> SG -> SG
        //
        // e.g.: cache local will look for runnables within the schedule groups of a given ring
        //       THEN realized chores within the schedule groups of a given ring
        //       THEN unrealized chores within the schedule groups of a given ring
        //       biasing such searches to the origin group where possible
        // 
        // The cache local algorithm is highly susceptible to livelock in certain scenarios.  Because there is a high degree of biasing of work due
        // to local runnable contexts and the tend to keep work "local", it is entirely possible that a series of yielders spin forever waiting on
        // work outside the bias.  This can happen in several ways:
        //
        // - The LRC can become deep enough that a dependency chain as follows is established:
        //       [OLD END] <critical> <yielder> <yielder> <yielder> [NEW END]
        //   A purely cache local search can spin on the yielders since the LRC is LIFO.
        //
        // - A dependency chain can be established between the LRC and the schedule group's list due to LRC overflow or the critical path becoming unblocked
        //   from an external context:
        //       LRC: <yielder> <yielder> <yielder>
        //       SG : <critical>
        //   A purely cache local search can spin on the yielders since the LRC is higher priority than the schedule group's list
        //
        // - A dependency chain can be established between two schedule groups within the same ring:
        //       SG1: <yielder> <yielder>
        //       SG2: <critical>
        //   A purely cache local search can spin on the yielders since the current schedule group is higher priority than other schedule groups
        //
        // - A dependency chain can be established between two schedule groups within different rings:
        //       R1 SG1: <yielder> <yielder>
        //       R2 SG2: <critical>
        //   A purely cache local search can spin on the yielders since the current scheduling ring is higher priority than other scheduling rings.
        //
        // In order to alleviate these circumstances and make forward progress, the cache local algorithm is altered to take a series of steps:
        //
        //     - If we continue to find "local" work (within the LRC or current schedule group) for N items, we will flip the direction of the LRC
        //       and access our LRC in FIFO order.  This is more expensive but it will eliminate a goodly portion of livelocks.
        //
        //     - If we continue to find "local" work for N+K items, we drop the LRC and current group bias and move on to another schedule group.  This
        //       resets the bias counters and begins again completely cache local on the new schedule group.
        //
        //     - If we have not transitioned rings in J items, we move to another scheduling ring.  This resets the bias counters and begins again
        //       completely cache local on the new scheduling ring.
        // 

        bool fFound = false;
        int count = 0;

        while (!fFound)
        {
            // The loop should not be exectued more than 2 times.
            count++;
            CORE_ASSERT(count <= 2);

            //
            // Do any up-front searching required for special circumstances (e.g.: UMS schedulers)
            //
            if (PreSearch(pWorkItem))
                return true;

            SchedulingRing *pCurrentRing = m_pCurrentRing;
            if (pCurrentRing == NULL)
                pCurrentRing = m_pStartingRing = m_pCurrentRing = m_pVirtualProcessor->GetOwningRing();

            SchedulingRing *pRing = pCurrentRing;

            // **************************************************
            // Initial Runnables Bias:
            //

            int bias = LocalBiasStage();
            CORE_ASSERT((count == 1) || (bias == 0));

            //
            // Cache local biases to the LRC, our own schedule group, and adjacent LRCs.  For runnables, this is always done at the top
            // of each search loop.
            //
            // Note that each of the Biased* functions will update appropriate biasing counters.
            //
            if ((allowableTypes & WorkItem::WorkItemTypeContext) && (bias < 3) && 
                    ((bias < 2 && GetLocalRunnable(pWorkItem, m_pVirtualProcessor, bias > 0)) || 
                     (GetRunnableContext(pWorkItem, pOriginGroup)) ||
                     (StealLocalRunnable(pWorkItem, m_pVirtualProcessor->GetOwningNode(), m_pVirtualProcessor))))
            {
                fFound = true;
            }
            else
            {
                // **************************************************
                // Scheduling Ring Slicing:
                //
                ScheduleGroupBase *pBiasedGroup = bias > 2 ? NULL : pOriginGroup;

                //
                // If we have gone so far along biasing to the current scheduling ring, move to the next ring and go back to being entirely cache local
                // on that particular scheduling ring.
                //
                if (DropRingBias())
                {
                    //
                    // We need to check the LRCs of the ring we're looking at to ensure forward progress (ahead of the normal runnables search).  When
                    // we inject a ring change, do this once right before the ring change.
                    //
                    SchedulingNode *pRingNode = pRing->GetOwningNode();
                    if (PerformOneTimeLRCScan())
                    {
                        fFound = StealLocalRunnable(pWorkItem, pRingNode, m_pVirtualProcessor);
                    }

                    if (fFound)
                    {
                        ResetLocalBias();
                    }
                    else
                    {
                        pRing = m_pScheduler->GetNextSchedulingRing(pCurrentRing, pRing);
                        ResetBias();
                    }
                }

                //
                // Slice through every scheduling ring.  The origin group will be skipped for runnables since the bias is above.  The bias for realized
                // and unrealized are in the appropriate search routines.  This is done to avoid code duplication with yield since the runnables bias is
                // inverted for yielding.
                //
                if (!fFound)
                {
                    while(pRing != NULL)
                    {
                        if (((allowableTypes & WorkItem::WorkItemTypeContext) && SearchCacheLocal_Runnables(pWorkItem, pRing, pBiasedGroup)) ||
                            ((allowableTypes & WorkItem::WorkItemTypeRealizedChore) && SearchCacheLocal_Realized(pWorkItem, pRing, pCurrentRing, pOriginGroup)) ||
                            ((allowableTypes & WorkItem::WorkItemTypeUnrealizedChore) && SearchCacheLocal_Unrealized(pWorkItem, pRing, pCurrentRing, pOriginGroup)) ||
                            ((allowableTypes & WorkItem::WorkItemTypeContext) && StealLocalRunnable(pWorkItem, pRing->GetOwningNode(), m_pVirtualProcessor)))
                        {
                            ResetLocalBias();
                            fFound = true;
                            break;
                        }

                        //
                        // If we ever move to another scheduling ring, we can completely drop any bias and go back to being completely cache local.
                        //
                        pRing = m_pScheduler->GetNextSchedulingRing(pCurrentRing, pRing);
                        ResetBias();
                    }
                }
            }

            if (!fFound)
            {
                //
                // If no work was found after a full loop, reset the ring at the top of the next search.  Note that if we cannot find work in the scheduler,
                // it is completely safe to reset any bias counts and go back to being completely cache local the next time we find work.
                //
                m_pCurrentRing = NULL;
                ResetBias();

                if (bias == 0)
                {
                    // We didn't find any work. There is no need to do another round of search since the bias didn't
                    // cause us to skip any queue.
                    break;
                }

                // When the bias is non-zero, we potentially skipped checking some of the queues (LRC, for example). If we didn't find
                // any work reset all the bias and do one more search loop.
            }
            else
            {
                Bias();
                m_pVirtualProcessor->UpdateRamblingState(m_pStartingRing != pRing, pRing);
                m_pCurrentRing = pRing;
            }
            
        }

        return fFound;
    }

    /// <summary>
    ///     Performs a cache local search for work in the yielding case.
    /// </summary>
    bool WorkSearchContext::SearchCacheLocalYield(WorkItem *pWorkItem, ScheduleGroupBase *pOriginGroup, ULONG allowableTypes)
    {
        //
        // The yielding case slices identically to the regular case excepting that the search is done in a pseudo-reverse order
        //
        bool fFound = false;

        //
        // Do any up-front searching required for special circumstances (e.g.: UMS schedulers)
        //
        if (PreSearch(pWorkItem))
            return true;

        SchedulingRing *pCurrentRing = m_pCurrentRing;

        if (m_pCurrentRing == NULL)
            pCurrentRing = m_pStartingRing = m_pCurrentRing = m_pVirtualProcessor->GetOwningRing();

        // **************************************************
        // Scheduling Ring Slicing:
        //

        SchedulingRing *pRing = pCurrentRing;
        ScheduleGroupBase *pBiasedGroup = pOriginGroup;

        //
        // If we have gone so far along biasing to the current scheduling ring, move to the next ring and go back to being entirely cache local
        // on that particular scheduling ring.
        //
        if (DropRingBias())
        {
            pRing = m_pScheduler->GetNextSchedulingRing(pCurrentRing, pRing);
            ResetBias();
        }

        while (pRing != NULL)
        {
            //
            // Recompute local bias on each ring transition
            //
            int bias = LocalBiasStage();

            if (((allowableTypes & WorkItem::WorkItemTypeUnrealizedChore) && SearchCacheLocal_Unrealized(pWorkItem, pRing, pCurrentRing, pBiasedGroup)) ||
                ((allowableTypes & WorkItem::WorkItemTypeRealizedChore) && SearchCacheLocal_Realized(pWorkItem, pRing, pCurrentRing, pBiasedGroup)) ||
                (bias < 1 && (allowableTypes & WorkItem::WorkItemTypeContext) && SearchCacheLocal_Runnables(pWorkItem, pRing, pBiasedGroup)) ||
                ((allowableTypes & WorkItem::WorkItemTypeContext) && StealLocalRunnable(pWorkItem, pRing->GetOwningNode(), m_pVirtualProcessor)))
            {
                fFound = true;
                break;
            }
            else if ((pRing == pCurrentRing) && (allowableTypes & WorkItem::WorkItemTypeContext))
            {
                // **************************************************
                // Local Bias:
                //

                if ((bias < 2 && GetRunnableContext(pWorkItem, pOriginGroup)) ||
                    (GetLocalRunnable(pWorkItem, m_pVirtualProcessor, bias > 2)))
                {
                    fFound = true;
                    break;
                }

            }

            //
            // If we ever move to another scheduling ring, we can completely drop any bias and go back to being completely cache local.
            //
            pRing = m_pScheduler->GetNextSchedulingRing(pCurrentRing, pRing);
            ResetBias();
        }

        if (fFound)
        {
            Bias();
            m_pCurrentRing = pRing;
        }
        else
        {
            //
            // If no work was found after a full loop, reset the ring at the top of the next search.  Note that if we cannot find work in the scheduler,
            // it is completely safe to reset any bias counts and go back to being completely cache local the next time we find work.
            //
            m_pCurrentRing = NULL;
            ResetBias();
        }

        return fFound;
    }
}
}
