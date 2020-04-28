#ifndef PT_GFX_HAL_UTIL_H
#define PT_GFX_HAL_UTIL_H 1

#include "PTGFXHAL.h"

#if defined(PTWIN32)

#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "../../Public/McRT/PTSConvUTF.h"
#include <assert.h>

inline PT_GFX_HAL_ITexture *PT_GFX_HAL_Texture_Asset_LoadDDS(PT_GFX_HAL_IDevice *pDevice, char const *pFileName)
{

	HANDLE hFile = INVALID_HANDLE_VALUE;
	{
		wchar_t wFileName[0X10000];
		uint32_t InCharsLeft = static_cast<uint32_t>(::strlen(reinterpret_cast<const char *>(pFileName))) + 1U;//°üÀ¨'\0'
		uint32_t OutCharsLeft = 0X10000U;
		bool bResult = ::PTSConv_UTF8ToUTF16(pFileName, &InCharsLeft, reinterpret_cast<char16_t *>(wFileName), &OutCharsLeft);
		assert(bResult);

		hFile = ::CreateFileW(wFileName, FILE_READ_DATA | FILE_READ_ATTRIBUTES, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	}

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	return PT_GFX_HAL_Texture_Asset_LoadDDS(
		pDevice,
		&hFile,
		[](void *fd_pUserData, void *buf, uintptr_t count)->intptr_t {
		DWORD CountDone;
		return (::ReadFile(*static_cast<HANDLE *>(fd_pUserData), buf, static_cast<DWORD>(count), &CountDone, NULL) != FALSE) ? static_cast<intptr_t>(CountDone) : -1;
	},
		[](void *fd_pUserData, int64_t offset, int32_t whence)->int64_t {
		assert(0 == FILE_BEGIN);
		assert(1 == FILE_CURRENT);
		assert(2 == FILE_END);
		LARGE_INTEGER liOffset;
		liOffset.QuadPart = offset;
		LARGE_INTEGER liOffsetNew;
		return (::SetFilePointerEx(*static_cast<HANDLE *>(fd_pUserData), liOffset, &liOffsetNew, static_cast<DWORD>(whence)) != FALSE) ? liOffsetNew.QuadPart : -1;
	},
		[](void *fd_pUserData, int64_t *st_size)->int32_t
	{
		LARGE_INTEGER FileSize;
		int32_t irtval = (::GetFileSizeEx(*static_cast<HANDLE *>(fd_pUserData), &FileSize) != FALSE) ? 0 : -1;
		(*st_size) = FileSize.QuadPart;
		return irtval;
	}
	);
}

#elif defined(PTPOSIX)

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>

inline PT_GFX_HAL_ITexture *PT_GFX_HAL_Texture_Asset_LoadDDS(PT_GFX_HAL_IDevice *pDevice, char const *pathname)
{
	int fd = ::open(pathname, O_RDONLY);

	if (fd == -1)
	{
		return NULL;
	}

	return PT_GFX_HAL_Texture_Asset_LoadDDS(
		pDevice,
		&fd,
		[](void *fd_pUserData, void *buf, uintptr_t count)->intptr_t {
		return ::read(*static_cast<int *>(fd_pUserData), buf, count);
	},
		[](void *fd_pUserData, int64_t offset, int32_t whence)->int64_t {
		assert(0 == SEEK_SET);
		assert(1 == SEEK_CUR);
		assert(2 == SEEK_END);
		return ::lseek64(*static_cast<int *>(fd_pUserData), offset, static_cast<int>(whence));
	},
		[](void *fd_pUserData, int64_t *st_size)->int32_t
	{
		struct stat64 buf;
		int32_t irtval = ::fstat64(*static_cast<int *>(fd_pUserData), &buf);
		*(st_size) = buf.st_size;
		return irtval;
	}
	);
}

#else
#error Unknown Platform //未知的平台
#endif

#endif