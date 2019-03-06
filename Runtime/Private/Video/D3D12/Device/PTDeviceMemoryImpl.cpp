//GNU LESSER GENERAL PUBLIC LICENSE Version 3
//张羽乔

#include "PTDeviceMemory.h"

#include "../../Common/Device/PTSubAlloc.h"

#include <assert.h>
#include <new>

class PTDeviceMemoryNUMAImpl :public IPTDeviceMemory
{
	//Memory Management Strategies
	//http://msdn.microsoft.com/en-us/library/mt613239
	//Critical //Scaled/Optional //Streaming
	ID3D12Heap *m_pHeapAssetSRV = NULL;
	IPTSubAlloc *m_pSubAllocAssetSRV = NULL;
	ID3D12Heap *m_pHeapAssetVBVIBV = NULL;
	IPTSubAlloc *m_pSubAllocAssetVBVIBV = NULL;
	//Re-used 
	ID3D12Heap *m_pHeapFrameRTVDSVSRV = NULL;
	IPTSubAlloc *m_pSubAllocFrameRTVDSVSRV = NULL;
	ID3D12Heap *m_pHeapFrameUAVSRV = NULL;
	IPTSubAlloc *m_pSubAllocFrameUAVSRV = NULL;
	ID3D12Heap *m_pHeapFrameCBV = NULL;
	IPTSubAlloc *m_pSubAllocFrameCBV = NULL;
	//Streaming 
	ID3D12Heap *m_pHeapStreamCPSC = NULL;
	IPTSubAlloc *m_pSubAllocStreamCPSC = NULL;

	inline ~PTDeviceMemoryNUMAImpl()
	{
		m_pHeapStreamCPSC->Release();

		m_pHeapFrameRTVDSVSRV->Release();
		m_pHeapFrameUAVSRV->Release();
		m_pHeapFrameCBV->Release();

		m_pHeapAssetSRV->Release();
		m_pHeapAssetVBVIBV->Release();
	}

	void Destory(IPTHeap *pHeap) override
	{
		this->~PTDeviceMemoryNUMAImpl();
		pHeap->AlignedFree(this);
	}

	IPTDeviceResource *AssetSRVCreate(
		D3D12_RESOURCE_DIMENSION Dimension,
		UINT64 Width,
		UINT Height,
		UINT16 DepthOrArraySize,
		UINT16 MipLevels,
		DXGI_FORMAT Format
	) override
	{
		
	}

public:
	inline PTDeviceMemoryNUMAImpl(
		ID3D12Heap *pHeapAssetSRV,
		ID3D12Heap *pHeapAssetVBVIBV,
		ID3D12Heap *pHeapFrameRTVDSVSRV,
		ID3D12Heap *pHeapFrameUAVSRV,
		ID3D12Heap *pHeapFrameCBV,
		ID3D12Heap *pHeapStreamCPSC)
		:
		m_pHeapAssetSRV(pHeapAssetSRV),
		m_pHeapAssetVBVIBV(pHeapAssetVBVIBV),
		m_pHeapFrameRTVDSVSRV(pHeapFrameRTVDSVSRV),
		m_pHeapFrameUAVSRV(pHeapFrameUAVSRV),
		m_pHeapFrameCBV(pHeapFrameCBV),
		m_pHeapStreamCPSC(pHeapStreamCPSC)
	{

	}
};

class PTDeviceMemoryUMAImpl :public IPTDeviceMemory
{
	//Memory Management Strategies
	//http://msdn.microsoft.com/en-us/library/mt613239
	//Critical //Scaled/Optional //Streaming
	ID3D12Heap *m_pHeapAssetSRV = NULL;
	ID3D12Heap *m_pHeapAssetVBVIBV = NULL;
	//Re-used 
	ID3D12Heap *m_pHeapFrameRTVDSVSRV = NULL;
	ID3D12Heap *m_pHeapFrameUAVSRV = NULL;
	ID3D12Heap *m_pHeapFrameCBV = NULL;
	//Zero-copy UMA Optimization
	//https://msdn.microsoft.com/en-us/library/dn859384
	//https://msdn.microsoft.com/en-us/library/dn914416

