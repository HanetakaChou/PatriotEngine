inline int mcrt_os_cond_timedwait(mcrt_os_cond *cond, mcrt_os_mutex *mutex, uint32_t timeout_ms)
{
	BOOL res = SleepConditionVariableCS(cond, mutex, timeout_ms);
	assert((res != FALSE) || (GetLastError() == ERROR_TIMEOUT));
	return ((res != FALSE) ? 0 : -1);
}