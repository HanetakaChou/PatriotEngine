inline void mcrt_os_mutex_init(mcrt_os_mutex *mutex)
{
	int res = pthread_mutex_init(mutex, NULL);
	assert(res == 0);
}

inline void mcrt_os_mutex_destroy(mcrt_os_mutex *mutex)
{
	int res = pthread_mutex_destroy(mutex);
	assert(res == 0);
}

inline void mcrt_os_mutex_lock(mcrt_os_mutex *mutex)
{
	int res = pthread_mutex_lock(mutex);
	assert(res == 0);
}

inline int mcrt_os_mutex_trylock(mcrt_os_mutex *mutex)
{
	int res = pthread_mutex_trylock(mutex);
	assert(res == 0 || res == EBUSY);

	return ((res == 0) ? 0 : -1);
}

inline void mcrt_os_mutex_unlock(mcrt_os_mutex *mutex)
{
	int res = pthread_mutex_unlock(mutex);
	assert(res == 0);
}

inline void mcrt_os_cond_init(mcrt_os_cond *cond)
{
	int res = pthread_cond_init(cond, NULl);
	assert(res == 0);
}

inline void mcrt_os_cond_destroy(mcrt_os_cond *cond)
{
	int res = pthread_cond_destroy(cond);
	assert(res == 0);
}

inline int mcrt_os_cond_wait(mcrt_os_cond *cond, mcrt_os_mutex *mutex)
{
	int res = pthread_cond_wait(cond, mutex);
	assert(res == 0);
	return ((res == 0) ? 0 : -1);
}

inline int mcrt_os_cond_timedwait(mcrt_os_cond *cond, mcrt_os_mutex *mutex, uint32_t timeout_ms)
{
	struct timespec ts;
	int res;

	/* ms = 10^-3, us = 10^-6, ns = 10^-9 */

	struct timeval tv;
	res = gettimeofday(&tv, NULL);
	assert(res == 0);

	ts.tv_sec = tv.tv_sec;
	ts.tv_nsec = tv.tv_usec * 1000;

	ts.tv_sec += timeout_ms / 1000;
	ts.tv_nsec += (timeout_ms % 1000) * 1000 * 1000;

	if (ts.tv_nsec >= (1000 * 1000 * 1000))
	{
		ts.tv_nsec -= (1000 * 1000 * 1000);
		++ts.tv_sec;
	}
	assert(ts.tv_nsec < 1000 * 1000 * 1000);

	res = pthread_cond_timedwait(cond, mutex, &ts);
	assert(res == 0 || res == ETIMEDOUT);

	return ((res == 0) ? 0 : -1);
}

inline void mcrt_os_cond_signal(mcrt_os_cond *cond)
{
	int res = pthread_cond_signal(cond);
	assert(res == 0);
}

inline void mcrt_os_cond_broadcast(mcrt_os_cond *cond)
{
	int res = pthread_cond_broadcast(cond);
	assert(res == 0);
}