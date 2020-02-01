#include <assert.h>

inline bool PTSThread_Create(PTSThreadEntry *pThreadEntry, void *pThreadParam, PTSThread *pThreadOut)
{
	HANDLE hThread = reinterpret_cast<HANDLE>(::_beginthreadex(NULL, 0U, pThreadEntry, pThreadParam, 0U, NULL));
	assert(hThread != NULL);

	(*pThreadOut) = hThread;

	return (hThread != NULL) ? true : false;
}

inline bool PTSThread_Detach(PTSThread *pThread)
{
	HANDLE hThread = *pThread;
	return (::CloseHandle(hThread) != FALSE) ? true : false;
}

inline bool PTSThread_Join(PTSThread *pThread)
{
	HANDLE hThread = *pThread;
	return ((::WaitForSingleObjectEx(hThread, INFINITE, FALSE) == WAIT_OBJECT_0) && (::CloseHandle(hThread) != FALSE)) ? true : false;
}

inline void PTS_Yield()
{
	::SwitchToThread();
}

inline void PTS_Sleep(uint32_t MilliSecond)
{
	::SleepEx(MilliSecond, FALSE);
}

inline PTSThreadID PTSThreadID_Self()
{
	return ::GetCurrentThreadId();
}

inline bool PTSThreadID_Equal(PTSThreadID TID1, PTSThreadID TID2)
{
	return (TID1 == TID2) ? true : false;
}

inline bool PTSSemaphore_Create(uint32_t iInitialValue, PTSSemaphore *pSemaphoreOut)
{
	HANDLE hSemaphore = ::CreateSemaphoreExW(NULL, iInitialValue, 32767, NULL, 0U, DELETE | SYNCHRONIZE | SEMAPHORE_MODIFY_STATE);//#define _POSIX_SEM_VALUE_MAX  32767 //与Posix一致
	assert(hSemaphore != NULL);

	(*pSemaphoreOut) = hSemaphore;

	return(hSemaphore != NULL) ? true : false;
}

inline bool PTSSemaphore_Passern(PTSSemaphore *pSemaphore)
{
	HANDLE hSemaphore = *pSemaphore;
	return (::WaitForSingleObjectEx(hSemaphore, INFINITE, FALSE) == WAIT_OBJECT_0) ? true : false;
}

inline bool PTSSemaphore_Vrijgeven(PTSSemaphore *pSemaphore)
{
	HANDLE hSemaphore = *pSemaphore;
	return (::ReleaseSemaphore(hSemaphore, 1, NULL) != FALSE) ? true : false;
}

inline bool PTSSemaphore_Delete(PTSSemaphore *pSemaphore)
{
	HANDLE hSemaphore = *pSemaphore;
	return (::CloseHandle(hSemaphore) != FALSE) ? true : false;
}

inline bool PTSTSD_Create(PTSTSD_KEY *pTSD_Key, PTSTSD_DESTRUCTOR *pDestructor)
{
	(*pTSD_Key) = ::FlsAlloc(pDestructor);
	return ((*pTSD_Key) != FLS_OUT_OF_INDEXES) ? true : false;
}

inline bool PTSTSD_SetValue(PTSTSD_KEY TSD_Key, PTSTSD_VALUE TSD_Value)
{
	BOOL wbResult = ::FlsSetValue(TSD_Key, TSD_Value);
	return (wbResult != FALSE) ? true : false;
}

inline PTSTSD_VALUE PTSTSD_GetValue(PTSTSD_KEY TSD_Key)
{
	return ::FlsGetValue(TSD_Key);
}

inline bool PTSTSD_Delete(PTSTSD_KEY TSD_Key)
{
	BOOL wbResult = ::FlsFree(TSD_Key);
	return (wbResult != FALSE) ? true : false;
}

inline int64_t PTSTick_Count()
{
	LARGE_INTEGER PerformanceCount;
	BOOL bResult = ::QueryPerformanceCounter(&PerformanceCount);
	assert(bResult != FALSE);
	return PerformanceCount.QuadPart;
}

inline int64_t PTSTick_Frequency()
{
	LARGE_INTEGER Frequency;
	BOOL bResult = ::QueryPerformanceFrequency(&Frequency);
	assert(bResult != FALSE);
	return Frequency.QuadPart;
}

#include <intrin.h>

#if defined(PTARM) || defined(PTARM64)
//#include <arm_acle.h> __yield
inline void PTCALL PTS_Pause()
{
	::__yield();
}
#elif defined(PTX86) || defined(PTX64)
inline void PTS_Pause()
{
	::_mm_pause();
}
#else
#error 未知的架构
#endif

