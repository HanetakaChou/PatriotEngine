#include <assert.h>
#include <stdlib.h>

#include "PTSConvUTF.h"

inline PTBOOL PTCALL PTSThread_Create(PTSThreadEntry *pThreadEntry, void *pThreadParam, PTSThread *pThreadOut)
{
	HANDLE hThread = reinterpret_cast<HANDLE>(::_beginthreadex(NULL, 0U, pThreadEntry, pThreadParam, 0U, NULL));
	assert(hThread != NULL);

	(*pThreadOut) = hThread;

	return (hThread != NULL) ? PTTRUE : PTFALSE;
}

inline PTBOOL PTCALL PTSThread_Detach(PTSThread *pThread)
{
	HANDLE hThread = *pThread;
	return (::CloseHandle(hThread) != FALSE) ? PTTRUE : PTFALSE;
}

inline PTBOOL PTCALL PTSThread_Join(PTSThread *pThread)
{
	HANDLE hThread = *pThread;
	return ((::WaitForSingleObjectEx(hThread, INFINITE, FALSE) == WAIT_OBJECT_0) && (::CloseHandle(hThread) != FALSE)) ? PTTRUE : PTFALSE;
}

inline PTBOOL PTCALL PTS_Yield()
{
	return (::SwitchToThread() != FALSE) ? PTTRUE : PTFALSE;
}

inline PTSThreadID PTCALL PTSThreadID_Self()
{
	return ::GetCurrentThreadId();
}

inline PTBOOL PTCALL PTSThreadID_Equal(PTSThreadID TID1, PTSThreadID TID2)
{
	return (TID1 == TID2) ? PTTRUE : PTFALSE;
}

inline PTBOOL PTCALL PTSSemaphore_Create(uint32_t iInitialValue, PTSSemaphore *pSemaphoreOut)
{
	HANDLE hSemaphore = ::CreateSemaphoreExW(NULL, iInitialValue, 32767, NULL, 0U, DELETE | SYNCHRONIZE | SEMAPHORE_MODIFY_STATE);//#define _POSIX_SEM_VALUE_MAX  32767 //与Posix一致
	assert(hSemaphore != NULL);

	(*pSemaphoreOut) = hSemaphore;

	return(hSemaphore != NULL) ? PTTRUE : PTFALSE;
}

inline PTBOOL PTCALL PTSSemaphore_Passern(PTSSemaphore *pSemaphore)
{
	HANDLE hSemaphore = *pSemaphore;
	return (::WaitForSingleObjectEx(hSemaphore, INFINITE, FALSE) == WAIT_OBJECT_0) ? PTTRUE : PTFALSE;
}

inline PTBOOL PTCALL PTSSemaphore_Vrijgeven(PTSSemaphore *pSemaphore)
{
	HANDLE hSemaphore = *pSemaphore;
	return (::ReleaseSemaphore(hSemaphore, 1, NULL) != FALSE) ? PTTRUE : PTFALSE;
}

inline PTBOOL PTCALL PTSSemaphore_Delete(PTSSemaphore *pSemaphore)
{
	HANDLE hSemaphore = *pSemaphore;
	return (::CloseHandle(hSemaphore) != FALSE) ? PTTRUE : PTFALSE;
}

inline PTBOOL PTCALL PTSTSD_Create(PTSTSD_KEY *pTSD_Key, PTSTSD_DESTRUCTOR *pDestructor)
{
	(*pTSD_Key) = ::FlsAlloc(pDestructor);
	return ((*pTSD_Key) != FLS_OUT_OF_INDEXES) ? PTTRUE : PTFALSE;
}

inline PTBOOL PTCALL PTSTSD_SetValue(PTSTSD_KEY TSD_Key, PTSTSD_VALUE TSD_Value)
{
	BOOL wbResult = ::FlsSetValue(TSD_Key, TSD_Value);
	return (wbResult != FALSE) ? PTTRUE : PTFALSE;
}

inline PTSTSD_VALUE PTCALL PTSTSD_GetValue(PTSTSD_KEY TSD_Key)
{
	return ::FlsGetValue(TSD_Key);
}

inline PTBOOL PTCALL PTSTSD_Delete(PTSTSD_KEY TSD_Key)
{
	BOOL wbResult = ::FlsFree(TSD_Key);
	return (wbResult != FALSE) ? PTTRUE : PTFALSE;
}

