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

inline bool mcrt_thread_setname(PTSThread *pThread, char const *name)
{
	HANDLE hThread = *pThread;
	LPCSTR threadName = name;

	//https://github.com/MicrosoftDocs/visualstudio-docs/blob/master/docs/debugger/how-to-set-a-thread-name-in-native-code.md#set-a-thread-name-by-throwing-an-exception

	DWORD const MS_VC_EXCEPTION = 0X406D1388;

	typedef struct tagTHREADNAME_INFO
	{
		DWORD dwType;	  // Must be 0x1000.
		LPCSTR szName;	  // Pointer to name (in user addr space).
		DWORD dwThreadID; // Thread ID (-1=caller thread).
		DWORD dwFlags;	  // Reserved for future use, must be zero.
	} THREADNAME_INFO;

	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = GetThreadId(hThread);
	info.dwFlags = 0;

	__try
	{
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR *)&info);
	}
	__except ((GetExceptionCode() == MS_VC_EXCEPTION) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{
		//Do Nothing
	}

	return true;
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

mcrt_event::mcrt_event()
#ifndef NDEBUG
	: m_is_valid(false)
#endif
{
}

void mcrt_event::create_manual_event(bool initial_state)
{
	m_manual_reset = true;
	m_state = initial_state;
	initialize();
}

void mcrt_event::create_auto_event(bool initial_state)
{
	m_manual_reset = false;
	m_state = initial_state;
	initialize();
}

void mcrt_event::initialize()
{
	assert(!m_is_valid);

	BOOL st = InitializeCriticalSectionAndSpinCount(&m_mutex);
	assert(st != FALSE);

	InitializeConditionVariable(&m_condition);

#ifndef NDEBUG
	m_is_valid = true;
#endif

	return true;
}

void mcrt_event::close_event()
{
	assert(m_is_valid);

	DeleteCriticalSection(&m_mutex);

#ifndef NDEBUG
	m_is_valid = false;
#endif
}

inline void mcrt_event::wait()
{
	assert(m_is_valid);

	EnterCriticalSection(&m_mutex);

	BOOL st;

	do
	{
		st = SleepConditionVariableCS(&m_condition, &m_mutex, INFINITE);
	} while ((st != FALSE) && (!m_state));

	if ((st != FALSE) && !m_manual_reset)
	{
		// Clear the state for auto-reset events so that only one waiter gets released
		m_state = false;
	}

	LeaveCriticalSection(&m_mutex);

	assert(st != FALSE);
}

inline bool mcrt_event::wait_time(uint32_t timeout_milliseconds)
{
	assert(m_is_valid);

	EnterCriticalSection(&m_mutex);

	BOOL st;

	do
	{
		st = SleepConditionVariableCS(&m_condition, &m_mutex, timeout_milliseconds);
	} while ((st != FALSE) && (!m_state));

	if ((st != FALSE) && !m_manual_reset)
	{
		// Clear the state for auto-reset events so that only one waiter gets released
		m_state = false;
	}

	LeaveCriticalSection(&m_mutex);

	bool wait_status;

	if (st != FALSE)
	{
		wait_status = true;
	}
	else
	{
		assert(GetLastError() == ERROR_TIMEOUT);
		wait_status = false;
	}

	return wait_status;
}

inline void mcrt_event::set()
{
	assert(m_is_valid);

	EnterCriticalSection(&m_mutex);
	m_state = true;
	LeaveCriticalSection(&m_mutex);

	// Unblock all threads waiting for the condition variable
	WakeAllConditionVariable(&m_condition);
}

inline void mcrt_event::reset()
{
	assert(m_is_valid);

	EnterCriticalSection(&m_mutex);
	m_state = false;
	LeaveCriticalSection(&m_mutex);
}

inline bool PTSSemaphore_Create(uint32_t iInitialValue, PTSSemaphore *pSemaphoreOut)
{
	HANDLE hSemaphore = ::CreateSemaphoreExW(NULL, iInitialValue, 32767, NULL, 0U, DELETE | SYNCHRONIZE | SEMAPHORE_MODIFY_STATE); //#define _POSIX_SEM_VALUE_MAX  32767 //与Posix一致
	assert(hSemaphore != NULL);

	(*pSemaphoreOut) = hSemaphore;

	return (hSemaphore != NULL) ? true : false;
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