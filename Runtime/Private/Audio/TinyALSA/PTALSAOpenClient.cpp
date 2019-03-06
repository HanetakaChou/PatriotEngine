#include "PTALSAOpenClient.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>

static inline void PT_DebugStringOutput(char const *pDebugString);
static inline bool PT_Number_IntToChar_HEX(uint32_t I, char *pC);

#include "../../../ThirdParty/CF/Base/uuid.h"

int PTALSAOpen(char *FilePathName, int FileMode)
{
#if defined(PTPOSIX)
#if defined(PTPOSIXXCB)
	char const szPTAudioOpenEXE[] = ".PTALSAOpenServer.sh";
#elif defined(PTPOSIXANDROID)
	//在Android中，需要用命名伪装成共享库，以被系统识别
	char const szPTAudioOpenEXE[] = "libPTALSAOpenServer.so";
#else
#error 未知的平台
#endif
#else
#error 未知的平台
#endif
	#define szPTALSAOpenDir ".PTALSAOpen"
	char const szPTAudioOpenKeyCode[] = "我们是共产主义接班人！";
	constexpr uint32_t const szPTAudioOpenKeyCodeLength = sizeof(szPTAudioOpenKeyCode) - 1U;

	int iResult;
	bool bNeedToMakeALSAOpenDir;
	{
		struct stat sFileStatusALSAOpenDir;
		iResult = ::stat(szPTALSAOpenDir, &sFileStatusALSAOpenDir);
		if (iResult == 0)
		{
			if (S_ISDIR(sFileStatusALSAOpenDir.st_mode))
			{
				bNeedToMakeALSAOpenDir = false;
			}
			else
			{
				while (::unlink(szPTALSAOpenDir) == 0)
				{
					::PT_DebugStringOutput("Client进程：我们遭到攻击，ALSAOpen目录已被占用 ！");
				}
				bNeedToMakeALSAOpenDir = true;
			}

		}
		else
		{
			bNeedToMakeALSAOpenDir = true;
		}
	}
	
	if (bNeedToMakeALSAOpenDir)
	{
		::umask(0);
		iResult = ::mkdir(szPTALSAOpenDir, S_IRWXU | S_IRWXG | S_IRWXO);
		if (iResult != 0)
		{
			::PT_DebugStringOutput("Client进程：创建ALSAOpen目录失败 ！");
			return -1;
		}
	}
	
	char szSockAddrSeverPathRelative[] = { szPTALSAOpenDir"/FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF" };
	uint32_t const szSockAddrSeverPathRelativeLength = sizeof(szSockAddrSeverPathRelative);

	//Bind Socket
	int iFdSocketSever;
	{
		iFdSocketSever = ::socket(AF_UNIX, SOCK_DGRAM, 0);
		assert(iFdSocketSever != -1);

		iResult = ::fcntl(iFdSocketSever, F_SETFD, ::fcntl(iFdSocketSever, F_GETFD) | FD_CLOEXEC);
		assert(iResult == 0);

		iResult = ::fcntl(iFdSocketSever, F_SETFL, ::fcntl(iFdSocketSever, F_GETFL) | O_NONBLOCK);
		assert(iResult == 0);

		struct sockaddr_un SockAddrSever;
		
		SockAddrSever.sun_family = AF_UNIX;

		char * const szUUID = szSockAddrSeverPathRelative + (sizeof(szPTALSAOpenDir) - 1U/*'\0'*/) + 1U/*'/'*/;
		uint32_t const uiUUIDIndex[] = { 0U,1U,2U,3U,4U,5U,6U,7U,9U,10U,11U,12U,14U,15U,16U,17U,19U,20U,21U,22U,24U,25U,26U,27U,28U,29U,30U,31U,32U,33U,34U,35U };
		do
		{
			uuid_t uiUUID;
			iResult = ::uuid_generate_time_safe(uiUUID);
			assert(iResult == 0);

			for (uint32_t i = 0U; i < sizeof(uuid_t); ++i)
			{
				bool bResult = ::PT_Number_IntToChar_HEX(uiUUID[i] / 16U, szUUID + uiUUIDIndex[2U * i]);
				assert(bResult);

				bResult = ::PT_Number_IntToChar_HEX(uiUUID[i] & 15U, szUUID + uiUUIDIndex[2U * i + 1U]);
				assert(bResult);
			}

			assert((sizeof(sockaddr_un) / sizeof(char)) >= (szSockAddrSeverPathRelativeLength + 1U));
			::memcpy(SockAddrSever.sun_path, szSockAddrSeverPathRelative, szSockAddrSeverPathRelativeLength + 1U/*'\0'*/);

			iResult = ::bind(iFdSocketSever, reinterpret_cast<struct sockaddr *>(&SockAddrSever), offsetof(struct sockaddr_un, sun_path) + szSockAddrSeverPathRelativeLength);
			if (iResult != 0)
			{
				::PT_DebugStringOutput("Client进程：我们可能遭到攻击，套接字地址已被占用 ！");
				assert(iResult == -1);
				assert(errno == EADDRINUSE);
			}

		} while (iResult != 0);
	}

	//For return -1
	class __Type_SafeRelease_SockAddrSever
	{
		char * m_szSockAddrSeverPathRelative;
	public:
		inline __Type_SafeRelease_SockAddrSever(
			char * szSockAddrSeverPathRelative
		)
			:
			m_szSockAddrSeverPathRelative(szSockAddrSeverPathRelative)
		{

		}

		inline ~__Type_SafeRelease_SockAddrSever()
		{
			int iResult = ::unlink(m_szSockAddrSeverPathRelative); //与Bind相对
			if (iResult != 0)
			{
				::PT_DebugStringOutput("Client进程：我们遭到攻击，套接字已被其它进程删除 ！");
				assert(errno == ENOENT);
			}
		}
	} __Variable_SafeRelease_SockAddrSever(szSockAddrSeverPathRelative);

	//Fork And Exec
	{
		//We Fork Here
		pid_t iPIDChild = ::fork();
		assert(iPIDChild != -1);
		if (iPIDChild == 0)
		{
			//Argv0 ExePath
			char szPathEXE[PATH_MAX];
#if defined(PTPOSIX)
#if defined(PTPOSIXXCB)
			//ProcSelfExe
			ssize_t szPathEXELength = ::readlink("/proc/self/exe", szPathEXE, PATH_MAX - 1);
			//--PTWindow.sh
			while (szPathEXELength >= 1 && szPathEXE[szPathEXELength - 1] != '/')
			{
				--szPathEXELength;
			}
			assert(szPathEXELength >= 1 && szPathEXE[szPathEXELength - 1] == '/');
			//szPTAudioOpenEXE + '\0'
			assert((szPathEXELength + sizeof(szPTAudioOpenEXE)) <= PATH_MAX);
			::memcpy(szPathEXE + szPathEXELength, szPTAudioOpenEXE, sizeof(szPTAudioOpenEXE));
#elif defined(PTPOSIXANDROID)
			//In Android 
			//The EXE File Is The /system/bin/app_process
			//We Can Not Use That Directory
			{
				static char const szPTTinyALSASO[] = "libtinyalsa.so";
				static constexpr uint32_t const szPTTinyALSASOLength = sizeof(szPTTinyALSASO) - 1U;
				static char const * const szPTTinyALSASOEnd = szPTTinyALSASO + szPTTinyALSASOLength;
				static uint32_t const s_Page_Size = 1024U * 4U;

				int iFDProcSelfMaps = ::open("/proc/self/maps", O_RDONLY);
				assert(iFDProcSelfMaps != -1);

				char szRowVector[4096U];
				assert(s_Page_Size == 4096U);

				char *szPathNameBegin = NULL;
				char *szPathNameEnd = NULL;

				bool bIsFound;

				ssize_t ssResult;
				while ((ssResult = ::read(iFDProcSelfMaps, szRowVector, 4096U)) != 0)
				{
					assert(ssResult != -1);
					assert(ssResult < 4096);
					assert(szRowVector[ssResult - 1] == '\n');
					szRowVector[ssResult] = '\0';

					char *szRowBegin = szRowVector;

					bIsFound = false;
					while ((*szRowBegin) != '\0')
					{
						bool bIsEqual = true;

						//RowEnd
						char *szRowEnd = szRowBegin;
						while ((*szRowEnd) != '\n')
						{
							++szRowEnd;
						}

						//The szRowEnd[0] is '\n' And The szPTTinyALSASOEnd[0] is '\0'
						//So We Skip When i == 0 
						for (int i = 1; i < szPTTinyALSASOLength; ++i)
						{
							if (szRowEnd - i < szRowBegin)
							{
								bIsEqual = false;
								break;
							}

							if (szRowEnd[-i] != szPTTinyALSASOEnd[-i])
							{
								bIsEqual = false;
								break;
							}
						}

						if (bIsEqual)
						{
							szPathNameEnd = szRowEnd;

							szPathNameBegin = szPathNameEnd;
							while ((*szPathNameBegin) != ' ' && (*szPathNameBegin) != '\t')
							{
								--szPathNameBegin;
							}
							//PathName Is Absolute
							assert(szPathNameBegin[1] == '/');
							++szPathNameBegin;
							bIsFound = true;
							break;
						}

						szRowBegin = szRowEnd + 1;
					}

					if (bIsFound)
					{
						break;
					}
				}

				int iResult = ::close(iFDProcSelfMaps);
				assert(iResult == 0);

				if (!bIsFound)
				{
					::PT_DebugStringOutput("Client进程：Can't Find The Directory Where The PTTinyALSASO In \n !!!");
					::exit(-1);
				}

				//The Directory Where The PTTinyALSASO In
				uint32_t szPathEXELength = uint32_t(szPathNameEnd - szPathNameBegin) - szPTTinyALSASOLength;
				assert(szPathEXELength > 0U);
				::memcpy(szPathEXE, szPathNameBegin, szPathEXELength);
				//szPTAudioOpenEXE + '\0'
				assert((szPathEXELength + sizeof(szPTAudioOpenEXE)) <= PATH_MAX);
				::memcpy(szPathEXE + szPathEXELength, szPTAudioOpenEXE, sizeof(szPTAudioOpenEXE));
			}
#else
#error 未知的平台
#endif
#else
#error 未知的平台
#endif
			//Argv1 SockAddrSeverPath
			char szSockAddrSeverPathAbsolute[PATH_MAX];
			//PathCWD
			char *szResult = ::getcwd(szSockAddrSeverPathAbsolute, PATH_MAX);
			assert(szResult != NULL);
			uint32_t szPathCWDLength = static_cast<uint32_t>(::strlen(szSockAddrSeverPathAbsolute));
			//'/'
			szSockAddrSeverPathAbsolute[szPathCWDLength] = '/';
			++szPathCWDLength;
			//szSockAddrSeverPathRelative + '\0'
			::memcpy(szSockAddrSeverPathAbsolute + szPathCWDLength, szSockAddrSeverPathRelative, szSockAddrSeverPathRelativeLength + 1U);

			//Argv3 FileToOpenMode
			uint32_t uiFileMode = static_cast<uint32_t>(FileMode);
			char szFileMode[] = "0XFFFFFFFF"; //The Max Length 
			uint32_t szFileModeLength = 10U;
			while (szFileModeLength > 2U)
			{
				bool bResult = ::PT_Number_IntToChar_HEX(uiFileMode % 0X10U, &szFileMode[szFileModeLength - 1U]);
				assert(bResult);
				uiFileMode /= 0X10U;
				--szFileModeLength;
			}

			//Argv
			char *Argv[] = { szPathEXE,szSockAddrSeverPathAbsolute,FilePathName,szFileMode,NULL };

			int iResult = ::execv(Argv[0], Argv); //kdesu Will Use The Envp
			if (iResult == -1)
			{
				::PT_DebugStringOutput("Client进程：The \"execv\" Fail ！");
				::PT_DebugStringOutput(szPathEXE);
				::exit(-1);
			}
		}

		int iStatusChild;
		pid_t iPIDResult = ::waitpid(iPIDChild, &iStatusChild, 0);
		assert(iPIDResult != -1);

		if (!WIFEXITED(iStatusChild) || !(WEXITSTATUS(iStatusChild) == 0))
		{
			::PT_DebugStringOutput("Client进程：PTAudioOpenEXE进程发生错误！");
			return -1;
		}
	}

	//Receive File By Socket
	int iFDFileToOpen;
	{
		typedef int TAncillaryData[1];

		struct msghdr MessageHeader;
		struct iovec IOVector[1];
		char IOBuffer[szPTAudioOpenKeyCodeLength];
		alignas(alignof(cmsghdr)) char MessageControlVector[CMSG_SPACE(sizeof(TAncillaryData))/*+CMSG_SPACE(sizeof(TAncillaryData2))*/];

		MessageHeader.msg_name = NULL;
		MessageHeader.msg_namelen = socklen_t(0);

		MessageHeader.msg_iov = IOVector;
		MessageHeader.msg_iovlen = 1U;
		IOVector[0].iov_base = IOBuffer;
		IOVector[0].iov_len = szPTAudioOpenKeyCodeLength;

		MessageHeader.msg_control = MessageControlVector;
		MessageHeader.msg_controllen = sizeof(MessageControlVector);

		cmsghdr *pMessageControlHeader = CMSG_FIRSTHDR(&MessageHeader);
		pMessageControlHeader->cmsg_level = SOL_SOCKET;
		pMessageControlHeader->cmsg_type = SCM_RIGHTS;
		pMessageControlHeader->cmsg_len = CMSG_LEN(sizeof(TAncillaryData));

		int(*pAncillaryData)[1] = reinterpret_cast<int(*)[1]>(CMSG_DATA(pMessageControlHeader));

		//CMSG_NXTHDR
		//For MessageControlHeader 2

		MessageHeader.msg_flags = 0;

		ssize_t ssResult = ::recvmsg(iFdSocketSever, &MessageHeader, 0);
		if (ssResult == -1)
		{
			::PT_DebugStringOutput("Client进程：接收控制消息失败！");
			return -1;
		}

		assert(ssResult == szPTAudioOpenKeyCodeLength);
		for (int i = 0; i < szPTAudioOpenKeyCodeLength; ++i)
		{
			assert(IOBuffer[i] == szPTAudioOpenKeyCode[i]);
		}

		iFDFileToOpen = (*pAncillaryData)[0];
	}

	//Close Socket
	{
		::close(iFdSocketSever); //与socket相对
	}

	return iFDFileToOpen;
}

