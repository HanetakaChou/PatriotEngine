#include <assert.h>
#include <errno.h>

PTBOOL PTSThread_Create(PTSThreadEntry *pThreadEntry, void *pThreadParam, PTSThread *pThreadOut)
{
	return (::pthread_create(pThreadOut, NULL, pThreadEntry, pThreadParam) == 0) ? PTTRUE : PTFALSE;
}

inline PTBOOL PTSThread_Detach(PTSThread *pThread)
{
	return (::pthread_detach(*pThread) == 0) ? PTTRUE : PTFALSE;
}

PTBOOL PTSThread_Join(PTSThread *pThread)
{
	return (::pthread_join(*pThread, NULL) == 0) ? PTTRUE : PTFALSE;
}

PTBOOL PTS_Yield()
{
	return (::sched_yield() == 0) ? PTTRUE : PTFALSE;
}

PTSThreadID PTSThreadID_Self()
{
	return ::pthread_self();
}

PTBOOL PTSThreadID_Equal(PTSThreadID TID1, PTSThreadID TID2)
{
	return (::pthread_equal(TID1, TID2) != 0) ? PTTRUE : PTFALSE;
}

PTBOOL PTSSemaphore_Create(uint32_t iInitialValue, PTSSemaphore *pSemaphoreOut)
{
	return (::sem_init(pSemaphoreOut, 0, iInitialValue) == 0) ? PTTRUE : PTFALSE;
}

PTBOOL PTSSemaphore_Passern(PTSSemaphore *pSemaphore)
{
	return (::sem_wait(pSemaphore) == 0) ? PTTRUE : PTFALSE;
}

PTBOOL PTSSemaphore_Vrijgeven(PTSSemaphore *pSemaphore)
{
	return (::sem_post(pSemaphore) == 0) ? PTTRUE : PTFALSE;
}

PTBOOL PTSSemaphore_Delete(PTSSemaphore *pSemaphore)
{
	return (::sem_destroy(pSemaphore) == 0) ? PTTRUE : PTFALSE;
}

PTBOOL PTSTSD_Create(PTSTSD_KEY *pTSD_Key, PTSTSD_DESTRUCTOR *pDestructor)
{
	int iResult = ::pthread_key_create(pTSD_Key, pDestructor);
	return (iResult == 0) ? PTTRUE : PTFALSE;
}

PTBOOL PTSTSD_SetValue(PTSTSD_KEY TSD_Key, PTSTSD_VALUE TSD_Value)
{
	int iResult = ::pthread_setspecific(TSD_Key, TSD_Value);
	return (iResult == 0) ? PTTRUE : PTFALSE;
}

inline PTSTSD_VALUE PTSTSD_GetValue(PTSTSD_KEY TSD_Key)
{
	return ::pthread_getspecific(TSD_Key);
}

PTBOOL PTSTSD_Delete(PTSTSD_KEY TSD_Key)
{
	int iResult = ::pthread_key_delete(TSD_Key);
	return (iResult == 0) ? PTTRUE : PTFALSE;
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
	__asm__ volatile ("yield");
}
#elif defined(PTX86) || defined(PTX64)
//#include <emmintrin.h>
//_mm_pause
//__builtin_ia32_pause
inline void PTS_Pause()
{
	__asm__ volatile ("pause");
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

inline int32_t PTSAtomic_Get(int32_t volatile *pTarget)
{
	return ::__atomic_load_n(pTarget, __ATOMIC_ACQUIRE);
}
inline int64_t PTSAtomic_Get(int64_t volatile *pTarget)
{
	return ::__atomic_load_n(pTarget, __ATOMIC_ACQUIRE);
}
inline uint32_t PTSAtomic_Get(uint32_t volatile *pTarget)
{
	return ::__atomic_load_n(pTarget, __ATOMIC_ACQUIRE);
}
inline uint64_t PTSAtomic_Get(uint64_t volatile *pTarget)
{
	return ::__atomic_load_n(pTarget, __ATOMIC_ACQUIRE);
}

inline void PTSAtomic_Set(int32_t volatile *pTarget, int32_t newValue)
{
	::__atomic_store_n(pTarget, newValue, __ATOMIC_RELEASE);
}
inline void PTSAtomic_Set(int64_t volatile *pTarget, int64_t newValue)
{
	::__atomic_store_n(pTarget, newValue, __ATOMIC_RELEASE);
}
inline void PTSAtomic_Set(uint32_t volatile *pTarget, uint32_t newValue)
{
	::__atomic_store_n(pTarget, newValue, __ATOMIC_RELEASE);
}
inline void PTSAtomic_Set(uint64_t volatile *pTarget, uint64_t newValue)
{
	::__atomic_store_n(pTarget, newValue, __ATOMIC_RELEASE);
}