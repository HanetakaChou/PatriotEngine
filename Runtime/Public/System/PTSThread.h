#ifndef PT_SYSTEM_THREAD_H
#define PT_SYSTEM_THREAD_H

#include "../PTCommon.h"
#include "PTSCommon.h"

#ifdef PTWIN32
#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <process.h>
typedef unsigned __stdcall PTThreadEntry(void *pThreadParam);
typedef HANDLE PTThread;
#elif defined(PTPOSIX)
#include <pthread.h>
#include <sched.h>
typedef void * PTThreadEntry(void *);
typedef pthread_t PTThread;
#else
#error 未知的平台
#endif
inline PTBOOL PTCALL PTThreadCreate(PTThreadEntry *, void *pThreadParam, PTThread *pThreadOut);
inline PTBOOL PTCALL PTThreadWait(PTThread *pThread);

inline PTBOOL PTCALL PTS_Yield();

#ifdef PTWIN32
typedef DWORD PTSThreadID;
#elif defined(PTPOSIX)
typedef pthread_t PTSThreadID;
#else
#error 未知的平台
#endif

inline PTSThreadID PTCALL PTSThreadID_Self();
inline PTBOOL PTCALL PTSThreadID_Equal(PTSThreadID TID1, PTSThreadID TID2);

#ifdef PTWIN32
typedef HANDLE PTSSemaphore;
#elif defined(PTPOSIX)
#include <semaphore.h>
typedef sem_t PTSSemaphore;
#else
#error 未知的平台
#endif
inline PTBOOL PTCALL PTSSemaphore_Create(uint32_t iInitialValue, PTSSemaphore *pSemaphoreOut);
inline PTBOOL PTCALL PTSSemaphore_Passern(PTSSemaphore *pSemaphore);
inline PTBOOL PTCALL PTSSemaphore_Vrijgeven(PTSSemaphore *pSemaphore);
inline PTBOOL PTCALL PTSSemaphore_Delete(PTSSemaphore *pSemaphore);

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

inline PTBOOL PTCALL PTSTSD_Create(PTSTSD_KEY *pTSD_Key, PTSTSD_DESTRUCTOR *pDestructor);
inline PTBOOL PTCALL PTSTSD_SetValue(PTSTSD_KEY TSD_Key, PTSTSD_VALUE TSD_Value);
inline PTSTSD_VALUE PTCALL PTSTSD_GetValue(PTSTSD_KEY TSD_Key);
inline PTBOOL PTCALL PTSTSD_Delete(PTSTSD_KEY TSD_Key);

#ifdef PTSYSTEM_SOCKET
#ifdef PTWIN32
#include <WinSock2.h>
typedef SOCKET PTSocket;
#elif defined(PTPOSIX)
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
typedef int PTSocket;
#else
#error 未知的平台
#endif
inline PTBOOL PTCALL PTSocketCreateTCP(PTSocket *pSocketOut);
inline PTBOOL PTCALL PTSocketCreateUDP(PTSocket *pSocketOut);
inline PTBOOL PTCALL PTSocketControlNonBlock(PTSocket *pSocket, PTBOOL bNonBlock);
inline PTBOOL PTCALL PTSocketDestory(PTSocket *pSocket);
inline PTBOOL PTCALL PTSocketBindIPV4(PTSocket *pSocket, uint8_t AddressIPV4[4], uint16_t Port);
inline PTBOOL PTCALL PTSocketTCPListen(PTSocket *pSocket);
inline PTBOOL PTCALL PTSocketTCPAccept(PTSocket *pSocketListen, PTSocket *pSocketAcceptOut);//NonBlock套接字Accept得到的套接字也一定是NonBlock的
inline PTBOOL PTCALL PTSocketTCPConnectIPV4(PTSocket *pSocket, uint8_t AddressIPV4[4], uint16_t Port);//false:阻塞
inline PTBOOL PTCALL PTSocketTCPShutdown(PTSocket *pSocket);
inline PTBOOL PTCALL PTSocketRead(PTSocket *pSocket, void *pBuffer, uint32_t NumberOfBytesToRead, uint32_t *pNumberOfBytesRead);//false:阻塞
inline PTBOOL PTCALL PTSocketWrite(PTSocket *pSocket, void *pBuffer, uint32_t NumberOfBytesToWrite, uint32_t *pNumberOfBytesWritten);//false:对端关闭读取
#endif

#ifdef PTWIN32

#elif defined(PTPOSIX)
#include <sys/time.h>
#else
#error 未知的平台
#endif
inline int64_t PTCALL PTTickCount();
inline int64_t PTCALL PTTickFrequency();

//Intrinsic

inline void PTCALL PTS_Pause();

inline int32_t PTCALL PTSAtomic_CompareAndSetI32(int32_t volatile *pTarget, int32_t expect, int32_t update);
inline int64_t PTCALL PTSAtomic_CompareAndSetI64(int64_t volatile *pTarget, int64_t expect, int64_t update);
inline uint32_t PTCALL PTSAtomic_CompareAndSetUI32(uint32_t volatile *pTarget, uint32_t expect, uint32_t update);
inline uint64_t PTCALL PTSAtomic_CompareAndSetUI64(uint64_t volatile *pTarget, uint64_t expect, uint64_t update);