#if defined(PTPOSIX)
#if defined(PTPOSIXXCB)
static inline void PT_DebugStringOutput(char const *pDebugString)
{
#ifndef NDEBUG
	::write(STDOUT_FILENO, pDebugString, ::strlen(pDebugString) + 1U);
#endif
}
#elif defined(PTPOSIXANDROID)
#ifndef NDEBUG
#include <android/log.h>
#endif
static inline void PT_DebugStringOutput(char const *pDebugString)
{
#ifndef NDEBUG
	::__android_log_write(ANDROID_LOG_ERROR, "PTALSAOpenClient", pDebugString);
#endif
}
#else
#error 未知的平台
#endif
#else
#error 未知的平台
#endif

static inline bool PT_Number_IntToChar_HEX(uint32_t I, char *pC)
{
	switch (I)
	{
	case 0U:
		(*pC) = '0';
		return true;
	case 1U:
		(*pC) = '1';
		return true;
	case 2U:
		(*pC) = '2';
		return true;
	case 3U:
		(*pC) = '3';
		return true;
	case 4U:
		(*pC) = '4';
		return true;
	case 5U:
		(*pC) = '5';
		return true;
	case 6U:
		(*pC) = '6';
		return true;
	case 7U:
		(*pC) = '7';
		return true;
	case 8U:
		(*pC) = '8';
		return true;
	case 9U:
		(*pC) = '9';
		return true;
	case 10U:
		(*pC) = 'A';
		return true;
	case 11U:
		(*pC) = 'B';
		return true;
	case 12U:
		(*pC) = 'C';
		return true;
	case 13U:
		(*pC) = 'D';
		return true;
	case 14U:
		(*pC) = 'E';
		return true;
	case 15U:
		(*pC) = 'F';
		return true;
	default:
		return false;
	}
}

//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
#include "../../../Public/System/Posix/PTSLocateSourceCode.inl"