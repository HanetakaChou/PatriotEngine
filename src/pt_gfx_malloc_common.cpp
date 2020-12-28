#include <pt_mcrt_intrin.h>
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
// VmaAllocator_T::AllocateVulkanMemory
//  ++m_OperationsSinceBudgetFetch

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

// VmaAllocation_T/VmaSuballocation <-> slob_block/slob_t
// VmaBlock/VmaBlockMetadata_Generic <-> slob_page

// VmaBlockVector::AllocateFromBlock/VmaBlockMetadata_Generic::CreateAllocationRequest
// m_SumFreeSize -> early return
// slob_alloc /* check sp->units */ Note that the reported space in a SLOB page is not necessarily contiguous, so the allocation is not guaranteed to succeed.
// slob_page_alloc

// SLOB
// kernel 3.5
// https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/mm/slob.c?h=v3.5&id=28a33cbc24e4256c143dce96c7d93bf423229f92
// [Rosenberg 2012] Dan Rosenberg. "A Heap of Trouble: Breaking the Linux Kernel SLOB Allocator." Virtual Security Research 2012.

// slab <-> slob_page
// buf <-> slob_t <-> slob_block

// Note that SLOB pages contain blocks of varying sizes, which differentiates SLOB from a classic slab allocator.

// https://www.kernel.org/doc/gorman/html/understand/understand011.html

uint64_t const gfx_malloc_common::MALLOC_OFFSET_INVALID = (~0ULL);
uint64_t const gfx_malloc_common::SLOB_OFFSET_INVALID = gfx_malloc_common::MALLOC_OFFSET_INVALID;

// NULL-Pointer Assignment Partition
// https://docs.microsoft.com/en-us/windows-hardware/drivers/gettingstarted/virtual-address-spaces
// https://www.kernel.org/doc/Documentation/arm/memory.txt
// https://www.kernel.org/doc/Documentation/arm64/memory.txt
// https://www.kernel.org/doc/Documentation/x86/x86_64/mm.txt
class gfx_malloc_common::slob_page_t *const gfx_malloc_common::slob_page_t::LIST_NEXT_INVALID = reinterpret_cast<class gfx_malloc_common::slob_page_t *>(1);
class gfx_malloc_common::slob_page_t *const gfx_malloc_common::slob_page_t::LIST_PREV_INVALID = reinterpret_cast<class gfx_malloc_common::slob_page_t *>(2);

uint64_t gfx_malloc_common::slob_alloc(
    uint64_t slob_break1,
    uint64_t slob_break2,
    class slob_page_t *list_head_free_slob_small,
    class slob_page_t *list_head_free_slob_medium,
    class slob_page_t *list_head_free_slob_large,
    uint64_t size,
    uint64_t align,
    class slob_page_t const **out_slob_page,
    void (*slob_lock_list_head_callback)(class gfx_malloc_common *self),
    void (*slob_unlock_list_head_callback)(class gfx_malloc_common *self),
    class slob_page_t *(*slob_new_pages_callback)(class gfx_malloc_common *self),
    class gfx_malloc_common *self)
{
    class slob_page_t *slob_list = NULL;
    if (size < slob_break1)
    {
        slob_list = list_head_free_slob_small;
    }
    else if (size < slob_break2)
    {
        slob_list = list_head_free_slob_medium;
    }
    else
    {
        slob_list = list_head_free_slob_large;
    }

    class slob_page_t *sp;
    uint64_t b = SLOB_OFFSET_INVALID;

    slob_lock_list_head_callback(self);
    for (sp = slob_page_t::list_begin(slob_list); sp != slob_page_t::list_end(slob_list); sp = slob_page_t::list_iterator_next(sp))
    {
        if (sp->size() < size)
        {
            //PT_LIKELY
            //Early reject
            //Early return
        }
        else
        {
            // Note that the reported space in a SLOB page is not necessarily
            // contiguous, so the allocation is not guaranteed to succeed.
            b = sp->alloc(size, align);

            if (SLOB_OFFSET_INVALID == b)
            {
                //PT_LIKELY
            }
            else
            {
                slob_page_t::list_erase(sp);
                slob_page_t::list_push_back(slob_list, sp);
                break;
            }
        }
    }
    slob_unlock_list_head_callback(self);

