#include "PTSFileImpl_PosixAndroid.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <new>
#include <assert.h>
#include "../../../Public/McRT/PTSMemoryAllocator.h"
#include "../../../Public/McRT/PTSThread.h"

inline PTSFileSystemImpl::PTSFileSystemImpl(int iFDDir_DataExternal, char const * StrPath_DataExternal) :m_iFDDir_DataExternal(iFDDir_DataExternal), m_StrPath_DataExternal(StrPath_DataExternal)
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
extern "C" PTMCRTAPI int PTS_iFDDir_DataExternal = -1;
extern "C" PTMCRTAPI char PTS_StrPath_DataExternal[0X10000] = { "" };

static int32_t s_FileSystem_Initialize_RefCount = 0;
extern "C" PTMCRTAPI bool PTCALL PTSFileSystem_Initialize()
{
	if (::PTSAtomic_GetAndAdd(&s_FileSystem_Initialize_RefCount, 1) == 0)
	{
		assert(s_FileSystem_Singleton_Pointer == NULL);
		assert(PTS_iFDDir_DataExternal != -1);
		assert(PTS_StrPath_DataExternal[0] != '\0');
		s_FileSystem_Singleton_Pointer = new(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSFileSystemImpl), alignof(PTSFileSystemImpl)))PTSFileSystemImpl(PTS_iFDDir_DataExternal, PTS_StrPath_DataExternal);
		assert(s_FileSystem_Singleton_Pointer != NULL);
		return true;
	}
	else
	{
		return true;
	}
}

extern "C" PTMCRTAPI IPTSFileSystem * PTCALL PTSFileSystem_ForProcess()
{
	return s_FileSystem_Singleton_Pointer;
}

char const * PTCALL PTSFileSystemImpl::RootPath()
{
	return m_StrPath_DataExternal;
}

IPTSFile * PTCALL PTSFileSystemImpl::File_Create(uint32_t OpenMode, char const *pFileName)
{
	int fd = -1;
	switch (OpenMode)
	{
	case FILE_OPEN_READONLY:
	{
		fd = ::openat64(m_iFDDir_DataExternal, pFileName, O_RDONLY);
	}
	break;
	case FILE_OPEN_READWRITE:
	{
		fd = ::openat64(m_iFDDir_DataExternal, pFileName, O_RDWR);
		if (fd == -1)
		{
			assert(errno == ENOENT);
			fd = ::openat64(m_iFDDir_DataExternal, pFileName, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
		}
	}
	break;
	}

	if (fd == -1)
	{
		return NULL;
	}
	else
	{
		return static_cast<IPTSFile *>(::new(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSFileImpl), alignof(PTSFileImpl)))PTSFileImpl(fd));
	}
}

inline PTSFileImpl::PTSFileImpl(int iFD) :m_iFD(iFD)
{

}

inline PTSFileImpl::~PTSFileImpl()
{
	int iResult = ::close(m_iFD);
	assert(iResult != -1);
}

void PTCALL PTSFileImpl::Release()
{
	this->~PTSFileImpl();
	::PTSMemoryAllocator_Free_Aligned(this);
}


int64_t PTCALL PTSFileImpl::Size()
{
	struct stat64 FileStatus;
	int iResult = ::fstat64(m_iFD, &FileStatus);
	assert(iResult != -1);

	return FileStatus.st_size;
}

intptr_t PTCALL PTSFileImpl::Read(void *pBuffer, uintptr_t Count)
{
	intptr_t CountDone = ::read(m_iFD, pBuffer, Count);
	assert(CountDone != -1);
	return CountDone;
}

intptr_t PTCALL PTSFileImpl::Write(void *pBuffer, uintptr_t Count)
{
	intptr_t CountDone = ::write(m_iFD, pBuffer, Count);
	assert(CountDone != -1);
	return CountDone;
}

int64_t PTCALL PTSFileImpl::Seek(uint32_t Whence, int64_t Offset)
{
	off64_t ret; 

	switch (Whence)
	{
	case FILE_SEEK_BEGIN:
	{
		ret = ::lseek64(m_iFD, Offset, SEEK_SET);
		assert(ret != -1);
	}
	break;
	case FILE_SEEK_CURRENT:
	{
		ret = ::lseek64(m_iFD, Offset, SEEK_CUR);
		assert(ret != -1);
	}
	break;
	case FILE_SEEK_END:
	{
		ret = ::lseek64(m_iFD, Offset, SEEK_END);
		assert(ret != -1);
	}
	break;
	default:
		assert(0);
	}

	return ret;
}