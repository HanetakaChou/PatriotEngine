#include "PTSFileImpl.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <new>
#include "../../../Public/System/PTSMemoryAllocator.h"
#include "../../../Public/System/PTSThread.h"

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

IPTSFile * PTCALL PTSFileSystemImpl::File_Create(char const *pFileName, uint32_t eOpenMode)
{
	int fd = -1;
	switch (eOpenMode)
	{
	case FILE_OPEN_READONLY:
	{
		fd = ::open64(pFileName, O_RDONLY);
	}
	break;
	case FILE_OPEN_READWRITE:
	{
		fd = ::open64(pFileName, O_RDWR);
		if (fd == -1)
		{
			fd = ::open64(pFileName, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
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

void PTCALL PTSFileSystemImpl::RootPath_Get(char *pPathName, size_t PathLength)
{

}

inline PTSFileImpl::PTSFileImpl(int fd) :m_fd(fd)
{

}

inline PTSFileImpl::~PTSFileImpl()
{
	int iResult = ::close(m_fd);
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
	int iResult = ::fstat64(m_fd, &FileStatus);
	assert(iResult != -1);

	return FileStatus.st_size;
}

intptr_t PTCALL PTSFileImpl::Read(void *pBuffer, uintptr_t Count)
{
	intptr_t CountDone = ::read(m_fd, pBuffer, Count);
	assert(CountDone != -1);
	return CountDone;
}

intptr_t PTCALL PTSFileImpl::Write(void *pBuffer, uintptr_t Count)
{
	intptr_t CountDone = ::write(m_fd, pBuffer, Count);
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
		ret = ::lseek64(m_fd, Offset, SEEK_SET);
		assert(ret != -1);
	}
	break;
	case FILE_SEEK_CURRENT:
	{
		ret = ::lseek64(m_fd, Offset, SEEK_CUR);
		assert(ret != -1);
	}
	break;
	case FILE_SEEK_END:
	{
		ret = ::lseek64(m_fd, Offset, SEEK_END);
		assert(ret != -1);
	}
	break;
	default:
		assert(0);
	}

	return ret;
}