#ifdef PTSYSTEM_SOCKET

inline PTBOOL PTCALL PTSocketCreateTCP(PTSocket *pSocketOut)
{
	SOCKET hSocket = ::WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0U, 0U);	
	assert(hSocket != INVALID_SOCKET);

	(*pSocketOut) = static_cast<uintptr_t>(hSocket);

	return(hSocket != INVALID_SOCKET) ? PTTRUE : PTFALSE;
}

inline PTBOOL PTCALL PTSocketCreateUDP(PTSocket *pSocketOut)
{
	SOCKET hSocket = ::WSASocketW(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0U, 0U);
	assert(hSocket != INVALID_SOCKET);

	(*pSocketOut) = static_cast<uintptr_t>(hSocket);

	return(hSocket != INVALID_SOCKET) ? PTTRUE : PTFALSE;
}

inline PTBOOL PTCALL PTSocketControlNonBlock(PTSocket *pSocket, PTBOOL bNonBlock)
{
	SOCKET hSocket = static_cast<SOCKET>(*pSocket);

	//Overlapped I/O 不同于 NonBlock I/O
	unsigned long bNonBlocking = (bNonBlock != PTFALSE) ? 1 : 0;
	DWORD cbBytesReturned;
	int iResult = ::WSAIoctl(hSocket, FIONBIO, &bNonBlocking, sizeof(unsigned long), NULL, 0U, &cbBytesReturned, NULL, NULL);
	assert(iResult == 0);

	return(iResult == 0) ? PTTRUE : PTFALSE;
}

inline PTBOOL PTCALL PTSocketDestory(PTSocket *pSocket)
{
	SOCKET hSocket = static_cast<SOCKET>(*pSocket);

	int iResult = ::closesocket(hSocket);
	assert(iResult == 0);
	//在不设置SO_LINGER的情况下不会阻塞

	return(iResult == 0) ? PTTRUE : PTFALSE;
}

inline PTBOOL PTCALL PTSocketBindIPV4(PTSocket *pSocket, uint8_t AddressIPV4[4], uint16_t Port)
{
	SOCKET hSocket = static_cast<SOCKET>(*pSocket);

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
		assert(::WSAGetLastError() == WSAEADDRINUSE);
		return PTFALSE;
	}
	else
	{
		return PTTRUE;
	}
}

inline PTBOOL PTCALL PTSocketTCPListen(PTSocket *pSocket)
{
	SOCKET hSocket = static_cast<SOCKET>(*pSocket);

	int iResult = ::listen(hSocket, SOMAXCONN);
	assert(iResult == 0);

	return(iResult == 0) ? PTTRUE : PTFALSE;
}

inline PTBOOL PTCALL PTSocketTCPAccept(PTSocket *pSocketListen, PTSocket *pSocketAcceptOut)
{
	SOCKET hSocketListen = static_cast<SOCKET>(*pSocketListen);

	SOCKET hSocketAcceptOut = ::WSAAccept(hSocketListen, NULL, NULL, NULL, NULL);
	
	(*pSocketAcceptOut) = static_cast<uintptr_t>(hSocketAcceptOut);

	if (hSocketAcceptOut == INVALID_SOCKET)
	{
		assert(::WSAGetLastError() == WSAEWOULDBLOCK);
		return PTFALSE;
	}
	else
	{
		return PTTRUE;
	}
}

inline PTBOOL PTCALL PTSocketTCPConnectIPV4(PTSocket *pSocket, uint8_t AddressIPV4[4], uint16_t Port)
{
	SOCKET hSocket = static_cast<SOCKET>(*pSocket);

	sockaddr_in AddressSocket;
	AddressSocket.sin_family = AF_INET;
	uint8_t *pUByte = reinterpret_cast<uint8_t *>(&AddressSocket.sin_addr);
	pUByte[0] = AddressIPV4[0];
	pUByte[1] = AddressIPV4[1];
	pUByte[2] = AddressIPV4[2];
	pUByte[3] = AddressIPV4[3];
	AddressSocket.sin_port = ::htons(Port);
	if (::WSAConnect(hSocket, reinterpret_cast<sockaddr *>(&AddressSocket), sizeof(sockaddr), NULL, NULL, NULL, NULL) != 0)
	{
		int WSAErrorCode = ::WSAGetLastError();
		if (WSAErrorCode == WSAEISCONN)
		{
			return PTTRUE;
		}
		else
		{
			assert(WSAErrorCode == WSAEWOULDBLOCK || WSAErrorCode == WSAEALREADY);
			return PTFALSE;
		}
	}
	else
	{
		return PTTRUE;
	}
}