	inline ~PTDeviceMemoryUMAImpl()
	{
		m_pHeapFrameRTVDSVSRV->Release();
		m_pHeapFrameUAVSRV->Release();
		m_pHeapFrameCBV->Release();

		m_pHeapAssetSRV->Release();
		m_pHeapAssetVBVIBV->Release();
	}

	void Destory(IPTHeap *pHeap) override
	{
		this->~PTDeviceMemoryUMAImpl();
		pHeap->AlignedFree(this);
	}

public:
	inline PTDeviceMemoryUMAImpl(
		ID3D12Heap *pHeapAssetSRV,
		ID3D12Heap *pHeapAssetVBVIBV,
		ID3D12Heap *pHeapFrameRTVDSVSRV,
		ID3D12Heap *pHeapFrameUAVSRV,
		ID3D12Heap *pHeapFrameCBV)
		:
		m_pHeapAssetSRV(pHeapAssetSRV),
		m_pHeapAssetVBVIBV(pHeapAssetVBVIBV),
		m_pHeapFrameRTVDSVSRV(pHeapFrameRTVDSVSRV),
		m_pHeapFrameUAVSRV(pHeapFrameUAVSRV),
		m_pHeapFrameCBV(pHeapFrameCBV)
	{

	}
};


IPTDeviceMemory *PTDeviceMemoryNUMANCC_Create(
	ID3D12Device *pDevice,
	IPTHeap *pHeap,
	uint64_t SizeMaxAssetSRV,
	uint64_t SizeMaxAssetVBVIBV,
	uint64_t SizeMaxFrameRTVDSVSRV,
	uint64_t SizeMaxFrameUAVSRV,
	uint64_t SizeMaxFrameCBV,
	uint64_t SizeMaxStreamCPSC)
{
	ID3D12Heap *pHeapAssetSRV = NULL;
	ID3D12Heap *pHeapAssetVBVIBV = NULL;
	ID3D12Heap *pHeapFrameRTVDSVSRV = NULL;
	ID3D12Heap *pHeapFrameUAVSRV = NULL;
	ID3D12Heap *pHeapFrameCBV = NULL;
	ID3D12Heap *pHeapStreamCPSC = NULL;

	D3D12_HEAP_DESC HeapDesc;
	HeapDesc.Properties.CreationNodeMask = 0X1U;
	HeapDesc.Properties.VisibleNodeMask = 0X1U;

	HeapDesc.Properties.Type = D3D12_HEAP_TYPE_CUSTOM;
	HeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE;
	HeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_L1;

	HeapDesc.SizeInBytes = SizeMaxAssetSRV;
	HeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;//No MSAA
	HeapDesc.Flags = D3D12_HEAP_FLAG_DENY_BUFFERS | D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES;
	pDevice->CreateHeap(&HeapDesc, IID_PPV_ARGS(&pHeapAssetSRV));

	HeapDesc.SizeInBytes = SizeMaxAssetVBVIBV;
	HeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;//No MSAA
	HeapDesc.Flags = D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES | D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES;
	pDevice->CreateHeap(&HeapDesc, IID_PPV_ARGS(&pHeapAssetVBVIBV));


	HeapDesc.SizeInBytes = SizeMaxFrameRTVDSVSRV;
	HeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;//No MSAA
	HeapDesc.Flags = D3D12_HEAP_FLAG_DENY_BUFFERS | D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES;
	pDevice->CreateHeap(&HeapDesc, IID_PPV_ARGS(&pHeapFrameRTVDSVSRV));

	HeapDesc.SizeInBytes = SizeMaxFrameUAVSRV;
	HeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;//No MSAA
	HeapDesc.Flags = D3D12_HEAP_FLAG_DENY_BUFFERS | D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES;
	pDevice->CreateHeap(&HeapDesc, IID_PPV_ARGS(&pHeapFrameUAVSRV));

	HeapDesc.Properties.Type = D3D12_HEAP_TYPE_CUSTOM;
	HeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE;
	HeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	HeapDesc.SizeInBytes = SizeMaxFrameCBV;
	HeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;//No MSAA
	HeapDesc.Flags = D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES | D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES;
	pDevice->CreateHeap(&HeapDesc, IID_PPV_ARGS(&pHeapFrameCBV));

	HeapDesc.SizeInBytes = SizeMaxStreamCPSC;
	HeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;//No MSAA
	HeapDesc.Flags = D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES | D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES;
	pDevice->CreateHeap(&HeapDesc, IID_PPV_ARGS(&pHeapStreamCPSC));

	void *pMemory = pHeap->AlignedAlloc(sizeof(PTDeviceMemoryNUMAImpl), alignof(PTDeviceMemoryNUMAImpl));
	assert(pMemory != NULL);

	return static_cast<IPTDeviceMemory *>(
		new(pMemory)PTDeviceMemoryNUMAImpl(
			pHeapAssetSRV,
			pHeapAssetVBVIBV,
			pHeapFrameRTVDSVSRV,
			pHeapFrameUAVSRV,
			pHeapFrameCBV,
			pHeapStreamCPSC
		)
		);
}


