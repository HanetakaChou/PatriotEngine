#include <pt_mcrt_intrin.h>
#include <pt_mcrt_atomic.h>
#include "pt_gfx_malloc.h"
#include <algorithm>

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

// https://www.kernel.org/doc/gorman/html/understand/understand011.html

#ifndef NDEBUG
uint64_t const gfx_malloc::slob_page::SLOB_PAGE_MAGIC = 0X11BEEF11CAFFE11ULL;
uint64_t const gfx_malloc::slob::SLOB_BREAK_POISON = (~0ULL);
uint64_t const gfx_malloc::slob::PAGE_SIZE_POISON = (~0ULL);
#endif
uint64_t const gfx_malloc::SLOB_OFFSET_POISON = (~0ULL);

inline gfx_malloc::slob_block_list_iter gfx_malloc::wrapped_prev(slob_block_list const &contain, slob_block_list_iter const &iter)
{
    assert(contain.begin() != iter);
    return std::prev(iter);
}

inline void gfx_malloc::wrapped_emplace_hint(slob_block_list &contain, slob_block_list_iter const &hint, uint64_t offset, uint64_t size)
{
    //https://gcc.gnu.org/onlinedocs/libstdc++/manual/associative.html#containers.associative.insert_hints
#ifndef NDEBUG
    size_t sz_before = contain.size();
#endif
    contain.emplace_hint(hint, offset, size);
#ifndef NDEBUG
    assert(contain.size() == (sz_before + 1U));
    assert(wrapped_prev(contain, hint)->offset() == offset);
    assert(wrapped_prev(contain, hint)->size() == size);
#endif
    return;
}

inline gfx_malloc::slob_block_list_iter gfx_malloc::wrapped_lower_bound(slob_block_list const &contain, uint64_t offset, uint64_t size)
{
    class slob_block block(offset, size);
    return contain.lower_bound(std::move(block));
}

inline bool gfx_malloc::slob_block::operator<(class slob_block const &other) const
{
    return ((this->m_offset + this->m_size) <= other.m_offset);
}

inline gfx_malloc::slob_block::slob_block(uint64_t offset, uint64_t size)
    : m_offset(offset),
      m_size(size)
{
}

inline uint64_t gfx_malloc::slob_block::offset() const
{
    return m_offset;
}

inline uint64_t gfx_malloc::slob_block::size() const
{
    return m_size;
}

inline void gfx_malloc::slob_block::merge_next(uint64_t size) const
{
    this->m_size += size;
    return;
}

inline void gfx_malloc::slob_block::merge_prev(uint64_t size) const
{
    assert(size <= m_offset);
    this->m_offset -= size;
    return;
}

inline void gfx_malloc::slob_block::recycle_as(uint64_t offset, uint64_t size) const
{
    this->m_offset = offset;
    this->m_size = size;
    return;
}

inline gfx_malloc::slob_page::slob_page(
#ifndef NDEBUG
    uint64_t page_size,
#endif
    void *page_memory)
    :
#ifndef NDEBUG
      m_magic(SLOB_PAGE_MAGIC),
#endif
      m_is_on_free_page_list(false),
#ifndef NDEBUG
      m_page_size(page_size),
#endif
      m_page_memory(page_memory)
{
}

gfx_malloc::slob_page::~slob_page()
{
#ifndef NDEBUG
    if (2U == m_free_block_list.size())
    {
        slob_block_list_iter iter_first = this->m_free_block_list.begin();
        assert(0U == iter_first->offset());

        slob_block_list_iter iter_second = std::next(iter_first);
        assert(m_page_size != (iter_second->offset() + iter_second->offset()));
    }
    else if (1U == m_free_block_list.size())
    {
        slob_block_list_iter iter_single = this->m_free_block_list.begin();
        assert((0U == iter_single->offset()) || ((iter_single->offset() + iter_single->size()) == m_page_size));
    }
    else
    {
        assert(false);
    }
#endif
}

inline uint64_t gfx_malloc::slob_page::sum_free_size()
{
    return m_sum_free_size;
}

inline void *gfx_malloc::slob_page::page_memory()
{
    return m_page_memory;
}