inline PTBOOL PTCALL PTSocketTCPShutdown(PTSocket *pSocket)
{
	SOCKET hSocket = static_cast<SOCKET>(*pSocket);

	int iResult = ::shutdown(hSocket, SD_SEND);
	assert(iResult == 0);

	return(iResult == 0) ? PTTRUE : PTFALSE;
}

inline PTBOOL PTCALL PTSocketRead(PTSocket *pSocket, void *pBuffer, uint32_t NumberOfBytesToRead, uint32_t *pNumberOfBytesRead)
{
	SOCKET hSocket = static_cast<SOCKET>(*pSocket);

	WSABUF IOVector[1];
	IOVector[0].len = static_cast<ULONG>(NumberOfBytesToRead);
	IOVector[0].buf = static_cast<CHAR *>(pBuffer);
	DWORD NumberOfBytesRecvd;
	DWORD Flags = 0U;
	if (::WSARecv(hSocket, IOVector, 1U, &NumberOfBytesRecvd, &Flags, NULL, NULL) != 0)
	{
		assert(::WSAGetLastError() == WSAEWOULDBLOCK);
		return PTFALSE;
	}
	else
	{
		//NumberOfBytesRecvd为0表示对端已经关闭写入（即Shutdown或Close）
		*pNumberOfBytesRead = NumberOfBytesRecvd;
		return PTTRUE;
	}
}

inline PTBOOL PTCALL PTSocketWrite(PTSocket *pSocket, void *pBuffer, uint32_t NumberOfBytesToWrite, uint32_t *pNumberOfBytesWritten)
{
	SOCKET hSocket = static_cast<SOCKET>(*pSocket);

	WSABUF IOVector[1];
	IOVector[0].len = static_cast<ULONG>(NumberOfBytesToWrite);
	IOVector[0].buf = static_cast<CHAR *>(pBuffer);
	DWORD NumberOfBytesSent;
	if (::WSASend(hSocket, IOVector, 1U, &NumberOfBytesSent, 0U, NULL, NULL) != 0)
	{
		//即使是非阻塞I/O也不应当出现WSAEWOULDBLOCK
		int ErrorCode = ::WSAGetLastError();
		assert(ErrorCode == WSAECONNABORTED || ErrorCode == WSAECONNRESET);//Posix下返回EPIPE且产生SIGPIPE信号
		return PTFALSE;
	}
	else
	{
		*pNumberOfBytesWritten = NumberOfBytesSent;
		return PTTRUE;
	}
}

#endif

inline int64_t PTCALL PTSTick_Count()
{
	LARGE_INTEGER PerformanceCount;
	BOOL bResult = ::QueryPerformanceCounter(&PerformanceCount);
	assert(bResult != FALSE);
	return PerformanceCount.QuadPart;
}

inline int64_t PTCALL PTSTick_Frequency()
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
inline void PTCALL PTS_Pause()
{
	::_mm_pause();
}
#else
#error 未知的架构
#endif

inline int32_t PTCALL PTSAtomic_CompareAndSetI32(int32_t volatile *pTarget, int32_t expect, int32_t update)
{ 
	return ::_InterlockedCompareExchange(reinterpret_cast<LONG volatile *>(pTarget), update, expect);
}
inline int64_t PTCALL PTSAtomic_CompareAndSetI64(int64_t volatile *pTarget, int64_t expect, int64_t update)
{
	return ::_InterlockedCompareExchange64(reinterpret_cast<LONGLONG volatile *>(pTarget), update, expect);
}
inline uint32_t PTCALL PTSAtomic_CompareAndSetUI32(uint32_t volatile *pTarget, uint32_t expect, uint32_t update)
{
	return static_cast<uint32_t>(::_InterlockedCompareExchange(reinterpret_cast<LONG volatile *>(pTarget), static_cast<LONG>(update), static_cast<LONG>(expect)));
}
inline uint64_t PTCALL PTSAtomic_CompareAndSetUI64(uint64_t volatile *pTarget, uint64_t expect, uint64_t update)
{
	return static_cast<uint64_t>(::_InterlockedCompareExchange64(reinterpret_cast<LONGLONG volatile *>(pTarget), static_cast<LONGLONG>(update), static_cast<LONGLONG>(expect)));
}

