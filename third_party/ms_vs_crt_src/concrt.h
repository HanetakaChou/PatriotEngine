/***
* ==++==
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
*
* ==--==
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*
* concrt.h
*
* Main public header file for ConcRT. This is the only header file a C++ program should
* have to include in order to avail itself of the core concurrency runtime features.
*
* Agents and PPL live in separate headers.
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/

#pragma once

#include <crtdefs.h>

#if !(defined (_M_AMD64) || defined (_M_IX86))
    #error ERROR: Concurrency Runtime is supported only on X64 and X86 architectures.
#endif  /* !(defined (_M_AMD64) || defined (_M_IX86)) */

#if defined (_M_CEE)
    #error ERROR: Concurrency Runtime is not supported when compiling /clr.
#endif  /* defined (_M_CEE) */

#ifndef __cplusplus
    #error ERROR: Concurrency Runtime is supported only for C++.
#endif  /* __cplusplus */

#include <exception>
#include <sal.h>
#include <limits.h>
#include <crtdbg.h>
#include <guiddef.h>
#include <intrin.h>
#include <new>

#pragma pack(push,_CRT_PACKING)

// Forward declare structs needed from windows headers

struct _SECURITY_ATTRIBUTES;
typedef _SECURITY_ATTRIBUTES* LPSECURITY_ATTRIBUTES;

// Define essential types needed from windows headers

typedef unsigned long DWORD;
typedef long HRESULT;
typedef void * HANDLE;

// Undefine Yield that is possibly defined by windows.h, and _YieldProcessor just in case

#undef Yield
#undef _YieldProcessor

#define _YieldProcessor _mm_pause

// Make sure exchange pointer intrinsics work on x86

#if defined (_M_IX86)

#undef _InterlockedExchangePointer
#undef _InterlockedCompareExchangePointer

#define _InterlockedExchangePointer(_Target, _Value) reinterpret_cast<void *>(static_cast<__w64 long>(_InterlockedExchange( \
    static_cast<long volatile *>(reinterpret_cast<__w64 long volatile *>(static_cast<void * volatile *>(_Target))), \
    static_cast<long>(reinterpret_cast<__w64 long>(static_cast<void *>(_Value))))))

#define _InterlockedCompareExchangePointer(_Target, _Exchange, _Comparand) reinterpret_cast<void *>(static_cast<__w64 long>(_InterlockedCompareExchange( \
    static_cast<long volatile *>(reinterpret_cast<__w64 long volatile *>(static_cast<void * volatile *>(_Target))), \
    static_cast<long>(reinterpret_cast<__w64 long>(static_cast<void *>(_Exchange))), \
    static_cast<long>(reinterpret_cast<__w64 long>(static_cast<void *>(_Comparand))))))

#define _InterlockedIncrementSizeT(_Target) _InterlockedIncrement(reinterpret_cast<long volatile *>(_Target))
#define _InterlockedDecrementSizeT(_Target) _InterlockedDecrement(reinterpret_cast<long volatile *>(_Target))
#define _InterlockedCompareExchangeSizeT(_Target, _Exchange, _Comparand) _InterlockedCompareExchange( \
    reinterpret_cast<long volatile *>(_Target), \
    static_cast<long>(_Exchange), \
    static_cast<long>(_Comparand))

#else  /* defined (_M_IX86) */

#define _InterlockedIncrementSizeT(_Target) _InterlockedIncrement64(reinterpret_cast<__int64 volatile *>(_Target))
#define _InterlockedDecrementSizeT(_Target) _InterlockedDecrement64(reinterpret_cast<__int64 volatile *>(_Target))
#define _InterlockedCompareExchangeSizeT(_Target, _Exchange, _Comparand) _InterlockedCompareExchange64( \
    reinterpret_cast<__int64 volatile *>(_Target), \
    static_cast<__int64>(_Exchange), \
    static_cast<__int64>(_Comparand))

#endif  /* defined (_M_IX86) */

// Used internally to represent the smallest unit in which to allocate hidden types

typedef void * _CONCRT_BUFFER;

/// <summary>
///     The <c>Concurrency</c> namespace provides classes and functions that give you access to the Concurrency Runtime,
///     a concurrent programming framework for C++. For more information, see <see cref="Concurrency Runtime"/>.
/// </summary>
/**/
namespace Concurrency
{
/// <summary>
///     Pauses the current context for a specified amount of time.
/// </summary>
/// <param name="_Milliseconds">
///     The number of milliseconds that the current context should be paused for.  If the <paramref name="_Milliseconds"/> parameter is set to
///     the value <c>0</c>, it indicates that the current context should yield execution to other runnable contexts before continuing.
/// </param>
/// <remarks>
///     If this method is called on a Concurrency Runtime scheduler context, the scheduler will find a different context to run on the underlying
///     resource. Since the scheduler is cooperative in nature, this context may not resume exactly after the number of milliseconds specified.
///     If the scheduler is busy executing other tasks that do not cooperatively yield to the scheduler in a hurry, the wait period could be
///     indefinite.
/// </remarks>
/**/
_CRTIMP void __cdecl wait(unsigned int _Milliseconds);

/// <summary>
///     Allocates a block of memory of the size specified from the Concurrency Runtime's Caching Suballocator.
/// </summary>
/// <param name="_NumBytes">
///     The number of bytes of memory to allocate.
/// </param>
/// <returns>
///     A pointer to newly allocated memory.
/// </returns>
/// <remarks>
///     For more information on what scenarios in your application could benefit from using the Caching Suballocator,
///     see <see cref="Task Scheduler (Concurrency Runtime)"/>.
/// </remarks>
/// <seealso cref="Concurrency::Free Function"/>
/**/
_CRTIMP void * __cdecl Alloc(size_t _NumBytes);

/// <summary>
///     Frees a block of memory previously allocated by the <c>Alloc</c> method to the Concurrency Runtime's Caching Suballocator.
/// </summary>
/// <param name="_PAllocation">
///     A pointer to memory previously allocated by the <c>Alloc</c> method which is to be freed. If the parameter <paramref name="_PAllocation"/>
///     is set to the value <c>NULL</c>, this method will ignore it and return immediately.
/// </param>
/// <remarks>
///     For more information on what scenarios in your application could benefit from using the Caching Suballocator,
///     see <see cref="Task Scheduler (Concurrency Runtime)"/>.
/// </remarks>
/// <seealso cref="Concurrency::Alloc Function"/>
/**/
_CRTIMP void __cdecl Free(void * _PAllocation);

/// <summary>
///     Concurrency::details contains definitions of support routines in the public namespaces and some macros.
///     Users should not directly interact with this internal namespace.
/// </summary>
/**/
namespace details
{
    //
    // Forward declarations:
    //
    class ContextBase;
    class _TaskCollectionBase;

    //
    // A utility to hide operator delete from certain objects while still allowing the runtime to delete them internally.
    //
    template<class _T>
    void _InternalDeleteHelper(_T * _PObject)
    {
        delete _PObject;
    }

    // This class's purpose is solely to direct allocations of ConcRT classes
    // through a single point, using internal allocator.
    struct _AllocBase
    {
        // Standard operator new
        void * operator new(size_t _Size)
        {
            return Concurrency::Alloc(_Size);
        }

        // Standard operator delete
        void operator delete(void * _Ptr) throw()
        {
            Concurrency::Free(_Ptr);
        }

        // Standard operator new, no-throw version
        void * operator new(size_t _Size, const std::nothrow_t&) throw()
        {
            void * _Ptr;

            try
            {
                _Ptr = Concurrency::Alloc(_Size);
            }
            catch(...)
            {
                _Ptr = NULL;
            }

            return (_Ptr);
        }

        // Standard operator delete, no-throw version
        void operator delete(void * _Ptr, const std::nothrow_t&) throw()
        {
            operator delete(_Ptr);
        }

        // Standard operator new array
        void * operator new[](size_t _Size)
        {
            return operator new(_Size);
        }

        // Standard operator delete array
        void operator delete[](void * _Ptr) throw()
        {
            operator delete(_Ptr);
        }

        // Standard operator new array, no-throw version
        void * operator new[](size_t _Size, const std::nothrow_t& _No_throw) throw ()
        {
            return operator new(_Size, _No_throw);
        }

        // Standard operator delete array, no-throw version
        void operator delete[](void * _Ptr, const std::nothrow_t& _No_throw) throw()
        {
            operator delete(_Ptr, _No_throw);
        }

        // Standard operator new with void* placement
        void * operator new(size_t, void * _Location) throw()
        {
            return _Location;
        }

        // Standard operator delete with void* placement
        void operator delete(void *, void *) throw()
        {
        }

        // Standard operator new array with void* placement
        void * __cdecl operator new[](size_t, void * _Location) throw()
        {
            return _Location;
        }

        // Standard operator delete array with void* placement
        void __cdecl operator delete[](void *, void *) throw()
        {
        }
    };

    //
    // Wrappers for atomic access
    //
    template <size_t _Size>
    struct _Subatomic_impl { };

    template<>
    struct _Subatomic_impl<4> {
        template <typename _Ty>
        static void _StoreWithRelease(volatile _Ty& _Location, _Ty _Rhs) {
            _ReadWriteBarrier();
            _Location = _Rhs;
        }

        template <typename _Ty>
        static _Ty _LoadWithAquire(volatile _Ty& _Location) {
            _ReadWriteBarrier();
            return _Location;
        }

        template <typename _Ty>
        static _Ty _CompareAndSwap(volatile _Ty& _Location, _Ty _NewValue, _Ty _Comperand) {
            return (_Ty)_InterlockedCompareExchange((volatile long*)&_Location, (long)_NewValue, (long)_Comperand);
        }

        template <typename _Ty>
        static _Ty _FetchAndAdd(volatile _Ty& _Location, _Ty _Addend) {
            return (_Ty)_InterlockedExchangeAdd((volatile long*)&_Location, (long)_Addend);
        }

        template <typename _Ty>
        static _Ty _Increment(volatile _Ty& _Location) {
            return (_Ty)_InterlockedIncrement((volatile long*)&_Location);
        }

        template <typename _Ty>
        static _Ty _Decrement(volatile _Ty& _Location) {
            return (_Ty)_InterlockedDecrement((volatile long*)&_Location);
        }
    };

#if defined (_M_X64)
    template<>
    struct _Subatomic_impl<8> {
        template <typename _Ty>
        static void _StoreWithRelease(volatile _Ty& _Location, _Ty _Rhs) {
            _ReadWriteBarrier();
            _Location = _Rhs;
        }

        template <typename _Ty>
        static _Ty _LoadWithAquire(volatile _Ty& _Location) {
            _ReadWriteBarrier();
            return _Location;
        }

        template <typename _Ty>
        static _Ty _CompareAndSwap(volatile _Ty& _Location, _Ty _NewValue, _Ty _Comperand) {
            return (_Ty)_InterlockedCompareExchange64((volatile __int64*)&_Location, (__int64)_NewValue, (__int64)_Comperand);
        }

        template <typename _Ty>
        static _Ty _FetchAndAdd(volatile _Ty& _Location, _Ty _Addend) {
            return (_Ty)_InterlockedExchangeAdd64((volatile __int64*)&_Location, (__int64)_Addend);
        }

        template <typename _Ty>
        static _Ty _Increment(volatile _Ty& _Location) {
            return (_Ty)_InterlockedIncrement64((volatile __int64*)&_Location);
        }

        template <typename _Ty>
        static _Ty _Decrement(volatile _Ty& _Location) {
            return (_Ty)_InterlockedDecrement64((volatile __int64*)&_Location);
        }
    };
#endif  /* defined (_M_X64) */


    //
    // Wrapper for atomic access.  Only works for 4-byte or 8-byte types (e.g., int, long, long long, size_t, pointer).
    // Anything else will likely fail to compile.
    //
    template <typename _Ty>
    class _Subatomic {
    private:
        volatile _Ty _M_value;

    public:
        operator _Ty() const volatile {
            return _Subatomic_impl<sizeof(_Ty)>::_LoadWithAquire(_M_value);
        }

        _Ty operator=(_Ty _Rhs) {
            _Subatomic_impl<sizeof(_Ty)>::_StoreWithRelease(_M_value, _Rhs);
            return _Rhs;
        }

        _Ty _CompareAndSwap(_Ty _NewValue, _Ty _Comperand) {
            return _Subatomic_impl<sizeof(_Ty)>::_CompareAndSwap(_M_value, _NewValue, _Comperand);
        }

        _Ty _FetchAndAdd(_Ty _Addend) {
            return _Subatomic_impl<sizeof(_Ty)>::_FetchAndAdd(_M_value, _Addend);
        }

        _Ty operator++() {
            return _Subatomic_impl<sizeof(_Ty)>::_Increment(_M_value);
        }

        _Ty operator++(int) {
            return _Subatomic_impl<sizeof(_Ty)>::_Increment(_M_value) - 1;
        }

        _Ty operator--() {
            return _Subatomic_impl<sizeof(_Ty)>::_Decrement(_M_value);
        }

        _Ty operator--(int) {
            return _Subatomic_impl<sizeof(_Ty)>::_Decrement(_M_value) + 1;
        }

        _Ty operator+=(_Ty _Addend) {
            return _FetchAndAdd(_Addend) + _Addend;
        }
    };

    //
    // An RAII class that spin-waits on a "rented" flag.
    //
    class _SpinLock
    {
    private:
        volatile long& _M_flag;

    public:
        _CRTIMP _SpinLock(volatile long& _Flag);
        _CRTIMP ~_SpinLock();

    private:
        _SpinLock(const _SpinLock&);
        void operator=(const _SpinLock&);
    };

    //
    // A class that holds the count used for spinning and is dependent
    // on the number of hardware threads
    //
    struct _SpinCount
    {
        // Initializes the spinCount to either 0 or SPIN_COUNT, depending on
        // the number of hardware threads.
        static void __cdecl _Initialize();

        // Returns the current value of s_spinCount
        _CRTIMP static unsigned int __cdecl _Value();

        // The number of iterations used for spinning
        static unsigned int _S_spinCount;
    };

    /// <summary>
    ///     Default method for yielding during a spin wait
    /// </summary>
    /**/
    void _CRTIMP __cdecl _UnderlyingYield();

    /// <summary>
    ///     Implements busy wait with no backoff
    /// </summary>
    /**/
    template<unsigned int _YieldCount = 1>
    class _CRTIMP _SpinWait
    {
    public:

        typedef void (__cdecl *_YieldFunction)();

        /// <summary>
        ///     Construct a spin wait object
        /// </summary>
        /**/
        _SpinWait(_YieldFunction _YieldMethod = _UnderlyingYield)
            : _M_yieldFunction(_YieldMethod), _M_state(_StateInitial)
        {
            // Defer initialization of other fields to _SpinOnce().
        }

        /// <summary>
        ///     Set a dynamic spin count.
        /// </summary>
        /**/
        void _SetSpinCount(unsigned int _Count)
        {
            _ASSERTE(_M_state == _StateInitial);
            if (_Count == 0)
            {
                // Specify a count of 0 if we are on a single proc.
                _M_state = _StateSingle;
            }
            else
            {
                _M_currentSpin = _Count;
                _M_currentYield = _YieldCount;
                _M_state = _StateSpin;
            }
        }

        /// <summary>
        ///     Spins for one time quantum,until a maximum spin is reached.
        /// </summary>
        /// <returns>
        ///     false if spin count has reached steady state, true otherwise.
        /// </returns>
        /// <remarks>
        ///     If the spin count is not changing that means that it is probably not a good idea to spin again
        ///     because there is either only one processor, or maximum spin has been reached and blocking is
        ///     probably a better solution. However, if called again, SpinOnce will spin for a maximum spin count.
        /// </remarks>
        /**/
        bool _SpinOnce()
        {
            switch (_M_state)
            {
            case _StateSpin:
            {
                unsigned long _Count = _NumberOfSpins();

                for (unsigned long _I = 0; _I < _Count; _I++)
                {
                    _YieldProcessor();
                }

                if (!_ShouldSpinAgain())
                {
                    _M_state = (_M_currentYield == 0) ? _StateBlock : _StateYield;
                }

                return true;
            }

            case _StateYield:
                _ASSERTE(_M_currentYield > 0);
                if (--_M_currentYield == 0)
                {
                    _M_state = _StateBlock;
                }

                // Execute the yield
                _DoYield();
                return true;

            case _StateBlock:
                // Reset to defaults if client does not block
                _Reset();
                return false;

            case _StateSingle:
                // No need to spin on a single processor: just execute the yield
                _DoYield();
                return false;

            case _StateInitial:
                // Reset counters to their default value and Spin once.
                _Reset();
                return _SpinOnce();
            default:
                // Unreached
                return false;
            };
        }

    protected:

        /// <summary>
        ///     State of the spin wait class.
        /// </summary>
        /**/
        enum _SpinState
        {
            _StateInitial,
            _StateSpin,
            _StateYield,
            _StateBlock,
            _StateSingle
        };

        /// <summary>
        ///     Yields its time slice using the specified yieldFunciton
        /// </summary>
        /**/
        void _DoYield()
        {
            bool _ShouldYield = (_YieldCount != 0);
            if (_ShouldYield)
            {
                _ASSERTE(_M_yieldFunction != NULL);
                _M_yieldFunction();
            }
            else
            {
                _YieldProcessor();
            }
        }

        /// <summary>
        ///     Resets the counts and state to the default.
        /// </summary>
        /**/
        void _Reset()
        {
            _M_state = _StateInitial;

            // Reset to the default spin value. The value specified
            // by the client is ignored on a reset.
            _SetSpinCount(_SpinCount::_Value());

            _ASSERTE(_M_state != _StateInitial);
        }

        /// <summary>
        ///     Determines the current spin count
        /// </summary>
        /// <returns>
        ///     The number of spins to execute for this iteration
        /// </returns>
        /**/
        unsigned long _NumberOfSpins()
        {
            return 1;
        }

        /// <summary>
        ///     Determines whether maximum spin has been reached
        /// </summary>
        /// <returns>
        ///     false if spin count has reached steady state, true otherwise.
        /// </returns>
        /**/
        bool _ShouldSpinAgain()
        {
            return (--_M_currentSpin > 0);
        }

        unsigned long  _M_currentSpin;
        unsigned long  _M_currentYield;
        _SpinState     _M_state;
        _YieldFunction _M_yieldFunction;
    };

    typedef _SpinWait<>   _SpinWaitBackoffNone;
    typedef _SpinWait<0>  _SpinWaitNoYield;

    //
    // This reentrant lock uses CRITICAL_SECTION and is intended for use when kernel blocking
    // is desirable and where it is either known that the lock will be taken recursively in
    // the same thread, or when it is just not known that a non-reentrant lock can be used safely.
    //
    class _ReentrantBlockingLock
    {
    public:
        // Constructor for _ReentrantBlockingLock
        _CRTIMP _ReentrantBlockingLock();

        // Destructor for _ReentrantBlockingLock
        _CRTIMP ~_ReentrantBlockingLock();

        // Acquire the lock, spin if necessary
        _CRTIMP void _Acquire();

        // Tries to acquire the lock, does not spin.
        // Returns true if the acquisition worked, false otherwise
        _CRTIMP bool _TryAcquire();

        // Releases the lock
        _CRTIMP void _Release();


        // An exception safe RAII wrapper.
        class _Scoped_lock
        {
        public:
            // Constructs a holder and acquires the specified lock
            explicit _Scoped_lock(_ReentrantBlockingLock& _Lock) : _M_lock(_Lock)
            {
                _M_lock._Acquire();
            }

            // Destroys the holder and releases the lock
            ~_Scoped_lock()
            {
                _M_lock._Release();
            }
        private:
            _ReentrantBlockingLock& _M_lock;

            _Scoped_lock(const _Scoped_lock&);                    // no copy constructor
            _Scoped_lock const & operator=(const _Scoped_lock&);  // no assignment operator
        };

    private:
        // Critical section requires windows.h. Hide the implementation so that
        // user code need not include windows.
        _CONCRT_BUFFER _M_criticalSection[(4 * sizeof(void *) + 2 * sizeof(long) + sizeof(_CONCRT_BUFFER) - 1) / sizeof(_CONCRT_BUFFER)];
    };

    //
    // This reentrant lock is a pure spin lock and is intended for use when kernel blocking
    // is desirable and where it is either known that the lock will be taken recursively in
    // the same thread, or when it is just not known that a non-reentrant lock can be used safely.
    //
    class _ReentrantLock
    {
    public:
        // Constructor for _ReentrantLock
        _CRTIMP _ReentrantLock();

        // Acquire the lock, spin if necessary
        _CRTIMP void _Acquire();

        // Tries to acquire the lock, does not spin
        // Returns true if the acquisition worked, false otherwise
        _CRTIMP bool _TryAcquire();

        // Releases the lock
        _CRTIMP void _Release();

        // An exception safe RAII wrapper.
        class _Scoped_lock
        {
        public:
            // Constructs a holder and acquires the specified lock
            explicit _Scoped_lock(_ReentrantLock& _Lock) : _M_lock(_Lock)
            {
                _M_lock._Acquire();
            }

            // Destroys the holder and releases the lock
            ~_Scoped_lock()
            {
                _M_lock._Release();
            }
        private:
            _ReentrantLock& _M_lock;

            _Scoped_lock(const _Scoped_lock&);                    // no copy constructor
            _Scoped_lock const & operator=(const _Scoped_lock&);  // no assignment operator
        };

