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

#ifndef NDEBUG
uint64_t const gfx_malloc::slob_page_stl::SLOB_PAGE_MAGIC = 0X11BEEF11CAFFE11ULL;
uint64_t const gfx_malloc::slob_stl::SLOB_BREAK_POISON = (~0ULL);
uint64_t const gfx_malloc::slob_stl::PAGE_SIZE_POISON = (~0ULL);
#endif
uint64_t const gfx_malloc::SLOB_OFFSET_INVALID = (~0ULL);

inline bool gfx_malloc::slob_block_stl::operator<(class slob_block_stl const &other) const
{
    return ((this->m_offset + this->m_size) <= other.m_offset);
}

inline gfx_malloc::slob_block_stl::slob_block_stl(uint64_t offset, uint64_t size)
    : m_offset(offset),
      m_size(size)
{
}

inline uint64_t gfx_malloc::slob_block_stl::offset() const
{
    return m_offset;
}

inline uint64_t gfx_malloc::slob_block_stl::size() const
{
    return m_size;
}

inline void gfx_malloc::slob_block_stl::merge_next(uint64_t size) const
{
    this->m_size += size;
    return;
}

inline void gfx_malloc::slob_block_stl::merge_prev(uint64_t size) const
{
    assert(size <= m_offset);
    this->m_offset -= size;
    return;
}

inline void gfx_malloc::slob_block_stl::recycle_as(uint64_t offset, uint64_t size) const
{
    this->m_offset = offset;
    this->m_size = size;
    return;
}

inline gfx_malloc::slob_page_stl::slob_page_stl(void *page_device_memory)
    :
#ifndef NDEBUG
      m_magic(SLOB_PAGE_MAGIC),
#endif
      m_is_on_free_list(false),
      m_page_device_memory(page_device_memory)
{
}

gfx_malloc::slob_page_stl::~slob_page_stl()
{
    assert(2U >= m_free.size());
    assert(2U != m_free.size() || ((0U == this->m_free.begin()->offset()) && ((std::next(this->m_free.begin())->offset() + std::next(this->m_free.begin())->size()) == m_page_size)));
    assert(1U != m_free.size() || (0U == this->m_free.begin()->offset()) || ((this->m_free.begin()->offset() + this->m_free.begin()->size()) == m_page_size));
}

inline uint64_t gfx_malloc::slob_page_stl::size()
{
    return m_size;
}

inline void *gfx_malloc::slob_page_stl::page_device_memory()
{
    return m_page_device_memory;
}

inline void gfx_malloc::slob_page_stl::insert_block(typename std::set<class slob_block_stl, std::less<class slob_block_stl>, mcrt::scalable_allocator<class slob_block_stl>>::iterator hint, uint64_t offset, uint64_t size)
{
    //https://gcc.gnu.org/onlinedocs/libstdc++/manual/associative.html#containers.associative.insert_hints
#ifndef NDEBUG
    size_t sz_before = this->m_free.size();
#endif
    class slob_block_stl b(offset, size);
    this->m_free.insert(hint, b);
#ifndef NDEBUG
    assert(this->m_free.size() == (sz_before + 1U));
    assert(std::prev(hint)->offset() == offset && std::prev(hint)->size() == size);
#endif
}

inline typename std::set<class gfx_malloc::slob_block_stl, std::less<class gfx_malloc::slob_block_stl>, mcrt::scalable_allocator<class gfx_malloc::slob_block_stl>>::iterator gfx_malloc::slob_page_stl::find_next_block(uint64_t offset, uint64_t size)
{
    class slob_block_stl b(offset, size);
    typename std::set<class slob_block_stl, std::less<class slob_block_stl>, mcrt::scalable_allocator<class slob_block_stl>>::iterator it_next = this->m_free.lower_bound(b);
    return it_next;
}

inline bool gfx_malloc::slob_page_stl::validate_free_list()
{
    uint64_t cal_size = 0U;
    for (auto it_cur = this->m_free.begin(), it_next = std::next(it_cur); it_cur != this->m_free.end(); it_cur = it_next)
    {
        if (!((m_free.end() == it_next) || ((it_cur->offset() + it_cur->size()) < it_next->offset())))
        {
            //We should merge when cur_offset + cur_size equals next_offset
            assert(false);
            return false;
        }

        cal_size += it_cur->size();
    }

    if (cal_size != m_size)
    {
        assert(false);
        return false;
    }

    return true;
}

inline bool gfx_malloc::slob_page_stl::validate_last_free(uint64_t offset, uint64_t size)
{
    auto it_next = this->find_next_block(offset, size);
    auto it_prev = (this->m_free.begin() != it_next) ? std::prev(it_next) : this->m_free.end();

    if (!((this->m_free.end() == it_prev) || ((it_prev->offset() + it_prev->size()) == offset)))
    {
        assert(false);
        return false;
    }

    if (!((this->m_free.end() == it_next) || (offset + size <= it_next->offset())))
    {
        assert(false);
        return false;
    }

    return true;
}

