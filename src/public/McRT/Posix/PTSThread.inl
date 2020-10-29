#include <assert.h>
#include <errno.h>
#include <time.h>

inline bool PTSThread_Create(PTSThreadEntry *pThreadEntry, void *pThreadParam, PTSThread *pThreadOut)
{
	return (::pthread_create(pThreadOut, NULL, pThreadEntry, pThreadParam) == 0) ? true : false;
}

inline bool PTSThread_Detach(PTSThread *pThread)
{
	return (::pthread_detach(*pThread) == 0) ? true : false;
}

inline bool PTSThread_Join(PTSThread *pThread)
{
	return (::pthread_join(*pThread, NULL) == 0) ? true : false;
}

inline bool mcrt_thread_setname(PTSThread *pThread, char const *name)
{
	pthread_t thread = (*pThread);

	//prctl(PR_SET_NAME PR_SET_NAME )

	int err = pthread_setname_np(thread, name);

	return (err == 0) ? true : false;
}

inline void PTS_Yield()
{
	::sched_yield();
}

inline void PTS_Sleep(uint32_t MilliSecond)
{
	struct timespec request;
	struct timespec remain;
	request.tv_sec = static_cast<time_t>(MilliSecond / 1000U);
	request.tv_nsec = static_cast<long>(1000000U * (MilliSecond % 1000U));
	::nanosleep(&request, &remain);
}

inline PTSThreadID PTSThreadID_Self()
{
	return ::pthread_self();
}

inline bool PTSThreadID_Equal(PTSThreadID TID1, PTSThreadID TID2)
{
	return (::pthread_equal(TID1, TID2) != 0) ? true : false;
}

mcrt_event::mcrt_event() : m_isValid(false)
{
}

bool mcrt_event::create_manual_event(bool initial_state)
{
	m_manual_reset = true;
	m_state = initial_state;
	initialize();
}

bool mcrt_event::create_auto_event(bool initial_state)
{
	m_manual_reset = false;
	m_state = initial_state;
	initialize();
}

bool mcrt_event::initialize()
{
	int st = pthread_mutex_init(&m_mutex, NULL);
	if (st != 0)
	{
		assert(!"Failed to initialize UnixEvent mutex");
		return false;
	}

	st = pthread_cond_init(&m_condition, NULL);
	if (st != 0)
	{
		assert(!"Failed to initialize UnixEvent condition variable");

		st = pthread_mutex_destroy(&m_mutex);
		assert(st == 0 && "Failed to destroy UnixEvent mutex");
		return false;
	}

	m_is_valid = true;

	return true;
}

void mcrt_event::close_event()
{
	if (m_is_valid)
	{
		int st = pthread_mutex_destroy(&m_mutex);
		assert(st == 0 && "Failed to destroy UnixEvent mutex");

		st = pthread_cond_destroy(&m_condition);
		assert(st == 0 && "Failed to destroy UnixEvent condition variable");
	}
}

inline bool mcrt_event::wait()
{
	int st = 0;

	pthread_mutex_lock(&m_mutex);

	while (!m_state)
	{
		st = pthread_cond_wait(&m_condition, &m_mutex);

		if (st != 0)
		{
			// wait failed or timed out
			break;
		}
	}

	if ((st == 0) && !m_manual_reset)
	{
		// Clear the state for auto-reset events so that only one waiter gets released
		m_state = false;
	}

	pthread_mutex_unlock(&m_mutex);

	bool wait_status;

	if (st == 0)
	{
		wait_status = true;
	}
	else
	{
		wait_status = false;
	}

	return wait_status;
}

inline bool mcrt_event::timedwait(uint32_t timeout_milliseconds)
{
	struct timeval now_time;
	gettimeofday(&now_time, NULL);

	struct timespec end_time;
	end_time.tv_sec = now_time.tv_sec;
	end_time.tv_nsec = 1000l * (now.tv_usec + 1000l * timeout_milliseconds);

	int st = 0;

	pthread_mutex_lock(&m_mutex);

	while (!m_state)
	{
		st = pthread_cond_timedwait(&m_condition, &m_mutex, &end_time);

		if (st != 0)
		{
			// wait failed or timed out
			break;
		}
	}

	if ((st == 0) && !m_manual_reset)
	{
		// Clear the state for auto-reset events so that only one waiter gets released
		m_state = false;
	}

	pthread_mutex_unlock(&m_mutex);

	bool wait_status;

	if (st == 0)
	{
		wait_status = true;
	}
	else if (st == ETIMEDOUT)
	{
		wait_status = false;
	}
	else
	{
		wait_status = false;
	}

	return wait_status;
}

