#include "../../../Public/System/PTSFile.h"

#ifdef PTWIN32DESKTOP

#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class PTSFileImpl :public IPTSFile
{
	HANDLE m_hFile;

	inline ~PTSFileImpl();
	void PTCALL Destruct() override;

	int64_t PTCALL Size_Get() override;
	uint32_t PTCALL Read(void *pBuffer, uint32_t nNumberOfBytesToRead) override;
	uint32_t PTCALL Write(void *pBuffer, uint32_t nNumberOfBytesToWrite)  override;
	int64_t PTCALL Seek(PTSFileMoveMethod eMoveMethod, int64_t nDistanceToMove)  override;

public:
	inline PTSFileImpl(HANDLE hFile);
};

static_assert((alignof(PTSFileStorage) % alignof(PTSFileImpl)) == 0, "PTSFileStorage Not Align");
static_assert(sizeof(PTSFileStorage) >= sizeof(PTSFileImpl), "PTSFileStorage Too Small");

class PTSFileSystemImpl :public IPTSFileSystem
{
	inline ~PTSFileSystemImpl();
	void PTCALL Destruct() override;

	IPTSFile * PTCALL PTSFile_Construct(PTSFileStorage *pStorage, char const *pFileName, PTSFileOpenMode eOpenMode) override;

	void PTCALL RootPath_Get(char *pPathPathName, size_t PathLength) override;

public:
	inline PTSFileSystemImpl();
};

static_assert((alignof(PTSFileSystemStorage) % alignof(PTSFileSystemImpl)) == 0, "PTSFileSystemStorage Not Align");
static_assert(sizeof(PTSFileSystemStorage) >= sizeof(PTSFileSystemImpl), "PTSFileSystemStorage Too Small");

#elif defined(PTWIN32RUNTIME)

#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windows.storage.h>

class PTSFileImpl :public IPTSFile
{
	ABI::Windows::Storage::IStorageFile *m_pFile;
	ABI::Windows::Storage::Streams::IRandomAccessStream *m_pFileStream;
	ABI::Windows::Storage::Streams::IInputStream *m_pFileStreamInput;
	ABI::Windows::Storage::Streams::IOutputStream *m_pFileStreamOutput;

	inline ~PTSFileImpl();
	void PTCALL Destruct() override;

	int64_t PTCALL Size_Get() override;
	uint32_t PTCALL Read(void *pBuffer, uint32_t nNumberOfBytesToRead) override;
	uint32_t PTCALL Write(void *pBuffer, uint32_t nNumberOfBytesToWrite)  override;
	int64_t PTCALL Seek(PTSFileMoveMethod eMoveMethod, int64_t nDistanceToMove)  override;

public:
	inline PTSFileImpl(ABI::Windows::Storage::IStorageFile *pFile, ABI::Windows::Storage::Streams::IRandomAccessStream *pFileStream, ABI::Windows::Storage::Streams::IInputStream *pFileStreamInput, ABI::Windows::Storage::Streams::IOutputStream *pFileStreamOutput);
};

static_assert((alignof(PTSFileStorage) % alignof(PTSFileImpl)) == 0, "PTSFileStorage Not Align");
static_assert(sizeof(PTSFileStorage) >= sizeof(PTSFileImpl), "PTSFileStorage Too Small");


class PTSFileSystemImpl :public IPTSFileSystem
{
	ABI::Windows::Storage::IStorageFolder *m_pStorageFolder;

	inline ~PTSFileSystemImpl();
	void PTCALL Destruct() override;

	IPTSFile * PTCALL PTSFile_Construct(PTSFileStorage *pStorage, char const *pFileName, PTSFileOpenMode eOpenMode) override;

	void PTCALL RootPath_Get(char *pPathName, size_t PathLength) override;

public:
	inline PTSFileSystemImpl(ABI::Windows::Storage::IStorageFolder *pStorageFolder);
};

static_assert((alignof(PTSFileSystemStorage) % alignof(PTSFileSystemImpl)) == 0, "PTSFileSystemStorage Not Align");
static_assert(sizeof(PTSFileSystemStorage) >= sizeof(PTSFileSystemImpl), "PTSFileSystemStorage Too Small");

#else
#error 未知的平台
#endif