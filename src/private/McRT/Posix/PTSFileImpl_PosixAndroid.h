#include "../../../Public/McRT/PTSFile.h"

class PTSFileSystemImpl :public IPTSFileSystem
{
	int const m_iFDDir_DataExternal;
	char const * const m_StrPath_DataExternal;

	char const * PTCALL RootPath() override;
	IPTSFile * PTCALL File_Create(uint32_t OpenMode, char const *pFileName) override;

	inline ~PTSFileSystemImpl();
	inline void PTCALL Internal_Release();
public:
	inline PTSFileSystemImpl(int iFDDir_DataExternal, char const * StrPath_DataExternal);
};

class PTSFileImpl :public IPTSFile
{
	int const m_iFD;

	int64_t PTCALL Size() override;
	intptr_t PTCALL Read(void *pBuffer, uintptr_t Count) override;
	intptr_t PTCALL Write(void *pBuffer, uintptr_t Count)  override;
	int64_t PTCALL Seek(uint32_t Whence, int64_t Offset)  override;

	inline ~PTSFileImpl();
	void PTCALL Release() override;
public:
	inline PTSFileImpl(int iFD);
};