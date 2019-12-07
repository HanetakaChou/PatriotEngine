#include "PTSFileImpl_Win32Runtime.h"
#include <new>
#include <assert.h>
#include <roapi.h>
#include <robuffer.h>
#include "../../../Public/McRT/Win32/PTSConvUTF.h"
#include "../../../Public/McRT/PTSMemoryAllocator.h"
#include "../../../Public/McRT/PTSThread.h"

inline PTSFileSystemImpl::PTSFileSystemImpl(ABI::Windows::Storage::IStorageFolder *pStorageFolder)
	:
	m_pStorageFolder(pStorageFolder)
{
	ABI::Windows::Storage::IStorageItem *pStorageItem;
	HRESULT hResult = m_pStorageFolder->QueryInterface(IID_PPV_ARGS(&pStorageItem));
	assert(SUCCEEDED(hResult));

	HSTRING hStr_Path;
	hResult = pStorageItem->get_Path(&hStr_Path);
	assert(SUCCEEDED(hResult));

	pStorageItem->Release();

	UINT32 uiResult;
	wchar_t const *wStr_Path = ::WindowsGetStringRawBuffer(hStr_Path, &uiResult);
	assert(uiResult != 0U);

	size_t InCharsLeft = static_cast<size_t>(uiResult) + 1U;//包括'\0'
	size_t OutCharsLeft = 0X10000U;
	::PTSConv_UTF16ToUTF8(wStr_Path, &InCharsLeft, m_RootPath, &OutCharsLeft);
}

inline PTSFileSystemImpl::~PTSFileSystemImpl()
{
	m_pStorageFolder->Release();
}

void PTSFileSystemImpl::Internal_Release()
{
	this->~PTSFileSystemImpl();
	::PTSMemoryAllocator_Free_Aligned(this);
}

//线程必须处于COM套间中
static PTSTSD_KEY s_FileSystem_Index;

static int32_t s_FileSystem_Initialize_RefCount = 0;
extern "C" PTMCRTAPI PTBOOL PTCALL PTSFileSystem_Initialize()
{
	if (::PTSAtomic_GetAndAdd(&s_FileSystem_Initialize_RefCount, 1) == 0)
	{
		PTBOOL tbResult;
		tbResult = ::PTSTSD_Create(
			&s_FileSystem_Index,
			[](void *pVoid)->void {
			//Destructtor
		
			PTSFileSystemImpl *pTaskScheduler = static_cast<PTSFileSystemImpl *>(pVoid);
			pTaskScheduler->Internal_Release();

			//COM套间
			::RoUninitialize();
		}
		);
		assert(tbResult != PTFALSE);
		return tbResult;
	}
	else
	{
		return PTTRUE;
	}
}

extern "C" PTMCRTAPI IPTSFileSystem * PTCALL PTSFileSystem_ForProcess()
{
	PTSFileSystemImpl *pFileSystem = static_cast<PTSFileSystemImpl *>(::PTSTSD_GetValue(s_FileSystem_Index));

	if (pFileSystem == NULL)
	{
		HRESULT hResult;

		//COM套间
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

		pFileSystem = ::new(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSFileSystemImpl), alignof(PTSFileSystemImpl)))PTSFileSystemImpl(pStorageFolder);
		assert(pFileSystem != NULL);

		PTBOOL tbResult = ::PTSTSD_SetValue(s_FileSystem_Index, pFileSystem);
		assert(tbResult != PTFALSE);
	}

	return pFileSystem;
}

char const * PTCALL PTSFileSystemImpl::RootPath()
{
	return m_RootPath;
}

IPTSFile * PTCALL PTSFileSystemImpl::File_Create(uint32_t OpenMode, char const *pFileName)
{
	HRESULT hResult;

	wchar_t wFileName[0X10000];
	size_t InCharsLeft = ::strlen(reinterpret_cast<const char *>(pFileName)) + 1U;//包括'\0'
	size_t OutCharsLeft = 0X10000;
	::PTSConv_UTF8ToUTF16(pFileName, &InCharsLeft, wFileName, &OutCharsLeft);

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
		switch (OpenMode)
		{
		case FILE_OPEN_READONLY:
		{
			hResult = pFile->OpenAsync(ABI::Windows::Storage::FileAccessMode_Read, &pAsyncOperation);
			assert(SUCCEEDED(hResult));
		}
		break;
		case FILE_OPEN_READWRITE:
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
		case FILE_OPEN_READONLY:
		{
			hResult = pFileStream->QueryInterface(IID_PPV_ARGS(&pFileStreamInput));
			assert(SUCCEEDED(hResult));
		}
		break;
		case FILE_OPEN_READWRITE:
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

	return static_cast<IPTSFile *>(::new(
		::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSFileImpl),alignof(PTSFileImpl))
		)PTSFileImpl(pFile, pFileStream, pFileStreamInput, pFileStreamOutput));
}

