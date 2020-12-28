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

class gfx_malloc_common
{
protected:
    static uint64_t const SLOB_OFFSET_INVALID;

    class slob_page_t
    {
        class slob_page_t *m_list_next;
        class slob_page_t *m_list_prev;

        static class slob_page_t *const LIST_NEXT_INVALID;
        static class slob_page_t *const LIST_PREV_INVALID;

        struct slob_block_t
        {
            uint64_t offset;
            uint64_t size;
        };

        uint64_t m_units;
        std::forward_list<slob_block_t, mcrt::scalable_allocator<slob_block_t>> m_free;

    protected:
        inline slob_page_t(uint64_t size)
        {
            //list
#ifndef NDEBUG
            m_list_next = LIST_NEXT_INVALID;
            m_list_prev = LIST_PREV_INVALID;
#endif

            //slob
            m_units = size;
            m_free.push_front(slob_block_t{0ULL, size});
        }

    public:
        static inline void list_head_init(class slob_page_t *list_head)
        {
            assert(LIST_NEXT_INVALID == list_head->m_list_next);
            assert(LIST_PREV_INVALID == list_head->m_list_prev);
            list_head->m_list_next = list_head;
            list_head->m_list_prev = list_head;
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
            return list_insert(list_head, value);
        }

        static inline void list_push_back(class slob_page_t *list_head, class slob_page_t *value)
        {
            return list_insert(list_head->m_list_prev, value);
        }

        static inline void list_insert(class slob_page_t *pos, class slob_page_t *value)
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
        void (*slob_lock_list_head_callback)(void),
        void (*slob_unlock_list_head_callback)(void),
        uint64_t size,
        uint64_t align,
        class slob_page_t const **out_slob_page,
        class slob_page_t *(*slob_new_pages_callback)(class gfx_malloc_common *self),
        class gfx_malloc_common *self);

public:
    virtual void *alloc_uniform_buffer(size_t size) = 0;
};

#endif
