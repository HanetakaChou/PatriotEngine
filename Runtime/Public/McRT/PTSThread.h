#ifndef PT_SYSTEM_THREAD_H
#define PT_SYSTEM_THREAD_H

#include "../PTCommon.h"
#include "PTSCommon.h"

#include <stddef.h>
#include <stdint.h>

#ifdef PTWIN32
#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <process.h>
typedef unsigned __stdcall PTSThreadEntry(void *pThreadParam);
typedef HANDLE PTSThread;
#elif defined(PTPOSIX)
#include <pthread.h>
#include <sched.h>
typedef void * PTSThreadEntry(void *);
typedef pthread_t PTSThread;
#else
#error 未知的平台
#endif
inline bool PTSThread_Create(PTSThreadEntry *, void *pThreadParam, PTSThread *pThreadOut);
inline bool PTSThread_Detach(PTSThread *pThread);
inline bool PTSThread_Join(PTSThread *pThread);

inline void PTS_Yield();
inline void PTS_Sleep(uint32_t MilliSecond);

#ifdef PTWIN32
typedef DWORD PTSThreadID;
#elif defined(PTPOSIX)
typedef pthread_t PTSThreadID;
#else
#error 未知的平台
#endif

inline PTSThreadID PTSThreadID_Self();
inline bool PTSThreadID_Equal(PTSThreadID TID1, PTSThreadID TID2);

#ifdef PTWIN32
typedef HANDLE PTSSemaphore;
#elif defined(PTPOSIX)
#include <semaphore.h>
typedef sem_t PTSSemaphore;
#else
#error 未知的平台
#endif
inline bool PTSSemaphore_Create(uint32_t iInitialValue, PTSSemaphore *pSemaphoreOut);
inline bool PTSSemaphore_Passern(PTSSemaphore *pSemaphore);
inline bool PTSSemaphore_Vrijgeven(PTSSemaphore *pSemaphore);
inline bool PTSSemaphore_Delete(PTSSemaphore *pSemaphore);

#ifdef PTWIN32
typedef DWORD PTSTSD_KEY;
typedef void WINAPI PTSTSD_DESTRUCTOR(void *);
#elif defined(PTPOSIX)
typedef pthread_key_t PTSTSD_KEY;
typedef void PTSTSD_DESTRUCTOR(void *);
#else
#error 未知的平台
#endif
typedef void *PTSTSD_VALUE;

inline bool PTSTSD_Create(PTSTSD_KEY *pTSD_Key, PTSTSD_DESTRUCTOR *pDestructor);
inline bool PTSTSD_SetValue(PTSTSD_KEY TSD_Key, PTSTSD_VALUE TSD_Value);
inline PTSTSD_VALUE PTSTSD_GetValue(PTSTSD_KEY TSD_Key);
inline bool PTSTSD_Delete(PTSTSD_KEY TSD_Key);

#ifdef PTWIN32

#elif defined(PTPOSIX)
#include <sys/time.h>
#else
#error 未知的平台
#endif
inline int64_t PTSTick_Count();
inline int64_t PTSTick_Frequency();

//Intrinsic

inline void PTS_Pause();

inline int32_t PTSAtomic_CompareAndSet(int32_t volatile *pTarget, int32_t expect, int32_t update);
inline int64_t PTSAtomic_CompareAndSet(int64_t volatile *pTarget, int64_t expect, int64_t update);
inline uint32_t PTSAtomic_CompareAndSet(uint32_t volatile *pTarget, uint32_t expect, uint32_t update);
inline uint64_t PTSAtomic_CompareAndSet(uint64_t volatile *pTarget, uint64_t expect, uint64_t update);

