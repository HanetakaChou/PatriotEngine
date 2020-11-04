// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
// 
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// utils.cpp
//
// Utility routines for use in ConcRT.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

extern "C" IMAGE_DOS_HEADER __ImageBase;

#if _UMS_ADVANCED_DEBUG

//**************************************************
//
// Global operator overloads for UMS debugging
//
//**************************************************

void ValidateHeapCall()
{
    if (UMS::Initialized())
    {
        UMSThreadProxy *pProxy = UMSThreadProxy::GetCurrent();

        //
        // Once a thread is transmogrified, it behaves as a truly external thread -- all validations are performed as such.
        //
        if (pProxy != NULL && !pProxy->IsTransmogrified())
        {
            CriticalRegionType type = pProxy->GetCriticalRegionType();

            //
            // Rules are more relaxed during shutdown when we're not responsible for scheduling anything.  We need to be in a CR for various
            // reasons, but heap allocations there won't hose us since there's nothing running.
            //
            bool validations = pProxy->IsShutdownValidations();

            if (!validations)
            {
                ContextBase *pContext = SchedulerBase::FastCurrentContext();
                if (pContext != NULL)
                    validations = pContext->IsShutdownValidations();
            }

            CORE_ASSERT(type != InsideHyperCriticalRegion || validations);
        }
    }
}

void *operator new(size_t size)
{
    ValidateHeapCall();
    return malloc(size);
}

void operator delete(void *pObj)
{
    ValidateHeapCall();
    free(pObj);
}

void *operator new[](size_t size)
{
    ValidateHeapCall();
    return malloc(size);
}

void operator delete[](void *pObj)
{
    ValidateHeapCall();
    free(pObj);
}


#endif // _UMS_ADVANCED_DEBUG

namespace Concurrency
{
namespace details
{

    /// <summary>
    ///     Default method for yielding during a spin wait
    /// </summary>
    void __cdecl _UnderlyingYield()
    {
        // Performs a yield appropriate to the scheduler.
        ContextBase *pCurrentContext = SchedulerBase::SafeFastCurrentContext();
        if (pCurrentContext != NULL)
        {
            pCurrentContext->SpinYield();
        }
        else
        {
            SwitchToThread();
        }
    }

    /// <summary>
    ///     Use Sleep(0) to do the yield.
    /// </summary>
    void __cdecl _Sleep0()
    {
        Sleep(0);
    }

    static bool g_TraceInitialized = false;
    static int g_DesiredTraceLevel = 0;
    static FILE* g_DebugOutFilePtr = stderr;
    static int g_CommitFrequency = 50;
    static unsigned __int64 g_TraceCount = 0;

    void _HyperNonReentrantLock::_Acquire()
    {
        ContextBase::StaticEnterHyperCriticalRegion();
        m_lock._Acquire();
    }

    bool _HyperNonReentrantLock::_TryAcquire()
    {
        ContextBase::StaticEnterHyperCriticalRegion();
        bool lockAcquired = m_lock._TryAcquire();
        if (!lockAcquired)
            ContextBase::StaticExitHyperCriticalRegion();
        return lockAcquired;
    }

    void _HyperNonReentrantLock::_Release()
    {
        m_lock._Release();
        ContextBase::StaticExitHyperCriticalRegion();
    }

    void _CriticalNonReentrantLock::_Acquire()
    {
        ContextBase::StaticEnterCriticalRegion();
        m_lock._Acquire();
    }

    bool _CriticalNonReentrantLock::_TryAcquire()
    {
        ContextBase::StaticEnterCriticalRegion();
        bool lockAcquired = m_lock._TryAcquire();
        if (!lockAcquired)
            ContextBase::StaticExitCriticalRegion();
        return lockAcquired;
    }

    void _CriticalNonReentrantLock::_Release()
    {
        m_lock._Release();
        ContextBase::StaticExitCriticalRegion();
    }

#if defined(CONCRT_TRACING)
    static bool
    IsWhitespace(
        wchar_t wch
        )
    {
        wchar_t buf[2];
        buf[0] = wch;
        buf[1] = 0;
        WORD type;
#pragma warning(disable: 25028)  // use of function or parameters passed to function 'GetStringTypeExW' need to be reviewed -- it has been reviewed
        if (!GetStringTypeExW(LOCALE_USER_DEFAULT, CT_CTYPE1, buf, 1, &type)) {
            return false;
        }
#pragma warning(default: 25028)
        return (type & C1_SPACE) != 0;
    }