inline void mcrt_event::set()
{
	pthread_mutex_lock(&m_mutex);
	m_state = true;
	pthread_mutex_unlock(&m_mutex);

	// Unblock all threads waiting for the condition variable
	pthread_cond_broadcast(&m_condition);
}

inline void mcrt_event::reset()
{
	pthread_mutex_lock(&m_mutex);
	m_state = false;
	pthread_mutex_unlock(&m_mutex);
}

inline bool PTSSemaphore_Create(uint32_t iInitialValue, PTSSemaphore *pSemaphoreOut)
{
	return (::sem_init(pSemaphoreOut, 0, iInitialValue) == 0) ? true : false;
}

inline bool PTSSemaphore_Passern(PTSSemaphore *pSemaphore)
{
	return (::sem_wait(pSemaphore) == 0) ? true : false;
}

inline bool PTSSemaphore_Vrijgeven(PTSSemaphore *pSemaphore)
{
	return (::sem_post(pSemaphore) == 0) ? true : false;
}

inline bool PTSSemaphore_Delete(PTSSemaphore *pSemaphore)
{
	return (::sem_destroy(pSemaphore) == 0) ? true : false;
}

inline bool PTSTSD_Create(PTSTSD_KEY *pTSD_Key, PTSTSD_DESTRUCTOR *pDestructor)
{
	int iResult = ::pthread_key_create(pTSD_Key, pDestructor);
	return (iResult == 0) ? true : false;
}

inline bool PTSTSD_SetValue(PTSTSD_KEY TSD_Key, PTSTSD_VALUE TSD_Value)
{
	int iResult = ::pthread_setspecific(TSD_Key, TSD_Value);
	return (iResult == 0) ? true : false;
}

inline PTSTSD_VALUE PTSTSD_GetValue(PTSTSD_KEY TSD_Key)
{
	return ::pthread_getspecific(TSD_Key);
}

inline bool PTSTSD_Delete(PTSTSD_KEY TSD_Key)
{
	int iResult = ::pthread_key_delete(TSD_Key);
	return (iResult == 0) ? true : false;
}

inline int64_t PTSTick_Count()
{
	timeval tv;
	int rtval = ::gettimeofday(&tv, NULL);
	assert(rtval == 0);
	return 1000000LL * static_cast<int64_t>(tv.tv_sec) + static_cast<int64_t>(tv.tv_usec);
}

inline int64_t PTSTick_Frequency()
{
	return 1000000LL;
}

#if defined(PTARM) || defined(PTARM64)
//#include <arm_acle.h>
//__yield
//__builtin_arm_yield
inline void PTS_Pause()
{
	__asm__ volatile("yield");
}
#elif defined(PTX86) || defined(PTX64)
//#include <emmintrin.h>
//_mm_pause
//__builtin_ia32_pause
inline void PTS_Pause()
{
	__asm__ volatile("pause");
}
#else
#error 未知的架构
#endif

inline int32_t PTSAtomic_CompareAndSet(int32_t volatile *pTarget, int32_t expect, int32_t update)
{
	return ::__sync_val_compare_and_swap(pTarget, expect, update);
}
inline int64_t PTSAtomic_CompareAndSet(int64_t volatile *pTarget, int64_t expect, int64_t update)
{
	return ::__sync_val_compare_and_swap(pTarget, expect, update);
}
inline uint32_t PTSAtomic_CompareAndSet(uint32_t volatile *pTarget, uint32_t expect, uint32_t update)
{
	return ::__sync_val_compare_and_swap(pTarget, expect, update);
}
inline uint64_t PTSAtomic_CompareAndSet(uint64_t volatile *pTarget, uint64_t expect, uint64_t update)
{
	return ::__sync_val_compare_and_swap(pTarget, expect, update);
}

inline int32_t PTSAtomic_GetAndSet(int32_t volatile *pTarget, int32_t newValue)
{
	return ::__sync_lock_test_and_set(pTarget, newValue);
}
inline int64_t PTSAtomic_GetAndSet(int64_t volatile *pTarget, int64_t newValue)
{
	return ::__sync_lock_test_and_set(pTarget, newValue);
}
inline uint32_t PTSAtomic_GetAndSet(uint32_t volatile *pTarget, uint32_t newValue)
{
	return ::__sync_lock_test_and_set(pTarget, newValue);
}
inline uint64_t PTSAtomic_GetAndSet(uint64_t volatile *pTarget, uint64_t newValue)
{
	return ::__sync_lock_test_and_set(pTarget, newValue);
}

