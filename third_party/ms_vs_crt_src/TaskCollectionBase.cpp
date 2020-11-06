// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// TaskCollectionBase.cpp
//
// General abstract collection of work counting / eventing implementation
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Called when an exception is raised on a chore on a given task collection, this makes a determination of what to do with the exception
    ///     and stores it for potential transport back to the thread performing a join on a task collection.
    /// </summary>
    void _TaskCollectionBase::_RaisedException()
    {
        //
        // Current strategy is that the first exception in is kept and rethrown.  We may update this in the future.
        //
        void * _OldStatus = _M_pException;
        for (;;)
        {
            //
            // We always overwrite the cancel exception being here.  Everything else is "more important".
            //
            std::exception_ptr *_pException = (std::exception_ptr *)((size_t)_OldStatus & ~_S_cancelBitsMask);
            if (_pException != NULL && (size_t)_pException != _S_cancelException)
                return;

            //
            // Maintain the lower bit as a cancel status flag to determine where to stop a cancellation.
            //
            size_t _cancelStatus = ((size_t)_OldStatus & _S_cancelBitsMask);

            void * _XchgStatus = _InterlockedCompareExchangePointer((void * volatile *) &_M_pException, (void *) (_S_nonNull | _cancelStatus), _OldStatus);
            if (_XchgStatus == _OldStatus)
                break;

            _OldStatus = _XchgStatus;
        }

        //
        // Note that this is safe as this will only be called on a chore executing on the collection; therefore it will not be touched by the forking
        // thread until after we "_CountUp" which comes after this.
        //
        void *_pExc = new std::exception_ptr(std::current_exception());
        _OldStatus = _M_pException;
        for(;;)
        {
            size_t _cancelStatus = ((size_t)_OldStatus & _S_cancelBitsMask);
            void *_pExcWC = (void *)((size_t)_pExc | _cancelStatus);

            void *_XchgStatus = _InterlockedCompareExchangePointer((void * volatile *) &_M_pException, _pExcWC, _OldStatus);
            if (_XchgStatus == _OldStatus)
                break;

            _OldStatus = _XchgStatus;
        }
    }

    /// <summary>
    ///     Potentially rethrows the exception which was set with _RaisedException.  The caller has responsibility to ensure that _RaisedException 
    ///     was called prior to calling this and that _M_pException has progressed beyond the _S_nonNull state.
    /// </summary>
    void _TaskCollectionBase::_RethrowException()
    {
        //
        // The cancellation exception is treated very specially within the runtime.  Do not arbitrarily rethrow from here.
        //
        std::exception_ptr *_pException = _Exception();
        if (_pException != NULL && (size_t)_pException != _S_cancelException)
        {
            std::exception_ptr _curException = *_Exception();

            delete _pException;
            _M_pException = NULL;

            if ( !__uncaught_exception())
                std::rethrow_exception(_curException);
        }
    }

    /// <summary>
    ///     Marks the collection for cancellation and returns whether the collection was thus marked.
    /// </summary>
    bool _TaskCollectionBase::_MarkCancellation()
    {
        void *_OldStatus = _M_pException;
        for(;;)
        {
            if ((size_t)_OldStatus & _S_cancelBitsMask)
                return false;

            void *_XchgStatus = _InterlockedCompareExchangePointer((void * volatile *) &_M_pException, 
                                                                   (void *)((size_t)_OldStatus | _S_cancelStarted),
                                                                   _OldStatus);
            if (_XchgStatus == _OldStatus)
                return true;

            _OldStatus = _XchgStatus;
        }
    }

    /// <summary>
    ///     Finishes the cancellation state (changing from _S_cancelStarted to one of the other states).  Note that only the 
    ///     thread which successfully marked cancellation may call this.
    /// </summary>
    void _TaskCollectionBase::_FinishCancelState(size_t _NewCancelState)
    {
        _ASSERTE(_CancelState() == _S_cancelStarted);
        _ASSERTE(_NewCancelState != _S_cancelNone && _NewCancelState != _S_cancelStarted);

        void *_OldStatus = _M_pException;
        for(;;)
        {
            void *_XchgStatus = _InterlockedCompareExchangePointer((void * volatile *) &_M_pException,
                                                                   (void *)(((size_t)_OldStatus & ~_S_cancelBitsMask) | _NewCancelState),
                                                                   _OldStatus);

            if (_XchgStatus == _OldStatus)
                break;

            _OldStatus = _XchgStatus;
        }
    }

    /// <summary>
    ///     Called when a cancellation is raised on a chore on a given task collection.  This makes a determination of what to do with the exception
    ///     and stores it for potential transport back to the thread performing a join on a chore collection.  Note that every other exception
    ///     has precedence over a cancellation.
    /// </summary>
    void _TaskCollectionBase::_RaisedCancel()
    {
        void *_OldStatus = _M_pException;
        for (;;)
        {
            std::exception_ptr *_pException = (std::exception_ptr *)((size_t)_OldStatus & ~_S_cancelBitsMask);
            if (_pException != NULL)
                return;

            size_t _cancelStatus = ((size_t)_OldStatus & _S_cancelBitsMask);
            void *pExcWC = (void *)(_S_cancelException | _cancelStatus);

            void *_XchgStatus = _InterlockedCompareExchangePointer((void * volatile *) &_M_pException, pExcWC, _OldStatus);
            if (_XchgStatus == _OldStatus)
                break;

            _OldStatus = _XchgStatus;
        }
    }

    /// <summary>
    ///     Called in order to determine whether this task collection will interrupt for a pending cancellation at or above it.
    /// 
    bool _TaskCollectionBase::_WillInterruptForPendingCancel()
    {
        //
        // We can only perform the interruption point if someone in the parentage chain is actually inlined.  The number of times where we get here
        // without such should be minimal.
        //
        // Note that structured collections do not initialize _M_pParent until they are inlined.  In order to avoid excess initialization in the
        // structured case, we key off that to determine the validity of the field.  Note that this check is perfectly okay for task collections
        // as well.
        //
        _TaskCollectionBase *pParent = _SafeGetParent();

        while (pParent != NULL)
        {
            if ((pParent->_IsStructured() && (static_cast<_StructuredTaskCollection *>(pParent))->_IsMarkedForCancellation()) ||
                (!pParent->_IsStructured() && (static_cast<_TaskCollection *>(pParent))->_IsMarkedForAbnormalExit()))
                return true;

            pParent = pParent->_SafeGetParent();
        }

        return false;
         
    }

    /// <summary>
    ///     Called in order to execute a supposed interruption point.
    /// </summary>
    void _TaskCollectionBase::_Interrupt(bool _FLocalCondition, int _LocalFlags)
    {
        ContextBase *pContext = SchedulerBase::FastCurrentContext();
        ASSERT(pContext != NULL);

        if (pContext->IsCanceled() || _FLocalCondition || _WillInterruptForPendingCancel())
        {
            if (_LocalFlags != 0)
                _M_inlineFlags |= _LocalFlags;

            throw task_canceled();
        }
    }

} // namespace details
} // namespace Concurrency