    static wchar_t *
    SkipWhitespace(
        __out_ecount(count) wchar_t * ptr,
        int count
        )
    {
        int i = 0;
        for (; i < count && ptr[i] != 0 && iswspace(ptr[i]); ++i)
            ;
        return &ptr[i];
    }
#endif

    template<typename SignedInt, typename UnsignedInt, UnsignedInt ui_max>
    static const wchar_t *
    StringToInt(
        const wchar_t * ptr,
        SignedInt * pvalue
        )
    {
        ASSERT(*ptr != L'\0');

        bool negative;
        if (*ptr == L'-') {
            negative = true;
            ++ptr;
        }
        else {
            negative = false;
            if (*ptr == L'+') {
                ++ptr;
            }
        }

        if (*ptr < L'0' || *ptr > L'9') {
            // No number present (possibly just a sign)
            return NULL;
        }

        UnsignedInt absval = 0;
        do {
            int digit = *ptr - L'0';
            if (absval > ui_max / 10 ||
                (absval == ui_max / 10 &&
                 digit > ui_max % 10))
            {
                // Unsigned overflow
                return NULL;
            }
            absval = absval * 10 + digit;
            ++ptr;
        } while (*ptr >= L'0' && *ptr <= L'9');

        SignedInt result = static_cast<SignedInt>(absval);
        if (negative) {
            result = -result;
            if (result > 0) {
                // Signed underflow
                return NULL;
            }
        }
        else {
            if (result < 0) {
                // Signed overflow
                return NULL;
            }
        }

        *pvalue = result;
        return ptr;
    }

#if defined(CONCRT_TRACING)
    static wchar_t * ReadEnvVar(const wchar_t * name, __out_ecount(maxlen) wchar_t * buffer, DWORD maxlen)
    {
        DWORD len = GetEnvironmentVariableW(name, buffer, maxlen);

        if (len == 0) {
            return NULL;
        }

        if (len >= MAX_PATH) {
            // name too long, just ignore it
            return NULL;
        }

        wchar_t * ptr = SkipWhitespace(buffer, maxlen);
        if (*ptr == L'\0') {
            return NULL;
        }

        wchar_t * endptr = ptr + lstrlenW(ptr);
        ASSERT(*endptr == L'\0');
        while (IsWhitespace(endptr[-1])) {
            --endptr;
        }
        *endptr = L'\0';

        ASSERT(*ptr != L'\0' && !IsWhitespace(*ptr));

        return ptr;
    }

    static int ProcessTraceValue(const wchar_t * ptr)
    {
        int value;
        ptr = StringToInt<int, unsigned int, UINT_MAX>(ptr, &value);
        if (ptr == NULL || *ptr != L'\0') {
            return 0;
        }
        return value;
    }
#endif

