// ==++==
//
//	Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// utils.h
//
// Header file containing the utility routine declarations.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

//**************************************************************************
// Previously Public Macro Definitions:
//**************************************************************************

// Enable tracing mechanisms
#if defined(_DEBUG) && defined(CONCRT_TRACING)
# define CONCRT_TRACE(...)  ::Concurrency::details::_ConcRT_Trace(__VA_ARGS__)
#else
# define CONCRT_TRACE(...)  ((void)0)
#endif

// Weaker than assert/verify, yet informational
#define CONCRT_MESSAGE(...) ::Concurrency::details::_ConcRT_DumpMessage(__VA_ARGS__)

#if defined(_DEBUG)
# define CONCRT_DEBUGMESSAGE(...) ::Concurrency::details::_ConcRT_DumpMessage(__VA_ARGS__)
#else
# define CONCRT_DEBUGMESSAGE(...) ((void)0)
#endif

#if defined(_DEBUG)
# define CONCRT_ASSERT(x)   (((x) ? ((void)0) : ::Concurrency::details::_ConcRT_Assert(#x, __FILE__, __LINE__)), __assume(x))
#else
# define CONCRT_ASSERT(x)   (__assume(x))
#endif

#if defined(_DEBUG)
# define CONCRT_COREASSERT(x)   (((x) ? ((void)0) : ::Concurrency::details::_ConcRT_CoreAssert(#x, __FILE__, __LINE__)), __assume(x))
#else
# define CONCRT_COREASSERT(x) CONCRT_ASSERT(x)
#endif

// will not evaluate 'x' multiple times
#if defined(_DEBUG) 
# define CONCRT_FASSERT(x)   {bool b = !!(x); (__assume(b), (b) ? ((void)0) : ::Concurrency::details::_ConcRT_Assert(#x, __FILE__, __LINE__));}
#else
# define CONCRT_FASSERT(x)   (__assume(x))
#endif

#if defined(_DEBUG)
# define CONCRT_VERIFY(x)   CONCRT_FASSERT(x)
#else
# define CONCRT_VERIFY(x)   (__assume(x), ((void)x))
#endif

//
// MTRACE:
//
// Memory tracing for UMS debugging (since it's nigh impossible elsewhere).  This should be improved once the scheduler is "stable".  Right now,
// buffers are interlocked incremented.
//
#define MTRACE_EVT_AFFINITIZED 1
#define MTRACE_EVT_ADDEDTORUNNABLES 2
#define MTRACE_EVT_UMSBLOCKED 3
#define MTRACE_EVT_CRITICALBLOCK 4
#define MTRACE_EVT_PULLEDFROMCOMPLETION 5
#define MTRACE_EVT_SWITCHTO_BLOCKING 6
#define MTRACE_EVT_SWITCHTO_IDLE 7
#define MTRACE_EVT_SWITCHTO_YIELDING 8
#define MTRACE_EVT_SWITCHTO_NESTING 9
#define MTRACE_EVT_CONTEXT_RELEASED 10
#define MTRACE_EVT_CONTEXT_ACQUIRED 11
#define MTRACE_EVT_SFW_FOUND 12
#define MTRACE_EVT_SFW_FOUNDBY 13
#define MTRACE_EVT_CRITICALNOTIFY 14
#define MTRACE_EVT_SUTNOTIFY 15
#define MTRACE_EVT_BLOCKUNBLOCKRACE 16
#define MTRACE_EVT_DEACTIVATE 17
#define MTRACE_EVT_ACTIVATE 18
#define MTRACE_EVT_INVERTED_ADDEDTORUNNABLES 19
#define MTRACE_EVT_CLAIMEDOWNERSHIP 20
#define MTRACE_EVT_MADEAVAILABLE 21
#define MTRACE_EVT_AVAILABLEVPROCS 22
#define MTRACE_EVT_SWITCHTO 23
#define MTRACE_EVT_WOKEAFTERDEACTIVATE 24
#define MTRACE_EVT_RMAWAKEN 25
#define MTRACE_EVT_ACTIVATED 26
#define MTRACE_EVT_SEARCHEDLOCALRUNNABLES 27
#define MTRACE_EVT_RESTARTRAMBLING 28
#define MTRACE_EVT_STARTRAMBLING 29
#define MTRACE_EVT_STOPRAMBLING 30
#define MTRACE_EVT_SFW_NEXTLOOP 31
#define MTRACE_EVT_UPDATERAMBLING_RING 32
#define MTRACE_EVT_UPDATERAMBLING_RESETRING 33
#define MTRACE_EVT_UPDATERAMBLING_ALLVPROCS 34
#define MTRACE_EVT_RETURNTOPRIMARY_BLOCKED 35
#define MTRACE_EVT_RETURNTOPRIMARY_YIELD 36
#define MTRACE_EVT_EXECUTE 37
#define MTRACE_EVT_EXECUTEFAIL 38
#define MTRACE_EVT_RETIRE 39
#define MTRACE_EVT_ORIGINALCOMPLETION 40
#define MTRACE_EVT_CONTEXTPOOLED 41
#define MTRACE_EVT_CONTEXTUNPOOLED 42
#define MTRACE_EVT_CONTEXTUNBOUND 43

