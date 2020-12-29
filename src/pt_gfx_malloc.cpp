#include <pt_mcrt_intrin.h>
#include <pt_mcrt_atomic.h>
#include "pt_gfx_malloc.h"

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

// NULL-Pointer Assignment Partition
// https://docs.microsoft.com/en-us/windows-hardware/drivers/gettingstarted/virtual-address-spaces
// https://www.kernel.org/doc/Documentation/arm/memory.txt
// https://www.kernel.org/doc/Documentation/arm64/memory.txt
// https://www.kernel.org/doc/Documentation/x86/x86_64/mm.txt
class gfx_malloc::list_node *const gfx_malloc::list_node::LIST_NODE_NEXT_INVALID = reinterpret_cast<class gfx_malloc::list_node *>(1);
class gfx_malloc::list_node *const gfx_malloc::list_node::LIST_NODE_PREV_INVALID = reinterpret_cast<class gfx_malloc::list_node *>(2);

uint64_t const gfx_malloc::slob_page::SLOB_PAGE_MAGIC = 0XbeefcaffeULL;

uint64_t const gfx_malloc::slob::SLOB_BREAK_INVALID = (~0ULL);

uint64_t const gfx_malloc::SLOB_OFFSET_INVALID = (~0ULL);

inline gfx_malloc::list_node::list_node()
    : m_next(LIST_NODE_NEXT_INVALID),
      m_prev(LIST_NODE_PREV_INVALID)
{
}

inline void gfx_malloc::list_node::list_head_node_init()
{
    assert(!this->is_in_list());
    this->m_next = this;
    this->m_prev = this;
    return;
}

inline void gfx_malloc::list_node::insert_after(class list_node *pos)
{
    class list_node *it_new = this;
    class list_node *it_prev = pos;
    class list_node *it_next = pos->m_next;
    //insert "it_new" between the consecutive "it_prev" and "it_next"
    assert(!this->is_in_list());
    assert(it_next->m_prev == it_prev);
    assert(it_prev->m_next == it_next);
    assert(it_new != it_prev && it_new != it_next);
    it_next->m_prev = it_new;
    it_new->m_next = it_next;
    it_new->m_prev = it_prev;
    it_prev->m_next = it_new;
    return;
}

inline void gfx_malloc::list_node::erase()
{
    class list_node *it_prev = this->m_prev;
    class list_node *it_next = this->m_next;
    assert(this->is_in_list());
    assert(it_prev->m_next == this);
    assert(it_next->m_prev == this);
    it_next->m_prev = it_prev;
    it_prev->m_next = it_next;
    this->m_next = LIST_NODE_NEXT_INVALID;
    this->m_prev = LIST_NODE_PREV_INVALID;
    return;
}

inline bool gfx_malloc::list_node::is_in_list()
{
    return (LIST_NODE_NEXT_INVALID != this->m_next && LIST_NODE_PREV_INVALID != this->m_prev);
}

inline class gfx_malloc::list_node *gfx_malloc::list_node::prev()
{
    return this->m_prev;
}

inline class gfx_malloc::list_node *gfx_malloc::list_node::next()
{
    return this->m_next;
}

inline gfx_malloc::list_head::list_head()
{
    m_head.list_head_node_init();
    return;
}

inline class gfx_malloc::list_node *gfx_malloc::list_head::begin()
{
    return m_head.next();
}

inline class gfx_malloc::list_node *gfx_malloc::list_head::end()
{
    return &m_head;
}

inline void gfx_malloc::list_head::push_front(class list_node *value)
{
    return value->insert_after(&m_head);
}

inline void gfx_malloc::list_head::push_back(class list_node *value)
{
    return value->insert_after(m_head.prev());
}

uint64_t const SLOB_OFFSET_INVALID = (~0ULL);

inline gfx_malloc::slob_block::slob_block(uint64_t offset, uint64_t size)
    : m_offset(offset), m_size(size), m_list()
{
}

inline class gfx_malloc::slob_block *gfx_malloc::slob_block::alloc(uint64_t offset, uint64_t size)
{
    return (new (mcrt_aligned_malloc(sizeof(struct slob_block), alignof(struct slob_block))) slob_block(offset, size));
}

inline void gfx_malloc::slob_block::recycle_as(uint64_t offset, uint64_t size)
{
    m_offset = offset;
    m_size = size;
}

inline void gfx_malloc::slob_block::free()
{
    assert(!this->m_list.is_in_list());
    mcrt_free(this);
}