IPTDeviceMemory *PTDeviceMemoryUMANCC_Create(
	ID3D12Device *pDevice,
	IPTHeap *pHeap,
	uint64_t SizeMaxAssetSRV,
	uint64_t SizeMaxAssetVBVIBV,
	uint64_t SizeMaxFrameRTVDSVSRV,
	uint64_t SizeMaxFrameUAVSRV,
	uint64_t SizeMaxFrameCBV)
{
	ID3D12Heap *pHeapAssetSRV = NULL;
	ID3D12Heap *pHeapAssetVBVIBV = NULL;
	ID3D12Heap *pHeapFrameRTVDSVSRV = NULL;
	ID3D12Heap *pHeapFrameUAVSRV = NULL;
	ID3D12Heap *pHeapFrameCBV = NULL;

	D3D12_HEAP_DESC HeapDesc;
	HeapDesc.Properties.CreationNodeMask = 0X1U;
	HeapDesc.Properties.VisibleNodeMask = 0X1U;

	HeapDesc.Properties.Type = D3D12_HEAP_TYPE_CUSTOM;
	HeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE;
	HeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	HeapDesc.SizeInBytes = SizeMaxAssetSRV;
	HeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;//No MSAA
	HeapDesc.Flags = D3D12_HEAP_FLAG_DENY_BUFFERS | D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES;
	pDevice->CreateHeap(&HeapDesc, IID_PPV_ARGS(&pHeapAssetSRV));

	HeapDesc.SizeInBytes = SizeMaxAssetVBVIBV;
	HeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;//No MSAA
	HeapDesc.Flags = D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES | D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES;
	pDevice->CreateHeap(&HeapDesc, IID_PPV_ARGS(&pHeapAssetVBVIBV));


	HeapDesc.SizeInBytes = SizeMaxFrameRTVDSVSRV;
	HeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;//No MSAA
	HeapDesc.Flags = D3D12_HEAP_FLAG_DENY_BUFFERS | D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES;
	pDevice->CreateHeap(&HeapDesc, IID_PPV_ARGS(&pHeapFrameRTVDSVSRV));

	HeapDesc.SizeInBytes = SizeMaxFrameUAVSRV;
	HeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;//No MSAA
	HeapDesc.Flags = D3D12_HEAP_FLAG_DENY_BUFFERS | D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES;
	pDevice->CreateHeap(&HeapDesc, IID_PPV_ARGS(&pHeapFrameUAVSRV));

	HeapDesc.SizeInBytes = SizeMaxFrameCBV;
	HeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;//No MSAA
	HeapDesc.Flags = D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES | D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES;
	pDevice->CreateHeap(&HeapDesc, IID_PPV_ARGS(&pHeapFrameCBV));

	void *pMemory = pHeap->AlignedAlloc(sizeof(PTDeviceMemoryUMAImpl), alignof(PTDeviceMemoryUMAImpl));
	assert(pMemory != NULL);

	return static_cast<IPTDeviceMemory *>(
		new(pMemory)PTDeviceMemoryUMAImpl(
			pHeapAssetSRV,
			pHeapAssetVBVIBV,
			pHeapFrameRTVDSVSRV,
			pHeapFrameUAVSRV,
			pHeapFrameCBV
		)
		);
}