#ifndef NDEBUG
inline bool gfx_malloc::slob_page::validate_free_block_list()
{
    uint64_t validated_sum_free_size = 0U;

    for (slob_block_list_iter iter_cur = this->m_free_block_list.begin(), iter_next = std::next(iter_cur); iter_cur != this->m_free_block_list.end(); iter_cur = iter_next)
    {
        if (m_free_block_list.end() != iter_next)
        {
            if ((iter_cur->offset() + iter_cur->size()) >= iter_next->offset())
            {
                //We should merge when cur_offset + cur_size equals next_offset
                assert(false);
                return false;
            }
        }

        validated_sum_free_size += iter_cur->size();
    }

    if (validated_sum_free_size != m_sum_free_size)
    {
        assert(false);
        return false;
    }

    return true;
}

inline bool gfx_malloc::slob_page::validate_is_last_free(uint64_t offset, uint64_t size)
{
    if (2U == m_free_block_list.size())
    {
        slob_block_list_iter iter_first = this->m_free_block_list.begin();
        if (0U != iter_first->offset())
        {
            assert(false);
            return false;
        }
        if (offset != iter_first->size())
        {
            assert(false);
            return false;
        }

        slob_block_list_iter iter_second = std::next(iter_first);
        if ((offset + size) != iter_second->offset())
        {
            assert(false);
            return false;
        }
        if (m_page_size != (iter_second->offset() + iter_second->offset()))
        {
            assert(false);
            return false;
        }
    }
    else if (1U == m_free_block_list.size())
    {
        slob_block_list_iter iter_single = this->m_free_block_list.begin();
        if (0U == iter_single->offset())
        {
            if (iter_single->size() != offset || (offset + size) != m_page_size)
            {
                assert(false);
                return false;
            }
        }
        else if ((iter_single->offset() + iter_single->size()) == m_page_size)
        {
            if (0U != offset || iter_single->offset() != size)
            {
                assert(false);
                return false;
            }
        }
        else
        {
            assert(false);
            return false;
        }
    }
    else
    {
        assert(false);
        return false;
    }

    return true;
}
#endif

