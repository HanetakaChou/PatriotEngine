#ifndef PT_SYSTEM_THREAD_H
#define PT_SYSTEM_THREAD_H

#include "../PTCommon.h"
#include "PTSCommon.h"

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
inline PTBOOL PTSThread_Create(PTSThreadEntry *, void *pThreadParam, PTSThread *pThreadOut);
inline PTBOOL PTSThread_Detach(PTSThread *pThread);
inline PTBOOL PTSThread_Join(PTSThread *pThread);

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
inline PTBOOL PTSThreadID_Equal(PTSThreadID TID1, PTSThreadID TID2);

#ifdef PTWIN32
typedef HANDLE PTSSemaphore;
#elif defined(PTPOSIX)
#include <semaphore.h>
typedef sem_t PTSSemaphore;
#else
#error 未知的平台
#endif
inline PTBOOL PTSSemaphore_Create(uint32_t iInitialValue, PTSSemaphore *pSemaphoreOut);
inline PTBOOL PTSSemaphore_Passern(PTSSemaphore *pSemaphore);
inline PTBOOL PTSSemaphore_Vrijgeven(PTSSemaphore *pSemaphore);
inline PTBOOL PTSSemaphore_Delete(PTSSemaphore *pSemaphore);

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

inline PTBOOL PTSTSD_Create(PTSTSD_KEY *pTSD_Key, PTSTSD_DESTRUCTOR *pDestructor);
inline PTBOOL PTSTSD_SetValue(PTSTSD_KEY TSD_Key, PTSTSD_VALUE TSD_Value);
inline PTSTSD_VALUE PTSTSD_GetValue(PTSTSD_KEY TSD_Key);
inline PTBOOL PTSTSD_Delete(PTSTSD_KEY TSD_Key);

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

inline int32_t PTSAtomic_GetAndSet(int32_t volatile *pTarget, int32_t newValue);
inline int64_t PTSAtomic_GetAndSet(int64_t volatile *pTarget, int64_t newValue);
inline uint32_t PTSAtomic_GetAndSet(uint32_t volatile *pTarget, uint32_t newValue);
inline uint64_t PTSAtomic_GetAndSet(uint64_t volatile *pTarget, uint64_t newValue);

template <typename T>
inline T* PTSAtomic_GetAndSet(T* *pTarget, T* newValue)
{
	return reinterpret_cast<T*>(::PTSAtomic_GetAndSet(reinterpret_cast<uintptr_t *>(pTarget), reinterpret_cast<uintptr_t>(newValue)));
}

inline int32_t PTSAtomic_GetAndAdd(int32_t volatile *pTarget, int32_t delta);
inline int64_t PTSAtomic_GetAndAdd(int64_t volatile *pTarget, int64_t delta);
inline uint32_t PTSAtomic_GetAndAdd(uint32_t volatile *pTarget, uint32_t delta);
inline uint64_t PTSAtomic_GetAndAdd(uint64_t volatile *pTarget, uint64_t delta);

inline int32_t PTSAtomic_Get(int32_t volatile *pTarget);
inline int64_t PTSAtomic_Get(int64_t volatile *pTarget);
inline uint32_t PTSAtomic_Get(uint32_t volatile *pTarget);
inline uint64_t PTSAtomic_Get(uint64_t volatile *pTarget);

template <typename T>
inline T* PTSAtomic_Get(T* *pTarget)
{
	return reinterpret_cast<T*>(::PTSAtomic_Get(reinterpret_cast<uintptr_t *>(pTarget)));
}

inline void PTSAtomic_Set(int32_t volatile *pTarget, int32_t newValue);
inline void PTSAtomic_Set(int64_t volatile *pTarget, int64_t newValue);
inline void PTSAtomic_Set(uint32_t volatile *pTarget, uint32_t newValue);
inline void PTSAtomic_Set(uint64_t volatile *pTarget, uint64_t newValue);

template <typename T>
inline void PTSAtomic_Set(T* *pTarget, T* newValue)
{
	::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(pTarget), reinterpret_cast<uintptr_t>(newValue));
}

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

