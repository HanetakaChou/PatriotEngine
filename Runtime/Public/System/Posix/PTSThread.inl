#include <assert.h>
#include <errno.h>

PTBOOL PTCALL PTThreadCreate(PTThreadEntry *pThreadEntry, void *pThreadParam, PTThread *pThreadOut)
{
	return (::pthread_create(pThreadOut, NULL, pThreadEntry, pThreadParam) == 0) ? PTTRUE : PTFALSE;
}

PTBOOL PTCALL PTThreadWait(PTThread *pThread)
{
	return (::pthread_join(*pThread, NULL) == 0) ? PTTRUE : PTFALSE;
}

PTBOOL PTCALL PTS_Yield()
{
	return (::sched_yield() == 0) ? PTTRUE : PTFALSE;
}

PTSThreadID PTCALL PTSThreadID_Self()
{
	return ::pthread_self();
}

PTBOOL PTCALL PTSThreadID_Equal(PTSThreadID TID1, PTSThreadID TID2)
{
	return (::pthread_equal(TID1, TID2) != 0) ? PTTRUE : PTFALSE;
}

PTBOOL PTCALL PTSpinLockCreate(PTSpinLock *pSpinLock)
{
	pSpinLock->bIsLocked = PTFALSE;
	return PTTRUE;
}

PTBOOL PTCALL PTSpinLockEnter(PTSpinLock *pSpinLock)
{
	while (::__sync_lock_test_and_set(&pSpinLock->bIsLocked, PTTRUE) != PTFALSE)
	{
		::PTS_Yield();
	}
	return PTTRUE;
}

PTBOOL PTCALL PTSpinLockLeave(PTSpinLock *pSpinLock)
{
	PTBOOL ptbResult = ::__sync_lock_test_and_set(&pSpinLock->bIsLocked, PTFALSE);
	assert(ptbResult == PTTRUE);

	return PTTRUE;
}

PTBOOL PTCALL PTSpinLockDestory(PTSpinLock *pSpinLock)
{
	return PTTRUE;
}

PTBOOL PTCALL PTSemaphoreCreate(uint32_t iInitialValue, PTSemaphore *pSemaphoreOut)
{
	return (::sem_init(pSemaphoreOut, 0, iInitialValue) == 0) ? PTTRUE : PTFALSE;
}

PTBOOL PTCALL PTSemaphorePassern(PTSemaphore *pSemaphore)
{
	return (::sem_wait(pSemaphore) == 0) ? PTTRUE : PTFALSE;
}

PTBOOL PTCALL PTSemaphoreVrijgeven(PTSemaphore *pSemaphore)
{
	return (::sem_post(pSemaphore) == 0) ? PTTRUE : PTFALSE;
}

PTBOOL PTCALL PTSemaphoreDestory(PTSemaphore *pSemaphore)
{
	return (::sem_destroy(pSemaphore) == 0) ? PTTRUE : PTFALSE;
}

PTBOOL PTCALL PTSTSD_Create(PTSTSD_KEY *pTSD_Key, PTSTSD_DESTRUCTOR *pDestructor)
{
	int iResult = ::pthread_key_create(pTSD_Key, pDestructor);
	return (iResult == 0) ? PTTRUE : PTFALSE;
}

PTBOOL PTCALL PTSTSD_SetValue(PTSTSD_KEY TSD_Key, PTSTSD_VALUE TSD_Value)
{
	int iResult = ::pthread_setspecific(TSD_Key, TSD_Value);
	return (iResult == 0) ? PTTRUE : PTFALSE;
}

inline PTSTSD_VALUE PTCALL PTSTSD_GetValue(PTSTSD_KEY TSD_Key)
{
	return ::pthread_getspecific(TSD_Key);
}

PTBOOL PTCALL PTSTSD_Delete(PTSTSD_KEY TSD_Key)
{
	int iResult = ::pthread_key_delete(TSD_Key);
	return (iResult == 0) ? PTTRUE : PTFALSE;
}

#ifdef PTSYSTEM_SOCKET

PTBOOL PTCALL PTSocketCreateTCP(PTSocket *pSocketOut)
{
	int hSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	assert(hSocket != -1);

	(*pSocketOut) = hSocket;

	return(hSocket != -1) ? PTTRUE : PTFALSE;
}

PTBOOL PTCALL PTSocketCreateUDP(PTSocket *pSocketOut)
{
	int hSocket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	assert(hSocket != -1);

	(*pSocketOut) = hSocket;

	return(hSocket != -1) ? PTTRUE : PTFALSE;
}

PTBOOL PTCALL PTSocketControlNonBlock(PTSocket *pSocket, PTBOOL bNonBlock)
{
	int hSocket = *pSocket;

	//Overlapped I/O 不同于 NonBlock I/O

	int FileFlags = ::fcntl(hSocket, F_GETFL);
	assert(FileFlags != -1);
	if (FileFlags == -1)
	{
		return PTFALSE;
	}

	if (bNonBlock)
	{
		FileFlags |= O_NONBLOCK;
	}
	else
	{
		FileFlags &= ~O_NONBLOCK;
	}

	int iResult = fcntl(hSocket, F_SETFL, FileFlags);
	assert(iResult != -1);

	return(iResult != -1) ? PTTRUE : PTFALSE;
}