inline uint64_t gfx_malloc::slob_block::offset()
{
    return m_offset;
}

inline uint64_t gfx_malloc::slob_block::size()
{
    return m_size;
}

inline void gfx_malloc::slob_block::merge_prev(uint64_t merge_count)
{
    assert(m_offset >= merge_count);
    m_offset -= merge_count;
    return;
}

inline void gfx_malloc::slob_block::merge_next(uint64_t merge_count)
{
    m_size += merge_count;
    return;
}

inline class gfx_malloc::list_node *gfx_malloc::slob_block::list()
{
    return &m_list;
}

inline class gfx_malloc::slob_block *gfx_malloc::slob_block::container_of(class list_node *list)
{
    return reinterpret_cast<class slob_block *>(reinterpret_cast<uintptr_t>(list) - static_cast<uintptr_t>(offsetof(slob_block, m_list)));
}

gfx_malloc::slob_page::slob_page(uint64_t size)
    :
#ifndef NDEBUG
      m_magic(SLOB_PAGE_MAGIC),
#endif
      m_units(size)
{
    assert(size > 0U);
    class slob_block *b = slob_block::alloc(0U, size);
    m_free.push_front(b->list());
}

inline uint64_t gfx_malloc::slob_page::size()
{
    return m_units;
}

inline uint64_t gfx_malloc::slob_page::alloc(uint64_t size, uint64_t align)
{
    for (class list_node *it_cur = this->m_free.begin(); it_cur != this->m_free.end(); it_cur = it_cur->next())
    {
        class slob_block *cur = slob_block::container_of(it_cur);
        uint64_t avail = cur->size();
        uint64_t aligned = mcrt_intrin_round_up(cur->offset(), align);
        uint64_t delta = aligned - cur->offset();
        //First-Fit
        if (avail >= (size + delta))
        {
            // VmaBlockMetadata_Generic::CheckAllocation

            // We seperate buffer and optimal-tiling-image
            // We have no linear-tiling-image
            // ---
            // Buffer-Image Granularity
            // ---
            // VkPhysicalDeviceLimits::bufferImageGranularity
            // https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/chap13.html#resources-bufferimagegranularity
            // This restriction is only needed when a linear (BUFFER / IMAGE_TILING_LINEAR) resource and a non-linear (IMAGE_TILING_OPTIMAL) resource are adjacent in memory and will be used simultaneously.
            // ---
            // [VulkanMemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
            // VmaBlocksOnSamePage
            // VmaIsBufferImageGranularityConflict

            // We maintain one list for free blocks while the [VulkanMemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator) maintains two lists
            // ---
            // VmaBlockMetadata_Generic::m_Suballocations // the list for all blocks (free or non-free)
            // VmaBlockMetadata_Generic::m_FreeSuballocationsBySize // the list for free blocks

            // VmaBlockMetadata_Generic::Alloc
            // VmaBlockMetadata_Generic::MergeFreeWithNext

            // recycle
            uint64_t cur_offset = cur->offset();
            uint64_t cur_size = cur->size();

            //merge with prev
            if (delta > 0U)
            {
                class list_node *it_prev = it_cur->prev();
                class slob_block *prev = (it_prev != this->m_free.end()) ? slob_block::container_of(it_prev) : NULL;
                assert((NULL == prev) || ((prev->offset() + prev->size()) <= cur_offset));

                if ((NULL != prev) && ((prev->offset() + prev->size()) == cur_offset))
                {
                    prev->merge_next(delta);
                }
                else
                {
                    assert(cur != NULL);
                    cur->recycle_as(cur_offset, delta);
                    cur = NULL; //offset or size has been changed
                    //the it_cur remains valid
                }
            }

            //merge with next
            if (avail > (size + delta))
            {
                class list_node *it_next = it_cur->next();
                class slob_block *next = (it_next != this->m_free.end()) ? slob_block::container_of(it_next) : NULL;
                assert((NULL == next) || (cur_offset + cur_size <= next->offset()));

                if ((NULL != next) && (cur_offset + cur_size == next->offset()))
                {
                    next->merge_prev(avail - (size + delta));
                }
                else if (NULL != cur)
                {
                    cur->recycle_as(cur_offset + (size + delta), avail - (size + delta));
                    cur = NULL;
                }
                else
                {
                    class slob_block *b = slob_block::alloc(cur_offset + (size + delta), avail - (size + delta));
                    b->list()->insert_after(it_cur);
                }
            }

            if (NULL != cur)
            {
                //not recycled
                cur->list()->erase();
                cur->free();
            }

            this->m_units -= size;
            if (0U == this->m_units)
            {
                this->m_list.erase();
            }
            return cur_offset;
        }
    }

    return SLOB_OFFSET_INVALID;
}

