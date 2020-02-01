#include "PTDeviceImpl.h"

#include "../../../../Public/McRT/PTExecutionSystem.h"

#include <assert.h>
#include <new>

extern "C" PTGRAPHICAPI IPTGDevice * PTCALL PTGDevice_Construct(PTGDeviceStorage *Storage)
{
	HRESULT hResult;

#ifndef NDEBUG
	ID3D12Debug *pDebug;
	hResult = ::D3D12GetDebugInterface(IID_PPV_ARGS(&pDebug));
	assert(SUCCEEDED(hResult));
	pDebug->EnableDebugLayer();
	pDebug->Release();
#endif

	IDXGIFactory4 *pDXGIFactory;
	hResult = ::CreateDXGIFactory2(
#ifdef PTDEBUG
		DXGI_CREATE_FACTORY_DEBUG,
#else
		0U,
#endif
		IID_PPV_ARGS(&pDXGIFactory)
	);
	assert(SUCCEEDED(hResult));


	ID3D12Device *pDevice = NULL;

	for (UINT i = 0U; ; ++i)
	{
		IDXGIAdapter1 *pAdapter;
		if (FAILED(pDXGIFactory->EnumAdapters1(i, &pAdapter)))
		{
			break;
		}

		bool bBreak = false;

		do {
			DXGI_ADAPTER_DESC1 AdapterDesc;
			pAdapter->GetDesc1(&AdapterDesc);
			if (AdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				break;
			}

			ID3D12Device *pDeviceTemp = NULL;
			if (FAILED(::D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDeviceTemp))))
			{
				break;
			}

			do {
				D3D12_FEATURE_DATA_ARCHITECTURE FeatureSupportData;
				FeatureSupportData.NodeIndex = 0U;//[In]
				pDeviceTemp->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE, &FeatureSupportData, sizeof(D3D12_FEATURE_DATA_ARCHITECTURE));

				//独立显卡
				if (FeatureSupportData.UMA != TRUE)
				{
					//独立显卡优先
					if (pDevice != NULL)
					{
						pDevice->Release();
					}
					//Steal
					pDevice = pDeviceTemp;
					pDeviceTemp = NULL;
					bBreak = true;//迭代结束
				}

				//集成显卡
				if (FeatureSupportData.UMA == TRUE)
				{
					//主显卡优先
					if (pDevice == NULL)
					{
						//Steal
						pDevice = pDeviceTemp;
						pDeviceTemp = NULL;
					}
				}

			} while (false);

			if (pDeviceTemp != NULL)
			{
				pDeviceTemp->Release();
			}

		} while (false);

		pAdapter->Release();

		if (bBreak)
		{
			break;
		}
	}

	if (pDevice == NULL)
	{
		return NULL;
	}

	return	::new(Storage)PTGraphicDeviceImpl(pDXGIFactory,pDevice);
}

inline PTGraphicDeviceImpl::~PTGraphicDeviceImpl()
{
#if 0
	bool PTResult;

	PTSocketDatagramStream SocketDatagramStream;
	SocketDatagramStream.m_CommandType = PTSocketDatagramStream::QUIT;

	PTSocketIOVector SocketIOVector[1];
	SocketIOVector[0].Base = static_cast<void *>(&SocketDatagramStream);
	SocketIOVector[0].Len = sizeof(PTSocketDatagramStream);
	uint32_t NumberOfBytesWritten[1];
	PTResult = ::PTSocketWriteVector(&m_hSocket, SocketIOVector, 1U, NumberOfBytesWritten);
	assert(PTResult);

	PTResult = ::PTThreadWait(&m_hThreadStream);
	assert(PTResult);

	PTResult = ::PTSocketDestory(&m_hSocketStream);
	assert(PTResult);

	PTResult = ::PTSocketDestory(&m_hSocket);
	assert(PTResult);

	m_pDeviceMemoryStream->Destory(m_pHeapStream);
	m_pHeapStream->Destroy();
	m_pDevice->Release();
	m_pDXGIFactory->Release();
#endif
}

void PTGraphicDeviceImpl::Destruct()
{
	this->~PTGraphicDeviceImpl();
}

