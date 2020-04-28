#ifndef PT_VIDEO_VULKAN_MEMORYALLOCATOR_H
#define PT_VIDEO_VULKAN_MEMORYALLOCATOR_H

#include "../PTVDMemoryAllocator.h"
#include "PTVDVulkanHeader.h"

class PTVD_DeviceMemoryManager
{
	uint32_t m_MemoryHeapIndexVector[PTVD_MEMORYTYPE_COUNT];
	uint32_t m_MemoryTypeIndexVector[PTVD_MEMORYTYPE_COUNT];
	VkDeviceSize m_MemoryHeapSizeRemainVector[3U]; //目前，没有超过3个堆的GPU存在 //https://vulkan.gpuinfo.org

public:
	bool Construct(VkInstanceWrapper &rInstanceWrapper, VkDeviceWrapper &rDeviceWrapper, VkPhysicalDevice hPhysicalDevice, bool HostVector[PTVD_MEMORYTYPE_COUNT]/* We Only Support Coherent Host*/, VkFormat *pTextureDS_SR_Format, VkFormat *pTextureDS_Format);

	bool Alloc(VkDeviceWrapper &rDeviceWrapper, VkDeviceMemory *phMemory, VkDeviceSize *pBlockSize, void **ppBlockAddress, uint32_t MemoryType_PTVD);

	bool Free(VkDeviceWrapper &rDeviceWrapper, VkDeviceMemory hMemory, bool bUnmap);
};

#endif