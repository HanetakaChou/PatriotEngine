#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <unistd.h>
#include <sys/types.h>
#include <grp.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <sys/mount.h>
#ifndef MS_LAZYTIME
#define MS_LAZYTIME (1<<25) //Fix For Android //Since Kernel 4.0
#endif
#include <sys/xattr.h>
#include <errno.h>
#include <stdio.h>

static inline void PT_DebugStringOutput(char const *pDebugString);
static inline void PTA_ERRNO();
static inline void PTA_PERROR(char const *szMsg);
static inline void PT_DebugStringOutput(char *pszPrefix /* Size Must Equal To uiPrefixLength + 10(0XFFFFFFFF) + 1('\0')(Must Be '\0' Number Must Be End No More Char) */, uint32_t uiPrefixLength, uint32_t uiNumber);
static inline void PT_DebugStringOutput(char *pszPrefix /* Size Must Equal To uiPrefixLength + uiContentLength + 1('\0') */, uint32_t uiPrefixLength, char const *pszContent /* Size Must Equal To uiContentLength + 1('\0')(Must Be '\0' No More Char) */, uint32_t uiContentLength);
static inline bool PT_Number_IntToChar_HEX(uint32_t I, char *pC);
static inline bool PT_Number_CharToInt_HEX(char C, uint32_t *pI);
static inline bool PT_Number_CharIsInt_DEC(char C);
static inline bool PT_Number_CharToInt_DEC(char C, int32_t *pI);
static inline bool PT_Info_DeviceIDToMount(char *szPathSource /* Size Is PATH_MAX */, char *szPathTarget /* Size Is PATH_MAX */, char *szFileSystemType /* Size Is 256 */, unsigned long *puiMountFlags, dev_t iDeviceID);

//Android ARM64
//实用工具
//命令行
//$(LLVMToolchainPrebuiltRoot)\bin\clang.exe -fdiagnostics-format=msvc -gcc-toolchain "$(GccToolchainPrebuiltPath)" -target "$(ClangTarget)" --sysroot="$(Sysroot)" -c %(FullPath) -o "$(IntDir)%(Filename).o" -I "$(Sysroot)\usr\include" -g2 -gdwarf-2 -Wall -O0 -fno-strict-aliasing -fno-omit-frame-pointer -fno-exceptions -fstack-protector -fpie -fno-short-enums -fno-rtti -std=c++11 -D PTPOSIX -D PTPOSIXANDROID -D PTARM64 -x c++ -fvisibility=hidden
//$(LLVMToolchainPrebuiltRoot)\bin\clang.exe -fdiagnostics-format=msvc -gcc-toolchain "$(GccToolchainPrebuiltPath)" -target "$(ClangTarget)" --sysroot="$(Sysroot)" -pie "$(IntDir)%(Filename).o" -o"$(OutDir)$(TargetFileName)" -Wl,-rpath-link="$(Sysroot)\usr\lib" -Wl,-L"$(Sysroot)\usr\lib" -llog -Wl,--no-undefined -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack 
//说明
//Compile To %(Filename).o And Link To $(TargetFileName)
//输出
//$(OutDir)$(TargetName)

