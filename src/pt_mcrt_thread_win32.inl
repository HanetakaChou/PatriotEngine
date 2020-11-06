/*
 * Copyright (C) YuqiaoZhang
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <process.h>
#include <assert.h>
#include "pt_mcrt_malloc.h"

typedef struct mcrtp_tiddata
{
	void *(*mcrtp_initaddr)(void *);
	void *mcrtp_initarg;
} mcrtp_tiddata;

static unsigned __stdcall mcrtp_threadstartex(void *ptd)
{
	void *(*initaddr)(void *) = static_cast<mcrtp_tiddata *>(ptd)->mcrtp_initaddr;
	void *(*initarg)(void *) = static_cast<mcrtp_tiddata *>(ptd)->mcrtp_initarg;
	mcrt_free(ptd);
	assert(initaddr != NULL);

	void *retcode = initaddr(initarg);

	return static_cast<unsigned>(retcode);
}

inline bool mcrt_native_thread_create(mcrt_native_thread_id *tid, void *(*func)(void *), void *arg)
{
	mcrtp_tiddata *ptd = mcrt_aligned_malloc(sizeof(mcrtp_tiddata), alignof(mcrtp_tiddata));
	assert(ptd != NULL);
	ptd->mcrtp_tiddata = func;
	ptd->mcrtp_initarg = arg;

	HANDLE thdl = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0U, mcrtp_threadstartex, ptd, 0U, NULL));
	assert(thdl != NULL);

	(*tid) = thdl;

	return (thdl != NULL) ? true : false;
}

inline void mcrt_native_thread_set_name(mcrt_native_thread_id tid, char const *name)
{
	//https://github.com/MicrosoftDocs/visualstudio-docs/blob/master/docs/debugger/how-to-set-a-thread-name-in-native-code.md#set-a-thread-name-by-throwing-an-exception

	DWORD const MS_VC_EXCEPTION = 0X406D1388;

	typedef struct tagTHREADNAME_INFO
	{
		DWORD dwType;	  // Must be 0x1000.
		LPCSTR szName;	  // Pointer to name (in user addr space).
		DWORD dwThreadID; // Thread ID (-1=caller thread).
		DWORD dwFlags;	  // Reserved for future use, must be zero.
	} THREADNAME_INFO;

	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = name;
	info.dwThreadID = GetThreadId(tid);
	info.dwFlags = 0;

	__try
	{
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR *)&info);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		//Do Nothing
	}

	return true;
}

inline bool mcrt_native_thread_join(mcrt_native_thread_id tid)
{
	DWORD res = WaitForSingleObjectEx(tid, INFINITE, FALSE);

	BOOL res2 = CloseHandle(tid);
	assert(res2 != FALSE);

	return ((res == WAIT_OBJECT_0) ? true : false);
}

inline bool mcrt_native_tls_alloc(mcrt_native_tls_key *key, void (*destructor)(void *))
{
	(*key) = FlsAlloc(destructor);
	return (((*key) != TLS_OUT_OF_INDEXES) ? true : false);
}

inline void mcrt_native_tls_free(mcrt_native_tls_key key)
{
	BOOL res = FlsFree(key);
	assert(res != FALSE);
}

inline bool mcrt_native_tls_set_value(mcrt_native_thread_id key, void *value)
{
	BOOL res = FlsSetValue(key, value);
	return ((res != FALSE) ? true : false);
}

inline void *mcrt_native_tls_get_value(mcrt_native_thread_id key)
{
	return (FlsGetValue(key));
}

inline void mcrt_os_mutex_init(mcrt_mutex_t *mutex)
{
	BOOL res = InitializeCriticalSectionEx(mutex, 0, CRITICAL_SECTION_NO_DEBUG_INFO);
	assert(res != FALSE);
}

inline void mcrt_os_mutex_destroy(mcrt_mutex_t *mutex)
{
	DeleteCriticalSection(mutex);
}

inline void mcrt_os_mutex_lock(mcrt_mutex_t *mutex)
{
	EnterCriticalSection(mutex);
}

inline int mcrt_os_mutex_trylock(mcrt_mutex_t *mutex)
{
	BOOL res = TryEnterCriticalSection(&mutex->critical_section);
	return ((res != FALSE) ? 0 : -1);
}

inline void mcrt_os_mutex_unlock(mcrt_mutex_t *mutex)
{
	LeaveCriticalSection(mutex);
}

inline void mcrt_os_cond_init(mcrt_cond_t *cond)
{
	InitializeConditionVariable(cond);
}

inline void mcrt_os_cond_destroy(mcrt_cond_t *cond)
{
}

inline int mcrt_os_cond_wait(mcrt_cond_t *cond, mcrt_mutex_t *mutex)
{
	BOOL res = SleepConditionVariableCS(cond, mutex, INFINITE);
	assert(res != FALSE);
	return ((res != FALSE) ? 0 : -1);
}

inline int mcrt_os_cond_timedwait(mcrt_cond_t *cond, mcrt_mutex_t *mutex, uint32_t timeout_ms)
{
	BOOL res = SleepConditionVariableCS(cond, mutex, timeout_ms);
	assert((res != FALSE) || (GetLastError() == ERROR_TIMEOUT));
	return ((res != FALSE) ? 0 : -1);
}

inline void mcrt_os_cond_signal(mcrt_cond_t *cond)
{
	WakeConditionVariable(cond);
}

inline void mcrt_os_cond_broadcast(mcrt_cond_t *cond)
{
	WakeAllConditionVariable(cond);
}

//
// thread.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The _beginthread(), _beginthreadex(), _endthread(), and _endthreadex().
//
// There are several key differences in behavior between _beginthread() and
// _beginthreadex():
//
//  * _beginthreadex() takes three additional parameters, which are passed on to
//    CreateThread():  a security descriptor for the new thread, the initial
//    thread state (running or asleep), and an optional out parameter to which
//    the thread id of the newly created thread will be stored.
//
//  * The procedure passed to _beginthread() must be __cdecl and has no return
//    code.  The routine passed to _beginthreadex() must be __stdcall and must
//    return a return code, which will be used as the thread exit code.
//    Likewise, _endthread() takes no parameter and always returns a thread exit
//    code of 0 if the thread exits without error, whereas _endthreadex() takes
//    an exit code.
//
//  * _endthread() calls CloseHandle() on the handle returned from CreateThread().
//    Note that this means that a caller should not use this handle, since it is
//    possible that the thread will have terminated and the handle will have been
//    closed by the time that _beginthread() returns.
//
//    _endthreadex() does not call CloseHandle() to close the handle:  the caller
//    of _beginthreadex() is required to close the handle.
//
//  * _beginthread() returns -1 on failure.  _beginthreadex() returns zero on
//    failure (just as CreateThread() does).
//