#ifndef PT_VIDEO_VULKAN_HEAP_H
#define PT_VIDEO_VULKAN_HEAP_H

#include "PTVDVulkanHeader.h"

#include "PTVDDevice.h"

#include <assert.h>

// @see Device::InitMemoryHeapProperties PAL(https://github.com/GPUOpen-Drivers/pal)
// heapProperties with cpuWriteCombined must be cpuUncached

// 


struct IPTVDHeapManager_NUMA
{
	//Memory Management Strategies
	//https://msdn.microsoft.com/en-us/library/windows/desktop/mt613239(v=vs.85).aspx#classification_strategy

	//virtual VkDeviceMemory HeapBufferUploadAllocate() = 0;
	//virtual VkDeviceMemory HeapBufferReadBackAllocate() = 0;
	//virtual VkDeviceMemory HeapBufferDeviceAllocate() = 0;

	//We Use WriteCombine in NonCacheCoherent
	//So We Need Not Flush Host Cache While Uploading
	virtual void BufferReadback_HostCacheInvalidate(VkDeviceWrapper &rhDeviceWrapper, VkDeviceMemory hMemory, uint64_t uiOffset, uint64_t uiSize) = 0; //Map
};

class PTVDHeapManager_NUMA_Base :public IPTVDHeapManager_NUMA
{
protected:
	inline PTVDHeapManager_NUMA_Base()
	{

	}
};

class PTVDHeapManager_NUMA_CC : public PTVDHeapManager_NUMA_Base
{
	uint32_t m_uiHeapIndex_System_Buffer;
	uint32_t m_uiHeapIndex_Video_Buffer;
	uint32_t m_uiHeapIndex_Video_Texture_Immutable_SRV;
	uint32_t m_uiHeapIndex_Video_Texture_Mutable_UAV;
	uint32_t m_uiHeapIndex_Video_Texture_Mutable_RTVDSV;

public:
	inline PTVDHeapManager_NUMA_CC(
		uint32_t uiHeapIndex_System_Buffer,
		uint32_t uiHeapIndex_Video_Buffer,
		uint32_t uiHeapIndex_Video_Texture_Immutable_SRV,
		uint32_t uiHeapIndex_Video_Texture_Mutable_UAV,
		uint32_t uiHeapIndex_Video_Texture_Mutable_RTVDSV
	)
		:
		m_uiHeapIndex_System_Buffer(uiHeapIndex_System_Buffer),
		m_uiHeapIndex_Video_Buffer(uiHeapIndex_Video_Buffer),
		m_uiHeapIndex_Video_Texture_Immutable_SRV(uiHeapIndex_Video_Texture_Immutable_SRV),
		m_uiHeapIndex_Video_Texture_Mutable_UAV(uiHeapIndex_Video_Texture_Mutable_UAV),
		m_uiHeapIndex_Video_Texture_Mutable_RTVDSV(uiHeapIndex_Video_Texture_Mutable_RTVDSV)
	{

	}

	void BufferReadback_HostCacheInvalidate(VkDeviceWrapper &rhDeviceWrapper, VkDeviceMemory hMemory, uint64_t uiOffset, uint64_t uiSize) override;
};

class PTVDHeapManager_NUMA_NCC : public PTVDHeapManager_NUMA_Base
{
	uint32_t m_uiHeapIndex_SystemUpload_Buffer;
	uint32_t m_uiHeapIndex_SystemReadback_Buffer;
	uint32_t m_uiHeapIndex_Video_Buffer;
	uint32_t m_uiHeapIndex_Video_Texture_Immutable_SRV;
	uint32_t m_uiHeapIndex_Video_Texture_Mutable_UAV;
	uint32_t m_uiHeapIndex_Video_Texture_Mutable_RTVDSV;

public:
	inline PTVDHeapManager_NUMA_NCC(
		uint32_t uiHeapIndex_SystemUpload_Buffer,
		uint32_t uiHeapIndex_SystemReadback_Buffer,
		uint32_t uiHeapIndex_Video_Buffer,
		uint32_t uiHeapIndex_Video_Texture_Immutable_SRV,
		uint32_t uiHeapIndex_Video_Texture_Mutable_UAV,
		uint32_t uiHeapIndex_Video_Texture_Mutable_RTVDSV
	) :
		m_uiHeapIndex_SystemUpload_Buffer(uiHeapIndex_SystemUpload_Buffer),
		m_uiHeapIndex_SystemReadback_Buffer(uiHeapIndex_SystemReadback_Buffer),
		m_uiHeapIndex_Video_Buffer(uiHeapIndex_Video_Buffer),
		m_uiHeapIndex_Video_Texture_Immutable_SRV(uiHeapIndex_Video_Texture_Immutable_SRV),
		m_uiHeapIndex_Video_Texture_Mutable_UAV(uiHeapIndex_Video_Texture_Mutable_UAV),
		m_uiHeapIndex_Video_Texture_Mutable_RTVDSV(uiHeapIndex_Video_Texture_Mutable_RTVDSV)
	{

	}

	void BufferReadback_HostCacheInvalidate(VkDeviceWrapper &rhDeviceWrapper, VkDeviceMemory hMemory, uint64_t uiOffset, uint64_t uiSize) override;
};