int main(int argc, char **argv)
{
	//Group Audio
	gid_t iGIDAudio;
	{
		//单线程应用不需要考虑重入（ReEntry）
		//::sysconf(_SC_GETGR_R_SIZE_MAX)
		//::getgrnam_r

		group *pGroupAudio = ::getgrnam("audio");
		if (pGroupAudio == NULL)
		{
			::PT_DebugStringOutput("用户组Audio不存在");
			return -1;
		}

		iGIDAudio = pGroupAudio->gr_gid;
	}

	//Fork
	gid_t iGIDEffective = ::getegid();
	if (iGIDEffective == iGIDAudio)
	{
		//Do Audio Open

		//Check Argument
		char *szSockAddrSeverPathAbsolute;
		char *szFileToOpenPath;
		char *szFileToOpenMode;
		{
			if (argc < 4)
			{
				::PT_DebugStringOutput("Audio进程：Invalid Argument !");
				::PT_DebugStringOutput("Audio进程：Usage：.PTAudioOpenHelper.sh SockAddrSeverPath FileToOpenPath !");

				::PT_DebugStringOutput("Audio进程：Invalid Argument !");
				::PT_DebugStringOutput("Audio进程：Usage：.PTAudioOpenHelper.sh SockAddrSeverPath FileToOpenPath !");

				if (argc >= 1)
				{
					::PT_DebugStringOutput("Audio进程：Argv[0]:----↓");
					::PT_DebugStringOutput(argv[0]);
					::PT_DebugStringOutput("Audio进程：Argv[0]:----↑");
				}

				if (argc >= 2)
				{
					::PT_DebugStringOutput("Audio进程：Argv[1]:----↓");
					::PT_DebugStringOutput(argv[1]);
					::PT_DebugStringOutput("Audio进程：Argv[10]:----↑");
				}

				if (argc >= 3)
				{
					::PT_DebugStringOutput("Audio进程：Argv[2]:----↓");
					::PT_DebugStringOutput(argv[2]);
					::PT_DebugStringOutput("Audio进程：Argv[2]:----↑");
				}

				return -1;
			}

			szSockAddrSeverPathAbsolute = argv[1];
			szFileToOpenPath = argv[2];
			szFileToOpenMode = argv[3];
		}

		//Check SockAddrSeverPath
		char *szSockAddrSeverDirectory;
		char *szSockAddrSeverPathRelative;
		uint32_t szSockAddrSeverPathRelativeLength;
		char szPathRoot[2] = "/";
		{
			uint32_t SockAddrPathAbsoluteLength = static_cast<uint32_t>(::strlen(szSockAddrSeverPathAbsolute));

			uint32_t szSockAddrSeverDirectoryLength = SockAddrPathAbsoluteLength;
			while (szSockAddrSeverDirectoryLength >= 1U && szSockAddrSeverPathAbsolute[szSockAddrSeverDirectoryLength - 1U] != '/')
			{
				--szSockAddrSeverDirectoryLength;
			}
			if (szSockAddrSeverDirectoryLength < 1U)
			{
				::PT_DebugStringOutput("Audio进程：SockAddrPath Must Be Absolute Not Relative !");
				return -1;
			}
			assert(szSockAddrSeverPathAbsolute[szSockAddrSeverDirectoryLength - 1U] == '/');

			szSockAddrSeverPathAbsolute[szSockAddrSeverDirectoryLength - 1U] = '\0';
			szSockAddrSeverDirectory = (szSockAddrSeverDirectoryLength >1U) ? szSockAddrSeverPathAbsolute : szPathRoot;
			szSockAddrSeverPathRelative = szSockAddrSeverPathAbsolute + szSockAddrSeverDirectoryLength;

			szSockAddrSeverPathRelativeLength = SockAddrPathAbsoluteLength - szSockAddrSeverDirectoryLength;
			assert(szSockAddrSeverPathRelative[szSockAddrSeverPathRelativeLength] == '\0');
			if ((szSockAddrSeverPathRelativeLength + 1U) > sizeof(sockaddr_un::sun_path))
			{
				::PT_DebugStringOutput("Audio进程：szSockAddrSeverPathRelative Too Long !");
				return -1;
			}
		}

		//Check FileToOpenPath
		{
			if (szFileToOpenPath[0] != '/')
			{
				::PT_DebugStringOutput("Audio进程：FileToOpenPath Must Be Absolute Not Relative !");
				return -1;
			}
		}

		//Check FileToOpenMode
		int FileToOpenMode;
		{
			uint32_t uiFileToOpenMode = 0U;
			uint32_t ValueToAdd;
			//0
			if ((*szFileToOpenMode) != '0')
			{
				::PT_DebugStringOutput("Audio进程：The FileToOpenMode Argument Is Invalid !");
				return -1;
			}
			++szFileToOpenMode;
			//X
			if ((*szFileToOpenMode) != 'X')
			{
				::PT_DebugStringOutput("Audio进程：The FileToOpenMode Argument Is Invalid !");
				return -1;
			}
			++szFileToOpenMode;
			//FFFFFFFF
			while ((*szFileToOpenMode) != '\0')
			{
				bool bResult = ::PT_Number_CharToInt_HEX(*szFileToOpenMode, &ValueToAdd);
				if (!bResult)
				{
					::PT_DebugStringOutput("Audio进程：The FileToOpenMode Argument Is Invalid !");
					return -1;
				}

				uiFileToOpenMode <<= 4U;
				uiFileToOpenMode += ValueToAdd;
				++szFileToOpenMode;
			}

			FileToOpenMode = static_cast<int>(uiFileToOpenMode);
		}

		//Open File
		int iFdFileToOpen;
		{
			iFdFileToOpen = ::open(szFileToOpenPath, FileToOpenMode);
			if (iFdFileToOpen == -1)
			{
				::PT_DebugStringOutput("Audio进程：File Can Not Be Opened !");
				return -1;
			}
		}

		//Send File By Socket
		{
			typedef int TAncillaryData[1];

			char szPTAudioOpenKeyCode[] = "我们是共产主义接班人！";
			static constexpr uint32_t const szPTAudioOpenKeyCodeLength = sizeof(szPTAudioOpenKeyCode) - 1U;

			struct msghdr MessageHeader;
			struct sockaddr_un SockAddrSever;
			struct iovec IOVector[1];
			char MessageControlVector[CMSG_SPACE(sizeof(TAncillaryData)) /*+CMSG_SPACE(sizeof(TAncillaryData2))*/];

			//SockAddr
			MessageHeader.msg_name = &SockAddrSever;
			MessageHeader.msg_namelen = offsetof(struct sockaddr_un, sun_path) + szSockAddrSeverPathRelativeLength;
			SockAddrSever.sun_family = AF_UNIX;
			assert((szSockAddrSeverPathRelativeLength + 1U) <= sizeof(sockaddr_un::sun_path));
			::memcpy(SockAddrSever.sun_path, szSockAddrSeverPathRelative, szSockAddrSeverPathRelativeLength + 1U);

			//RegularData
			MessageHeader.msg_iov = IOVector;
			MessageHeader.msg_iovlen = 1U;
			IOVector[0].iov_base = szPTAudioOpenKeyCode;
			IOVector[0].iov_len = szPTAudioOpenKeyCodeLength;

			//ControlMessage(Include AncillaryData)
			MessageHeader.msg_control = MessageControlVector;
			MessageHeader.msg_controllen = szPTAudioOpenKeyCodeLength;

			cmsghdr *pMessageControlHeader = CMSG_FIRSTHDR(&MessageHeader);
			pMessageControlHeader->cmsg_level = SOL_SOCKET;
			pMessageControlHeader->cmsg_type = SCM_RIGHTS;
			pMessageControlHeader->cmsg_len = CMSG_LEN(sizeof(TAncillaryData));

			int(*pAncillaryData)[1] = reinterpret_cast<int(*)[1]>(CMSG_DATA(pMessageControlHeader));
			(*pAncillaryData)[0] = iFdFileToOpen;

			//CMSG_NXTHDR
			//For MessageControlHeader 2

			MessageHeader.msg_flags = 0;

			int iFdSocketClient = socket(AF_UNIX, SOCK_DGRAM, 0);
			assert(iFdSocketClient != -1);

			int iResult = ::fcntl(iFdSocketClient, F_SETFL, ::fcntl(iFdSocketClient, F_GETFL) | O_NONBLOCK);
			assert(iResult == 0);

			iResult = ::chdir(szSockAddrSeverDirectory);
			if (iResult != 0)
			{
				::PT_DebugStringOutput("Audio进程：无法切换到接收端套接字所在目录 !");
				assert(errno == ENOENT);
				return -1;
			}

			ssize_t ssResult = ::sendmsg(iFdSocketClient, &MessageHeader, 0);
			if (ssResult == -1)
			{
				::PT_DebugStringOutput("Audio进程：Send Control Message Failed !");
				return -1;
			}

			//RegularData Only //Not Include Control Message
			assert(ssResult == szPTAudioOpenKeyCodeLength);

			iResult = ::close(iFdSocketClient);
			assert(iResult == 0);
		}

		//Close File
		{
			int iResult = ::close(iFdFileToOpen);
			assert(iResult == 0);
		}

		return 0;
	}
	else
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

		if (::geteuid() == 0 || ::getegid() == 0)
		{
			//We Are ROOT
			::setreuid(0, 0);
			::setregid(0, 0);

			int iResult;

			char szPathEXESelf[PATH_MAX];
			ssize_t szPathEXESelfLength = ::readlink("/proc/self/exe", szPathEXESelf, PATH_MAX - 1U);
			assert(szPathEXESelfLength != -1);
			szPathEXESelf[szPathEXESelfLength] = '\0';

			int iFDEXESelf = ::open(szPathEXESelf, O_RDONLY);
			assert(iFDEXESelf != -1);

			struct stat sFileStatusEXESelf;
			iResult = ::fstat(iFDEXESelf, &sFileStatusEXESelf);
			assert(iResult != -1);

			//Since The FileSystem Was Mounted With MS_NOSUID //https://source.android.com/security/enhancements/enhancements43
			//The SET-GID-FLAG Has No Effect
			//Remount Is Not A Good Method
			//First, The Init Process Will Mount The FileSystem With MS_NOSUID Accroding To The Configuration File EveryTime The Device Reboot //So We Need To Request ROOT Privilege EveryTime
			//Second，It Can Cause Security Problem

			//Since We Can Gain ROOT Privilege To Set The SET-GID-FLAG For The EXE File 
			//The Directory Where The SU IN Must Mounted Without MS_NOSUID
			//We Can Copy The EXE File To That Directory
			//Unlink The EXE File OurSelf And Make A SymbolLink To The EXE File In That Diectory

			char szMountSourceExeSelf[PATH_MAX];
			char szMountPointExeSelf[PATH_MAX];
			char szFileSystemTypeExeSelf[256];
			unsigned long uiMountFlagsExeSelf;
			::PT_Info_DeviceIDToMount(szMountSourceExeSelf, szMountPointExeSelf, szFileSystemTypeExeSelf, &uiMountFlagsExeSelf, sFileStatusEXESelf.st_dev);
			if (uiMountFlagsExeSelf&MS_NOSUID)
			{
				char szPathSUIN[PATH_MAX];
				uint32_t szPathSUINLength; //The Directory Length
				char szMountSourceSUIN[PATH_MAX];
				char szMountPointSUIN[PATH_MAX];
				char szFileSystemTypeSUIN[256];
				unsigned long uiMountFlagsSUIN;
				bool bIsFound = false;
				{
					char const *szEnvPathVector = ::getenv("PATH");
					char const *szEnvPathBegin = szEnvPathVector;
					char const *szEnvPathEnd = NULL;
					do {
						szEnvPathEnd = szEnvPathBegin;
						while ((*szEnvPathEnd) != '\0')
						{
							if ((*szEnvPathEnd) == ':')
							{
								break;
							}
							++szEnvPathEnd;
						}

						//The Directory Where The SU Probably In
						szPathSUINLength = uint32_t(szEnvPathEnd - szEnvPathBegin);
						::memcpy(szPathSUIN, szEnvPathBegin, szPathSUINLength);
						//The Relative Path Of The SU
						assert((szPathSUINLength + 4U) <= PATH_MAX);
						::memcpy(szPathSUIN + szPathSUINLength, "/su", 4U);
						//Include '/'
						++szPathSUINLength;

						struct stat sFileStatusSU;
						int iResult = ::stat(szPathSUIN, &sFileStatusSU);

						if (iResult == 0)
						{
							bool bResult = ::PT_Info_DeviceIDToMount(szMountSourceSUIN, szMountPointSUIN, szFileSystemTypeSUIN, &uiMountFlagsSUIN, sFileStatusSU.st_dev);
							if (!bResult)
							{
								::PT_DebugStringOutput("ROOT进程：Can't Find The Mount Point Where The DeviceID Present!!!");
								return -1;
							}
							if (!(uiMountFlagsSUIN&MS_NOSUID))
							{
								assert((sFileStatusSU.st_uid == 0 && (sFileStatusSU.st_mode&S_IXUSR) && (sFileStatusSU.st_mode&S_ISUID)) || (sFileStatusSU.st_gid == 0 && (sFileStatusSU.st_mode&S_IXGRP) && (sFileStatusSU.st_mode&S_ISGID)));
								bIsFound = true;
								break;
							}
						}
						szEnvPathBegin = szEnvPathEnd + 1;
					} while ((*szEnvPathEnd) != 0);
				}

				if (!bIsFound)
				{
					//The SU Must Can Be Found
					//Since We Can Gain ROOT Privilege To Set The SET-GID-FLAG For The EXE File 
					//It Is Occult
					::PT_DebugStringOutput("ROOT进程：无法找到文件\"su\"！");
					return -1;
				}

				bool bMountPointReadOnlySUIN = (uiMountFlagsSUIN&MS_RDONLY);
				if (bMountPointReadOnlySUIN)
				{
					bool bMountSuccess = true;
					{
						//The Most Robust Way To Use The "mount" In Android !!!

						int iFDPairStdIn[2];
						iResult = ::socketpair(AF_UNIX, SOCK_STREAM, 0, iFDPairStdIn);
						assert(iResult == 0);
						int iFDPairStdErr[2];
						iResult = ::socketpair(AF_UNIX, SOCK_STREAM, 0, iFDPairStdErr);
						assert(iResult == 0);

						//Use Socket To Hook STDIN
						int iFDStdInCached = ::fcntl(STDIN_FILENO, F_DUPFD, 0);
						if (iFDStdInCached != -1)
						{
							iResult = ::close(STDIN_FILENO);
							assert(iResult == 0);
						}
						iResult = ::fcntl(iFDPairStdIn[0], F_DUPFD, STDIN_FILENO);
						assert(iResult == STDIN_FILENO);
						iResult = ::close(iFDPairStdIn[0]);
						assert(iResult == 0);

						//Use Socket To Hook STDERR
						int iFDStdErrCached = ::fcntl(STDERR_FILENO, F_DUPFD, 0);
						if (iFDStdErrCached != -1)
						{
							iResult = ::close(STDERR_FILENO);
							assert(iResult == 0);
						}
						iResult = ::fcntl(iFDPairStdErr[1], F_DUPFD, STDERR_FILENO);
						assert(iResult == STDERR_FILENO);
						iResult = ::close(iFDPairStdErr[1]);
						assert(iResult == 0);

						//Write Command To The ROOT Shell
						ssize_t ssResult = ::write(iFDPairStdIn[1], "mount -o remount,rw ", sizeof("mount -o remount,rw ") - 1U/*'\0'*/);
						assert(ssResult != -1);
						ssResult = ::write(iFDPairStdIn[1], szMountSourceSUIN, ::strlen(szMountSourceSUIN));
						assert(ssResult != -1);
						ssResult = ::write(iFDPairStdIn[1], " ", sizeof(" ") - 1U/*'\0'*/);
						assert(ssResult != -1);
						ssResult = ::write(iFDPairStdIn[1], szMountPointSUIN, ::strlen(szMountPointSUIN) + 1U/*'\0'*/);
						assert(ssResult != -1);

						//Read Will Return 0 And Then The Child Process That We Wait Can Exit
						iResult = ::close(iFDPairStdIn[1]);
						assert(iResult == 0);

						//How-To SU //Guidelines for problem-free su usage //https://su.chainfire.eu
						//We Need To Swith Context Under The SELinux //Otherwise, The Mount Will Fail！
						int iStatusChild = ::system("su");

						//Restore The STDIN
						iResult = ::close(STDIN_FILENO);
						assert(iResult == 0);
						if (iFDStdInCached != -1)
						{
							iResult = ::fcntl(iFDStdInCached, F_DUPFD, STDIN_FILENO);
							assert(iResult == STDIN_FILENO);
							iResult = ::close(iFDStdInCached);
							assert(iResult == 0);
						}

						//Restore The STDERR
						iResult = ::close(STDERR_FILENO);
						assert(iResult == 0);
						if (iFDStdErrCached != -1)
						{
							iResult = ::fcntl(iFDStdErrCached, F_DUPFD, STDERR_FILENO);
							assert(iResult == STDERR_FILENO);
							iResult = ::close(iFDStdErrCached);
							assert(iResult == 0);
						}

						if (!WIFEXITED(iStatusChild) || !(WEXITSTATUS(iStatusChild) == 0))
						{
							::PT_DebugStringOutput("ROOT进程：SU进程发生错误!");
							//ERRNO ------------------------------------------
							char szERRNO[] = "ERRNO: 0XFFFFFFFF";
							uint32_t uiNumber = (WIFEXITED(iStatusChild)) ? (WEXITSTATUS(iStatusChild)) : -1;
							char *pszNumber = szERRNO + sizeof("ERRNO: ") - 1U;
							assert(pszNumber[0] = '0');
							assert(pszNumber[1] = 'X');
							uint32_t uiNumberLength = 10U;
							assert(pszNumber[uiNumberLength] == '\0');
							while (uiNumberLength > 2U)
							{
								bool bResult = ::PT_Number_IntToChar_HEX(uiNumber % 0X10U, &pszNumber[uiNumberLength - 1U]);
								assert(bResult);
								uiNumber /= 0X10U;
								--uiNumberLength;
							}
							::PT_DebugStringOutput(szERRNO);
							//PERROR ------------------------------------------
							char szSystemErrorMessage[4096] = "PERROR: ";
							ssResult = ::read(iFDPairStdErr[0], szSystemErrorMessage + sizeof("PERROR: ") - 1U/*'\0'*/, 4096U - 1U/*'\0'*/ - (sizeof("PERROR: ") - 1U/*'\0'*/));
							assert(ssResult != -1);
							if (ssResult != 0)
							{
								assert(ssResult >= 0);
								(szSystemErrorMessage + sizeof("PERROR: ") - 1U)[ssResult] = '\0';
								::PT_DebugStringOutput(szSystemErrorMessage);
							}
							//Return -1
							bMountSuccess = false;
						}

						iResult = ::close(iFDPairStdErr[0]);
						assert(iResult == 0);
					}

					if (!bMountSuccess)
					{
						//To Do ???
						//I Don't Know Why???
						//But On My Phone //I Execute The "mount" In Android First And Then System Call "mount" //I Success

						iResult = ::access(szMountPointSUIN, R_OK | X_OK);
						if (iResult != 0)
						{
							::PT_DebugStringOutput("ROOT进程：Access SU目录 失败！↓");
							//ERRNO ------------------------------------------
							::PTA_ERRNO();
							//PERROR ------------------------------------------
							::PTA_PERROR(NULL);
							//MountPoint ------------------------------------------
							char szMountPoint[(sizeof("MountPoint: ") - 1U) + PATH_MAX] = "MountPoint: ";
							::PT_DebugStringOutput(szMountPoint, sizeof("MountPoint: ") - 1U, szMountPointSUIN, ::strlen(szMountPointSUIN));
							//------------------------------------------
							::PT_DebugStringOutput("ROOT进程：Access SU目录 失败！↑");
							return -1;
						}

						unsigned long uiMountFlags = MS_REMOUNT | (uiMountFlagsSUIN^MS_RDONLY);
						iResult = ::mount(NULL, szMountPointSUIN, NULL, uiMountFlags, NULL);
						if (iResult != 0)
						{
							::PT_DebugStringOutput("ROOT进程：ReMount SU目录 ReadWrite 失败！↓");
							//ERRNO ------------------------------------------
							::PTA_ERRNO();
							//PERROR ------------------------------------------
							::PTA_PERROR(NULL);
							//MountPoint ------------------------------------------
							char szMountPoint[(sizeof("MountPoint: ") - 1U) + PATH_MAX] = "MountPoint: ";
							::PT_DebugStringOutput(szMountPoint, sizeof("MountPoint: ") - 1U, szMountPointSUIN, ::strlen(szMountPointSUIN));
							//MountFlags ------------------------------------------
							char szMountFlagsBefore[] = "MountFlags: 0XFFFFFFFF";
							::PT_DebugStringOutput(szMountFlagsBefore, sizeof("MountFlags: ") - 1U, uiMountFlags);
							//------------------------------------------
							::PT_DebugStringOutput("ROOT进程：ReMount SU目录 ReadWrite 失败！↑");
							return -1;
						}
					}
				}

				assert((szPathSUINLength + sizeof(szPTAudioOpenEXE)) <= PATH_MAX);
				::memcpy(szPathSUIN + szPathSUINLength, szPTAudioOpenEXE, sizeof(szPTAudioOpenEXE));

				//umask Does Not Affect chmod
				//Ignore To 避免不必要的麻烦
				::umask(0);

				int iFDEXEToWrite = ::open(szPathSUIN, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);

				if (iFDEXEToWrite == -1)
				{
					while (::unlink(szPathSUIN) == 0)
					{
						::PT_DebugStringOutput("ROOT进程：尝试删除SU目录中已存在的EXE文件！↓");
						::PT_DebugStringOutput(szPathSUIN);
						::PT_DebugStringOutput("ROOT进程：尝试删除SU目录中已存在的EXE文件！↑");
					}

					iFDEXEToWrite = ::open(szPathSUIN, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
					if (iFDEXEToWrite == -1)
					{
						::PT_DebugStringOutput("ROOT进程：无法在SU目录中创建EXE文件！↓");
						::PT_DebugStringOutput(szPathSUIN);
						::PT_DebugStringOutput("ROOT进程：无法在SU目录中创建EXE文件！↑");
						return -1;
					}
				}

				//Do The Copy
				char BlockBuffer[4096];
				ssize_t ssResultRead;
				while ((ssResultRead = ::read(iFDEXESelf, BlockBuffer, 4096U)) != 0)
				{
					if (ssResultRead == -1)
					{
						::PT_DebugStringOutput("ROOT进程：复制EXE文件时无法读取！");
						return -1;
					}
					assert(ssResultRead > 0);
					ssize_t ssResultWrite = ::write(iFDEXEToWrite, BlockBuffer, static_cast<size_t>(ssResultRead));
					if (ssResultWrite == -1)
					{
						::PT_DebugStringOutput("ROOT进程：复制EXE文件时无法写入！↓");
						//ERRNO ------------------------------------------
						::PTA_ERRNO();
						//PERROR ------------------------------------------
						::PTA_PERROR(NULL);
						//ByteCountToWrite ------------------------------------------
						char szByteCountToWrite[] = "ByteCountToWrite: 0XFFFFFFFF";
						::PT_DebugStringOutput(szByteCountToWrite, sizeof("ByteCountToWrite: ") - 1U, static_cast<size_t>(ssResultRead));
						//------------------------------------------
						::PT_DebugStringOutput("ROOT进程：复制EXE文件时无法写入！↑");
						return -1;
					}
					assert(ssResultWrite == ssResultRead);
				}

				iResult = ::fchown(iFDEXEToWrite, sFileStatusEXESelf.st_uid, iGIDAudio);
				if (iResult != 0)
				{
					::PT_DebugStringOutput("ROOT进程：无法将SU目录中的PTAudioOpenEXE文件的所有者组群设置为\"audio\"！");
				}

				iResult = ::fchmod(iFDEXEToWrite, S_IRWXU | S_IRGRP | S_IXGRP | S_ISGID | S_IROTH | S_IXOTH);
				if (iResult != 0)
				{
					::PT_DebugStringOutput("ROOT进程：无法开启SU目录中的PTAudioOpenEXE文件的\"设置GID\"特殊权限！");
					return -1;
				}

				int iResult = ::close(iFDEXEToWrite);
				assert(iResult == 0);

				if (bMountPointReadOnlySUIN)
				{
					unsigned long uiMountFlags = MS_REMOUNT | MS_BIND | MS_RDONLY;
					int iResult = ::mount(szMountSourceSUIN, szMountPointSUIN, szFileSystemTypeSUIN, uiMountFlags, NULL);
					if (iResult != 0)
					{
						::PT_DebugStringOutput("ROOT进程：ReMount SU目录 ReadOnly 失败！↓");
						//ERRNO ------------------------------------------
						::PTA_ERRNO();
						//MountPoint ------------------------------------------
						char szMountPoint[(sizeof("MountPoint: ") - 1U) + PATH_MAX] = "MountPoint: ";
						::PT_DebugStringOutput(szMountPoint, sizeof("MountPoint: ") - 1U, szMountPointSUIN, ::strlen(szMountPointSUIN));
						//MountFlags ------------------------------------------
						char szMountFlags[] = "MountFlags: 0XFFFFFFFF";
						::PT_DebugStringOutput(szMountFlags, sizeof("MountFlags: ") - 1U, uiMountFlags);
						//------------------------------------------
						::PT_DebugStringOutput("ROOT进程：ReMount SU目录 ReadOnly 失败！↑");
						return -1;
					}
				}

				iResult = ::close(iFDEXESelf);
				assert(iResult == 0);

				bool bMountPointExeSelfReadOnly = (uiMountFlagsExeSelf&MS_RDONLY);
				if (bMountPointExeSelfReadOnly)
				{
					bool bMountSuccess = true;
					{
						//The Most Robust Way To Use The "mount" In Android !!!

						int iFDPairStdIn[2];
						iResult = ::socketpair(AF_UNIX, SOCK_STREAM, 0, iFDPairStdIn);
						assert(iResult == 0);
						int iFDPairStdErr[2];
						iResult = ::socketpair(AF_UNIX, SOCK_STREAM, 0, iFDPairStdErr);
						assert(iResult == 0);

						//Use Socket To Hook STDIN
						int iFDStdInCached = ::fcntl(STDIN_FILENO, F_DUPFD, 0);
						if (iFDStdInCached != -1)
						{
							iResult = ::close(STDIN_FILENO);
							assert(iResult == 0);
						}
						iResult = ::fcntl(iFDPairStdIn[0], F_DUPFD, STDIN_FILENO);
						assert(iResult == STDIN_FILENO);
						iResult = ::close(iFDPairStdIn[0]);
						assert(iResult == 0);

						//Use Socket To Hook STDERR
						int iFDStdErrCached = ::fcntl(STDERR_FILENO, F_DUPFD, 0);
						if (iFDStdErrCached != -1)
						{
							iResult = ::close(STDERR_FILENO);
							assert(iResult == 0);
						}
						iResult = ::fcntl(iFDPairStdErr[1], F_DUPFD, STDERR_FILENO);
						assert(iResult == STDERR_FILENO);
						iResult = ::close(iFDPairStdErr[1]);
						assert(iResult == 0);

						//Write Command To The ROOT Shell
						ssize_t ssResult = ::write(iFDPairStdIn[1], "mount -o remount,rw ", sizeof("mount -o remount,rw ") - 1U/*'\0'*/);
						assert(ssResult != -1);
						ssResult = ::write(iFDPairStdIn[1], szMountSourceExeSelf, ::strlen(szMountSourceExeSelf));
						assert(ssResult != -1);
						ssResult = ::write(iFDPairStdIn[1], " ", sizeof(" ") - 1U/*'\0'*/);
						assert(ssResult != -1);
						ssResult = ::write(iFDPairStdIn[1], szMountPointExeSelf, ::strlen(szMountPointExeSelf) + 1U/*'\0'*/);
						assert(ssResult != -1);

						//Read Will Return 0 And Then The Child Process That We Wait Can Exit
						iResult = ::close(iFDPairStdIn[1]);
						assert(iResult == 0);

						//How-To SU //Guidelines for problem-free su usage //https://su.chainfire.eu
						//We Need To Swith Context Under The SELinux //Otherwise, The Mount Will Fail！
						int iStatusChild = ::system("su");

						//Restore The STDIN
						iResult = ::close(STDIN_FILENO);
						assert(iResult == 0);
						if (iFDStdInCached != -1)
						{
							iResult = ::fcntl(iFDStdInCached, F_DUPFD, STDIN_FILENO);
							assert(iResult == STDIN_FILENO);
							iResult = ::close(iFDStdInCached);
							assert(iResult == 0);
						}

						//Restore The STDERR
						iResult = ::close(STDERR_FILENO);
						assert(iResult == 0);
						if (iFDStdErrCached != -1)
						{
							iResult = ::fcntl(iFDStdErrCached, F_DUPFD, STDERR_FILENO);
							assert(iResult == STDERR_FILENO);
							iResult = ::close(iFDStdErrCached);
							assert(iResult == 0);
						}

						if (!WIFEXITED(iStatusChild) || !(WEXITSTATUS(iStatusChild) == 0))
						{
							::PT_DebugStringOutput("ROOT进程：SU进程发生错误!");
							//ERRNO ------------------------------------------
							char szERRNO[] = "ERRNO: 0XFFFFFFFF";
							uint32_t uiNumber = (WIFEXITED(iStatusChild)) ? (WEXITSTATUS(iStatusChild)) : -1;
							char *pszNumber = szERRNO + sizeof("ERRNO: ") - 1U;
							assert(pszNumber[0] = '0');
							assert(pszNumber[1] = 'X');
							uint32_t uiNumberLength = 10U;
							assert(pszNumber[uiNumberLength] == '\0');
							while (uiNumberLength > 2U)
							{
								bool bResult = ::PT_Number_IntToChar_HEX(uiNumber % 0X10U, &pszNumber[uiNumberLength - 1U]);
								assert(bResult);
								uiNumber /= 0X10U;
								--uiNumberLength;
							}
							::PT_DebugStringOutput(szERRNO);
							//PERROR ------------------------------------------
							char szSystemErrorMessage[4096] = "PERROR: ";
							ssResult = ::read(iFDPairStdErr[0], szSystemErrorMessage + sizeof("PERROR: ") - 1U/*'\0'*/, 4096U - 1U/*'\0'*/ - (sizeof("PERROR: ") - 1U/*'\0'*/));
							assert(ssResult != -1);
							if (ssResult != 0)
							{
								assert(ssResult >= 0);
								(szSystemErrorMessage + sizeof("PERROR: ") - 1U)[ssResult] = '\0';
								::PT_DebugStringOutput(szSystemErrorMessage);
							}
							//Return -1
							bMountSuccess = false;
						}

						iResult = ::close(iFDPairStdErr[0]);
						assert(iResult == 0);
					}

					if (!bMountSuccess)
					{
						unsigned long uiMountFlags = MS_REMOUNT | (uiMountFlagsExeSelf^MS_RDONLY);
						iResult = ::mount(szMountSourceExeSelf, szMountPointExeSelf, szFileSystemTypeExeSelf, uiMountFlags, NULL);
						if (iResult != 0)
						{
							::PT_DebugStringOutput("ROOT进程：ReMount 私有目录 ReadWrite 失败！↓");
							//ERRNO ------------------------------------------
							::PTA_ERRNO();
							//PError
							::PTA_PERROR(NULL);
							//MountPoint ------------------------------------------
							char szMountPoint[(sizeof("MountPoint: ") - 1U) + PATH_MAX] = "MountPoint: ";
							::PT_DebugStringOutput(szMountPoint, sizeof("MountPoint: ") - 1U, szMountPointExeSelf, ::strlen(szMountPointExeSelf));
							//MountFlags ------------------------------------------
							char szMountFlags[] = "MountFlags: 0XFFFFFFFF";
							::PT_DebugStringOutput(szMountFlags, sizeof("MountFlags: ") - 1U, uiMountFlags);
							//------------------------------------------
							::PT_DebugStringOutput("ROOT进程：ReMount 私有目录 ReadWrite 失败！↑");
							return -1;
						}
					}
				}

				while (::unlink(szPathEXESelf) == 0)
				{
					::PT_DebugStringOutput("ROOT进程：尝试删除私有目录中的EXE文件！↓");
					::PT_DebugStringOutput(szPathEXESelf);
					::PT_DebugStringOutput("ROOT进程：尝试删除私有目录中的EXE文件！↑");
				}

				iResult = ::symlink(szPathSUIN, szPathEXESelf);
				if (iResult == -1)
				{
					::PT_DebugStringOutput("ROOT进程：无法创建EXE文件的符号链接！");
					return -1;
				}

				if (bMountPointExeSelfReadOnly)
				{
					unsigned long uiMountFlags = MS_REMOUNT | MS_BIND | MS_RDONLY;
					iResult = ::mount(szMountSourceExeSelf, szMountPointExeSelf, szFileSystemTypeExeSelf, uiMountFlags, NULL);
					if (iResult != 0)
					{
						::PT_DebugStringOutput("ROOT进程：ReMount 私有目录 ReadOnly 失败！↓");
						//ERRNO ------------------------------------------
						char szERRNO[] = "ERRNO: 0XFFFFFFFF";
						::PT_DebugStringOutput(szERRNO, sizeof("ERRNO: ") - 1U, errno);
						//MountPoint ------------------------------------------
						char szMountPoint[(sizeof("MountPoint: ") - 1U) + PATH_MAX] = "MountPoint: ";
						::PT_DebugStringOutput(szMountPoint, sizeof("MountPoint: ") - 1U, szMountPointExeSelf, ::strlen(szMountPointExeSelf));
						//MountFlags ------------------------------------------
						char szMountFlags[] = "MountFlags: 0XFFFFFFFF";
						::PT_DebugStringOutput(szMountFlags, sizeof("MountFlags: ") - 1U, uiMountFlags);
						//------------------------------------------
						::PT_DebugStringOutput("ROOT进程：ReMount 私有目录 ReadOnly 失败！↑");
						return -1;
					}
				}

				return 0;
			}
			else
			{
				iResult = ::fchown(iFDEXESelf, sFileStatusEXESelf.st_uid, iGIDAudio);
				if (iResult != 0)
				{
					::PT_DebugStringOutput("ROOT进程：无法将私有目录中EXE文件的所有者组群设置为\"audio\"！");
					return -1;
				}

				iResult = ::fchmod(iFDEXESelf, S_IRWXU | S_IRGRP | S_IXGRP | S_ISGID | S_IROTH | S_IXOTH);
				if (iResult != 0)
				{
					::PT_DebugStringOutput("ROOT进程：无法开启私有目录中EXE文件的\"设置GID\"特殊权限！");
					return -1;
				}

				iResult = ::close(iFDEXESelf);
				assert(iResult == 0);

				return 0;
			}
		}
		else
		{
			//We Use SU To Request ROOT Privilege

			//Argv0
			//The EXE File MayBe Replaced By A SymbolLink By The ROOT Process
			//To Solve The Problem Caused By The MS_NOSUID
			//Kernel Will Mark The EXE File Deleted If We ReadLink Later
			//So We ReadLink Here
			char szPathEXE[PATH_MAX];
			ssize_t ssPathEXELength = ::readlink("/proc/self/exe", szPathEXE, PATH_MAX - 1);
			szPathEXE[ssPathEXELength] = '\0';

#if defined(PTPOSIX)
#if defined(PTPOSIXXCB)
			pid_t iPIDChild = ::fork();
			assert(iPIDChild != -1);
			if (iPIDChild == 0)
			{
				//We Are Child
				//在PosixXCB下，暂时依赖于kdesu
				//尝试创建新的会话，并用伪终端作为控制终端，以取消对kdesu的依赖

				//Argv0
				char szPathSU[] = "kdesu";
				//Argv1
				char szOptionC[] = "-c";
				//Argv2
				//szPathEXE
				//Argv
				char *Argv[] = { szPathSU,szOptionC,szPathEXE,NULL };

				int iResult = ::execvp(Argv[0], Argv); //We Should Find kdesu In PATH and kdesu Will Use The Envp
				assert(iResult == -1);
				{
					char pDebugString[] = "非特权进程：无法创建SU进程！\n";
					PT_DebugStringOutput(pDebugString);
					return -1;
				}
			}

			//We Are Parent
			int iStatusChild;
			pid_t iPIDResult = ::waitpid(iPIDChild, &iStatusChild, 0);
			assert(iPIDResult != -1);

			if (!WIFEXITED(iStatusChild) || !(WEXITSTATUS(iStatusChild) == 0))
			{
				char pDebugString[] = "非特权进程：SU进程发生错误！\n";
				PT_DebugStringOutput(pDebugString);
				return -1;
			}
#elif defined(PTPOSIXANDROID)
			//szPathEXE

			//The Most Robust Way To Use The "su" In Android !!!
			int iFDPairStdIn[2];
			int iResult = ::socketpair(AF_UNIX, SOCK_STREAM, 0, iFDPairStdIn);
			assert(iResult == 0);

			//Use Socket To Hook STDIN
			int iFDStdInCached = ::fcntl(STDIN_FILENO, F_DUPFD, 0);
			if (iFDStdInCached != -1)
			{
				iResult = ::close(STDIN_FILENO);
				assert(iResult == 0);
			}
			iResult = ::fcntl(iFDPairStdIn[0], F_DUPFD, STDIN_FILENO);
			assert(iResult == STDIN_FILENO);
			iResult = ::close(iFDPairStdIn[0]);
			assert(iResult == 0);

			//Write Command To The ROOT Shell
			ssize_t ssResult = ::write(iFDPairStdIn[1], szPathEXE, ssPathEXELength + 1U/*'\0'*/);
			assert(ssResult != -1);

			//Read Will Return 0 And Then The Child Process That We Wait Can Exit
			iResult = ::close(iFDPairStdIn[1]);
			assert(iResult == 0);

			//How-To SU //Guidelines for problem-free su usage //https://su.chainfire.eu
			//We Need To Swith Context Under The SELinux //Otherwise, The Mount Will Fail！
			int iStatusChild = ::system("su");

			//Restore The STDIN
			iResult = ::close(STDIN_FILENO);
			assert(iResult == 0);
			if (iFDStdInCached != -1)
			{
				iResult = ::fcntl(iFDStdInCached, F_DUPFD, STDIN_FILENO);
				assert(iResult == STDIN_FILENO);
				iResult = ::close(iFDStdInCached);
				assert(iResult == 0);
			}

			if (!WIFEXITED(iStatusChild) || !(WEXITSTATUS(iStatusChild) == 0))
			{
				::PT_DebugStringOutput("非特权进程：SU进程发生错误!");
				return -1;
			}
#else
#error 未知的平台
#endif
#else
#error 未知的平台
#endif
			//We Execute Audio Process
			//GID Audio And SET-GID-FLAG Has Done!
			{
				//Argv1 Argv2 And Argv3
				if (argc < 4)
				{
					::PT_DebugStringOutput("非特权进程：Invalid Argument !");
					::PT_DebugStringOutput("非特权进程：Usage：.PTAudioOpenHelper.sh SockAddrSeverPath FileToOpenPath !");

					if (argc >= 1)
					{
						::PT_DebugStringOutput("非特权进程：Argv[0]:----↓");
						::PT_DebugStringOutput(argv[0]);
						::PT_DebugStringOutput("非特权进程：Argv[0]:----↑");
					}

					if (argc >= 2)
					{
						::PT_DebugStringOutput("非特权进程：Argv[1]:----↓");
						::PT_DebugStringOutput(argv[1]);
						::PT_DebugStringOutput("非特权进程：Argv[10]:----↑");
					}

					if (argc >= 3)
					{
						::PT_DebugStringOutput("非特权进程：Argv[2]:----↓");
						::PT_DebugStringOutput(argv[2]);
						::PT_DebugStringOutput("非特权进程：Argv[2]:----↑");
					}

					return -1;
				}

				//Argv
				char *Argv[] = { szPathEXE,argv[1],argv[2],argv[3],NULL };

				//char *Envp[] = { NULL };
				//int iResult = ::execve(Argv[0], Argv, Envp); //We Don't Need The Envp

				int iResult = ::execv(Argv[0], Argv); //The Kernel May Optimal This (For Example, Use Copy-On-Write For environ) And We Follow The Convertion
				assert(iResult == -1);
				{
					::PT_DebugStringOutput("非特权进程：无法创建Audio进程！");
					return -1;
				}
			}
		}
	}
}