PTBOOL PTCALL PTSocketBindIPV4(PTSocket *pSocket, uint8_t AddressIPV4[4], uint16_t Port)
{
	int hSocket = *pSocket;

	sockaddr_in AddressSocket;
	AddressSocket.sin_family = AF_INET;
	uint8_t *pUByte = reinterpret_cast<uint8_t *>(&AddressSocket.sin_addr);
	pUByte[0] = AddressIPV4[0];
	pUByte[1] = AddressIPV4[1];
	pUByte[2] = AddressIPV4[2];
	pUByte[3] = AddressIPV4[3];
	AddressSocket.sin_port = ::htons(Port);
	if (::bind(hSocket, reinterpret_cast<sockaddr *>(&AddressSocket), sizeof(sockaddr)) != 0)
	{
		assert(errno == EADDRINUSE);
		return PTFALSE;
	}
	else
	{
		return PTTRUE;
	}
}

PTBOOL PTCALL PTSocketDestory(PTSocket *pSocket)
{
	int hSocket = *pSocket;

	int iResult = ::close(hSocket);
	assert(iResult == 0);
	//在不设置SO_LINGER的情况下不会阻塞

	return(iResult == 0) ? PTTRUE : PTFALSE;
}

PTBOOL PTCALL PTSocketTCPListen(PTSocket *pSocket)
{
	int hSocket = *pSocket;

	int iResult = ::listen(hSocket, SOMAXCONN);
	assert(iResult == 0);

	return(iResult == 0) ? PTTRUE : PTFALSE;
}

PTBOOL PTCALL PTSocketTCPAccept(PTSocket *pSocketListen, PTSocket *pSocketAcceptOut)
{
	int hSocketListen = *pSocketListen;

	int hSocketAcceptOut = ::accept(hSocketListen, NULL, NULL);

	(*pSocketAcceptOut) = hSocketAcceptOut;

	if (hSocketAcceptOut == -1)
	{
		assert(errno == EAGAIN || errno == EWOULDBLOCK);
		return PTFALSE;
	}
	else
	{
		return PTTRUE;
	}
}

PTBOOL PTCALL PTSocketTCPConnectIPV4(PTSocket *pSocket, uint8_t AddressIPV4[4], uint16_t Port)
{
	int hSocket = *pSocket;

	sockaddr_in AddressSocket;
	AddressSocket.sin_family = AF_INET;
	uint8_t *pUByte = reinterpret_cast<uint8_t *>(&AddressSocket.sin_addr);
	pUByte[0] = AddressIPV4[0];
	pUByte[1] = AddressIPV4[1];
	pUByte[2] = AddressIPV4[2];
	pUByte[3] = AddressIPV4[3];
	AddressSocket.sin_port = ::htons(Port);
	if (::connect(hSocket, reinterpret_cast<sockaddr *>(&AddressSocket), sizeof(sockaddr)) != 0)
	{
		if (errno == EISCONN)
		{
			return PTTRUE;
		}
		else
		{
			assert(errno == EAGAIN || errno == EWOULDBLOCK || errno == EALREADY);
			return PTFALSE;
		}
	}
	else
	{
		return PTTRUE;
	}
}

PTBOOL PTCALL PTSocketTCPShutdown(PTSocket *pSocket)
{
	int hSocket = *pSocket;

	int iResult = ::shutdown(hSocket, SHUT_WR);
	assert(iResult == 0);

	return(iResult == 0) ? PTTRUE : PTFALSE;
}

PTBOOL PTCALL PTSocketRead(PTSocket *pSocket, void *pBuffer, uint32_t NumberOfBytesToRead, uint32_t *pNumberOfBytesRead)
{
	int hSocket = *pSocket;

	ssize_t NumberOfBytesRecvd = ::recv(hSocket, pBuffer, NumberOfBytesToRead, 0);

	if (NumberOfBytesRecvd == -1)
	{
		assert(errno == EAGAIN || errno == EWOULDBLOCK);
		return PTFALSE;
	}
	else
	{
		//NumberOfBytesRecvd为0表示对端已经关闭写入（即Shutdown或Close）
		*pNumberOfBytesRead = static_cast<uint32_t>(NumberOfBytesRecvd);
		return PTTRUE;
	}
}

PTBOOL PTCALL PTSocketWrite(PTSocket *pSocket, void *pBuffer, uint32_t NumberOfBytesToWrite, uint32_t *pNumberOfBytesWritten)
{
	int hSocket = *pSocket;

	ssize_t NumberOfBytesSent = ::send(hSocket, pBuffer, NumberOfBytesToWrite, 0);
	
	if (NumberOfBytesSent == -1)
	{
		//即使是非阻塞I/O也不应当出现WSAEWOULDBLOCK
		//屏蔽信号
		//Posix下返回EPIPE且产生SIGPIPE信号
		assert(errno == ECONNABORTED || errno == ECONNRESET);
		return PTFALSE;
	}
	else
	{
		*pNumberOfBytesWritten = static_cast<uint32_t>(NumberOfBytesSent);
		return PTTRUE;
	}
}

