inline void mcrt_os_mutex_init(mcrt_os_mutex *mutex)
{
	BOOL res = InitializeCriticalSectionEx(mutex, 0, CRITICAL_SECTION_NO_DEBUG_INFO);
	assert(res != FALSE);
}

inline void mcrt_os_mutex_destroy(mcrt_os_mutex *mutex)
{
	DeleteCriticalSection(mutex);
}

inline void mcrt_os_mutex_lock(mcrt_os_mutex *mutex)
{
	EnterCriticalSection(mutex);
}

inline int mcrt_os_mutex_trylock(mcrt_os_mutex *mutex)
{
	BOOL res = TryEnterCriticalSection(&mutex->critical_section);
	return ((res == 0) ? 0 : -1);
}

inline void mcrt_os_mutex_unlock(mcrt_os_mutex *mutex)
{
	LeaveCriticalSection(mutex);
}

inline void mcrt_os_cond_init(mcrt_os_cond *cond)
{
	InitializeConditionVariable(cond);
}

inline void mcrt_os_cond_destroy(mcrt_os_cond *cond)
{
}

inline int mcrt_os_cond_wait(mcrt_os_cond *cond, mcrt_os_mutex *mutex)
{
	BOOL res = SleepConditionVariableCS(cond, mutex, INFINITE);
	assert(res != FALSE);
	return ((res != FALSE) ? 0 : -1);
}

inline int mcrt_os_cond_timedwait(mcrt_os_cond *cond, mcrt_os_mutex *mutex, uint32_t timeout_ms)
{
	BOOL res = SleepConditionVariableCS(cond, mutex, timeout_ms);
	assert((res != FALSE) || (GetLastError() == ERROR_TIMEOUT));
	return ((res != FALSE) ? 0 : -1);
}

inline void mcrt_os_cond_signal(mcrt_os_cond *cond)
{
	WakeConditionVariable(cond);
}

inline void mcrt_os_cond_broadcast(mcrt_os_cond *cond)
{
	WakeAllConditionVariable(cond);
}