    private:
        long _M_recursionCount;
        volatile long _M_owner;
    };

    //
    // This non-reentrant lock uses CRITICAL_SECTION and is intended for use in situations
    // where it is known that the lock will not be taken recursively, and can thus be more
    // efficiently implemented.
    //
    class _NonReentrantBlockingLock
    {
    public:
        // Constructor for _NonReentrantBlockingLock
        //
        // The constructor is exported because of _NonReentrantLock's
        // inclusion in DevUnitTests.  It is also used in RM.
        _CRTIMP _NonReentrantBlockingLock();

        // Constructor for _NonReentrantBlockingLock
        _CRTIMP ~_NonReentrantBlockingLock();

        // Acquire the lock, spin if necessary
        _CRTIMP void _Acquire();

        // Tries to acquire the lock, does not spin
        // Returns true if the lock is taken, false otherwise
        _CRTIMP bool _TryAcquire();

        // Releases the lock
        _CRTIMP void _Release();

        // An exception safe RAII wrapper.
        class _Scoped_lock
        {
        public:
            // Constructs a holder and acquires the specified lock
            explicit _Scoped_lock(_NonReentrantBlockingLock& _Lock) : _M_lock(_Lock)
            {
                _M_lock._Acquire();
            }

            // Destroys the holder and releases the lock
            ~_Scoped_lock()
            {
                _M_lock._Release();
            }
        private:
            _NonReentrantBlockingLock& _M_lock;

            _Scoped_lock(const _Scoped_lock&);                    // no copy constructor
            _Scoped_lock const & operator=(const _Scoped_lock&);  // no assignment operator
        };

    private:
        // Critical section requires windows.h. Hide the implementation so that
        // user code need not include windows.h
        _CONCRT_BUFFER _M_criticalSection[(4 * sizeof(void *) + 2 * sizeof(long) + sizeof(_CONCRT_BUFFER) - 1) / sizeof(_CONCRT_BUFFER)];
    };

    //
    // A Reader-Writer Lock is intended for use in situations with many readers and rare
    // writers.
    //
    // A writer request immediately blocks future readers and then waits until all current
    // readers drain.  A reader request does not block future writers and must wait until
    // all writers are done, even those that cut in front of it.  In any race between a
    // reader and a writer, the writer always wins.
    //
    class _ReaderWriterLock
    {
    public:
        // Constructor for _ReaderWriterLock
        //
        // The constructor and destructor are exported because of _ReaderWriterLock's
        // inclusion in DevUnitTests.  We may want to revisit whether we actually want
        // to export this in the future.
        _CRTIMP _ReaderWriterLock();

        // Acquire lock for reading.  Spins until all writers finish, new writers
        // can cut in front of a waiting reader.
        _CRTIMP void _AcquireRead();

        // Release lock for reading.  The last reader changes m_state to State.kFree
        _CRTIMP void _ReleaseRead();

        // Acquire lock for writing. Spin until no readers exist, then acquire lock
        // and prevent new readers.
        _CRTIMP void _AcquireWrite();

        // Release lock for writing.
        _CRTIMP void _ReleaseWrite();

        // Try to acquire the write lock, do not spin if unable to acquire.
        // Returns true if the acquisition worked, false otherwise
        _CRTIMP bool _TryAcquireWrite();

        // Returns true if it is in write state, false otherwise
        bool _HasWriteLock() const
        {
            return (_M_state == _Write);
        }

        // Guarantees that all writers are out of the lock.  This does nothing if there are no pending writers.
        void _FlushWriteOwners();

        // An exception safe RAII wrapper.
        class _Scoped_lock
        {
        public:
            // Constructs a holder and acquires the writer lock
            explicit _Scoped_lock(_ReaderWriterLock& _Lock) : _M_lock(_Lock)
            {
                _M_lock._AcquireWrite();
            }

            // Destroys the holder and releases the writer lock
            ~_Scoped_lock()
            {
                _M_lock._ReleaseWrite();
            }

        private:

            _ReaderWriterLock& _M_lock;

            _Scoped_lock(const _Scoped_lock&);                    // no copy constructor
            _Scoped_lock const & operator=(const _Scoped_lock&);  // no assignment operator
        };

        // An exception safe RAII wrapper for reads.
        class _Scoped_lock_read
        {
        public:
            // Constructs a holder and acquires the reader lock
            explicit _Scoped_lock_read(_ReaderWriterLock& _Lock) : _M_lock(_Lock)
            {
                _M_lock._AcquireRead();
            }

            // Destroys the holder and releases the reader lock
            ~_Scoped_lock_read()
            {
                _M_lock._ReleaseRead();
            }

        private:

            _ReaderWriterLock& _M_lock;

            _Scoped_lock_read(const _Scoped_lock_read&);                    // no copy constructor
            _Scoped_lock_read const & operator=(const _Scoped_lock_read&);  // no assignment operator
        };

    private:
        // State enum where:
        // -1    --> write mode
        // 0     --> free
        // n > 0 --> n readers have locked in read mode.
        enum _State
        {
          _Write = -1,
          _Free  = 0,
          _Read  = 1
        };

        // The current state of the lock, mapping to the State enum.  This is also
        // an indicator of the number of readers holding the lock, for any number > 0.
        volatile long _M_state;

        // A writer increments this as soon as it wants to lock and decrements this
        // after releasing the lock. To prevent writers from starving, a reader will
        // wait until this counter is zero, and only then will try to obtain the lock.
        volatile long _M_numberOfWriters;

        // Spin-Wait-Until variant
        static void __cdecl _WaitEquals(volatile const long& _Location, long _Value, long _Mask = 0xFFFFFFFF);
    };

    //
    // An exception safe RAII wrapper for  _malloca()
    //
    class _MallocaHolder
    {
    public:

        _MallocaHolder(void *pMemory) : m_pMemory(pMemory)
        {
        }

        ~_MallocaHolder()
        {
            _freea(m_pMemory);
        }

    private:

        void *m_pMemory;
    };

    // Forward declarations
    class _StructuredTaskCollection;
    class _TaskCollection;
    class _UnrealizedChore;
} // namespace details

//**************************************************************************
// Public Namespace:
//
// Anything in the Concurrency namespace is intended for direct client consumption.
//
//**************************************************************************

//
// Forward declarations:
//
class Scheduler;

/// <summary>
///     This class describes an exception that is thrown due to failure to acquire a critical resource in the Concurrency Runtime.
/// </summary>
/// <remarks>
///     This exception is typically thrown when a call to the operating system from within the Concurrency Runtime
///     fails.  The error code which would normally be returned from a call to the Win32 method <c>GetLastError</c> is
///     converted to a value of type <c>HRESULT</c> and can be retrieved via the <c>get_error_code</c> method.
/// </remarks>
/**/
class scheduler_resource_allocation_error : public std::exception
{
public:
    /// <summary>
    ///     Constructs a <c>scheduler_resource_allocation_error</c> object.
    /// </summary>
    /// <param name="_Message">
    ///     A descriptive message of the error.
    /// </param>
    /// <param name="_Hresult">
    ///     The <c>HRESULT</c> value of the error that caused the exception.
    /// </param>
    /**/
    _CRTIMP scheduler_resource_allocation_error(const char * _Message, HRESULT _Hresult) throw();

    /// <summary>
    ///     Constructs a <c>scheduler_resource_allocation_error</c> object.
    /// </summary>
    /// <param name="_Hresult">
    ///     The <c>HRESULT</c> value of the error that caused the exception.
    /// </param>
    /**/
    explicit _CRTIMP scheduler_resource_allocation_error(HRESULT _Hresult) throw();

    /// <summary>
    ///     Returns the error code that caused the exception.
    /// </summary>
    /// <returns>
    ///     The <c>HRESULT</c> value of the error that caused the exception.
    /// </returns>
    /**/
    _CRTIMP HRESULT get_error_code() const throw();

private:
    HRESULT _Hresult;
};

/// <summary>
///     This class describes an exception that is thrown whenever an unsupported operating system is used.
///     The Concurrency Runtime does not support operating systems earlier than Windows XP with Service Pack 3.
/// </summary>
/**/
class unsupported_os  : public std::exception
{
public:
    /// <summary>
    ///     Constructs an <c>unsupported_os</c> object.
    /// </summary>
    /// <param name="_Message">
    ///     A descriptive message of the error.
    /// </param>
    /**/
    explicit _CRTIMP unsupported_os(const char * _Message) throw();

    /// <summary>
    ///     Constructs an <c>unsupported_os</c> object.
    /// </summary>
    /**/
    _CRTIMP unsupported_os() throw();
};

/// <summary>
///     This class describes an exception that is thrown whenever an operation is performed which requires a scheduler
///     to be attached to the current context and one is not.
/// </summary>
/// <seealso cref="Scheduler Class"/>
/// <seealso cref="Scheduler::Attach Method"/>
/**/
class scheduler_not_attached  : public std::exception
{
public:
    /// <summary>
    ///     Constructs a <c>scheduler_not_attached</c> object.
    /// </summary>
    /// <param name="_Message">
    ///     A descriptive message of the error.
    /// </param>
    /**/
    explicit _CRTIMP scheduler_not_attached(const char * _Message) throw();

    /// <summary>
    ///     Constructs a <c>scheduler_not_attached</c> object.
    /// </summary>
    /**/
    _CRTIMP scheduler_not_attached() throw();
};

/// <summary>
///     This class describes an exception that is thrown whenever the <c>Attach</c> method is called on a <c>Scheduler</c>
///     object which is already attached to the current context.
/// </summary>
/// <seealso cref="Scheduler Class"/>
/// <seealso cref="Scheduler::Attach Method"/>
/**/
class improper_scheduler_attach : public std::exception
{
public:
    /// <summary>
    ///     Constructs an <c>improper_scheduler_attach</c> object.
    /// </summary>
    /// <param name="_Message">
    ///     A descriptive message of the error.
    /// </param>
    /**/
    explicit _CRTIMP improper_scheduler_attach(const char * _Message) throw();

    /// <summary>
    ///     Constructs an <c>improper_scheduler_attach</c> object.
    /// </summary>
    /**/
    _CRTIMP improper_scheduler_attach() throw();
};

/// <summary>
///     This class describes an exception that is thrown whenever the <c>CurrentScheduler::Detach</c> method is called on
///     a context which has not been attached to any scheduler via the <c>Attach</c> method of a <c>Scheduler</c> object.
/// </summary>
/// <seealso cref="Scheduler Class"/>
/// <seealso cref="CurrentScheduler::Detach Method"/>
/// <seealso cref="Scheduler::Attach Method"/>
/**/
class improper_scheduler_detach : public std::exception
{
public:

    /// <summary>
    ///     Constructs an <c>improper_scheduler_detach</c> object.
    /// </summary>
    /// <param name="_Message">
    ///     A descriptive message of the error.
    /// </param>
    /**/
    explicit _CRTIMP improper_scheduler_detach(const char * _Message) throw();

    /// <summary>
    ///     Constructs an <c>improper_scheduler_detach</c> object.
    /// </summary>
    /**/
    _CRTIMP improper_scheduler_detach() throw();
};

/// <summary>
///     This class describes an exception that is thrown whenever the <c>Reference</c> method is called on a <c>Scheduler</c>
///     object that is shutting down, from a context that is not part of that scheduler.
/// </summary>
/// <seealso cref="Scheduler Class"/>
/// <seealso cref="Scheduler::Reference Method"/>
/**/
class improper_scheduler_reference : public std::exception
{
public:

    /// <summary>
    ///     Constructs an <c>improper_scheduler_reference</c> object.
    /// </summary>
    /// <param name="_Message">
    ///     A descriptive message of the error.
    /// </param>
    /**/
    explicit _CRTIMP improper_scheduler_reference(const char* _Message) throw();

    /// <summary>
    ///     Constructs an <c>improper_scheduler_reference</c> object.
    /// </summary>
    /**/
    _CRTIMP improper_scheduler_reference() throw();
};

/// <summary>
///     This class describes an exception that is thrown whenever the <c>Scheduler::SetDefaultSchedulerPolicy</c> method is
///     called when a default scheduler already exists within the process.
/// </summary>
/// <seealso cref="Scheduler::SetDefaultSchedulerPolicy Method"/>
/**/
class default_scheduler_exists : public std::exception
{
public:
    /// <summary>
    ///     Constructs a <c>default_scheduler_exists</c> object.
    /// </summary>
    /// <param name="_Message">
    ///     A descriptive message of the error.
    /// </param>
    /**/
    explicit _CRTIMP default_scheduler_exists(const char * _Message) throw();

    /// <summary>
    ///     Constructs a <c>default_scheduler_exists</c> object.
    /// </summary>
    /**/
    _CRTIMP default_scheduler_exists() throw();
};

/// <summary>
///     This class describes an exception that is thrown whenever calls to the <c>Block</c> and <c>Unblock</c> methods of a
///     <c>Context</c> object are not properly paired.
/// </summary>
/// <remarks>
///     Calls to the <c>Block</c> and <c>Unblock</c> methods of a <c>Context</c> object must always be properly paired.
///     The Concurrency Runtime allows the operations to happen in either order. For example, a call to <c>Block</c>
///     may followed by a call to <c>Unblock</c>, or vice-versa.  This exception would be thrown if, for instance, two calls to the
///     <c>Unblock</c> method were made in a row, on a <c>Context</c> object which was not blocked.
/// </remarks>
/// <seealso cref="Context Class"/>
/// <seealso cref="Context::Unblock Method"/>
/// <seealso cref="Context::Block Method"/>
/**/
class context_unblock_unbalanced : public std::exception
{
public:
    /// <summary>
    ///     Constructs a <c>context_unblock_unbalanced</c> object.
    /// </summary>
    /// <param name="_Message">
    ///     A descriptive message of the error.
    /// </param>
    /**/
    explicit _CRTIMP context_unblock_unbalanced(const char * _Message) throw();

    /// <summary>
    ///     Constructs a <c>context_unblock_unbalanced</c> object.
    /// </summary>
    /**/
    _CRTIMP context_unblock_unbalanced() throw();
};

/// <summary>
///     This class describes an exception that is thrown whenever the <c>Unblock</c> method of a <c>Context</c> object is called
///     from the same context.  This would indicate an attempt by a given context to unblock itself.
/// </summary>
/// <seealso cref="Context Class"/>
/// <seealso cref="Context::Unblock Method"/>
/**/
class context_self_unblock : public std::exception
{
public:
    /// <summary>
    ///     Constructs a <c>context_self_unblock</c> object.
    /// </summary>
    /// <param name="_Message">
    ///     A descriptive message of the error.
    /// </param>
    /**/
    explicit _CRTIMP context_self_unblock(const char * _Message) throw();

    /// <summary>
    ///     Constructs a <c>context_self_unblock</c> object.
    /// </summary>
    /**/
    _CRTIMP context_self_unblock() throw();
};

/// <summary>
///     This class describes an exception that is thrown whenever there are tasks still scheduled to a <c>task_group</c> or
///     <c>structured_task_group</c> object at the time that object's destructor executes.  This exception will never be thrown
///     if the destructor is reached due to stack unwinding as the result of an exception.
/// </summary>
/// <remarks>
///     Absent exception flow, you are responsible for calling either the <c>wait</c> or <c>run_and_wait</c> method of a <c>task_group</c> or
///     <c>structured_task_group</c> object before allowing that object to destruct.  The runtime throws this exception as an
///     indication that you forgot to call the <c>wait</c> or <c>run_and_wait</c> method.
/// </remarks>
/// <seealso cref="task_group Class"/>
/// <seealso cref="task_group::wait Method"/>
/// <seealso cref="task_group::run_and_wait Method"/>
/// <seealso cref="structured_task_group Class"/>
/// <seealso cref="structured_task_group::wait Method"/>
/// <seealso cref="structured_task_group::run_and_wait Method"/>
/**/
class missing_wait : public std::exception
{
public:
    /// <summary>
    ///     Constructs a <c>missing_wait</c> object.
    /// </summary>
    /// <param name="_Message">
    ///     A descriptive message of the error.
    /// </param>
    /**/
    explicit _CRTIMP missing_wait(const char * _Message) throw();

    /// <summary>
    ///     Constructs a <c>missing_wait</c> object.
    /// </summary>
    /**/
    _CRTIMP missing_wait() throw();
};

/// <summary>
///     This class describes an exception that is thrown whenever a messaging block is given a pointer to a target which is
///     invalid for the operation being performed.
/// </summary>
/// <remarks>
///     This exception is typically thrown for reasons such as a target attempting to consume a message which is reserved
///     for a different target or releasing a reservation that it does not hold.
/// </remarks>
/// <seealso cref="Asynchronous Message Blocks"/>
/**/
class bad_target : public std::exception
{
public:
    /// <summary>
    ///     Constructs a <c>bad_target</c> object.
    /// </summary>
    /// <param name="_Message">
    ///     A descriptive message of the error.
    /// </param>
    /**/
    explicit _CRTIMP bad_target(const char * _Message) throw();

    /// <summary>
    ///     Constructs a <c>bad_target</c> object.
    /// </summary>
    /**/
    _CRTIMP bad_target() throw();
};

/// <summary>
///     This class describes an exception that is thrown whenever a messaging block is unable to find a requested message.
/// </summary>
/// <seealso cref="Asynchronous Message Blocks"/>
/**/
class message_not_found : public std::exception
{
public:
    /// <summary>
    ///     Constructs a <c>message_not_found</c> object.
    /// </summary>
    /// <param name="_Message">
    ///     A descriptive message of the error.
    /// </param>
    /**/
    explicit _CRTIMP message_not_found(const char * _Message) throw();

    /// <summary>
    ///     Constructs a <c>message_not_found</c> object.
    /// </summary>
    /**/
    _CRTIMP message_not_found() throw();
};

/// <summary>
///     This class describes an exception that is thrown whenever the <c>link_target</c> method of a messaging block is
///     called and the messaging block is unable to link to the target.  This may be the result of exceeding the number of
///     links the messaging block is allowed or attempting to link a specific target twice to the same source.
/// </summary>
/// <seealso cref="Asynchronous Message Blocks"/>
/**/
class invalid_link_target : public std::exception
{
public:
    /// <summary>
    ///     Constructs an <c>invalid_link_target</c> object.
    /// </summary>
    /// <param name="_Message">
    ///     A descriptive message of the error.
    /// </param>
    /**/
    explicit _CRTIMP invalid_link_target(const char * _Message) throw();

    /// <summary>
    ///     Constructs an <c>invalid_link_target</c> object.
    /// </summary>
    /**/
    _CRTIMP invalid_link_target() throw();
};

/// <summary>
///     This class describes an exception that is thrown whenever an invalid or unknown key is passed to a <c>SchedulerPolicy</c>
///     object constructor, or the <c>SetPolicyValue</c> method of a <c>SchedulerPolicy</c> object is passed a key that must
///     be changed via other means such as the <c>SetConcurrencyLimits</c> method.
/// </summary>
/// <seealso cref="SchedulerPolicy Class"/>
/// <seealso cref="PolicyElementKey Enumeration"/>
/// <seealso cref="SchedulerPolicy::SetPolicyValue Method"/>
/// <seealso cref="SchedulerPolicy::SetConcurrencyLimits Method"/>
/**/
class invalid_scheduler_policy_key : public std::exception
{
public:
    /// <summary>
    ///     Constructs an <c>invalid_scheduler_policy_key</c> object.
    /// </summary>
    /// <param name="_Message">
    ///     A descriptive message of the error.
    /// </param>
    /**/
    explicit _CRTIMP invalid_scheduler_policy_key(const char * _Message) throw();

    /// <summary>
    ///     Constructs an <c>invalid_scheduler_policy_key</c> object.
    /// </summary>
    /**/
    _CRTIMP invalid_scheduler_policy_key() throw();
};

/// <summary>
///     This class describes an exception that is thrown whenever a policy key of a <c>SchedulerPolicy</c> object is
///     set to an invalid value for that key.
/// </summary>
/// <seealso cref="SchedulerPolicy Class"/>
/// <seealso cref="PolicyElementKey Enumeration"/>
/// <seealso cref="SchedulerPolicy::SetPolicyValue Method"/>
/// <seealso cref="SchedulerPolicy::SetConcurrencyLimits Method"/>
/**/
class invalid_scheduler_policy_value : public std::exception
{
public:
    /// <summary>
    ///     Constructs an <c>invalid_scheduler_policy_value</c> object.
    /// </summary>
    /// <param name="_Message">
    ///     A descriptive message of the error.
    /// </param>
    /**/
    explicit _CRTIMP invalid_scheduler_policy_value(const char * _Message) throw();

    /// <summary>
    ///     Constructs an <c>invalid_scheduler_policy_value</c> object.
    /// </summary>
    /**/
    _CRTIMP invalid_scheduler_policy_value() throw();
};

/// <summary>
///     This class describes an exception that is thrown whenever an attempt is made to set the concurrency limits of a
///     <c>SchedulerPolicy</c> object such that the value of the <c>MinConcurrency</c> key is less than the value of the
///     <c>MaxConcurrency</c> key.
/// </summary>
/// <seealso cref="SchedulerPolicy Class"/>
/// <seealso cref="PolicyElementKey Enumeration"/>
/// <seealso cref="SchedulerPolicy::SetConcurrencyLimits Method"/>
/**/
class invalid_scheduler_policy_thread_specification : public std::exception
{
public:
    /// <summary>
    ///     Constructs an <c>invalid_scheduler_policy_value</c> object.
    /// </summary>
    /// <param name="_Message">
    ///     A descriptive message of the error.
    /// </param>
    /**/
    explicit _CRTIMP invalid_scheduler_policy_thread_specification(const char * _Message) throw();