#endif

inline int64_t PTTickCount()
{
	timeval tv;
	int rtval = ::gettimeofday(&tv, NULL);
	assert(rtval == 0);
	return 1000000LL * static_cast<int64_t>(tv.tv_sec) + static_cast<int64_t>(tv.tv_usec);
}

inline int64_t PTTickFrequency()
{
	return 1000000LL;
}

#if defined(PTARM) || defined(PTARM64)
//#include <arm_acle.h> __yield
inline void PTCALL PTS_Pause()
{
	__builtin_arm_yield();
}
#elif defined(PTX86) || defined(PTX64)
inline void PTCALL PTS_Pause()
{
	__builtin_ia32_pause();
}
#else
#error 未知的架构
#endif

inline int32_t PTCALL PTSAtomic_CompareAndSetI32(int32_t volatile *pTarget, int32_t expect, int32_t update)
{
	return ::__sync_val_compare_and_swap(pTarget, expect, update);
}
inline int64_t PTCALL PTSAtomic_CompareAndSetI64(int64_t volatile *pTarget, int64_t expect, int64_t update)
{
	return ::__sync_val_compare_and_swap(pTarget, expect, update);
}
inline uint32_t PTCALL PTSAtomic_CompareAndSetUI32(uint32_t volatile *pTarget, uint32_t expect, uint32_t update)
{
	return ::__sync_val_compare_and_swap(pTarget, expect, update);
}
inline uint64_t PTCALL PTSAtomic_CompareAndSetUI64(uint64_t volatile *pTarget, uint64_t expect, uint64_t update)
{
	return ::__sync_val_compare_and_swap(pTarget, expect, update);
}

inline int32_t PTCALL PTSAtomic_GetAndSetI32(int32_t volatile *pTarget, int32_t newValue)
{
	return ::__sync_lock_test_and_set(pTarget, newValue);
}
inline int64_t PTCALL PTSAtomic_GetAndSetI64(int64_t volatile *pTarget, int64_t newValue)
{
	return ::__sync_lock_test_and_set(pTarget, newValue);

}
inline uint32_t PTCALL PTSAtomic_GetAndSetUI32(uint32_t volatile *pTarget, uint32_t newValue)
{
	return ::__sync_lock_test_and_set(pTarget, newValue);
}
inline uint64_t PTCALL PTSAtomic_GetAndSetUI64(uint64_t volatile *pTarget, uint64_t newValue)
{
	return ::__sync_lock_test_and_set(pTarget, newValue);
}


inline int32_t PTCALL PTSAtomic_GetAndAddI32(int32_t volatile *pTarget, int32_t delta)
{
	return ::__sync_fetch_and_add(pTarget, delta);
}
inline int64_t PTCALL PTSAtomic_GetAndAddI64(int64_t volatile *pTarget, int64_t delta)
{
	return ::__sync_fetch_and_add(pTarget, delta);
}
inline uint32_t PTCALL PTSAtomic_GetAndAddUI32(uint32_t volatile *pTarget, uint32_t delta)
{
	return ::__sync_fetch_and_add(pTarget, delta);
}
inline uint64_t PTCALL PTSAtomic_GetAndAddUI64(uint64_t volatile *pTarget, uint64_t delta)
{
	return ::__sync_fetch_and_add(pTarget, delta);
}


inline int32_t PTCALL PTSAtomic_GetI32(int32_t volatile *pTarget)
{
	return ::__atomic_load_n(pTarget, __ATOMIC_ACQUIRE);
}
inline int64_t PTCALL PTSAtomic_GetI64(int64_t volatile *pTarget)
{
	return ::__atomic_load_n(pTarget, __ATOMIC_ACQUIRE);
}
inline uint32_t PTCALL PTSAtomic_GetUI32(uint32_t volatile *pTarget)
{
	return ::__atomic_load_n(pTarget, __ATOMIC_ACQUIRE);
}
inline uint64_t PTCALL PTSAtomic_GetUI64(uint64_t volatile *pTarget)
{
	return ::__atomic_load_n(pTarget, __ATOMIC_ACQUIRE);
}


inline void PTCALL PTSAtomic_SetI32(int32_t volatile *pTarget, int32_t newValue)
{
	::__atomic_store_n(pTarget, newValue, __ATOMIC_RELEASE);
}
inline void PTCALL PTSAtomic_SetI64(int64_t volatile *pTarget, int64_t newValue)
{
	::__atomic_store_n(pTarget, newValue, __ATOMIC_RELEASE);
}
inline void PTCALL PTSAtomic_SetUI32(uint32_t volatile *pTarget, uint32_t newValue)
{
	::__atomic_store_n(pTarget, newValue, __ATOMIC_RELEASE);
}
inline void PTCALL PTSAtomic_SetUI64(uint64_t volatile *pTarget, uint64_t newValue)
{
	::__atomic_store_n(pTarget, newValue, __ATOMIC_RELEASE);
}