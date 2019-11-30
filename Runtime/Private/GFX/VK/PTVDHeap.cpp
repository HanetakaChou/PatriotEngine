#include "PTVDHeap.h"
#include <assert.h>

void PTVDHeapManager_NUMA_CC::BufferReadback_HostCacheInvalidate(VkDeviceWrapper &rhDeviceWrapper, VkDeviceMemory hMemory, uint64_t uiOffset, uint64_t uiSize)
{

}

void PTVDHeapManager_NUMA_NCC::BufferReadback_HostCacheInvalidate(VkDeviceWrapper &rhDeviceWrapper, VkDeviceMemory hMemory, uint64_t uiOffset, uint64_t uiSize)
{
	VkMappedMemoryRange MemoryRange[1];
	MemoryRange[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	MemoryRange[0].pNext = NULL;
	MemoryRange[0].memory = hMemory;
	MemoryRange[0].offset = uiOffset;
	MemoryRange[0].size = uiSize;

	VkResult eResult = rhDeviceWrapper.InvalidateMappedMemoryRanges(1U, MemoryRange);
	assert(eResult == VK_SUCCESS);
}

void PTVDHeapManager_UMA_CC::BufferReadback_HostCacheInvalidate(VkDeviceWrapper &rhDeviceWrapper, VkDeviceMemory hMemory, uint64_t uiOffset, uint64_t uiSize)
{

}

void PTVDHeapManager_UMA_NCC::BufferReadback_HostCacheInvalidate(VkDeviceWrapper &rhDeviceWrapper, VkDeviceMemory hMemory, uint64_t uiOffset, uint64_t uiSize)
{
	VkMappedMemoryRange MemoryRange[1];
	MemoryRange[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	MemoryRange[0].pNext = NULL;
	MemoryRange[0].memory = hMemory;
	MemoryRange[0].offset = uiOffset;
	MemoryRange[0].size = uiSize;
	
	VkResult eResult = rhDeviceWrapper.InvalidateMappedMemoryRanges(1U, MemoryRange);
	assert(eResult == VK_SUCCESS);
}