inline int32_t PTSAtomic_CompareAndSet(int32_t volatile *pTarget, int32_t expect, int32_t update)
{ 
	return ::_InterlockedCompareExchange(reinterpret_cast<LONG volatile *>(pTarget), update, expect);
}
inline int64_t PTSAtomic_CompareAndSet(int64_t volatile *pTarget, int64_t expect, int64_t update)
{
	return ::_InterlockedCompareExchange64(reinterpret_cast<LONGLONG volatile *>(pTarget), update, expect);
}
inline uint32_t PTSAtomic_CompareAndSet(uint32_t volatile *pTarget, uint32_t expect, uint32_t update)
{
	return static_cast<uint32_t>(::_InterlockedCompareExchange(reinterpret_cast<LONG volatile *>(pTarget), static_cast<LONG>(update), static_cast<LONG>(expect)));
}
inline uint64_t PTSAtomic_CompareAndSet(uint64_t volatile *pTarget, uint64_t expect, uint64_t update)
{
	return static_cast<uint64_t>(::_InterlockedCompareExchange64(reinterpret_cast<LONGLONG volatile *>(pTarget), static_cast<LONGLONG>(update), static_cast<LONGLONG>(expect)));
}

inline int32_t PTSAtomic_GetAndSet(int32_t volatile *pTarget, int32_t newValue)
{
	return ::_InterlockedExchange(reinterpret_cast<LONG volatile *>(pTarget), newValue);
}
inline int64_t PTSAtomic_GetAndSet(int64_t volatile *pTarget, int64_t newValue)
{
	return InterlockedExchange64(reinterpret_cast<LONGLONG volatile *>(pTarget), newValue);

}
inline uint32_t PTSAtomic_GetAndSet(uint32_t volatile *pTarget, uint32_t newValue)
{
	return static_cast<uint32_t>(::_InterlockedExchange(reinterpret_cast<LONG volatile *>(pTarget), static_cast<LONG>(newValue)));
}
inline uint64_t PTSAtomic_GetAndSet(uint64_t volatile *pTarget, uint64_t newValue)
{
	return static_cast<uint64_t>(InterlockedExchange64(reinterpret_cast<LONGLONG volatile *>(pTarget), static_cast<LONGLONG>(newValue)));
}

inline int32_t PTSAtomic_GetAndAdd(int32_t volatile *pTarget, int32_t delta)
{
	return ::_InterlockedExchangeAdd(reinterpret_cast<LONG volatile *>(pTarget), delta);
}

inline int64_t PTSAtomic_GetAndAdd(int64_t volatile *pTarget, int64_t delta)
{
	return InterlockedExchangeAdd64(reinterpret_cast<LONGLONG volatile *>(pTarget), delta);
}

inline uint32_t PTSAtomic_GetAndAdd(uint32_t volatile *pTarget, uint32_t delta)
{
	return static_cast<uint32_t>(::InterlockedExchangeAdd(reinterpret_cast<LONG volatile *>(pTarget), static_cast<LONG>(delta)));
}

inline uint64_t PTSAtomic_GetAndAdd(uint64_t volatile *pTarget, uint64_t delta)
{

	return static_cast<uint64_t>(InterlockedExchangeAdd64(reinterpret_cast<LONGLONG volatile *>(pTarget), static_cast<LONGLONG>(delta)));
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
inline void PTS_AcquireReleaseBarrier()
{
	::PTS_HardwareReadWriteBarrier();
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
inline void PTS_AcquireReleaseBarrier()
{
	::PTS_CompilerReadWriteBarrier();
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
	::_ReadBarrier();
}

inline void PTS_CompilerWriteBarrier()
{
	::_WriteBarrier();
}

inline void PTS_CompilerReadWriteBarrier()
{
	::_ReadWriteBarrier();
}

#if defined(PTARM) || defined(PTARM64)
inline void PTS_HardwareReadBarrier()
{
	__dmb(_ARM_BARRIER_SY);
}
inline void PTS_HardwareWriteBarrier()
{
	__dmb(_ARM_BARRIER_ST);
}
inline void PTS_HardwareReadWriteBarrier()
{
	__dmb(_ARM_BARRIER_SY);
}
#elif defined(PTX86) || defined(PTX64)
inline void PTS_HardwareReadBarrier()
{
	_mm_mfence();
}
inline void PTS_HardwareWriteBarrier()
{
	_mm_mfence();
}
inline void PTS_HardwareReadWriteBarrier()
{
	_mm_mfence();
}
#else
#error 未知的架构
#endif