struct PTSocketDatagramStream
{
	enum :uint32_t
	{
		QUIT
	}m_CommandType;
	uint32_t huhuhu;
};

struct PTParamStream
{
	IPTHeap *pHeap;
	IPTDeviceMemory *pDeviceMemory;
	PTSocket hSocket;
	PTSemaphore hSemaphore;
};

inline unsigned __stdcall PTMainStream(void *pThreadParam);

void PTGraphicDeviceImpl::MemoryInitialize(
	//Critical //Scaled/Optional //Streaming
	uint64_t SizeMaxAssetSRV,
	uint64_t SizeMaxAssetVBVIBV,
	//Re-used 
	uint64_t SizeMaxFrameRTVDSVSRV,
	uint64_t SizeMaxFrameUAVSRV,
	uint64_t SizeMaxFrameCBV,
	//Streaming 
	uint64_t SizeMaxStreamCPSC
)
{
#if 0
	m_pHeapStream = ::PTMemorySysem_Get()->HeapCreate(512U * 1024U * 1024U);

	D3D12_FEATURE_DATA_ARCHITECTURE FeatureSupportData;
	FeatureSupportData.NodeIndex = 0U;//[In]
	m_pDevice->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE, &FeatureSupportData, sizeof(D3D12_FEATURE_DATA_ARCHITECTURE));

	m_pDeviceMemoryStream =
		(FeatureSupportData.UMA != TRUE) ?
		(::PTDeviceMemoryNUMANCC_Create(
			m_pDevice,
			m_pHeapStream,
			SizeMaxAssetSRV,
			SizeMaxAssetVBVIBV,
			SizeMaxFrameRTVDSVSRV,
			SizeMaxFrameUAVSRV,
			SizeMaxFrameCBV,
			SizeMaxStreamCPSC)) :
			((FeatureSupportData.CacheCoherentUMA != TRUE) ?
		(::PTDeviceMemoryUMANCC_Create(
			m_pDevice,
			m_pHeapStream,
			SizeMaxAssetSRV,
			SizeMaxAssetVBVIBV,
			SizeMaxFrameRTVDSVSRV,
			SizeMaxFrameUAVSRV,
			SizeMaxFrameCBV)) :
			(::PTDeviceMemoryUMACC_Create(
				m_pDevice,
				m_pHeapStream,
				SizeMaxAssetSRV,
				SizeMaxAssetVBVIBV,
				SizeMaxFrameRTVDSVSRV,
				SizeMaxFrameUAVSRV,
				SizeMaxFrameCBV))
				);

	bool PTResult;

	PTResult = ::PTSocketDatagramPair(&m_hSocket, &m_hSocketStream);
	assert(PTResult);

	PTParamStream ThreadParam;
	ThreadParam.pHeap = m_pHeapStream;
	ThreadParam.pDeviceMemory = m_pDeviceMemoryStream;
	ThreadParam.hSocket = m_hSocketStream;

	PTResult = ::PTSemaphoreCreate(0U, &ThreadParam.hSemaphore);
	assert(PTResult);

	PTResult = ::PTThreadCreate(PTMainStream, &ThreadParam, &m_hThreadStream);
	assert(PTResult);

	PTResult = ::PTSemaphorePassern(&ThreadParam.hSemaphore);
	assert(PTResult);

	PTResult = ::PTSemaphoreDestory(&ThreadParam.hSemaphore);
	assert(PTResult);
#endif
}

//类似于实现文件资源管理器
//异步加载类似于滚动条只显示部分图像（缩略图）
//游戏逻辑在内存中创建类似与临时文件

