#include "PTSFileImpl.h"

#ifdef PTWIN32DESKTOP
#include <assert.h>
#include <string.h>
#include <new>
#include "../../../Public/System/Win32/PTSConvUTF.h"

inline PTSFileImpl::~PTSFileImpl()
{
	BOOL wbResult = ::CloseHandle(m_hFile);
	assert(wbResult != FALSE);
}

void PTCALL PTSFileImpl::Destruct()
{
	this->~PTSFileImpl();
}

int64_t PTCALL PTSFileImpl::Size_Get()
{
	LARGE_INTEGER FileSize;
	BOOL wbResult = ::GetFileSizeEx(m_hFile, &FileSize);
	assert(wbResult != FALSE);
	return FileSize.QuadPart;
}

uint32_t PTCALL PTSFileImpl::Read(void *pBuffer, uint32_t nNumberOfBytesToRead)
{
	DWORD NumberOfBytesRead;
	return ::ReadFile(m_hFile, pBuffer, nNumberOfBytesToRead, &NumberOfBytesRead, NULL) ? NumberOfBytesRead : 0U;
}

uint32_t PTCALL PTSFileImpl::Write(void *pBuffer, uint32_t nNumberOfBytesToWrite)
{
	DWORD NumberOfBytesWritten;
	return ::WriteFile(m_hFile, pBuffer, nNumberOfBytesToWrite, &NumberOfBytesWritten, NULL) ? NumberOfBytesWritten : 0U;
}

int64_t PTCALL PTSFileImpl::Seek(PTSFileMoveMethod eMoveMethod, int64_t nDistanceToMove)
{
	LARGE_INTEGER li;
	li.QuadPart = nDistanceToMove;
	LARGE_INTEGER liNew;
	switch (eMoveMethod)
	{
	case Begin:
	{
		BOOL wbResult = ::SetFilePointerEx(m_hFile, li, &liNew, FILE_BEGIN);
		assert(wbResult != FALSE);
	}
	break;
	case Current:
	{
		BOOL wbResult = ::SetFilePointerEx(m_hFile, li, &liNew, FILE_CURRENT);
		assert(wbResult != FALSE);
	}
	break;
	case End:
	{
		BOOL wbResult = ::SetFilePointerEx(m_hFile, li, &liNew, FILE_END);
		assert(wbResult != FALSE);
	}
	break;
	default:
		assert(0);
	}

	return liNew.QuadPart;
}

inline PTSFileImpl::PTSFileImpl(HANDLE hFile) :m_hFile(hFile)
{

}

inline PTSFileSystemImpl::~PTSFileSystemImpl()
{

}

void PTCALL PTSFileSystemImpl::Destruct()
{
	this->~PTSFileSystemImpl();
}