#if _UMSTRACE

//
// Scheduler Level:
//
#define VMTRACE(traceevt, ctx, vp, data) ::Concurrency::details::_ConcRT_VMTrace((int)traceevt, (void *)ctx, (void *)vp, (ULONG_PTR)data);
#define CMTRACE(traceevt, ctx, vp, data) ::Concurrency::details::_ConcRT_CMTrace((int)traceevt, (void *)ctx, (void *)vp, (ULONG_PTR)data);
#define VCMTRACE(traceevt, ctx, vp, data) { VMTRACE(traceevt, ctx, vp, data); CMTRACE(traceevt, ctx, vp, data); }

//
// RM Level:
//
#define RVMTRACE(traceevt, ctx, vp, data) ::Concurrency::details::_ConcRT_RVMTrace((int)traceevt, (void *)ctx, (void *)vp, (ULONG_PTR)data);
#define RPMTRACE(traceevt, ctx, vp, data) ::Concurrency::details::_ConcRT_RPMTrace((int)traceevt, (void *)ctx, (void *)vp, (ULONG_PTR)data);
#define RVPMTRACE(traceevt, ctx, vp, data) { RVMTRACE(traceevt, ctx, vp, data); RPMTRACE(traceevt, ctx, vp, data); }
#else
#define VMTRACE(traceevt, ctx, vp, data) 
#define CMTRACE(traceevt, ctx, vp, data)
#define VCMTRACE(traceevt, ctx, vp, data)
#define RVMTRACE(traceevt, ctx, vp, data)
#define RPMTRACE(traceevt, ctx, vp, data)
#define RVPMTRACE(traceevt, ctx, vp, data)
#endif // _UMS_TRACE


#define CONCRT_TRACE_ALL                0xFFFF
#define CONCRT_TRACE_SCHEDULER          0x0001
#define CONCRT_TRACE_MSG                0x0002
#define CONCRT_TRACE_SGROUP             0x0004
#define CONCRT_TRACE_SCHEDULER_INSTANCE 0x0008
#define CONCRT_TRACE_COLLECTIONS        0x0010
#define CONCRT_TRACE_EVENT              0x0020
#define CONCRT_TRACE_CHORES             0x0040
#define CONCRT_TRACE_WORKQUEUE          0x0080
#define CONCRT_TRACE_UNIT               0x0100
#define CONCRT_TRACE_HILLCLIMBING       0x0200
#define CONCRT_TRACE_DYNAMIC_RM         0x0400

// Various macros are defined in public headers as CONCRT_whatever.  Define
// them here without the CONCRT_ prefix for use in the internal implementation.

#define TRACE                       CONCRT_TRACE
#define ASSERT                      CONCRT_ASSERT
#define CORE_ASSERT                 CONCRT_COREASSERT
#define FASSERT                     CONCRT_FASSERT // evaluates argument just once, but __assume is not as effective in influencing oacr
#define VERIFY                      CONCRT_VERIFY
#define MESSAGE                     CONCRT_MESSAGE
#define DEBUGMESSAGE                CONCRT_DEBUGMESSAGE

