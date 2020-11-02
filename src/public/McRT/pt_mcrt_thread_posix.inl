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