#if defined(PTPOSIX)
#if defined(PTPOSIXXCB)
static inline void PT_DebugStringOutput(char const *pDebugString)
{
#ifndef NDEBUG
	::write(STDOUT_FILENO, pDebugString, ::strlen(pDebugString) + 1U);
	::write(STDOUT_FILENO, "\n", 2U);
#endif
}
#elif defined(PTPOSIXANDROID)
#ifndef NDEBUG
#include <android/log.h>
#endif
static inline void PT_DebugStringOutput(char const *pDebugString)
{
#ifndef NDEBUG
	::__android_log_write(ANDROID_LOG_ERROR, "PTALSAOpenServer", pDebugString);
#endif
}
#else
#error 未知的平台
#endif
#else
#error 未知的平台
#endif

static inline void PTA_ERRNO()
{
	char szERRNO[] = "ERRNO: 0XFFFFFFFF";
	uint32_t uiNumber = errno;
	char *pszNumber = szERRNO + sizeof("ERRNO: ") - 1U;
	assert(pszNumber[0] = '0');
	assert(pszNumber[1] = 'X');
	uint32_t uiNumberLength = 10U;
	assert(pszNumber[uiNumberLength] == '\0');
	while (uiNumberLength > 2U)
	{
		bool bResult = ::PT_Number_IntToChar_HEX(uiNumber % 0X10U, &pszNumber[uiNumberLength - 1U]);
		assert(bResult);
		uiNumber /= 0X10U;
		--uiNumberLength;
	}
	::PT_DebugStringOutput(szERRNO);
}