#define TRACE_ALL                   CONCRT_TRACE_ALL
#define TRACE_SCHEDULER             CONCRT_TRACE_SCHEDULER
#define TRACE_MSG                   CONCRT_TRACE_MSG
#define TRACE_SGROUP                CONCRT_TRACE_SGROUP
#define TRACE_SCHEDULER_INSTANCE    CONCRT_TRACE_SCHEDULER_INSTANCE
#define TRACE_COLLECTIONS           CONCRT_TRACE_COLLECTIONS
#define TRACE_EVENT                 CONCRT_TRACE_EVENT
#define TRACE_CHORES                CONCRT_TRACE_CHORES
#define TRACE_WORKQUEUE             CONCRT_TRACE_WORKQUEUE
#define TRACE_UNIT                  CONCRT_TRACE_UNIT

// Useful Macros

#define UNREACHED 0

#define KB 1024
#define DEFAULTCONTEXTSTACKSIZE (64 * KB)

#define WIDEN2(str) L ## str
#define WIDEN(str)  WIDEN2(str)

#define __WFILE__   WIDEN(__FILE__)

#define DIM(array)  (sizeof(array) / sizeof(array[0]))

// Ensure we use the intrinsic forms of Interlocked* APIs

#undef  InterlockedAnd
#undef  InterlockedCompareExchange
#undef  InterlockedDecrement
#undef  InterlockedExchange
#undef  InterlockedExchangeAdd
#undef  InterlockedIncrement
#undef  InterlockedOr
#undef  InterlockedXor

#define InterlockedAnd             _InterlockedAnd
#define InterlockedCompareExchange _InterlockedCompareExchange
#define InterlockedDecrement       _InterlockedDecrement
#define InterlockedExchange        _InterlockedExchange
#define InterlockedExchangeAdd     _InterlockedExchangeAdd
#define InterlockedIncrement       _InterlockedIncrement
#define InterlockedOr              _InterlockedOr
#define InterlockedXor             _InterlockedXor

#ifdef _M_X64
# undef  InterlockedAnd64
# undef  InterlockedOr64
# undef  InterlockedXor64
# undef  InterlockedIncrement64
# define InterlockedAnd64 _InterlockedAnd64
# define InterlockedOr64  _InterlockedOr64
# define InterlockedXor64 _InterlockedXor64
# define InterlockedIncrement64 _InterlockedIncrement64
#endif

#if defined(_M_IA64) || defined(_M_AMD64) || (defined(_M_IX86) && _MSC_FULL_VER >= 140030626)
#define USE_ICX64   1
# undef  InterlockedCompareExchange64
# define InterlockedCompareExchange64 _InterlockedCompareExchange64
#else
#undef  USE_ICX64
#endif

#undef  InterlockedCompareExchangePointer
#undef  InterlockedExchangePointer

#define InterlockedCompareExchangePointer _InterlockedCompareExchangePointer
#define InterlockedExchangePointer        _InterlockedExchangePointer

LONGLONG
FORCEINLINE
InterlockedInc64 (
    __inout LONGLONG volatile *Addend
    )
{
#ifdef _M_X64
    return _InterlockedIncrement64(Addend);
#else
    LONGLONG Old;

    do {
        Old = *Addend;
    } while (_InterlockedCompareExchange64(Addend,
                                          Old + 1,
                                          Old) != Old);

    return Old + 1;
#endif
}

#undef InterlockedIncrementSizeT
#undef InterlockedDecrementSizeT
#undef InterlockedCompareExchangeSizeT

#ifdef _WIN64
#define InterlockedIncrementSizeT(x) (size_t)(InterlockedIncrement64(reinterpret_cast<volatile LONGLONG *>((x))))
#define InterlockedDecrementSizeT(x) (size_t)(InterlockedDecrement64(reinterpret_cast<volatile LONGLONG *>((x))))
#define InterlockedCompareExchangeSizeT(x,y,z) (size_t)(InterlockedCompareExchange64(reinterpret_cast<volatile LONGLONG *>((x)), (LONGLONG)((y)), (LONGLONG)((z))))
#else
#define InterlockedIncrementSizeT(x) (size_t)(InterlockedIncrement(reinterpret_cast<volatile LONG *>((x))))
#define InterlockedDecrementSizeT(x) (size_t)(InterlockedDecrement(reinterpret_cast<volatile LONG *>((x))))
#define InterlockedCompareExchangeSizeT(x,y,z) (size_t)(InterlockedCompareExchange(reinterpret_cast<volatile LONG *>((x)), (LONG)((y)), (LONG)((z))))
#endif

