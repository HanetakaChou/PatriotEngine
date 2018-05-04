#include "../../../Public/System/PTSFile.h"

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
	int const m_fd;

	int64_t PTCALL Size_Get() override;
	intptr_t PTCALL Read(void *pBuffer, uintptr_t Count) override;
	intptr_t PTCALL Write(void *pBuffer, uintptr_t Count)  override;
	int64_t PTCALL Seek(uint32_t Whence, int64_t Offset)  override;

	inline ~PTSFileImpl();
	void PTCALL Release() override;
public:
	inline PTSFileImpl(int fd);
};