#if defined(PTPOSIX)
#if defined(PTPOSIXXCB)
#elif defined(PTPOSIXANDROID)
static inline void PTA_PERROR(char const *szMsg)
{
#ifndef NDEBUG
	int iFDPairStdErr[2];
	int iResult = ::socketpair(AF_UNIX, SOCK_STREAM, 0, iFDPairStdErr);
	assert(iResult == 0);

	//Use Socket To Hook STDERR
	int iFDStdErrCached = ::fcntl(STDERR_FILENO, F_DUPFD, 0);
	if (iFDStdErrCached != -1)
	{
		iResult = ::close(STDERR_FILENO);
		assert(iResult == 0);
	}
	iResult = ::fcntl(iFDPairStdErr[1], F_DUPFD, STDERR_FILENO);
	assert(iResult == STDERR_FILENO);
	iResult = ::close(iFDPairStdErr[1]);
	assert(iResult == 0);

	//PError In Android
	::perror(szMsg);

	//Restore The STDERR
	iResult = ::close(STDERR_FILENO);
	assert(iResult == 0);
	if (iFDStdErrCached != -1)
	{
		iResult = ::fcntl(iFDStdErrCached, F_DUPFD, STDERR_FILENO);
		assert(iResult == STDERR_FILENO);
		iResult = ::close(iFDStdErrCached);
		assert(iResult == 0);
	}

	char szSystemErrorMessage[4096] = "PERROR: ";
	ssize_t ssResult = ::read(iFDPairStdErr[0], szSystemErrorMessage + sizeof("PERROR: ") - 1U/*'\0'*/, 4096U - 1U/*'\0'*/ - (sizeof("PERROR: ") - 1U/*'\0'*/));
	assert(ssResult != -1);
	if (ssResult != 0)
	{
		assert(ssResult >= 0);
		(szSystemErrorMessage + sizeof("PERROR: ") - 1U)[ssResult] = '\0';
		::PT_DebugStringOutput(szSystemErrorMessage);
	}

	iResult = ::close(iFDPairStdErr[0]);
	assert(iResult == 0);
#endif
}