bool
FORCEINLINE
SafeInterlockedIncrement (
    __inout LONG volatile *Addend
    )
{
    LONG Old;

    do {
        Old = *Addend;
        if (Old == 0)
            return false;
    } while (_InterlockedCompareExchange(Addend,
                                         Old + 1,
                                         Old) != Old);

    return true;
}



// For HillClimbing

inline double GetCurrentHiRezTime()
{
    static LARGE_INTEGER qpcFreq;
    if (0 == qpcFreq.QuadPart)
    {
        QueryPerformanceFrequency(&qpcFreq);
    }

    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);

    return (double)time.QuadPart / (double)qpcFreq.QuadPart;
}

template<typename T>
T sign(T val)
{
    if (val == 0)
    {
        return 0;
    }
    return val > 0 ? 1 : -1;
}

USHORT
inline
NumberOfBitsSet(
    __in ULONG_PTR mask
    )
{
    USHORT count = 0;

    while (mask != 0)
    {
        ++count;
        mask &= (mask - 1);  
    }
    return count;
}

namespace Concurrency
{
namespace details
{
    _CRTIMP void _ConcRT_Trace(int trace_level, const wchar_t * format, ...);
    _CRTIMP void _ConcRT_DumpMessage(const wchar_t * format, ...);
    _CRTIMP void _ConcRT_Assert(const char* value, const char* filename, int lineno);
    _CRTIMP void _ConcRT_CoreAssert(const char* value, const char* filename, int lineno);
    _CRTIMP void _ConcRT_VMTrace(int traceevt, void *pCtx, void *pVp, ULONG_PTR data);
    _CRTIMP void _ConcRT_CMTrace(int traceevt, void *pCtx, void *pVp, ULONG_PTR data);
    _CRTIMP void _ConcRT_RVMTrace(int traceevt, void *pCtx, void *pVp, ULONG_PTR data);
    _CRTIMP void _ConcRT_RPMTrace(int traceevt, void *pCtx, void *pVp, ULONG_PTR data);

    void InitializeUtilityRoutines();

    /// <summary>
    ///     Static methods related to security such as encode/decode pointer
    /// </summary>
    class Security
    {
    public:

        static ULONG_PTR s_cookie;
        static volatile long s_initialized;

        static ULONG_PTR InitializeCookie();

        static PVOID EncodePointer(PVOID ptr);
        static PVOID DecodePointer(PVOID ptr);
    };

//
// Logging utilities specific to the UMS scheduler
//
#if _UMSTRACE

    #define _UMSTRACE_BUFFER_SIZE 16384

    struct _TraceEntry
    {
        int m_traceEvt;
        void *m_pCtx;
        void *m_pVproc;
        ULONG_PTR m_data;
        DWORD m_tid;
    };

    class _TraceBuffer
    {
    public: 

        _TraceBuffer() : m_tracePtr(0xFFFFFFFF)
        {
        }

        ~_TraceBuffer()
        {
        }

        void Trace(int traceEvt, void *pCtx, void *pVproc, ULONG_PTR data)
        {
            ULONG ptr = (ULONG)InterlockedIncrement((volatile LONG *)&m_tracePtr);
            _TraceEntry *pTrace = m_trace + (ptr % _UMSTRACE_BUFFER_SIZE);
            pTrace->m_traceEvt = traceEvt;
            pTrace->m_pCtx = pCtx;
            pTrace->m_pVproc = pVproc;
            pTrace->m_data = data;
            pTrace->m_tid = GetCurrentThreadId();
        }
        
    private:
        
        volatile ULONG m_tracePtr;
        _TraceEntry m_trace[_UMSTRACE_BUFFER_SIZE];
    };

#endif // _UMSTRACE