    /// <summary>
    ///     Constructs an <c>invalid_scheduler_policy_value</c> object.
    /// </summary>
    /**/
    _CRTIMP invalid_scheduler_policy_thread_specification() throw();
};

/// <summary>
///     This class describes an exception that is thrown when an invalid operation is performed that is not more accurately
///     described by another exception type thrown by the Concurrency Runtime.
/// </summary>
/// <remarks>
///     The various methods which throw this exception will generally document under what circumstances they will throw it.
/// </remarks>
/**/
class invalid_operation : public std::exception
{
public:
    /// <summary>
    ///     Constructs an <c>invalid_operation</c> object.
    /// </summary>
    /// <param name="_Message">
    ///     A descriptive message of the error.
    /// </param>
    /**/
    explicit _CRTIMP invalid_operation(const char * _Message) throw();

    /// <summary>
    ///     Constructs an <c>invalid_operation</c> object.
    /// </summary>
    /**/
    _CRTIMP invalid_operation() throw();
};

/// <summary>
///     This class describes an exception that is thrown when the Concurrency Runtime detects that you neglected to call the
///     <c>CurrentScheduler::Detach</c> method on a context that attached to a second scheduler via the <c>Attach</c> method
///     of the <c>Scheduler</c> object.
/// </summary>
/// <remarks>
///     This exception is only thrown when you nest one scheduler inside another by calling the <c>Attach</c> method of a
///     <c>Scheduler</c> object on a context that is already owned by or attached to another scheduler.  The Concurrency Runtime
///     throws this exception opportunistically when it can detect the scenario as an aid to locating the problem.  Not every
///     instance of neglecting to call the <c>CurrentScheduler::Detach</c> method is guaranteed to throw this exception.
/// </remarks>
/// <seealso cref="Scheduler Class"/>
/// <seealso cref="CurrentScheduler::Detach Method"/>
/// <seealso cref="Scheduler::Attach Method"/>
/**/
class nested_scheduler_missing_detach : public std::exception
{
public:
    /// <summary>
    ///     Constructs a <c>nested_scheduler_missing_detach</c> object.
    /// </summary>
    /// <param name="_Message">
    ///     A descriptive message of the error.
    /// </param>
    /**/
    explicit _CRTIMP nested_scheduler_missing_detach(const char * _Message) throw();

    /// <summary>
    ///     Constructs a <c>nested_scheduler_missing_detach</c> object.
    /// </summary>
    /**/
    _CRTIMP nested_scheduler_missing_detach() throw();
};

/// <summary>
///     This class describes an exception that is thrown when an operation has timed out.
/// </summary>
/**/
class operation_timed_out : public std::exception
{
public:
    /// <summary>
    ///     Constructs an <c>operation_timed_out</c> object.
    /// </summary>
    /// <param name="_Message">
    ///     A descriptive message of the error.
    /// </param>
    /**/
    explicit _CRTIMP operation_timed_out(const char * _Message) throw();

    /// <summary>
    ///     Constructs an <c>operation_timed_out</c> object.
    /// </summary>
    /**/
    _CRTIMP operation_timed_out() throw();
};

/// <summary>
///     This class describes an exception that is thrown when a <c>task_handle</c> object is scheduled multiple times
///     via the <c>run</c> method of a <c>task_group</c> or <c>structured_task_group</c> object without an intervening
///     call to either the <c>wait</c> or <c>run_and_wait</c> methods.
/// </summary>
/// <seealso cref="task_handle Class"/>
/// <seealso cref="task_group Class"/>
/// <seealso cref="task_group::run Method"/>
/// <seealso cref="task_group::wait Method"/>
/// <seealso cref="task_group::run_and_wait Method"/>
/// <seealso cref="structured_task_group Class"/>
/// <seealso cref="structured_task_group::run Method"/>
/// <seealso cref="structured_task_group::wait Method"/>
/// <seealso cref="structured_task_group::run_and_wait Method"/>
/**/
class invalid_multiple_scheduling : public std::exception
{
public:
    /// <summary>
    ///     Constructs an <c>invalid_multiple_scheduling</c> object.
    /// </summary>
    /// <param name="_Message">
    ///     A descriptive message of the error.
    /// </param>
    /**/
    explicit _CRTIMP invalid_multiple_scheduling(const char * _Message) throw();

    /// <summary>
    ///     Constructs an <c>invalid_multiple_scheduling</c> object.
    /// </summary>
    /**/
    _CRTIMP invalid_multiple_scheduling() throw();
};

/// <summary>
///     This class describes an exception that is thrown when the <c>Context::Oversubscribe</c> method is called with
///     the <paramref name="_BeginOversubscription"/> parameter set to <c>false</c> without a prior call to the
///     <c>Context::Oversubscribe</c> method with the <paramref name="_BeginOversubscription"/> parameter set to <c>true</c>.
/// </summary>
/// <seealso cref="Context::Oversubscribe Method"/>
/**/
class invalid_oversubscribe_operation : public std::exception
{
public:
    /// <summary>
    ///     Constructs an <c>invalid_oversubscribe_operation</c> object.
    /// </summary>
    /// <param name="_Message">
    ///     A descriptive message of the error.
    /// </param>
    /**/
    explicit _CRTIMP invalid_oversubscribe_operation(const char * _Message) throw();

    /// <summary>
    ///     Constructs an <c>invalid_oversubscribe_operation</c> object.
    /// </summary>
    /**/
    _CRTIMP invalid_oversubscribe_operation() throw();
};

/// <summary>
///     This class describes an exception that is thrown whenever a lock is acquired improperly.
/// </summary>
/// <remarks>
///     Typically, this exception is thrown when an attempt is made to acquire a non-reentrant lock
///     recursively on the same context.
/// </remarks>
/// <seealso cref="critical_section Class"/>
/// <seealso cref="reader_writer_lock Class"/>
/**/
class improper_lock : public std::exception
{
public:

    /// <summary>
    ///     Constructs an <c>improper_lock exception</c>.
    /// </summary>
    /// <param name="_Message">
    ///     A descriptive message of the error.
    /// </param>
    /**/
    explicit _CRTIMP improper_lock(const char * _Message) throw();

    /// <summary>
    ///     Constructs an <c>improper_lock</c> exception.
    /// </summary>
    /**/
    _CRTIMP improper_lock() throw();
};

/// <summary>
///     An elementary abstraction for a task, defined as <c>void (__cdecl * TaskProc)(void *)</c>. A <c>TaskProc</c> is called to
///     invoke the body of a task.
/// </summary>
/**/
typedef void (__cdecl * TaskProc)(void *);

/// <summary>
///     Represents an abstraction for a schedule group.  Schedule groups organize a set of related work that benefits from being
///     scheduled close together either temporally, by executing another task in the same group before moving to another group, or
///     spatially, by executing multiple items within the same group on the same NUMA node or physical socket.
/// </summary>
/// <seealso cref="CurrentScheduler Class"/>
/// <seealso cref="Scheduler Class"/>
/// <seealso cref="Task Scheduler (Concurrency Runtime)"/>
/**/
class ScheduleGroup
{
public:

    /// <summary>
    ///     Schedules a light-weight task within the schedule group.
    /// </summary>
    /// <param name="_Proc">
    ///     A pointer to the function to execute to perform the body of the light-weight task.
    /// </param>
    /// <param name="_Data">
    ///     A void pointer to the data that will be passed as a parameter to the body of the task.
    /// </param>
    /// <remarks>
    ///     Calling the <c>ScheduleTask</c> method implicitly places a reference count on the schedule group which is removed by the runtime
    ///     at an appropriate time after the task executes.
    /// </remarks>
    /// <seealso cref="ScheduleGroup::Reference Method"/>
    /**/
    virtual void ScheduleTask(TaskProc _Proc, void * _Data) =0;

    /// <summary>
    ///     Returns an identifier for the schedule group that is unique within the scheduler to which the group belongs.
    /// </summary>
    /// <returns>
    ///     An identifier for the schedule group that is unique within the scheduler to which the group belongs.
    /// </returns>
    /**/
    virtual unsigned int Id() const =0;

    /// <summary>
    ///     Increments the schedule group's reference count.
    /// </summary>
    /// <returns>
    ///     The newly incremented reference count.
    /// </returns>
    /// <remarks>
    ///     This is typically used to manage the lifetime of the schedule group for composition.  When the reference count of a schedule
    ///     group falls to zero, the schedule group is deleted by the runtime. A schedule group created using either the
    ///     <see cref="CurrentScheduler::CreateScheduleGroup Method">CurrentScheduler::CreateScheduleGroup</see> method, or the
    ///     <see cref="Scheduler::CreateScheduleGroup Method">Scheduler::CreateScheduleGroup</see> method starts out with a reference
    ///     count of one.
    /// </remarks>
    /// <seealso cref="ScheduleGroup::Release Method"/>
    /// <seealso cref="CurrentScheduler::CreateScheduleGroup Method"/>
    /// <seealso cref="Scheduler::CreateScheduleGroup Method"/>
    /**/
    virtual unsigned int Reference() =0;

    /// <summary>
    ///     Decrements this scheduler group's reference count.
    /// </summary>
    /// <returns>
    ///     The newly decremented reference count.
    /// </returns>
    /// <remarks>
    ///     This is typically used to manage the lifetime of the schedule group for composition.  When the reference count of a schedule
    ///     group falls to zero, the schedule group is deleted by the runtime.  Once you have called the <c>Release</c> method the specific number
    ///     of times to remove the creation reference count and any additional references placed via the <c>Reference</c> method, you may not
    ///     utilize the schedule group further.  Doing so will result in undefined behavior.
    ///     <para>A schedule group is associated with a particular scheduler instance.  You must ensure that all references to the
    ///     schedule group are released before all references to the scheduler are released, since the latter could result in the scheduler
    ///     being destroyed.  Doing otherwise results in undefined behavior.</para>
    /// </remarks>
    /// <seealso cref="ScheduleGroup::Reference Method"/>
    /// <seealso cref="CurrentScheduler::CreateScheduleGroup Method"/>
    /// <seealso cref="Scheduler::CreateScheduleGroup Method"/>
    /**/
    virtual unsigned int Release() =0;

protected:

    //
    // Privatize operator delete.  Clients should utilize Release to relinquish a schedule group.
    //
    template<class _T> friend void Concurrency::details::_InternalDeleteHelper(_T * _PObject);

    /// <summary>
    ///     A <c>ScheduleGroup</c> object is destroyed internally by the runtime when all external references to it are released.
    ///     It may not be explicitly deleted.
    /// </summary>
    /// <param name="_PObject">
    ///     A pointer to the object to be deleted.
    /// </param>
    /**/
    void operator delete(void * _PObject)
    {
        ::operator delete(_PObject);
    }
};

/// <summary>
///     Special value for the policy keys <c>MinConcurrency</c> and <c>MaxConcurrency</c>.  Defaults to the number of hardware
///     threads on the machine in the absence of other constraints.
/// </summary>
/// <seealso cref="PolicyElementKey Enumeration"/>
/**/
const unsigned int MaxExecutionResources = 0xFFFFFFFF;

/// <summary>
///     Special value for the policy key <c>ContextPriority</c> indicating that the thread priority of all contexts in the scheduler
///     should be the same as that of the thread which created the scheduler.
/// </summary>
/// <seealso cref="PolicyElementKey Enumeration"/>
/**/
const unsigned int INHERIT_THREAD_PRIORITY = 0x0000F000;

/// <summary>
///     Policy keys describing aspects of scheduler behavior.  Each policy element is described by a key-value pair.  For more information
///     about scheduler policies and their impact on schedulers, see <see cref="Task Scheduler (Concurrency Runtime)"/>.
/// </summary>
/// <seealso cref="SchedulerPolicy Class"/>
/// <seealso cref="CurrentScheduler Class"/>
/// <seealso cref="Scheduler Class"/>
/// <seealso cref="Task Scheduler (Concurrency Runtime)"/>
/**/
enum PolicyElementKey
{
    /// <summary>
    ///     The type of threads that the scheduler will utilize for underlying execution contexts.  For more information, see
    ///     <see cref="SchedulerType Enumeration"/>.
    ///     <para>Valid values  : A member of the <c>SchedulerType</c> enumeration, either <c>ThreadScheduler</c> or <c>UmsThreadDefault</c></para>
    ///     <para>Default value : <c>ThreadScheduler</c>.  This translates to Win32 threads on all operating systems.</para>
    /// </summary>
    /**/
    SchedulerKind,

    /// <summary>
    ///     The maximum concurrency level desired by the scheduler.  The resource manager will try to initially allocate this many virtual processors.
    ///     The special value <see cref="MaxExecutionResources Constant">MaxExecutionResources</see> indicates that the desired concurrency level
    ///     is same as the number of hardware threads on the machine.  If the value specified for <c>MinConcurrency</c> is greater than the number
    ///     of hardware threads on the machine and <c>MaxConcurrency</c> is specified as <c>MaxExecutionResources</c>, the value for <c>MaxConcurrency</c>
    ///     is raised to match what is set for <c>MinConcurrency</c>.
    ///     <para>Valid values  : Positive integers and the special value <c>MaxExecutionResources</c></para>
    ///     <para>Default value : <c>MaxExecutionResources</c></para>
    /// </summary>
    /**/
    MaxConcurrency,

    /// <summary>
    ///     The minimum concurrency level that must be provided to the scheduler by the resource manager.  The number of virtual processors assigned
    ///     to a scheduler will never go below the minimum.  The special value <see cref="MaxExecutionResources Constant">MaxExecutionResources</see>
    ///     indicates that the minimum concurrency level is same as the number of hardware threads on the machine.  If the value specified for
    ///     <c>MaxConcurrency</c> is less than the number of hardware threads on the machine and <c>MinConcurrency</c> is specified as
    ///     <c>MaxExecutionResources</c>, the value for <c>MinConcurrency</c> is lowered to match what is set for <c>MaxConcurrency</c>.
    ///     <para>Valid values  : Non-negative integers and the special value <c>MaxExecutionResources</c>.  Note that for scheduler policies
    ///     used for the construction of Concurrency Runtime schedulers or any policy with the <c>SchedulerKind</c> policy key set to the value
    ///     <c>UMSThreadDefault</c>, the value <c>0</c> is invalid.</para>
    ///     <para>Default value : <c>1</c></para>
    /// </summary>
    /**/
    MinConcurrency,

    /// <summary>
    ///     Tentative number of virtual processors per hardware thread.  The target oversubscription factor may be increased by the Resource Manager,
    ///     if necessary, to satisfy <c>MaxConcurrency</c> with the hardware threads on the machine.
    ///     <para>Valid values  : Positive integers</para>
    ///     <para>Default value : <c>1</c></para>
    /// </summary>
    /**/
    TargetOversubscriptionFactor,

    /// <summary>
    ///     When the <c>SchedulingProtocol</c> policy key is set to the value <c>EnhanceScheduleGroupLocality</c>, this specifies the maximum number
    ///     of runnable contexts allowed to be cached in per virtual processor local queues.  Such contexts will typically run in last-in-first-out
    ///     (LIFO) order on the virtual processor that caused them to become runnable.  Note that this policy key has no meaning when the
    ///     <c>SchedulingProtocol</c> key is set to the value <c>EnhanceForwardProgress</c>.
    ///     <para>Valid values  : Non-negative integers</para>
    ///     <para>Default value : <c>8</c></para>
    /// </summary>
    /**/
    LocalContextCacheSize,

    /// <summary>
    ///     The reserved stack size of each context in the scheduler in kilobytes.
    ///     <para>Valid values  : Positive integers</para>
    ///     <para>Default value : <c>0</c>, indicating that the process' default value for stack size be used.</para>
    /// </summary>
    /**/
    ContextStackSize,

    /// <summary>
    ///     The operating system thread priority of each context in the scheduler.  If this key is set to the value <see cref="INHERIT_THREAD_PRIORITY Constant">
    ///     INHERIT_THREAD_PRIORITY</see> the contexts in the scheduler will inherit the priority of the thread that created the scheduler.
    ///     <para>Valid values  : Any of the valid values for the Windows <c>SetThreadPriority</c> function and the special value
    ///     <c>INHERIT_THREAD_PRIORITY</c></para>
    ///     <para>Default value : <c>THREAD_PRIORITY_NORMAL</c></para>
    /// </summary>
    /**/
    ContextPriority,

    /// <summary>
    ///     Describes which scheduling algorithm will be used by the scheduler.  For more information, see <see cref="SchedulingProtocolType Enumeration"/>.
    ///     <para>Valid values  : A member of the <c>SchedulingProtocolType</c> enumeration, either <c>EnhanceScheduleGroupLocality</c>
    ///     or <c>EnhanceForwardProgress</c></para>
    ///     <para>Default value : <c>EnhanceScheduleGroupLocality</c></para>
    /// </summary>
    /**/
    SchedulingProtocol,

    /// <summary>
    ///     Determines whether the resources for the scheduler will be rebalanced according to statistical information gathered from the
    ///     scheduler or only based on the subscription level of underlying hardware threads. For more information, see
    ///     <see cref="DynamicProgressFeedbackType Enumeration"/>.
    ///     <para>Valid values  : A member of the <c>DynamicProgressFeedbackType</c> enumeration, either <c>ProgressFeedbackEnabled</c> or
    ///     <c>ProgressFeedbackDisabled</c></para>
    ///     <para>Default value : <c>ProgressFeedbackEnabled</c></para>
    /// </summary>
    /**/
    DynamicProgressFeedback,

    /// <summary>
    ///     The maximum policy element key.  Not a valid element key.
    /// </summary>
    /**/
    MaxPolicyElementKey
};

/// <summary>
///     Used by the <c>SchedulerKind</c> policy to describe the type of threads that the scheduler should utilize for underlying execution contexts.
///     For more information on available scheduler policies, see <see cref="PolicyElementKey Enumeration"/>.
/// </summary>
/// <seealso cref="SchedulerPolicy Class"/>
/// <seealso cref="PolicyElementKey Enumeration"/>
/**/
enum SchedulerType
{
    /// <summary>
    ///     Indicates an explicit request of regular Win32 threads.
    /// </summary>
    /**/
    ThreadScheduler,

    /// <summary>
    ///     Indicates a request for UMS threads if available on the underlying platform; otherwise, regular Win32 threads.  Note that if
    ///     you call the <c>GetPolicyValue</c> method of a <c>SchedulerPolicy</c> object and this value is returned, it is an indication
    ///     that the underlying platform supports UMS threads and that the policy will create a scheduler utilizing UMS threads.
    /// </summary>
    /**/
    UmsThreadDefault
};

/// <summary>
///     Used by the <c>SchedulingProtocol</c> policy to describe which scheduling algorithm will be utilized for the scheduler.  For more
///     information on available scheduler policies, see <see cref="PolicyElementKey Enumeration"/>.
/// </summary>
/// <seealso cref="SchedulerPolicy Class"/>
/// <seealso cref="PolicyElementKey Enumeration"/>
/**/
enum SchedulingProtocolType
{
    /// <summary>
    ///     The scheduler prefers to continue to work on tasks within the current schedule group before moving to another schedule group.
    ///     Unblocked contexts are cached per virtual-processor and are typically scheduled in a last-in-first-out (LIFO) fashion by the
    ///     virtual processor which unblocked them.
    /// </summary>
    /**/
    EnhanceScheduleGroupLocality,

    /// <summary>
    ///     The scheduler prefers to round-robin through schedule groups after executing each task.  Unblocked contexts are typically
    ///     scheduled in a first-in-first-out (FIFO) fashion.  Virtual processors do not cache unblocked contexts.
    /// </summary>
    /**/
    EnhanceForwardProgress
};

/// <summary>
///     Used by the <c>DynamicProgressFeedback</c> policy to describe whether resources for the scheduler will be rebalanced according to
///     statistical information gathered from the scheduler or only based on virtual processors going in and out of the idle state through
///     calls to the <c>Activate</c> and <c>Deactivate</c> methods on the <c>IVirtualProcessorRoot</c> interface.  For more information
///     on available scheduler policies, see <see cref="PolicyElementKey Enumeration"/>.
/// </summary>
/// <seealso cref="PolicyElementKey Enumeration"/>
/**/
enum DynamicProgressFeedbackType
{
    /// <summary>
    ///     The scheduler does not gather progress information.  Rebalancing is done based solely on the subscription level of the underlying
    ///     hardware thread.  For more information on subscription levels, see
    ///     <see cref="IExecutionResource::CurrentSubscriptionLevel Method">IExecutionResource::CurrentSubscriptionLevel</see>.
    /// </summary>
    /**/
    ProgressFeedbackDisabled,

