#ifndef PT_SYSTEM_FILE_H
#define PT_SYSTEM_FILE_H

#include "../PTCommon.h"
#include "PTSCommon.h"
#include "PTSMemoryAllocator.h"

struct IPTSFile
{
	enum PTSFileMoveMethod :uint32_t
	{
		Begin = 0,
		Current = 1,
		End = 2
	};

	virtual void PTCALL Destruct() = 0;
	virtual int64_t PTCALL Size_Get() = 0;
	virtual uint32_t PTCALL Read(void *pBuffer, uint32_t nNumberOfBytesToRead) = 0;
	virtual uint32_t PTCALL Write(void *pBuffer, uint32_t nNumberOfBytesToWrite) = 0;
	virtual int64_t PTCALL Seek(PTSFileMoveMethod eMoveMethod, int64_t nDistanceToMove) = 0;
};

struct alignas(alignof(uintptr_t)) PTSFileStorage { char _Pad[sizeof(uintptr_t) + sizeof(uintptr_t) + sizeof(uintptr_t) + sizeof(uintptr_t) + sizeof(uintptr_t) + sizeof(uintptr_t) + sizeof(uintptr_t) + sizeof(uintptr_t)]; };

//二进制兼容
//将以下代码添加到 PTSFileCustomImpl 的头文件中
//static_assert((alignof(PTSFileStorage) % alignof(PTSFileCustomImpl)) == 0, "PTSFileStorage Not Align");
//static_assert(sizeof(PTSFileStorage) >= sizeof(PTSFileCustomImpl), "PTSFileStorage Too Small");

struct IPTSFileSystem
{
	enum PTSFileOpenMode :uint32_t
	{
		ReadOnly = 0,
		ReadWrite = 1
	};

	virtual void PTCALL Destruct() = 0;

	virtual IPTSFile * PTCALL PTSFile_Construct(PTSFileStorage *pStorage, char const *pFileName, PTSFileOpenMode eOpenMode) = 0;

#if 0
	inline IPTSFile * PTCALL PTSFile_Create(IPTSHeap *pHeap, const char *pFileName, PTSFileOpenMode eOpenMode)
	{
		PTSFileStorage *pStorage = static_cast<PTSFileStorage *>(pHeap->AlignedAlloc(sizeof(PTSFileStorage), alignof(PTSFileStorage)));
		return (pStorage != NULL) ? this->PTSFile_Construct(pStorage, pFileName, eOpenMode) : NULL;
	}
#endif

	virtual void PTCALL RootPath_Get(char *pPathName, size_t PathLength) = 0;
};

struct alignas(alignof(uintptr_t)) PTSFileSystemStorage 
{ 
	char _Pad[sizeof(uintptr_t)
		+ sizeof(uintptr_t)
		+ sizeof(uintptr_t)
		+ sizeof(uintptr_t)
		+ sizeof(uintptr_t)
		+ sizeof(uintptr_t)
		+ sizeof(uintptr_t)
		+ sizeof(uintptr_t)];
};

//二进制兼容
//将以下代码添加到 PTSFileSystemCustomImpl 的头文件中
//static_assert((alignof(PTSFileSystemStorage) % alignof(PTSFileSystemCustomImpl)) == 0, "PTSFileSystemStorage Not Align");
//static_assert(sizeof(PTSFileSystemStorage) >= sizeof(PTSFileSystemCustomImpl), "PTSFileSystemStorage Too Small");

extern "C" PTSYSTEMAPI IPTSFileSystem * PTCALL PTSFileSystem_Construct(PTSFileSystemStorage *pStorage);

#if 0
inline IPTSFileSystem * PTCALL PTSFileSystem_Create(IPTSHeap *pHeap)
{
	PTSFileSystemStorage *pStorage = static_cast<PTSFileSystemStorage *>(pHeap->AlignedAlloc(sizeof(PTSFileSystemStorage), alignof(PTSFileSystemStorage)));
	return (pStorage != NULL) ? ::PTSFileSystem_Construct(pStorage) : NULL;
};
#endif

#endif
