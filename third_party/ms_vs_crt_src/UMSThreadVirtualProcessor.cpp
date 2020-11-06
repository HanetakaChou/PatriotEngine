// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// UMSThreadVirtualProcessor.cpp
//
// Source file containing the UMSThreadVirtualProcessor implementation.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Construct an UMS virtual processor
    /// </summary>
    UMSThreadVirtualProcessor::UMSThreadVirtualProcessor(SchedulingNode *pOwningNode, IVirtualProcessorRoot *pOwningRoot) :
        m_pSchedulingContext(NULL)
    {
        // Defer to Initialize 
        Initialize(pOwningNode, pOwningRoot);
    }

    /// <summary>
    ///     Destroy the UMS virtual processor
    /// </summary>
    UMSThreadVirtualProcessor::~UMSThreadVirtualProcessor() 
    {
        delete m_pSchedulingContext;
    }

    /// <summary>
    ///     Initializes the virtual processor. This API is called by the constructor, and when a virtual processor is to
    ///     be re-initialized, when it is pulled of the free pool in the list array.
    /// </summary>
    /// <param name="pOwningNode">
    ///     The owning schedule node for this virtual processor
    /// </param>
    /// <param name="pOwningRoot"> 
    ///     The owning IVirtualProcessorRoot
    /// </param>
    void UMSThreadVirtualProcessor::Initialize(SchedulingNode *pOwningNode, IVirtualProcessorRoot *pOwningRoot)
    {
        // Initialize the base class
        VirtualProcessor::Initialize(pOwningNode, pOwningRoot);

        m_pCriticalContext = NULL;
        m_fCriticalIsReady = FALSE;

        //
        // Much as I'd love to defer these to StartupWorkerContext, it cannot be.  That can be called from moving the completion list
        // to runnables which *MUST* be inside a hyper-critical region to avoid deadlock in the case of blocking between grabbing
        // the list and putting it on the runnables.
        //
        SchedulerBase *pScheduler = m_pOwningNode->GetScheduler();

        if (m_pSchedulingContext != NULL)
            delete m_pSchedulingContext;

        m_pSchedulingContext = new UMSSchedulingContext(static_cast<UMSThreadScheduler *>(pScheduler), this);
    }

    /// <summary>
    ///     Start a worker context executing on this.virtual processor.
    /// </summary>
    void UMSThreadVirtualProcessor::StartupWorkerContext(ScheduleGroupBase* pGroup)
    {
        TRACE(TRACE_SCHEDULER, L"UMSThreadVirtualProcessor::StartupWorkerContext");

        //
        // This virtual processor might already have a context attached to it from the Dispatch loop, where it is waiting for work
        // to come in. If this is the case, there is no need to spin off another internal context to do the work.
        //
        // If it does not -- there is a subtle difference between UMS utilization of the RM and Win32 thread utilization of the RM.
        // The first context which we pass to the root to activate with is the scheduling context.  It is a special context
        // which cannot run work.  When a UMS thread blocks, the scheduling context is what actively gets switched back to.
        //
        if (m_pExecutingContext == NULL)
        {
            m_pStartingGroup = pGroup;

            VMTRACE(MTRACE_EVT_ACTIVATE, m_pSchedulingContext, this, SchedulerBase::FastCurrentContext());
#if _UMSTRACE
            ContextBase *pCurrentContext = SchedulerBase::FastCurrentContext();
            CMTRACE(MTRACE_EVT_ACTIVATE, (pCurrentContext && !pCurrentContext->IsExternal()) ? static_cast<InternalContextBase *>(pCurrentContext) : NULL, this, m_pSchedulingContext);
#endif // _UMSTRACE

            m_pOwningRoot->Activate(m_pSchedulingContext);
        }
        else
        {
            Activate(m_pExecutingContext);
        }
    }



    /// <summary>
    ///     Notification that a critically blocked context has come off the completion list.
    /// </summary>
    void UMSThreadVirtualProcessor::CriticalNotify()
    {
        InterlockedExchange(&m_fCriticalIsReady, TRUE);
        AttemptWake();
    }

    /// <summary>
    ///     Affinitizes an internal context to the virtual processor.
    /// </summary>
    /// <param name="pContext">
    ///     The internal context to affinitize.
    /// </param>
    void UMSThreadVirtualProcessor::Affinitize(InternalContextBase *pContext)
    {
        if (pContext != NULL)
        {
            VirtualProcessor::Affinitize(pContext);
        }
        else
        {
            //
            // We want to switch to the scheduling Context. Setting
            // executingContext to NULL will accomplish that.
            //
            m_pExecutingContext = NULL;
        }
    }

    /// <summary>
    ///     Attempts to wake the virtual processor due to a notification coming in that is critical to progress on
    ///     the virtual processor (e.g.:  a critically blocked context coming back on the completion list from
    ///     another virtual processor).
    /// </summary>
    void UMSThreadVirtualProcessor::AttemptWake()
    {
        if (ClaimExclusiveOwnership())
        {
            CORE_ASSERT(m_pExecutingContext != NULL);
            Activate(m_pExecutingContext);
        }
    }

    /// <summary>
    ///     Makes a virtual processor available for scheduling work.  This can only be called from the scheduling
    ///     context. 
    /// </summary>
    void UMSThreadVirtualProcessor::MakeAvailableFromSchedulingContext()
    {
        //
        // This makes the virtual processor available for wake/sleep *FROM* the scheduling context.
        //
        m_pExecutingContext = m_pSchedulingContext;
        MakeAvailable();
    }

    /// <summary>
    ///     Indicates if a virtual processor is deactivated waiting for a SFW context
    /// </summary>
    bool UMSThreadVirtualProcessor::IsWaitingForReservedContext()
    {
        // If the deactivate call came from scheduling context, then we are
        // waiting for an SFW context.
        return (m_pExecutingContext == m_pSchedulingContext);
    }

    /// <summary>
    ///     Returns a type-cast of pContext to an InternalContextBase or NULL if it is not.
    /// </summary>
    InternalContextBase *UMSThreadVirtualProcessor::ToInternalContext(IExecutionContext *pContext)
    {
        if (pContext == m_pSchedulingContext)
            return NULL;
        else
            return static_cast<InternalContextBase *>(pContext);
    }

    /// <summary>
    ///     Stub called in SFW before we search for runnable contexts.
    /// </summary>
    /// <returns>
    ///     A context which should be run.
    /// </returns>
    InternalContextBase *UMSThreadVirtualProcessor::PreRunnableSearch()
    {
        UMSThreadScheduler *pScheduler = static_cast<UMSThreadScheduler *>(m_pOwningNode->GetScheduler());
        pScheduler->MoveCompletionListToRunnables(this);

        return NULL;
    }

} // namespace details
} // namespace Concurrency

