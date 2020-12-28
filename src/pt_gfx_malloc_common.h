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

#ifndef _PT_GFX_MALLOC_COMMON_H_
#define _PT_GFX_MALLOC_COMMON_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_common.h>
#include <pt_gfx_common.h>
#include <pt_mcrt_common.h>
#include <pt_mcrt_scalable_allocator.h>
#include <forward_list>
#include <assert.h>
#include <new>

class list_node
{
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

class gfx_malloc_common
{
protected:
    static uint64_t const SLOB_OFFSET_INVALID;

    class slob_page_t
    {

        class slob_block_t
        {
            //slob
            uint64_t m_offset;
            uint64_t m_size;

            //forward_list
            class slob_block_t *m_forward_list_next;

            static class slob_block_t *const FORWARD_LIST_NEXT_UNINIT;

            inline slob_block_t(uint64_t offset, uint64_t size)
                : m_offset(offset), m_size(size)
#ifndef NDEBUG
                  ,
                  m_forward_list_next(FORWARD_LIST_NEXT_UNINIT)
#endif
            {
            }

        public:
            inline slob_block_t(class slob_block_t *forward_list_next)
                : m_offset(0U), m_size(0U), m_forward_list_next(forward_list_next)
            {
                //forward_list_head_init
            }

            inline uint64_t offset() { return m_offset; }

            inline uint64_t size() { return m_size; }

            static inline class slob_block_t *alloc(uint64_t offset, uint64_t size)
            {
                return (new (mcrt_aligned_malloc(sizeof(class slob_block_t), alignof(class slob_block_t))) slob_block_t(offset, size));
            }

            static inline class slob_block_t *forward_list_iterator_next(class slob_block_t *it)
            {
                return it->m_forward_list_next;
            }

            static inline void forward_list_insert_after(class slob_block_t *pos, class slob_block_t *value)
            {
                class slob_block_t *it_new = value;
                class slob_block_t *it_prev = pos;
                class slob_block_t *it_next = pos->m_forward_list_next;
                //insert "it_new" between the consecutive "it_prev" and "it_next"
                assert(slob_block_t::FORWARD_LIST_NEXT_UNINIT == it_new->m_forward_list_next);
                assert(it_prev->m_forward_list_next == it_next);
                assert(it_new != it_prev && it_new != it_next);
                it_new->m_forward_list_next = it_next;
                it_prev->m_forward_list_next = it_new;
            }
        };

        class slob_block_forward_list_t
        {
            class slob_block_t m_head;
            static class slob_block_t *const FORWARD_LIST_NEXT_END;

        public:
            inline slob_block_forward_list_t()
                : m_head(forward_list_end())
            {

                //m_head.forward_list_head_init();
            }

            inline class slob_block_t *forward_list_begin()
            {
                return slob_block_t::forward_list_iterator_next(&m_head);
            }

            inline class slob_block_t *forward_list_end()
            {
                return FORWARD_LIST_NEXT_END;
            }

            inline void forward_list_push_front(class slob_block_t *value)
            {
                return slob_block_t::forward_list_insert_after(&m_head, value);
            }
        };

        uint64_t m_units;
        class list_head m_free;

        class slob_page_t *m_list_next;
        class slob_page_t *m_list_prev;

        static class slob_page_t *const LIST_NEXT_INVALID;
        static class slob_page_t *const LIST_PREV_INVALID;

    protected:
        slob_page_t(uint64_t size);

    public:
        inline void list_head_init()
        {
            assert(LIST_NEXT_INVALID == this->m_list_next);
            assert(LIST_PREV_INVALID == this->m_list_prev);
            this->m_list_next = this;
            this->m_list_prev = this;
        }

        static inline class slob_page_t *list_begin(class slob_page_t *list_head)
        {
            return list_head->m_list_next;
        }

        static inline class slob_page_t *list_end(class slob_page_t *list_head)
        {
            return list_head;
        }

        static inline class slob_page_t *list_iterator_next(class slob_page_t *it)
        {
            return it->m_list_next;
        }

        static inline void list_push_front(class slob_page_t *list_head, class slob_page_t *value)
        {
            return list_insert_after(list_head, value);
        }

        static inline void list_push_back(class slob_page_t *list_head, class slob_page_t *value)
        {
            return list_insert_after(list_head->m_list_prev, value);
        }

        static inline void list_insert_after(class slob_page_t *pos, class slob_page_t *value)
        {
            class slob_page_t *it_new = value;
            class slob_page_t *it_prev = pos;
            class slob_page_t *it_next = pos->m_list_next;
            //insert "it_new" between the consecutive "it_prev" and "it_next"
            assert(LIST_NEXT_INVALID == it_new->m_list_next);
            assert(LIST_PREV_INVALID == it_new->m_list_prev);
            assert(it_next->m_list_prev == it_prev);
            assert(it_prev->m_list_next == it_next);
            assert(it_new != it_prev && it_new != it_next);
            it_next->m_list_prev = it_new;
            it_new->m_list_next = it_next;
            it_new->m_list_prev = it_prev;
            it_prev->m_list_next = it_new;
        }

        static inline void list_erase(class slob_page_t *pos)
        {
            class slob_page_t *it_prev = pos->m_list_prev;
            class slob_page_t *it_next = pos->m_list_next;
            assert(LIST_NEXT_INVALID != it_next);
            assert(LIST_PREV_INVALID != it_prev);
            assert(it_prev->m_list_next == pos);
            assert(it_next->m_list_prev == pos);
            it_next->m_list_prev = it_prev;
            it_prev->m_list_next = it_next;
#ifndef NDEBUG
            pos->m_list_next = LIST_NEXT_INVALID;
            pos->m_list_prev = LIST_PREV_INVALID;
#endif
        }

        inline uint64_t size();

        inline uint64_t alloc(uint64_t size, uint64_t align);
    };

protected:
    //The "slob_page_t::alloc: is not MT-safe //we put it in the scope of the list_head lock
    //The "slob_new_pages_callback" is MT-safe (internally synchronized)
    static uint64_t slob_alloc(
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
        class gfx_malloc_common *self);

public:
    virtual void *alloc_uniform_buffer(size_t size) = 0;

    virtual uint64_t alloc_transfer_dst_and_sampled_image(size_t size, size_t alignment, void **out_device_memory) = 0; /*out_size*/

    //virtual

    static uint64_t const MALLOC_OFFSET_INVALID;
};

#endif
