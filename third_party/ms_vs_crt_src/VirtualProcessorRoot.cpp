// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// VirtualProcessorRoot.cpp
//
// Part of the ConcRT Resource Manager -- this file contains the internal implementation for the base virtual
// processor root.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    // The current unique identifier being handed out to created virtual processor roots.
    long VirtualProcessorRoot::s_currentId = 0;

    /// <summary>
    ///     Constructs a new virtual processor root.
    /// </summary>
    /// <param name="pSchedulerProxy">
    ///     The scheduler proxy this root is created for. A scheduler proxy holds RM data associated with an instance of
    ///     a scheduler.
    /// </param>
    /// <param name="pNode">
    ///     The processor node that this root belongs to. The processor node is one among the nodes allocated to the
    ///     scheduler proxy.
    /// </param>
    /// <param name="coreIndex">
    ///     The index into the array of cores for the processor node specified.
    /// </param>
    VirtualProcessorRoot::VirtualProcessorRoot(SchedulerProxy * pSchedulerProxy, SchedulerNode* pNode, unsigned int coreIndex) :
        m_executionResource(pSchedulerProxy, pNode, coreIndex),
        m_activationFence(0),
        m_fRemovedFromScheduler(false),
        m_fOversubscribed(false)
    {
        m_id = (unsigned int)InterlockedIncrement(&s_currentId);
        m_executionResource.MarkAsVirtualProcessorRoot(this);
    }

    /// <summary>
    ///     Called to indicate that a scheduler is done with a virtual processor root and wishes to return it to the resource manager.
    /// </summary>
    /// <param name="pScheduler">
    ///     The scheduler making the request to remove this virtual processor root.
    /// </param>
    void VirtualProcessorRoot::Remove(IScheduler *pScheduler)
    {
        if (pScheduler == NULL)
            throw std::invalid_argument("pScheduler");

        if (GetSchedulerProxy()->Scheduler() != pScheduler)
            throw invalid_operation();

        ResetSubscriptionLevel();

        // This particular call does not have to worry about the RM receiving a SchedulerShutdown for the scheduler proxy in question.
        GetSchedulerProxy()->DestroyVirtualProcessorRoot(this);
    }

    /// <summary>
    ///     This API is called when a virtual processor root is being destroyed. It removes the effect of this virtual processor root
    ///     on the subscription level for the underlying core.
    /// <summary>
    void VirtualProcessorRoot::ResetSubscriptionLevel()
    {
        // If the activation fence on this root is 1, the virtual processor root was in the activated state when
        // it was removed - we need to reduce the subscription level here.
        ASSERT(m_activationFence == 0 || m_activationFence == 1);
        if (m_activationFence == 1)
        {
            GetSchedulerProxy()->DecrementCoreSubscription(GetExecutionResource());
        }
    }

} // namespace details
} // namespace Concurrency