#if defined(PTWIN32DESKTOP) ||defined(PTWIN32RUNTIME) || defined(PTPOSIXLINUXGLIBC) || defined(PTPOSIXLINUXBIONIC)
//Nothing
#elif defined(PTPOSIXMACH)
#include <assert.h>
template<size_t const SizeOfPointerUnsigned>
inline uintptr_t PTSAtomic_CompareAndSet_Helper(uintptr_t volatile *pTarget, uintptr_t expect, uintptr_t update)
{
	switch (SizeOfPointerUnsigned)
	{
	case 4U: return static_cast<uintptr_t>(PTSAtomic_CompareAndSet(reinterpret_cast<uint32_t volatile *>(pTarget), static_cast<uint32_t>(expect), static_cast<uint32_t>(update)));
	case 8U: return static_cast<uintptr_t>(PTSAtomic_CompareAndSet(reinterpret_cast<uint64_t volatile *>(pTarget), static_cast<uint32_t>(expect), static_cast<uint64_t>(update)));
	default: assert(0); return ~0U;
	}
}
inline uintptr_t PTSAtomic_CompareAndSet(uintptr_t volatile *pTarget, uintptr_t expect, uintptr_t update)
{
	return PTSAtomic_CompareAndSet_Helper<sizeof(uintptr_t)>(pTarget, expect, update);
}
#else
#error 未知的平台
#endif

template <typename T>
inline T* PTSAtomic_CompareAndSet(T* *pTarget, T* expect,T* update)
{
	return reinterpret_cast<T*>(::PTSAtomic_CompareAndSet(reinterpret_cast<uintptr_t *>(pTarget), reinterpret_cast<uintptr_t>(expect), reinterpret_cast<uintptr_t>(update)));
}

inline int32_t PTSAtomic_GetAndSet(int32_t volatile *pTarget, int32_t newValue);
inline int64_t PTSAtomic_GetAndSet(int64_t volatile *pTarget, int64_t newValue);
inline uint32_t PTSAtomic_GetAndSet(uint32_t volatile *pTarget, uint32_t newValue);
inline uint64_t PTSAtomic_GetAndSet(uint64_t volatile *pTarget, uint64_t newValue);

#if defined(PTWIN32DESKTOP) ||defined(PTWIN32RUNTIME) || defined(PTPOSIXLINUXGLIBC) || defined(PTPOSIXLINUXBIONIC)
//Nothing
#elif defined(PTPOSIXMACH)
#include <assert.h>
template<size_t const SizeOfPointerUnsigned>
inline uintptr_t PTSAtomic_GetAndSet_Helper(uintptr_t volatile *pTarget, uintptr_t newValue)
{
	switch (SizeOfPointerUnsigned)
	{
	case 4U: return static_cast<uintptr_t>(PTSAtomic_GetAndSet(reinterpret_cast<uint32_t volatile *>(pTarget), static_cast<uint32_t>(newValue)));
	case 8U: return static_cast<uintptr_t>(PTSAtomic_GetAndSet(reinterpret_cast<uint64_t volatile *>(pTarget), static_cast<uint64_t>(newValue)));
	default: assert(0); return ~0U;
	}
}
inline uintptr_t PTSAtomic_GetAndSet(uintptr_t volatile *pTarget, uintptr_t newValue)
{
	return PTSAtomic_GetAndSet_Helper<sizeof(uintptr_t)>(pTarget, newValue);
}
#else
#error 未知的平台
#endif

template <typename T>
inline T* PTSAtomic_GetAndSet(T* *pTarget, T* newValue)
{
	return reinterpret_cast<T*>(::PTSAtomic_GetAndSet(reinterpret_cast<uintptr_t *>(pTarget), reinterpret_cast<uintptr_t>(newValue)));
}

inline int32_t PTSAtomic_GetAndAdd(int32_t volatile *pTarget, int32_t delta);
inline int64_t PTSAtomic_GetAndAdd(int64_t volatile *pTarget, int64_t delta);
inline uint32_t PTSAtomic_GetAndAdd(uint32_t volatile *pTarget, uint32_t delta);
inline uint64_t PTSAtomic_GetAndAdd(uint64_t volatile *pTarget, uint64_t delta);