inline uint64_t gfx_malloc::slob_page::internal_alloc(uint64_t size, uint64_t align)
{
    for (slob_block_list_iter iter_cur = this->m_free_block_list.begin(), iter_next = std::next(iter_cur); iter_cur != this->m_free_block_list.end(); iter_cur = iter_next)
    {
        uint64_t avail = iter_cur->size();
        uint64_t aligned = mcrt_intrin_round_up(iter_cur->offset(), align);
        uint64_t delta = aligned - iter_cur->offset();

        // We use a simple first-fit algorithm while the [VulkanMemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator) uses the best-fit algorithm
        // ---
        // VmaBlockVector::AllocateFromBlock
        //  VmaBlockMetadata_Generic::CreateAllocationRequest
        //  VmaBlockMetadata_Generic::Alloc
        // ---
        // VmaBlockMetadata_Generic::CreateAllocationRequest
        //  VmaBinaryFindFirstNotLess
        // ---
        // VmaBlockMetadata_Generic::Alloc / VmaBlockMetadata_Generic::Free(Suballocation)
        // VmaBlockMetadata_Generic::RegisterFreeSuballocation
        // VmaVectorInsertSorted
        if (avail >= (size + delta))
        {
            // VmaBlockMetadata_Generic::CheckAllocation
            //
            // We seperate buffer and optimal-tiling-image
            // We have no linear-tiling-image
            // ---
            // Buffer-Image Granularity
            // ---
            // VkPhysicalDeviceLimits::bufferImageGranularity
            // https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/chap13.html#resources-bufferimagegranularity
            // This restriction is only needed when a linear (BUFFER / IMAGE_TILING_LINEAR) resource and a non-linear (IMAGE_TILING_OPTIMAL) resource are adjacent in memory and will be used simultaneously.
            // ---
            // VmaBlocksOnSamePage
            // VmaIsBufferImageGranularityConflict

            // We maintain one list for free blocks while the [VulkanMemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator) maintains two lists
            // ---
            // VmaBlockMetadata_Generic::m_Suballocations // the list for all blocks (free or non-free)
            // VmaBlockMetadata_Generic::m_FreeSuballocationsBySize // the list for free blocks // sorted by VmaVectorInsertSorted

            // VmaBlockMetadata_Generic::Alloc
            // VmaBlockMetadata_Generic::MergeFreeWithNext

            // recycle
            uint64_t cur_offset = iter_cur->offset();
            uint64_t cur_size = iter_cur->size();

            //merge with prev
            if (delta > 0U)
            {
                slob_block_list_iter iter_prev = (iter_cur != this->m_free_block_list.begin()) ? wrapped_prev(this->m_free_block_list, iter_cur) : std::move(slob_block_list_iter{});
                assert((this->m_free_block_list.begin() == iter_cur) || ((iter_prev->offset() + iter_prev->size()) <= cur_offset));
                if ((this->m_free_block_list.begin() != iter_cur) && ((iter_prev->offset() + iter_prev->size()) == cur_offset))
                {
                    iter_prev->merge_next(delta);
                }
                else
                {
                    assert(m_free_block_list.end() != iter_cur);
                    iter_cur->recycle_as(cur_offset, delta);
                    iter_cur = this->m_free_block_list.end();
                }
            }

            //merge with next
            if (avail > (size + delta))
            {
                assert(std::next(iter_cur) == iter_next);
                assert((this->m_free_block_list.end() == iter_next) || (cur_offset + cur_size <= iter_next->offset()));
                if ((this->m_free_block_list.end() != iter_next) && (cur_offset + cur_size == iter_next->offset()))
                {
                    iter_next->merge_prev(avail - (size + delta));
                }
                else if (m_free_block_list.end() != iter_cur)
                {
                    iter_cur->recycle_as(cur_offset + (size + delta), avail - (size + delta));
                    iter_cur = m_free_block_list.end();
                }
                else
                {
                    wrapped_emplace_hint(this->m_free_block_list, iter_next, cur_offset + (size + delta), avail - (size + delta));
                }
            }

            //not recycled
            if (m_free_block_list.end() != iter_cur)
            {
                m_free_block_list.erase(iter_cur);
            }

            this->m_sum_free_size -= size;
            return cur_offset;
        }
    }

    return SLOB_OFFSET_POISON;
}

inline void gfx_malloc::slob_page::internal_free(uint64_t offset, uint64_t size)
{
    assert(size > 0U);

    // We use the STL set to find the matching block while the [VulkanMemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator) traverse the list to find the matching block
    // ---
    // VmaBlockMetadata_Generic::Free
    // VmaAllocation(_T) - the public struct
    // VmaSuballocation - the internal struct
    slob_block_list_iter iter_next = wrapped_lower_bound(this->m_free_block_list, offset, size);
    slob_block_list_iter iter_prev = (this->m_free_block_list.begin() != iter_next) ? wrapped_prev(this->m_free_block_list, iter_next) : slob_block_list_iter{};
    assert((this->m_free_block_list.begin() == iter_next) || ((iter_prev->offset() + iter_prev->size()) <= offset));
    assert((this->m_free_block_list.end() == iter_next) || (offset + size <= iter_next->offset()));

    bool contiguous_with_prev = ((this->m_free_block_list.begin() != iter_next) && ((iter_prev->offset() + iter_prev->size()) == offset));
    bool contiguous_with_next = ((this->m_free_block_list.end() != iter_next) && (offset + size == iter_next->offset()));

    if ((!contiguous_with_prev) && (!contiguous_with_next))
    {
        wrapped_emplace_hint(this->m_free_block_list, iter_next, offset, size);
    }
    else if (!contiguous_with_next)
    {
        assert(contiguous_with_prev);
        iter_prev->merge_next(size);
    }
    else if (!contiguous_with_prev)
    {
        assert(contiguous_with_next);
        iter_next->merge_prev(size);
    }
    else
    {
        assert(contiguous_with_prev && contiguous_with_next);
        iter_next->merge_prev(size);
        assert((iter_prev->offset() + iter_prev->size()) == iter_next->offset());
        iter_next->merge_prev(iter_prev->size());
        m_free_block_list.erase(iter_prev);
    }
}

