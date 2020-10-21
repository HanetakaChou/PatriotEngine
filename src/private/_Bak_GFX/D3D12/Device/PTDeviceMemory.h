#include "../../../../Public/McRT/PTMemorySystem.h"

#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <d3d12.h>

struct IPTDeviceMemory
{
	virtual void Destory(IPTHeap *pHeap) = 0;

	virtual void EnqueueAssetSRVTexture2DCreate(
		ID3D12Device *pRenderMainDevice,
		ID3D12CommandQueue *pRenderMainCommandQueueCopy,
		IPTHeap *pStreamMainHeap,
		UINT64 Width,
		UINT Height,
		UINT16 ArraySize,
		UINT16 MipLevels,
		DXGI_FORMAT Format,
		const void * DataArray[],
		UINT RowPitchArray[],
		UINT SlicePitchArray[],
		UINT64 *pRenderMainOffset,
		ID3D12Resource **ppRenderMainResource
	) = 0;

	//http://msdn.microsoft.com/en-us/library/windows/desktop/dn899226
	//http://msdn.microsoft.com/en-us/library/windows/desktop/dn899217


	//����������
	//COMMON������COPY_DEST
	//COPY_DEST˥�˵�COMMON

	//����/3D������
	//COMMON˥�˵�NON_PIXEL_SHADER_RESOURCE|PIXEL_SHADER_RESOURCE

	//��Ҫͬ�����ȴ�ExecuteCommandLists���

	virtual void EnqueueWait(ID3D12CommandQueue *pRenderMainCommandQueueCopy) = 0;
};

struct IPTDeviceResource
{
	virtual void Destory(IPTHeap *pHeap) = 0;

	virtual ID3D12Resource *Get() = 0;
};

IPTDeviceMemory *PTDeviceMemoryNUMANCC_Create(
	ID3D12Device *pDevice,
	IPTHeap *pHeap,
	uint64_t SizeMaxAssetSRV,
	uint64_t SizeMaxAssetVBVIBV,
	uint64_t SizeMaxFrameRTVDSVSRV,
	uint64_t SizeMaxFrameUAVSRV,
	uint64_t SizeMaxFrameCBV,
	uint64_t SizeMaxStreamCPSC);

IPTDeviceMemory *PTDeviceMemoryUMANCC_Create(
	ID3D12Device *pDevice,
	IPTHeap *pHeap,
	uint64_t SizeMaxAssetSRV,
	uint64_t SizeMaxAssetVBVIBV,
	uint64_t SizeMaxFrameRTVDSVSRV,
	uint64_t SizeMaxFrameUAVSRV,
	uint64_t SizeMaxFrameCBV);


IPTDeviceMemory *PTDeviceMemoryUMACC_Create(
	ID3D12Device *pDevice,
	IPTHeap *pHeap,
	uint64_t SizeMaxAssetSRV,
	uint64_t SizeMaxAssetVBVIBV,
	uint64_t SizeMaxFrameRTVDSVSRV,
	uint64_t SizeMaxFrameUAVSRV,
	uint64_t SizeMaxFrameCBV);