#else
#error 未知的平台
#endif
#else
#error 未知的平台
#endif

static inline void PT_DebugStringOutput(char *pszPrefix /* Size Must Equal To uiPrefixLength + 10(0XFFFFFFFF) + 1('\0')(Must Be '\0' Number Must Be End No More Char) */, uint32_t uiPrefixLength, uint32_t uiNumber)
{
#ifndef NDEBUG
	char *pszNumber = pszPrefix + uiPrefixLength;
	assert(pszNumber[0] = '0');
	assert(pszNumber[1] = 'X');
	uint32_t uiNumberLength = 10U;
	//Number Must Be End No More Char
	assert(pszNumber[uiNumberLength] == '\0');
	while (uiNumberLength > 2U)
	{
		bool bResult = ::PT_Number_IntToChar_HEX(uiNumber % 0X10U, &pszNumber[uiNumberLength - 1U]);
		assert(bResult);
		uiNumber /= 0X10U;
		--uiNumberLength;
	}
	::PT_DebugStringOutput(pszPrefix);
#endif
}

static inline void PT_DebugStringOutput(char *pszPrefix /* Size Must Equal To uiPrefixLength + uiContentLength + 1('\0') */, uint32_t uiPrefixLength, char const *pszContent /* Size Must Equal To uiContentLength + 1('\0')(Must Be '\0' No More Char) */, uint32_t uiContentLength)
{
#ifndef NDEBUG
	assert(pszContent[uiContentLength] == '\0');
	::memcpy(pszPrefix + uiPrefixLength, pszContent, uiContentLength + 1U);
	::PT_DebugStringOutput(pszPrefix);
#endif
}

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