inline void gfx_malloc::slob_page::free(uint64_t offset, uint64_t size)
{
    assert(SLOB_PAGE_MAGIC == this->m_magic);
}

inline class gfx_malloc::list_node *gfx_malloc::slob_page::list()
{
    return &m_list;
}

inline class gfx_malloc::slob_page *gfx_malloc::slob_page::container_of(class list_node *list)
{
    return reinterpret_cast<class slob_page *>(reinterpret_cast<uintptr_t>(list) - static_cast<uintptr_t>(offsetof(slob_page, m_list)));
}

inline void gfx_malloc::slob::lock()
{
    assert(mcrt_atomic_load(&m_slob_lock) == false);
#ifndef NDEBUG
    mcrt_atomic_store(&m_slob_lock, true);
#endif
}

inline void gfx_malloc::slob::unlock()
{
    assert(mcrt_atomic_load(&m_slob_lock) == true);
#ifndef NDEBUG
    mcrt_atomic_store(&m_slob_lock, false);
#endif
}

gfx_malloc::slob::slob() : m_slob_break1(SLOB_BREAK_INVALID), m_slob_break2(SLOB_BREAK_INVALID)
{
}

void gfx_malloc::slob::init(uint64_t page_size)
{
    assert(SLOB_BREAK_INVALID == m_slob_break1);
    assert(SLOB_BREAK_INVALID == m_slob_break2);
    m_page_size = page_size;
    m_slob_break1 = page_size / 16ULL; //(page_size * 256/*SLOB_BREAK1*/) / 4096
    m_slob_break2 = page_size / 4ULL;  //(page_size * 1024/*SLOB_BREAK2*/) / 4096
}

class gfx_malloc::slob_page *gfx_malloc::slob::alloc(
    uint64_t size,
    uint64_t align,
    uint64_t *out_offset)
{
    assert(SLOB_BREAK_INVALID != m_slob_break1);
    assert(SLOB_BREAK_INVALID != m_slob_break2);

    class list_head *slob_list;
    if (size < m_slob_break1)
    {
        slob_list = &m_free_slob_small;
    }
    else if (size < m_slob_break2)
    {
        slob_list = &m_free_slob_medium;
    }
    else if (size < m_page_size)
    {
        slob_list = &m_free_slob_large;
    }
    else
    {
        slob_list = NULL;
    }

    if (NULL != slob_list)
    {
        class slob_page *sp;
        uint64_t b = SLOB_OFFSET_INVALID;

        this->lock();
        for (class list_node *it_sp = slob_list->begin(); it_sp != slob_list->end(); it_sp = it_sp->next())
        {
            class slob_page *sp = slob_page::container_of(it_sp);

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
                    // Improve fragment distribution and reduce our average
                    // search time by starting our next search here. (see
                    // Knuth vol 1, sec 2.5, pg 449)
                    it_sp->erase();
                    slob_list->push_back(it_sp);
                    break;
                }
            }
        }
        this->unlock();

        if (SLOB_OFFSET_INVALID != b)
        {
            //PT_LIKELY
        }
        else
        {
            //The "slob::new_pages" is MT-safe (internally synchronized)
            sp = this->new_pages();

            if (NULL != sp)
            {
                this->lock();
                slob_list->push_front(sp->list());
                b = sp->alloc(size, align);
                this->unlock();

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
        (*out_offset) = b;
        return sp;
    }
    else
    {
        (*out_offset) = SLOB_OFFSET_INVALID;
        return NULL;
    }
}

void gfx_malloc::slob::free(
    slob_page *sp,
    uint64_t offset,
    uint64_t size)
{
    return sp->free(offset, size);
}

// wrap
// VmaAllocation_T - public struct
// VmaSuballocation - internal struct
// VmaBlockMetadata_Generic::Free // search m_Suballocations find VmaSuballocation which the hAllocation matches the VmaAllocation_T

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
//     VmaBlockMetadata_Generic::Free // search m_Suballocations find VmaSuballocation which the hAllocation matches the VmaAllocation_T
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