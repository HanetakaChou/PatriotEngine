// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// FreeVirtualProcessorRoot.cpp
//
// Part of the ConcRT Resource Manager -- this header file contains the internal implementation for the free virtual
// processor root (represents a virtual processor as handed to a scheduler).
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{

    /// <summary>
    ///     Constructs a new free virtual processor root.
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
    FreeVirtualProcessorRoot::FreeVirtualProcessorRoot(SchedulerProxy *pSchedulerProxy, SchedulerNode* pNode, unsigned int coreIndex)
        : VirtualProcessorRoot(pSchedulerProxy, pNode, coreIndex),
        m_pExecutingProxy(NULL)
    {
    }

    /// <summary>
    ///     Causes the scheduler to start running a thread proxy on the specified virtual processor root which will execute
    ///     the Dispatch method of the context supplied by pContext. Alternatively, it can be used to resume a
    ///     virtual processor root that was de-activated by a previous call to Deactivate.
    /// </summary>
    /// <param name="pContext">
    ///     The context which will be dispatched on a (potentially) new thread running atop this virtual processor root.
    /// </param>
    void FreeVirtualProcessorRoot::Activate(Concurrency::IExecutionContext *pContext)
    {
        if (pContext == NULL)
            throw std::invalid_argument("pContext");

        FreeThreadProxy * pProxy = static_cast<FreeThreadProxy *> (pContext->GetProxy());

        if (m_pExecutingProxy != NULL)
        {
            // The root already has an associated thread proxy. Check that the context provided is associated with
            // the same proxy.
            if (pProxy != m_pExecutingProxy)
            {
                throw invalid_operation();
            }

            ASSERT(m_pExecutingProxy->GetVirtualProcessorRoot() == this);
        }
        else
        {
            if (pProxy == NULL)
            {
                pProxy = static_cast<FreeThreadProxy *> (GetSchedulerProxy()->GetNewThreadProxy(pContext));
            }

            // An activated root increases the subscription level on the underlying core. Future changes to the subscription
            // level are made in Deactivate (before and after blocking).
            GetSchedulerProxy()->IncrementCoreSubscription(GetExecutionResource());

            // Affinitization sets this as the executing proxy for the virtual processor root.
            Affinitize(pProxy);
        }

        ASSERT(m_pExecutingProxy == pProxy);
        ASSERT(pProxy->GetVirtualProcessorRoot() != NULL);
        ASSERT(pProxy->GetExecutionContext() != NULL);

        // All calls to Activate after the first one can potentially race with the paired deactivate. This is allowed by the API, and we use the fence below
        // to reduce kernel transitions in case of this race.
        LONG newVal = InterlockedIncrement(&m_activationFence);
        if (newVal == 2)
        {
            // We received two activations in a row. According to the contract with the client, this is allowed, but we should expect a deactivation
            // soon after. Simply return instead of signalling the event. The deactivation will reduce the count back to 1. In addition, we're not responsible
            // for changing the idle state on the core.
        }
        else
        {
            ASSERT(newVal == 1);
            m_pExecutingProxy->ResumeExecution();
        }
    }

    /// <summary>
    ///     Causes the thread proxy running atop this virtual processor root to temporarily stop dispatching pContext.
    /// </summary>
    /// <param name="pContext">
    ///     The context which should temporarily stop being dispatched by the thread proxy running atop this virtual processor root.
    /// </param>
    bool FreeVirtualProcessorRoot::Deactivate(Concurrency::IExecutionContext *pContext)
    {
        if (pContext == NULL)
            throw std::invalid_argument("pContext");

        if (m_pExecutingProxy == NULL)
            throw invalid_operation();

        FreeThreadProxy * pProxy = static_cast<FreeThreadProxy *> (pContext->GetProxy());

        if (m_pExecutingProxy != pProxy)
        {
            throw invalid_operation();
        }

        LONG newVal = InterlockedDecrement(&m_activationFence);

        if (newVal == 0)
        {
            // Reduce the subscription level on the core while the root is suspended. The count is used by dynamic resource management
            // to tell which cores allocated to a scheduler are unused, so that they can be temporarily repurposed.
            GetSchedulerProxy()->DecrementCoreSubscription(GetExecutionResource());
            m_pExecutingProxy->SuspendExecution();
            GetSchedulerProxy()->IncrementCoreSubscription(GetExecutionResource());
        }
        else
        {
            ASSERT(newVal == 1);
            // The activation for this deactivation came in early, so we return early here without making a kernel transition.
        }

        return true;
    }

    /// <summary>
    ///     Forces all data in the memory heirarchy of one processor to be visible to all other processors.
    /// </summary>
    /// <param name="pContext">
    ///     The context which is currently being dispatched by this root.
    /// </param>
    void FreeVirtualProcessorRoot::EnsureAllTasksVisible(Concurrency::IExecutionContext *pContext)
    {
        if (pContext == NULL)
            throw std::invalid_argument("pContext");

        if (m_pExecutingProxy == NULL)
            throw invalid_operation();

        FreeThreadProxy * pProxy = static_cast<FreeThreadProxy *> (pContext->GetProxy());

        if (m_pExecutingProxy != pProxy)
        {
            throw invalid_operation();
        }

        GetSchedulerProxy()->GetResourceManager()->FlushStoreBuffers();
    }

    /// <summary>
    ///     Called to affinitize the given thread proxy to this virtual processor.
    /// </summary>
    /// <param name="pThreadProxy">
    ///     The new thread proxy to run atop this virtual processor root.
    /// </param>
    void FreeVirtualProcessorRoot::Affinitize(FreeThreadProxy *pThreadProxy)
    {
        //
        // Wait until the thread proxy is firmly blocked. This is essential to prevent vproc root orphanage
        // if the thread proxy we're switching to is IN THE PROCESS of switching out to a different one. An example of how this
        // could happen:

        // 1] ctxA is running on vp1. It is in the process of blocking, and wants to switch to ctxB. This means ctxA's thread proxy
        // tpA must affintize ctxB's thread proxy tpB to its own vproc root, vproot1.

        // 2] At the exact same time, ctxA is unblocked by ctxY and put onto a runnables collection in its scheduler. Meanwhile, ctxZ
        // executing on vp2, has also decided to block. It picks ctxA off the runnables collection, and proceeds to switch to it.
        // This means that ctxZ's thread proxy tpZ must affinitize ctxA's thread proxy tpA to ITS vproc root vproot2.

        // 3] Now, if tpZ affintizes tpA to vproot2 BEFORE tpA has had a chance to affintize tpB to vproot1, tpB gets mistakenly
        // affintized to vproot2, and vproot1 is orphaned.

        // In order to prevent this, tpZ MUST wait until AFTER tpA has finished its affinitization. This is indicated via the 
        // blocked flag. tpA will set its blocked flag to 1, after it has finished affintizing tpB to vproot1, at which point it is
        // safe for tpZ to modify tpA's vproc root and change it from vproot1 to vproot2.
        //

        pThreadProxy->SpinUntilBlocked();

        m_pExecutingProxy = pThreadProxy;
        pThreadProxy->SetVirtualProcessorRoot(this);

        HardwareAffinity newAffinity = GetSchedulerProxy()->GetNodeAffinity(GetNodeId());
        pThreadProxy->SetAffinity(newAffinity);
    }

} // namespace details
} // namespace Concurrency
