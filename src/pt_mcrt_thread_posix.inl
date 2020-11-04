inline bool mcrt_native_thread_create(mcrt_native_thread_id *tid, void *(*func)(void *), void *arg)
{
	int res = pthread_create(tid, NULL, (void *(*)(void *))func, arg);
	return ((res == 0) ? true : false);
}

inline bool mcrt_native_tls_alloc(mcrt_native_tls_key *key, void (*destructor)(void *))
{
	int res = pthread_key_create(key, destructor);
	assert(res == 0);
	return ((res == 0) ? true : false);
}

inline void mcrt_native_tls_free(mcrt_native_tls_key key)
{
	int res = pthread_key_delete(key);
	assert(res == 0);
}

inline bool mcrt_native_tls_set_value(mcrt_native_tls_key key, void *value)
{
	int res = pthread_setspecific(key, value);
	return ((res == 0) ? true : false);
}

inline void *mcrt_native_tls_get_value(mcrt_native_tls_key key)
{
	return pthread_getspecific(key);
}

inline void mcrt_os_mutex_init(mcrt_mutex_t *mutex)
{
	int res = pthread_mutex_init(mutex, NULL);
	assert(res == 0);
}

inline void mcrt_os_mutex_destroy(mcrt_mutex_t *mutex)
{
	int res = pthread_mutex_destroy(mutex);
	assert(res == 0);
}

inline void mcrt_os_mutex_lock(mcrt_mutex_t *mutex)
{
	int res = pthread_mutex_lock(mutex);
	assert(res == 0);
}

inline int mcrt_os_mutex_trylock(mcrt_mutex_t *mutex)
{
	int res = pthread_mutex_trylock(mutex);
	assert(res == 0 || res == EBUSY);

	return ((res == 0) ? 0 : -1);
}

inline void mcrt_os_mutex_unlock(mcrt_mutex_t *mutex)
{
	int res = pthread_mutex_unlock(mutex);
	assert(res == 0);
}

inline void mcrt_os_cond_init(mcrt_cond_t *cond)
{
	int res = pthread_cond_init(cond, NULL);
	assert(res == 0);
}

inline void mcrt_os_cond_destroy(mcrt_cond_t *cond)
{
	int res = pthread_cond_destroy(cond);
	assert(res == 0);
}

inline int mcrt_os_cond_wait(mcrt_cond_t *cond, mcrt_mutex_t *mutex)
{
	int res = pthread_cond_wait(cond, mutex);
	assert(res == 0);
	return ((res == 0) ? 0 : -1);
}

inline int mcrt_os_cond_timedwait(mcrt_cond_t *cond, mcrt_mutex_t *mutex, uint32_t timeout_ms)
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

inline void mcrt_os_cond_signal(mcrt_cond_t *cond)
{
	int res = pthread_cond_signal(cond);
	assert(res == 0);
}

inline void mcrt_os_cond_broadcast(mcrt_cond_t *cond)
{
	int res = pthread_cond_broadcast(cond);
	assert(res == 0);
}