    /// <summary>
    ///     The scheduler gathers progress information and passes it to the resource manager.  The resource manager will utilize this statistical
    ///     information to rebalance resources on behalf of the scheduler in addition to the subscription level of the underlying
    ///     hardware thread.  For more information on subscription levels, see
    ///     <see cref="IExecutionResource::CurrentSubscriptionLevel Method">IExecutionResource::CurrentSubscriptionLevel</see>.
    /// </summary>
    /**/
    ProgressFeedbackEnabled
};

/// <summary>
///     The <c>SchedulerPolicy</c> class contains a set of key/value pairs, one for each policy element, that control the behavior of a
///     scheduler instance.
/// </summary>
/// <remarks>
///     For more information about the policies which can be controlled via the <c>SchedulerPolicy</c> class, see
///     <see cref="PolicyElementKey Enumeration"/>.
/// </remarks>
/// <seealso cref="PolicyElementKey Enumeration"/>
/// <seealso cref="CurrentScheduler Class"/>
/// <seealso cref="Scheduler Class"/>
/// <seealso cref="Task Scheduler (Concurrency Runtime)"/>
/**/
class SchedulerPolicy
{
public:

    /// <summary>
    ///     Constructs a new scheduler policy and populates it with values for <see cref="PolicyElementKey Enumeration">policy keys</see>
    ///     supported by Concurrency Runtime schedulers and the Resource Manager.
    /// </summary>
    /// <remarks>
    ///     <para>The first constructor creates a new scheduler policy where all policies will be initialized to their default values.</para>
    ///     <para>The second constructor creates a new scheduler policy that uses a named-parameter style of initialization. Values after
    ///     the <paramref name="_PolicyKeyCount"/> parameter are supplied as key/value pairs.  Any policy key which is not specified in this
    ///     constructor will have its default value. This constructor could throw the exceptions <see cref="invalid_scheduler_policy_key Class">
    ///     invalid_scheduler_policy_key</see>, <see cref="invalid_scheduler_policy_value Class">invalid_scheduler_policy_value </see> or
    ///     <see cref="invalid_scheduler_policy_thread_specification Class"> invalid_scheduler_policy_thread_specification</see>.</para>
    ///     <para>The third constructor is a copy constructor. Often, the most convenient way to define a new scheduler policy is to copy an
    ///     existing policy and modify it via the <c>SetPolicyValue</c> or <c>SetConcurrencyLimits</c> methods.</para>
    /// </remarks>
    /// <seealso cref="SchedulerPolicy::SetPolicyValue Method"/>
    /// <seealso cref="SchedulerPolicy::GetPolicyValue Method"/>
    /// <seealso cref="SchedulerPolicy::SetConcurrencyLimits Method"/>
    /// <seealso cref="PolicyElementKey Enumeration"/>
    /**/
    _CRTIMP SchedulerPolicy();

    /// <summary>
    ///     Constructs a new scheduler policy and populates it with values for <see cref="PolicyElementKey Enumeration">policy keys</see>
    ///     supported by Concurrency Runtime schedulers and the Resource Manager.
    /// </summary>
    /// <param name="_PolicyKeyCount">
    ///     The number of key/value pairs that follow the <paramref name="_PolicyKeyCount"/> parameter.
    /// </param>
    /// <remarks>
    ///     <para>The first constructor creates a new scheduler policy where all policies will be initialized to their default values.</para>
    ///     <para>The second constructor creates a new scheduler policy that uses a named-parameter style of initialization. Values after </para>
    ///     the <paramref name="_PolicyKeyCount"/> parameter are supplied as key/value pairs.  Any policy key which is not specified in this
    ///     constructor will have its default value. This constructor could throw the exceptions <see cref="invalid_scheduler_policy_key Class">
    ///     invalid_scheduler_policy_key</see>, <see cref="invalid_scheduler_policy_value Class">invalid_scheduler_policy_value </see> or
    ///     <see cref="invalid_scheduler_policy_thread_specification Class"> invalid_scheduler_policy_thread_specification</see>.
    ///     <para>The third constructor is a copy constructor. Often, the most convenient way to define a new scheduler policy is to copy an
    ///     existing policy and modify it via the <c>SetPolicyValue</c> or <c>SetConcurrencyLimits</c> methods.</para>
    /// </remarks>
    /// <seealso cref="SchedulerPolicy::SetPolicyValue Method"/>
    /// <seealso cref="SchedulerPolicy::GetPolicyValue Method"/>
    /// <seealso cref="SchedulerPolicy::SetConcurrencyLimits Method"/>
    /// <seealso cref="PolicyElementKey Enumeration"/>
    /**/
    _CRTIMP SchedulerPolicy(size_t _PolicyKeyCount, ...);

    /// <summary>
    ///     Constructs a new scheduler policy and populates it with values for <see cref="PolicyElementKey Enumeration">policy keys</see>
    ///     supported by Concurrency Runtime schedulers and the Resource Manager.
    /// </summary>
    /// <param name="_SrcPolicy">
    ///     The source policy to copy.
    /// </param>
    /// <remarks>
    ///     <para>The first constructor creates a new scheduler policy where all policies will be initialized to their default values.</para>
    ///     <para>The second constructor creates a new scheduler policy that uses a named-parameter style of initialization. Values after </para>
    ///     the <paramref name="_PolicyKeyCount"/> parameter are supplied as key/value pairs.  Any policy key which is not specified in this
    ///     constructor will have its default value. This constructor could throw the exceptions <see cref="invalid_scheduler_policy_key Class">
    ///     invalid_scheduler_policy_key</see>, <see cref="invalid_scheduler_policy_value Class">invalid_scheduler_policy_value </see> or
    ///     <see cref="invalid_scheduler_policy_thread_specification Class"> invalid_scheduler_policy_thread_specification</see>.
    ///     <para>The third constructor is a copy constructor. Often, the most convenient way to define a new scheduler policy is to copy an
    ///     existing policy and modify it via the <c>SetPolicyValue</c> or <c>SetConcurrencyLimits</c> methods.</para>
    /// </remarks>
    /// <seealso cref="SchedulerPolicy::SetPolicyValue Method"/>
    /// <seealso cref="SchedulerPolicy::GetPolicyValue Method"/>
    /// <seealso cref="SchedulerPolicy::SetConcurrencyLimits Method"/>
    /// <seealso cref="PolicyElementKey Enumeration"/>
    /**/
    _CRTIMP SchedulerPolicy(const SchedulerPolicy& _SrcPolicy);

    /// <summary>
    ///     Assigns the scheduler policy from another scheduler policy.
    /// </summary>
    /// <param name="_RhsPolicy">
    ///     The policy to assign to this policy.
    /// </param>
    /// <returns>
    ///     A reference to the scheduler policy.
    /// </returns>
    /// <remarks>
    ///     Often, the most convenient way to define a new scheduler policy is to copy an existing policy and modify it via the
    ///     <c>SetPolicyValue</c> or <c>SetConcurrencyLimits</c> methods.
    /// </remarks>
    /// <seealso cref="SchedulerPolicy::SetPolicyValue Method"/>
    /// <seealso cref="SchedulerPolicy::SetConcurrencyLimits Method"/>
    /// <seealso cref="PolicyElementKey Enumeration"/>
    /**/
    _CRTIMP SchedulerPolicy& operator=(const SchedulerPolicy& _RhsPolicy);

    /// <summary>
    ///     Destroys a scheduler policy.
    /// </summary>
    /**/
    _CRTIMP ~SchedulerPolicy();

    /// <summary>
    ///     Retrieves the value of the policy key supplied as the <paramref name="_Key"/> parameter.
    /// </summary>
    /// <param name="_Key">
    ///     The policy key to retrieve a value for.
    /// </param>
    /// <returns>
    ///     If the key specified by the <paramref name="_Key"/> parameter is supported, the policy value for the key cast to an <c>unsigned int</c>.
    /// </returns>
    /// <remarks>
    ///     The method will throw <see cref="invalid_scheduler_policy_key Class">invalid_scheduler_policy_key</see> for an invalid policy key.
    /// </remarks>
    /// <seealso cref="SchedulerPolicy::SetPolicyValue Method"/>
    /// <seealso cref="SchedulerPolicy::SetConcurrencyLimits Method"/>
    /// <seealso cref="PolicyElementKey Enumeration"/>
    /**/
    _CRTIMP unsigned int GetPolicyValue(PolicyElementKey _Key) const;

    /// <summary>
    ///     Sets the value of the policy key supplied as the <paramref name="_Key"/> parameter and returns the old value.
    /// </summary>
    /// <param name="_Key">
    ///     The policy key to set a value for.
    /// </param>
    /// <param name="_Value">
    ///     The value to set the policy key to.
    /// </param>
    /// <returns>
    ///     If the key specified by the <paramref name="_Key"/> parameter is supported, the old policy value for the key cast to an <c>unsigned int</c>.
    /// </returns>
    /// <remarks>
    ///     The method will throw <see cref="invalid_scheduler_policy_key Class">invalid_scheduler_policy_key </see> for an invalid policy key
    ///     or any policy key whose value cannot be set by the <c>SetPolicyValue</c> method.
    ///     <para>The method will throw <see cref="invalid_scheduler_policy_value Class">invalid_scheduler_policy_value</see> for a value that
    ///     is not supported for the key specified by the <paramref name="_Key"/> parameter.</para>
    ///     <para>Note that this method is not allowed to set the <c>MinConcurrency</c> or <c>MaxConcurrency</c> policies.  To set these values, use
    ///     the <see cref="SchedulerPolicy::SetConcurrencyLimits Method">SetConcurrencyLimits</see> method.</para>
    /// </remarks>
    /// <seealso cref="SchedulerPolicy::GetPolicyValue Method"/>
    /// <seealso cref="SchedulerPolicy::SetConcurrencyLimits Method"/>
    /// <seealso cref="PolicyElementKey Enumeration"/>
    /**/
    _CRTIMP unsigned int SetPolicyValue(PolicyElementKey _Key, unsigned int _Value);

    /// <summary>
    ///     Simultaneously sets the <c>MinConcurrency</c> and <c>MaxConcurrency</c> policies on the <c>SchedulerPolicy</c> object.
    /// </summary>
    /// <param name="_MinConcurrency">
    ///     The value for the <c>MinConcurrency</c> policy key.
    /// </param>
    /// <param name="_MaxConcurrency">
    ///     The value for the <c>MaxConcurrency</c> policy key.
    /// </param>
    /// <remarks>
    ///     The method will throw <see cref="invalid_scheduler_policy_thread_specification Class">invalid_scheduler_policy_thread_specification
    ///     </see> if the value specified for the <c>MinConcurrency</c> policy is greater than that specified for the <c>MaxConcurrency</c> policy.
    ///     <para>The method may also throw <see cref="invalid_scheduler_policy_value Class">invalid_scheduler_policy_value </see> for other
    ///     invalid values.</para>
    /// </remarks>
    /// <seealso cref="SchedulerPolicy::GetPolicyValue Method"/>
    /// <seealso cref="SchedulerPolicy::SetPolicyValue Method"/>
    /// <seealso cref="PolicyElementKey Enumeration"/>
    /**/
    _CRTIMP void SetConcurrencyLimits(unsigned int _MinConcurrency, unsigned int _MaxConcurrency = MaxExecutionResources);

    /// <summary>
    ///     Checks if this policy is a valid policy for a Concurrency Runtime scheduler.  If it is not, an appropriate exception will be thrown.
    /// </summary>
    /// <remarks>
    ///     The method will throw <see cref="invalid_scheduler_policy_value Class">invalid_scheduler_policy_value </see> if a policy value supplied
    ///     in the <c>SchedulerPolicy</c> object cannot be used to create a Concurrency Runtime scheduler.  Note that such a policy is not necessarily
    ///     invalid.  The Concurrency Runtime's Resource Manager also utilizes the <c>SchedulerPolicy</c> class to describe requirements.
    /// </remarks>
    /**/
    void _ValidateConcRTPolicy() const;

private:

    struct _PolicyBag
    {
        union
        {
            unsigned int _M_pPolicyBag[MaxPolicyElementKey];
            struct
            {
                SchedulerType _M_schedulerKind;
                unsigned int _M_maxConcurrency;
                unsigned int _M_minConcurrency;
                unsigned int _M_targetOversubscriptionFactor;
                unsigned int _M_localContextCacheSize;
                unsigned int _M_contextStackSize;
                unsigned int _M_contextPriority;
                SchedulingProtocolType _M_schedulingProtocol;
                DynamicProgressFeedbackType _M_dynamicProgressFeedback;
            } _M_specificValues;
        } _M_values;
    } *_M_pPolicyBag;

    /// <summary>
    ///     Initializes the scheduler policy.
    /// </summary>
    /**/
    void _Initialize(size_t _PolicyKeyCount, va_list * _PArgs);

    /// <summary>
    ///     Make this policy a copy of the source policy.
    /// </summary>
    /**/
    void _Assign(const SchedulerPolicy& _SrcPolicy);

    /// <summary>
    ///     Returns true if the key supplied is a supported key.
    /// </summary>
    /**/
    static bool __cdecl _ValidPolicyKey(PolicyElementKey _Key);

    /// <summary>
    ///     Returns true if a policy value is in a valid range.
    /// </summary>
    /**/
    static bool __cdecl _ValidPolicyValue(PolicyElementKey _Key, unsigned int _Value);

    /// <summary>
    ///     Returns true if concurrency limit combinations are valid.
    /// </summary>
    /**/
    static bool __cdecl _AreConcurrencyLimitsValid(unsigned int _MinConcurrency, unsigned int _MaxConcurrency);
    bool _AreConcurrencyLimitsValid() const;

    /// <summary>
    ///     Test a policy's concurrency combinations.
    /// </summary>
    /**/
    bool _ArePolicyCombinationsValid() const;

    /// <summary>
    ///     Resolves some of the policy keys that are set to defaults, based on the characteristics of the underlying system.
    /// </summary>
    /**/
    void _ResolvePolicyValues();

    /// <summary>
    ///     Stringify policy keys.
    /// </summary>
    /**/
    static char * __cdecl _StringFromPolicyKey(unsigned int _Index);
};

/// <summary>
///     Represents an abstraction for the current scheduler associated with the calling context.
/// </summary>
/// <remarks>
///     If there is no scheduler (see <see cref="Scheduler Class">Scheduler</see>) associated with the calling context, many
///     methods within the <c>CurrentScheduler</c> class will result in attachment of the process' default scheduler.  This may
///     also imply that the process' default scheduler is created during such a call.
/// </remarks>
/// <seealso cref="Scheduler Class"/>
/// <seealso cref="PolicyElementKey Enumeration"/>
/// <seealso cref="Task Scheduler (Concurrency Runtime)"/>
/**/
class CurrentScheduler
{
private:
    CurrentScheduler() {}

public:
    /// <summary>
    ///     Returns a unique identifier for the current scheduler.
    /// </summary>
    /// <returns>
    ///     If a scheduler is associated with the calling context, a unique identifier for that scheduler; otherwise, the value <c>-1</c>.
    /// </returns>
    /// <remarks>
    ///     This method will not result in scheduler attachment if the calling context is not already associated with a scheduler.
    /// </remarks>
    /**/
    _CRTIMP static unsigned int __cdecl Id();

    /// <summary>
    ///     Returns a copy of the policy that the current scheduler was created with.
    /// </summary>
    /// <returns>
    ///     A copy of the policy that that the current scheduler was created with.
    /// </returns>
    /// <remarks>
    ///     This method will result in the process' default scheduler being created and/or attached to the calling context if there is no
    ///     scheduler currently associated with the calling context.
    /// </remarks>
    /// <seealso cref="SchedulerPolicy Class"/>
    /**/
    _CRTIMP static SchedulerPolicy __cdecl GetPolicy();

    /// <summary>
    ///     Returns a pointer to the scheduler associated with the calling context, also referred to as the current scheduler.
    /// </summary>
    /// <returns>
    ///     A pointer to the scheduler associated with the calling context (the current scheduler).
    /// </returns>
    /// <remarks>
    ///     This method will result in the process' default scheduler being created and/or attached to the calling context if there is no
    ///     scheduler currently associated with the calling context. No additional reference is placed on the <c>Scheduler</c> object
    ///     returned by this method.
    /// </remarks>
    /**/
    _CRTIMP static Scheduler * __cdecl Get();

    /// <summary>
    ///     Returns the current number of virtual processors for the scheduler associated with the calling context.
    /// </summary>
    /// <returns>
    ///     If a scheduler is associated with the calling context, the current number of virtual processors for that scheduler; otherwise,
    ///     the value <c>-1</c>.
    /// </returns>
    /// <remarks>
    ///     This method will not result in scheduler attachment if the calling context is not already associated with a scheduler.
    ///     <para>The return value from this method is an instantaneous sampling of the number of virtual processors for the scheduler associated
    ///     with the calling context.  This value may be stale the moment it is returned.</para>
    /// </remarks>
    /**/
    _CRTIMP static unsigned int __cdecl GetNumberOfVirtualProcessors();

    /// <summary>
    ///     Creates a new scheduler whose behavior is described by the <paramref name="_Policy"/> parameter and attaches it to the calling context.
    ///     The newly created scheduler will become the current scheduler for the calling context.
    /// </summary>
    /// <param name="_Policy">
    ///     The scheduler policy that describes the behavior of the newly created scheduler.
    /// </param>
    /// <remarks>
    ///     The attachment of the scheduler to the calling context implicitly places a reference count on the scheduler.
    ///     <para>After a scheduler is created with the <c>Create</c> method, you must call the <see cref="CurrentScheduler::Detach Method">
    ///     CurrentScheduler::Detach</see> method at some point in the future in order to allow the scheduler to shut down.</para>
    ///     <para>If this method is called from a context that is already attached to a different scheduler, the existing scheduler is remembered
    ///     as the previous scheduler, and the newly created scheduler becomes the current scheduler. When you call the <c>CurrentScheduler::Detach</c>
    ///     method at a later point, the previous scheduler is restored as the current scheduler.</para>
    ///     <para>This method may throw a variety of exceptions, including <see cref="scheduler_resource_allocation_error Class">
    ///     scheduler_resource_allocation_error</see> and <see cref="invalid_scheduler_policy_value Class">invalid_scheduler_policy_value</see>.</para>
    /// </remarks>
    /// <seealso cref="SchedulerPolicy Class"/>
    /// <seealso cref="CurrentScheduler::Detach Method"/>
    /// <seealso cref="Scheduler::Reference Method"/>
    /// <seealso cref="Scheduler::Release Method"/>
    /// <seealso cref="Task Scheduler (Concurrency Runtime)"/>
    /**/
    _CRTIMP static void __cdecl Create(const SchedulerPolicy& _Policy);

    /// <summary>
    ///     Detaches the current scheduler from the calling context and restores the previously attached scheduler as the current
    ///     scheduler, if one exists.  After this method returns, the calling context is then managed by the scheduler that was previously
    ///     attached to the context via either the <c>CurrentScheduler::Create</c> or <c>Scheduler::Attach</c> method.
    /// </summary>
    /// <remarks>
    ///     The <c>Detach</c> method implicitly removes a reference count from the scheduler.
    ///     <para>If there is no scheduler attached to the calling context, calling this method will result in an <see cref="improper_scheduler_detach Class">
    ///     improper_scheduler_detach</see> exception being thrown.</para>
    /// </remarks>
    /// <seealso cref="Scheduler::Attach Method"/>
    /// <seealso cref="CurrentScheduler::Create Method"/>
    /**/
    _CRTIMP static void __cdecl Detach();

    /// <summary>
    ///     Causes the Windows event handle passed in the <paramref name="_ShutdownEvent"/> parameter to be signaled when the scheduler associated with
    ///     the current context shuts down and destroys itself.  At the time the event is signaled, all work that had been scheduled to the
    ///     scheduler is complete.  Multiple shutdown events may be registered through this method.
    /// </summary>
    /// <param name="_ShutdownEvent">
    ///     A handle to a Windows event object which will be signaled by the runtime when the scheduler associated with the current context
    ///     shuts down and destroys itself.
    /// </param>
    /// <remarks>
    ///     If there is no scheduler attached to the calling context, calling this method will result in a <see cref="scheduler_not_attached Class">
    ///     scheduler_not_attached </see> exception being thrown.
    /// </remarks>
    /**/
    _CRTIMP static void __cdecl RegisterShutdownEvent(HANDLE _ShutdownEvent);

    /// <summary>
    ///     Creates a new schedule group within the scheduler associated with the calling context.
    /// </summary>
    /// <returns>
    ///     A pointer to the newly created schedule group.  This <c>ScheduleGroup</c> object has an initial reference count placed on it.
    /// </returns>
    /// <remarks>
    ///     This method will result in the process' default scheduler being created and/or attached to the calling context if there is no
    ///     scheduler currently associated with the calling context.
    ///     <para>You must invoke the <see cref="ScheduleGroup::Release Method">Release</see> method on a schedule group when you are
    ///     done scheduling work to it. The scheduler will destroy the schedule group when all work queued to it has completed.</para>
    ///     <para>Note that if you explicitly created this scheduler, you must release all references to schedule groups within it, before
    ///     you release your reference on the scheduler, via detaching the current context from it.</para>
    /// </remarks>
    /// <seealso cref="ScheduleGroup Class"/>
    /// <seealso cref="ScheduleGroup::Release Method"/>
    /// <seealso cref="Task Scheduler (Concurrency Runtime)"/>
    /**/
    _CRTIMP static ScheduleGroup * __cdecl CreateScheduleGroup();