static inline bool PT_Number_CharToInt_HEX(char C, uint32_t *pI)
{
	switch (C)
	{
	case '0':
		(*pI) = 0U;
		return true;
	case '1':
		(*pI) = 1U;
		return true;
	case '2':
		(*pI) = 2U;
		return true;
	case '3':
		(*pI) = 3U;
		return true;
	case '4':
		(*pI) = 4U;
		return true;
	case '5':
		(*pI) = 5U;
		return true;
	case '6':
		(*pI) = 6U;
		return true;
	case '7':
		(*pI) = 7U;
		return true;
	case '8':
		(*pI) = 8U;
		return true;
	case '9':
		(*pI) = 9U;
		return true;
	case 'A':
		(*pI) = 10U;
		return true;
	case 'B':
		(*pI) = 11U;
		return true;
	case 'C':
		(*pI) = 12U;
		return true;
	case 'D':
		(*pI) = 13U;
		return true;
	case 'E':
		(*pI) = 14U;
		return true;
	case 'F':
		(*pI) = 15U;
		return true;
	default:
		return false;
	}
}

static inline bool PT_Number_CharIsInt_DEC(char C)
{
	switch (C)
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return true;
	default:
		return false;
	}
}

static inline bool PT_Number_CharToInt_DEC(char C, int32_t *pI)
{
	switch (C)
	{
	case '0':
		(*pI) = 0;
		return true;
	case '1':
		(*pI) = 1;
		return true;
	case '2':
		(*pI) = 2;
		return true;
	case '3':
		(*pI) = 3;
		return true;
	case '4':
		(*pI) = 4;
		return true;
	case '5':
		(*pI) = 5;
		return true;
	case '6':
		(*pI) = 6;
		return true;
	case '7':
		(*pI) = 7;
		return true;
	case '8':
		(*pI) = 8;
		return true;
	case '9':
		(*pI) = 9;
		return true;
	default:
		return false;
	}
}