inline uint64_t gfx_malloc::slob_page_stl::internal_alloc(uint64_t size, uint64_t align)
{
    for (auto it_cur = this->m_free.begin(), it_next = std::next(it_cur); it_cur != this->m_free.end(); it_cur = it_next)
    {
        uint64_t avail = it_cur->size();
        uint64_t aligned = mcrt_intrin_round_up(it_cur->offset(), align);
        uint64_t delta = aligned - it_cur->offset();
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
            uint64_t cur_offset = it_cur->offset();
            uint64_t cur_size = it_cur->size();

            //merge with prev
            if (delta > 0U)
            {
                auto it_prev = std::prev(it_cur);
                assert((this->m_free.end() == it_prev) || ((it_prev->offset() + it_prev->size()) <= cur_offset));
                if ((this->m_free.end() != it_prev) && ((it_prev->offset() + it_prev->size()) == cur_offset))
                {
                    it_prev->merge_next(delta);
                }
                else
                {
                    assert(m_free.end() != it_cur);
                    it_cur->recycle_as(cur_offset, delta);
                    it_cur = m_free.end();
                }
            }

            //merge with next
            if (avail > (size + delta))
            {
                assert(std::next(it_cur) == it_next);
                assert((this->m_free.end() == it_next) || (cur_offset + cur_size <= it_next->offset()));
                if ((this->m_free.end() != it_next) && (cur_offset + cur_size == it_next->offset()))
                {
                    it_next->merge_prev(avail - (size + delta));
                }
                else if (m_free.end() != it_cur)
                {
                    it_cur->recycle_as(cur_offset + (size + delta), avail - (size + delta));
                    it_cur = m_free.end();
                }
                else
                {
                    this->insert_block(it_next, cur_offset + (size + delta), avail - (size + delta));
                }
            }

            if (m_free.end() != it_cur)
            {
                //not recycled
                m_free.erase(it_cur);
            }

            this->m_size -= size;
            return cur_offset;
        }
    }

    return SLOB_OFFSET_INVALID;
}

inline void gfx_malloc::slob_page_stl::internal_free(uint64_t offset, uint64_t size)
{
    assert(size > 0U);

    auto it_next = this->find_next_block(offset, size); //We use STL while the VulkanMemoryAllocator use index array to support random access and binary search
    auto it_prev = (this->m_free.begin() != it_next) ? std::prev(it_next) : this->m_free.end();
    assert((this->m_free.end() == it_prev) || ((it_prev->offset() + it_prev->size()) <= offset));
    assert((this->m_free.end() == it_next) || (offset + size <= it_next->offset()));

    bool merge_with_prev = ((this->m_free.end() != it_prev) && ((it_prev->offset() + it_prev->size()) == offset));
    bool merge_with_next = ((this->m_free.end() != it_next) && (offset + size == it_next->offset()));

    if ((!merge_with_prev) && (!merge_with_next))
    {
        this->insert_block(it_next, offset, size);
    }
    else if (!merge_with_next)
    {
        assert(merge_with_prev);
        it_prev->merge_next(size);
    }
    else if (!merge_with_prev)
    {
        assert(merge_with_next);
        it_next->merge_prev(size);
    }
    else
    {
        assert(merge_with_prev && merge_with_next);
        it_prev->merge_next(size);
        assert((it_prev->offset() + it_prev->size()) == it_next->offset());
        it_prev->merge_next(it_next->size());
        m_free.erase(it_next);
    }
}

inline uint64_t gfx_malloc::slob_page_stl::alloc(uint64_t size, uint64_t align)
{
    assert(SLOB_PAGE_MAGIC == this->m_magic);
    assert(this->validate_free_list());
    uint64_t offset = this->internal_alloc(size, align);
    assert(this->validate_free_list());
    return offset;
}

inline void gfx_malloc::slob_page_stl::free(uint64_t offset, uint64_t size)
{
    assert(SLOB_PAGE_MAGIC == this->m_magic);
    assert(this->validate_free_list());
    this->internal_free(offset, size);
    assert(this->validate_free_list());
    return;
}

inline bool gfx_malloc::slob_page_stl::is_on_free_list()
{
    assert(SLOB_PAGE_MAGIC == this->m_magic);
    assert(!m_is_on_free_list || ((&(*this->m_it_free_slob_list) == this) && (0U != this->m_free.size())));
    assert(m_is_on_free_list || ((&(*this->m_it_full_slob_list) == this) && (0U == this->m_free.size())));
    return m_is_on_free_list;
}

