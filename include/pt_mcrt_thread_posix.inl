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

#if defined(_PT_MCRT_THREAD_H_) && !defined(_PT_MCRT_THREAD_POSIX_INL_)
#define _PT_MCRT_THREAD_POSIX_INL_ 1

#include <stddef.h>
#include <sys/time.h>
#include <errno.h>
#include <assert.h>

inline bool mcrt_native_thread_create(mcrt_native_thread_id *tid, void *(*func)(void *), void *arg)
{
	PT_MAYBE_UNUSED int res = pthread_create(tid, NULL, (void *(*)(void *))func, arg);
	return ((res == 0) ? true : false);
}

inline void mcrt_native_thread_set_name(mcrt_native_thread_id tid, char const *name)
{
#if defined(PT_POSIX_LINUX)
	PT_MAYBE_UNUSED int res = pthread_setname_np(tid, name);
	assert(res == 0);
	return;
#elif defined(PT_POSIX_MACH)
	if (pthread_equal(pthread_self(), tid))
	{
		PT_MAYBE_UNUSED int res = pthread_setname_np(name);
		assert(res == 0);
		return;
	}
	else
	{
		assert(false);
		return;
	}
#else
#error Unknown Platform
#endif
}

inline bool mcrt_native_thread_join(mcrt_native_thread_id tid)
{
	void *retcode;
	PT_MAYBE_UNUSED int res = pthread_join(tid, &retcode);
	return ((res == 0) ? true : false);
}

inline mcrt_native_thread_id mcrt_native_thread_id_get(void)
{
	return pthread_self();
}

inline bool mcrt_native_thread_id_equal(mcrt_native_thread_id tid_left, mcrt_native_thread_id tid_right)
{
	return (0 != pthread_equal(tid_left, tid_right));
}

inline bool mcrt_native_tls_alloc(mcrt_native_tls_key *key, void (*destructor)(void *))
{
	PT_MAYBE_UNUSED int res = pthread_key_create(key, destructor);
	assert(res == 0);
	return ((res == 0) ? true : false);
}

inline void mcrt_native_tls_free(mcrt_native_tls_key key)
{
	PT_MAYBE_UNUSED int res = pthread_key_delete(key);
	assert(res == 0);
}

inline bool mcrt_native_tls_set_value(mcrt_native_tls_key key, void *value)
{
	PT_MAYBE_UNUSED int res = pthread_setspecific(key, value);
	return ((res == 0) ? true : false);
}

inline void *mcrt_native_tls_get_value(mcrt_native_tls_key key)
{
	return (pthread_getspecific(key));
}

inline void mcrt_os_yield(void)
{
#if defined(PT_POSIX_LINUX)
#if defined(PT_POSIX_LINUX_ANDROID)
	PT_MAYBE_UNUSED int res = sched_yield();
	assert(res == 0);
	return;
#elif defined(PT_POSIX_LINUX_X11)
	PT_MAYBE_UNUSED int res = pthread_yield();
	assert(res == 0);
	return;
#else
#error Unknown Platform
#endif
#elif defined(PT_POSIX_MACH)
	return pthread_yield_np();
#else
#error Unknown Platform
#endif
}

inline void mcrt_os_mutex_init(mcrt_mutex_t *mutex)
{
	PT_MAYBE_UNUSED int res = pthread_mutex_init(mutex, NULL);
	assert(res == 0);
}

inline void mcrt_os_mutex_destroy(mcrt_mutex_t *mutex)
{
	PT_MAYBE_UNUSED int res = pthread_mutex_destroy(mutex);
	assert(res == 0);
}

inline void mcrt_os_mutex_lock(mcrt_mutex_t *mutex)
{
	PT_MAYBE_UNUSED int res = pthread_mutex_lock(mutex);
	assert(res == 0);
}

inline int mcrt_os_mutex_trylock(mcrt_mutex_t *mutex)
{
	PT_MAYBE_UNUSED int res = pthread_mutex_trylock(mutex);
	assert(res == 0 || res == EBUSY);

	return ((res == 0) ? 0 : -1);
}

inline void mcrt_os_mutex_unlock(mcrt_mutex_t *mutex)
{
	PT_MAYBE_UNUSED int res = pthread_mutex_unlock(mutex);
	assert(res == 0);
}

inline void mcrt_os_cond_init(mcrt_cond_t *cond)
{
	PT_MAYBE_UNUSED int res = pthread_cond_init(cond, NULL);
	assert(res == 0);
}

inline void mcrt_os_cond_destroy(mcrt_cond_t *cond)
{
	PT_MAYBE_UNUSED int res = pthread_cond_destroy(cond);
	assert(res == 0);
}

inline int mcrt_os_cond_wait(mcrt_cond_t *cond, mcrt_mutex_t *mutex)
{
	PT_MAYBE_UNUSED int res = pthread_cond_wait(cond, mutex);
	assert(res == 0);
	return ((res == 0) ? 0 : -1);
}

inline int mcrt_os_cond_timedwait(mcrt_cond_t *cond, mcrt_mutex_t *mutex, uint32_t timeout_ms)
{
	struct timespec ts;
	{

		struct timeval tv;
		PT_MAYBE_UNUSED int res_get_time_of_day = gettimeofday(&tv, NULL);
		assert(res_get_time_of_day == 0);

		// ms = 10^-3, us = 10^-6, ns = 10^-9
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
	}

	PT_MAYBE_UNUSED int res_pthread_cond_timedwait = pthread_cond_timedwait(cond, mutex, &ts);
	assert(res_pthread_cond_timedwait == 0 || res_pthread_cond_timedwait == ETIMEDOUT);

	return ((res_pthread_cond_timedwait == 0) ? 0 : -1);
}

inline void mcrt_os_cond_signal(mcrt_cond_t *cond)
{
	PT_MAYBE_UNUSED int res = pthread_cond_signal(cond);
	assert(res == 0);
}

inline void mcrt_os_cond_broadcast(mcrt_cond_t *cond)
{
	PT_MAYBE_UNUSED int res = pthread_cond_broadcast(cond);
	assert(res == 0);
}

inline void mcrt_os_sleep(uint32_t milli_second)
{
#if defined(PT_POSIX_LINUX)
	struct timespec request = {((time_t)milli_second) / ((time_t)1000), ((long)1000000) * (((long)milli_second) % ((long)1000))};
#elif defined(PT_POSIX_MACH)
	struct timespec request = {((__darwin_time_t)milli_second) / ((__darwin_time_t)1000), ((long)1000000) * (((long)milli_second) % ((long)1000))};
#else
#error Unknown Platform
#endif
	struct timespec remain;

	int res_sleep;
	while ((-1 == (res_sleep = nanosleep(&request, &remain))) && (EINTR == errno))
	{
		assert(remain.tv_nsec > 0 || remain.tv_sec > 0);
		request = remain;
	}
	assert(0 == res_sleep);
}

#else
#error "Never use <pt_mcrt_thread_posix.inl> directly; include <pt_mcrt_thread.h> instead."
#endif
