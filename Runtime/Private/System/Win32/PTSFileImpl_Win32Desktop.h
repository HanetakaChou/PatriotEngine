#include "../../../Public/System/PTSFile.h"

#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class PTSFileSystemImpl :public IPTSFileSystem
{
	IPTSFile * PTCALL PTSFile_Create(char const *pFileName, uint32_t eOpenMode) override;
	void PTCALL RootPath_Get(char *pPathPathName, size_t PathLength) override;

	inline ~PTSFileSystemImpl();
	inline void PTCALL Internal_Release();
public:
	inline PTSFileSystemImpl();
};

class PTSFileImpl :public IPTSFile
{
	HANDLE const m_hFile;

	int64_t PTCALL Size_Get() override;
	intptr_t PTCALL Read(void *pBuffer, uintptr_t Count) override;
	intptr_t PTCALL Write(void *pBuffer, uintptr_t Count)  override;
	int64_t PTCALL Seek(uint32_t eMoveMethod, int64_t nDistanceToMove)  override;

	inline ~PTSFileImpl();
	void PTCALL Release() override;
public:
	inline PTSFileImpl(HANDLE hFile);
};