inline int32_t PTSAtomic_CompareAndSet(int32_t volatile *pTarget, int32_t expect, int32_t update) { return ::PTSAtomic_CompareAndSetI32(pTarget, expect, update); }
inline uint32_t PTSAtomic_CompareAndSet(uint32_t volatile *pTarget, uint32_t expect, uint32_t update) { return ::PTSAtomic_CompareAndSetUI32(pTarget, expect, update); }
inline int64_t PTSAtomic_CompareAndSet(int64_t volatile *pTarget, int64_t expect, int64_t update) { return ::PTSAtomic_CompareAndSetI64(pTarget, expect, update); }
inline uint64_t PTSAtomic_CompareAndSet(uint64_t volatile *pTarget, uint64_t expect, uint64_t update) { return ::PTSAtomic_CompareAndSetUI64(pTarget, expect, update); }

inline int32_t PTCALL PTSAtomic_GetAndSetI32(int32_t volatile *pTarget, int32_t newValue);
inline int64_t PTCALL PTSAtomic_GetAndSetI64(int64_t volatile *pTarget, int64_t newValue);
inline uint32_t PTCALL PTSAtomic_GetAndSetUI32(uint32_t volatile *pTarget, uint32_t newValue);
inline uint64_t PTCALL PTSAtomic_GetAndSetUI64(uint64_t volatile *pTarget, uint64_t newValue);

inline int32_t PTSAtomic_GetAndSet(int32_t volatile *pTarget, int32_t newValue) { return ::PTSAtomic_GetAndSetI32(pTarget, newValue); }
inline int64_t PTSAtomic_GetAndSet(int64_t volatile *pTarget, int64_t newValue) { return ::PTSAtomic_GetAndSetI64(pTarget, newValue); }
inline uint32_t PTSAtomic_GetAndSet(uint32_t volatile *pTarget, uint32_t newValue) { return ::PTSAtomic_GetAndSetUI32(pTarget, newValue); }
inline uint64_t PTSAtomic_GetAndSet(uint64_t volatile *pTarget, uint64_t newValue) { return ::PTSAtomic_GetAndSetUI64(pTarget, newValue); }

inline int32_t PTCALL PTSAtomic_GetAndAddI32(int32_t volatile *pTarget, int32_t delta);
inline int64_t PTCALL PTSAtomic_GetAndAddI64(int64_t volatile *pTarget, int64_t delta);
inline uint32_t PTCALL PTSAtomic_GetAndAddUI32(uint32_t volatile *pTarget, uint32_t delta);
inline uint64_t PTCALL PTSAtomic_GetAndAddUI64(uint64_t volatile *pTarget, uint64_t delta);

inline int32_t PTSAtomic_GetAndAdd(int32_t volatile *pTarget, int32_t delta) { return ::PTSAtomic_GetAndAddI32(pTarget, delta); }
inline int64_t PTSAtomic_GetAndAdd(int64_t volatile *pTarget, int64_t delta) { return ::PTSAtomic_GetAndAddI64(pTarget, delta); }
inline uint32_t PTSAtomic_GetAndAdd(uint32_t volatile *pTarget, uint32_t delta) { return ::PTSAtomic_GetAndAddUI32(pTarget, delta); }
inline uint64_t PTSAtomic_GetAndAdd(uint64_t volatile *pTarget, uint64_t delta) { return ::PTSAtomic_GetAndAddUI64(pTarget, delta); }

inline int32_t PTCALL PTSAtomic_GetI32(int32_t volatile *pTarget);
inline int64_t PTCALL PTSAtomic_GetI64(int64_t volatile *pTarget);
inline uint32_t PTCALL PTSAtomic_GetUI32(uint32_t volatile *pTarget);
inline uint64_t PTCALL PTSAtomic_GetUI64(uint64_t volatile *pTarget);

inline int32_t PTSAtomic_Get(int32_t volatile *pTarget) { return ::PTSAtomic_GetI32(pTarget); }
inline int64_t PTSAtomic_Get(int64_t volatile *pTarget) { return ::PTSAtomic_GetI64(pTarget); }
inline uint32_t PTSAtomic_Get(uint32_t volatile *pTarget) { return ::PTSAtomic_GetUI32(pTarget); }
inline uint64_t PTSAtomic_Get(uint64_t volatile *pTarget) { return ::PTSAtomic_GetUI64(pTarget); }

inline void PTCALL PTSAtomic_SetI32(int32_t volatile *pTarget, int32_t newValue);
inline void PTCALL PTSAtomic_SetI64(int64_t volatile *pTarget, int64_t newValue);
inline void PTCALL PTSAtomic_SetUI32(uint32_t volatile *pTarget, uint32_t newValue);
inline void PTCALL PTSAtomic_SetUI64(uint64_t volatile *pTarget, uint64_t newValue);

inline void PTSAtomic_Set(int32_t volatile *pTarget, int32_t newValue) { return ::PTSAtomic_SetI32(pTarget, newValue); }
inline void PTSAtomic_Set(int64_t volatile *pTarget, int64_t newValue) { return ::PTSAtomic_SetI64(pTarget, newValue); }
inline void PTSAtomic_Set(uint32_t volatile *pTarget, uint32_t newValue) { return ::PTSAtomic_SetUI32(pTarget, newValue); }
inline void PTSAtomic_Set(uint64_t volatile *pTarget, uint64_t newValue) { return ::PTSAtomic_SetUI64(pTarget, newValue); }

#if defined PTWIN32
#include "Win32/PTSThread.inl"
#elif defined PTPOSIX
#include "Posix/PTSThread.inl"
#else
#error 未知的平台
#endif

#endif

