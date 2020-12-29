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
#include <forward_list>
#include <assert.h>
#include <new>

class gfx_malloc
{
    class list_node
    {
        static class list_node *const LIST_NODE_NEXT_INVALID;
        static class list_node *const LIST_NODE_PREV_INVALID;

        class list_node *m_next;
        class list_node *m_prev;

    public:
        inline list_node();
        inline void list_head_node_init();
        inline void insert_after(class list_node *pos);
        inline void erase();
        inline bool is_in_list();
        inline class list_node *prev();
        inline class list_node *next();
    };

    class list_head
    {
        list_node m_head;

    public:
        inline list_head();
        inline class list_node *begin();
        inline class list_node *end();
        inline void push_front(class list_node *value);
        inline void push_back(class list_node *value);
    };

    class slob_block
    {
        uint64_t m_offset;
        uint64_t m_size;
        class list_node m_list;
        inline slob_block(uint64_t offset, uint64_t size);

    public:
        static inline class slob_block *alloc(uint64_t offset, uint64_t size);
        inline void recycle_as(uint64_t offset, uint64_t size);
        inline void free();
        inline uint64_t offset();
        inline uint64_t size();
        inline void merge_prev(uint64_t merge_count);
        inline void merge_next(uint64_t merge_count);
        inline class list_node *list();
        static inline class slob_block *container_of(class list_node *list);
    };

protected:
    class slob_page
    {
#ifndef NDEBUG
        static uint64_t const SLOB_PAGE_MAGIC;
        uint64_t m_magic;
#endif
        uint64_t m_units;
        class list_head m_free;
        class list_node m_list;

    protected:
        slob_page(uint64_t size);

    public:
        inline uint64_t size();
        inline uint64_t alloc(uint64_t size, uint64_t align);
        inline void free(uint64_t offset, uint64_t size);
        inline class list_node *list();
        static inline class slob_page *container_of(class list_node *list);
    };

    class slob
    {
        static uint64_t const SLOB_BREAK_INVALID;
        uint64_t m_slob_break1;
        uint64_t m_slob_break2;

        class list_head m_free_slob_small;
        class list_head m_free_slob_medium;
        class list_head m_free_slob_large;

#ifndef NDEBUG
        bool m_slob_lock;
#endif
        inline void lock();
        inline void unlock();

        virtual class slob_page *new_pages() = 0;

    protected:
        uint64_t m_page_size;

        slob();
        void init(uint64_t page_size);

    public:
        //The "slob_page::alloc" is not MT-safe //we put it in the scope of the list_head lock
        //The "slob::new_pages" is MT-safe (internally synchronized)
        class slob_page *alloc(
            uint64_t size,
            uint64_t align,
            uint64_t *out_offset);
        //free (offset size) //like unmap

        static void free(
            slob_page *sp,
            uint64_t offset,
            uint64_t size);
    };

    static uint64_t const SLOB_OFFSET_INVALID;

public:
    virtual void *alloc_uniform_buffer(size_t size) = 0;
};

#endif
