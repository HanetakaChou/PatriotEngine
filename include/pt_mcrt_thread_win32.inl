//
// Copyright (C) YuqiaoZhang(HanetakaYuminaga)
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#if defined(_PT_MCRT_THREAD_H_) && !defined(_PT_MCRT_THREAD_WIN32_INL_)
#define _PT_MCRT_THREAD_WIN32_INL_ 1

#include <process.h>
#include <assert.h>

inline bool mcrt_native_thread_create(mcrt_native_thread_id *tid, unsigned(__stdcall*func)(void *), void *arg)
{
	HANDLE thdl = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0U, func, arg, 0U, NULL));
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
}

inline bool mcrt_native_thread_join(mcrt_native_thread_id tid)
{
	DWORD res = WaitForSingleObjectEx(tid, INFINITE, FALSE);

	BOOL res2 = CloseHandle(tid);
	assert(res2 != FALSE);

	return ((res == WAIT_OBJECT_0) ? true : false);
}

inline mcrt_native_thread_id mcrt_native_thread_id_get(void)
{
	return GetCurrentThread();
}

inline bool mcrt_native_thread_id_equal(mcrt_native_thread_id tid_left, mcrt_native_thread_id tid_right)
{
	return (tid_left == tid_right);
}

inline bool mcrt_native_tls_alloc(mcrt_native_tls_key *key, void (NTAPI *destructor)(void *))
{
	(*key) = FlsAlloc(destructor);
	return (((*key) != TLS_OUT_OF_INDEXES) ? true : false);
}

inline void mcrt_native_tls_free(mcrt_native_tls_key key)
{
	BOOL res = FlsFree(key);
	assert(res != FALSE);
}

inline bool mcrt_native_tls_set_value(mcrt_native_tls_key key, void *value)
{
	BOOL res = FlsSetValue(key, value);
	return ((res != FALSE) ? true : false);
}

inline void *mcrt_native_tls_get_value(mcrt_native_tls_key key)
{
	return (FlsGetValue(key));
}

inline void mcrt_os_yield()
{
	SwitchToThread();
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
	BOOL res = TryEnterCriticalSection(mutex);
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

inline void mcrt_os_sleep(uint32_t milli_second)
{
	SleepEx(milli_second, FALSE);
}

#else
#error "Never use <pt_mcrt_thread_win32.inl> directly; include <pt_mcrt_thread.h> instead."
#endif