    /// <summary>
    ///     Use Sleep(0) to do the yield.
    /// </summary>
    void __cdecl _Sleep0();

    /// <summary>
    ///     Spin WHILE the value of the variable is equal to a given value.
    ///     _Ty and _U should be comparable types
    /// </summary>
    template<typename _Ty, typename _U>
    static inline void SpinwaitWhileEq( volatile _Ty& location, _U value )
    {
        _SpinWaitBackoffNone spinWait;
        while( location==value )
        {
            spinWait._SpinOnce();
        }
    }

    /// <summary>
    ///     Spin UNTIL the value of the variable is equal to a given value.
    ///     _Ty and _U should be comparable types
    /// </summary>
    template<typename _Ty, typename _U>
    static inline void SpinwaitUntilEq( volatile _Ty& location, const _U value )
    {
        _SpinWaitBackoffNone spinWait;
        while( location!=value )
        {
            spinWait._SpinOnce();
        }
    }

    /// <summary>
    ///     Spin UNTIL the value of the variable is equal to a given value.
    ///     Uses Sleep(0) to yield
    /// </summary>
    void
    inline
    SpinUntilValueEquals(
         __in LONG volatile * Address,
         __in LONG Value
         )
    {
        if (*Address != Value)
        {
            _SpinWaitBackoffNone spinWait(_Sleep0);

            do
            {
                spinWait._SpinOnce();
            } while (*Address != Value);
        }
    }

    /// <summary>
    ///     Spin UNTIL the specified bits are set
    ///     Uses Sleep(0) to yield
    /// </summary>
    LONG
    inline
    SpinUntilBitsSet(
         __in LONG volatile * Address,
         __in LONG Bits
         )
    {
        LONG val = *Address;
        if ((val & Bits) != Bits)
        {
            _SpinWaitBackoffNone spinWait(_Sleep0);

            do
            {
                spinWait._SpinOnce();
                val = *Address;
            } while ((val & Bits) != Bits);
        }
        return val;
    }

    /// <summary>
    ///     Spin UNTIL the specified bits are reset.
    ///     Uses Sleep(0) to yield
    /// </summary>
    LONG
    inline
    SpinUntilBitsReset(
         __in LONG volatile * Address,
         __in LONG Bits
         )
    {
        LONG val = *Address;
        if ((val & Bits) != 0)
        {
            _SpinWaitBackoffNone spinWait(_Sleep0);

            do
            {
                spinWait._SpinOnce();
                val = *Address;
            } while ((val & Bits) != 0);
        }
        return val;
    }

    /// <summary>
    ///     This non-reentrant lock is a pure spin lock and is intended for use in situations
    ///     where it is known that the lock will not be taken recursively, and can thus be more 
    ///     efficiently implemented.
    /// </summary>
    class _NonReentrantLock
    {
    public:
        /// <summary>
        ///     Constructor for _NonReentrantLock
        /// </summary>
        _NonReentrantLock()
            : _M_Lock(0)
        {
        }

        /// <summary>
        ///     Acquire the lock, spin if necessary
        /// </summary>
        void _Acquire()
        {
#if defined(_DEBUG)
            _DebugAcquire();
#else // !_DEBUG

            if (InterlockedExchange(&_M_Lock, 1) != 0)
            {
                _SpinWaitBackoffNone spinWait(_Sleep0);

                do
                {
                    spinWait._SpinOnce();
                }
                while (InterlockedExchange(&_M_Lock, 1) != 0);
            }

#endif // !_DEBUG
        }

        /// <summary>
        ///     Tries to acquire the lock, does not spin.
        ///     Returns true if the lock is taken, false otherwise
        /// </summary>
        bool _TryAcquire()
        {
#if defined(_DEBUG)
            return _DebugTryAcquire();
#else // !_DEBUG
            return (_M_Lock == 0 && InterlockedExchange(&_M_Lock, 1) == 0);
#endif // _DEBUG
        }

        /// <summary>
        ///     Releases the lock
        /// </summary>
        void _Release()
        {
#if defined(_DEBUG)
            _M_Lock &= ~1;
#else // !_DEBUG
            _M_Lock = 0;
#endif // _DEBUG
        }