inline int32_t PTCALL PTSAtomic_GetAndSetI32(int32_t volatile *pTarget, int32_t newValue)
{
	return ::_InterlockedExchange(reinterpret_cast<LONG volatile *>(pTarget), newValue);
}
inline int64_t PTCALL PTSAtomic_GetAndSetI64(int64_t volatile *pTarget, int64_t newValue)
{
	return ::InterlockedExchange64(reinterpret_cast<LONGLONG volatile *>(pTarget), newValue);

}
inline uint32_t PTCALL PTSAtomic_GetAndSetUI32(uint32_t volatile *pTarget, uint32_t newValue)
{
	return static_cast<uint32_t>(::_InterlockedExchange(reinterpret_cast<LONG volatile *>(pTarget), static_cast<LONG>(newValue)));
}
inline uint64_t PTCALL PTSAtomic_GetAndSetUI64(uint64_t volatile *pTarget, uint64_t newValue)
{
	return static_cast<uint64_t>(::InterlockedExchange64(reinterpret_cast<LONGLONG volatile *>(pTarget), static_cast<LONGLONG>(newValue)));
}

inline int32_t PTCALL PTSAtomic_GetAndAddI32(int32_t volatile *pTarget, int32_t delta)
{
	return ::_InterlockedExchangeAdd(reinterpret_cast<LONG volatile *>(pTarget), delta);
}

inline int64_t PTCALL PTSAtomic_GetAndAddI64(int64_t volatile *pTarget, int64_t delta)
{
	return ::_InterlockedExchangeAdd64(reinterpret_cast<LONGLONG volatile *>(pTarget), delta);
}

inline uint32_t PTCALL PTSAtomic_GetAndAddUI32(uint32_t volatile *pTarget, uint32_t delta)
{
	return static_cast<uint32_t>(::_InterlockedExchangeAdd(reinterpret_cast<LONG volatile *>(pTarget), static_cast<LONG>(delta)));
}

inline uint64_t PTCALL PTSAtomic_GetAndAddUI64(uint64_t volatile *pTarget, uint64_t delta)
{

	return static_cast<uint64_t>(::_InterlockedExchangeAdd64(reinterpret_cast<LONGLONG volatile *>(pTarget), static_cast<LONGLONG>(delta)));
}

inline int32_t PTCALL PTSAtomic_GetI32(int32_t volatile *pTarget)
{
	int32_t value = *pTarget;
	::_ReadBarrier();
	return value;
}
inline int64_t PTCALL PTSAtomic_GetI64(int64_t volatile *pTarget)
{
	int64_t value = *pTarget;
	::_ReadBarrier();
	return value;
}
inline uint32_t PTCALL PTSAtomic_GetUI32(uint32_t volatile *pTarget)
{
	uint32_t value = *pTarget;
	::_ReadBarrier();
	return value;
}
inline uint64_t PTCALL PTSAtomic_GetUI64(uint64_t volatile *pTarget)
{
	uint64_t value = *pTarget;
	::_ReadBarrier();
	return value;
}

inline void PTCALL PTSAtomic_SetI32(int32_t volatile *pTarget, int32_t newValue)
{
	::_WriteBarrier();
	(*pTarget) = newValue;
}

inline void PTCALL PTSAtomic_SetI64(int64_t volatile *pTarget, int64_t newValue)
{
	::_WriteBarrier();
	(*pTarget) = newValue;
}

inline void PTCALL PTSAtomic_SetUI32(uint32_t volatile *pTarget, uint32_t newValue)
{
	::_WriteBarrier();
	(*pTarget) = newValue;
}

inline void PTCALL PTSAtomic_SetUI64(uint64_t volatile *pTarget, uint64_t newValue)
{
	::_WriteBarrier();
	(*pTarget) = newValue;
}