inline class gfx_malloc::slob_page_stl *gfx_malloc::slob_page_stl::init_on_free_list(
    std::list<class slob_page_stl, mcrt::scalable_allocator<class slob_page_stl>> *slob_list,
    uint64_t page_size,
    void *page_device_memory)
{
    slob_list->push_front(slob_page_stl{page_device_memory});
    class slob_page_stl *sp = &(*slob_list->begin());
    assert(page_device_memory == sp->m_page_device_memory);
    sp->m_size = page_size;
    sp->insert_block(sp->m_free.begin(), 0U, page_size);
#ifndef NDEBUG
    sp->m_page_size = page_size;
#endif
    assert(false == sp->m_is_on_free_list);
    assert(0U != sp->m_free.size());
    sp->m_is_on_free_list = true;
    sp->m_it_free_slob_list = slob_list->begin();
    assert(&(*sp->m_it_free_slob_list) == sp);

    return sp;
}

inline void gfx_malloc::slob_page_stl::clear_on_free_list(
    std::list<class slob_page_stl, mcrt::scalable_allocator<class slob_page_stl>> *free_slob_list,
    std::list<class slob_page_stl, mcrt::scalable_allocator<class slob_page_stl>> *full_slob_list)
{
    assert(SLOB_PAGE_MAGIC == this->m_magic);
    assert(true == this->m_is_on_free_list);
    assert(&(*this->m_it_free_slob_list) == this);
    assert(0U == this->m_free.size());
    full_slob_list->splice(full_slob_list->begin(), (*free_slob_list), this->m_it_free_slob_list);
    this->m_is_on_free_list = false;
    this->m_it_full_slob_list = full_slob_list->begin();
    assert(&(*this->m_it_full_slob_list) == this);
    return;
}

inline void gfx_malloc::slob_page_stl::set_on_free_list(
    std::list<class slob_page_stl, mcrt::scalable_allocator<class slob_page_stl>> *free_slob_list,
    std::list<class slob_page_stl, mcrt::scalable_allocator<class slob_page_stl>> *full_slob_list)
{
    assert(SLOB_PAGE_MAGIC == this->m_magic);
    assert(false == this->m_is_on_free_list);
    assert(&(*this->m_it_full_slob_list) == this);
    assert(0U != this->m_free.size());
    free_slob_list->splice(free_slob_list->begin(), (*full_slob_list), this->m_it_full_slob_list);
    this->m_is_on_free_list = true;
    this->m_it_free_slob_list = free_slob_list->begin();
    assert(&(*this->m_it_free_slob_list) == this);
    return;
}

inline void gfx_malloc::slob_page_stl::destroy_on_free_list(
    std::list<class slob_page_stl, mcrt::scalable_allocator<class slob_page_stl>> *free_slob_list,
    std::list<class slob_page_stl, mcrt::scalable_allocator<class slob_page_stl>> *full_slob_list,
    uint64_t offset, uint64_t size)
{
    assert(SLOB_PAGE_MAGIC == this->m_magic);
    assert(this->validate_free_list());
    assert(this->validate_last_free(offset, size));

    if (this->m_is_on_free_list)
    {
        assert((&(*this->m_it_free_slob_list) == this) && (0U != this->m_free.size()));
        free_slob_list->erase(this->m_it_free_slob_list);
    }
    else
    {
        assert((&(*this->m_it_full_slob_list) == this) && (0U == this->m_free.size()));
        full_slob_list->erase(this->m_it_full_slob_list);
    }
}

inline gfx_malloc::slob_stl::slob_stl()
#ifndef NDEBUG
    : m_slob_break1(SLOB_BREAK_POISON),
      m_slob_break2(SLOB_BREAK_POISON),
      m_page_size(PAGE_SIZE_POISON)
#endif
{
}

inline void gfx_malloc::slob_stl::init(uint64_t page_size)
{
    assert(SLOB_BREAK_POISON == m_slob_break1);
    assert(SLOB_BREAK_POISON == m_slob_break2);
    assert(PAGE_SIZE_POISON == m_page_size);
    m_page_size = page_size;
    m_slob_break1 = page_size / 16ULL; //(page_size * 256/*SLOB_BREAK1*/) / 4096
    m_slob_break2 = page_size / 4ULL;  //(page_size * 1024/*SLOB_BREAK2*/) / 4096
}

inline void gfx_malloc::slob_stl::lock()
{
    assert(mcrt_atomic_load(&m_slob_lock) == false);
#ifndef NDEBUG
    mcrt_atomic_store(&m_slob_lock, true);
#endif
}

