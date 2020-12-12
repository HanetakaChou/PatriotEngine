#include "PTSFileImpl_Win32Desktop.h"
#include <new>
#include <string.h>
#include <assert.h>
#include "../../../Public/McRT/PTSMemoryAllocator.h"
#include "../../../Public/McRT/PTSThread.h"
#include "../../../Public/McRT/PTSConvUTF.h"

inline PTSFileSystemImpl::PTSFileSystemImpl()
{
	wchar_t wPathName[0X10000];
	DWORD uiResult = ::GetCurrentDirectoryW(0X10000, wPathName);
	assert(uiResult != 0U);
	uint32_t InCharsLeft = static_cast<uint32_t>(uiResult) + 1U;//����'\0'
	uint32_t OutCharsLeft = 0X10000U;
	bool bResult = ::PTSConv_UTF16ToUTF8(reinterpret_cast<char16_t *>(wPathName), &InCharsLeft, m_RootPath, &OutCharsLeft);
	assert(bResult);
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
extern "C" PTMCRTAPI PTBOOL PTCALL PTSFileSystem_Initialize()
{
	if (::PTSAtomic_GetAndAdd(&s_FileSystem_Initialize_RefCount, 1) == 0)
	{
		assert(s_FileSystem_Singleton_Pointer == NULL);
		::PTSAtomic_Set(&s_FileSystem_Singleton_Pointer, new(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSFileSystemImpl), alignof(PTSFileSystemImpl)))PTSFileSystemImpl{});
		assert(s_FileSystem_Singleton_Pointer != NULL);
		return PTTRUE;
	}
	else
	{
		return PTTRUE;
	}
}

extern "C" PTMCRTAPI IPTSFileSystem * PTCALL PTSFileSystem_ForProcess()
{
	return s_FileSystem_Singleton_Pointer;
}

char const * PTCALL PTSFileSystemImpl::RootPath()
{
	return m_RootPath;
}

IPTSFile * PTCALL PTSFileSystemImpl::File_Create(uint32_t OpenMode, char const *pFileName)
{
	wchar_t wFileName[0X10000];
	uint32_t InCharsLeft = static_cast<uint32_t>(::strlen(reinterpret_cast<const char *>(pFileName))) + 1U;//����'\0'
	uint32_t OutCharsLeft = 0X10000U;
	bool bResult = ::PTSConv_UTF8ToUTF16(pFileName, &InCharsLeft, reinterpret_cast<char16_t *>(wFileName), &OutCharsLeft);
	assert(bResult);

	HANDLE hFile = INVALID_HANDLE_VALUE;

	switch (OpenMode)
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

int64_t PTCALL PTSFileImpl::Size()
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

int64_t PTCALL PTSFileImpl::Seek(uint32_t Whence, int64_t Offset)
{
	LARGE_INTEGER liOffset;
	liOffset.QuadPart = Offset;
	LARGE_INTEGER liOffsetNew;
	BOOL wbResult;
	switch (Whence)
	{
	case FILE_SEEK_BEGIN:
	{
		wbResult = ::SetFilePointerEx(m_hFile, liOffset, &liOffsetNew, FILE_BEGIN);
		assert(wbResult != FALSE);
	}
	break;
	case FILE_SEEK_CURRENT:
	{
		wbResult = ::SetFilePointerEx(m_hFile, liOffset, &liOffsetNew, FILE_CURRENT);
		assert(wbResult != FALSE);
	}
	break;
	case FILE_SEEK_END:
	{
		wbResult = ::SetFilePointerEx(m_hFile, liOffset, &liOffsetNew, FILE_END);
		assert(wbResult != FALSE);
	}
	break;
	default:
		assert(0);
	}

	return (wbResult != FALSE) ? liOffsetNew.QuadPart : -1;
}