    void InitializeUtilityRoutines()
    {
#if defined(CONCRT_TRACING)
        if (g_TraceInitialized)
        {
            return;
        }
        else
        {
            g_TraceInitialized = true;
        }

        wchar_t buffer[MAX_PATH+1];
        // bits to match TRACE arenas
        const wchar_t * ptr = ReadEnvVar(L"CONCRT_TRACE", buffer, DIM(buffer)); 
        if (ptr != NULL)
            g_DesiredTraceLevel = ProcessTraceValue(buffer);
        // when > 0, fflush every g_CommitFrequency TRACE statements, when <= 0 no flush
        ptr = ReadEnvVar(L"CONCRT_COMMIT_FREQUENCY", buffer, DIM(buffer)); 
        if (ptr != NULL)
            g_CommitFrequency = ProcessTraceValue(buffer);
        else 
            g_CommitFrequency = 1;
        // when not set will go to debug output, else stdout, stderr or filename
        const wchar_t* pwszOutputFile = ReadEnvVar(L"CONCRT_TRACE_OUTPUT", buffer, DIM(buffer)); 
        if (pwszOutputFile != NULL)
        {
            if (wcsncmp(L"stdout", pwszOutputFile, MAX_PATH) == 0)
                g_DebugOutFilePtr = stdout;
            else if (wcsncmp(L"stderr", pwszOutputFile, MAX_PATH) == 0)
            {
                g_DebugOutFilePtr = stderr;
                g_CommitFrequency = 0;
            }
            else 
            {
                buffer[MAX_PATH] = 0;
                errno_t errNo = 0;
                if ((errNo = _wfopen_s(&g_DebugOutFilePtr, pwszOutputFile, L"w+tc")) != 0)
                {
                    if (swprintf_s(buffer, MAX_PATH, L"Cannot open trace output: %S, errno=%d\n", pwszOutputFile, errNo) < 0)
                        // bad state -- try again w/o string
                        fprintf(stderr, "Cannot open trace output: errno=%d\n", errNo);
                    else
                    {
                        OutputDebugStringW(buffer);
                        fprintf(stderr, "Cannot open trace output: %S, errno=%d\n", pwszOutputFile, errNo);
                    }
                    // default to stderr
                    g_DebugOutFilePtr = stderr;
                }
            }
        }
#endif
    }

    void _ConcRT_Assert(const char *value, const char* filename, int lineno)
    {
        const int size = 1024;
        wchar_t buffer[size+1];
        const ContextBase * pContext = SchedulerBase::SafeFastCurrentContext();

        //
        // If we're on the primary we don't want to execute any blocking APIs
        //
        if (UMSSchedulingContext::OnPrimary())
        {
            DebugBreak();
            return;
        }

        if (pContext != NULL && pContext->GetScheduler() != NULL) {
            swprintf_s(buffer, size, L"[%d:%d:%d:%d(%d)] %S: !!!!!!!Assert Failed(%S: %d)\n",
                pContext->GetVirtualProcessorId(),
                pContext->GetId(),
                pContext->GetScheduleGroupId(),
                pContext->ScheduleGroupRefCount(),
                GetCurrentThreadId(),
                value,
                filename,
                lineno);
        }
        else
            swprintf_s(buffer, size, L"[%d] %S: !!!!!!!Assert Failed(%S: %d)\n",
                GetCurrentThreadId(),
                value,
                filename,
                lineno);

        buffer[1024] = 0;

        fwprintf(g_DebugOutFilePtr, buffer);
        fflush(g_DebugOutFilePtr);

        OutputDebugStringW(buffer);
        DebugBreak();
        exit(-8);
    }

    void _ConcRT_CoreAssert(const char *value, const char* filename, int lineno)
    {
        //
        // Nothing here can block in any way.
        //
        DebugBreak();
    }

    template <size_t size>
    void 
    ConcRT_FillBuffer(
        wchar_t (&buffer)[size],
        const wchar_t * format,
        va_list args
        )
    {
        // Format the prefix giving the current context, thread, and vproc IDs
        int lenPrefix = 0;
        ContextBase * pContext = SchedulerBase::SafeFastCurrentContext();
        if (pContext != NULL && pContext->GetScheduler() != NULL) {
            lenPrefix = swprintf_s(buffer, size, L"[%d:%d:%d:%d(%d)] ",
                                   pContext->GetVirtualProcessorId(),
                                   pContext->GetId(),
                                   pContext->GetScheduleGroupId(),
                                   pContext->ScheduleGroupRefCount(),
                                   GetCurrentThreadId());
            if (lenPrefix < 0) {
                // Error in swprintf_s, don't bother with a prefix
                lenPrefix = 0;
            }
        } 

        // Format the trace message
        vswprintf_s(buffer + lenPrefix,
                    DIM(buffer) - lenPrefix,
                    format, args);

        // Append the trailing newline if missing
        int lenBuffer = static_cast<int>(wcslen(buffer));
        if (lenBuffer > 0 && buffer[lenBuffer - 1] != L'\n')
        {
            if (lenBuffer < DIM(buffer) - 1)
            {
                buffer[lenBuffer] = L'\n';
                buffer[lenBuffer + 1] = L'\0';
            }
            else
            {
                buffer[lenBuffer - 1] = L'\n';
            }
        }
    }

