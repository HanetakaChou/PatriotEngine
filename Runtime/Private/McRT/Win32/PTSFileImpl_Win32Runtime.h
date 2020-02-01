#include "../../../Public/McRT/PTSFile.h"

#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windows.storage.h>

class PTSFileSystemImpl :public IPTSFileSystem
{
	ABI::Windows::Storage::IStorageFolder * const m_pStorageFolder;
	char m_RootPath[0X10000];

	char const * PTCALL RootPath() override;
	IPTSFile * PTCALL File_Create(uint32_t OpenMode, char const *pFileName) override;

	inline ~PTSFileSystemImpl();
public:
	inline PTSFileSystemImpl(ABI::Windows::Storage::IStorageFolder *pStorageFolder);
	inline void Internal_Release();
};

class PTSFileImpl :public IPTSFile
{
	ABI::Windows::Storage::IStorageFile * const m_pFile;
	ABI::Windows::Storage::Streams::IRandomAccessStream *const m_pFileStream;
	ABI::Windows::Storage::Streams::IInputStream * const m_pFileStreamInput;
	ABI::Windows::Storage::Streams::IOutputStream * const m_pFileStreamOutput;

	int64_t PTCALL Size() override;
	intptr_t PTCALL Read(void *pBuffer, uintptr_t Count) override;
	intptr_t PTCALL Write(void *pBuffer, uintptr_t Count)  override;
	int64_t PTCALL Seek(uint32_t Whence, int64_t Offset)  override;

	inline ~PTSFileImpl();
	void PTCALL Release() override;
public:
	inline PTSFileImpl(ABI::Windows::Storage::IStorageFile *pFile, ABI::Windows::Storage::Streams::IRandomAccessStream *pFileStream, ABI::Windows::Storage::Streams::IInputStream *pFileStreamInput, ABI::Windows::Storage::Streams::IOutputStream *pFileStreamOutput);
};