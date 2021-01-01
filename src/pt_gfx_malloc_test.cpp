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

#include <stddef.h>
#include <stdint.h>
#include "pt_gfx_malloc.h"

class gfx_malloc_test : public gfx_malloc
{
    static uint64_t transfer_dst_and_sampled_image_slob_new_pages(void *);
    static void transfer_dst_and_sampled_image_slob_free_pages(uint64_t, void *);

public:
    inline gfx_malloc_test();
    inline uint64_t transfer_dst_and_sampled_image_alloc(uint64_t size, uint64_t align, void **out_page_handle, uint64_t *out_offset);
    inline void transfer_dst_and_sampled_image_free(void *page_handle, uint64_t offset, uint64_t size, uint64_t page_memory_handle);
};

int main(int argc, char **argv)
{
    class gfx_malloc_test malloc_test;
    return 0;
}

inline gfx_malloc_test::gfx_malloc_test()
{
    // 256MB
    this->transfer_dst_and_sampled_image_init(256ULL * 1024ULL * 1024ULL);
}

inline uint64_t gfx_malloc_test::transfer_dst_and_sampled_image_alloc(uint64_t size, uint64_t align, void **out_page_handle, uint64_t *out_offset)
{
    return this->gfx_malloc::transfer_dst_and_sampled_image_alloc(size, align, transfer_dst_and_sampled_image_slob_new_pages, this, out_page_handle, out_offset);
}

inline void gfx_malloc_test::transfer_dst_and_sampled_image_free(void *page_handle, uint64_t offset, uint64_t size, uint64_t page_memory_handle)
{
    return this->gfx_malloc::transfer_dst_and_sampled_image_free(page_handle, offset, size, page_memory_handle, transfer_dst_and_sampled_image_slob_free_pages, this);
}

uint64_t gfx_malloc_test::transfer_dst_and_sampled_image_slob_new_pages(void *)
{
    return 0XDEADBEEFULL;
}

void gfx_malloc_test::transfer_dst_and_sampled_image_slob_free_pages(uint64_t page_memory_handle, void *)
{
    assert(0XDEADBEEFULL == page_memory_handle);
}