inline uint64_t gfx_malloc::slob_page::alloc(uint64_t size, uint64_t align)
{
    assert(SLOB_PAGE_MAGIC == this->m_magic);

    assert(this->validate_free_block_list());
    uint64_t offset = this->internal_alloc(size, align);
    assert(this->validate_free_block_list());
    return offset;
}

inline void gfx_malloc::slob_page::free(uint64_t offset, uint64_t size)
{
    assert(SLOB_PAGE_MAGIC == this->m_magic);

    assert(this->validate_free_block_list());
    this->internal_free(offset, size);
    assert(this->validate_free_block_list());
    return;
}

inline bool gfx_malloc::slob_page::is_on_free_list()
{
    assert(SLOB_PAGE_MAGIC == this->m_magic);

    assert(!m_is_on_free_page_list || (0U != this->m_free_block_list.size()));
    assert(m_is_on_free_page_list || (0U == this->m_free_block_list.size()));
    assert(&(*this->m_iter_page_list.m_iter) == this);
    return m_is_on_free_page_list;
}

inline class gfx_malloc::slob_page *gfx_malloc::slob_page::init_on_page_free_list(
    slob_page_list *free_page_list,
    uint64_t page_size,
    void *page_memory)
{
    free_page_list->emplace_front(
#ifndef NDEBUG
        page_size,
#endif
        page_memory);
    class slob_page *sp = &(*free_page_list->begin());
    assert(page_memory == sp->m_page_memory);
    wrapped_emplace_hint(sp->m_free_block_list, sp->m_free_block_list.begin(), 0U, page_size);
    sp->m_sum_free_size = page_size;

    sp->m_is_on_free_page_list = true;
    sp->m_page_list = free_page_list;
    sp->m_iter_page_list.m_iter = free_page_list->begin();
    assert(true == sp->m_is_on_free_page_list);
    assert(0U != sp->m_free_block_list.size());
    assert(&(*sp->m_iter_page_list.m_iter) == sp);
    return sp;
}

inline void gfx_malloc::slob_page::clear_on_free_page_list(slob_page_list *full_page_list)
{
    assert(SLOB_PAGE_MAGIC == this->m_magic);

    assert(true == this->m_is_on_free_page_list);
    assert(0U == this->m_free_block_list.size());
    assert(full_page_list != this->m_page_list);
    assert(&(*this->m_iter_page_list.m_iter) == this);

    full_page_list->splice(full_page_list->begin(), (*this->m_page_list), this->m_iter_page_list.m_iter);
    this->m_is_on_free_page_list = false;
    this->m_page_list = full_page_list;
    this->m_iter_page_list.m_iter = full_page_list->begin();

    assert(false == this->m_is_on_free_page_list);
    assert(0U == this->m_free_block_list.size());
    assert(full_page_list == this->m_page_list);
    assert(&(*this->m_iter_page_list.m_iter) == this);
    return;
}

inline void gfx_malloc::slob_page::set_on_free_page_list(slob_page_list *free_page_list)
{
    assert(SLOB_PAGE_MAGIC == this->m_magic);

    assert(false == this->m_is_on_free_page_list);
    assert(0U < this->m_free_block_list.size());
    assert(free_page_list != this->m_page_list);
    assert(&(*this->m_iter_page_list.m_iter) == this);

    free_page_list->splice(free_page_list->begin(), (*this->m_page_list), this->m_iter_page_list.m_iter);
    this->m_is_on_free_page_list = true;
    this->m_page_list = free_page_list;
    this->m_iter_page_list.m_iter = free_page_list->begin();

    assert(true == this->m_is_on_free_page_list);
    assert(0U != this->m_free_block_list.size());
    assert(free_page_list == this->m_page_list);
    assert(&(*this->m_iter_page_list.m_iter) == this);
    return;
}