inline PTSFileImpl::PTSFileImpl(
	ABI::Windows::Storage::IStorageFile *pFile, 
	ABI::Windows::Storage::Streams::IRandomAccessStream *pFileStream, 
	ABI::Windows::Storage::Streams::IInputStream *pFileStreamInput, 
	ABI::Windows::Storage::Streams::IOutputStream *pFileStreamOutput) 
	:
	m_pFile(pFile),
	m_pFileStream(pFileStream), 
	m_pFileStreamInput(pFileStreamInput),
	m_pFileStreamOutput(pFileStreamOutput)
{

}

inline PTSFileImpl::~PTSFileImpl()
{
	if (m_pFileStreamOutput != NULL)
	{
		m_pFileStreamOutput->Release();
	}

	m_pFileStreamInput->Release();

	m_pFileStream->Release();

	m_pFile->Release();
}


void PTCALL PTSFileImpl::Release()
{
	this->~PTSFileImpl();
	::PTSMemoryAllocator_Free_Aligned(this);
}

int64_t PTCALL PTSFileImpl::Size()
{
	UINT64 uiSize;
	HRESULT hResult = m_pFileStream->get_Size(&uiSize);
	assert(SUCCEEDED(hResult));

	return static_cast<int64_t>(uiSize);
}

intptr_t PTCALL PTSFileImpl::Read(void *pBuffer, uintptr_t Count)
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
	
	l_Lambda.m_Capacity = static_cast<UINT32>(Count);
	l_Lambda.m_Length = 0U;
	l_Lambda.pByte = static_cast<BYTE *>(pBuffer);

	HRESULT hResult;

	ABI::Windows::Foundation::IAsyncOperationWithProgress<ABI::Windows::Storage::Streams::IBuffer*, UINT32> *pAsyncOperation;
	hResult = m_pFileStreamInput->ReadAsync(&l_Lambda, static_cast<UINT32>(Count), ABI::Windows::Storage::Streams::InputStreamOptions_None, &pAsyncOperation);
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

	assert(eAsyncStatus == ABI::Windows::Foundation::AsyncStatus::Completed);

	ABI::Windows::Storage::Streams::IBuffer *pRuntimeObjectBuffer;
	hResult = pAsyncOperation->GetResults(&pRuntimeObjectBuffer);
	assert(SUCCEEDED(hResult));
	assert(pRuntimeObjectBuffer == static_cast<ABI::Windows::Storage::Streams::IBuffer *>(&l_Lambda));

	hResult = pAsycInfo->Close();
	assert(SUCCEEDED(hResult));

	pAsycInfo->Release();

	pAsyncOperation->Release();

	return SUCCEEDED(hResult) ? static_cast<intptr_t>(l_Lambda.m_Length) : -1;
}

intptr_t PTCALL PTSFileImpl::Write(void *pBuffer, uintptr_t Count)
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

	l_Lambda.m_Capacity = static_cast<UINT32>(Count);
	l_Lambda.m_Length = static_cast<UINT32>(Count);
	l_Lambda.pByte = static_cast<BYTE *>(pBuffer);

	HRESULT hResult;

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

	assert(eAsyncStatus == ABI::Windows::Foundation::AsyncStatus::Completed);

	UINT32 CountDone;
	hResult = pAsyncOperation->GetResults(&CountDone);
	assert(SUCCEEDED(hResult));

	hResult = pAsycInfo->Close();
	assert(SUCCEEDED(hResult));

	pAsycInfo->Release();

	pAsyncOperation->Release();

	return SUCCEEDED(hResult) ? static_cast<intptr_t>(CountDone) : -1;
}

int64_t PTCALL PTSFileImpl::Seek(uint32_t Whence, int64_t Offset)
{
	UINT64 Position_Seek;
	HRESULT hResult;

	switch (Whence)
	{
	case FILE_SEEK_BEGIN:
	{
		Position_Seek = Offset;
	}
	break;
	case FILE_SEEK_CURRENT:
	{
		UINT64 Position_Current;
		hResult = m_pFileStream->get_Position(&Position_Current);
		assert(SUCCEEDED(hResult));
		Position_Seek = Position_Current + Offset;
	}
	break;
	case FILE_SEEK_END:
	{
		UINT64 Position_End;
		hResult = m_pFileStream->get_Size(&Position_End);
		assert(SUCCEEDED(hResult));
		Position_Seek = Position_End + Offset;
	}
	break;
	default:
		assert(0);
	}

	hResult = m_pFileStream->Seek(Position_Seek);
	assert(SUCCEEDED(hResult));

	return SUCCEEDED(hResult) ? static_cast<int64_t>(Position_Seek) : -1;
}