    void
    _ConcRT_DumpMessage(
        const wchar_t * format,
        ...
        )
    {
        wchar_t buffer[1024+1];
        va_list args;
        va_start(args, format);
        ConcRT_FillBuffer(buffer, format, args);
        va_end(args);
        buffer[1024] = 0;
        OutputDebugStringW(buffer);
        fwprintf(stderr, buffer);
    }

    // Trace -- Used for tracing and debugging
    void
    _ConcRT_Trace(
        int trace_level,
        const wchar_t * format,
        ...
        )
    {
        InitializeUtilityRoutines();

        // Check if tracing is disabled
        if ((g_DesiredTraceLevel & trace_level) == 0) {
            return;
        }

        wchar_t buffer[1024+1];

        va_list args;
        va_start(args, format);
        ConcRT_FillBuffer(buffer, format, args);
        va_end(args);

        buffer[1024] = 0;

        if (g_DebugOutFilePtr != NULL)
        {
            fwprintf(g_DebugOutFilePtr, buffer);
            if (g_CommitFrequency > 0 && (g_TraceCount++ % g_CommitFrequency) == 0)
                fflush(g_DebugOutFilePtr);
        }
        else
        {
            OutputDebugStringW(buffer);
        }
    }

#if _UMSTRACE
    void _ConcRT_VMTrace(int traceEvt, void *pCtx, void *pVproc, ULONG_PTR data)
    {
        if (pVproc != NULL)
            reinterpret_cast<VirtualProcessor *>(pVproc)->Trace(traceEvt, pCtx, pVproc, data);
    }

    void _ConcRT_CMTrace(int traceEvt, void *pCtx, void *pVproc, ULONG_PTR data)
    {
        if (pCtx != NULL)
            reinterpret_cast<InternalContextBase *>(pCtx)->Trace(traceEvt, pCtx, pVproc, data);
    }

    void _ConcRT_RVMTrace(int traceEvt, void *pCtx, void *pVproc, ULONG_PTR data)
    {
        if (pVproc != NULL)
            reinterpret_cast<UMSFreeVirtualProcessorRoot *>(pVproc)->Trace(traceEvt, pCtx, pVproc, data);
    }

    void _ConcRT_RPMTrace(int traceEvt, void *pCtx, void *pVproc, ULONG_PTR data)
    {
        if (pCtx != NULL)
            reinterpret_cast<UMSThreadProxy *>(pCtx)->Trace(traceEvt, pCtx, pVproc, data);
    }
#endif // _UMSTRACE

    //
    // _SpinLock
    //
    _CRTIMP _SpinLock::_SpinLock(volatile long& flag)
        : _M_flag(flag)
    {
        if ( _InterlockedCompareExchange(&_M_flag, 1, 0) != 0 )
        {
            _SpinWaitBackoffNone spinWait;
            do 
            {
                spinWait._SpinOnce();
            } while ( _InterlockedCompareExchange(&_M_flag, 1, 0) != 0 );
        }
    }

    _CRTIMP _SpinLock::~_SpinLock()
    {
        _InterlockedExchange(&_M_flag, 0);
    }



    _CRTIMP unsigned long Log2(size_t n) {
        unsigned long r;
#if defined(_M_X64)
        _BitScanReverse64(&r, n);
#else
        _BitScanReverse(&r, n);
#endif
        return r;
    }


    // Globals used for ConcRT shutdown
    volatile LONG LoadLibraryCount = 0;
    HMODULE HostModule = NULL;

    //
    // Adds a reference on a DLL where ConcRT is hosted, if it is a DLL, otherwise does nothing.
    // This is used to shutdown ConcRT on our own terms and not be forced into a difficult synchronous
    // shutdown by user's call to FreeLibrary. Note that this does not matter if a process shutdown
    // happens because all threads and ConcRT along with them are rudely terminated by the OS.
    //
    void ReferenceLoadLibrary()
    {
        HMODULE currentModuleHandle = (HMODULE) &__ImageBase;
        HMODULE currentExeHandle = GetModuleHandle(NULL);

        // Do this only if ConcRT is hosted inside a DLL
        if (currentModuleHandle != currentExeHandle)
        {
            WCHAR wcDllPath[MAX_PATH];

            if (GetModuleFileNameW(currentModuleHandle, wcDllPath, MAX_PATH))
            {
                HostModule = LoadLibraryW(wcDllPath);
            }
            else
            {
                throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
            }
        }
    }