inline unsigned __stdcall PTMainStream(void *pThreadParam)
{
#if 0
	{
		BOOL Result = ::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
		assert(Result != FALSE);
	}

	IPTHeap *pHeap;
	IPTDeviceMemory *pDeviceMemory;
	PTSocket hSocket;
	{
		PTParamStream *pThreadGraphicStreamParam = static_cast<PTParamStream *>(pThreadParam);
		pHeap = pThreadGraphicStreamParam->pHeap;
		pDeviceMemory = pThreadGraphicStreamParam->pDeviceMemory;
		hSocket = pThreadGraphicStreamParam->hSocket;
		bool PTResult;
		PTResult = ::PTSemaphoreVrijgeven(&pThreadGraphicStreamParam->hSemaphore);
		assert(PTResult);
	}

	PTSocketDatagramStream SocketDatagram;

	PTSocketIOVector SocketIOVector[1];
	SocketIOVector[0].Base = static_cast<void *>(&SocketDatagram);
	SocketIOVector[0].Len = sizeof(PTSocketDatagramStream);
	uint32_t NumberOfBytesRead[1];
	bool PTResult;

	do {
		PTResult = ::PTSocketReadVector(&hSocket, SocketIOVector, 1U, NumberOfBytesRead);
		assert(PTResult);
	} while (SocketDatagram.m_CommandType != PTSocketDatagramStream::QUIT);

	return 0U;
#endif
}


IPTGMesh *PTGraphicDeviceImpl::MeshCreate(
	char const *pFileName,
	void (PTPTR *pVertexCountDecoder)(
		IPTFile *pFile,
		uint32_t *pVertexCountOut
		),
	void (PTPTR *pVertexDataDecoder)(
		IPTFile *pFile,
		PTFORMAT_R32G32B32_FLOAT PositionArrayOut[],
		PTFORMAT_R8G8B8A8_SNORM NormalArrayOut[],
		PTFORMAT_R8G8B8A8_SNORM TangentArrayOut[],
		PTFORMAT_R8G8B8A8_SNORM BiTangentArrayOut[],
		//PBR(Physically Based Rendering)
		PTFORMAT_R8G8_UNORM UVArrayOut_Normal[],
		PTFORMAT_R8G8_UNORM UVArrayOut_Albedo[],
		PTFORMAT_R8G8_UNORM UVArrayOut_ColorSpecular[],
		PTFORMAT_R8G8_UNORM UVArrayOut_Glossiness[]
		),
	void (PTPTR *pIndexCountDecoder)(
		IPTFile *pFile,
		uint32_t *pIndexCountOut
		),
	void (PTPTR *pIndexDataDecoder)(
		IPTFile *pFile,
		uint32_t IndexArrayOut[]
		)
)
{
	return NULL;
}

IPTGMeshSkin *PTGraphicDeviceImpl::MeshSkinCreate(
	char const *pFileName,
	void (PTPTR *pVertexCountDecoder)(
		IPTFile *pFile,
		uint32_t *pVertexCountOut
		),
	void (PTPTR *pVertexDataDecoder)(
		IPTFile *pFile,
		PTFORMAT_R32G32B32_FLOAT PositionArrayOut[],
		PTFORMAT_R8G8B8A8_SNORM NormalArrayOut[],
		PTFORMAT_R8G8B8A8_SNORM TangentArrayOut[],
		PTFORMAT_R8G8B8A8_SNORM BiTangentArrayOut[],
		//PBR(Physically Based Rendering)
		PTFORMAT_R8G8_UNORM UVArrayOut_Normal[],
		PTFORMAT_R8G8_UNORM UVArrayOut_Albedo[],
		PTFORMAT_R8G8_UNORM UVArrayOut_Specular[],
		PTFORMAT_R8G8_UNORM UVArrayOut_Glossiness[],
		//Skin <-> VertexBlend
		PTFORMAT_R8G8B8A8_UINT BlendIndexArrayOut[],
		PTFORMAT_R8G8B8A8_UNORM BlendWeightArrayOut[]
		),
	void (PTPTR *pIndexCountDecoder)(
		IPTFile *pFile,
		uint32_t *pIndexCountOut
		),
	void (PTPTR *pIndexDataDecoder)(
		IPTFile *pFile,
		uint32_t IndexArrayOut[]
		),
	void (PTPTR *pJointCountDecoder)(
		IPTFile *pFile,
		uint32_t *pJointCountOut
		),
	void (PTPTR *pJointDataDecoder)(
		IPTFile *pFile,
		PTM44F JointArrayOut_InverseBind[]
		)
)
{
	return NULL;
}