inline void gfx_malloc::slob_page::destroy_on_free_page_list(uint64_t offset, uint64_t size)
{
    assert(SLOB_PAGE_MAGIC == this->m_magic);

    assert((size + this->m_sum_free_size) == this->m_page_size);
    assert(this->validate_free_block_list());
    assert(this->validate_is_last_free(offset, size));

    assert(this->m_is_on_free_page_list);
    assert(0U != this->m_free_block_list.size());
    assert(&(*this->m_iter_page_list.m_iter) == this);
    this->m_page_list->erase(this->m_iter_page_list.m_iter);
}

inline gfx_malloc::slob::slob()
#ifndef NDEBUG
    : m_slob_break1(SLOB_BREAK_POISON),
      m_slob_break2(SLOB_BREAK_POISON),
      m_page_size(PAGE_SIZE_POISON)
#endif
{
}

inline void gfx_malloc::slob::init(uint64_t page_size)
{
    assert(SLOB_BREAK_POISON == m_slob_break1);
    assert(SLOB_BREAK_POISON == m_slob_break2);
    assert(PAGE_SIZE_POISON == m_page_size);
    m_page_size = page_size;
    m_slob_break1 = page_size / 16ULL; //(page_size * 256/*SLOB_BREAK1*/) / 4096
    m_slob_break2 = page_size / 4ULL;  //(page_size * 1024/*SLOB_BREAK2*/) / 4096
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

inline gfx_malloc::slob_page_list *gfx_malloc::slob::get_free_page_list(uint64_t size)
{
    assert(SLOB_BREAK_POISON != m_slob_break1);
    assert(SLOB_BREAK_POISON != m_slob_break2);
    assert(PAGE_SIZE_POISON != m_page_size);

    slob_page_list *free_page_list;
    if (size < this->m_slob_break1)
    {
        free_page_list = &this->m_free_slob_small;
    }
    else if (size < this->m_slob_break2)
    {
        free_page_list = &this->m_free_slob_medium;
    }
    else if (size < this->m_page_size)
    {
        free_page_list = &this->m_free_slob_large;
    }
    else
    {
        free_page_list = NULL;
    }
    return free_page_list;
}

inline class gfx_malloc::slob_page *gfx_malloc::slob::alloc(
    uint64_t size,
    uint64_t align,
    void *slob_new_pages_callback(void *),
    void *slob_new_pages_callback_data,
    uint64_t *out_offset)
{
    if (size < m_page_size)
    {
        // Note that SLOB pages contain blocks of varying sizes, which differentiates SLOB
        // from a classic slab allocator.
        slob_page_list *free_page_list = get_free_page_list(size);
        assert(NULL != free_page_list);

        class slob_page *sp = NULL;
        uint64_t b = SLOB_OFFSET_POISON;

        this->lock();
        for (slob_page_list_iter iter_sp = free_page_list->begin(), iter_next = std::next(iter_sp); iter_sp != free_page_list->end(); iter_sp = iter_next)
        {
            // We "early return" by "sum_free_size"
            assert(iter_sp->sum_free_size() > 0U);
            if (iter_sp->sum_free_size() >= size)
            {
                // Note that the reported space in a SLOB page is not necessarily
                // contiguous, so the allocation is not guaranteed to succeed.
                b = iter_sp->alloc(size, align);
                if (0U == iter_sp->sum_free_size())
                {
                    iter_sp->clear_on_free_page_list(&this->m_full_slob);
                }

                if (SLOB_OFFSET_POISON != b)
                {
                    sp = &(*iter_sp);

                    // Finally, as an optimization, the appropriate linked list
                    // of pages is cycled such that the next search will begin at the page used to
                    // successfully service the most recent allocation.
                    if (free_page_list->begin() != iter_sp)
                    {
                        //we can move the head node in STL
                        free_page_list->splice(free_page_list->begin(), (*free_page_list), iter_sp);
                        assert(&(*free_page_list->begin()) == sp);
                    }

                    break;
                }
            }
        }
        this->unlock();

        if (SLOB_OFFSET_POISON == b)
        {
            //The "slob_new_pages" is internally synchronized
            void *page_memory = slob_new_pages_callback(slob_new_pages_callback_data);
            if (NULL != page_memory)
            {
                this->lock();
                auto iter_page = slob_page::init_on_page_free_list(free_page_list, this->m_page_size, page_memory);
                b = iter_page->alloc(size, align);
                assert(0U < iter_page->sum_free_size());
                this->unlock();
                //MALLOC BUG
                assert(SLOB_OFFSET_POISON != b);
                sp = &(*iter_page);
            }
            else
            {
                //MALLOC FAIL
                assert(SLOB_OFFSET_POISON == b);
                assert(NULL == sp);
            }
        }

        assert(SLOB_OFFSET_POISON == b || NULL != sp); //SLOB_OFFSET_POISON != b ⇒ NULL != sp
        assert(SLOB_OFFSET_POISON != b || NULL == sp); //SLOB_OFFSET_POISON == b ⇒ NULL == sp
        (*out_offset) = b;
        return sp;
    }
    else
    {
        (*out_offset) = SLOB_OFFSET_POISON;
        return NULL;
    }
}

inline void gfx_malloc::slob::free(
    class slob_page *sp,
    uint64_t offset,
    uint64_t size,
    void slob_free_pages_callback(void *, void *),
    void *slob_free_pages_callback_data)
{
    this->lock();
    if ((sp->sum_free_size() + size) < this->m_page_size)
    {
        if (sp->is_on_free_list())
        {
            sp->free(offset, size);
        }
        else
        {
            sp->free(offset, size);
            slob_page_list *free_page_list = get_free_page_list(size);
            assert(NULL != free_page_list);
            sp->set_on_free_page_list(free_page_list);
        }
        this->unlock();
        return;
    }
    else
    {
        void *page_memory = sp->page_memory();
        sp->destroy_on_free_page_list(offset, size);
        this->unlock();
        slob_free_pages_callback(page_memory, slob_free_pages_callback_data);
        return;
    }
}

gfx_malloc::gfx_malloc()
{
}

gfx_malloc::~gfx_malloc()
{
}

void gfx_malloc::transfer_dst_and_sampled_image_init(uint64_t page_size)
{
    this->m_transfer_dst_and_sampled_image_slob.init(page_size);
}

void *gfx_malloc::transfer_dst_and_sampled_image_alloc(uint64_t size, uint64_t align, void *slob_new_pages_callback(void *), void *slob_new_pages_callback_data, void **out_gfx_malloc_page, uint64_t *out_offset)
{
    class slob_page *sp = this->m_transfer_dst_and_sampled_image_slob.alloc(size, align, slob_new_pages_callback, slob_new_pages_callback_data, out_offset);
    if (NULL != sp)
    {
        (*out_gfx_malloc_page) = sp;
        return sp->page_memory();
    }
    else
    {
        (*out_gfx_malloc_page) = NULL;
        return NULL;
    }
}

void gfx_malloc::transfer_dst_and_sampled_image_free(void *gfx_malloc_page, uint64_t offset, uint64_t size, void *page_memory, void slob_free_pages_callback(void *, void *), void *slob_free_pages_callback_data)
{
    class slob_page *sp = static_cast<class slob_page *>(gfx_malloc_page);
    assert(page_memory == sp->page_memory());
    return m_transfer_dst_and_sampled_image_slob.free(sp, offset, size, slob_free_pages_callback, slob_free_pages_callback_data);
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
//      new VmaAllocation_T (just alloc memory of VmaAllocation) //use VmaAllocationObjectAllocator/VmaPoolAllocator //may be replaced by tbbmalloc
//      VmaAllocation_T::Ctor currentFrameIndex
//      VmaBlockMetadata_Generic::Alloc //use VmaAllocationRequest and VmaAllocation_T to update data of VmaBlock // VmaSuballocation.hAllocation=VmaAllocation_T //not update VmaAllocation_T
//       VmaBlockMetadata_Generic::UnregisterFreeSuballocation
//        VmaVectorRemove //waste time? //shall we replace the vector(m_FreeSuballocationsBySize) by other containers?
//       VmaBlockMetadata_Generic::RegisterFreeSuballocation(paddingEnd) //
//        ValidateFreeSuballocationList //check the sort
//        VmaVectorInsertSorted //sort
//       VmaBlockMetadata_Generic::RegisterFreeSuballocation(paddingBegin)
//       update early out cache (m_SumFreeSize)
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