    if (SLOB_OFFSET_INVALID != b)
    {
        //PT_LIKELY
    }
    else
    {
        //The "slob_new_pages_callback" is MT-safe
        sp = slob_new_pages_callback(self);

        if (NULL != sp)
        {
            slob_lock_list_head_callback(self);
            slob_page_t::list_push_front(slob_list, sp);
            b = sp->alloc(size, align);
            slob_unlock_list_head_callback(self);

            if (SLOB_OFFSET_INVALID != b)
            {
                //PT_LIKELY
            }
            else
            {
                //MALLOC BUG
                assert(false);
                assert(SLOB_OFFSET_INVALID == b);
            }
        }
        else
        {
            //MALLOC FAIL
            assert(SLOB_OFFSET_INVALID == b);
            assert(NULL == sp);
        }
    }

    assert(SLOB_OFFSET_INVALID == b || NULL != sp); //SLOB_OFFSET_INVALID != b ⇒ NULL != sp
    assert(SLOB_OFFSET_INVALID != b || NULL == sp); //SLOB_OFFSET_INVALID == b ⇒ NULL == sp
    (*out_slob_page) = sp;

    return b;
}

inline uint64_t gfx_malloc_common::slob_page_t::size()
{
    return m_units;
}

inline uint64_t gfx_malloc_common::slob_page_t::alloc(uint64_t size, uint64_t align)
{
    for (auto cur = m_free.begin(); cur != m_free.end(); cur = std::next(cur))
    {
        uint64_t avail = cur->size;
        uint64_t aligned = mcrt_intrin_round_up(cur->offset, align);
        uint64_t delta = aligned - cur->offset;
    }

    return SLOB_OFFSET_INVALID;
}

// VmaAllocator_T::VmaAllocator_T
//  VmaAllocator_T::CalcPreferredBlockSize //VMA_SMALL_HEAP_MAX_SIZE 1024MB block 1024 / 8 = 128MB //VMA_DEFAULT_LARGE_HEAP_BLOCK_SIZE 256MB
//  vma_new VmaBlockVector //maxBlockCount->SIZE_MAX