    /// <summary>
    ///     Schedules a light-weight task within the scheduler associated with the calling context.  The light-weight task will be placed
    ///     in a schedule group of the runtime's choosing.
    /// </summary>
    /// <param name="_Proc">
    ///     A pointer to the function to execute to perform the body of the light-weight task.
    /// </param>
    /// <param name="_Data">
    ///     A void pointer to the data that will be passed as a parameter to the body of the task.
    /// </param>
    /// <remarks>
    ///     This method will result in the process' default scheduler being created and/or attached to the calling context if there is no
    ///     scheduler currently associated with the calling context.
    /// </remarks>
    /// <seealso cref="Task Scheduler (Concurrency Runtime)"/>
    /// <seealso cref="ScheduleGroup Class"/>
    /**/
    _CRTIMP static void __cdecl ScheduleTask(TaskProc _Proc, void * _Data);
};

/// <summary>
///     Represents an abstraction for a Concurrency Runtime scheduler.
/// </summary>
/// <remarks>
///     The Concurrency Runtime scheduler uses execution contexts, which map to the operating system's execution contexts, such as a thread or a UMS thread,
///     to execute the work queued to it by your application.  At any time, the concurrency level of a scheduler is equal to the number of virtual processor
///     granted to it by the Resource Manager.  A virtual processor is an abstraction for a processing resource and maps to a hardware thread on the
///     underlying system. Only a single scheduler context may execute on a virtual processor at a given time.
///     <para> The Concurrency Runtime will create a default scheduler per process to execute parallel work.  In addition you may create your own scheduler
///     instances and manipulate it using this class.</para>
/// </remarks>
/// <seealso cref="Scheduler Class"/>
/// <seealso cref="PolicyElementKey Enumeration"/>
/// <seealso cref="Task Scheduler (Concurrency Runtime)"/>
/**/
class Scheduler
{
protected:
    /// <summary>
    ///     An object of the <c>Scheduler</c> class can only created using factory methods, or implicitly.
    /// </summary>
    /// <remarks>
    ///     The process' default scheduler is created implicitly when you utilize many of the runtime functions which require a scheduler
    ///     to be attached to the calling context.  Methods within the <c>CurrentScheduler</c> class and features of the PPL and agents layers
    ///     typically perform implicit attachment.
    ///     <para>You can also create a scheduler explicitly through either the <c>CurrentScheduler::Create</c> method or the <c>Scheduler::Create</c>
    ///     method.</para>
    /// </remarks>
    /// <seealso cref="CurrentScheduler Class"/>
    /// <seealso cref="CurrentScheduler::Create Method"/>
    /// <seealso cref="Scheduler::Create Method"/>
    /**/
    Scheduler() {}

    /// <summary>
    ///     An object of the <c>Scheduler</c> class is implicitly destroyed when all external references to it cease to exist.
    /// </summary>
    /**/
    virtual ~Scheduler() {}

public:

    /// <summary>
    ///     Creates a new scheduler whose behavior is described by the <paramref name="_Policy"/> parameter, places an initial reference on
    ///     the scheduler, and returns a pointer to it.
    /// </summary>
    /// <param name="_Policy">
    ///     The scheduler policy that describes behavior of the newly created scheduler.
    /// </param>
    /// <returns>
    ///     A pointer to a newly created scheduler.  This <c>Scheduler</c> object has an initial reference count placed on it.
    /// </returns>
    /// <remarks>
    ///     After a scheduler is created with the <c>Create</c> method, you must call the <see cref="Release Method">Release</see> method at some point
    ///     in the future in order to remove the initial reference count and allow the scheduler to shut down.
    ///     <para>A scheduler created with this method is not attached to the calling context. It may be attached to a context via the
    ///     <see cref="Scheduler::Attach Method">Attach</see> method.</para>
    ///     <para>This method may throw a variety of exceptions, including <see cref="scheduler_resource_allocation_error Class">
    ///     scheduler_resource_allocation_error</see> and <see cref="invalid_scheduler_policy_value Class">invalid_scheduler_policy_value</see>.</para>
    /// </remarks>
    /// <seealso cref="Scheduler::Release Method"/>
    /// <seealso cref="Scheduler::Attach Method"/>
    /// <seealso cref="CurrentScheduler::Create Method"/>
    /// <seealso cref="PolicyElementKey Enumeration"/>
    /// <seealso cref="Task Scheduler (Concurrency Runtime)"/>
    /**/
    _CRTIMP static Scheduler * __cdecl Create(const SchedulerPolicy& _Policy);

    /// <summary>
    ///     Returns a unique identifier for the scheduler.
    /// </summary>
    /// <returns>
    ///     A unique identifier for the scheduler.
    /// </returns>
    /**/
    virtual unsigned int Id() const =0;

    /// <summary>
    ///     Returns the current number of virtual processors for the scheduler.
    /// </summary>
    /// <returns>
    ///     The current number of virtual processors for the scheduler.
    ///     <para>The return value from this method is an instantaneous sampling of the number of virtual processors for the scheduler.
    ///     This value may be stale the moment it is returned.</para>
    /// </returns>
    /**/
    virtual unsigned int GetNumberOfVirtualProcessors() const =0;

    /// <summary>
    ///     Returns a copy of the policy that the scheduler was created with.
    /// </summary>
    /// <returns>
    ///     A copy of the policy that the scheduler was created with.
    /// </returns>
    /// <seealso cref="SchedulerPolicy Class"/>
    /// <seealso cref="PolicyElementKey Enumeration"/>
    /// <seealso cref="Task Scheduler (Concurrency Runtime)"/>
    /**/
    virtual SchedulerPolicy GetPolicy() const =0;

    /// <summary>
    ///     Increments the scheduler's reference count.
    /// </summary>
    /// <returns>
    ///     The newly incremented reference count.
    /// </returns>
    /// <remarks>
    ///     This is typically used to manage the lifetime of the scheduler for composition.  When the reference count of a scheduler
    ///     falls to zero, the scheduler will shut down and destruct itself once all work on the scheduler has completed.
    ///     <para>The method will throw an <see cref="improper_scheduler_reference Class">improper_scheduler_reference</see> exception if the reference
    ///     count prior to calling the <c>Reference</c> method was zero and the call is made from a context that is not owned by the scheduler.</para>
    /// </remarks>
    /// <seealso cref="Scheduler::Release Method"/>
    /// <seealso cref="Scheduler::Create Method"/>
    /**/
    virtual unsigned int Reference() =0 ;

    /// <summary>
    ///     Decrements this scheduler's reference count.
    /// </summary>
    /// <returns>
    ///     The newly decremented reference count.
    /// </returns>
    /// <remarks>
    ///     This is typically used to manage the lifetime of the scheduler for composition.  When the reference count of a scheduler
    ///     falls to zero, the scheduler will shut down and destruct itself once all work on the scheduler has completed.
    /// </remarks>
    /// <seealso cref="Scheduler::Reference Method"/>
    /// <seealso cref="Scheduler::Create Method"/>
    /**/
    virtual unsigned int Release() =0;

    /// <summary>
    ///     Causes the Windows event handle passed in the <paramref name="_Event"/> parameter to be signaled when the scheduler
    ///     shuts down and destroys itself.  At the time the event is signaled, all work that had been scheduled to the
    ///     scheduler is complete.  Multiple shutdown events may be registered through this method.
    /// </summary>
    /// <param name="_Event">
    ///     A handle to a Windows event object which will be signaled by the runtime when the scheduler shuts down and destroys itself.
    /// </param>
    /**/
    virtual void RegisterShutdownEvent(HANDLE _Event) =0;

    /// <summary>
    ///     Attaches the scheduler to the calling context.  After this method returns, the calling context is managed by the scheduler and
    ///     the scheduler becomes the current scheduler.
    /// </summary>
    /// <remarks>
    ///     Attaching a scheduler implicitly places a reference on the scheduler.
    ///     <para>At some point in the future, you must call the <see cref="CurrentScheduler::Detach Method">CurrentScheduler::Detach</see>
    ///     method in order to allow the scheduler to shut down.</para>
    ///     <para>If this method is called from a context that is already attached to a different scheduler, the existing scheduler is remembered
    ///     as the previous scheduler, and the newly created scheduler becomes the current scheduler. When you call the <c>CurrentScheduler::Detach</c>
    ///     method at a later point, the previous scheduler is restored as the current scheduler.</para>
    ///     <para>This method will throw an <see cref="improper_scheduler_attach Class">improper_scheduler_attach</see> exception if this scheduler
    ///     is the current scheduler of the calling context.</para>
    /// </remarks>
    /// <seealso cref="CurrentScheduler::Detach Method"/>
    /**/
    virtual void Attach() =0;

    /// <summary>
    ///     Allows a user defined policy to be used to create the default scheduler.  This method may only be called when no default
    ///     scheduler exists within the process.  Once a default policy has been set, it remains in effect until the next valid call
    ///     to either the <c>SetDefaultSchedulerPolicy</c> or the <see cref="Scheduler::ResetDefaultSchedulerPolicy Method">ResetDefaultSchedulerPolicy
    ///     </see> method.
    /// </summary>
    /// <param name="_Policy">
    ///     The policy to be set as the default scheduler policy.
    /// </param>
    /// <remarks>
    ///     If the <c>SetDefaultSchedulerPolicy</c> method is called when a default scheduler already exists within the process, the runtime
    ///     will throw a <see cref="default_scheduler_exists Class">default_scheduler_exists</see> exception.
    /// </remarks>
    /// <seealso cref="Scheduler::ResetDefaultSchedulerPolicy Method"/>
    /// <seealso cref="SchedulerPolicy Class"/>
    /// <seealso cref="PolicyElementKey Enumeration"/>
    /// <seealso cref="Task Scheduler (Concurrency Runtime)"/>
    /**/
    _CRTIMP static void __cdecl SetDefaultSchedulerPolicy(const SchedulerPolicy& _Policy);

    /// <summary>
    ///     Resets the default scheduler policy to the runtime's default.  The next time a default scheduler is created, it will use the
    ///     runtime's default policy settings.
    /// </summary>
    /// <remarks>
    ///     This method may be called while a default scheduler exists within the process. It will not affect the policy of the existing
    ///     default scheduler. However, if the default scheduler were to shutdown, and a new default were to be created at a later
    ///     point, the new scheduler would use the runtime's default policy settings.
    /// </remarks>
    /// <seealso cref="Scheduler::SetDefaultSchedulerPolicy Method"/>
    /// <seealso cref="SchedulerPolicy Class"/>
    /**/
    _CRTIMP static void __cdecl ResetDefaultSchedulerPolicy();

    /// <summary>
    ///     Creates a new schedule group within the scheduler.
    /// </summary>
    /// <returns>
    ///     A pointer to the newly created schedule group.  This <c>ScheduleGroup</c> object has an initial reference count placed on it.
    /// </returns>
    /// <remarks>
    ///     You must invoke the <see cref="ScheduleGroup::Release Method">Release</see> method on a schedule group when you are
    ///     done scheduling work to it. The scheduler will destroy the schedule group when all work queued to it has completed.
    ///     <para>Note that if you explicitly created this scheduler, you must release all references to schedule groups within it, before
    ///     you release your references on the scheduler.</para>
    /// </remarks>
    /// <seealso cref="ScheduleGroup Class"/>
    /// <seealso cref="ScheduleGroup::Release Method"/>
    /// <seealso cref="Task Scheduler (Concurrency Runtime)"/>
    /**/
    virtual ScheduleGroup * CreateScheduleGroup() =0;

    /// <summary>
    ///     Schedules a light-weight task within the scheduler.  The light-weight task will be placed in a schedule group of the runtime's choosing.
    /// </summary>
    /// <param name="proc">
    ///     A pointer to the function to execute to perform the body of the light-weight task.
    /// </param>
    /// <param name="_Data">
    ///     A void pointer to the data that will be passed as a parameter to the body of the task.
    /// </param>
    /// <seealso cref="Task Scheduler (Concurrency Runtime)"/>
    /// <seealso cref="ScheduleGroup Class"/>
    /**/
    virtual void ScheduleTask(TaskProc proc, void * _Data) =0;
};

/// <summary>
///     Represents an abstraction for an execution context.
/// </summary>
/// <remarks>
///     The Concurrency Runtime scheduler (see <see cref="Scheduler Class">Scheduler</see>) uses execution contexts to execute the work queued
///     to it by your application.  A Win32 thread and a user mode schedulable (UMS) thread are examples of execution contexts on a Windows
///     operating system. UMS threads are supported only on 64-bit operating systems with version Windows 7 and higher.
///     <para>At any time, the concurrency level of a scheduler is equal to the number of virtual processors granted to it by the Resource Manager.
///     A virtual processor is an abstraction for a processing resource and maps to a hardware thread on the underlying system. Only a single scheduler
///     context may execute on a virtual processor at a given time.</para>
///     <para> The scheduler is cooperative in nature and an executing context may yield its virtual processor to a different context at any time if
///     it wishes to enter a wait state. When its wait it satisfied, it cannot resume until an available virtual processor from the scheduler begins
///     executing it.</para>
/// </remarks>
/// <seealso cref="Scheduler Class"/>
/// <seealso cref="Task Scheduler (Concurrency Runtime)"/>
/**/
class Context
{
public:

    /// <summary>
    ///     Returns an identifier for the context that is unique within the scheduler to which the context belongs.
    /// </summary>
    /// <returns>
    ///     An identifier for the context that is unique within the scheduler to which the context belongs.
    /// </returns>
    /**/
    virtual unsigned int GetId() const =0;

    /// <summary>
    ///     Returns an identifier for the virtual processor that the context is currently executing on.
    /// </summary>
    /// <returns>
    ///     If the context is currently executing on a virtual processor, an identifier for the virtual processor that the context
    ///     is currently executing on; otherwise, the value <c>-1</c>.
    /// </returns>
    /// <remarks>
    ///     The return value from this method is an instantaneous sampling of the virtual processor that the context is executing
    ///     on.  This value may be stale the moment it is returned and cannot be relied upon.  Typically, this method is used
    ///     for debugging or tracing purposes only.
    /// </remarks>
    /**/
    virtual unsigned int GetVirtualProcessorId() const =0;

    /// <summary>
    ///     Returns an identifier for the schedule group that the context is currently working on.
    /// </summary>
    /// <returns>
    ///     An identifier for the schedule group the context is currently working on.
    /// </returns>
    /// <remarks>
    ///     The return value from this method is an instantaneous sampling of the schedule group that the context is executing
    ///     on.  If this method is called on a context other than the current context, the value may be stale the moment it is
    ///     returned and cannot be relied upon.  Typically, this method is used for debugging or tracing purposes only.
    /// </remarks>
    /// <seealso cref="ScheduleGroup Class"/>
    /**/
    virtual unsigned int GetScheduleGroupId() const =0;

    /// <summary>
    ///     Returns an identifier for the current context that is unique within the scheduler to which the current context belongs.
    /// </summary>
    /// <returns>
    ///     If the current context is attached to a scheduler, an identifier for the current context that is unique within the scheduler
    ///     to which the current context belongs; otherwise, the value <c>-1</c>.
    /// </returns>
    /**/
    _CRTIMP static unsigned int __cdecl Id();

    /// <summary>
    ///     Returns an identifier for the virtual processor that the current context is executing on.
    /// </summary>
    /// <returns>
    ///     If the current context is attached to a scheduler, an identifier for the virtual processor that the current context is
    ///     executing on; otherwise, the value <c>-1</c>.
    /// </returns>
    /// <remarks>
    ///     The return value from this method is an instantaneous sampling of the virtual processor that the current context is executing
    ///     on.  This value may be stale the moment it is returned and cannot be relied upon.  Typically, this method is used
    ///     for debugging or tracing purposes only.
    /// </remarks>
    /**/
    _CRTIMP static unsigned int __cdecl VirtualProcessorId();

    /// <summary>
    ///     Returns an identifier for the schedule group that the current context is working on.
    /// </summary>
    /// <returns>
    ///     If the current context is attached to a scheduler and working on a schedule group, an identifier for the scheduler group that the
    ///     current context is working on; otherwise, the value <c>-1</c>.
    /// </returns>
    /// <seealso cref="ScheduleGroup Class"/>
    /**/
    _CRTIMP static unsigned int __cdecl ScheduleGroupId();

    /// <summary>
    ///     Blocks the current context.
    /// </summary>
    /// <remarks>
    ///     This method will result in the process' default scheduler being created and/or attached to the calling context if there is no
    ///     scheduler currently associated with the calling context.
    ///     <para>If the calling context is running on a virtual processor, the virtual processor will find another runnable context to
    ///     execute or may potentially create a new one.</para>
    ///     <para>Once the <c>Block</c> method has been called or will be called, you must pair it with a call to the <see cref="Context::Unblock Method">
    ///     Unblock</see> method from another execution context in order for it to run again.  Be aware that there is a critical period between
    ///     the point where your code publishes its context for another thread to be able to call the <c>Unblock</c> method and the point
    ///     where the actual method call to <c>Block</c> is made.  During this period, it is imperative that you not call any method which
    ///     may in turn block and unblock for its own reasons (e.g.: acquiring a lock).  Calls to the <c>Block</c> and <c>Unblock</c> method
    ///     do not track the reason for the blocking and unblocking.  Only one object should have ownership of a <c>Block</c>-<c>Unblock</c>
    ///     pair.</para>
    ///     <para>This method may throw a variety of exceptions, including <see cref="scheduler_resource_allocation_error Class">
    ///     scheduler_resource_allocation_error</see>.</para>
    /// </remarks>
    /// <seealso cref="Context::Unblock Method"/>
    /// <seealso cref="Task Scheduler (Concurrency Runtime)"/>
    /**/
    _CRTIMP static void __cdecl Block();

    /// <summary>
    ///     Unblocks the context and causes it to become runnable.
    /// </summary>
    /// <remarks>
    ///     It is perfectly legal for a call to the <c>Unblock</c> method to come before a corresponding call to the <see cref="Context::Block Method">
    ///     Block</see> method. As long as calls to the <c>Block</c> and <c>Unblock</c> methods are properly paired, the runtime properly handles the natural race of
    ///     either ordering.  An <c>Unblock</c> call coming before a <c>Block</c> call simply negates the effect of the <c>Block</c> call.
    ///     <para>There are several exceptions which can be thrown from this method.  If a context attempts to call the <c>Unblock</c> method on
    ///     itself, a <see cref="context_self_unblock Class">context_self_unblock</see> exception will be thrown.  If calls to <c>Block</c> and
    ///     <c>Unblock</c> are not properly paired (e.g.: two calls to <c>Unblock</c> are made for a context which is currently running), a
    ///     <see cref="context_unblock_unbalanced Class">context_unblock_unbalanced</see> exception will be thrown.</para>
    ///
    ///     <para>Be aware that there is a critical period between the point where your code publishes its context for another thread to
    ///     be able to call the <c>Unblock</c> method and the point where the actual method call to <c>Block</c> is made.  During this period,
    ///     it is imperative that you not call any method which may in turn block and unblock for its own reasons (e.g.: acquiring a lock).
    ///     Calls to the <c>Block</c> and <c>Unblock</c> method do not track the reason for the blocking and unblocking.  Only one object should have
    ///     ownership of a <c>Block</c> and <c>Unblock</c> pair.</para>
    /// </remarks>
    /// <seealso cref="Context::Block Method"/>
    /// <seealso cref="Task Scheduler (Concurrency Runtime)"/>
    /**/
    virtual void Unblock() =0;

    /// <summary>
    ///     Determines whether or not the context is synchronously blocked.  A context is considered to be synchronously
    ///     blocked if it explicitly performed an action which led to blocking.
    /// </summary>
    /// <returns>
    ///     Whether the context is synchronously blocked.
    /// </returns>
    /// <remarks>
    ///     A context is considered to be synchronously blocked if it explicitly performed an action which led to blocking.  On the thread scheduler,
    ///     this would indicate a call to the <c>Context::Block</c> method or a synchronization object which was built using the runtime's
    ///     <c>Context::Block</c> method.  On the UMS scheduler, this could also indicate a call to a Windows method which explicitly blocked
    ///     using Windows synchronization primitives.  Page faults, thread suspensions, and other asynchronous events which the UMS scheduler is aware
    ///     of do not constitute synchronous blocking.
    ///     <para>The return value from this method is an instantaneous sample of whether the context is synchronously blocked.  This value may
    ///     be stale the moment it is returned and can only be used under very specific circumstances.</para>
    /// </remarks>
    /// <seealso cref="Context::Block Method"/>
    /**/
    virtual bool IsSynchronouslyBlocked() const =0;

    /// <summary>
    ///     Yields execution so that another context may execute.  If no other context is available to yield to,
    ///     the method simply returns.
    /// </summary>
    /// <remarks>
    ///     This yield variant is intended for use within spin loops.
    ///     <para>This method will result in the process' default scheduler being created and/or attached to the calling context if there is no
    ///     scheduler currently associated with the calling context.</para>
    /// </remarks>
    /**/
    _CRTIMP static void __cdecl _SpinYield();

    /// <summary>
    ///     Yields execution so that another context may execute.  If no other context is available to yield to, the scheduler
    ///     may yield to another operating system thread.
    /// </summary>
    /// <remarks>
    ///     This method will result in the process' default scheduler being created and/or attached to the calling context if there is no
    ///     scheduler currently associated with the calling context.
    /// </remarks>
    /// <seealso cref="Context::Block Method"/>
    /// <seealso cref="Context::Unblock Method"/>
    /**/
    _CRTIMP static void __cdecl Yield();

