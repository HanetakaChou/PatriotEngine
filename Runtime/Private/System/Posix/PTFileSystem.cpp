#include <fcntl.h>
#include <sys/stat.h>

inline bool PTFileOpen(const char *pFileName, PTFileOpenMode eOpenMode, PTFile *pFileOut)
{
	pFileOut->m_fd = -1;
	switch (eOpenMode)
	{
	case PTFileOpenMode::ReadOnly:
	{
		pFileOut->m_fd = ::open(pFileName, O_RDONLY);
	}
	break;
	case PTFileOpenMode::ReadWrite:
	{
		pFileOut->m_fd = ::open(pFileName, O_RDWR);
		if (pFileOut->m_fd == -1)
		{
			pFileOut->m_fd = ::open(pFileName, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
		}
	}
	break;
	}
	return pFileOut->m_fd != -1;
}

inline bool PTFileClose(PTFile *pFile)
{
	return ::close(pFile->m_fd) != -1;
}

inline bool PTFileSize(PTFile *pFile, uint64_t *pSizeOut)
{
	struct stat64 FileStatus;
	if (fstat64(pFile->m_fd, &FileStatus) == -1)
		return false;
	(*pSizeOut) = static_cast<uint64_t>(FileStatus.st_size);
	return true;
}

inline uint32_t PTFileRead(PTFile *pFile, void *pBuffer, uint32_t nNumberOfBytesToRead)
{
	ssize_t NumberOfBytesRead = ::read(pFile->m_fd, pBuffer, nNumberOfBytesToRead);
	return (NumberOfBytesRead != -1) ? (NumberOfBytesRead > 0 ? NumberOfBytesRead : 0U) : 0U;
}

inline uint32_t PTFileWrite(PTFile *pFile, void *pBuffer, uint32_t nNumberOfBytesToWrite)
{
	ssize_t NumberOfBytesWritten = ::write(pFile->m_fd, pBuffer, nNumberOfBytesToWrite);
	return (NumberOfBytesWritten != -1) ? (NumberOfBytesWritten > 0 ? NumberOfBytesWritten : 0U) : 0U;
}

inline bool PTFileSeek(PTFile *pFile, PTFileMoveMethod eMoveMethod, uint64_t nDistanceToMove, uint64_t *pNewFilePointer)
{
	off64_t ret = ::lseek64(pFile->m_fd, static_cast<off64_t>(nDistanceToMove), static_cast<int>(eMoveMethod));
	if (ret == -1)
		return false;
	(*pNewFilePointer) = static_cast<uint64_t>(ret);
	return true;
}