struct IPTVDHeapManager_UMA
{
	//Memory Management Strategies
	//https://msdn.microsoft.com/en-us/library/windows/desktop/mt613239(v=vs.85).aspx#classification_strategy

	//virtual VkDeviceMemory HeapBufferUploadAllocate() = 0;
	//virtual VkDeviceMemory HeapBufferReadBackAllocate() = 0;
	
	//在Direct3D12中
	//可以用ID3D12Resource::WriteToSubresource对UMA显卡Zero-Copy优化
	//https://msdn.microsoft.com/en-us/library/windows/desktop/dn914416(v=vs.85).aspx
	//-------------------------------------------------------------------
	//在Vulkan中
	//vkGetImageSubresourceLayout只适用于VK_IMAGE_TILING_LINEAR
	//-------------------------------------------------------------------
	//Vulkan-Docs
	//Synchronization Examples
	//https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples
	//Upload data from the CPU to an image sampled in a fragment shader
	//This path is universal to both UMA and discrete systems, as images should be converted to optimal tiling on upload.

	//virtual VkDeviceMemory HeapTextureReUsedAllocate() = 0;

	//We Use WriteCombine in NonCacheCoherent
	//So We Need Not Flush Host Cache While Uploading
	virtual void BufferReadback_HostCacheInvalidate(VkDeviceWrapper &rhDeviceWrapper, VkDeviceMemory hMemory, uint64_t uiOffset, uint64_t uiSize) = 0; //Map
};

class PTVDHeapManager_UMA_Base :public IPTVDHeapManager_UMA
{
protected:
	inline PTVDHeapManager_UMA_Base()
	{

	}
};

class PTVDHeapManager_UMA_CC : public PTVDHeapManager_UMA_Base
{
	uint32_t m_uiHeapIndex_System_Buffer;
	uint32_t m_uiHeapIndex_SystemOptimal_Texture_Immutable_SRV;
	uint32_t m_uiHeapIndex_SystemOptimal_Texture_Mutable_UAV;
	uint32_t m_uiHeapIndex_SystemOptimal_Texture_Mutable_RTVDSV;

public:
	inline PTVDHeapManager_UMA_CC(
		uint32_t uiHeapIndex_System_Buffer,
		uint32_t uiHeapIndex_SystemOptimal_Texture_Immutable_SRV,
		uint32_t uiHeapIndex_SystemOptimal_Texture_Mutable_UAV,
		uint32_t uiHeapIndex_SystemOptimal_Texture_Mutable_RTVDSV
	) 
		:
		m_uiHeapIndex_System_Buffer(uiHeapIndex_System_Buffer),
		m_uiHeapIndex_SystemOptimal_Texture_Immutable_SRV(uiHeapIndex_SystemOptimal_Texture_Immutable_SRV),
		m_uiHeapIndex_SystemOptimal_Texture_Mutable_UAV(uiHeapIndex_SystemOptimal_Texture_Mutable_UAV),
		m_uiHeapIndex_SystemOptimal_Texture_Mutable_RTVDSV(uiHeapIndex_SystemOptimal_Texture_Mutable_RTVDSV)
	{

	}

	void BufferReadback_HostCacheInvalidate(VkDeviceWrapper &rhDeviceWrapper, VkDeviceMemory hMemory, uint64_t uiOffset, uint64_t uiSize) override;
};

class PTVDHeapManager_UMA_NCC : public PTVDHeapManager_UMA_Base
{
	uint32_t m_uiHeapIndex_SystemUpload_Buffer;
	uint32_t m_uiHeapIndex_SystemReadback_Buffer;
	uint32_t m_uiHeapIndex_SystemOptimal_Texture_Immutable_SRV;
	uint32_t m_uiHeapIndex_SystemOptimal_Texture_Mutable_UAV;
	uint32_t m_uiHeapIndex_SystemOptimal_Texture_Mutable_RTVDSV;

public:
	inline PTVDHeapManager_UMA_NCC(
		uint32_t uiHeapIndex_SystemUpload_Buffer,
		uint32_t uiHeapIndex_SystemReadback_Buffer,
		uint32_t uiHeapIndex_SystemOptimal_Texture_Immutable_SRV,
		uint32_t uiHeapIndex_SystemOptimal_Texture_Mutable_UAV,
		uint32_t uiHeapIndex_SystemOptimal_Texture_Mutable_RTVDSV
	) :
		m_uiHeapIndex_SystemUpload_Buffer(uiHeapIndex_SystemUpload_Buffer),
		m_uiHeapIndex_SystemReadback_Buffer(uiHeapIndex_SystemReadback_Buffer),
		m_uiHeapIndex_SystemOptimal_Texture_Immutable_SRV(uiHeapIndex_SystemOptimal_Texture_Immutable_SRV),
		m_uiHeapIndex_SystemOptimal_Texture_Mutable_UAV(uiHeapIndex_SystemOptimal_Texture_Mutable_UAV),
		m_uiHeapIndex_SystemOptimal_Texture_Mutable_RTVDSV(uiHeapIndex_SystemOptimal_Texture_Mutable_RTVDSV)
	{

	}

	void BufferReadback_HostCacheInvalidate(VkDeviceWrapper &rhDeviceWrapper, VkDeviceMemory hMemory, uint64_t uiOffset, uint64_t uiSize) override;
};

#endif