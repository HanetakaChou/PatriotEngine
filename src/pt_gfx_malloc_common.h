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
#include <deque>
#include <assert.h>

class gfx_malloc_common
{
protected:
    static uint64_t const slob_invalid_offset;

    class slob_page_t
    {
        class slob_page_t *m_list_next;
        class slob_page_t *m_list_prev;

        struct slob_block_t
        {
            uint64_t offset;
            uint64_t size;
        };

        std::deque<slob_block_t, mcrt::scalable_allocator<slob_block_t>> m_free;

    protected:
        inline slob_page_t(uint64_t size)
        {
            //slob_block_t
            m_free.push_back(slob_block_t{0ULL, size});

            //init_list_head
            m_list_next = this;
            m_list_prev = this;
        }

    public:
        template <typename _Predicate>
        static inline void find_if_not(class slob_page_t *list_head, _Predicate __pred)
        {
            for (class slob_page_t *__it = list_head; __it != list_head; __it = __it->m_list_next)
            {
                if (__pred(__it))
                {
                }
                else
                {
                    break;
                }
            }
        }

        uint64_t alloc(uint64_t size, uint64_t align);
    };

protected:
    //slob_page_t::alloc not MT-safe //we put it in the scope of the list_head lock
    //MT-safe: slob_new_pages_callback
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