static inline bool PT_Info_DeviceIDToMount(char *szPathSource /* Size Is PATH_MAX */, char *szPathTarget /* Size Is PATH_MAX */, char *szFileSystemType /* Size Is 256 */, unsigned long *puiMountFlags, dev_t iDeviceID)
{
	static constexpr uint32_t const s_Page_Size = 1024U * 4U;

	int iFDProcSelfMountInfo = ::open("/proc/self/mountinfo", O_RDONLY);
	assert(iFDProcSelfMountInfo != -1);

	char szRowVector[4096U];
	assert(s_Page_Size == 4096U);

	bool bIsFound = false;

	ssize_t ssResult;
	while ((ssResult = ::read(iFDProcSelfMountInfo, szRowVector, 4096U)) != 0)
	{
		assert(ssResult != -1);
		assert(ssResult < 4096);
		assert(szRowVector[ssResult - 1] == '\n');
		szRowVector[ssResult] = '\0';

		char *szRowBegin = szRowVector;

		while ((*szRowBegin) != '\0')
		{
			char *szRowEnd = szRowBegin;

			//MountID
			while ((*szRowEnd) != ' ')
			{
				assert(::PT_Number_CharIsInt_DEC(*szRowEnd));
				++szRowEnd;
			}

			assert((*szRowEnd) == ' ');
			while ((*szRowEnd) == ' ')
			{
				++szRowEnd;
			}

			//ParentID
			while ((*szRowEnd) != ' ')
			{
				assert(::PT_Number_CharIsInt_DEC(*szRowEnd));
				++szRowEnd;
			}

			assert((*szRowEnd) == ' ');
			while ((*szRowEnd) == ' ')
			{
				++szRowEnd;
			}

			//MajorID
			int32_t MajorID = 0;
			assert(::PT_Number_CharIsInt_DEC(*szRowEnd));
			{
				int32_t ValueToAdd;
				while (::PT_Number_CharToInt_DEC(*szRowEnd, &ValueToAdd))
				{
					MajorID *= 10U;
					MajorID += ValueToAdd;
					++szRowEnd;
				}
			}

			assert((*szRowEnd) == ':');
			++szRowEnd;

			//MinorID
			int32_t MinorID = 0;
			assert(::PT_Number_CharIsInt_DEC(*szRowEnd));
			{
				int32_t ValueToAdd;
				while (::PT_Number_CharToInt_DEC(*szRowEnd, &ValueToAdd))
				{
					MinorID *= 10U;
					MinorID += ValueToAdd;
					++szRowEnd;
				}
			}

			//DeviceID
			if (makedev(MajorID, MinorID) != iDeviceID)
			{
				while ((*szRowEnd) != '\n')
				{
					assert((*szRowEnd) != '\0');
					++szRowEnd;
				}

				szRowBegin = szRowEnd + 1;
			}
			else
			{
				assert((*szRowEnd) == ' ');
				while ((*szRowEnd) == ' ')
				{
					++szRowEnd;
				}

				//Root
				assert((*szRowEnd) == '/');
				while ((*szRowEnd) != ' ')
				{
					assert((*szRowEnd) != '\0');
					++szRowEnd;
				}

				assert((*szRowEnd) == ' ');
				while ((*szRowEnd) == ' ')
				{
					++szRowEnd;
				}

				//Mount Point
				char *szPathMountPointBegin = szRowEnd;
				assert((*szRowEnd) == '/');
				while ((*szRowEnd) != ' ')
				{
					assert((*szRowEnd) != '\0');
					++szRowEnd;
				}
				char *szPathMountPointEnd = szRowEnd;

				//The TargetPath Where The DeviceID Present
				uint32_t szPathTargetLength = uint32_t(szPathMountPointEnd - szPathMountPointBegin);
				assert(szPathTargetLength < PATH_MAX);
				::memcpy(szPathTarget, szPathMountPointBegin, szPathTargetLength);
				szPathTarget[szPathTargetLength] = '\0';

				assert((*szRowEnd) == ' ');
				while ((*szRowEnd) == ' ')
				{
					++szRowEnd;
				}

				//Mount Options And Super options

				char *szOptionBegin = NULL;
				char *szOptionEnd = NULL;
				(*puiMountFlags) = 0U;

				//Mount Options
				//ro MS_RDONLY
				//rw
				//nosuid MS_NOSUID
				//nodev MS_NODEV
				//noexec MS_NOEXEC
				//nodiratime MS_NODIRATIME
				//relatime MS_RELATIME
				//noatime MS_NOATIME

				//The Order Is Predefined //So We Can Optimize

				struct
				{
					const char *m_szMountFlag;
					unsigned long m_uiMountFlag;
				}MountOptionVector[] = {
					{ "ro",MS_RDONLY },
				{ "rw",0U },
				{ "nosuid",MS_NOSUID },
				{ "nodev",MS_NODEV },
				{ "noexec",MS_NOEXEC },
				{ "nodiratime",0U },
				{ "relatime",0U },
				{ "noatime",0U },
				};

				constexpr uint32_t const MountOptionVectorLength = sizeof(MountOptionVector) / sizeof(MountOptionVector[0]);
				uint32_t uiIndexMountOptionVectorBegin = 0U;

				szOptionBegin = szRowEnd;
				do {
					szOptionEnd = szOptionBegin;
					while ((*szOptionEnd) != ' ')
					{
						assert((*szOptionEnd) != '\0');
						if ((*szOptionEnd) == ',')
						{
							break;
						}
						++szOptionEnd;
					}

					//There MayBe Some Other Flags We Don't Care
					//assert(uiIndexMountOptionVectorBegin < MountOptionVectorLength);

					for (uint32_t uiIndexMountOption = uiIndexMountOptionVectorBegin; uiIndexMountOption < MountOptionVectorLength; ++uiIndexMountOption)
					{
						if ([szOptionBegin, szOptionEnd, &MountOptionVector, uiIndexMountOption](char const *)->bool
						{
							for (int iIndexChar = 0; szOptionBegin + iIndexChar < szOptionEnd; ++iIndexChar)
							{
								if (szOptionBegin[iIndexChar] != MountOptionVector[uiIndexMountOption].m_szMountFlag[iIndexChar])
								{
									return false;
								}
							}
							return true;
						}("我们是共产主义接班人！"))
						{
							(*puiMountFlags) |= MountOptionVector[uiIndexMountOption].m_uiMountFlag;
							uiIndexMountOptionVectorBegin = uiIndexMountOption + 1U;
							break;
						}
					}

					szOptionBegin = szOptionEnd + 1;

				} while ((*szOptionEnd) != ' ');

				szRowEnd = szOptionEnd;

				assert((*szRowEnd) == ' ');

				//Optional Fields
				//We Don't Care

				//Separator
				while ((*szRowEnd) != '-')
				{
					assert((*szRowEnd) != '\0');
					assert((*szRowEnd) != '\n');
					++szRowEnd;
				}

				++szRowEnd;
				assert((*szRowEnd) == ' ');
				while ((*szRowEnd) == ' ')
				{
					++szRowEnd;
				}

				//FileSystem Type
				char *szFileSystemTypeBegin = szRowEnd;
				while ((*szRowEnd) != ' ')
				{
					assert((*szRowEnd) != '\0');
					++szRowEnd;
				}
				char *szFileSystemTypeEnd = szRowEnd;

				//The SourcePath Where The DeviceID Present
				uint32_t szFileSystemTypeLength = uint32_t(szFileSystemTypeEnd - szFileSystemTypeBegin);
				assert(szFileSystemTypeLength < 256U);
				::memcpy(szFileSystemType, szFileSystemTypeBegin, szFileSystemTypeLength);
				szFileSystemType[szFileSystemTypeLength] = '\0';

				assert((*szRowEnd) == ' ');
				while ((*szRowEnd) == ' ')
				{
					++szRowEnd;
				}

				//Mount Source
				char *szPathMountSourceBegin = szRowEnd;
				assert((*szRowEnd) == '/');
				while ((*szRowEnd) != ' ')
				{
					assert((*szRowEnd) != '\0');
					++szRowEnd;
				}
				char *szPathMountSourceEnd = szRowEnd;

				//The SourcePath Where The DeviceID Present
				uint32_t szPathSourceLength = uint32_t(szPathMountSourceEnd - szPathMountSourceBegin);
				assert(szPathSourceLength < PATH_MAX);
				::memcpy(szPathSource, szPathMountSourceBegin, szPathSourceLength);
				szPathSource[szPathSourceLength] = '\0';

				assert((*szRowEnd) == ' ');
				while ((*szRowEnd) == ' ')
				{
					++szRowEnd;
				}

				//Super Options

				//ro MS_RDONLY
				//rw
				//sync MS_SYNCHRONOUS
				//mand MS_MANDLOCK
				//lazytime MS_LAZYTIME

				//The Data Argument
				//To Do: We Just Pass NULL To Data Currently

				//The Order Is Predefined //So We Can Optimize

				struct
				{
					const char *m_szMountFlag;
					unsigned long m_uiMountFlag;
				}SuperOptionVector[] = {
					{ "ro",MS_RDONLY },
				{ "rw",0U },
				{ "sync",MS_SYNCHRONOUS },
				{ "mand",0U }, //Need Cap Above Kernel 4.5 //So We Ignore
				{ "lazytime",MS_LAZYTIME }
				};

				constexpr uint32_t const SuperOptionVectorLength = sizeof(SuperOptionVector) / sizeof(SuperOptionVector[0]);
				uint32_t uiIndexSuperOptionVectorBegin = 0U;

				szOptionBegin = szRowEnd;
				do {
					szOptionEnd = szOptionBegin;
					while ((*szOptionEnd) != ' ')
					{
						assert((*szOptionEnd) != '\0');
						if ((*szOptionEnd) == ',')
						{
							break;
						}
						++szOptionEnd;
					}

					for (uint32_t uiIndexSuperOption = uiIndexSuperOptionVectorBegin; uiIndexSuperOption < SuperOptionVectorLength; ++uiIndexSuperOption)
					{
						if ([szOptionBegin, szOptionEnd, &SuperOptionVector, uiIndexSuperOption](char const *)->bool
						{
							for (int iIndexChar = 0; szOptionBegin + iIndexChar < szOptionEnd; ++iIndexChar)
							{
								if (szOptionBegin[iIndexChar] != SuperOptionVector[uiIndexSuperOption].m_szMountFlag[iIndexChar])
								{
									return false;
								}
							}
							return true;
						}("我们是共产主义接班人！"))
						{
							(*puiMountFlags) |= SuperOptionVector[uiIndexSuperOption].m_uiMountFlag;
							uiIndexSuperOptionVectorBegin = uiIndexSuperOption + 1U;
							break;
						}
					}

					szOptionBegin = szOptionEnd + 1;

				} while ((*szOptionEnd) != ' ');

				bIsFound = true;
				break;
			}
		}

		if (bIsFound)
		{
			break;
		}
	}

	int iResult = ::close(iFDProcSelfMountInfo);
	assert(iResult == 0);

	return bIsFound;
}
