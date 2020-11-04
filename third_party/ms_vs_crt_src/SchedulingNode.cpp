// ==++==
//
//	Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// SchedulingNode.cpp
//
// Source file containing the SchedulingNode implementation.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    SchedulingNode::SchedulingNode(SchedulingRing *pRing) :
        m_pRing(pRing),
        m_virtualProcessorAvailableCount(0),
        m_virtualProcessorCount(0), // needed for scheduling rings
        m_ramblingCount(0),
        m_virtualProcessors(pRing->m_pScheduler, 256, ListArray<VirtualProcessor>::DeletionThresholdInfinite)
    {
        m_pScheduler = m_pRing->m_pScheduler;
        m_id = m_pRing->Id();
    }

    SchedulingNode::~SchedulingNode()
    {
        Cleanup();
    }

    void SchedulingNode::Cleanup() 
    {
        //
        // Do not clean up m_pRing here, it is done at SchedulerBase::m_rings
        //

        // Cleanup of the virtual processors does not need to explicitly happen.  When
        // the destructor of the list array is called, it will internally delete
        // all of its elements
    }

    /// <summary>
    ///     Creates and adds a new virtual processor in the node to associated with the root provided.
    ///     NOTE: For non-oversubscribed vprocs this API is currently will only work for initial allocation.
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
    VirtualProcessor* SchedulingNode::AddVirtualProcessor(IVirtualProcessorRoot *pOwningRoot, bool fOversubscribed)
    {
        bool fThrottleOversubscribed = false;

        ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();

        // Try and grab a virtual processor from the free pool before creating a new one
        VirtualProcessor *pVirtualProcessor = m_virtualProcessors.PullFromFreePool();
        if (pVirtualProcessor == NULL)
        {
            pVirtualProcessor = m_pScheduler->CreateVirtualProcessor(this, pOwningRoot);
        }
        else
        {
            pVirtualProcessor->Initialize(this, pOwningRoot);
        }

        if (fOversubscribed)
        {
            ASSERT(pCurrentContext != NULL && !pCurrentContext->IsExternal());
            InternalContextBase * pOversubsbscribingContext = static_cast<InternalContextBase *>(pCurrentContext);

            pVirtualProcessor->m_fOversubscribed = true;
            pVirtualProcessor->m_pOversubscribingContext = pOversubsbscribingContext;

            // The oversubscribed vproc is fenced by adding it to the list array below.
            pOversubsbscribingContext ->SetOversubscribedVProc(pVirtualProcessor);

            //
            // If we have hit a throttling point, a background thread will make this virtual processor available.
            //
            fThrottleOversubscribed = (m_pScheduler->ShouldThrottleOversubscriber() && (m_pScheduler->ThrottlingTime(1) != 0));
            if (fThrottleOversubscribed)
                pVirtualProcessor->Hide(true);
        }

        // We increment the total count of virtual processors on the node since the rambling logic uses this count. 
        InterlockedIncrement(&m_virtualProcessorCount);

        // If no virtual processors are 'available' in the scheduler, try to start this one up right away, if not, make it available,
        // and increment the counts to indicate this. The only exception is the first virtual processor added as part of the initial
        // set of virtual processors.
        if (!fThrottleOversubscribed)
        {
            if ((m_pScheduler->m_virtualProcessorAvailableCount == 0) && (m_pScheduler->m_virtualProcessorCount > 0))
            {
                // The check above is not accurate, since the count may increase right after the check -> in the worst case, the virtual
                // processor is activated when it should've been left available.

                // We should only be activating virtual processors as they are added, if they are either oversubscribed or as a result
                // of core migration. The initial set of virtual processors should never be activated here.
                ASSERT(pCurrentContext == NULL || fOversubscribed);

                if (InterlockedExchange(&pVirtualProcessor->m_fAvailable, FALSE) == TRUE)
                {
                    // The vproc should be added to the list array only after it is fully initialized. If this is an oversubscribed vproc,
                    // we need to synchronize with a concurrent RemoveCore, which assumes it can party on the vproc if it is found in the list
                    // array.
                    m_virtualProcessors.Add(pVirtualProcessor);

                    // Activation of a virtual processor synchronizes with finalization. If the scheduler is in the middle of finalization
                    // or has already shutdown, the API will return false.
                    bool activated = m_pScheduler->VirtualProcessorActive(true);

                    if (activated)
                    {
                        ScheduleGroupBase * pGroup = (pCurrentContext != NULL) ? pCurrentContext->GetScheduleGroup() : m_pRing->GetAnonymousScheduleGroup();
                        pVirtualProcessor->StartupWorkerContext(pGroup);
                    }
                    else
                    {
                        // We do nothing here since the scheduler is shutting down/has shutdown. The virtual processor remains unavailable,
                        // and since we didn't increment availabe counts, we don't have to decrement them.
                    }
                }
                else
                {
                    // The virtual processor has not been added to the list of virtual processors, therefore, we expect it to be available.
                    ASSERT(false);
                }
            }
            else
            {
                // Increment the counts on the scheduling node and the scheduler before inserting the virtual processor in the list within the node
                // else the vproc available count could potentially go < 0 (4 billion) temporarily, and this may create issues.
                InterlockedIncrement(&m_virtualProcessorAvailableCount);
                InterlockedIncrement(&m_pScheduler->m_virtualProcessorAvailableCount);

                // The vproc should be added to the list array only after it is fully initialized. If this is an oversubscribed vproc,
                // we need to synchronize with a concurrent RemoveVirtualProcessor, which assumes it can party on the vproc if it is
                // found in the list array.
                m_virtualProcessors.Add(pVirtualProcessor);
            }
        }
        else
        {
            //
            // Do not increment the available counts.  This virtual processor is "special" until the background thread says it can wake up.
            //
            m_virtualProcessors.Add(pVirtualProcessor);
            m_pScheduler->ThrottleOversubscriber(pVirtualProcessor);
        }

        return pVirtualProcessor;
    }

    /// <summary>
    ///     Find the virtual processor in this node that matches the root provided.
    /// </summary>
    /// <param name="pRoot">
    ///     The virtual processor root to match.
    /// </param>
    /// <remarks>
    ///     IMPORTANT: This API is only called while removing virtual processors via IScheduler::RemoveVirtualProcessors.
    ///     If this functionality is needed at other call sites in the future, the implementation may need to be
    ///     reevaluated (see comments below).
    /// </remarks>
    VirtualProcessor* SchedulingNode::FindMatchingVirtualProcessor(IVirtualProcessorRoot* pRoot)
    {
        int arraySize = m_virtualProcessors.MaxIndex();

        for (int i = 0; i < arraySize; i++)
        {
            VirtualProcessor *pVirtualProcessor = m_virtualProcessors[i];

            // It is ok to test the owning root here without a lock. If the owning root matches what we're looking for,
            // we are guarannteed it will not change (by way of the virtual processor being retired and reused). This is because
            // the call to IVirtualProcessorRoot::Remove in the virtual processor retirement code path is serialized in the RM
            // before or after the call to IScheduler::RemoveVirtualProcessors. i.e. if we find an owning root that matches, the retirement
            // path is unable to set it to NULL until after we're done.
            if ((pVirtualProcessor != NULL) && (pVirtualProcessor->m_pOwningRoot == pRoot))
            {
                return pVirtualProcessor;
            }
        }

        return NULL;
    }

    InternalContextBase *SchedulingNode::StealLocalRunnableContext(VirtualProcessor* pSkipVirtualProcessor)
    {
        InternalContextBase *pContext = NULL;
        int skipIndex, startIndex;
        int arraySize = m_virtualProcessors.MaxIndex();

        if (pSkipVirtualProcessor != NULL)
        {
            skipIndex = pSkipVirtualProcessor->m_listArrayIndex;
            startIndex = 1;
        }
        else
        {
            skipIndex = 0;
            startIndex = 0;
        }

        for (int i = startIndex; i < arraySize; i++)
        {
            int index = (i + skipIndex) % arraySize;
            VirtualProcessor *pVirtualProcessor = m_virtualProcessors[index];
            if (pVirtualProcessor == NULL)
            {
                continue;
            }
#if _UMSTRACE
            ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();
            CMTRACE(MTRACE_EVT_SEARCHEDLOCALRUNNABLES, (pCurrentContext && !pCurrentContext->IsExternal()) ? static_cast<InternalContextBase *>(pCurrentContext) : NULL, pVirtualProcessor, pSkipVirtualProcessor);
#endif // _UMSTRACE
            
            pContext = pVirtualProcessor->m_localRunnableContexts.Steal();
            if (pContext != NULL)
            {
#if defined(_DEBUG)
                pContext->SetDebugBits(CTX_DEBUGBIT_STOLENFROMLOCALRUNNABLECONTEXTS);
#endif // _DEBUG

                break;
            }
        }
        return pContext;
    }

    /// <summary>
    ///     Find an available virtual processor in the scheduling node. We claim ownership of the virtual
    ///     processor and return it.
    /// </summary>
    VirtualProcessor* SchedulingNode::FindAvailableVirtualProcessor(VirtualProcessor *pBias)
    {
        if (pBias != NULL && !pBias->IsHidden() && pBias->ClaimExclusiveOwnership())
            return pBias;

        // The callers of this API MUST check that that the available virtual processor count in the scheduling node
        // is non-zero before calling the API. We avoid putting that check here since it would evaluate to false
        // most of the time, and it saves the function call overhead on fast paths (chore push)

        for (int i = 0; i < m_virtualProcessors.MaxIndex(); i++)
        {
            VirtualProcessor *pVirtualProcessor = m_virtualProcessors[i];

            if (pVirtualProcessor != NULL && !pVirtualProcessor->IsHidden() && pVirtualProcessor->ClaimExclusiveOwnership())
                return pVirtualProcessor;
        }

        return NULL;
    }

    //
    /// When all vprocs are rambling (searching foreign nodes for work), find the ring with the most vprocs working on it and set that to
    /// the owning ring for all vprocs.
    //
    void SchedulingNode::CheckForNewOwningRing()
    {
        const int reasonableNodeCount = 64;
        int nodeWeights[reasonableNodeCount];
        int *pNodeWeights;
        if (m_pScheduler->m_nodeCount > reasonableNodeCount)
            pNodeWeights = (int*) _alloca(m_pScheduler->m_nodeCount * sizeof(int));
        else
            pNodeWeights = nodeWeights;
        memset(nodeWeights, 0, sizeof(nodeWeights));
        int maxWeight = 0, maxWeightIdx = 0;

        for (int i = 0; i < m_virtualProcessors.MaxIndex(); i++)
        {
            VirtualProcessor *pVirtualProcessor = m_virtualProcessors[i];
            if (pVirtualProcessor != NULL) {
                int &weight = nodeWeights[pVirtualProcessor->m_pCurrentRing->Id()];
                ++weight;
                if (weight > maxWeight)
                {
                    maxWeight = weight;
                    maxWeightIdx = pVirtualProcessor->m_pCurrentRing->Id();
                }
            }
        }

        //
        // change the owning ring to the one with the most vprocs working on it
        //
        for (int i = 0; i < m_virtualProcessors.MaxIndex(); i++)
        {
            VirtualProcessor *pVirtualProcessor = m_virtualProcessors[i];
            if (pVirtualProcessor != NULL)
            {
#if _UMSTRACE
                InternalContextBase *pCurrentContext = static_cast<InternalContextBase *>(SchedulerBase::FastCurrentContext());
                CMTRACE(MTRACE_EVT_UPDATERAMBLING_ALLVPROCS, pCurrentContext, pVirtualProcessor, m_pScheduler->m_rings[maxWeightIdx]);
#endif // _UMSTRACE

                pVirtualProcessor->m_pOwningRing = m_pScheduler->m_rings[maxWeightIdx];

                // m_fRambling is set to false on multiple paths. Decrement the count only if this path
                // successfully sets it to false
                if (InterlockedExchange(&pVirtualProcessor->m_fRambling, FALSE) == TRUE)
                    InterlockedDecrement(&m_ramblingCount);
            }
        }
    }
} // namespace details
} // namespace Concurrency