inline int32_t PTSAtomic_GetAndAdd(int32_t volatile *pTarget, int32_t delta)
{
	return ::__sync_fetch_and_add(pTarget, delta);
}
inline int64_t PTSAtomic_GetAndAdd(int64_t volatile *pTarget, int64_t delta)
{
	return ::__sync_fetch_and_add(pTarget, delta);
}
inline uint32_t PTSAtomic_GetAndAdd(uint32_t volatile *pTarget, uint32_t delta)
{
	return ::__sync_fetch_and_add(pTarget, delta);
}
inline uint64_t PTSAtomic_GetAndAdd(uint64_t volatile *pTarget, uint64_t delta)
{
	return ::__sync_fetch_and_add(pTarget, delta);
}

#if defined(PTARM) || defined(PTARM64)
inline void PTS_AcquireBarrier()
{
	::PTS_HardwareReadBarrier();
}
inline void PTS_ReleaseBarrier()
{
	::PTS_HardwareWriteBarrier();
}
#elif defined(PTX86) || defined(PTX64)
inline void PTS_AcquireBarrier()
{
	::PTS_CompilerReadBarrier();
}
inline void PTS_ReleaseBarrier()
{
	::PTS_CompilerWriteBarrier();
}
#else
#error 未知的架构
#endif

inline int32_t PTSAtomic_Get(int32_t const volatile *pTarget)
{
	int32_t value = *pTarget;
	::PTS_AcquireBarrier();
	return value;
}
inline int64_t PTSAtomic_Get(int64_t const volatile *pTarget)
{
	int64_t value = *pTarget;
	::PTS_AcquireBarrier();
	return value;
}
inline uint32_t PTSAtomic_Get(uint32_t const volatile *pTarget)
{
	uint32_t value = *pTarget;
	::PTS_AcquireBarrier();
	return value;
}
inline uint64_t PTSAtomic_Get(uint64_t const volatile *pTarget)
{
	uint64_t value = *pTarget;
	::PTS_AcquireBarrier();
	return value;
}

inline void PTSAtomic_Set(int32_t volatile *pTarget, int32_t newValue)
{
	::PTS_ReleaseBarrier();
	(*pTarget) = newValue;
}

inline void PTSAtomic_Set(int64_t volatile *pTarget, int64_t newValue)
{
	::PTS_ReleaseBarrier();
	(*pTarget) = newValue;
}

inline void PTSAtomic_Set(uint32_t volatile *pTarget, uint32_t newValue)
{
	::PTS_ReleaseBarrier();
	(*pTarget) = newValue;
}

inline void PTSAtomic_Set(uint64_t volatile *pTarget, uint64_t newValue)
{
	::PTS_ReleaseBarrier();
	(*pTarget) = newValue;
}

inline void PTS_CompilerReadBarrier()
{
	__asm__ __volatile__(""
						 :
						 :
						 : "memory");
}

inline void PTS_CompilerWriteBarrier()
{
	__asm__ __volatile__(""
						 :
						 :
						 : "memory");
}

inline void PTS_CompilerReadWriteBarrier()
{
	__asm__ __volatile__(""
						 :
						 :
						 : "memory");
}

#if defined(PTARM)
inline void PTS_HardwareReadBarrier()
{
	__asm__ __volatile__("dmb sy"
						 :
						 :
						 : "memory");
}
inline void PTS_HardwareWriteBarrier()
{
	__asm__ __volatile__("dmb st"
						 :
						 :
						 : "memory");
}
inline void PTS_HardwareReadWriteBarrier()
{
	__asm__ __volatile__("dmb sy"
						 :
						 :
						 : "memory");
}
#elif defined(PTARM64)
inline void PTS_HardwareReadBarrier()
{
	__asm__ __volatile__("dmb ld"
						 :
						 :
						 : "memory");
}
inline void PTS_HardwareWriteBarrier()
{
	__asm__ __volatile__("dmb st"
						 :
						 :
						 : "memory");
}
inline void PTS_HardwareReadWriteBarrier()
{
	__asm__ __volatile__("dmb sy"
						 :
						 :
						 : "memory");
}
#elif defined(PTX86) || defined(PTX64)
inline void PTS_HardwareReadBarrier()
{
	__asm__ __volatile__("mfence"
						 :
						 :
						 : "memory");
}
inline void PTS_HardwareWriteBarrier()
{
	__asm__ __volatile__("mfence"
						 :
						 :
						 : "memory");
}
inline void PTS_HardwareReadWriteBarrier()
{
	__asm__ __volatile__("mfence"
						 :
						 :
						 : "memory");
}
#else
#error 未知的架构
#endif
