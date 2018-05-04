#include "../../../Public/System/PTSFile.h"

#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windows.storage.h>

class PTSFileSystemImpl :public IPTSFileSystem
{
	ABI::Windows::Storage::IStorageFolder * const m_pStorageFolder;

	IPTSFile * PTCALL PTSFile_Create(char const *pFileName, uint32_t eOpenMode) override;
	void PTCALL RootPath_Get(char *pPathName, size_t PathLength) override;

	inline ~PTSFileSystemImpl();
public:
	inline PTSFileSystemImpl(ABI::Windows::Storage::IStorageFolder *pStorageFolder);
	inline void Internal_Release();
};

class PTSFileImpl :public IPTSFile
{
	ABI::Windows::Storage::IStorageFile *m_pFile;
	ABI::Windows::Storage::Streams::IRandomAccessStream *m_pFileStream;
	ABI::Windows::Storage::Streams::IInputStream *m_pFileStreamInput;
	ABI::Windows::Storage::Streams::IOutputStream *m_pFileStreamOutput;

	int64_t PTCALL Size_Get() override;
	int32_t PTCALL Read(void *pBuffer, uint32_t nNumberOfBytesToRead) override;
	int32_t PTCALL Write(void *pBuffer, uint32_t nNumberOfBytesToWrite)  override;
	int64_t PTCALL Seek(uint32_t eMoveMethod, int64_t nDistanceToMove)  override;

	inline ~PTSFileImpl();
	void PTCALL Release() override;
public:
	inline PTSFileImpl(ABI::Windows::Storage::IStorageFile *pFile, ABI::Windows::Storage::Streams::IRandomAccessStream *pFileStream, ABI::Windows::Storage::Streams::IInputStream *pFileStreamInput, ABI::Windows::Storage::Streams::IOutputStream *pFileStreamOutput);
};