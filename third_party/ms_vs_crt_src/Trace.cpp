// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
// 
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// Trace.cpp
//
// Implementation of ConcRT tracing API.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    TRACEHANDLE g_ConcRTPRoviderHandle;
    TRACEHANDLE g_ConcRTSessionHandle;
    ULONG g_EnableFlags;    // Determines which class of events to log
    UCHAR g_EnableLevel;    // Determines the serverity of errors to log
    bool g_TraceOn;         // Determines whether to log events
    volatile long g_enableCount;

    Etw* g_pEtw;

    void Etw::EnsureSingleton()
    {
        // Initialize ETW dynamically, and only once, to avoid a static dependency on Advapi32.dll.
        if (g_pEtw == NULL)
        {
            _StaticLock::_Scoped_lock lockHolder(s_lock);

            if (g_pEtw == NULL)
            {
                g_pEtw = new Etw();
            }
        }
    }

    Etw::Etw() throw()
    {
        HMODULE hAdvapi = LoadLibraryW(L"advapi32.dll");
        if (hAdvapi != NULL)
        {
            m_pfnRegisterTraceGuidsW = (FnRegisterTraceGuidsW*) Security::EncodePointer(GetProcAddress(hAdvapi, "RegisterTraceGuidsW"));
            m_pfnUnregisterTraceGuids = (FnUnregisterTraceGuids*) Security::EncodePointer(GetProcAddress(hAdvapi, "UnregisterTraceGuids"));
            m_pfnTraceEvent = (FnTraceEvent*) Security::EncodePointer(GetProcAddress(hAdvapi, "TraceEvent"));
            m_pfnGetTraceLoggerHandle = (FnGetTraceLoggerHandle*) Security::EncodePointer(GetProcAddress(hAdvapi, "GetTraceLoggerHandle"));
            m_pfnGetTraceEnableLevel = (FnGetTraceEnableLevel*) Security::EncodePointer(GetProcAddress(hAdvapi, "GetTraceEnableLevel"));
            m_pfnGetTraceEnableFlags = (FnGetTraceEnableFlags*) Security::EncodePointer(GetProcAddress(hAdvapi, "GetTraceEnableFlags"));
        }
    }

    ULONG Etw::RegisterGuids(WMIDPREQUEST controlCallBack, LPCGUID providerGuid, ULONG guidCount, PTRACE_GUID_REGISTRATION eventGuidRegistration, PTRACEHANDLE providerHandle)
    {
        if (m_pfnRegisterTraceGuidsW != _encoded_null())
        {
            FnRegisterTraceGuidsW* pfnRegisterTraceGuidsW = (FnRegisterTraceGuidsW*) Security::DecodePointer(m_pfnRegisterTraceGuidsW);
            return pfnRegisterTraceGuidsW(controlCallBack, NULL, providerGuid, guidCount, eventGuidRegistration, NULL, NULL, providerHandle);
        }

        return ERROR_PROC_NOT_FOUND;
    }

    ULONG Etw::UnregisterGuids(TRACEHANDLE handle)
    {
        if (m_pfnUnregisterTraceGuids != _encoded_null())
        {
            FnUnregisterTraceGuids* pfnUnregisterTraceGuids = (FnUnregisterTraceGuids*) Security::DecodePointer(m_pfnUnregisterTraceGuids);
            return pfnUnregisterTraceGuids(handle);
        }

        return ERROR_PROC_NOT_FOUND;
    }

    ULONG Etw::Trace(TRACEHANDLE handle, PEVENT_TRACE_HEADER eventHeader)
    {
        if (m_pfnTraceEvent != _encoded_null())
        {
            FnTraceEvent* pfnTraceEvent = (FnTraceEvent*) Security::DecodePointer(m_pfnTraceEvent);
            return pfnTraceEvent(handle, eventHeader);
        }

        return ERROR_PROC_NOT_FOUND;
    }

    TRACEHANDLE Etw::GetLoggerHandle(PVOID buffer)
    {
        if (m_pfnGetTraceLoggerHandle != _encoded_null())
        {
            FnGetTraceLoggerHandle* pfnGetTraceLoggerHandle = (FnGetTraceLoggerHandle*) Security::DecodePointer(m_pfnGetTraceLoggerHandle);
            return pfnGetTraceLoggerHandle(buffer);
        }

        SetLastError(ERROR_PROC_NOT_FOUND);
        return (TRACEHANDLE)INVALID_HANDLE_VALUE;
    }

    UCHAR Etw::GetEnableLevel(TRACEHANDLE handle)
    {
        if (m_pfnGetTraceEnableLevel != _encoded_null())
        {
            FnGetTraceEnableLevel* pfnGetTraceEnableLevel = (FnGetTraceEnableLevel*) Security::DecodePointer(m_pfnGetTraceEnableLevel);
            return pfnGetTraceEnableLevel(handle);
        }

        SetLastError(ERROR_PROC_NOT_FOUND);
        return 0;
    }

    ULONG Etw::GetEnableFlags(TRACEHANDLE handle)
    {
        if (m_pfnGetTraceEnableFlags != _encoded_null())
        {
            FnGetTraceEnableFlags* pfnGetTraceEnableFlags = (FnGetTraceEnableFlags*) Security::DecodePointer(m_pfnGetTraceEnableFlags);
            return pfnGetTraceEnableFlags(handle);
        }

        SetLastError(ERROR_PROC_NOT_FOUND);
        return 0;
    }


    /// <summary>WMI control call back</summary>
    ULONG WINAPI ControlCallback(WMIDPREQUESTCODE requestCode, void* requestContext, ULONG* reserved, void* buffer)
    {
        DWORD rc;

        switch (requestCode)
        {
        case WMI_ENABLE_EVENTS:     // Enable the provider
            g_ConcRTSessionHandle = g_pEtw->GetLoggerHandle(buffer);
            if ((HANDLE)g_ConcRTSessionHandle == INVALID_HANDLE_VALUE)
                return GetLastError();

            SetLastError(ERROR_SUCCESS);
            g_EnableLevel = g_pEtw->GetEnableLevel(g_ConcRTSessionHandle);
            if (g_EnableLevel == 0)
            {
                rc = GetLastError();
                if (rc == ERROR_SUCCESS)
                {
                    // Enable level of 0 means TRACE_LEVEL_INFORMATION
                    g_EnableLevel = TRACE_LEVEL_INFORMATION;
                }
                else
                {
                    return rc;
                }
            }

            g_EnableFlags = g_pEtw->GetEnableFlags(g_ConcRTSessionHandle);
            if (g_EnableFlags == 0)
            {
                rc = GetLastError();
                if (rc != ERROR_SUCCESS)
                    return rc;
            }

            // Tracing is now enabled.
            g_TraceOn = true;
            break;

        case WMI_DISABLE_EVENTS:    // Disable the provider
            g_TraceOn = false;
            g_ConcRTSessionHandle = 0;
            break;

        case WMI_EXECUTE_METHOD:
        case WMI_REGINFO:
        case WMI_DISABLE_COLLECTION:
        case WMI_ENABLE_COLLECTION:
        case WMI_SET_SINGLE_ITEM:
        case WMI_SET_SINGLE_INSTANCE:
        case WMI_GET_SINGLE_INSTANCE:
        case WMI_GET_ALL_DATA:
        default:
            return ERROR_INVALID_PARAMETER;
        }

        return ERROR_SUCCESS;
    }

    void PPL_Trace_Event(const GUID& guid, ConcRT_EventType eventType, UCHAR level)
    {
        if (g_pEtw != NULL)
        {
            CONCRT_TRACE_EVENT_HEADER_COMMON concrtHeader = {0};

            concrtHeader.header.Size = sizeof concrtHeader;
            concrtHeader.header.Flags = WNODE_FLAG_TRACED_GUID;
            concrtHeader.header.Guid = guid;
            concrtHeader.header.Class.Type = (UCHAR) eventType;
            concrtHeader.header.Class.Level = level;

            g_pEtw->Trace(g_ConcRTSessionHandle, &concrtHeader.header);
        }
    }
} // namespace details