    //
    // Adds a reference to a host module and then creates the thread. First reference is managed by LoadLibrary,
    // and all subsequent ones are reference counted internally to avoid LoadLibrary call overhead.
    //
    HANDLE LoadLibraryAndCreateThread
    (
        LPSECURITY_ATTRIBUTES lpThreadAttributes,
        SIZE_T dwStackSize,
        LPTHREAD_START_ROUTINE lpStartAddress,
        LPVOID lpParameter,
        DWORD dwCreationFlags,
        LPDWORD lpThreadId
    )
    {
        // Make sure that library (DLL) is not unloaded while ConcRT threads are still running
        if (InterlockedIncrement(&LoadLibraryCount) == 1)
        {
            ReferenceLoadLibrary();
            SchedulerBase::ReferenceStaticOneShot();
        }

        HANDLE threadHandle = CreateThread(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId);

        // Make sure the background thread is properly created
        if (threadHandle == NULL)
        {
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
        }

        return threadHandle;
    }

    //
    // Removes a reference count on a host module and in the case of last reference frees the library.
    //
    void FreeLibraryAndDestroyThread(DWORD exitCode)
    {
        // If this is the last ConcRT thread leaving the process try to cleanup
        if (InterlockedDecrement(&LoadLibraryCount) == 0)
        {
            SchedulerBase::CheckOneShotStaticDestruction();

            // If this is a DLL release the last LoadLibrary reference
            if (HostModule != NULL)
            {
                FreeLibraryAndExitThread(HostModule, exitCode);
            }
        }
    }

// We will use the GS cookie as a starting point
extern "C" uintptr_t __security_cookie;

    //
    // Initializes the cookie used to encode global data
    //
    ULONG_PTR  Security::InitializeCookie()
    {
        CORE_ASSERT(Security::s_initialized == 0);
        Security::s_initialized = 1;

        // Take advantage of ASLR and per-process cookie
        ULONG_PTR cookie = (ULONG_PTR)::EncodePointer((PVOID)&Security::s_cookie);

        // security cookie should be initialized before us.
        cookie ^= (ULONG_PTR)__security_cookie;

        // Add other randomization factors such as the thread creation time.

        FILETIME creationTime;
        FILETIME notused;

        if (GetThreadTimes(GetCurrentThread(), &creationTime, &notused, &notused, &notused))
        {
#if defined(_WIN64)
            ULARGE_INTEGER ul;

            ul.LowPart = creationTime.dwLowDateTime;
            ul.HighPart = creationTime.dwHighDateTime;
            cookie ^= ul.QuadPart;
#else
            cookie ^= creationTime.dwLowDateTime;
            cookie ^= creationTime.dwHighDateTime;
#endif
        }

        return cookie;
    }

    //
    // Encode the given pointer value
    //
    PVOID Security::EncodePointer(PVOID ptr)
    {
        CORE_ASSERT(Security::s_initialized != 0);
        return (PVOID)((ULONG_PTR)(ptr) ^ Security::s_cookie);
    }

    //
    // Decode the given pointer value
    //
    PVOID Security::DecodePointer(PVOID ptr)
    {
        return EncodePointer(ptr);
    }

} // namespace details
} // namespace Concurrency

//
// ConcRT static cleanup:
//
extern "C"
void __cdecl _concrt_static_cleanup(void);

_CRTALLOC(".CRT$XPB") static _PVFV pterm = _concrt_static_cleanup;

extern "C"
void __cdecl _concrt_static_cleanup(void)
{
    // Cleanup the TLS unless inside an EXE
    if (HostModule != NULL)
    {
        SchedulerBase::CheckOneShotStaticDestruction();
    }
}