inline int32_t PTSAtomic_Get(int32_t const volatile *pTarget);
inline int64_t PTSAtomic_Get(int64_t const volatile *pTarget);
inline uint32_t PTSAtomic_Get(uint32_t const volatile *pTarget);
inline uint64_t PTSAtomic_Get(uint64_t const volatile *pTarget);

#if defined(PTWIN32DESKTOP) ||defined(PTWIN32RUNTIME) || defined(PTPOSIXLINUXGLIBC) || defined(PTPOSIXLINUXBIONIC)
//Nothing
#elif defined(PTPOSIXMACH)
#include <assert.h>
template<size_t const SizeOfPointerUnsigned>
inline uintptr_t PTSAtomic_Get_Helper(uintptr_t const volatile *pTarget)
{
	switch (SizeOfPointerUnsigned)
	{
	case 4U: return static_cast<uintptr_t>(PTSAtomic_Get(reinterpret_cast<uint32_t const volatile *>(pTarget)));
	case 8U: return static_cast<uintptr_t>(PTSAtomic_Get(reinterpret_cast<uint64_t const volatile *>(pTarget)));
	default: assert(0); return ~0U;
	}
}
inline uintptr_t PTSAtomic_Get(uintptr_t const volatile *pTarget)
{
	return PTSAtomic_Get_Helper<sizeof(uintptr_t)>(pTarget);
}
#else
#error 未知的平台
#endif

template <typename T>
inline T* PTSAtomic_Get(T const* *pTarget)
{
	return reinterpret_cast<T*>(::PTSAtomic_Get(reinterpret_cast<uintptr_t const*>(pTarget)));
}

inline void PTSAtomic_Set(int32_t volatile *pTarget, int32_t newValue);
inline void PTSAtomic_Set(int64_t volatile *pTarget, int64_t newValue);
inline void PTSAtomic_Set(uint32_t volatile *pTarget, uint32_t newValue);
inline void PTSAtomic_Set(uint64_t volatile *pTarget, uint64_t newValue);

#if defined(PTWIN32DESKTOP) ||defined(PTWIN32RUNTIME) || defined(PTPOSIXLINUXGLIBC) || defined(PTPOSIXLINUXBIONIC)
//Nothing
#elif defined(PTPOSIXMACH)
#include <assert.h>
template<size_t const SizeOfPointerUnsigned>
inline void PTSAtomic_Set_Helper(uintptr_t volatile *pTarget, uintptr_t newValue)
{
	switch (SizeOfPointerUnsigned)
	{
	case 4U: PTSAtomic_Set(reinterpret_cast<uint32_t volatile *>(pTarget), static_cast<uint32_t>(newValue)); break;
	case 8U: PTSAtomic_Set(reinterpret_cast<uint64_t volatile *>(pTarget), static_cast<uint64_t>(newValue)); break;
	default: assert(0);
	}
}

inline void PTSAtomic_Set(uintptr_t volatile *pTarget, uintptr_t newValue)
{
	PTSAtomic_Set_Helper<sizeof(uintptr_t)>(pTarget, newValue);
}
#else
#error 未知的平台
#endif

template <typename T>
inline void PTSAtomic_Set(T* *pTarget, T* newValue)
{
	::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(pTarget), reinterpret_cast<uintptr_t>(newValue));
}

inline void PTS_AcquireBarrier();
inline void PTS_ReleaseBarrier();
inline void PTS_AcquireReleaseBarrier();

inline void PTS_CompilerReadBarrier();
inline void PTS_CompilerWriteBarrier();
inline void PTS_CompilerReadWriteBarrier();

inline void PTS_HardwareReadBarrier();
inline void PTS_HardwareWriteBarrier();
inline void PTS_HardwareReadWriteBarrier();

#if defined PTWIN32
#include "Win32/PTSThread.inl"
#elif defined PTPOSIX
#include "Posix/PTSThread.inl"
#else
#error 未知的平台
#endif

#endif