        bool _IsLockHeld() const 
        {
            return (_M_Lock != 0);
        }

        /// <summary>
        ///     An exception safe RAII wrapper.
        /// </summary>
        class _Scoped_lock
        {
        public:
            explicit _Scoped_lock(_NonReentrantLock& _Lock) : _M_lock(_Lock)
            {
                _M_lock._Acquire();
            }

            ~_Scoped_lock()
            {
                _M_lock._Release();
            }
        private:
            _NonReentrantLock& _M_lock;

            _Scoped_lock(const _Scoped_lock&);                    // no copy constructor
            _Scoped_lock const & operator=(const _Scoped_lock&);  // no assignment operator
        };

    private:
        // The lock being held
        volatile long _M_Lock;

        bool _DebugTryAcquire();
        void _DebugAcquire();
    };

    /// <summary>
    ///     A variant of _NonReentrantLock which ensures that the lock is taken in a hyper critical region.
    /// </summary>
    class _HyperNonReentrantLock
    {
    public:
        void _Acquire();
        bool _TryAcquire();
        void _Release();

        bool _IsLockHeld() const
        {
            return m_lock._IsLockHeld();
        }

        /// <summary>
        ///     An exception safe RAII wrapper.
        /// </summary>
        class _Scoped_lock
        {
        public:
            explicit _Scoped_lock(_HyperNonReentrantLock& _Lock) : _M_lock(_Lock)
            {
                _M_lock._Acquire();
            }

            ~_Scoped_lock()
            {
                _M_lock._Release();
            }
        private:
            _HyperNonReentrantLock& _M_lock;

            _Scoped_lock(const _Scoped_lock&);                    // no copy constructor
            _Scoped_lock const & operator=(const _Scoped_lock&);  // no assignment operator
        };

    private:
        _NonReentrantLock m_lock;
    };

    /// <summary>
    ///     A variant of _NonReentrantLock which ensures that the lock is taken in a critical region.
    /// </summary>
    class _CriticalNonReentrantLock
    {
    public:
        void _Acquire();
        bool _TryAcquire();
        void _Release();

        bool _IsLockHeld() const
        {
            return m_lock._IsLockHeld();
        }

        /// <summary>
        ///     An exception safe RAII wrapper.
        /// </summary>
        class _Scoped_lock
        {
        public:
            explicit _Scoped_lock(_CriticalNonReentrantLock& _Lock) : _M_lock(_Lock)
            {
                _M_lock._Acquire();
            }

            ~_Scoped_lock()
            {
                _M_lock._Release();
            }
        private:
            _CriticalNonReentrantLock& _M_lock;

            _Scoped_lock(const _Scoped_lock&);                    // no copy constructor
            _Scoped_lock const & operator=(const _Scoped_lock&);  // no assignment operator
        };

    private:
        _NonReentrantLock m_lock;
    };


    typedef _NonReentrantLock _StaticLock;


    // Wrapper around _BitScanReverse (for concurrent_vector)
    _CRTIMP unsigned long Log2(size_t);

