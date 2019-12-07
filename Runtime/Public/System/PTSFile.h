#ifndef PT_SYSTEM_FILE_H
#define PT_SYSTEM_FILE_H

#include "../PTCommon.h"
#include "PTSCommon.h"
#include <stdint.h>

struct IPTSFile;

struct IPTSFileSystem
{
	enum :uint32_t
	{
		FILE_OPEN_READONLY = 0,
		FILE_OPEN_READWRITE = 1
	};

	virtual char const * PTCALL RootPath() = 0;
	virtual IPTSFile * PTCALL File_Create(uint32_t OpenMode, char const *pFileName) = 0;
};

struct IPTSFile
{
	enum :uint32_t
	{
		FILE_SEEK_BEGIN = 0U,
		FILE_SEEK_CURRENT = 1,
		FILE_SEEK_END = 2
	};

	virtual void PTCALL Release() = 0;

	virtual int64_t PTCALL Size() = 0;
	virtual intptr_t PTCALL Read(void *pBuffer, uintptr_t Count) = 0;
	virtual intptr_t PTCALL Write(void *pBuffer, uintptr_t Count) = 0;
	virtual int64_t PTCALL Seek(uint32_t Whence, int64_t Offset) = 0;
};

extern "C" PTMCRTAPI bool PTCALL PTSFileSystem_Initialize();
extern "C" PTMCRTAPI IPTSFileSystem * PTCALL PTSFileSystem_ForProcess();

#endif
