#include "PTDeviceMemory.h"

#include "../../Common/Device/PTSubAlloc.h"

#include <assert.h>
#include <new>
#include <DirectXCollision.h>

class PTDeviceMemoryUMAImpl :public IPTDeviceMemory
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
		m_pSubAllocFrameCBV->Destroy(pHeap);
		m_pSubAllocFrameUAVSRV->Destroy(pHeap);
		m_pSubAllocFrameRTVDSVSRV->Destroy(pHeap);

		m_pSubAllocAssetVBVIBV->Destroy(pHeap);
		m_pSubAllocAssetSRV->Destroy(pHeap);

		this->~PTDeviceMemoryUMAImpl();
		pHeap->AlignedFree(this);
	}

	void EnqueueAssetSRVTexture2DCreate(
		ID3D12Device *pRenderMainDevice,
		ID3D12CommandQueue *,//Zero-copy UMA Optimization
		IPTHeap *pStreamMainHeap,
		UINT64 Width,
		UINT Height,
		UINT16 ArraySize,
		UINT16 MipLevels,
		DXGI_FORMAT Format,
		void * DataArray[],
		UINT RowPitchArray[],
		UINT SlicePitchArray[],
		UINT64 *pRenderMainOffset,
		ID3D12Resource **ppRenderMainResource
	) override;

	void EnqueueWait(
		ID3D12CommandQueue *//Zero-copy UMA Optimization
	) override;