    // **************************************************
    // Safe Points:
    //
    // Pre-declare structures used for safe points.  These must be defined early due to usage within collections and other utilities.
    //
    // A safe point is defined as a region past which every virtual processor is guaranteed to have made a particular observation.  Operations
    // subject to safe points are usually defined as two phase operations where phase 1 of an operation is performed, and phase 2 is registered
    // to occur at the next safe point.  The actual safe point may occur an **ARBITRARY** amount of time later.  If a given virtual processor
    // is sleeping, it may not make the observation until it awakens, is retired, or the scheduler finalizes.  Likewise, if a given virtual processor
    // is performing work which is not cooperatively blocking, it may not make the observation until the next cooperative event.  Thus, the operation 
    // performed at a safe point must not be performance critical relative to when it was scheduled.
    //
    // The typical uses of safe points are things like deferred deletions from lock free lists.  For example, an element may be removed from 
    // a ListArray as phase 1 and deleted on reaching the next safe point as phase 2.  This guarantees that every virtual processor has observed 
    // the removal and isn't touching the element.
    //
    // Each virtual processor contains a SafePointMarker which performs necessary data versioning for this mechanism to work.
    //
    // ******************** READ THIS NOW ********************
    //
    // Safe points are observations by virtual processors, not every context running on the scheduler.  While this distinction does not matter so much
    // on the thread scheduler, it is **EXTREMELY IMPORTANT** on the UMS scheduler.  As such, the usage of safe points to guard an operation X must follow
    // a set of rules:
    //
    //     - The operation X must be inclusively bound by a critical region on the UMS scheduler. 
    //
    //       As an example, consider lock-free traversal of a ListArray.  Internal contexts traverse list array objects without regard to the state of
    //       the objects.  The ListArray code frees objects at safe points.  The safe point is guarding the reference of ListArray objects.  Therefore,
    //       the entire region from inclusively between saying p = ListArray[x] and the last dereference of p must be bounded by a critical region.
    //
    //       As a second example, detached work stealing queues release their reference on their schedule group at retirement at a safe point.  Code which
    //       steals from a detached work stealing queue does not put a new reference on the schedule group until calling WorkItem::TransferReferences.  The
    //       safe point is guarding the region between the steal and the placement of the reference.  Therefore, the entire region inclusively between
    //       WorkQueue::Steal... and WorkItem::TransferReferences must be inclusively bounded by a critical region.
    //

    template<class T> class SQueue;

    /// <summary>
    ///     An intrusive object which is inserted into the list of work a scheduler must invoke on the next safe point.
    /// </summary>
    class SafePointInvocation
    {
    public:

        typedef void (*InvocationFunction)(void *);

        /// <summary>
        ///     Registers a particular function to be called with particular data when a given scheduler reaches the next safe point
        ///     after the call is made.  This is an intrusive invocation with the current SafePointInvocation class incuring no heap
        ///     allocations.
        /// </summary>
        /// <param name="pInvocationFunction">
        ///     The function which will be invoked at the next safe point
        /// </param>
        /// <param name="pData">
        ///     User specified data.
        /// </param>
        /// <param name="pScheduler">
        ///     The scheduler on which to wait for a safe point to invoke pInvocationFunction.
        /// </param>
        void InvokeAtNextSafePoint(InvocationFunction pInvocationFunction, void *pData, SchedulerBase *pScheduler);

    private:

        friend class SchedulerBase;
        template<class T> friend class SQueue;

        /// <summary>
        ///     The invocation of the callback for this particular registration.
        /// </summary>
        void Invoke()
        {
            m_pInvocation(m_pData);
        }

        // The client invocation function
        InvocationFunction m_pInvocation;

        // The client data
        void *m_pData;

        // The data version for this safe point.
        ULONG m_safePointVersion;

        // The queue linkage (spin-lock guarded)
        SafePointInvocation *m_pNext;

    };

    /// <summary>
    ///     This performs all version tracking for a particular virtual processor.  Only the scheduler touches this data structure.
    /// </summary>
    class SafePointMarker
    {
    public:

        /// <summary>
        ///     Construct a new safe point marker.
        /// </summary>
        SafePointMarker()
        {
            Reset();
        }

        /// <summary>
        ///     Reset a safe point marker.
        /// </summary>
        void Reset()
        {
            //
            // Zero is a special key indicating that it has made no data observations.
            //
            m_lastObservedVersion = 0;
        }

    private:

        friend class SchedulerBase;

        // The last observed version of data.
        ULONG m_lastObservedVersion;
    };

    /// <summary>
    ///     Adds a reference to a host module and then creates the thread. First reference is managed by LoadLibrary,
    ///     and all subsequent ones are reference counted internally to avoid LoadLibrary call overhead.
    /// </summary>
    HANDLE LoadLibraryAndCreateThread
    (
        LPSECURITY_ATTRIBUTES lpThreadAttributes,
        SIZE_T dwStackSize,
        LPTHREAD_START_ROUTINE lpStartAddress,
        LPVOID lpParameter,
        DWORD dwCreationFlags,
        LPDWORD lpThreadId
    );

    /// <summary>
    ///     Removes a reference count on a host module and in the case of last reference frees the library.
    /// </summary>
    void FreeLibraryAndDestroyThread(DWORD exitCode);
} // namespace details
} // namespace Concurrency