    /// <summary>
    ///     Returns an indication of whether the task collection which is currently executing inline on the current context
    ///     is in the midst of an active cancellation (or will be shortly).
    /// </summary>
    /// <returns>
    ///     If a scheduler is attached to the calling context and a task group is executing a task inline on that context,
    ///     an indication of whether that task group is in the midst of an active cancellation (or will be shortly); otherwise,
    ///     the value <c>false</c>.
    ///     <para>This method will not result in scheduler attachment if the calling context is not already associated with a scheduler.</para>
    /// </returns>
    /**/
    _CRTIMP static bool __cdecl IsCurrentTaskCollectionCanceling();

    /// <summary>
    ///     Returns a pointer to the current context.
    /// </summary>
    /// <returns>
    ///     A pointer to the current context.
    /// </returns>
    /// <remarks>
    ///     This method will result in the process' default scheduler being created and/or attached to the calling context if there is no
    ///     scheduler currently associated with the calling context.
    /// </remarks>
    /**/
    _CRTIMP static Context * __cdecl CurrentContext();

    /// <summary>
    ///     Injects an additional virtual processor into a scheduler for the duration of a block of code when invoked on a context executing
    ///     on one of the virtual processors in that scheduler.
    /// </summary>
    /// <param name="_BeginOversubscription">
    ///     If <c>true</c>, an indication that an extra virtual processor should be added for the duration of the oversubscription.
    ///     If <c>false</c>, an indication that the oversubscription should end and the previously added virtual processor should be removed.
    /// </param>
    /// <remarks>
    ///     Note that on a scheduler which utilizes user mode schedulable (UMS) threads, this method does not inject a new virtual processor.  The
    ///     UMS scheduler can detect kernel blocking and choose to run a new context on a virtual processor whose executing context blocked.
    ///     <para>For more information on using oversubscription in your application, see <see cref="Task Scheduler (Concurrency Runtime)"/>.</para>
    /// </remarks>
    /// <seealso cref="Task Scheduler (Concurrency Runtime)"/>
    /**/
    _CRTIMP static void __cdecl Oversubscribe(bool _BeginOversubscription);

protected:

    //
    // Privatize operator delete.  The scheduler internally manages contexts.
    //
    template<class _T> friend void Concurrency::details::_InternalDeleteHelper(_T * _PObject);

    /// <summary>
    ///     A <c>Context</c> object is destroyed internally by the runtime. It may not be explicitly deleted.
    /// </summary>
    /// <param name="_PObject">
    ///     A pointer to the object to be deleted.
    /// </param>
    /**/
    void operator delete(void * _PObject)
    {
        ::operator delete(_PObject);
    }
};

/// <summary>
///     A non-reentrant mutex which is explicitly aware of the Concurrency Runtime.
/// </summary>
/// <remarks>
///     For more information, see <see cref="Synchronization Data Structures"/>.
/// </remarks>
/// <seealso cref="reader_writer_lock Class"/>
/**/
class critical_section
{
public:

    /// <summary>
    ///     Constructs a new critical section.
    /// </summary>
    /**/
    _CRTIMP critical_section();

    /// <summary>
    ///     Destroys a critical section.
    /// </summary>
    /// <remarks>
    ///     It is expected that the lock is no longer held when the destructor runs.  Allowing the critical section to destruct with the lock
    ///     still held results in undefined behavior.
    /// </remarks>
    /**/
    _CRTIMP ~critical_section();

    /// <summary>
    ///     Acquires this critical section.
    /// </summary>
    /// <remarks>
    ///     It is often safer to utilize the <see cref="critical_section::scoped_lock Class">scoped_lock</see> construct to acquire and release
    ///     a <c>critical_section</c> object in an exception safe way.
    ///     <para>If the lock is already held by the calling context, an <see cref="improper_lock Class">improper_lock</see> exception will be
    ///     thrown.</para>
    /// </remarks>
    /// <seealso cref="critical_section::unlock Method"/>
    /// <seealso cref="critical_section::scoped_lock Class"/>
    /**/
    _CRTIMP void lock();

    /// <summary>
    ///     Tries to acquire the lock without blocking.
    /// </summary>
    /// <returns>
    ///     If the lock was acquired, the value <c>true</c>; otherwise, the value <c>false</c>.
    /// </returns>
    /// <seealso cref="critical_section::unlock Method"/>
    /**/
    _CRTIMP bool try_lock();

    /// <summary>
    ///     Unlocks the critical section.
    /// </summary>
    /// <remarks>
    ///     If the lock is not held by the calling thread, an <see cref="improper_unlock Class">improper_unlock</see> exception will be thrown.
    /// </remarks>
    /// <seealso cref="critical_section::lock Method"/>
    /// <seealso cref="critical_section::try_lock Method"/>
    /**/
    _CRTIMP void unlock();

    /// <summary>
    ///     A reference to a <c>critical_section</c> object.
    /// </summary>
    /**/
    typedef critical_section& native_handle_type;

    /// <summary>
    ///     Returns a platform specific native handle, if one exists.
    /// </summary>
    /// <returns>
    ///     A reference to the critical section.
    /// </returns>
    /// <remarks>
    ///     A <c>critical_section</c> object is not associated with a platform specific native handle for the Windows operating system.
    ///     The method simply returns a reference to the object itself.
    /// </remarks>
    /**/
    _CRTIMP native_handle_type native_handle();

    /// <summary>
    ///     Guarantees that if any context holds the lock at the time the method is called, that context has released
    ///     the lock before this method returns.
    /// </summary>
    /// <remarks>
    ///     If no context holds the lock at the instant this method is called, it returns instantly.
    /// </remarks>
    /**/
    void _Flush_current_owner();

    /// <summary>
    ///     Acquires this critical section given a specific node to lock.
    /// </summary>
    /// <param name="_PLockingNode">
    ///     The node that needs to own the lock.
    /// </param>
    /// <param name="_FHasExternalNode">
    ///     An indication if the node being locked is external to the critical_section.
    /// </param>
    /// <remarks>
    ///     If the lock is already held by the calling context, an <see cref="improper_lock Class">.improper_lock</see> exception will be thrown.
    /// </remarks>
    /**/
    void _Acquire_lock(void * _PLockingNode, bool _FHasExternalNode);

    /// <summary>
    ///     An exception safe RAII wrapper for a <c>critical_section</c> object.
    /// </summary>
    /**/
    class scoped_lock
    {
    public:

        /// <summary>
        ///     Constructs a <c>scoped_lock</c> object and acquires the <c>critical_section</c> object passed in the <paramref name="_Critical_section"/>
        ///     parameter.  If the critical section is held by another thread, this call will block.
        /// </summary>
        /// <param name="_Critical_section">
        ///     The critical section to lock.
        /// </param>
        /// <seealso cref="critical_section Class"/>
        /**/
        explicit _CRTIMP scoped_lock(critical_section& _Critical_section);

        /// <summary>
        ///     Destroys a <c>scoped_lock</c> object and releases the critical section supplied in its constructor.
        /// </summary>
        /// <seealso cref="critical_section Class"/>
        /**/
        _CRTIMP ~scoped_lock();

    private:

        critical_section& _M_critical_section;
        _CONCRT_BUFFER _M_node[(2 * sizeof(void *) + 1 * sizeof(unsigned int) + sizeof(_CONCRT_BUFFER) - 1) / sizeof(_CONCRT_BUFFER)];

        scoped_lock(const scoped_lock&);                    // no copy constructor
        scoped_lock const & operator=(const scoped_lock&);  // no assignment operator
    };

private:
    /// <summary>
    ///     The node allocated on the stack never really owns the lock. The reason for that is that
    ///     it would go out of scope and its insides would not be visible in unlock() where it would potentially
    ///     need to unblock the next in the queue. Instead, its state is transferred to the internal
    ///     node which is used as a scratch node.
    /// </summary>
    /// <param name="_PLockingNode">
    ///     The node that needs to own the lock.
    /// </param>
    /**/
    void _Switch_to_active(void * _PLockingNode);

    _CONCRT_BUFFER  _M_activeNode[(2 * sizeof(void *) + 1 * sizeof(unsigned int) + sizeof(_CONCRT_BUFFER) - 1) / sizeof(_CONCRT_BUFFER)];
    void * volatile _M_pHead;
    void * volatile _M_pTail;

    /// <summary>
    ///     Hide copy constructor for a critical section
    /// </summary>
    /**/
    critical_section(const critical_section&);

    /// <summary>
    ///     Hide assignment operator for a critical section
    /// </summary>
    /**/
    critical_section& operator=(const critical_section&);
};

/// <summary>
///    A writer-preference queue-based reader-writer lock with local only spinning.  The lock grants first in - first out (FIFO) access to writers
///    and starves readers under a continuous load of writers.
/// </summary>
/// <remarks>
///     For more information, see <see cref="Synchronization Data Structures"/>.
/// </remarks>
/// <seealso cref="critical_section Class"/>
/**/
class reader_writer_lock
{
public:

    /// <summary>
    ///     Constructs a new <c>reader_writer_lock</c> object.
    /// </summary>
    /**/
    _CRTIMP reader_writer_lock();

    /// <summary>
    ///     Destroys the <c>reader_writer_lock</c> object.
    /// </summary>
    /// <remarks>
    ///     It is expected that the lock is no longer held when the destructor runs.  Allowing the reader writer lock to destruct with the lock
    ///     still held results in undefined behavior.
    /// </remarks>
    /**/
    _CRTIMP ~reader_writer_lock();

    /// <summary>
    ///     Acquires the reader-writer lock as a writer.
    /// </summary>
    /// <remarks>
    ///     It is often safer to utilize the <see cref="reader_writer_lock::scoped_lock Class">scoped_lock</see> construct to acquire and release
    ///     a <c>reader_writer_lock</c> object as a writer in an exception safe way.
    ///     <para>Once a writer attempts to acquire the lock, any future readers will block until the writers have successfully acquired
    ///     and released the lock. This lock is biased towards writers and can starve readers under a continuous load of writers.</para>
    ///     <para>Writers are chained so that a writer exiting the lock releases the next writer in line.</para>
    ///     <para>If the lock is already held by the calling context, an <see cref="improper_lock Class">improper_lock</see> exception will be
    ///     thrown.</para>
    /// </remarks>
    /// <seealso cref="reader_writer_lock::unlock Method"/>
    /**/
    _CRTIMP void lock();

    /// <summary>
    ///     Attempts to acquire the reader-writer lock as a writer without blocking.
    /// </summary>
    /// <returns>
    ///     If the lock was acquired, the value <c>true</c>; otherwise, the value <c>false</c>.
    /// </returns>
    /// <seealso cref="reader_writer_lock::unlock Method"/>
    /**/
    _CRTIMP bool try_lock();

    /// <summary>
    ///     Acquires the reader-writer lock as a reader. If there are writers, active readers have to wait until they are done.
    ///     The reader simply registers an interest in the lock and waits for writers to release it.
    /// </summary>
    /// <remarks>
    ///     It is often safer to utilize the <see cref="reader_writer_lock::scoped_lock_read Class">scoped_lock_read</see> construct to acquire
    ///     and release a <c>reader_writer_lock</c> object as a reader in an exception safe way.
    ///     <para>If there are writers waiting on the lock, the reader will wait until all writers in line have acquired
    ///     and released the lock.  This lock is biased towards writers and can starve readers under a continuous load of writers.</para>
    /// </remarks>
    /// <seealso cref="reader_writer_lock::unlock Method"/>
    /**/
    _CRTIMP void lock_read();

    /// <summary>
    ///     Attempts to acquire the reader-writer lock as a reader without blocking.
    /// </summary>
    /// <returns>
    ///     If the lock was acquired, the value <c>true</c>; otherwise, the value <c>false</c>.
    /// </returns>
    /// <seealso cref="reader_writer_lock::unlock Method"/>
    /**/
    _CRTIMP bool try_lock_read();

    /// <summary>
    ///     Unlocks the reader-writer lock based on who locked it, reader or writer.
    /// </summary>
    /// <remarks>
    ///     If there are writers waiting on the lock, the release of the lock will always go to the next writer in FIFO
    ///     order.   This lock is biased towards writers and can starve readers under a continuous load of writers.
    /// </remarks>
    /// <seealso cref="reader_writer_lock::lock Method"/>
    /// <seealso cref="reader_writer_lock::lock_read Method"/>
    /// <seealso cref="reader_writer_lock::try_lock Method"/>
    /// <seealso cref="reader_writer_lock::try_lock_read Method"/>
    /**/
    _CRTIMP void unlock();

    /// <summary>
    ///     Acquires a write lock given a specific write node to lock.
    /// </summary>
    /// <param name="_PLockingNode">
    ///     The node that needs to own the lock.
    /// </param>
    /// <param name="_FHasExternalNode">
    ///     An indication if the node being locked is external to the <c>reader_writer_lock</c> object.
    /// </param>
    /// <remarks>
    ///     If the lock is already held by the calling context, an <see cref="improper_lock Class">.improper_lock</see> exception will be
    ///     thrown.
    /// </remarks>
    /**/
    void _Acquire_lock(void * _PLockingNode, bool _FHasExternalNode);

    /// <summary>
    ///     An exception safe RAII wrapper that can be used to acquire <c>reader_writer_lock</c> lock objects as a writer.
    /// </summary>
    /**/
    class scoped_lock
    {
    public:
        /// <summary>
        ///     Constructs a <c>scoped_lock</c> object and acquires the <c>reader_writer_lock</c> object passed in the
        ///     <paramref name="_Reader_writer_lock"/> parameter as a writer.  If the lock is held by another thread, this call will block.
        /// </summary>
        /// <param name="_Reader_writer_lock">
        ///     The <c>reader_writer_lock</c> object to acquire as a writer.
        /// </param>
        /**/
        explicit _CRTIMP scoped_lock(reader_writer_lock& _Reader_writer_lock);

        /// <summary>
        ///     Destroys a <c>reader_writer_lock</c> object and releases the lock supplied in its constructor.
        /// </summary>
        /**/
        _CRTIMP ~scoped_lock();

    private:

        reader_writer_lock& _M_reader_writer_lock;
        _CONCRT_BUFFER _M_writerNode[(2 * sizeof(void *) + 1 * sizeof(unsigned int) + sizeof(_CONCRT_BUFFER) - 1) / sizeof(_CONCRT_BUFFER)];

        scoped_lock(const scoped_lock&);                    // no copy constructor
        scoped_lock const & operator=(const scoped_lock&);  // no assignment operator
    };

    /// <summary>
    ///     An exception safe RAII wrapper that can be used to acquire <c>reader_writer_lock</c> lock objects as a reader.
    /// </summary>
    /**/
    class scoped_lock_read
    {
    public:
        /// <summary>
        ///     Constructs a <c>scoped_lock_read</c> object and acquires the <c>reader_writer_lock</c> object passed in the
        ///     <paramref name="_Reader_writer_lock"/> parameter as a reader.  If the lock is held by another thread as a writer or there
        ///     are pending writers, this call will block.
        /// </summary>
        /// <param name="_Reader_writer_lock">
        ///     The <c>reader_writer_lock</c> object to acquire as a reader.
        /// </param>
        /**/
        explicit _CRTIMP scoped_lock_read(reader_writer_lock& _Reader_writer_lock);

        /// <summary>
        ///     Destroys a <c>scoped_lock_read</c> object and releases the lock supplied in its constructor.
        /// </summary>
        /**/
        _CRTIMP ~scoped_lock_read();

    private:

        reader_writer_lock& _M_reader_writer_lock;

        scoped_lock_read(const scoped_lock_read&);                    // no copy constructor
        scoped_lock_read const & operator=(const scoped_lock_read&);  // no assignment operator
    };

private:

    /// <summary>
    ///     Called for the first context in the writer queue. It sets the queue head and it tries to
    ///     claim the lock if readers are not active.
    /// </summary>
    /// <param name="_PWriter">
    ///     The first writer in the queue.
    /// </param>
    /**/
    bool _Set_next_writer(void * _PWriter);

    /// <summary>
    ///     Called when writers are done with the lock, or when lock was free for claiming by
    ///     the first reader coming in. If in the meantime there are more writers interested
    ///     the list of readers is finalized and they are convoyed, while head of the list
    ///     is reset to NULL.
    /// </summary>
    /// <returns>
    ///     Pointer to the head of the reader list.
    /// </returns>
    /**/
    void * _Get_reader_convoy();

    /// <summary>
    ///     Called from unlock() when a writer is holding the lock. Writer unblocks the next writer in the list
    ///     and is being retired. If there are no more writers, but there are readers interested, then readers
    ///     are unblocked.
    /// </summary>
    /**/
    void _Unlock_writer();

    /// <summary>
    ///     Called from unlock() when a reader is holding the lock. Reader count is decremented and if this
    ///     is the last reader it checks whether there are interested writers that need to be unblocked.
    /// </summary>
    /**/
    void _Unlock_reader();

    /// <summary>
    ///     When the last writer leaves the lock, it needs to reset the tail to NULL so that the next coming
    ///     writer would know to try to grab the lock. If the CAS to NULL fails, then some other writer
    ///     managed to grab the tail before the reset, so this writer needs to wait until the link to
    ///     the next writer is complete before trying to release the next writer.
    /// </summary>
    /// <param name="_PWriter">
    ///     Last writer in the queue.
    /// </param>
    /**/
    void _Remove_last_writer(void * _PWriter);

    /// <summary>
    ///     The writer node allocated on the stack never really owns the lock. The reason for that is that
    ///     it would go out of scope and its insides would not be visible in unlock() where it would potentially
    ///     need to unblock the next writer in the queue. Instead, its state is transferred to the internal
    ///     writer node which is used as a scratch node.
    /// </summary>
    /// <param name="_PWriter">
    ///     The writer that needs to own the lock.
    /// </param>
    /**/
    void _Switch_to_active(void * _PWriter);

    _CONCRT_BUFFER _M_activeWriter[(2 * sizeof(void *) + 1 * sizeof(unsigned int) + sizeof(_CONCRT_BUFFER) - 1) / sizeof(_CONCRT_BUFFER)];
    void *         _M_pReaderHead;
    void *         _M_pWriterHead;
    void *         _M_pWriterTail;
    volatile long  _M_lockState;

    /// <summary>
    ///     Hide copy constructor for a reader_writer_lock
    /// </summary>
    /**/
    reader_writer_lock (const reader_writer_lock& _Lock);

    /// <summary>
    ///     Hide assignment operator for a reader_writer_lock
    /// </summary>
    /**/
    reader_writer_lock& operator=(const reader_writer_lock& _Lock);
};

/// <summary>
///     Value indicating that a wait timed out.
/// </summary>
/// <seealso cref="event Class"/>
/// <seealso cref="event::wait Method"/>
/// <seealso cref="event::wait_for_multiple Method"/>
/**/
const size_t COOPERATIVE_WAIT_TIMEOUT = SIZE_MAX;

/// <summary>
///     Value indicating that a wait should never time out.
/// </summary>
/// <seealso cref="event Class"/>
/// <seealso cref="event::wait Method"/>
/// <seealso cref="event::wait_for_multiple Method"/>
/**/
const unsigned int COOPERATIVE_TIMEOUT_INFINITE = (unsigned int)-1;

/// <summary>
///     A manual reset event which is explicitly aware of the Concurrency Runtime.
/// </summary>
/// <remarks>
///     For more information, see <see cref="Synchronization Data Structures"/>.
/// </remarks>
/**/
class event
{
public:

    /// <summary>
    ///     Constructs a new event.
    /// </summary>
    /**/
    _CRTIMP event();

    /// <summary>
    ///     Destroys an event.
    /// </summary>
    /// <remarks>
    ///     It is expected that there are no threads waiting on the event when the destructor runs.  Allowing the event to destruct with threads
    ///     still waiting upon it results in undefined behavior.
    /// </remarks>
    /**/
    _CRTIMP ~event();

    /// <summary>
    ///     Waits for the event to become signaled.
    /// </summary>
    /// <param name="_Timeout">
    ///     Indicates the number of milliseconds before the wait times out.  The value <c>COOPERATIVE_TIMEOUT_INFINITE</c> signifies that
    ///     there is no timeout.
    /// </param>
    /// <returns>
    ///     If the wait was satisfied, the value <c>0</c> is returned; otherwise, the value <c>COOPERATIVE_WAIT_TIMEOUT</c> to indicate that
    ///     the wait timed out without the event becoming signaled.
    /// </returns>
    /// <seealso cref="event::set Method"/>
    /// <seealso cref="COOPERATIVE_TIMEOUT_INFINITE Variable">COOPERATIVE_TIMEOUT_INFINITE</seealso>
    /// <seealso cref="COOPERATIVE_WAIT_TIMEOUT Variable">COOPERATIVE_WAIT_TIMEOUT</seealso>
    /**/
    _CRTIMP size_t wait(unsigned int _Timeout = COOPERATIVE_TIMEOUT_INFINITE);

    /// <summary>
    ///     Signals the event.
    /// </summary>
    /// <remarks>
    ///     Signaling the event may cause an arbitrary number of contexts which are waiting on the event to become runnable.
    /// </remarks>
    /// <seealso cref="event::wait Method"/>
    /// <seealso cref="event::reset Method"/>
    /**/
    _CRTIMP void set();

    /// <summary>
    ///     Resets the event to a non-signaled state.
    /// </summary>
    /// <seealso cref="event::set Method"/>
    /// <seealso cref="event::wait Method"/>
    /**/
    _CRTIMP void reset();

