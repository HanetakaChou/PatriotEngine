// ==++==
//
//	Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// Trace.h
//
// Header file containing internal declarations for event tracing infrastructure.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#pragma once

namespace Concurrency
{
namespace details
{
    // Singleton ETW wrapper to avoid having a static dependency on advapi32.dll.
    class Etw
    {
    private:
        Etw() throw();

    public:
        static void EnsureSingleton();

    public:
        ULONG RegisterGuids(WMIDPREQUEST controlCallBack, LPCGUID providerGuid, ULONG guidCount, PTRACE_GUID_REGISTRATION eventGuidRegistration, PTRACEHANDLE providerHandle);
        ULONG UnregisterGuids(TRACEHANDLE handle);
        ULONG Trace(TRACEHANDLE handle, PEVENT_TRACE_HEADER eventHeader);
        TRACEHANDLE GetLoggerHandle(PVOID);
        UCHAR GetEnableLevel(TRACEHANDLE handle);
        ULONG GetEnableFlags(TRACEHANDLE handle);

    private:
        typedef ULONG WINAPI FnRegisterTraceGuidsW(WMIDPREQUEST, PVOID, LPCGUID, ULONG, PTRACE_GUID_REGISTRATION, LPCWSTR, LPCWSTR, PTRACEHANDLE);
        FnRegisterTraceGuidsW* m_pfnRegisterTraceGuidsW;

        typedef ULONG WINAPI FnUnregisterTraceGuids(TRACEHANDLE);
        FnUnregisterTraceGuids* m_pfnUnregisterTraceGuids;

        typedef ULONG WINAPI FnTraceEvent(TRACEHANDLE, PEVENT_TRACE_HEADER);
        FnTraceEvent* m_pfnTraceEvent;

        typedef TRACEHANDLE WINAPI FnGetTraceLoggerHandle(PVOID);
        FnGetTraceLoggerHandle* m_pfnGetTraceLoggerHandle;

        typedef UCHAR WINAPI FnGetTraceEnableLevel(TRACEHANDLE);
        FnGetTraceEnableLevel* m_pfnGetTraceEnableLevel;

        typedef ULONG WINAPI FnGetTraceEnableFlags(TRACEHANDLE);
        FnGetTraceEnableFlags* m_pfnGetTraceEnableFlags;

        static _StaticLock s_lock;
        static Etw* s_singleton;
    };

    extern bool g_TraceOn;
    extern UCHAR g_EnableLevel;
    extern TRACEHANDLE g_ConcRTSessionHandle;


    extern Etw* g_pEtw;
} // namespace details

/// <summary>
///     Common trace header structure for all ConcRT diagnostic events
/// </summary>
struct CONCRT_TRACE_EVENT_HEADER_COMMON
{
    EVENT_TRACE_HEADER header;
    DWORD VirtualProcessorID;
    DWORD SchedulerID;
    DWORD ContextID;
    DWORD ScheduleGroupID;
};
}
