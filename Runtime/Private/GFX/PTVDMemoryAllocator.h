#ifndef PT_VIDEO_MEMORYALLOCATOR_H
#define PT_VIDEO_MEMORYALLOCATOR_H

#include <stdint.h>

enum : uint32_t
{

	PTVD_MEMORYTYPE_BUFFER_STAGING,
	PTVD_MEMORYTYPE_BUFFER_CONSTANT,
	PTVD_MEMORYTYPE_BUFFER_UNORDEREDACCESS_SHADERRESOURCE,
	PTVD_MEMORYTYPE_BUFFER_VERTEX,
	PTVD_MEMORYTYPE_BUFFER_INDEX,
	//PTVD_MEMORYTYPE_BUFFER_CONSTANT_IMMUTABLE,              //For Static Objects In The Scene
	PTVD_MEMORYTYPE_TEXTURE_RENDERTARGET_SHADERRESOURCE,    //Transient
	//PTVD_MEMORYTYPE_TEXTURE_RENDERTARGET,                 //No Need This Type. We Can Write To The SwapChain Directly
	PTVD_MEMORYTYPE_TEXTURE_DEPTHSTENCIL_SHADERRESOURCE,    //Transient
	PTVD_MEMORYTYPE_TEXTURE_DEPTHSTENCIL,				    //Transient
	PTVD_MEMORYTYPE_TEXTURE_UNORDEREDACCESS_SHADERRESOURCE, //No Need Transient. We Can't Use ComputeShader In RenderPass.
	PTVD_MEMORYTYPE_TEXTURE_SHADERRESOURCE,
	PTVD_MEMORYTYPE_COUNT
};

struct PTVD_MemoryObject;
class PTVD_MemoryAllocator;

#if defined(PTVDVK)
#include "VK/PTVDVulkanHeader.h"
#elif defined(PTVDD3D12)

#else
#error δ֪��ͼ��API
#endif

#if defined(PTVDVK)
PTVD_MemoryAllocator *PTVD_MemoryAllocator_Create(VkInstanceWrapper &rInstanceWrapper, VkDeviceWrapper &rDeviceWrapper);
#elif defined(PTVDD3D12)

#else
#error δ֪��ͼ��API
#endif

PTVD_MemoryObject *PTVD_MemoryAllocator_Alloc(PTVD_MemoryAllocator *pMemoryAllocator_PTVD, uint32_t MemoryType, uint32_t Size);

#if defined(PTVDVK)
VkDeviceMemory PTVD_MemoryObject_Memory(PTVD_MemoryObject *pMemoryObject);
VkDeviceSize PTVD_MemoryObject_MemoryOffset(PTVD_MemoryObject *pMemoryObject);
#elif defined(PTVDD3D12)

#else
#error δ֪��ͼ��API
#endif


#endif