    /// <summary>
    ///     Waits for multiple events to become signaled.
    /// </summary>
    /// <param name="_PPEvents">
    ///     An array of events to wait on.  The number of events within the array is indicated by the <paramref name="_Count"/> parameter.
    /// </param>
    /// <param name="_Count">
    ///     The count of events within the array supplied in the <paramref name="_PPEvents"/> parameter.
    /// </param>
    /// <param name="_FWaitAll">
    ///     If set to the value <c>true</c>, the parameter specifies that all events within the array supplied in the <paramref name="_PPEvents"/>
    ///     parameter must become signaled in order to satisfy the wait.  If set to the value <c>false</c>, it specifies that any event within the
    ///     array supplied in the <paramref name="_PPEvents"/>  parameter becoming signaled will satisfy the wait.
    /// </param>
    /// <param name="_Timeout">
    ///     Indicates the number of milliseconds before the wait times out.  The value <c>COOPERATIVE_TIMEOUT_INFINITE</c> signifies that
    ///     there is no timeout.
    /// </param>
    /// <returns>
    ///     If the wait was satisfied, the index within the array supplied in the <paramref name="_PPEvents"/> parameter which satisfied
    ///     the wait condition; otherwise, the value <c>COOPERATIVE_WAIT_TIMEOUT</c> to indicate that the wait timed out without the condition
    ///     being satisfied.
    /// </returns>
    /// <remarks>
    ///     If the parameter <paramref name="_FWaitAll"/> is set to the value <c>true</c> to indicate that all events must become signaled to satisfy
    ///     the wait, the index returned by the function carries no special significance other than the fact that it is not the value
    ///     <c>COOPERATIVE_WAIT_TIMEOUT</c>.
    /// </remarks>
    /// <seealso cref="event::wait Method"/>
    /// <seealso cref="COOPERATIVE_TIMEOUT_INFINITE Variable">COOPERATIVE_TIMEOUT_INFINITE</seealso>
    /// <seealso cref="COOPERATIVE_WAIT_TIMEOUT Variable">COOPERATIVE_WAIT_TIMEOUT</seealso>
    /**/
    _CRTIMP static size_t __cdecl wait_for_multiple(event ** _PPEvents, size_t _Count, bool _FWaitAll, unsigned int _Timeout = COOPERATIVE_TIMEOUT_INFINITE);

private:

    // Prevent bad usage of copy-constructor and copy-assignment
    event(const event& _Event);
    event& operator=(const event& _Event);

    void * volatile _M_pWaitChain;
    void * _M_pResetChain;
    Concurrency::critical_section _M_lock;
};

namespace details
{
    // This is a non-reentrant lock wrapper around the ConcRT critical-section
    // and used by agents/messaging
    class _NonReentrantPPLLock
    {
    public:

        // Constructor for _NonReentrantPPLLock
        _CRTIMP _NonReentrantPPLLock();

        // Acquire the lock, spin if necessary
        _CRTIMP void _Acquire(void * _Lock_node);

        // Releases the lock
        _CRTIMP void _Release();

        // An exception safe RAII wrapper.
        class _Scoped_lock
        {
        public:
            // Constructs a holder and acquires the specified lock
            _CRTIMP explicit _Scoped_lock(_NonReentrantPPLLock& _Lock);

            // Destroys the holder and releases the lock
            _CRTIMP ~_Scoped_lock();

        private:
            _NonReentrantPPLLock& _M_lock;
            _CONCRT_BUFFER  _M_lockNode[(3 * sizeof(void *) + sizeof(_CONCRT_BUFFER) - 1) / sizeof(_CONCRT_BUFFER)];

            _Scoped_lock(const _Scoped_lock&);                    // no copy constructor
            _Scoped_lock const & operator=(const _Scoped_lock&);  // no assignment operator
        };

    private:
        // critical_section
        Concurrency::critical_section _M_criticalSection;
    };

    // This is a reentrant lock implemented using the ConcRT critical section
    class _ReentrantPPLLock
    {
    public:
        // Constructor for _ReentrantPPLLock
        _CRTIMP _ReentrantPPLLock();

        // Acquire the lock, spin if necessary
        _CRTIMP void _Acquire(void * _Lock_node);

        // Releases the lock
        _CRTIMP void _Release();

        // An exception safe RAII wrapper.
        class _Scoped_lock
        {
        public:
            // Constructs a holder and acquires the specified lock
            _CRTIMP explicit _Scoped_lock(_ReentrantPPLLock& _Lock);

            // Destroys the holder and releases the lock
            _CRTIMP ~_Scoped_lock();

        private:
            _ReentrantPPLLock& _M_lock;
            _CONCRT_BUFFER  _M_lockNode[(3 * sizeof(void *) + sizeof(_CONCRT_BUFFER) - 1) / sizeof(_CONCRT_BUFFER)];

            _Scoped_lock(const _Scoped_lock&);                    // no copy constructor
            _Scoped_lock const & operator=(const _Scoped_lock&);  // no assignment operator
        };

    private:
        // critical_section
        Concurrency::critical_section _M_criticalSection;

        // The number of times this lock has been taken recursively
        long _M_recursionCount;

        // The current owner of the lock
        volatile long _M_owner;
    };

    struct _Chore
    {
    protected:
        // Constructors.
        explicit _Chore(TaskProc _PFunction) : m_pFunction(_PFunction)
        {
        }

        _Chore()
        {
        }

        virtual ~_Chore()
        {
        }

    public:

        // The function which invokes the work of the chore.
        TaskProc m_pFunction;
    };

    // _UnrealizedChore represents an unrealized chore -- a unit of work that scheduled in a work
    // stealing capacity.  Some higher level construct (language or library) will map atop this to provide
    // an usable abstraction to clients.
    class _UnrealizedChore : public _Chore, public _AllocBase
    {
    public:
        // Constructor for an unrealized chore.
        _UnrealizedChore() :
            _M_pTaskCollection(NULL)
        {
        }

        // Method that executes the unrealized chore.
        void _Invoke()
        {
            _M_pChoreFunction(this);
        }

        // Sets the attachment state of the chore at the time of stealing.
        void _SetDetached(bool _FDetached);

        // Returns the owning collection of the chore.
        Concurrency::details::_TaskCollectionBase* _OwningCollection() const
        {
            return _M_pTaskCollection;
        }

        // Set flag that indicates whether the scheduler owns the lifetime of the object and is responsible for freeing it.
        // The flag is ignored by _StructuredTaskCollection
        void _SetRuntimeOwnsLifetime(bool fValue)
        {
            _M_fRuntimeOwnsLifetime = fValue;
        }

        // Returns the flag that indicates whether the scheduler owns the lifetime of the object and is responsible for freeing it.
        // The flag is ignored by _StructuredTaskCollection
        bool _GetRuntimeOwnsLifetime() const
        {
            return _M_fRuntimeOwnsLifetime;
        }

        // Allocator to be used when runtime owns lifetime.
        template <typename _ChoreType, typename _Function>
        static _ChoreType * _InternalAlloc(const _Function& _Func)
        {
            _ChoreType * _Chore = new _ChoreType(_Func);
            _Chore->_M_fRuntimeOwnsLifetime = true;
            return _Chore;
        }

    protected:
        // Invocation bridge between the _UnrealizedChore and PPL.
        template <typename _ChoreType>
        static void __cdecl _InvokeBridge(_ChoreType * _PChore)
        {
            (*_PChore)();
        }

        // Place associated task collection in a safe state.
        _CRTIMP void _CheckTaskCollection();

    private:

        friend class _StructuredTaskCollection;
        friend class _TaskCollection;
        typedef void (__cdecl * CHOREFUNC)(_UnrealizedChore * _PChore);

        // The collection of work to which this particular chore belongs.
        Concurrency::details::_TaskCollectionBase * _M_pTaskCollection;

        // Internal invocation inside the scheduler.
        CHOREFUNC _M_pChoreFunction;

        // Indicates whether the scheduler owns the lifetime of the object and is responsible for freeing it.
        // This flag is ignored by _StructuredTaskCollection
        bool _M_fRuntimeOwnsLifetime;

        // An indication of whether the chore (if stolen) was detached.
        bool _M_fDetached;

        // The internal wrapper around invocation of stolen structured chores.
        __declspec(noinline)
        static void __cdecl _StructuredChoreWrapper(_UnrealizedChore * _PChore);

        // The internal wrapper around invocation of stolen unstructured chores.
        __declspec(noinline)
        static void __cdecl _UnstructuredChoreWrapper(_UnrealizedChore * _PChore);

        // To free memory allocated with _InternalAlloc.
        static void _InternalFree(_UnrealizedChore * _PChore);
    };

    // Represents possible results of waiting on a task collection.
    enum _TaskCollectionStatus
    {
        _NotComplete,
        _Completed,
        _Canceled
    };

    // _TaskCollectionBase represents an abstract set of work and provides shared waiting semantics for stolen work.
    class _TaskCollectionBase
    {
    public:
        // Constructs a new task collection.
        _TaskCollectionBase() :
            _M_completedStolenChores(_CollectionNotInitialized),
            _M_unpoppedChores(0),
            _M_pException(NULL),
            _M_inliningDepth(_S_notInlined)
        {
        }

        // Returns the owning context of the task collection.
        void * _OwningContext() const
        {
            return _M_pOwningContext;
        }

        // Returns the inlining depth.
        int _InliningDepth() const
        {
            return _M_inliningDepth;
        }

        // Returns whether this is a structured collection or not.
        bool _IsStructured()
        {
            return (_M_inlineFlags & _S_structured) != 0;
        }

    protected:

        friend class Concurrency::details::_UnrealizedChore;

        enum _TaskCollectionBaseState
        {
            _CollectionNotInitialized = LONG_MIN,
            _CollectionInitializationInProgress = LONG_MIN+1,
            _CollectionInitialized = 0
        };

        // Returns the exception portion of _M_pException.
        std::exception_ptr * _Exception() const
        {
            return (std::exception_ptr *) ((size_t)_M_pException & ~_S_cancelBitsMask);
        }

        // Indicates whether or not this task collection has an abnormal exit.
        bool _IsAbnormalExit() const
        {
            return _M_pException != NULL;
        }

        // Returns the cancel flags.
        size_t _CancelState() const
        {
            return (size_t) _M_pException & _S_cancelBitsMask;
        }

        // Returns whether or not the collection is marked for cancellation.
        bool _IsMarkedForCancellation() const
        {
            return (_CancelState() & _S_cancelBitsMask) != 0;
        }

        // Returns whether an inline cancellation was performed.
        bool _PerformedInlineCancel() const
        {
            _ASSERTE(_CancelState() != _S_cancelStarted);
            return _CancelState() == _S_cancelShotdownOwner;
        }

        // Returns the parent collection safely.
        _TaskCollectionBase *_SafeGetParent()
        {
            return ((_M_inliningDepth != _S_notInlined) ? _M_pParent : NULL);
        }

        // Called in order to execute a supposed interruption point.
        void _Interrupt(bool _FLocalCondition, int _LocalFlags = 0);

        // Called in order to determine whether this task collection will interrupt for a pending cancellation at or above it.
        bool _WillInterruptForPendingCancel();

        // Called when an exception is raised on a chore on a given task collection, this makes a determination of what to do with the exception
        // and squirrels it away for potential transport back to the thread performing a join on a chore collection.
        void _RaisedException();

        // Potentially rethrows the exception which was set with _RaisedException.  The caller has responsibility to ensure that _RaisedException
        //  was called prior to calling this and that _M_pException has progressed beyond the _S_nonNull state.
        void _RethrowException();

        // Marks the collection for cancellation and returns whether the collection was thus marked.
        bool _MarkCancellation();

        // Finishes the cancellation state (changing from _S_cancelStarted to one of the other states).  Note that only the
        // thread which successfully marked cancellation may call this.
        void _FinishCancelState(size_t _NewCancelState);

        // Called when a cancellation is raised on a chore on a given task collection.  This makes a determination of what to do with the exception
        // and squirrels it away for potential transport back to the thread performing a join on a chore collection.  Note that every other exception
        // has precedence over a cancellation.
        void _RaisedCancel();

        // Tracks the parent collection.  (e.g.: A task collection B created during execution of a chore C on task collection A is
        // considered a child of A).
        _TaskCollectionBase * _M_pParent;

        // Tracks the inlining depth of this collection for cancellation purposes and packs a series of definition bits.
        int _M_inliningDepth : 28;
        int _M_inlineFlags : 4;

        // The context which owns the task collection.  This is the context where the collection is created.
        void * _M_pOwningContext;

        // The number of unpopped chores associated with the task collection (set by the derived
        // class during chore association.
        long _M_unpoppedChores;

        // The number of stolen chores executed so far.
        volatile long _M_completedStolenChores;

        // The stored exception which has been marshaled from the thread a stolen chore ran upon to the thread that is waiting on the
        // task collection.
        //
        // The lower two bits of _M_pException are utilized for the cancellation state machine.  The upper 30 are the exception pointer.  This implies
        // that the excepiton pointer must be 4-byte aligned.  Because of intermediate states, the exception pointer cannot be between 0x8 and 0xF.  The heap should
        // not be allocating such...
        //
        std::exception_ptr * _M_pException;

        // Cancellation states
        static const size_t _S_cancelBitsMask = 0x3;
        static const size_t _S_cancelNone = 0x0;
        static const size_t _S_cancelStarted = 0x1;
        static const size_t _S_cancelDeferredShootdownOwner = 0x2;
        static const size_t _S_cancelShotdownOwner = 0x3;

        // Intermediate exceptions.
        static const size_t _S_nonNull = 0x8;
        static const size_t _S_cancelException = 0xC;

        // initialization state for inlining depth.
        static const int _S_notInlined = -1;

        // Inline flags.
        static const int _S_structured = 0x00000001;
        static const int _S_localCancel = 0x00000002;
        static const int _S_reserved = 0x0000000C;

    private:

        // Prevent bad usage of copy-constructor and copy-assignment
        _TaskCollectionBase(const _TaskCollectionBase& _Collection);
        _TaskCollectionBase& operator=(const _TaskCollectionBase& _Collection);
    };

    /// <summary>
    ///     Structured task collections represent groups of work which follow a strictly LIFO ordered paradigm
    ///     queueing and waiting respectively.  They can only be waited upon once (via object destructor)
    ///     and can only be used from a single thread of execution.
    /// </summary>
    /**/
    class _StructuredTaskCollection : public _TaskCollectionBase
    {
    public:

        /// <summary>
        ///     Construct a new structured task collection.
        /// </summary>
        /**/
        _StructuredTaskCollection()
        {
            _M_pOwningContext = NULL;
            _M_inlineFlags = _S_structured;
        }

        /// <summary>
        ///     Destruct a task collection and wait on all associated work to finish. Clients must call '_StructuredTaskCollection::Wait'
        ///     or '_StructuredTaskCollection::Cancel' prior to destructing the object.  If there are chores remaining in the queues, an
        ///     exception (missing_wait) is thrown. If the destructor is running due to exception unwinding, it will abort any scheduled work.
        ///     If another exception occurs because work is aborted, the process will terminate (C++ semantics).
        /// </summary>
        /**/
        ~_StructuredTaskCollection()
        {
            if (!_TaskCleanup())
            {
                throw missing_wait();
            }
        }

        /// <summary>
        ///     Schedules a chore that may potentially run in parallel.  The chore is pushed onto the associated workstealing queue, and
        ///     will be executed in a LIFO order.Note that the specified chore may only be scheduled upon a single task collection at a given time.
        ///     Any attempt to schedule the same chore multiple times on one or more task collection will result in an invalid_multiple_scheduling
        ///     exception.  Once the chore is guaranteed to have been executed (via calling the Wait method), it can be rescheduled to an
        ///     arbitrary task collection.
        /// </summary>
        /// <param name="_PChore">
        ///     The new unrealized chore to schedule
        /// </param>
        /**/
        _CRTIMP void _Schedule(_UnrealizedChore * _PChore);

        /// <summary>
        ///     Cancels work on the task collection.
        /// </summary>
        /**/
        _CRTIMP void _Cancel();

        /// <summary>
        ///     Informs the caller whether or not the task collection is currently in the midst of cancellation.  Note that this
        ///     does not necessarily indicate that Cancel was called on the collection (although such certainly qualifies this function
        ///     to return true).  It may be the case that the task collection is executing inline and a task collection further up in the work
        ///     tree was canceled.  In cases such as these where we can determine ahead of time that cancellation will flow through
        ///     this collection, true will be returned as well.
        /// </summary>
        /// <returns>
        ///     An indication of whether the task collection is in the midst of a cancellation (or is guaranteed to be shortly).
        /// </returns>
        /**/
        _CRTIMP bool _IsCanceling();

        /// <summary>
        ///     A cancellation friendly wrapper with which to execute _PChore and then
        ///     waits for all chores running in the _StructuredTaskCollection to finish (normally or abnormally). This method encapsulates
        ///     all the running tasks in an exception handling block, and will re-throw any exceptions that occur in any of it tasks
        ///     (if those exceptions occur on another thread, they are marshaled from that thread to the thread where the _StructuredTaskCollection
        ///     was created, and re-thrown). After this function returns, the _StructuredTaskCollection cannot be used for scheduling further work.
        /// </summary>
        /// <param name="_PChore">
        ///     An _UnrealizedChore which when non-null will be called to invoke the chore in a cancelation friendly manner.
        /// </param>
        /// <returns>
        ///     An indication of the status of the wait.
        /// </returns>
        /**/
        _CRTIMP _TaskCollectionStatus __stdcall _RunAndWait(_UnrealizedChore * _PChore = NULL);

        /// <summary>
        ///     Waits for all chores running in the _StructuredTaskCollection to finish (normally or abnormally). This method encapsulates
        ///     all the running tasks in an exception handling block, and will re-throw any exceptions that occur in any of it tasks
        ///     (if those exceptions occur on another thread, they are marshaled from that thread to the thread where the _StructuredTaskCollection
        ///     was created, and re-thrown). After this function returns, the _StructuredTaskCollection cannot be used for scheduling further work.
        /// </summary>
        /// <returns>
        ///     An indication of the status of the wait.
        /// </returns>
        /**/
        _TaskCollectionStatus _Wait()
        {
            return _RunAndWait();
        }

        /// <summary>
        ///     Called to cancel any contexts which stole chores from the given collection.
        /// </summary>
        /**/
        void _CancelStolenContexts();

    private:

        friend class _UnrealizedChore;

        /// <summary>
        ///     Internal routine to abort work on the task collection.
        /// </summary>
        /**/
        _CRTIMP void _Abort();

        /// <summary>
        ///     Performs task cleanup normally done at destruction time.
        /// </summary>
        /**/
        bool _TaskCleanup()
        {
            //
            // Users are required to call Wait() before letting the destructor run.  Otherwise, throw.  Note that before throwing,
            // we must actually wait on the tasks since they contain pointers into stack frames and unwinding without the wait is
            // instant stack corruption.
            //
            if (_M_unpoppedChores > 0)
            {
                _Abort();

                if (!__uncaught_exception())
                {
                    return false;
                }
            }

            return true;
        }

        /// <summary>
        ///     Internal initialization of the structured task collection
        /// </summary>
        /**/
        void _Initialize();

        /// <summary>
        ///     Waits on a specified number of stolen chores.
        /// </summary>
        /// <param name="_StolenChoreCount">
        ///     The number of stolen chores to wait upon
        /// </param>
        /**/
        void _WaitOnStolenChores(long _StolenChoreCount);

        /// <summary>
        ///     Indicates that a stolen chore has completed.
        /// </summary>
        /**/
        void _CountUp();

        //
        // _StructuredTaskCollection::_M_event is used to construct an structured event object only when it is needed to block.  The structured event object
        //  has no state to cleanup, therefore no dtor code is required.
        //
        _CONCRT_BUFFER _M_event[(sizeof(void*) + sizeof(_CONCRT_BUFFER) - 1) / sizeof(_CONCRT_BUFFER)];
    };

    /// <summary>
    ///     Task collections represent groups of work which step outside the strict structuring of the
    ///     _StructuredTaskCollection definition.  Any groups of work which do not follow LIFO ordering, are waited
    ///     upon multiple times, or are passed between arbitrary threads require utilization of this definition
    ///     of a task collection.  It has additional overhead over the _StructuredTaskCollection.
    /// </summary>
    /**/
    class _TaskCollection : public _TaskCollectionBase
    {
    public:

        /// <summary>
        ///     Constructs a new task collection.
        /// </summary>
        /**/
        _CRTIMP _TaskCollection();

        /// <summary>
        ///     Destroys a task collection.  Clients must call '_TaskCollection::Wait' or '_TaskCollection::Cancel' prior to destructing
        ///     the object.  If there are chores remaining in the queues, an exception (missing_wait) is thrown. If the destructor
        ///     is running due to exception unwinding, it will abort any scheduled work.  If another exception occurs because work
        ///     is aborted, the process will terminate (C++ semantics).
        /// </summary>
        /**/
        _CRTIMP ~_TaskCollection();

        /// <summary>
        ///     Schedules a chore that may potentially run in parallel.  The chore is pushed onto the associated workstealing queue, and
        ///     will be executed in a LIFO order. The tasks scheduled into a _TaskCollection are scheduled into the current scheduler.
        ///     Note that the specified chore may only be scheduled upon a single task collection at a given time.  Any attempt to schedule the same
        ///     chore multiple times on one or more task collections will result in an invalid_multiple_scheduling exception.  Once the chore is
        ///     guaranteed to have been executed (via calling the Wait method), it can be rescheduled to an arbitrary task collection.
        /// </summary>
        /// <param name="_PChore">
        ///     The new unrealized chore to schedule
        /// </param>
        /**/
        _CRTIMP void _Schedule(_UnrealizedChore * _PChore);