public:
	inline PTDeviceMemoryUMAImpl(
		ID3D12Heap *pHeapAssetSRV,
		IPTSubAlloc *pSubAllocAssetSRV,
		ID3D12Heap *pHeapAssetVBVIBV,
		IPTSubAlloc *pSubAllocAssetVBVIBV,
		ID3D12Heap *pHeapFrameRTVDSVSRV,
		IPTSubAlloc *pSubAllocFrameRTVDSVSRV,
		ID3D12Heap *pHeapFrameUAVSRV,
		IPTSubAlloc *pSubAllocFrameUAVSRV,
		ID3D12Heap *pHeapFrameCBV,
		IPTSubAlloc *pSubAllocFrameCBV)
		:
		m_pHeapAssetSRV(pHeapAssetSRV),
		m_pSubAllocAssetSRV(pSubAllocAssetSRV),
		m_pHeapAssetVBVIBV(pHeapAssetVBVIBV),
		m_pSubAllocAssetVBVIBV(pSubAllocAssetVBVIBV),
		m_pHeapFrameRTVDSVSRV(pHeapFrameRTVDSVSRV),
		m_pSubAllocFrameRTVDSVSRV(pSubAllocFrameRTVDSVSRV),
		m_pHeapFrameUAVSRV(pHeapFrameUAVSRV),
		m_pSubAllocFrameUAVSRV(pSubAllocFrameUAVSRV),
		m_pHeapFrameCBV(pHeapFrameCBV),
		m_pSubAllocFrameCBV(pSubAllocFrameCBV)
	{

	}
};


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

	IPTSubAlloc *pSubAllocAssetSRV = NULL;
	IPTSubAlloc *pSubAllocAssetVBVIBV = NULL;
	IPTSubAlloc *pSubAllocFrameRTVDSVSRV = NULL;
	IPTSubAlloc *pSubAllocFrameUAVSRV = NULL;
	IPTSubAlloc *pSubAllocFrameCBV = NULL;

	pSubAllocAssetSRV = ::PTSubAlloc_Create(pHeap, SizeMaxAssetSRV, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
	pSubAllocAssetVBVIBV = ::PTSubAlloc_Create(pHeap, SizeMaxAssetVBVIBV, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
	pSubAllocFrameRTVDSVSRV = ::PTSubAlloc_Create(pHeap, SizeMaxFrameRTVDSVSRV, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
	pSubAllocFrameUAVSRV = ::PTSubAlloc_Create(pHeap, SizeMaxFrameUAVSRV, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
	pSubAllocFrameCBV = ::PTSubAlloc_Create(pHeap, SizeMaxFrameCBV, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);

	void *pMemory = pHeap->AlignedAlloc(sizeof(PTDeviceMemoryUMAImpl), alignof(PTDeviceMemoryUMAImpl));
	assert(pMemory != NULL);

	return static_cast<IPTDeviceMemory *>(
		new(pMemory)PTDeviceMemoryUMAImpl(
			pHeapAssetSRV,
			pSubAllocAssetSRV,
			pHeapAssetVBVIBV,
			pSubAllocAssetVBVIBV,
			pHeapFrameRTVDSVSRV,
			pSubAllocFrameRTVDSVSRV,
			pHeapFrameUAVSRV,
			pSubAllocFrameUAVSRV,
			pHeapFrameCBV,
			pSubAllocFrameCBV
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

	IPTSubAlloc *pSubAllocAssetSRV = NULL;
	IPTSubAlloc *pSubAllocAssetVBVIBV = NULL;
	IPTSubAlloc *pSubAllocFrameRTVDSVSRV = NULL;
	IPTSubAlloc *pSubAllocFrameUAVSRV = NULL;
	IPTSubAlloc *pSubAllocFrameCBV = NULL;

	pSubAllocAssetSRV = ::PTSubAlloc_Create(pHeap, SizeMaxAssetSRV, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
	pSubAllocAssetVBVIBV = ::PTSubAlloc_Create(pHeap, SizeMaxAssetVBVIBV, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
	pSubAllocFrameRTVDSVSRV = ::PTSubAlloc_Create(pHeap, SizeMaxFrameRTVDSVSRV, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
	pSubAllocFrameUAVSRV = ::PTSubAlloc_Create(pHeap, SizeMaxFrameUAVSRV, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
	pSubAllocFrameCBV = ::PTSubAlloc_Create(pHeap, SizeMaxFrameCBV, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);

	void *pMemory = pHeap->AlignedAlloc(sizeof(PTDeviceMemoryUMAImpl), alignof(PTDeviceMemoryUMAImpl));
	assert(pMemory != NULL);

	return static_cast<IPTDeviceMemory *>(
		new(pMemory)PTDeviceMemoryUMAImpl(
			pHeapAssetSRV,
			pSubAllocAssetSRV,
			pHeapAssetVBVIBV,
			pSubAllocAssetVBVIBV,
			pHeapFrameRTVDSVSRV,
			pSubAllocFrameRTVDSVSRV,
			pHeapFrameUAVSRV,
			pSubAllocFrameUAVSRV,
			pHeapFrameCBV,
			pSubAllocFrameCBV
		)
		);
}

//d3dx12.h
inline UINT D3D12CalcSubresource(UINT MipSlice, UINT ArraySlice, UINT PlaneSlice, UINT MipLevels, UINT ArraySize)
{
	return MipSlice + ArraySlice * MipLevels + PlaneSlice * MipLevels * ArraySize;
}

void PTDeviceMemoryUMAImpl::EnqueueAssetSRVTexture2DCreate(
	ID3D12Device *pRenderMainDevice,
	ID3D12CommandQueue *,//Zero-copy UMA Optimization
	IPTHeap *pStreamMainHeap,
	UINT64 Width,
	UINT Height,
	UINT16 ArraySize,
	UINT16 MipLevels,
	DXGI_FORMAT Format,
	void * SrcDataArray[],
	UINT SrcRowPitchArray[],
	UINT SlicePitchArray[],
	UINT64 *pRenderMainOffset,
	ID3D12Resource **ppRenderMainResource
)
{
	HRESULT Result;

	D3D12_RESOURCE_DESC Desc;
	Desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	Desc.Alignment = 0U;
	Desc.Width = Width;
	Desc.Height = Height;
	Desc.DepthOrArraySize = ArraySize;
	Desc.MipLevels = MipLevels;
	Desc.Format = Format;
	Desc.SampleDesc.Count = 1U;
	Desc.SampleDesc.Quality = 0U;
	Desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	Desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	D3D12_RESOURCE_ALLOCATION_INFO AllocInfo = pRenderMainDevice->GetResourceAllocationInfo(0X1U, 1U, &Desc);
	Desc.Alignment = AllocInfo.Alignment;

	UINT64 RenderMainOffset = m_pSubAllocAssetSRV->AlignedAlloc(pStreamMainHeap, AllocInfo.SizeInBytes, AllocInfo.Alignment);

	ID3D12Resource *pRenderMainResource;
	Result = pRenderMainDevice->CreatePlacedResource(m_pHeapAssetSRV, RenderMainOffset, &Desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, NULL, IID_PPV_ARGS(&pRenderMainResource));
	assert(SUCCEEDED(Result));

	//Zero-copy UMA Optimization
	UINT NumSubresource = ArraySize*MipLevels;//Plane个数为1
	D3D12_RANGE ReadRangeZero = { 0U,0U };
	for (UINT IndexSubresource = 0U; IndexSubresource < NumSubresource; ++IndexSubresource)
	{
		pRenderMainResource->Map(IndexSubresource, &ReadRangeZero, NULL);
		pRenderMainResource->WriteToSubresource(IndexSubresource, NULL, SrcDataArray[IndexSubresource], SrcRowPitchArray[IndexSubresource], SlicePitchArray[IndexSubresource]);
		pRenderMainResource->Unmap(IndexSubresource, NULL);//与相干性无关，与VkSemaphore有关
	}

	(*pRenderMainOffset) = RenderMainOffset;
	(*ppRenderMainResource) = pRenderMainResource;
}

void PTDeviceMemoryUMAImpl::EnqueueWait(
	ID3D12CommandQueue *//Zero-copy UMA Optimization
)
{
	//Zero-copy UMA Optimization
	return;
}

