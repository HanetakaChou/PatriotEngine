#include <process.h>

inline bool mcrt_native_thread_create(mcrt_native_thread_id *tid, void *(*func)(void *), void *arg)
{
	HANDLE hThread = reinterpret_cast<HANDLE>(::_beginthreadex(NULL, 0U, pThreadEntry, pThreadParam, 0U, NULL));
	assert(hThread != NULL);

	(*pThreadOut) = hThread;

	return (hThread != NULL) ? true : false;
	int res = pthread_create(tid, NULL, (void *(*)(void *))func, arg);
	return ((res == 0) ? true : false);
}

inline bool mcrt_native_tls_alloc(mcrt_native_thread_id *key, void (*destructor)(void *))
{

}

inline void mcrt_native_tls_free(mcrt_native_thread_id key);
inline bool mcrt_native_tls_set_value(mcrt_native_thread_id key, void *value);
inline void *mcrt_native_tls_get_value(mcrt_native_thread_id key);

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
	return ((res == 0) ? 0 : -1);
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