IPTSFile * PTCALL PTSFileSystemImpl::PTSFile_Construct(PTSFileStorage *pStorage, char const *pFileName, PTSFileOpenMode eOpenMode)
{
	wchar_t wFileName[0X10000];
	size_t InCharsLeft = ::strlen(reinterpret_cast<const char *>(pFileName)) + 1U;//包括'\0'
	size_t OutCharsLeft = 0X10000;
	::PTSConv_UTF8ToUTF16(pFileName, &InCharsLeft, wFileName, &OutCharsLeft);

	HANDLE hFile = INVALID_HANDLE_VALUE;

	switch (eOpenMode)
	{
	case PTSFileOpenMode::ReadOnly:
	{
		hFile = ::CreateFileW(wFileName, FILE_READ_DATA | FILE_READ_ATTRIBUTES, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	break;
	case PTSFileOpenMode::ReadWrite:
	{
		hFile = ::CreateFileW(wFileName, FILE_READ_DATA | FILE_READ_ATTRIBUTES | FILE_WRITE_DATA | FILE_WRITE_ATTRIBUTES, 0U, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			assert(::GetLastError() == ERROR_FILE_NOT_FOUND);
			hFile = ::CreateFileW(wFileName, FILE_READ_DATA | FILE_READ_ATTRIBUTES | FILE_WRITE_DATA | FILE_WRITE_ATTRIBUTES, 0U, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		}
	}
	break;
	default:
		assert(0);
	}

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}
	else
	{
		return static_cast<IPTSFile *>(::new(pStorage)PTSFileImpl(hFile));
	}
}

void PTCALL PTSFileSystemImpl::RootPath_Get(char *pPathName, size_t PathLength)
{
	wchar_t wPathName[0X10000];
	DWORD uiResult = ::GetCurrentDirectoryW(0X10000, wPathName);
	assert(uiResult != 0U);
	size_t InCharsLeft = static_cast<size_t>(uiResult) + 1U;//包括'\0'
	size_t OutCharsLeft = PathLength;
	::PTSConv_UTF16ToUTF8(wPathName, &InCharsLeft, pPathName, &OutCharsLeft);
}

inline PTSFileSystemImpl::PTSFileSystemImpl()
{
	
}

IPTSFileSystem * PTCALL PTSFileSystem_Construct(PTSFileSystemStorage *pStorage)
{
	return static_cast<IPTSFileSystem *>(::new(pStorage)PTSFileSystemImpl());
}

#elif defined(PTWIN32RUNTIME)
#include <roapi.h>
#include <robuffer.h>
#include <assert.h>
#include <new>
#include "../../../Public/System/Win32/PTSConvUTF.h"
#include "../../../Public/System/PTSThread.h"

inline PTSFileImpl::~PTSFileImpl()
{
	HRESULT hResult;

	hResult = ::RoInitialize(RO_INIT_MULTITHREADED);
	assert(SUCCEEDED(hResult));

	if (m_pFileStreamOutput != NULL)
	{
		m_pFileStreamOutput->Release();
	}

	m_pFileStreamInput->Release();

	m_pFileStream->Release();

	m_pFile->Release();

	::RoUninitialize();
}

void PTCALL PTSFileImpl::Destruct()
{
	this->~PTSFileImpl();
}

int64_t PTCALL PTSFileImpl::Size_Get()
{
	HRESULT hResult;

	hResult = ::RoInitialize(RO_INIT_MULTITHREADED);
	assert(SUCCEEDED(hResult));

	UINT64 uiSize;
	hResult = m_pFileStream->get_Size(&uiSize);
	assert(SUCCEEDED(hResult));

	::RoUninitialize();

	return static_cast<int64_t>(uiSize);
}

uint32_t PTCALL PTSFileImpl::Read(void *pBuffer, uint32_t uiNumberOfBytesToRead)
{
	class :public ABI::Windows::Storage::Streams::IBuffer, public Windows::Storage::Streams::IBufferByteAccess, public IAgileObject
	{
		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) override
		{
			if (riid == __uuidof(IUnknown) || riid == __uuidof(ABI::Windows::Storage::Streams::IBuffer))
			{
				*ppvObject = static_cast<ABI::Windows::Storage::Streams::IBuffer *>(this);
				return S_OK;
			}
			else if (riid == __uuidof(Windows::Storage::Streams::IBufferByteAccess))
			{
				*ppvObject = static_cast<Windows::Storage::Streams::IBufferByteAccess *>(this);
				return S_OK;
			}
			else if (riid == __uuidof(IAgileObject))
			{
				*ppvObject = static_cast<IAgileObject *>(this);
				return S_OK;
			}
			else
			{
				return E_NOINTERFACE;
			}
		}
		ULONG STDMETHODCALLTYPE AddRef(void) override
		{
			return 1U;
		}
		ULONG STDMETHODCALLTYPE Release(void) override
		{
			return 1U;
		}
		//IInspectable
		HRESULT STDMETHODCALLTYPE GetIids(ULONG *iidCount, IID **iids) override
		{
			(*iids) = static_cast<IID *>(::CoTaskMemAlloc(sizeof(IID) * 1));
			if ((*iids) == NULL)
				return E_OUTOFMEMORY;

			*iidCount = 1U;
			(*iids)[0] = __uuidof(ABI::Windows::Storage::Streams::IBuffer);
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING *className) override
		{
			return ::WindowsCreateString(L"PTSFileImpl_Read_Lambda", _countof(L"PTSFileImpl_Read_Lambda") - 1U, className);
		}
		HRESULT STDMETHODCALLTYPE GetTrustLevel(TrustLevel* trustLevel) override
		{
			*trustLevel = ::BaseTrust;
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE get_Capacity(UINT32 *pCapacity) override
		{
			(*pCapacity) = m_Capacity;
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE get_Length(UINT32 *pLength) override
		{
			(*pLength) = m_Length;
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE put_Length(UINT32 Length) override
		{
			m_Length = Length;
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE Buffer(BYTE **ppByte) override
		{
			(*ppByte) = pByte;
			return S_OK;
		}
	public:
		UINT32 m_Capacity;
		UINT32 m_Length;
		BYTE *pByte;
	}l_Lambda;
	//当前线程会基于IAsyncInfo::get_Status忙式等待，因此存放在当前线程的栈中是安全的
	
	l_Lambda.m_Capacity = uiNumberOfBytesToRead;
	l_Lambda.m_Length = 0U;
	l_Lambda.pByte = static_cast<BYTE *>(pBuffer);

	HRESULT hResult;

	hResult = ::RoInitialize(RO_INIT_MULTITHREADED);
	assert(SUCCEEDED(hResult));

	ABI::Windows::Foundation::IAsyncOperationWithProgress<ABI::Windows::Storage::Streams::IBuffer*, UINT32> *pAsyncOperation;
	hResult = m_pFileStreamInput->ReadAsync(&l_Lambda, uiNumberOfBytesToRead, ABI::Windows::Storage::Streams::InputStreamOptions_None, &pAsyncOperation);
	assert(SUCCEEDED(hResult));

	ABI::Windows::Foundation::IAsyncInfo *pAsycInfo;
	hResult = pAsyncOperation->QueryInterface(IID_PPV_ARGS(&pAsycInfo));
	assert(SUCCEEDED(hResult));

	ABI::Windows::Foundation::AsyncStatus eAsyncStatus;
	do {
		hResult = pAsycInfo->get_Status(&eAsyncStatus);
		assert(SUCCEEDED(hResult));
		//Wait
		::PTS_Yield();
	} while (eAsyncStatus == ABI::Windows::Foundation::AsyncStatus::Started);

	//assert(eAsyncStatus != ABI::Windows::Foundation::AsyncStatus::Canceled);
	//assert(eAsyncStatus != ABI::Windows::Foundation::AsyncStatus::Error);
	assert(eAsyncStatus == ABI::Windows::Foundation::AsyncStatus::Completed);

	ABI::Windows::Storage::Streams::IBuffer *pRuntimeObjectBuffer;
	hResult = pAsyncOperation->GetResults(&pRuntimeObjectBuffer);
	assert(SUCCEEDED(hResult));
	assert(pRuntimeObjectBuffer == static_cast<ABI::Windows::Storage::Streams::IBuffer *>(&l_Lambda));

	hResult = pAsycInfo->Close();
	assert(SUCCEEDED(hResult));

	pAsycInfo->Release();

	pAsyncOperation->Release();

	::RoUninitialize();

	return l_Lambda.m_Length;
}

uint32_t PTCALL PTSFileImpl::Write(void *pBuffer, uint32_t uiNumberOfBytesToWrite)
{
	assert(m_pFileStreamOutput != NULL);

	class :public ABI::Windows::Storage::Streams::IBuffer, public Windows::Storage::Streams::IBufferByteAccess, public IAgileObject
	{
		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) override
		{
			if (riid == __uuidof(IUnknown) || riid == __uuidof(ABI::Windows::Storage::Streams::IBuffer))
			{
				*ppvObject = static_cast<ABI::Windows::Storage::Streams::IBuffer *>(this);
				return S_OK;
			}
			else if (riid == __uuidof(Windows::Storage::Streams::IBufferByteAccess))
			{
				*ppvObject = static_cast<Windows::Storage::Streams::IBufferByteAccess *>(this);
				return S_OK;
			}
			else if (riid == __uuidof(IAgileObject))
			{
				*ppvObject = static_cast<IAgileObject *>(this);
				return S_OK;
			}
			else
			{
				return E_NOINTERFACE;
			}
		}
		ULONG STDMETHODCALLTYPE AddRef(void) override
		{
			return 1U;
		}
		ULONG STDMETHODCALLTYPE Release(void) override
		{
			return 1U;
		}
		//IInspectable
		HRESULT STDMETHODCALLTYPE GetIids(ULONG *iidCount, IID **iids) override
		{
			(*iids) = static_cast<IID *>(::CoTaskMemAlloc(sizeof(IID) * 1));
			if ((*iids) == NULL)
				return E_OUTOFMEMORY;

			*iidCount = 1U;
			(*iids)[0] = __uuidof(ABI::Windows::Storage::Streams::IBuffer);
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING *className) override
		{
			return ::WindowsCreateString(L"PTSFileImpl_Write_Lambda", _countof(L"PTSFileImpl_Write_Lambda") - 1U, className);
		}
		HRESULT STDMETHODCALLTYPE GetTrustLevel(TrustLevel* trustLevel) override
		{
			*trustLevel = ::BaseTrust;
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE get_Capacity(UINT32 *pCapacity) override
		{
			(*pCapacity) = m_Capacity;
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE get_Length(UINT32 *pLength) override
		{
			(*pLength) = m_Length;
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE put_Length(UINT32 Length) override
		{
			m_Length = Length;
			return S_OK;
		}
		HRESULT STDMETHODCALLTYPE Buffer(BYTE **ppByte) override
		{
			(*ppByte) = pByte;
			return S_OK;
		}
	public:
		UINT32 m_Capacity;
		UINT32 m_Length;
		BYTE *pByte;
	}l_Lambda;
	//当前线程会基于IAsyncInfo::get_Status忙式等待，因此存放在当前线程的栈中是安全的

	l_Lambda.m_Capacity = uiNumberOfBytesToWrite;
	l_Lambda.m_Length = uiNumberOfBytesToWrite;
	l_Lambda.pByte = static_cast<BYTE *>(pBuffer);

	HRESULT hResult;

	hResult = ::RoInitialize(RO_INIT_MULTITHREADED);
	assert(SUCCEEDED(hResult));

	ABI::Windows::Foundation::IAsyncOperationWithProgress<UINT32, UINT32> *pAsyncOperation;
	hResult = m_pFileStreamOutput->WriteAsync(&l_Lambda, &pAsyncOperation);
	assert(SUCCEEDED(hResult));

	ABI::Windows::Foundation::IAsyncInfo *pAsycInfo;
	hResult = pAsyncOperation->QueryInterface(IID_PPV_ARGS(&pAsycInfo));
	assert(SUCCEEDED(hResult));

	ABI::Windows::Foundation::AsyncStatus eAsyncStatus;
	do {
		hResult = pAsycInfo->get_Status(&eAsyncStatus);
		assert(SUCCEEDED(hResult));
		//Wait
		::PTS_Yield();
	} while (eAsyncStatus == ABI::Windows::Foundation::AsyncStatus::Started);

	//assert(eAsyncStatus != ABI::Windows::Foundation::AsyncStatus::Canceled);
	//assert(eAsyncStatus != ABI::Windows::Foundation::AsyncStatus::Error);
	assert(eAsyncStatus == ABI::Windows::Foundation::AsyncStatus::Completed);

	UINT32 uiNumberOfBytesWritten;
	hResult = pAsyncOperation->GetResults(&uiNumberOfBytesWritten);
	assert(SUCCEEDED(hResult));

	hResult = pAsycInfo->Close();
	assert(SUCCEEDED(hResult));

	pAsycInfo->Release();

	pAsyncOperation->Release();

	::RoUninitialize();

	return uiNumberOfBytesWritten;
}

int64_t PTCALL PTSFileImpl::Seek(PTSFileMoveMethod eMoveMethod, int64_t iDistanceToMove)
{
	HRESULT hResult;

	hResult = ::RoInitialize(RO_INIT_MULTITHREADED);
	assert(SUCCEEDED(hResult));

	int64_t Position_Seek;

	switch (eMoveMethod)
	{
	case Begin:
	{
		Position_Seek = iDistanceToMove;
	}
	break;
	case Current:
	{
		UINT64 Position_Current;
		hResult = m_pFileStream->get_Position(&Position_Current);
		assert(SUCCEEDED(hResult));
		Position_Seek = static_cast<int64_t>(Position_Current) + iDistanceToMove;
	}
	break;
	case End:
	{
		UINT64 Position_End;
		hResult = m_pFileStream->get_Size(&Position_End);
		assert(SUCCEEDED(hResult));
		Position_Seek = static_cast<int64_t>(Position_End) + iDistanceToMove;
	}
	break;
	default:
		assert(0);
	}

	hResult = m_pFileStream->Seek(static_cast<UINT64>(Position_Seek));
	assert(SUCCEEDED(hResult));

	::RoUninitialize();

	return 0;
}

inline PTSFileImpl::PTSFileImpl(ABI::Windows::Storage::IStorageFile *pFile, ABI::Windows::Storage::Streams::IRandomAccessStream *pFileStream, ABI::Windows::Storage::Streams::IInputStream *pFileStreamInput, ABI::Windows::Storage::Streams::IOutputStream *pFileStreamOutput) :m_pFile(pFile), m_pFileStream(pFileStream), m_pFileStreamInput(pFileStreamInput), m_pFileStreamOutput(pFileStreamOutput)
{

}

inline PTSFileSystemImpl::~PTSFileSystemImpl()
{
	HRESULT hResult;

	hResult = ::RoInitialize(RO_INIT_MULTITHREADED);
	assert(SUCCEEDED(hResult));

	m_pStorageFolder->Release();

	::RoUninitialize();
}

void PTCALL PTSFileSystemImpl::Destruct()
{
	this->~PTSFileSystemImpl();
}

IPTSFile * PTCALL PTSFileSystemImpl::PTSFile_Construct(PTSFileStorage *pStorage, char const *pFileName, PTSFileOpenMode eOpenMode)
{
	HRESULT hResult;

	wchar_t wFileName[0X10000];
	size_t InCharsLeft = ::strlen(reinterpret_cast<const char *>(pFileName)) + 1U;//包括'\0'
	size_t OutCharsLeft = 0X10000;
	::PTSConv_UTF8ToUTF16(pFileName, &InCharsLeft, wFileName, &OutCharsLeft);

	hResult = ::RoInitialize(RO_INIT_MULTITHREADED);
	assert(SUCCEEDED(hResult));

	ABI::Windows::Storage::IStorageFile *pFile = NULL;
	{
		HSTRING_HEADER hFileName_Header;
		HSTRING hFileName;
		hResult = ::WindowsCreateStringReference(wFileName, static_cast<UINT32>(::wcsnlen(wFileName, 0X10000)), &hFileName_Header, &hFileName);

		ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Storage::StorageFile*> *pAsyncOperation;
		hResult = m_pStorageFolder->CreateFileAsync(hFileName, ABI::Windows::Storage::CreationCollisionOption_OpenIfExists, &pAsyncOperation);
		assert(SUCCEEDED(hResult));

		ABI::Windows::Foundation::IAsyncInfo *pAsycInfo;
		hResult = pAsyncOperation->QueryInterface(IID_PPV_ARGS(&pAsycInfo));
		assert(SUCCEEDED(hResult));

		ABI::Windows::Foundation::AsyncStatus eAsyncStatus;
		do {
			hResult = pAsycInfo->get_Status(&eAsyncStatus);
			assert(SUCCEEDED(hResult));
			//Wait
			::PTS_Yield();
		} while (eAsyncStatus == ABI::Windows::Foundation::AsyncStatus::Started);

		//assert(eAsyncStatus != ABI::Windows::Foundation::AsyncStatus::Canceled);
		//assert(eAsyncStatus != ABI::Windows::Foundation::AsyncStatus::Error);
		assert(eAsyncStatus == ABI::Windows::Foundation::AsyncStatus::Completed);

		hResult = pAsyncOperation->GetResults(&pFile);
		assert(SUCCEEDED(hResult));

		hResult = pAsycInfo->Close();
		assert(SUCCEEDED(hResult));

		pAsycInfo->Release();

		pAsyncOperation->Release();
	}

	ABI::Windows::Storage::Streams::IRandomAccessStream *pFileStream = NULL;
	{
		ABI::Windows::Foundation::IAsyncOperation<ABI::Windows::Storage::Streams::IRandomAccessStream*> *pAsyncOperation;
		switch (eOpenMode)
		{
		case PTSFileOpenMode::ReadOnly:
		{
			hResult = pFile->OpenAsync(ABI::Windows::Storage::FileAccessMode_Read, &pAsyncOperation);
			assert(SUCCEEDED(hResult));
		}
		break;
		case PTSFileOpenMode::ReadWrite:
		{
			hResult = pFile->OpenAsync(ABI::Windows::Storage::FileAccessMode_ReadWrite, &pAsyncOperation);
			assert(SUCCEEDED(hResult));	
		}
		break;
		default:
			pAsyncOperation = NULL;
			assert(0);
		}

		ABI::Windows::Foundation::IAsyncInfo *pAsycInfo;
		hResult = pAsyncOperation->QueryInterface(IID_PPV_ARGS(&pAsycInfo));
		assert(SUCCEEDED(hResult));

		ABI::Windows::Foundation::AsyncStatus eAsyncStatus;
		do {
			hResult = pAsycInfo->get_Status(&eAsyncStatus);
			assert(SUCCEEDED(hResult));
			//Wait
			::PTS_Yield();
		} while (eAsyncStatus == ABI::Windows::Foundation::AsyncStatus::Started);

		//assert(eAsyncStatus != ABI::Windows::Foundation::AsyncStatus::Canceled);
		//assert(eAsyncStatus != ABI::Windows::Foundation::AsyncStatus::Error);
		assert(eAsyncStatus == ABI::Windows::Foundation::AsyncStatus::Completed);

		hResult = pAsyncOperation->GetResults(&pFileStream);
		assert(SUCCEEDED(hResult));

		hResult = pAsycInfo->Close();
		assert(SUCCEEDED(hResult));

		pAsycInfo->Release();

		pAsyncOperation->Release();
	}

	//FileRandomAccessStream : IRandomAccessStream, IClosable, IInputStream, IOutputStream
	ABI::Windows::Storage::Streams::IInputStream *pFileStreamInput = NULL;
	ABI::Windows::Storage::Streams::IOutputStream *pFileStreamOutput = NULL;
	{
		switch (eOpenMode)
		{
		case PTSFileOpenMode::ReadOnly:
		{
			hResult = pFileStream->QueryInterface(IID_PPV_ARGS(&pFileStreamInput));
			assert(SUCCEEDED(hResult));
		}
		break;
		case PTSFileOpenMode::ReadWrite:
		{
			hResult = pFileStream->QueryInterface(IID_PPV_ARGS(&pFileStreamInput));
			assert(SUCCEEDED(hResult));

			hResult = pFileStream->QueryInterface(IID_PPV_ARGS(&pFileStreamOutput));
			assert(SUCCEEDED(hResult));
		}
		break;
		default:
			assert(0);
		}
	}
	
	::RoUninitialize();

	return static_cast<IPTSFile *>(::new(pStorage)PTSFileImpl(pFile, pFileStream, pFileStreamInput, pFileStreamOutput));
}

void PTCALL PTSFileSystemImpl::RootPath_Get(char *pPathName, size_t PathLength)
{
	HRESULT hResult;

	hResult = ::RoInitialize(RO_INIT_MULTITHREADED);
	assert(SUCCEEDED(hResult));

	ABI::Windows::Storage::IStorageItem *pStorageItem;
	hResult = m_pStorageFolder->QueryInterface(IID_PPV_ARGS(&pStorageItem));
	assert(SUCCEEDED(hResult));

	HSTRING hPath;
	hResult = pStorageItem->get_Path(&hPath);
	assert(SUCCEEDED(hResult));

	pStorageItem->Release();

	UINT32 uiResult;
	wchar_t const *wPathName = ::WindowsGetStringRawBuffer(hPath, &uiResult);
	assert(uiResult != 0U);

	size_t InCharsLeft = static_cast<size_t>(uiResult) + 1U;//包括'\0'
	size_t OutCharsLeft = PathLength;
	::PTSConv_UTF16ToUTF8(wPathName, &InCharsLeft, pPathName, &OutCharsLeft);

	::RoUninitialize();
}

inline PTSFileSystemImpl::PTSFileSystemImpl(ABI::Windows::Storage::IStorageFolder *pStorageFolder) :m_pStorageFolder(pStorageFolder)
{

}

IPTSFileSystem * PTCALL PTSFileSystem_Construct(PTSFileSystemStorage *pStorage)
{
	HRESULT hResult;
	
	hResult = ::RoInitialize(RO_INIT_MULTITHREADED);
	assert(SUCCEEDED(hResult));

	ABI::Windows::Storage::IApplicationDataStatics *pApplicationDataStatics;
	{
		HSTRING_HEADER hString_Header;
		HSTRING hString;
		hResult = ::WindowsCreateStringReference(::RuntimeClass_Windows_Storage_ApplicationData, _countof(::RuntimeClass_Windows_Storage_ApplicationData) - 1U, &hString_Header, &hString);
		assert(SUCCEEDED(hResult));
		hResult = ::RoGetActivationFactory(hString, IID_PPV_ARGS(&pApplicationDataStatics));
		assert(SUCCEEDED(hResult));
	}

	ABI::Windows::Storage::IApplicationData *pApplicationData;
	hResult = pApplicationDataStatics->get_Current(&pApplicationData);
	assert(SUCCEEDED(hResult));

	ABI::Windows::Storage::IStorageFolder *pStorageFolder;
	hResult = pApplicationData->get_LocalFolder(&pStorageFolder);
	assert(SUCCEEDED(hResult));
	
	pApplicationData->Release();

	pApplicationDataStatics->Release();

	::RoUninitialize();

	return static_cast<IPTSFileSystem *>(::new(pStorage)PTSFileSystemImpl(pStorageFolder));
}

#else
#error 未知的平台
#endif