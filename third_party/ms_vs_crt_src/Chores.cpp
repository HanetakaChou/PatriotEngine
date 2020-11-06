// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// Chores.cpp
//
// Miscellaneous implementations of things related to individuals chores
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Sets the attachment state of the chore at the time of stealing.
    /// </summary>
    void _UnrealizedChore::_SetDetached(bool _FDetached)
    {
        _M_fDetached = _FDetached;
    }

    /// <summary>
    ///     To free memory allocated with _InternalAlloc.
    /// </summary>
    void _UnrealizedChore::_InternalFree(_UnrealizedChore * _PChore)
    {
        if (_PChore->_M_fRuntimeOwnsLifetime)
        {
            delete _PChore;
        }
    }

    /// <summary>
    ///     Place associated task collection in a safe state.
    /// </summary>
    void _UnrealizedChore::_CheckTaskCollection()
    {
        //
        // If _M_pTaskCollection is non-NULL, the chore is still scheduled to a task collection.  This is only happening
        // from a handle destructor and we have blown back through a stack based handle while it's still scheduled.  We must
        // wait.  The semantic we choose is that this means cancellation too.
        //
        Concurrency::details::_TaskCollectionBase *pBase = _M_pTaskCollection;
        if (pBase != NULL)
        {
            bool fThrow = false;

            if (_M_pChoreFunction == &_UnrealizedChore::_StructuredChoreWrapper)
            {
                _StructuredTaskCollection *pTaskCollection = static_cast<_StructuredTaskCollection *>(pBase);
                fThrow = !pTaskCollection->_TaskCleanup();
            }
            else
            {
                _TaskCollection *pTaskCollection = static_cast<_TaskCollection *>(pBase);
                fThrow = !pTaskCollection->_TaskCleanup(true);
            }

            if (fThrow)
                throw missing_wait();
        }
    }

    /// <summary>
    ///     Wrapper around execution of a structured chore that performs appropriate notification
    ///     and exception handling semantics.
    /// </summary>
    __declspec(noinline)
    void __cdecl _UnrealizedChore::_StructuredChoreWrapper(_UnrealizedChore * pChore)
    {
        InternalContextBase *pContext = static_cast<InternalContextBase *> (SchedulerBase::FastCurrentContext());
        ASSERT(!pContext->IsCanceled());

        _StructuredTaskCollection *pTaskCollection = static_cast<_StructuredTaskCollection *> (pChore->_M_pTaskCollection);
        ContextBase *pOriginContext = reinterpret_cast <ContextBase *> (pTaskCollection->_M_pOwningContext);

        pContext->SetRootCollection(pTaskCollection);

        pOriginContext->AddStealer(pContext, false);

        try
        {
            //
            // We need to consider this a possible interruption point.  It's entirely possible that we stole and raced with a
            // cancellation thread.  The collection was canceled after we stole(e.g.: removed from the WSQ) but before we added ourselves
            // to the stealing chain list above.  In this case, the entire context will wait until completion (bad).  Immediately
            // after we go on the list (a memory barrier) we need to check the collection cancelation flag.  If the collection is going away,
            // we need to get out *NOW* otherwise the entire subtree executes.
            //
            if (pTaskCollection->_IsAbnormalExit())
                throw task_canceled();

            pChore->m_pFunction(pChore);
        }
        catch(const task_canceled &)
        {
            //
            // If someone manually threw the task_canceled exception, we will have a cancel count but not a canceled context.  This
            // means we need to apply the cancel one level up.  Normally, the act of throwing would do that via being caught in the
            // wait, but this is special "marshaling" for task_canceled.
            //
            if (pContext->IsCanceled() && !pContext->IsCanceledContext())
                pTaskCollection->_Cancel();
        }
        catch(...)
        {
            //
            // Track the exception that was thrown here.  _RaisedException makes the decision on what
            // exceptions to keep and what to discard.  The flags it sets will indicate to the thread calling ::Wait that it must rethrow.
            //
            pTaskCollection->_RaisedException();
            pTaskCollection->_Cancel();
        }

        pOriginContext->RemoveStealer(pContext);

        pContext->ClearCancel();
        pContext->SetRootCollection(NULL);
        pChore->_M_pTaskCollection = NULL;
        pTaskCollection->_CountUp();
    }

    /// <summary>
    ///     Wrapper around execution of an ustructured chore that performs appropriate notification
    ///     and exception handling semantics.
    /// </summary>
    __declspec(noinline)
    void __cdecl _UnrealizedChore::_UnstructuredChoreWrapper(_UnrealizedChore * pChore)
    {
        // assuming a task collection ctor has already been called

        InternalContextBase *pContext = static_cast<InternalContextBase *> (SchedulerBase::FastCurrentContext());
        ASSERT(pContext != NULL && !pContext->IsCanceled());

        _TaskCollection* pTaskCollection = static_cast<_TaskCollection *> (pChore->_M_pTaskCollection);
        ContextBase *pOriginContext = reinterpret_cast <ContextBase *> (pTaskCollection->_M_pOwningContext);

        pContext->SetRootCollection(pTaskCollection);

        //
        // pOriginContext is only safe to touch if the act of stealing from a non-detached context put a hold on that context
        // to block deletion until we are chained for cancellation.
        //
        SafeRWList<ListEntry> *pList = reinterpret_cast<SafeRWList<ListEntry> *> (pTaskCollection->_M_stealTracker);
        _ASSERTE(sizeof(pTaskCollection->_M_stealTracker) >= sizeof(*pList));


        if (pChore->_M_fDetached)
        {
            //
            // We cannot touch the owning context -- it was detached as of the steal.  The chain goes onto the task collection.
            //
            pContext->NotifyTaskCollectionChainedStealer();
            pList->AddTail(&(pContext->m_stealChain));
        }
        else
        {
            pList->AcquireWrite();
            pTaskCollection->_M_activeStealersForCancellation++;
            pList->ReleaseWrite();
            pOriginContext->AddStealer(pContext, true);
        }

        //
        // Waiting on the indirect alias may throw (e.g.: the entire context may have been canceled).  If it
        // throws, we need to deal with appropriate marshaling.
        //
        try
        {
            //
            // Set up an indirect alias for this task collection.  Any usage of the original task collection
            // within this stolen chore will automatically redirect through the indirect alias.  This allows
            // preservation of single-threaded semantics within the task collection while allowing it to be "accessed"
            // from stolen chores (multiple threads).
            //
            // This stack based collection will wait on stolen chores at destruction time.  In the event the collection is not
            // used during the steal, this doesn't do much.
            //
            _TaskCollection indirectAlias(pTaskCollection, false);

            pContext->SetIndirectAlias(&indirectAlias);

            try
            {
                //
                // We need to consider this a possible interruption point.  It's entirely possible that we stole and raced with a
                // cancellation thread.  The collection was canceled after we stole(e.g.: removed from the WSQ) but before we added ourselves 
                // to the stealing chain list above.  In this case, the entire context will wait until completion (bad).  Immediately
                // after we go on the list (a memory barrier), we need to check the collection cancellation flag.  If the collection is going away,
                // we need to get out *NOW* otherwise the entire subtree executes.
                //
                if (pTaskCollection->_M_pOriginalCollection->_M_exitCode != 0 ||
                    (pTaskCollection->_M_executionStatus != TASKCOLLECTION_EXECUTION_STATUS_CLEAR &&
                    pTaskCollection->_M_executionStatus != TASKCOLLECTION_EXECUTION_STATUS_INLINE &&
                    pTaskCollection->_M_executionStatus != TASKCOLLECTION_EXECUTION_STATUS_INLINE_WAIT_WITH_OVERFLOW_STACK)) 
                    throw task_canceled();

                pChore->m_pFunction(pChore);
            }
            catch(const task_canceled &)
            {
                //
                // If someone manually threw the task_canceled exception, we will have a cancel count but not a canceled context.  This
                // means we need to apply the cancel one level up.  Normally, the act of throwing would do that via being caught in the 
                // wait, but this is special "marshaling" for task_canceled.
                //
                if (pContext->IsCanceled() && !pContext->IsCanceledContext())
                    pTaskCollection->_Cancel();
            }
            catch(...)
            {
                //
                // Track the exception that was thrown here and subsequently cancel all work.  _RaisedException makes the decision on what
                // exceptions to keep and what to discard.  The flags it sets will indicate to the thread calling ::Wait that it must rethrow.
                //
                pTaskCollection->_RaisedException();
                pTaskCollection->_Cancel();
            }

            indirectAlias._Wait();
        }
        catch(const task_canceled &)
        {
            //
            // If someone manually threw the task canceled exception out of a task on the indirect alias, the same thing applies as to
            // a directly stolen chore (above).
            //
            if (pContext->IsCanceled() && !pContext->IsCanceledContext())
                pTaskCollection->_Cancel();
        }
        catch(...)
        {
            //
            // Track the exception that was thrown here and subsequently cancel all work.  _RaisedException makes the decision on what
            // exceptions to keep and what to discard.  The flags it sets will indicate to the thread calling ::Wait that it must rethrow.
            //
            pTaskCollection->_RaisedException();
            pTaskCollection->_Cancel();
        }

        pContext->SetIndirectAlias(NULL);

        if ( !pChore->_M_fDetached)
        {
            //
            // pOriginContext may die at any point (detachment).  When it does, it will transfer the stolen chore trace from the context to the
            // given task collection (us) under lock.  We can, therefore, take this lock and check if we are still okay to check the context.
            //
            pList->AcquireWrite();

            if (pContext->IsContextChainedStealer())
                pOriginContext->RemoveStealer(pContext);
            else
                pList->UnlockedRemove(&(pContext->m_stealChain));

            pTaskCollection->_M_activeStealersForCancellation--;
            
            pList->ReleaseWrite();

        } 
        else
        {
            pList->Remove(&(pContext->m_stealChain));
        }

        pContext->ClearCancel();
        pContext->ClearAliasTable();
        pContext->SetRootCollection(NULL);
        pChore->_M_pTaskCollection = NULL;

        //
        // This must come prior to the count-up.  If it's stack allocated, the countup will cause unwinding.
        //
        _UnrealizedChore::_InternalFree(pChore);

        pTaskCollection->_NotifyCompletedChore();
    }
} // namespace details

} // namespace Concurrency
