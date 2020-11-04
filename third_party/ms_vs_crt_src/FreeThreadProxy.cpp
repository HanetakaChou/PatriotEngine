// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// FreeThreadProxy.cpp
//
// Part of the ConcRT Resource Manager -- this source file contains the internal definition for the free thread
// proxy.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Called in order to perform a cooperative context switch between one context and another.  After this call, pContext will
    ///     be running atop the virtual processor root and the context which was running will not.  What happens to the context that
    ///     was running depends on the value of the reason argument.
    /// </summary>
    /// <param name="pContext">
    ///     The context to cooperatively switch to.
    /// </param>
    /// <param name="switchState">
    ///     Indicates the state of the thread proxy that is executing the switch.  This can determine ownership of the underlying thread
    ///     proxy and context.
    /// </param>
    void FreeThreadProxy::SwitchTo(Concurrency::IExecutionContext *pContext, SwitchingProxyState switchState)
    {
        if (pContext == NULL)
            throw std::invalid_argument("pContext");

        // Find out if this context already has a thread proxy, if not, we have to request one from the factory.
        FreeThreadProxy * pProxy = static_cast<FreeThreadProxy *> (pContext->GetProxy());

        if (pProxy == NULL)
        {
            // Find a thread proxy from the pool that corresponds to the stack size and priority we need. Since this
            // is a context in the same scheduler as the current context's scheduler, we can use existing values of
            // stack size and priority.
            pProxy = static_cast<FreeThreadProxy *> (m_pRoot->GetSchedulerProxy()->GetNewThreadProxy(pContext));
        }

        // The 'next' thread proxy must be affinitized to the 'this' proxy's vproc root VPRoot1, BEFORE the blocked flag
        // is set. Not doing this could result in vproc root orphanage. See VirtualProcessorRoot::Affinitize for details.
        static_cast<FreeVirtualProcessorRoot *>(m_pRoot)->Affinitize(pProxy);

        m_pRoot = NULL;
        ASSERT(m_fBlocked == FALSE);

        switch (switchState)
        {
            case Blocking:
                //
                // Signal the other thread proxy and block until switched to, or until a virtual processor is activated with
                // the context running on this thread proxy.
                //
                InterlockedExchange(&m_fBlocked, TRUE);
                SignalObjectAndWait(pProxy->m_hBlock, m_hBlock, INFINITE, TRUE);
                ASSERT(m_fBlocked == TRUE);
                InterlockedExchange(&m_fBlocked, FALSE);

                break;
            case Nesting:
                //
                // Signal the other thread proxy that now owns this virtual processor, but do not block. The current thread proxy
                // is about to move to a nested scheduler.
                //
                ASSERT(pProxy->m_pRoot != NULL);
                ASSERT(pProxy->m_pContext != NULL);
                pProxy->ResumeExecution();

                break;
            case Idle:
                //
                // Return without blocking, indicating to the caller that the scheduler should yield this thread proxy
                // back to the RM, by exiting the contexts dispatch loop.
                //
                ASSERT(pProxy->m_pRoot != NULL);
                ASSERT(pProxy->m_pContext != NULL);
                pProxy->ResumeExecution();

                break;
            default:

                ASSERT(false);
                break;
        }
    }

    /// <summary>
    ///     Called in order to perform a cooperative context switch out.  After this call, the context which was running will be blocked
    ///     until it is switched to, or used to activate a virtual processor.
    /// </summary>
    void FreeThreadProxy::SwitchOut()
    {
        // A thread proxy that calls SwitchOut usually does not have a valid virtual processor root. Even it if is non-null,
        // it has probably been returned to the RM by the scheduler that owned it. NULL it out here, so we don't accidentally
        // assume its valid later.
        m_pRoot = NULL;

        ASSERT(m_fBlocked == 0);
        SuspendExecution();
    }

    /// <summary>
    ///     Called right after obtaining a thread proxy from the factory. Associates the thread proxy with the execution
    ///     context it is about to run.
    /// </summary>
    void FreeThreadProxy::AssociateExecutionContext(Concurrency::IExecutionContext * pContext)
    {
        m_pContext = pContext;
        pContext->SetProxy(this);
    }

    /// <summary>
    ///     Returns a thread proxy to the factory when it is no longer in use.
    /// </summary>
    void FreeThreadProxy::ReturnIdleProxy()
    {
        _ASSERTE(m_pFactory != NULL);
        m_pContext = NULL;
        m_pFactory->ReclaimProxy(this);
    }

    /// <summary>
    ///     The main dispatch loop for the free thread proxy.
    /// </summary>
    void FreeThreadProxy::Dispatch()
    {
        // Send the default dispatch state into Dispatch.
        DispatchState dispatchState;

        if (!m_fCanceled)
        {
            TlsSetValue(m_pFactory->GetExecutionResourceTls(), (LPVOID) (((size_t) this) | TlsResourceInProxy));
        }

        while (!m_fCanceled)
        {
            _ASSERTE(m_pContext != NULL);
            _ASSERTE(m_pRoot != NULL);

            // Call the dispatch loop of the registered context.
            m_pContext->SetProxy(this);
            m_pContext->Dispatch(&dispatchState);

            // The dispatch loop returns when the scheduler that the proxy was given to, has decided to return it back to the RM.
            // It should be returned to the free proxy factory, so that it can be handed out to a different virtual processor root
            // (bound to a different context).

            m_pContext = NULL;
            m_pRoot = NULL;

            // Return to the idle pool in the RM. If the pool is full, the proxy will be canceled.
            ReturnIdleProxy();

            // Wait on the Block event until signaled.
            SuspendExecution();
        }
    }
} // namespace details
} // namespace Concurrency
