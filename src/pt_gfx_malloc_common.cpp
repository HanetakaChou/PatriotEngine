
#include "pt_gfx_malloc_common.h"

// linux
// https://github.com/torvalds/linux/blob/master/mm/slab.c //CONFIG_SLAB 
// https://github.com/torvalds/linux/blob/master/mm/slub.c //CONFIG_SLUB   

// slab 
// [Bonwick 1994] Jeff Bonwick. "The Slab Allocator: An Object-Caching Kernel Memory Allocator." USENIX 1994    

// tbbmalloc 
// [Hudson 2006] Richard L. Hudson, Bratin Saha, Ali-Reza Adl-Tabatabai, Benjamin C. Hertzberg. "McRT-Malloc: a scalable transactional memory allocator". ISMM 2006.   

// __kmalloc //mm/slub.c
//  __do_kmalloc
//    kmalloc_slab -> kmem_cache // size_index_elem
//    slab_alloc -> /* slab_object */
//      slab_alloc_node
//      ____cache_alloc
//       get_valid_first_slab

// linux::kmem_cache <-> slab::cache <-> tbbmalloc::bin 
// linux::kmem_cache_node //get_valid_first_slab
// include/linux/slab_def.h
// include/linux/slub_def.h
// # mm/slab.h //for slob

// linux::page <-> slab::slab <-> tbbmalloc::block
// include/linux/mm_types.h

// kmem_cache_shrink
// Defragmentation by sorting partial list
// Slab allocators in the Linux Kernel: SLAB, SLOB, SLUB

// A Heap of Trouble: Breaking the Linux Kernel SLOB Allocator

// VK_EXT_memory_budget
// VmaAllocator_T::GetBudget
//  check m_OperationsSinceBudgetFetch
// we are not alone ?

// github
// vulkaninfo "usable for"  
//

/*
The implementation guarantees certain properties about the memory requirements returned by vkGetBufferMemoryRequirements2, vkGetImageMemoryRequirements2, vkGetBufferMemoryRequirements and vkGetImageMemoryRequirements:

The memoryTypeBits member always contains at least one bit set.

If buffer is a VkBuffer not created with the VK_BUFFER_CREATE_SPARSE_BINDING_BIT bit set, or if image is linear image, then the memoryTypeBits member always contains at least one bit set corresponding to a VkMemoryType with a propertyFlags that has both the VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT bit and the VK_MEMORY_PROPERTY_HOST_COHERENT_BIT bit set. In other words, mappable coherent memory can always be attached to these objects.

If buffer was created with VkExternalMemoryBufferCreateInfo::handleTypes set to 0 or image was created with VkExternalMemoryImageCreateInfo::handleTypes set to 0, the memoryTypeBits member always contains at least one bit set corresponding to a VkMemoryType with a propertyFlags that has the VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT bit set.

The memoryTypeBits member is identical for all VkBuffer objects created with the same value for the flags and usage members in the VkBufferCreateInfo structure and the handleTypes member of the VkExternalMemoryBufferCreateInfo structure passed to vkCreateBuffer. Further, if usage1 and usage2 of type VkBufferUsageFlags are such that the bits set in usage2 are a subset of the bits set in usage1, and they have the same flags and VkExternalMemoryBufferCreateInfo::handleTypes, then the bits set in memoryTypeBits returned for usage1 must be a subset of the bits set in memoryTypeBits returned for usage2, for all values of flags.

The alignment member is a power of two.

The alignment member is identical for all VkBuffer objects created with the same combination of values for the usage and flags members in the VkBufferCreateInfo structure passed to vkCreateBuffer.

The alignment member satisfies the buffer descriptor offset alignment requirements associated with the VkBuffer’s usage:

If usage included VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT or VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT, alignment must be an integer multiple of VkPhysicalDeviceLimits::minTexelBufferOffsetAlignment.

If usage included VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, alignment must be an integer multiple of VkPhysicalDeviceLimits::minUniformBufferOffsetAlignment.

If usage included VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, alignment must be an integer multiple of VkPhysicalDeviceLimits::minStorageBufferOffsetAlignment.

For images created with a color format, the memoryTypeBits member is identical for all VkImage objects created with the same combination of values for the tiling member, the VK_IMAGE_CREATE_SPARSE_BINDING_BIT bit of the flags member, the VK_IMAGE_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT bit of the flags member, handleTypes member of VkExternalMemoryImageCreateInfo, and the VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT of the usage member in the VkImageCreateInfo structure passed to vkCreateImage.

For images created with a depth/stencil format, the memoryTypeBits member is identical for all VkImage objects created with the same combination of values for the format member, the tiling member, the VK_IMAGE_CREATE_SPARSE_BINDING_BIT bit of the flags member, the VK_IMAGE_CREATE_SPLIT_INSTANCE_BIND_REGIONS_BIT bit of the flags member, handleTypes member of VkExternalMemoryImageCreateInfo, and the VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT of the usage member in the VkImageCreateInfo structure passed to vkCreateImage.

If the memory requirements are for a VkImage, the memoryTypeBits member must not refer to a VkMemoryType with a propertyFlags that has the VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT bit set if the image did not have VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT bit set in the usage member of the VkImageCreateInfo structure passed to vkCreateImage.

If the memory requirements are for a VkBuffer, the memoryTypeBits member must not refer to a VkMemoryType with a propertyFlags that has the VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT bit set.

Note
The implication of this requirement is that lazily allocated memory is disallowed for buffers in all cases.

The size member is identical for all VkBuffer objects created with the same combination of creation parameters specified in VkBufferCreateInfo and its pNext chain.

The size member is identical for all VkImage objects created with the same combination of creation parameters specified in VkImageCreateInfo and its pNext chain.

Note
This, however, does not imply that they interpret the contents of the bound memory identically with each other. That additional guarantee, however, can be explicitly requested using VK_IMAGE_CREATE_ALIAS_BIT.
*/