// vmaCreateBuffer //pass allocationcount //alloc multi buffer once
// VmaAllocator_T::AllocateMemory
//  vmaFindMemoryTypeIndex // usage->requiredFlags/preferredFlags->memorytypeindex //count cost // vmaFindMemoryTypeIndexForBufferInfo/vmaFindMemoryTypeIndexForImageInfo not used by the library
//  VmaAllocator_T::GetMemoryTypeMinAlignment //we don't need non coherent
//  VmaAllocator_T::AllocateMemoryOfType
//   memorytypeindex-> VmaBlockVector
//   VmaBlockVector::Allocate
//    corruption detection ?
//    VmaMutexLockWrite lock //we may asset?
//    VmaBlockVector::AllocatePage
//     VmaAllocator_T::GetBudget->freeMemory(size)->canCreateNewBlock
//     early reject //larger than maximum block size
//     // can become lost //can make other lost // set by user?
//     // m_Algorithm + VmaAllocationCreateInfo.flags & STRATEGY_MASK -> the algorithm
//     1. Search existing allocations. Try to allocate without making other allocations lost.
//     VmaBlockVector::AllocateFromBlock //best fit
//     2. Try to create new block.
//     VmaBlockVector::CalcMaxBlockSize // max existing block
//     newBlockSize =  1/8 1/4 1/2 m_PreferredBlockSize //if fail try 1/2 1/4 1/8
//     VmaBlockVector::CreateBlock
//      VmaAllocator_T::AllocateVulkanMemory //estimate budget (atomic)
//      VmaDeviceMemoryBlock::Init //VmaDeviceMemoryBlock.m_hMemory -> VkDeviceMemory // VmaDeviceMemoryBlock::Map 在block的层级控制map //refcount
//       m_Algorithm(0)->VmaBlockMetadata_Generic //Linear/Buddy //Block 1to1 BlockMetadata //maybe Block is more public while BlockMetadata is more internal
//       whole_size -> VmaSuballocation (offset size) //VMA_SUBALLOCATION_TYPE: free(not in used) / buffer / image -> VmaIsBufferImageGranularityConflict
//       <m_Suballocations.index> m_FreeSuballocationsBySize //sort by size
//     VmaBlockVector::AllocateFromBlock //VmaDeviceMemoryBlock 1-1 VmaBlockMetadata(_Generic)
//      VmaBlockMetadata_Generic::CreateAllocationRequest //just produce -> request (offset sumFreeSize(the size of VmaSuballocation) item(the VmaSuballocation))
//       m_SumFreeSize -> early return
//       VmaBinaryFindFirstNotLess //can move the check bufferImageGranularity here
//       VmaBlockMetadata_Generic::CheckAllocation -> check bufferImageGranularity //there may more than one suballoc in current page  //also used in "canMakeOtherLost"
//       "canMakeOtherLost"
//       new VmaAllocation_T (just alloc memory of VmaAllocation) //use VmaAllocationObjectAllocator/VmaPoolAllocator //may be replaced by tbbmalloc
//       VmaAllocation_T::Ctor currentFrameIndex
//       VmaBlockMetadata_Generic::Alloc //use VmaAllocationRequest and VmaAllocation_T to update data of VmaBlock // VmaSuballocation.hAllocation=VmaAllocation_T //not update VmaAllocation_T
//        VmaBlockMetadata_Generic::UnregisterFreeSuballocation
//         VmaVectorRemove //waste time? //shall we replace the vector(m_FreeSuballocationsBySize) by other containers?
//        VmaBlockMetadata_Generic::RegisterFreeSuballocation(paddingEnd) //
//         ValidateFreeSuballocationList //check the sort
//         VmaVectorInsertSorted //sort
//        VmaBlockMetadata_Generic::RegisterFreeSuballocation(paddingBegin)
//        update early out cache (m_SumFreeSize)
//      VmaBlockVector::UpdateHasEmptyBlock
//      VmaAllocation_T::InitBlockAllocation //update VmaAllocation_T //m_BlockAllocation / m_Dedicate
//      VmaBlockMetadata_Generic::Validate()
//      m_Budget.AddAllocation
//      FillDebugPattern

// VmaAllocation_T //public data structure //can be omitted?
// VmaSuballocation internal data structure

// vmaDestroyBuffer
//  VmaAllocator_T::FreeMemory
//   TouchAllocation //update m_LastUseFrameIndex
//   retrieve blockvector //via pool
//   VmaBlockVector::Free
//     update budget
//     //VmaMutexLockWrite lock //we may asset?
//     VmaDeviceMemoryBlock::unmap //refcount
//     VmaBlockMetadata_Generic::Free
//      VmaBlockMetadata_Generic::FreeSuballocation
//       UnregisterFreeSuballocation
//       MergeFreeWithNext
//       RegisterFreeSuballocation
//     check empty
//     remove block //VmaDeviceMemoryBlock::Destroy -> VmaAllocator_T::FreeVulkanMemory
//     UpdateHasEmptyBlock
//     IncrementallySortBlocks
//

// VmaAllocator_T::FlushOrInvalidateAllocation

// https://github.com/ValveSoftware/dxvk
// DxvkDevice::createImage          // src/dxvk/dxvk_device.cpp
//  DxvkImage::DxvkImage            // src/dxvk/dxvk_image.cpp
//   DxvkMemoryAllocator::alloc     // src/dxvk/dxvk_memory.cpp
//