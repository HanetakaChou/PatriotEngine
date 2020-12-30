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
#include <pt_mcrt_scalable_allocator.h>
#include <list>
#include <set>
#include <new>
#include <assert.h>

class gfx_malloc
{
    static uint64_t const SLOB_OFFSET_INVALID;

    class slob_block_stl
    {
        mutable uint64_t m_offset;
        mutable uint64_t m_size;

    public:
        inline bool operator<(class slob_block_stl const &other) const;
        inline slob_block_stl(uint64_t offset, uint64_t size);
        inline uint64_t offset() const;
        inline uint64_t size() const;
        inline void merge_next(uint64_t size) const;
        inline void merge_prev(uint64_t size) const;
        inline void recycle_as(uint64_t offset, uint64_t size) const;
    };

    class slob_page_stl
    {
#ifndef NDEBUG
        static uint64_t const SLOB_PAGE_MAGIC;
        uint64_t m_magic;
#endif

        bool m_is_on_free_list;
        typename std::list<class slob_page_stl, mcrt::scalable_allocator<class slob_page_stl>>::iterator m_it_free_slob_list;
        typename std::list<class slob_page_stl, mcrt::scalable_allocator<class slob_page_stl>>::iterator m_it_full_slob_list;

        uint64_t m_size;
        std::set<class slob_block_stl, std::less<class slob_block_stl>, mcrt::scalable_allocator<class slob_block_stl>> m_free;

#ifndef NDEBUG
        uint64_t m_page_size;
#endif

        void *m_page_device_memory;

        inline slob_page_stl(void *page_device_memory);

        inline bool validate_free_list();

        inline bool validate_last_free(uint64_t offset, uint64_t size);

        inline uint64_t internal_alloc(uint64_t size, uint64_t align);

        inline void internal_free(uint64_t offset, uint64_t size);

        inline void insert_block(typename std::set<class slob_block_stl, std::less<class slob_block_stl>, mcrt::scalable_allocator<class slob_block_stl>>::iterator hint, uint64_t offset, uint64_t size);

        inline typename std::set<class slob_block_stl, std::less<class slob_block_stl>, mcrt::scalable_allocator<class slob_block_stl>>::iterator find_next_block(uint64_t offset, uint64_t size);

    public:
        inline ~slob_page_stl();

        inline uint64_t size();

        inline void *page_device_memory();

        inline uint64_t alloc(uint64_t size, uint64_t align);

        inline void free(uint64_t offset, uint64_t size);

        inline bool is_on_free_list();

        static inline class slob_page_stl *init_on_free_list(
            std::list<class slob_page_stl, mcrt::scalable_allocator<class slob_page_stl>> *slob_list,
            uint64_t page_size,
            void *page_device_memory);

        inline void clear_on_free_list(
            std::list<class slob_page_stl, mcrt::scalable_allocator<class slob_page_stl>> *free_slob_list,
            std::list<class slob_page_stl, mcrt::scalable_allocator<class slob_page_stl>> *full_slob_list);

        inline void set_on_free_list(
            std::list<class slob_page_stl, mcrt::scalable_allocator<class slob_page_stl>> *free_slob_list,
            std::list<class slob_page_stl, mcrt::scalable_allocator<class slob_page_stl>> *full_slob_list);

        inline void destroy_on_free_list(
            std::list<class slob_page_stl, mcrt::scalable_allocator<class slob_page_stl>> *free_slob_list,
            std::list<class slob_page_stl, mcrt::scalable_allocator<class slob_page_stl>> *full_slob_list,
            uint64_t offset, uint64_t size);
    };

private:
    class slob_stl
    {
#ifndef NDEBUG
        static uint64_t const SLOB_BREAK_POISON;
        static uint64_t const PAGE_SIZE_POISON;
#endif
        uint64_t m_slob_break1;
        uint64_t m_slob_break2;

        std::list<class slob_page_stl, mcrt::scalable_allocator<class slob_page_stl>> m_free_slob_small;
        std::list<class slob_page_stl, mcrt::scalable_allocator<class slob_page_stl>> m_free_slob_medium;
        std::list<class slob_page_stl, mcrt::scalable_allocator<class slob_page_stl>> m_free_slob_large;
        std::list<class slob_page_stl, mcrt::scalable_allocator<class slob_page_stl>> m_full_slob;

#ifndef NDEBUG
        bool m_slob_lock;
#endif
        inline void lock();
        inline void unlock();

        inline void *new_pages() { return NULL; };
        inline void free_pages(void *page_device_memory) {}

        uint64_t m_page_size;

    public:
        inline slob_stl();

        inline void init(uint64_t page_size);

        //The "slob_new_pages" is internally synchronized
        inline class slob_page_stl *alloc(
            uint64_t size,
            uint64_t align,
            void *slob_new_pages_callback(void *),
            void *slob_new_pages_callback_data,
            uint64_t *out_offset);

        //We pass size in //like unmap
        inline void free(
            class slob_page_stl *sp,
            uint64_t offset,
            uint64_t size,
            void slob_free_pages_callback(void *, void *),
            void *slob_free_pages_callback_data);
    };

    slob_stl m_transfer_dst_and_sampled_image_slob;

public:
    gfx_malloc();
    ~gfx_malloc();

    //using gfx_malloc_page_handle = typename std::list<class slob_page_stl, mcrt::scalable_allocator<class slob_page_stl>>::iterator;
    virtual void *alloc_uniform_buffer(size_t size) = 0;

protected:
    void transfer_dst_and_sampled_image_init(uint64_t page_size);
    void *transfer_dst_and_sampled_image_alloc(uint64_t size, uint64_t align, void *slob_new_pages_callback(void *), void *slob_new_pages_callback_data, void **out_gfx_malloc_page, uint64_t *out_offset);
    void transfer_dst_and_sampled_image_free(void *gfx_malloc_page, uint64_t offset, uint64_t size, void *page_device_memory, void slob_free_pages_callback(void *, void *), void *slob_free_pages_callback_data);
};

#endif
