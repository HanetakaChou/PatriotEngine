#include "PTSFileImpl_Win32Desktop.h"
#include <new>
#include <string.h>
#include <assert.h>
#include "../../../Public/System/PTSMemoryAllocator.h"
#include "../../../Public/System/PTSThread.h"
#include "../../../Public/System/Win32/PTSConvUTF.h"

inline PTSFileSystemImpl::PTSFileSystemImpl()
{

}

inline PTSFileSystemImpl::~PTSFileSystemImpl()
{

}

void PTCALL PTSFileSystemImpl::Internal_Release()
{
	this->~PTSFileSystemImpl();
	::PTSMemoryAllocator_Free_Aligned(this);
}

static PTSFileSystemImpl *s_FileSystem_Singleton_Pointer = NULL;

static int32_t s_FileSystem_Initialize_RefCount = 0;
extern "C" PTSYSTEMAPI PTBOOL PTCALL PTSFileSystem_Initialize()
{
	if (::PTSAtomic_GetAndAdd(&s_FileSystem_Initialize_RefCount, 1) == 0)
	{
		assert(s_FileSystem_Singleton_Pointer == NULL);
		s_FileSystem_Singleton_Pointer = new(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSFileSystemImpl), alignof(PTSFileSystemImpl)))PTSFileSystemImpl{};
		assert(s_FileSystem_Singleton_Pointer != NULL);
		return PTTRUE;
	}
	else
	{
		return PTTRUE;
	}
}

extern "C" PTSYSTEMAPI IPTSFileSystem * PTCALL PTSFileSystem_ForProcess()
{
	return s_FileSystem_Singleton_Pointer;
}

IPTSFile * PTCALL PTSFileSystemImpl::PTSFile_Create(char const *pFileName, uint32_t eOpenMode)
{
	wchar_t wFileName[0X10000];
	size_t InCharsLeft = ::strlen(reinterpret_cast<const char *>(pFileName)) + 1U;//°üÀ¨'\0'
	size_t OutCharsLeft = 0X10000;
	::PTSConv_UTF8ToUTF16(pFileName, &InCharsLeft, wFileName, &OutCharsLeft);

	HANDLE hFile = INVALID_HANDLE_VALUE;

	switch (eOpenMode)
	{
	case FILE_OPEN_READONLY:
	{
		hFile = ::CreateFileW(wFileName, FILE_READ_DATA | FILE_READ_ATTRIBUTES, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	break;
	case FILE_OPEN_READWRITE:
	{
		hFile = ::CreateFileW(wFileName, FILE_READ_DATA | FILE_READ_ATTRIBUTES | FILE_WRITE_DATA | FILE_WRITE_ATTRIBUTES, 0U, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			assert(::GetLastError() == ERROR_FILE_NOT_FOUND);
			hFile = ::CreateFileW(wFileName, FILE_READ_DATA | FILE_READ_ATTRIBUTES | FILE_WRITE_DATA | FILE_WRITE_ATTRIBUTES, 0U, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		}
	}
	break;
	default:
		assert(0);
	}

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}
	else
	{
		return static_cast<IPTSFile *>(::new(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSFileImpl), alignof(PTSFileImpl)))PTSFileImpl(hFile));
	}
}

void PTCALL PTSFileSystemImpl::RootPath_Get(char *pPathName, size_t PathLength)
{
	wchar_t wPathName[0X10000];
	DWORD uiResult = ::GetCurrentDirectoryW(0X10000, wPathName);
	assert(uiResult != 0U);
	size_t InCharsLeft = static_cast<size_t>(uiResult) + 1U;//°üÀ¨'\0'
	size_t OutCharsLeft = PathLength;
	::PTSConv_UTF16ToUTF8(wPathName, &InCharsLeft, pPathName, &OutCharsLeft);
}

inline PTSFileImpl::PTSFileImpl(HANDLE hFile) :m_hFile(hFile)
{

}

inline PTSFileImpl::~PTSFileImpl()
{
	BOOL wbResult = ::CloseHandle(m_hFile);
	assert(wbResult != FALSE);
}

void PTCALL PTSFileImpl::Release()
{
	this->~PTSFileImpl();
	::PTSMemoryAllocator_Free_Aligned(this);
}

int64_t PTCALL PTSFileImpl::Size_Get()
{
	LARGE_INTEGER FileSize;
	BOOL wbResult = ::GetFileSizeEx(m_hFile, &FileSize);
	assert(wbResult != FALSE);
	return FileSize.QuadPart;
}

intptr_t PTCALL PTSFileImpl::Read(void *pBuffer, uintptr_t Count)
{
	DWORD CountDone;
	return (::ReadFile(m_hFile, pBuffer, static_cast<DWORD>(Count), &CountDone, NULL) != FALSE) ? static_cast<intptr_t>(CountDone) : -1;
}

intptr_t PTCALL PTSFileImpl::Write(void *pBuffer, uintptr_t Count)
{
	DWORD CountDone;
	return (::WriteFile(m_hFile, pBuffer, static_cast<DWORD>(Count), &CountDone, NULL) != FALSE) ? static_cast<intptr_t>(CountDone) : -1;
}

int64_t PTCALL PTSFileImpl::Seek(uint32_t eMoveMethod, int64_t nDistanceToMove)
{
	LARGE_INTEGER li;
	li.QuadPart = nDistanceToMove;
	LARGE_INTEGER liNew;
	BOOL wbResult;
	switch (eMoveMethod)
	{
	case FILE_SEEK_BEGIN:
	{
		wbResult = ::SetFilePointerEx(m_hFile, li, &liNew, FILE_BEGIN);
		assert(wbResult != FALSE);
	}
	break;
	case FILE_SEEK_CURRENT:
	{
		wbResult = ::SetFilePointerEx(m_hFile, li, &liNew, FILE_CURRENT);
		assert(wbResult != FALSE);
	}
	break;
	case FILE_SEEK_END:
	{
		wbResult = ::SetFilePointerEx(m_hFile, li, &liNew, FILE_END);
		assert(wbResult != FALSE);
	}
	break;
	default:
		assert(0);
	}

	return (wbResult != FALSE) ? liNew.QuadPart : -1;
}