IPTGRender *PTGraphicDeviceImpl::RenderConstruct(PTRenderStorage *pStorage)
{
	return NULL;
}

IPTGRenderVR *PTGraphicDeviceImpl::RenderVRConstruct(PTRenderVRStorage *pStorage)
{
	return NULL;
}

/*

inline IPTGraphicDevice * PTGraphicDeviceNUMA_Create(IPTHeap *pHeap, IDXGIFactory4 *pDXGIFactory, ID3D12Device *pDevice)
{
	HRESULT Result;

	ID3D12CommandQueue *pCommandQueueDirect = NULL;
	ID3D12CommandQueue *pCommandQueueCompute = NULL;
	ID3D12CommandQueue *pCommandQueueCopy = NULL;
	{
		D3D12_COMMAND_QUEUE_DESC CommandQueueDesc;
		CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		CommandQueueDesc.NodeMask = 0X00000001U;

		CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		CommandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		Result = pDevice->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(&pCommandQueueDirect));
		assert(SUCCEEDED(Result));

		CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		CommandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		Result = pDevice->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(&pCommandQueueCompute));
		assert(SUCCEEDED(Result));

		CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
		CommandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
		Result = pDevice->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(&pCommandQueueCopy));
		assert(SUCCEEDED(Result));
	}

	ID3D12CommandAllocator *pCommandAllocatorDirect = NULL;
	ID3D12CommandAllocator *pCommandAllocatorCompute = NULL;
	ID3D12CommandAllocator *pCommandAllocatorCopy = NULL;
	{
		Result = pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pCommandAllocatorDirect));
		assert(SUCCEEDED(Result));
		Result = pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&pCommandAllocatorCompute));
		assert(SUCCEEDED(Result));
		Result = pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&pCommandAllocatorCopy));
		assert(SUCCEEDED(Result));
	}

	void *pMemory = pHeap->AlignedAlloc(sizeof(PTGraphicDeviceNUMAImpl), alignof(PTGraphicDeviceNUMAImpl));
	assert(pMemory != NULL);

	return static_cast<IPTGraphicDevice *>(
		new(pMemory)PTGraphicDeviceNUMAImpl(
			pDXGIFactory,
			pDevice,
			pCommandQueueDirect,
			pCommandQueueCompute,
			pCommandQueueCopy,
			pCommandAllocatorDirect,
			pCommandAllocatorCompute,
			pCommandAllocatorCopy
		)
		);
}

inline IPTGraphicDevice * PTGraphicDeviceUMA_Create(IPTHeap *pHeap, IDXGIFactory4 *pDXGIFactory, ID3D12Device *pDevice, bool IsNCC)
{
	HRESULT Result;

	ID3D12CommandQueue *pCommandQueueDirect = NULL;
	ID3D12CommandQueue *pCommandQueueCompute = NULL;
	{
		D3D12_COMMAND_QUEUE_DESC CommandQueueDesc;
		CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		CommandQueueDesc.NodeMask = 0X00000001U;

		CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		CommandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		Result = pDevice->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(&pCommandQueueDirect));
		assert(SUCCEEDED(Result));

		CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		CommandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		Result = pDevice->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(&pCommandQueueCompute));
		assert(SUCCEEDED(Result));
	}

	ID3D12CommandAllocator *pCommandAllocatorDirect = NULL;
	ID3D12CommandAllocator *pCommandAllocatorCompute = NULL;
	{
		Result = pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pCommandAllocatorDirect));
		assert(SUCCEEDED(Result));
		Result = pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&pCommandAllocatorCompute));
		assert(SUCCEEDED(Result));
	}

	void *pMemory = pHeap->AlignedAlloc(sizeof(PTGraphicDeviceUMAImpl), alignof(PTGraphicDeviceUMAImpl));
	assert(pMemory != NULL);

	return static_cast<IPTGraphicDevice *>(
		new(pMemory)PTGraphicDeviceUMAImpl(
			pDXGIFactory,
			pDevice,
			pCommandQueueDirect,
			pCommandQueueCompute,
			pCommandAllocatorDirect,
			pCommandAllocatorCompute
		)
		);
}

*/