/// <summary>
///     Enable tracing
/// </summary>
/// <returns>
///     If tracing was correctly initiated, S_OK is returned, otherwise E_NOT_STARTED is returned
/// </returns>
_CRTIMP HRESULT EnableTracing()
{
    if (InterlockedIncrement(&g_enableCount) == 1)
    {
        // Initialize ETW dynamically, and only once, to avoid a static dependency on Advapi32.dll.
        Etw::EnsureSingleton();

        static TRACE_GUID_REGISTRATION eventGuidRegistration[] = {
            { &Concurrency::ConcRTEventGuid, NULL },
            { &Concurrency::SchedulerEventGuid, NULL },
            { &Concurrency::ScheduleGroupEventGuid, NULL },
            { &Concurrency::ContextEventGuid, NULL },
            { &Concurrency::ChoreEventGuid, NULL },
            { &Concurrency::LockEventGuid, NULL },
            { &Concurrency::ResourceManagerEventGuid, NULL }
        };

        ULONG eventGuidCount = sizeof eventGuidRegistration / sizeof eventGuidRegistration[0];

        ULONG rc = g_pEtw->RegisterGuids(Concurrency::details::ControlCallback, &ConcRT_ProviderGuid, eventGuidCount, eventGuidRegistration, &g_ConcRTPRoviderHandle);
        if (rc != ERROR_SUCCESS)
            return HRESULT_FROM_WIN32(rc);
    }

    return S_OK;
}


/// <summary>
///     Disables tracing
/// </summary>
/// <returns>
///     If tracing was correctly disabled, S_OK is returned.  If tracing was not previously initiated,
///     E_NOT_STARTED is returned
/// </returns>
_CRTIMP HRESULT DisableTracing()
{
    if (InterlockedDecrement(&g_enableCount) == 0)
    {
        ULONG rc = g_pEtw->UnregisterGuids(g_ConcRTPRoviderHandle);
        if (rc != ERROR_SUCCESS)
            return HRESULT_FROM_WIN32(rc);
    }

    if (g_enableCount < 0)
        g_enableCount = 0;

    return S_OK;
}

// Trace an event signaling the begin of a PPL function
_CRTIMP void _Trace_ppl_function(const GUID& guid, UCHAR level, ConcRT_EventType type)
{
    if (g_TraceOn && level <= g_EnableLevel)
        Concurrency::details::PPL_Trace_Event(guid, type, level);
}

} // namespace Concurrency