// spec : VkPhysicalDeviceMemoryProperties
// At least one heap must include VK_MEMORY_HEAP_DEVICE_LOCAL_BIT in VkMemoryHeap::flags.
// There must be at least one memory type with both the VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT and VK_MEMORY_PROPERTY_HOST_COHERENT_BIT bits set in its propertyFlags. //for upload purpose
// lower index implies greater performance or subset // prefer lower index

// vertex buffer
// index buffer
// sample image
// required = VK_MEMORY_HEAP_DEVICE_LOCAL_BIT // the UMA driver may compress the buffer/texture to boost performance
//

// uniform buffer
// required = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
// preferred = VK_MEMORY_HEAP_DEVICE_LOCAL_BIT // AMD "Special pool of video memory" [Sawicki 2018] Adam Sawicki. "Memory Management in Vulkan and DX12." GDC 2018.

// VmaAllocator_T::VmaAllocator_T
//  vma_new VmaBlockVector //maxBlockCount->SIZE_MAX

// vmaCreateBuffer //pass allocationcount //alloc multi buffer once 
// VmaAllocator_T::AllocateMemory
//  vmaFindMemoryTypeIndex // usage->requiredFlags/preferredFlags->memorytypeindex //count cost // vmaFindMemoryTypeIndexForBufferInfo/vmaFindMemoryTypeIndexForImageInfo not used by the library
//  VmaAllocator_T::GetMemoryTypeMinAlignment //we don't need non coherent 
//  VmaAllocator_T::AllocateMemoryOfType  
//   memorytypeindex-> VmaBlockVector  
//   VmaBlockVector::Allocate                    
//    corruption detection ?
//    VmaBlockVector::AllocatePage
//     VmaAllocator_T::GetBudget->freeMemory(size)->canCreateNewBlock
//     early reject //larger than maximum block size
//     // can become lost //can make other lost // set by user?
//     // m_Algorithm + VmaAllocationCreateInfo.flags & STRATEGY_MASK -> the algorithm
//     1. Search existing allocations. Try to allocate without making other allocations lost.
//     VmaBlockVector::AllocateFromBlock //best fit 
//     2. Try to create new block.
//     VmaBlockVector::CalcMaxBlockSize // max existing block
//     newBlockSize = 1/8 1/4 1/2 m_PreferredBlockSize
//     VmaBlockVector::CreateBlock 

// slob

// https://github.com/ValveSoftware/dxvk
// DxvkDevice::createImage          // src/dxvk/dxvk_device.cpp
//  DxvkImage::DxvkImage            // src/dxvk/dxvk_image.cpp
//   DxvkMemoryAllocator::alloc     // src/dxvk/dxvk_memory.cpp
// 