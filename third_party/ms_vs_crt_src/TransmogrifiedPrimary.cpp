// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// TransmogrifiedPrimary.cpp
//
// A very special primary thread whose sole purpose is to "virtually" transmogrify a UMS thread into an NT thread
// for the purposes of scheduler nesting.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Consturct a transmogrified primary
    /// </summary>
    TransmogrifiedPrimary::TransmogrifiedPrimary() :
        m_pBoundProxy(NULL),
        m_hBlock(NULL),
        m_hComplete(NULL),
        m_queueCount(0),
        m_fCanceled(false)
    {
        Initialize();

        m_hComplete = CreateEventW(NULL, FALSE, TRUE, NULL);
        if (m_hComplete == NULL)
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
    }

    /// <summary>
    ///     Initialize the transmogrified primary.
    /// </summary>
    void TransmogrifiedPrimary::Initialize()
    {
        if (!UMS::CreateUmsCompletionList(&m_pCompletionList))
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));

        m_hBlock = CreateEventW(NULL, FALSE, FALSE, NULL);
        if (m_hBlock == NULL)
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));

        m_hPrimary = LoadLibraryAndCreateThread(NULL,
                                  0,
                                  PrimaryMain,
                                  this,
                                  0,
                                  &m_primaryId);
    }


    /// <summary>
    ///     Destroys the transmogrified primary.
    /// </summary>
    TransmogrifiedPrimary::~TransmogrifiedPrimary()
    {
        CloseHandle(m_hPrimary);
        if (m_hComplete != NULL)
            CloseHandle(m_hComplete);

        if (m_hBlock != NULL)
        {
            CloseHandle(m_hBlock);
        }

        UMS::DeleteUmsCompletionList(m_pCompletionList);
    }

    /// <summary>
    ///     Queues a thread to execute to completion and asynchronously returns.  It is not safe to utilize this method and ExecuteToCompletion
    ///     simultaneously.
    /// </summary>
    /// <param name="pProxy">
    ///     The proxy to queue to completion.
    /// </param>
    void TransmogrifiedPrimary::QueueToCompletion(UMSThreadProxy *pProxy)
    {
        // We need to use a hypercritical lock here since
        // this is called from the primary as well.
        UMSThreadProxy *pCurrentProxy = UMSThreadProxy::GetCurrent();
        if (pCurrentProxy != NULL)
        {
            pCurrentProxy->EnterHyperCriticalRegion();
        }

        m_queuedExecutions.Enqueue(pProxy);

        if (pCurrentProxy != NULL)
        {
            pCurrentProxy->ExitHyperCriticalRegion();
        }

        if (InterlockedIncrement(&m_queueCount) == 1)
        {
            WaitForCompletion();
            Unblock();
        }
    }

    /// <summary>
    ///     The UMS primary function.  This is invoked when the primary switches into UMS scheduling mode or whenever a given
    ///     context blocks or yields.
    /// </summary>
    /// <param name="reason">
    ///     The reason for the UMS invocation.
    /// </param>
    /// <param name="activationPayload">
    ///     The activation payload (depends on reason)
    /// </param>
    /// <param name="pData">
    ///     The context (the primary pointer)
    /// </param>
    void NTAPI TransmogrifiedPrimary::PrimaryInvocation(UMS_SCHEDULER_REASON reason, ULONG_PTR activationPayload, PVOID pData)
    {
        TransmogrifiedPrimary *pRoot = NULL;
        PUMS_CONTEXT pPrimaryContext = UMS::GetCurrentUmsThread();

        if (reason != UmsSchedulerStartup)
        {
            //
            // activationPayload and pData might be NULL (blocking), so we're left with storing the TransmogrifiedPrimary in either
            // TLS or the UMS context (the primary does have one).  At present, it's in the UMS context.
            //
            if (!UMS::QueryUmsThreadInformation(pPrimaryContext, UmsThreadUserContext, &pRoot, sizeof(pRoot), NULL))
                throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
        }
        else
        {
            pRoot = reinterpret_cast<TransmogrifiedPrimary *>(pData);

            if (!UMS::SetUmsThreadInformation(pPrimaryContext, UmsThreadUserContext, &pRoot, sizeof(pRoot)))
                throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
        }

        UMSThreadProxy *pProxy = NULL;

        switch(reason)
        {
            case UmsSchedulerStartup:
            {
                WaitForSingleObject(pRoot->m_hBlock, INFINITE);
                if (pRoot->m_fCanceled)
                {
                    CORE_ASSERT(pRoot->m_queueCount == 0);
                    return;
                }

                pRoot->Execute(pRoot->BindNewProxy());
                CORE_ASSERT(false);

                break;
            }

            case UmsSchedulerThreadBlocked:
            {
                pProxy = pRoot->HandleBlocking();

                if (pProxy == NULL)
                {
                    //
                    // No work was found. We are done
                    //
                    return;
                }

                pRoot->Execute(pProxy);

                CORE_ASSERT(false);
                break;
            }
            case UmsSchedulerThreadYield:
            {
                pProxy = pRoot->HandleYielding();

                if (pProxy == NULL)
                {
                    //
                    // No work was found.  We are done.
                    //
                    return;
                }

                pRoot->Execute(pProxy);

                CORE_ASSERT(false);
                break;
            }
            default:
                CORE_ASSERT(false);
                break;
        }
    }

    /// <summary>
    ///     Handle blocking for an UT on this primary
    /// </summary>
    UMSThreadProxy * TransmogrifiedPrimary::HandleBlocking()
    {
        UMSThreadProxy *pProxy = m_pBoundProxy;
        CORE_ASSERT(pProxy != NULL);

        //
        // Wait for the blocked thread to complete
        //
        PUMS_CONTEXT pUMSContext = NULL;
        if (!UMS::DequeueUmsCompletionListItems(m_pCompletionList, INFINITE, &pUMSContext))
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));

        //
        // The completed thread should be the one we are running
        //
        UMSThreadProxy *pCompletedProxy = UMSThreadProxy::FromUMSContext(pUMSContext);
        CORE_ASSERT(pCompletedProxy == pProxy && UMS::GetNextUmsListItem(pUMSContext) == NULL);

        //
        // If the thread terminated, either someone did an ExitThread or the thread we meant to run to completion did.  If so, block.
        //
        if (pProxy->IsTerminated())
        {
            //
            // This is the **FIRST** place it's safe to delete the proxy and move on. 
            //
            delete pProxy;
            m_pBoundProxy = NULL;

            // 
            // Search for new work
            //
            return SearchForWork();
        } 
        else
        {
            //
            // proxy has not run to completion yet.
            //
            return pProxy;
        }
    }

    /// <summary>
    ///     Handle yielding for an UT on this primary
    /// </summary>
    UMSThreadProxy * TransmogrifiedPrimary::HandleYielding()
    {
        UMSThreadProxy *pProxy = m_pBoundProxy;
        CORE_ASSERT(pProxy != NULL);

        switch(pProxy->m_yieldAction)
        {
            case UMSThreadProxy::ActionStartup:
            {
                //
                // UT startup
                //
                UMSFreeThreadProxy * pStartedProxy = static_cast<UMSFreeThreadProxy *>(pProxy);
                pStartedProxy->m_yieldAction = UMSThreadProxy::ActionNone;
                SetEvent(pStartedProxy->m_hBlock);
                break;
            }

            default:
            {
                //
                // When the thread explicity yields, it's blocked as far as we're concerned and someone else can run it.  This would be the case
                // on an exit from nesting.
                //
                pProxy->NotifyBlocked(false);
                break;
            }
        };

        m_pBoundProxy = NULL;
        return SearchForWork();
    }

    /// <summary>
    ///     Execute the given proxy on this primary
    /// </summary>
    /// <param name="pProxy">
    ///     The proxy to execute
    /// </param>
    void TransmogrifiedPrimary::Execute(UMSThreadProxy *pProxy)
    {
        CORE_ASSERT(pProxy != NULL);       

        m_pBoundProxy = pProxy;

        for(;;)
        {
            UMS::ExecuteUmsThread(pProxy->GetUMSContext());
            CORE_ASSERT(!pProxy->IsTerminated());
            Sleep(0);
        }
    }
    /// <summary>
    ///     Search for work queued in the case of multiple binding
    /// </summary>
    UMSThreadProxy * TransmogrifiedPrimary::SearchForWork()
    {
        CORE_ASSERT(m_pBoundProxy == NULL);

        //
        // This decrement is for the PREVIOUSLY executed work item.
        //
        LONG queueCount = InterlockedDecrement(&m_queueCount);
        if (queueCount == 0)
        {
            SetEvent(m_hComplete);
            CompletedTransmogrification();
            WaitForSingleObject(m_hBlock, INFINITE);
            if (m_fCanceled)
            {
                CORE_ASSERT(m_queueCount == 0);
                return NULL;
            }
        }

        //
        // Dequeue new work and bind it to the primary
        //
        return BindNewProxy();
    }

    /// <summary>
    ///     Binds a new proxy to the transmogrified primary and returns it.
    /// </summary>
    UMSThreadProxy *TransmogrifiedPrimary::BindNewProxy()
    {
        m_pBoundProxy = m_queuedExecutions.Dequeue();
        CORE_ASSERT(m_pBoundProxy != NULL);
        return m_pBoundProxy;
    }

    /// <summary>
    ///     The primary thread for this transmogrified primary.
    /// </summary>
    /// <param name="pContext">
    ///     The TransmogrifiedPrimary that this thread manages.
    /// </param>
    DWORD CALLBACK TransmogrifiedPrimary::PrimaryMain(LPVOID pContext)
    {
        TransmogrifiedPrimary *pPrimary = reinterpret_cast<TransmogrifiedPrimary *>(pContext);
        UMS_SCHEDULER_STARTUP_INFO si;

        si.UmsVersion = UMS_VERSION;
        si.CompletionList = pPrimary->m_pCompletionList;
        si.SchedulerProc = (PUMS_SCHEDULER_ENTRY_POINT) &PrimaryInvocation;
        si.SchedulerParam = pContext;

        if (!UMS::EnterUmsSchedulingMode(&si))
            throw new scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));

        delete pPrimary;
        FreeLibraryAndDestroyThread(0);
        return 0;
    }

    /// <summary>
    ///     Construct a new cached transmogrified primary.  The primary will be placed back upon the cache of the specified transmogrificator
    ///     at the end of each transmogrification.
    /// </summary>
    CachedTransmogrifiedPrimary::CachedTransmogrifiedPrimary(Transmogrificator *pTransmogrificator) :
        m_pTransmogrificator(pTransmogrificator)
    {
    }

    /// <summary>
    ///     Called when a transmogrification is complete.
    /// </summary>
    void CachedTransmogrifiedPrimary::CompletedTransmogrification()
    {
        m_pTransmogrificator->ReturnToCache(this);
    }


} // namespace details
} // namespace Concurrency