inline void gfx_malloc::slob_stl::unlock()
{
    assert(mcrt_atomic_load(&m_slob_lock) == true);
#ifndef NDEBUG
    mcrt_atomic_store(&m_slob_lock, false);
#endif
}

inline class gfx_malloc::slob_page_stl *gfx_malloc::slob_stl::alloc(
    uint64_t size,
    uint64_t align,
    void *slob_new_pages_callback(void *),
    void *slob_new_pages_callback_data,
    uint64_t *out_offset)
{
    assert(SLOB_BREAK_POISON != m_slob_break1);
    assert(SLOB_BREAK_POISON != m_slob_break2);
    assert(PAGE_SIZE_POISON != m_page_size);

    std::list<class gfx_malloc::slob_page_stl, mcrt::scalable_allocator<class gfx_malloc::slob_page_stl>> *slob_list;
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
        class slob_page_stl *sp = NULL;
        uint64_t b = SLOB_OFFSET_INVALID;

        this->lock();
        for (auto it_sp = slob_list->begin(), it_next = std::next(it_sp); it_sp != slob_list->end(); it_sp = it_next)
        {
            assert(it_sp->size() > 0U);
            if (it_sp->size() >= size)
            {
                // Note that the reported space in a SLOB page is not necessarily
                // contiguous, so the allocation is not guaranteed to succeed.
                b = it_sp->alloc(size, align);
                if (0U == it_sp->size())
                {
                    it_sp->clear_on_free_list(slob_list, &m_full_slob);
                }

                if (SLOB_OFFSET_INVALID != b)
                {
                    sp = &(*it_sp);

                    // Finally, as an optimization, the appropriate linked list
                    // of pages is cycled such that the next search will begin at the page used to
                    // successfully service the most recent allocation.
                    if (slob_list->begin() != it_sp)
                    {
                        //slob_list->move_head_after(slob_page_list_head::prev(sp)); //we can move the head node in STL
                        slob_list->splice(slob_list->begin(), (*slob_list), it_sp);
                        assert(&(*slob_list->begin()) == sp);
                    }

                    break;
                }
            }
        }
        this->unlock();

        if (SLOB_OFFSET_INVALID == b)
        {
            //The "slob_new_pages" is internally synchronized
            void *page_device_memory = slob_new_pages_callback(slob_new_pages_callback_data);
            if (NULL != page_device_memory)
            {
                this->lock();
                auto it_sp = slob_page_stl::init_on_free_list(slob_list, m_page_size, page_device_memory);
                b = it_sp->alloc(size, align);
                if (0U == it_sp->size())
                {
                    it_sp->clear_on_free_list(slob_list, &m_full_slob);
                }
                this->unlock();
                //MALLOC BUG
                assert(SLOB_OFFSET_INVALID != b);
                sp = &(*it_sp);
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

inline void gfx_malloc::slob_stl::free(
    class slob_page_stl *sp,
    uint64_t offset,
    uint64_t size,
    void slob_free_pages_callback(void *, void *),
    void *slob_free_pages_callback_data)
{
    if ((sp->size() + size) < this->m_page_size)
    {
        this->lock();
        if (sp->is_on_free_list())
        {
            sp->free(offset, size);
        }
        else
        {
            sp->free(offset, size);
            std::list<class gfx_malloc::slob_page_stl, mcrt::scalable_allocator<class gfx_malloc::slob_page_stl>> *slob_list;
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
            assert(NULL != slob_list);
            sp->set_on_free_list(slob_list, &m_full_slob);
        }
        this->unlock();
        return;
    }
    else
    {
        void *page_device_memory = sp->page_device_memory();
        this->lock();
        std::list<class gfx_malloc::slob_page_stl, mcrt::scalable_allocator<class gfx_malloc::slob_page_stl>> *slob_list;
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
        assert(NULL != slob_list);
        sp->destroy_on_free_list(slob_list, &m_full_slob, offset, size);
        this->unlock();
        slob_free_pages_callback(page_device_memory, slob_free_pages_callback_data);
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
    class slob_page_stl *sp = this->m_transfer_dst_and_sampled_image_slob.alloc(size, align, slob_new_pages_callback, slob_new_pages_callback_data, out_offset);
    if (NULL != sp)
    {
        (*out_gfx_malloc_page) = sp;
        return sp->page_device_memory();
    }
    else
    {
        (*out_gfx_malloc_page) = NULL;
        return NULL;
    }
}

void gfx_malloc::transfer_dst_and_sampled_image_free(void *gfx_malloc_page, uint64_t offset, uint64_t size, void *page_device_memory, void slob_free_pages_callback(void *, void *), void *slob_free_pages_callback_data)
{
    class slob_page_stl *sp = static_cast<class slob_page_stl *>(gfx_malloc_page);
    assert(page_device_memory == sp->page_device_memory());
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