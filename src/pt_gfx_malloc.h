/*
 * Copyright (C) YuqiaoZhang(HanetakaYuminaga)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _PT_GFX_MALLOC_H_
#define _PT_GFX_MALLOC_H_ 1

#include <stddef.h>
#include <stdint.h>

#include <pt_common.h>
#include <pt_gfx_common.h>
#include <pt_mcrt_common.h>
#include <pt_mcrt_thread.h>

#include <list>
#include <set>

#include <pt_mcrt_scalable_allocator.h>

#include <assert.h>

class gfx_malloc
{

    // [RingBuffer](https://docs.microsoft.com/en-us/windows/win32/direct3d12/fence-based-resource-management) related
private:
    class ring_buffer
    {
#ifndef NDEBUG
        bool m_buffer_lock;
        mcrt_native_thread_id m_buffer_thread_id; // indicates the "owner" // is only valid when locked
#endif
        uint64_t m_buffer_size;
        uint64_t m_buffer_begin;
        uint64_t m_buffer_end;
        uint64_t m_buffer_remainder;

    public:
        inline ring_buffer();

        inline void init(uint64_t buffer_size);
        inline void lock();
        inline uint64_t offset();
        inline bool validate_offset(uint64_t size); // false means the offset would be zero
        inline bool alloc(uint64_t size, uint64_t *out_offset);
        inline void free(uint64_t offset, uint64_t size);
        inline void unlock();
    };

    class ring_buffer m_transfer_src_buffer;

    class ring_buffer m_uniform_buffer;

protected:
    void transfer_src_buffer_init(uint64_t buffer_size);
    void transfer_src_buffer_lock();
    uint64_t transfer_src_buffer_offset();
    bool transfer_src_buffer_validate_offset(uint64_t size);
    bool transfer_src_buffer_alloc(uint64_t size, uint64_t *out_offset);
    void transfer_src_buffer_free(uint64_t offset, uint64_t size);
    void transfer_src_buffer_unlock();

    void uniform_buffer_init(uint64_t buffer_size);
    void uniform_buffer_lock();
    uint64_t uniform_buffer_offset();
    bool uniform_buffer_validate_offset(uint64_t size);
    bool uniform_buffer_alloc(uint64_t size, uint64_t *out_offset);
    void uniform_buffer_free(uint64_t offset, uint64_t size);
    void uniform_buffer_unlock();

    // SLOB related
private:
    static uint64_t const SLOB_OFFSET_POISON;

    class slob_block;
    class slob_block_list_comp;
    class slob_page;

    class slob_block_list_comp
    {
    public:
        inline bool operator()(class slob_block const &lhs, class slob_block const &rhs) const;
    };
    static_assert(std::is_pod<slob_block_list_comp>::value, "");

    typedef std::set<class slob_block, class slob_block_list_comp, mcrt::scalable_allocator<class slob_block>> slob_block_list;
    typedef std::set<class slob_block, class slob_block_list_comp, mcrt::scalable_allocator<class slob_block>>::iterator slob_block_list_iter;
    typedef std::list<class slob_page, mcrt::scalable_allocator<class slob_page>> slob_page_list;
    typedef std::list<class slob_page, mcrt::scalable_allocator<class slob_page>>::iterator slob_page_list_iter;
    static inline slob_block_list_iter wrapped_prev(slob_block_list const &contain, slob_block_list_iter const &iter);
    static inline slob_block_list_iter wrapped_next(slob_block_list const &contain, slob_block_list_iter const &iter);
    static inline void wrapped_emplace_hint(slob_block_list &contain, slob_block_list_iter const &hint, uint64_t offset, uint64_t size);
    static inline slob_block_list_iter wrapped_lower_bound(slob_block_list const &contain, uint64_t offset, uint64_t size);
    static inline slob_page_list_iter wrapped_next(slob_page_list const &contain, slob_page_list_iter const &iter);

    class slob_block
    {
        mutable uint64_t m_offset;
        mutable uint64_t m_size;

    public:
        inline uint64_t offset() const;
        inline uint64_t size() const;
        inline void merge_next(uint64_t size) const;
        inline void merge_prev(uint64_t size) const;
        inline void recycle_as(uint64_t offset, uint64_t size) const;
        friend class slob_block_list_comp;
        friend inline void gfx_malloc::wrapped_emplace_hint(slob_block_list &contain, slob_block_list_iter const &hint, uint64_t offset, uint64_t size);
        friend inline slob_block_list_iter gfx_malloc::wrapped_lower_bound(slob_block_list const &contain, uint64_t offset, uint64_t size);
    };
    static_assert(std::is_pod<slob_block>::value, "");

    class slob_page
    {
#ifndef NDEBUG
        static uint64_t const SLOB_PAGE_MAGIC;
        uint64_t m_magic;
#endif
        // for early return/reject
        uint64_t m_sum_free_size;
        // block free list
        slob_block_list m_free_block_list;

        bool m_is_on_free_page_list;
        slob_page_list *m_page_list;
        // work around the "python stack overflow" bug for "libstdc++ Pretty-printers"
        struct
        {
            slob_page_list_iter m_iter;
        } m_iter_page_list;

#ifndef NDEBUG
        uint64_t m_page_size;
#endif
        uint64_t m_page_memory_handle; //handle

#ifndef NDEBUG
        inline bool validate_free_block_list();

        inline bool validate_is_last_free(uint64_t offset, uint64_t size);
#endif

        inline uint64_t internal_alloc(uint64_t size, uint64_t align);

        inline void internal_free(uint64_t offset, uint64_t size);

    public:
        inline slob_page(
#ifndef NDEBUG
            uint64_t page_size,
#endif
            uint64_t page_memory_handle);

        inline ~slob_page();

        inline uint64_t sum_free_size();

        inline uint64_t page_memory_handle();

        inline uint64_t alloc(uint64_t size, uint64_t align);

        inline void free(uint64_t offset, uint64_t size);

        inline bool is_on_free_page_list();

        static inline class slob_page *init_on_free_page_list(slob_page_list *free_page_list, uint64_t page_size, uint64_t page_memory_handle);

        inline void clear_on_free_page_list(slob_page_list *full_page_list);

        inline void set_on_free_page_list(slob_page_list *free_page_list);

        inline void destroy_on_free_page_list(uint64_t offset, uint64_t size);
    };

    class slob
    {
#ifndef NDEBUG
        static uint64_t const SLOB_BREAK_POISON;
        static uint64_t const PAGE_SIZE_POISON;
#endif
        uint64_t m_slob_break1;
        uint64_t m_slob_break2;

        slob_page_list m_small_free_page_list;
        slob_page_list m_medium_free_page_list;
        slob_page_list m_large_free_page_list;
        slob_page_list m_full_page_list;

#ifndef NDEBUG
        bool m_slob_lock;
#endif
        inline void lock();
        inline void unlock();

        uint64_t m_page_size;

        inline slob_page_list *get_free_page_list(uint64_t size);

    public:
        inline slob();

        inline void init(uint64_t page_size);

        //The "slob_new_pages" is internally synchronized
        inline class slob_page *alloc(
            uint64_t size,
            uint64_t align,
            uint64_t slob_new_pages_callback(void *),
            void *slob_new_pages_callback_data,
            uint64_t *out_offset);

        //We pass size in //like unmap
        inline void free(
            class slob_page *page,
            uint64_t offset,
            uint64_t size,
            void slob_free_pages_callback(uint64_t, void *),
            void *slob_free_pages_callback_data);
    };

    // We never use a buffer as both the vertex buffer and the index buffer
    class slob m_transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_slob;

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
    class slob m_transfer_dst_and_sampled_image_slob;

protected:
    static uint64_t const PAGE_MEMORY_HANDLE_POISON;

    void transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_init(uint64_t page_size);
    uint64_t transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_alloc(uint64_t size, uint64_t align, uint64_t slob_new_pages_callback(void *), void *slob_new_pages_callback_data, void **out_page_handle, uint64_t *out_offset);
    void transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_free(void *page_handle, uint64_t offset, uint64_t size, uint64_t page_memory_handle, void slob_free_pages_callback(uint64_t, void *), void *slob_free_pages_callback_data);

    void transfer_dst_and_sampled_image_init(uint64_t page_size);
    uint64_t transfer_dst_and_sampled_image_alloc(uint64_t size, uint64_t align, uint64_t slob_new_pages_callback(void *), void *slob_new_pages_callback_data, void **out_page_handle, uint64_t *out_offset);
    void transfer_dst_and_sampled_image_free(void *page_handle, uint64_t offset, uint64_t size, uint64_t page_memory_handle, void slob_free_pages_callback(uint64_t, void *), void *slob_free_pages_callback_data);

    // CPP related
protected:
    gfx_malloc();
    ~gfx_malloc();
};

#endif