        /// <summary>
        ///     Cancels work on the task collection.
        /// </summary>
        /**/
        _CRTIMP void _Cancel();

        /// <summary>
        ///     Informs the caller whether or not the task collection is currently in the midst of a cancellation.  Note that this
        ///     does not necessarily indicate that Cancel was called on the collection (although such certainly qualifies this function
        ///     to return true).  It may be the case that the task collection is executing inline and a task collection further up in the work
        ///     tree was canceled.  In cases such as these where we can determine ahead of time that cancellation will flow through
        ///     this collection, true will be returned as well.
        /// </summary>
        /// <returns>
        ///     An indication of whether the task collection is in the midst of a cancellation (or is guaranteed to be shortly).
        /// </returns>
        /**/
        _CRTIMP bool _IsCanceling();

        /// <summary>
        ///     A cancellation friendly wrapper with which to execute _PChore and then
        ///     waits for all chores running in the _TaskCollection to finish (normally or abnormally). This method encapsulates
        ///     all the running tasks in an exception handling block, and will re-throw any exceptions that occur in any of it tasks
        ///     (if those exceptions occur on another thread, they are marshaled from that thread to the thread where the _TaskCollection
        ///     was created, and re-thrown). After this function returns, the  _TaskCollection cannot be used for scheduling further work.
        /// </summary>
        /// <param name="_PChore">
        ///     An _UnrealizedChore which when non-null will be called to invoke the chore in a cancelation friendly manner.
        /// </param>
        /// <returns>
        ///     An indication of the status of the wait.
        /// </returns>
        /// </summary>
        /**/
        _CRTIMP _TaskCollectionStatus __stdcall _RunAndWait(_UnrealizedChore * _PChore = NULL);

        /// <summary>
        ///     Waits for all chores running in the _TaskCollection to finish (normally or abnormally). This method encapsulates
        ///     all the running tasks in an exception handling block, and will re-throw any exceptions that occur in any of it tasks
        ///     (if those exceptions occur on another thread, they are marshaled from that thread to the thread where the _TaskCollection
        ///     was created, and re-thrown). After this function returns, the  _TaskCollection cannot be used for scheduling further work.
        /// </summary>
        /// <returns>
        ///     An indication of the status of the wait.
        /// </returns>
        /// </summary>
        /**/
        _TaskCollectionStatus _Wait()
        {
            return _RunAndWait();
        }

        /// <summary>
        ///     Returns whether this task collection is marked for abnormal exit.
        /// </summary>
        /**/
        bool _IsMarkedForAbnormalExit() const;

    private:

        friend class _UnrealizedChore;
        friend class Concurrency::details::ContextBase;

        /// <summary>
        ///     Determines if the task collection is a stale alias (an object which was left over from a deferred delete
        ///     of a direct alias but which happens to match the hash key for a newly allocated task collection)
        /// </summary>
        /**/
        bool _IsStaleAlias() const;

        /// <summary>
        ///     Releases an alias -- this will free it if the release is the last man out.
        /// </summary>
        /**/
        void _ReleaseAlias();

        /// <summary>
        ///     Returns the object which this is an alias for.
        /// </summary>
        /**/
        _TaskCollection * _OriginalCollection() const;

        /// <summary>
        ///     Constructs an alias collection based on a specifed origin collection
        /// </summary>
        /// <param name="_POriginCollection">
        ///     Specifies which collection the newly constructed one will alias
        /// </param>
        /// <param name="_FDirectAlias">
        ///     Specifies whether the newly constructed collection is a direct alias
        /// </param>
        /**/
        _TaskCollection(_TaskCollection * _POriginCollection, bool _FDirectAlias);

        /// <summary>
        ///     Returns the local alias of a task collection on the current context.
        /// </summary>
        /**/
        _TaskCollection * _Alias();

        /// <summary>
        ///     Internal routine to abort work on the task collection.
        /// </summary>
        /// <param name="fLeaveCanceled">
        ///     An indication as to whether or not to leave the task collection canceled after the abort.
        /// </param>
        /**/
        void _Abort(bool fLeaveCanceled = false);

        /// <summary>
        ///     Returns whether the task collection is an alias.
        /// </summary>
        /**/
        bool _IsAlias() const;

        /// <summary>
        ///     Returns whether the task collection is an indirect alias.
        /// </summary>
        /**/
        bool _IsIndirectAlias() const;

        /// <summary>
        ///     Returns whether the task collection is a direct alias.
        /// </summary>
        /**/
        bool _IsDirectAlias() const;

        /// <summary>
        ///     Returns whether this task collection has a direct alias.
        /// </summary>
        /**/
        bool _HasDirectAlias() const;

        /// <summary>
        ///     Cancels work on the task collection.  This is an internal version.
        /// </summary>
        /// <param name="_InsideException">
        ///     Indicates whether the cancellation is taking place due to
        ///     exception unwinding within the runtime
        /// </param>
        /// <param name="_PSnapPoint">
        ///     A snapshot of the direct alias list which is what the call will effect
        /// </param>
        /**/
        void _Cancel(bool _InsideException, _TaskCollection * _PSnapPoint);

        /// <summary>
        ///     Called for every new chore put into the task collection.  Assures appropriate synchronization with waiters.
        /// </summary>
        /**/
        void _NotifyNewChore();

        /// <summary>
        ///     Called for every completed chore from the task collection.  Assures appropriate synchronization with waiters.
        /// </summary>
        /**/
        void _NotifyCompletedChore();

        /// <summary>
        ///     Waits on the given task collection and every alias.
        /// </summary>
        /// <param name="_PSnapPoint">
        ///     A snapshot of the direct alias list which is what the call will effect
        /// </param>
        /**/
        void _FullAliasWait(_TaskCollection * _PSnapPoint);

        /// <summary>
        ///     Resets the task collection for future usage.
        /// </summary>
        /// <param name="_PSnapPoint">
        ///     A snapshot of the direct alias list which is what the call will effect
        /// </param>
        /**/
        void _Reset(_TaskCollection * _PSnapPoint);

        /// <summary>
        ///     Called when an exception is raised on a chore on an unstructured task collection, this makes a determination of what to do with the exception
        ///     and squirrels it away for potential transport back to the thread performing a join on a task collection.  This specifically handles situations
        ///     on for unstructured task collections before calling _TaskCollectionBase::_RaisedException.
        /// </summary>
        /**/
        void _RaisedException();

        /// <summary>
        ///     Called in order to set the cancellation status of the collection.
        /// </summary>
        /// <param name="_Status">
        ///     The cancellation status to set
        /// </param>
        /// <param name="_DisallowIfNotClear">
        ///     Indicates whether the set should be disallowed if there is previous cancellation state
        /// </param>
        /// <returns>
        ///     An indication of whether the set succeeded
        /// </returns>
        /**/
        bool _SetCancelState(long _Status, bool _DisallowIfNotClear);

        /// <summary>
        ///     Called to cancel a single alias of a task collection from an arbitrary thread.
        /// </summary>
        /// <param name="_InsideException">
        ///     Indicates whether the cancellation is taking place due to
        ///     exception unwinding within the runtime
        /// </param>
        /**/
        void _CancelFromArbitraryThread(bool _InsideException);

        /// <summary>
        ///     Cancels all direct aliases of the task collection.
        /// </summary>
        /// <param name="_InsideException">
        ///     Indicates whether the cancellation is taking place due to
        ///     exception unwinding within the runtime
        /// </param>
        /// <param name="_PSnapPoint">
        ///     A snapshot of the direct alias list which is what the call will effect
        /// </param>
        /**/
        void _CancelDirectAliases(bool _InsideException, _TaskCollection * _PSnapPoint);

        /// <summary>
        ///     Called to cancel any contexts which stole chores from the given collection.  This is *PART* of a cancellation
        ///     scheme.  The remainder must be handled by the derived class in particular.  This should be called last.
        /// </summary>
        /// <param name="_InsideException">
        ///     Indicates whether the cancellation is taking place due to
        ///     exception unwinding within the runtime
        /// </param>
        /// <param name="_FInlineGated">
        ///     Indicates whether the inline context is safe and blocked from becoming inaccessible during
        ///     the duration of the call
        /// </param>
        /**/
        void _CancelStolenContexts(bool _InsideException, bool _FInlineGated);

        /// <summary>
        ///     Returns the steal tracking list.
        /// </summary>
        /**/
        void *_GetStealTrackingList() const;

        /// <summary>
        ///     Internal initialization of the task collection
        /// </summary>
        /**/
        void _Initialize();

        /// <summary>
        ///     Performs an abortive sweep of the WSQ for inline stack overflow.
        /// </summary>
        /// <param name="_PCtx">
        ///     The context to sweep
        /// </param>
        /**/
        void _AbortiveSweep(void *_PCtx);

        /// <summary>
        ///     A predicate function checking whether a given chore belongs to a given collection.
        /// </summary>
        /// <param name="_PChore">
        ///     The chore to check
        /// </param>
        /// <param name="_PData">
        ///     The data to check against
        /// </param>
        /// <returns>
        ///     Whether or not the chore belongs to the collection
        /// </returns>
        /**/
        static bool __cdecl _CollectionMatchPredicate(_UnrealizedChore *_PChore, void *_PData);

        /// <summary>
        ///     Called to sweep an aborted chore in the case of inline stack overflow.
        /// </summary>
        /// <param name="_PChore">
        ///     The chore to sweep
        /// </param>
        /// <param name="_PData">
        ///     The data that was passed to the sweep predicate
        /// </param>
        /// <returns>
        ///     An indication of whether the chore is now gone
        /// </returns>
        /**/
        static bool __cdecl _SweepAbortedChore(_UnrealizedChore *_PChore, void *_PData);

        /// <summary>
        ///     Performs task cleanup normally done at destruction time.
        /// </summary>
        /// <param name="fExceptional">
        ///     An indication if the cleanup is exceptional and the collection should be left in a canceled state.
        /// </param>
        /**/
        bool _TaskCleanup(bool fExceptional);

        /// <summary>
        ///     Tracks contexts that have stolen chores from this collection.  This is storage for an internal list and lock.  Note that this list is only
        ///     used for detached schedule groups.
        /// </summary>
        /**/
        _CONCRT_BUFFER _M_stealTracker[(3 * sizeof(void *) + 2 * sizeof(long) + sizeof(_CONCRT_BUFFER) - 1) / sizeof(_CONCRT_BUFFER)];

        /// <summary>
        ///     A count of active stealers for *CANCELLATION PURPOSES ONLY*.  This is non-interlocked and guarded by the same lock as the
        ///     stealers list on this task collection.
        /// </summary>
        /**/
        long _M_activeStealersForCancellation;

        /// <summary>
        ///     An indication of the exit code of the chore.  Anything non-zero here indicates cancellation of one
        ///     form or another.
        /// </summary>
        /**/
        volatile long _M_exitCode;

        /// <summary>
        ///     The status of the task collection.
        /// </summary>
        /**/
        volatile long _M_executionStatus;

        /// <summary>
        ///     An event on which to wait for stolen chores to complete.
        /// </summary>
        /**/
        event _M_event;

        _TaskCollection * _M_pOriginalCollection;
        _TaskCollection * _M_pNextAlias;
        void * _M_pTaskExtension;

        int _M_taskCookies[2];

        volatile long _M_flags;
        volatile long _M_chaining;

        DWORD _M_boundQueueId;
        int _M_stackPos;
    };

    //
    // Internal stub class.
    //
    class _TimerStub;

    //
    // Internal wrapper around timers in order to allow timer messaging blocks to share implementation with internal ConcRT runtime
    // timers.
    //
    class _Timer
    {
    protected:
        // Constructs a new timer.
        //
        // _Ms: The duration and period of the timer in milliseconds.
        // _FRepeating: An indication of whether the timer is repeating (periodic) or not.
        _CRTIMP _Timer(unsigned int _Ms, bool _FRepeating);

        // Destroys the timer.
        _CRTIMP ~_Timer();

        // Starts the timer.
        _CRTIMP void _Start();

        // Stops the timer.
        _CRTIMP void _Stop();

    private:
        friend class _TimerStub;

        // Called when the timer fires.
        virtual void _Fire() =0;

        // The actual timer
        HANDLE _M_hTimer;

        // The duration and period of the timer.
        unsigned int _M_ms;

        // Whether the timer is repeating (periodic by _M_ms)
        bool _M_fRepeating;
    };
} // namespace details


/// <summary>
///     Enables tracing in the Concurrency Runtime.
/// </summary>
/// <returns>
///     If tracing was correctly initiated, <c>S_OK</c> is returned; otherwise, <c>E_NOT_STARTED</c> is returned.
/// </returns>
/**/
_CRTIMP HRESULT __cdecl EnableTracing();

/// <summary>
///     Disables tracing in the Concurrency Runtime.
/// </summary>
/// <returns>
///     If tracing was correctly disabled, <c>S_OK</c> is returned.  If tracing was not previously initiated,
///     <c>E_NOT_STARTED</c> is returned
/// </returns>
/**/
_CRTIMP HRESULT __cdecl DisableTracing();

/// <summary>
///     The types of events that can be traced using the tracing functionality offered by the Concurrency Runtime.
/// </summary>
/**/
enum ConcRT_EventType
{
    /// <summary>
    ///     An event type used for miscellaneous events.
    /// </summary>
    /**/
    CONCRT_EVENT_GENERIC    = 0,
    /// <summary>
    ///     An event type that marks the beginning of a start/end event pair.
    /// </summary>
    /**/
    CONCRT_EVENT_START      = 1,
    /// <summary>
    ///     An event type that marks the beginning of a start/end event pair.
    /// </summary>
    /**/
    CONCRT_EVENT_END        = 2,
    /// <summary>
    ///     An event type that represents the act of a context blocking.
    /// </summary>
    /**/
    CONCRT_EVENT_BLOCK      = 3,
    /// <summary>
    ///     An event type that represents the act of unblocking a context.
    /// </summary>
    /**/
    CONCRT_EVENT_UNBLOCK    = 4,
    /// <summary>
    ///     An event type that represents the act of a context yielding.
    /// </summary>
    /**/
    CONCRT_EVENT_YIELD      = 5,
    /// <summary>
    ///     An event type that represents the act of a context becoming idle.
    /// </summary>
    /**/
    CONCRT_EVENT_IDLE       = 6,
    /// <summary>
    ///     An event type that represents the act of a attaching to a scheduler.
    /// </summary>
    /**/
    CONCRT_EVENT_ATTACH     = 7,
    /// <summary>
    ///     An event type that represents the act of a detaching from a scheduler.
    /// </summary>
    /**/
    CONCRT_EVENT_DETACH     = 8,
};

// Common trace header structure for all ConcRT diagnostic events
//      struct CONCRT_TRACE_EVENT_HEADER_COMMON
//      {
//          EVENT_TRACE_HEADER header;
//          DWORD VirtualProcessorID;
//          DWORD SchedulerID;
//          DWORD ContextID;
//          DWORD ScheduleGroupID;
//      };

/// <summary>
///     The ETW provider GUID for the Concurrency Runtime.
/// </summary>
/**/
extern "C" const __declspec(selectany) GUID ConcRT_ProviderGuid = { 0xF7B697A3, 0x4DB5, 0x4d3b, { 0xBE, 0x71, 0xC4, 0xD2, 0x84, 0xE6, 0x59, 0x2F } };

//
// GUIDS for events
//

/// <summary>
///     A category GUID describing ETW events fired by the Concurrency Runtime that are not more specifically described by another category.
/// </summary>
/// <remarks>
///     This category of events is not currently fired by the Concurrency Runtime.
/// </remarks>
/**/
extern "C" const __declspec(selectany) GUID ConcRTEventGuid = { 0x72B14A7D, 0x704C, 0x423e, { 0x92, 0xF8, 0x7E, 0x6D, 0x64, 0xBC, 0xB9, 0x2A } };

/// <summary>
///     A category GUID describing ETW events fired by the Concurrency Runtime that are directly related to scheduler activity.
/// </summary>
/// <seealso cref="CurrentScheduler Class"/>
/// <seealso cref="Scheduler Class"/>
/**/
extern "C" const __declspec(selectany) GUID SchedulerEventGuid = { 0xE2091F8A, 0x1E0A, 0x4731, { 0x84, 0xA2, 0x0D, 0xD5, 0x7C, 0x8A, 0x52, 0x61 } };

/// <summary>
///     A category GUID describing ETW events fired by the Concurrency Runtime that are directly related to schedule groups.
/// </summary>
/// <remarks>
///     This category of events is not currently fired by the Concurrency Runtime.
/// </remarks>
/// <seealso cref="ScheduleGroup Class"/>
/**/
extern "C" const __declspec(selectany) GUID ScheduleGroupEventGuid = { 0xE8A3BF1F, 0xA86B, 0x4390, { 0x9C, 0x60, 0x53, 0x90, 0xB9, 0x69, 0xD2, 0x2C } };

/// <summary>
///     A category GUID describing ETW events fired by the Concurrency Runtime that are directly related to contexts.
/// </summary>
/// <seealso cref="Context Class"/>
/**/
extern "C" const __declspec(selectany) GUID ContextEventGuid = { 0x5727A00F, 0x50BE, 0x4519, { 0x82, 0x56, 0xF7, 0x69, 0x98, 0x71, 0xFE, 0xCB } };

/// <summary>
///     A category GUID describing ETW events fired by the Concurrency Runtime that are directly related to chores or tasks.
/// </summary>
/// <remarks>
///     This category of events is not currently fired by the Concurrency Runtime.
/// </remarks>
/// <seealso cref="task_group Class"/>
/// <seealso cref="structured_task_group Class"/>
/**/
extern "C" const __declspec(selectany) GUID ChoreEventGuid = { 0x7E854EC7, 0xCDC4, 0x405a, { 0xB5, 0xB2, 0xAA, 0xF7, 0xC9, 0xE7, 0xD4, 0x0C } };

/// <summary>
///     A category GUID describing ETW events fired by the Concurrency Runtime that are directly related to virtual processors.
/// </summary>
/**/
extern "C" const __declspec(selectany) GUID VirtualProcessorEventGuid = { 0x2f27805f, 0x1676, 0x4ecc, { 0x96, 0xfa, 0x7e, 0xb0, 0x9d, 0x44, 0x30, 0x2f } };

/// <summary>
///     A category GUID describing ETW events fired by the Concurrency Runtime that are directly related to locks.
/// </summary>
/// <remarks>
///     This category of events is not currently fired by the Concurrency Runtime.
/// </remarks>
/// <seealso cref="critical_section Class"/>
/// <seealso cref="reader_writer_lock Class"/>
/**/
extern "C" const __declspec(selectany) GUID LockEventGuid = { 0x79A60DC6, 0x5FC8, 0x4952, { 0xA4, 0x1C, 0x11, 0x63, 0xAE, 0xEC, 0x5E, 0xB8 } };

/// <summary>
///     A category GUID describing ETW events fired by the Concurrency Runtime that are directly related to the resource manager.
/// </summary>
/// <remarks>
///     This category of events is not currently fired by the Concurrency Runtime.
/// </remarks>
/// <seealso cref="IResourceManager Structure"/>
/**/
extern "C" const __declspec(selectany) GUID ResourceManagerEventGuid = { 0x2718D25B, 0x5BF5, 0x4479, { 0x8E, 0x88, 0xBA, 0xBC, 0x64, 0xBD, 0xBF, 0xCA } };

/// <summary>
///     A category GUID describing ETW events fired by the Concurrency Runtime that are directly related to usage of the <c>parallel_invoke</c>
///     function.
/// </summary>
/// <seealso cref="parallel_invoke Function"/>
/**/
extern "C" const __declspec(selectany) GUID PPLParallelInvokeEventGuid = { 0xd1b5b133, 0xec3d, 0x49f4, { 0x98, 0xa3, 0x46, 0x4d, 0x1a, 0x9e, 0x46, 0x82 } };

/// <summary>
///     A category GUID describing ETW events fired by the Concurrency Runtime that are directly related to usage of the <c>parallel_for</c>
///     function.
/// </summary>
/// <seealso cref="parallel_for Function"/>
/**/
extern "C" const __declspec(selectany) GUID PPLParallelForEventGuid = { 0x31c8da6b, 0x6165, 0x4042, { 0x8b, 0x92, 0x94, 0x9e, 0x31, 0x5f, 0x4d, 0x84 } };

/// <summary>
///     A category GUID describing ETW events fired by the Concurrency Runtime that are directly related to usage of the <c>parallel_for_each</c>
///     function.
/// </summary>
/// <seealso cref="parallel_for_each Function"/>
/**/
extern "C" const __declspec(selectany) GUID PPLParallelForeachEventGuid = { 0x5cb7d785, 0x9d66, 0x465d, { 0xba, 0xe1, 0x46, 0x11, 0x6, 0x1b, 0x54, 0x34 } };

// Trace an event signaling a parallel function
_CRTIMP void __cdecl _Trace_ppl_function(const GUID& _Guid, unsigned char _Level, ConcRT_EventType _Type);

}

#pragma pack(pop)