IPTDeviceMemory *PTDeviceMemoryUMACC_Create(
	ID3D12Device *pDevice,
	IPTHeap *pHeap,
	uint64_t SizeMaxAssetSRV,
	uint64_t SizeMaxAssetVBVIBV,
	uint64_t SizeMaxFrameRTVDSVSRV,
	uint64_t SizeMaxFrameUAVSRV,
	uint64_t SizeMaxFrameCBV)
{
	ID3D12Heap *pHeapAssetSRV = NULL;
	ID3D12Heap *pHeapAssetVBVIBV = NULL;
	ID3D12Heap *pHeapFrameRTVDSVSRV = NULL;
	ID3D12Heap *pHeapFrameUAVSRV = NULL;
	ID3D12Heap *pHeapFrameCBV = NULL;

	D3D12_HEAP_DESC HeapDesc;
	HeapDesc.Properties.CreationNodeMask = 0X1U;
	HeapDesc.Properties.VisibleNodeMask = 0X1U;

	HeapDesc.Properties.Type = D3D12_HEAP_TYPE_CUSTOM;
	HeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	HeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	HeapDesc.SizeInBytes = SizeMaxAssetSRV;
	HeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;//No MSAA
	HeapDesc.Flags = D3D12_HEAP_FLAG_DENY_BUFFERS | D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES;
	pDevice->CreateHeap(&HeapDesc, IID_PPV_ARGS(&pHeapAssetSRV));

	HeapDesc.SizeInBytes = SizeMaxAssetVBVIBV;
	HeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;//No MSAA
	HeapDesc.Flags = D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES | D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES;
	pDevice->CreateHeap(&HeapDesc, IID_PPV_ARGS(&pHeapAssetVBVIBV));


	HeapDesc.SizeInBytes = SizeMaxFrameRTVDSVSRV;
	HeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;//No MSAA
	HeapDesc.Flags = D3D12_HEAP_FLAG_DENY_BUFFERS | D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES;
	pDevice->CreateHeap(&HeapDesc, IID_PPV_ARGS(&pHeapFrameRTVDSVSRV));

	HeapDesc.SizeInBytes = SizeMaxFrameUAVSRV;
	HeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;//No MSAA
	HeapDesc.Flags = D3D12_HEAP_FLAG_DENY_BUFFERS | D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES;
	pDevice->CreateHeap(&HeapDesc, IID_PPV_ARGS(&pHeapFrameUAVSRV));

	HeapDesc.SizeInBytes = SizeMaxFrameCBV;
	HeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;//No MSAA
	HeapDesc.Flags = D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES | D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES;
	pDevice->CreateHeap(&HeapDesc, IID_PPV_ARGS(&pHeapFrameCBV));

	void *pMemory = pHeap->AlignedAlloc(sizeof(PTDeviceMemoryUMAImpl), alignof(PTDeviceMemoryUMAImpl));
	assert(pMemory != NULL);

	return static_cast<IPTDeviceMemory *>(
		new(pMemory)PTDeviceMemoryUMAImpl(
			pHeapAssetSRV,
			pHeapAssetVBVIBV,
			pHeapFrameRTVDSVSRV,
			pHeapFrameUAVSRV,
			pHeapFrameCBV
		)
		);
}