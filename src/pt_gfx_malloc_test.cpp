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
#include <pt_mcrt_scalable_allocator.h>
#include "pt_gfx_malloc.h"
#include <vector>
#include <set>
#include <numeric>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

class gfx_malloc_test : public gfx_malloc
{
    uint64_t m_transfer_dst_and_sampled_image_page_memory_handle_counter;
    std::set<uint64_t, std::less<uint64_t>, mcrt::scalable_allocator<uint64_t>> m_transfer_dst_and_sampled_image_page_dict;
    static uint64_t transfer_dst_and_sampled_image_slob_new_pages(void *);
    static void transfer_dst_and_sampled_image_slob_free_pages(uint64_t, void *);

public:
    inline gfx_malloc_test();
    inline uint64_t transfer_dst_and_sampled_image_alloc(uint64_t size, uint64_t align, void **out_page_handle, uint64_t *out_offset);
    inline void transfer_dst_and_sampled_image_free(void *page_handle, uint64_t offset, uint64_t size, uint64_t page_memory_handle);
};

// [VulkanMemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
//
// Copyright (c) 2017-2019 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//
/** @file src/Tests.cpp **/
//
// PerformMainTests
// MainTest
// MainTest_GetNextAllocationSize
// MainTest_Allocate

struct allocation_size
{
    uint32_t probability;
    uint32_t image_size_min;
    uint32_t image_size_max;
};

struct test_allocation_result
{
    void *page_handle;
    uint64_t offset;
    uint64_t size;
    uint64_t page_memory_handle;
};

static inline void MainTest_GetNextAllocationSize(std::vector<struct allocation_size, mcrt::scalable_allocator<struct allocation_size>> const &config_allocation_sizes, struct drand48_data &rand_buf, uint32_t &out_image_size_width, uint32_t &out_image_size_height);
static inline void MainTest_Allocate(uint32_t image_size_width, uint32_t image_size_height, struct drand48_data &rand_buf, uint64_t &out_alignment, uint64_t &out_size);

int main(int argc, char **argv)
{
    // PerformMainTests
    // 0 = small, 1 = large, 2 = small and large
    uint32_t const small_vs_large_count = 1U;
    // 0 = varying sizes min...max, 1 = set of constant sizes
    uint32_t const constant_sizes_count = 1U;
    // 0 = 100%, additional_operations = 0, 1 = 50%, 2 = 5%, 3 = 95% additional_operations = a lot
    uint32_t const begin_bytes_to_allocate_count = 4U;
    for (uint32_t small_vs_large_index = 0U; small_vs_large_index < small_vs_large_count; ++small_vs_large_index)
    {
        for (uint32_t constant_sizes_index = 0U; constant_sizes_index < constant_sizes_count; ++constant_sizes_index)
        {
            for (size_t begin_bytes_to_allocate_index = 0U; begin_bytes_to_allocate_index < begin_bytes_to_allocate_count; ++begin_bytes_to_allocate_index)
            {
                class gfx_malloc_test malloc_test;

                // Config
                std::vector<struct allocation_size, mcrt::scalable_allocator<struct allocation_size>> config_allocation_sizes;
                uint64_t config_max_bytes_to_allocate;
                uint64_t config_begin_bytes_to_allocate;
                uint32_t config_additional_operation_count;

                if (1U == small_vs_large_index || 2U == small_vs_large_index)
                {
                    config_max_bytes_to_allocate = 4ULL * 1024 * 1024 * 1024; // 4 GB
                }
                else if (0U == small_vs_large_index)
                {
                    config_max_bytes_to_allocate = 4ULL * 1024 * 1024;
                }
                else
                {
                    assert(false);
                }

                // Small
                if (0U == small_vs_large_index || 2U == small_vs_large_index)
                {
                    // Varying size
                    if (0U == constant_sizes_index)
                    {
                        config_allocation_sizes.push_back({4, 4, 32});
                    }
                    // Constant sizes
                    else if (1U == constant_sizes_index)
                    {
                        config_allocation_sizes.push_back({1, 4, 4});
                        config_allocation_sizes.push_back({1, 8, 8});
                        config_allocation_sizes.push_back({1, 16, 16});
                        config_allocation_sizes.push_back({1, 32, 32});
                    }
                    else
                    {
                        assert(false);
                    }
                }
                // Large
                if (1U == small_vs_large_index || 2U == small_vs_large_index)
                {
                    // Varying size
                    if (0U == constant_sizes_index)
                    {
                        config_allocation_sizes.push_back({4, 256, 2048});
                    }
                    // Constant sizes
                    else if (1U == constant_sizes_index)
                    {
                        config_allocation_sizes.push_back({1, 256, 256});
                        config_allocation_sizes.push_back({1, 512, 512});
                        config_allocation_sizes.push_back({1, 1024, 1024});
                        config_allocation_sizes.push_back({1, 2048, 2048});
                    }
                    else
                    {
                        assert(false);
                    }
                }

                if (0U == begin_bytes_to_allocate_index)
                {
                    config_begin_bytes_to_allocate = config_max_bytes_to_allocate;
                    config_additional_operation_count = 0U;
                }
                else if (1U == begin_bytes_to_allocate_index)
                {
                    config_begin_bytes_to_allocate = config_max_bytes_to_allocate * 50 / 100;
                    config_additional_operation_count = 1024;
                }
                else if (2U == begin_bytes_to_allocate_index)
                {
                    config_begin_bytes_to_allocate = config_max_bytes_to_allocate * 5 / 100;
                    config_additional_operation_count = 1024;
                }
                else if (3U == begin_bytes_to_allocate_index)
                {
                    config_begin_bytes_to_allocate = config_max_bytes_to_allocate * 95 / 100;
                    config_additional_operation_count = 1024;
                }
                else
                {
                    assert(false);
                }

                // MainTest
                struct drand48_data rand_buf;
                srand48_r(time(NULL), &rand_buf);

                std::vector<struct test_allocation_result, mcrt::scalable_allocator<struct test_allocation_result>> test_allocation_results;

                // BEGIN ALLOCATIONS
                uint64_t total_allocated_bytes = 0U;
                while (total_allocated_bytes < config_begin_bytes_to_allocate)
                {
                    // MainTest_GetNextAllocationSize
                    uint32_t image_size_width;
                    uint32_t image_size_height;
                    MainTest_GetNextAllocationSize(config_allocation_sizes, rand_buf, image_size_width, image_size_height);

                    // MainTest_Allocate
                    uint64_t alignment;
                    uint64_t size;
                    MainTest_Allocate(image_size_width, image_size_height, rand_buf, alignment, size);

                    void *page_handle;
                    uint64_t offset;
                    uint64_t page_memory_handle = malloc_test.transfer_dst_and_sampled_image_alloc(size, alignment, &page_handle, &offset);
                    test_allocation_results.push_back({page_handle, offset, size, page_memory_handle});
                    total_allocated_bytes += size;
                }

                // ADDITIONAL ALLOCATIONS AND FREES
                uint32_t do_nothing_operation_count = 0U;
                for (size_t i = 0; i < config_additional_operation_count; ++i)
                {
                    // true = allocate, false = free
                    long int r1;
                    int res1 = lrand48_r(&rand_buf, &r1);
                    assert(0 == res1);
                    bool allocate = ((r1 % 2) != 0);

                    if (allocate && (total_allocated_bytes < config_max_bytes_to_allocate))
                    {
                        uint32_t image_size_width;
                        uint32_t image_size_height;
                        MainTest_GetNextAllocationSize(config_allocation_sizes, rand_buf, image_size_width, image_size_height);

                        // MainTest_Allocate
                        uint64_t alignment;
                        uint64_t size;
                        MainTest_Allocate(image_size_width, image_size_height, rand_buf, alignment, size);

                        void *page_handle;
                        uint64_t offset;
                        uint64_t page_memory_handle = malloc_test.transfer_dst_and_sampled_image_alloc(size, alignment, &page_handle, &offset);
                        test_allocation_results.push_back({page_handle, offset, size, page_memory_handle});
                        total_allocated_bytes += size;
                    }
                    else if (!test_allocation_results.empty())
                    {
                        long int r2;
                        int res2 = lrand48_r(&rand_buf, &r2);
                        assert(0 == res2);
                        size_t index_to_free = (r2 % test_allocation_results.size());

                        struct test_allocation_result const &allocation_result = test_allocation_results[index_to_free];
                        malloc_test.transfer_dst_and_sampled_image_free(allocation_result.page_handle, allocation_result.offset, allocation_result.size, allocation_result.page_memory_handle);
                        std::swap(test_allocation_results[index_to_free], test_allocation_results.back());
                        test_allocation_results.pop_back();
                    }
                    else
                    {
                        ++do_nothing_operation_count;
                    }
                }

                // DEALLOCATION
                while (!test_allocation_results.empty())
                {
                    long int r1;
                    int res1 = lrand48_r(&rand_buf, &r1);
                    assert(0 == res1);
                    size_t index_to_free = (r1 % test_allocation_results.size());

                    struct test_allocation_result const &allocation_result = test_allocation_results[index_to_free];
                    malloc_test.transfer_dst_and_sampled_image_free(allocation_result.page_handle, allocation_result.offset, allocation_result.size, allocation_result.page_memory_handle);
                    std::swap(test_allocation_results[index_to_free], test_allocation_results.back());
                    test_allocation_results.pop_back();
                }

                malloc_test.~gfx_malloc_test();
            }
        }
    }

    return 0;
}

static inline void MainTest_GetNextAllocationSize(std::vector<struct allocation_size, mcrt::scalable_allocator<struct allocation_size>> const &config_allocation_sizes, struct drand48_data &rand_buf, uint32_t &out_image_size_width, uint32_t &out_image_size_height)
{
    uint32_t alloc_size_index = 0U;
    {

        uint32_t allocation_size_probability_sum = std::accumulate(
            config_allocation_sizes.begin(),
            config_allocation_sizes.end(),
            static_cast<uint32_t>(0U),
            [](uint32_t sum, struct allocation_size const &alloc_size) {
                return sum + alloc_size.probability;
            });

        long int r1;
        int res1 = lrand48_r(&rand_buf, &r1);
        assert(0 == res1);
        r1 %= allocation_size_probability_sum;
        while (r1 >= config_allocation_sizes[alloc_size_index].probability)
        {
            r1 -= config_allocation_sizes[alloc_size_index].probability;
            ++alloc_size_index;
        }
    }

    struct allocation_size const &alloc_size = config_allocation_sizes[alloc_size_index];
    if (alloc_size.image_size_max == alloc_size.image_size_min)
    {
        out_image_size_width = alloc_size.image_size_max;
        out_image_size_height = alloc_size.image_size_max;
    }
    else
    {
        long int r1;
        int res1 = lrand48_r(&rand_buf, &r1);
        assert(0 == res1);
        out_image_size_width = alloc_size.image_size_min + (r1 % (alloc_size.image_size_max - alloc_size.image_size_min));

        long int r2;
        int res2 = lrand48_r(&rand_buf, &r2);
        assert(0 == res2);
        out_image_size_height = alloc_size.image_size_min + (r1 % (alloc_size.image_size_max - alloc_size.image_size_min));
    }
}

static inline void MainTest_Allocate(uint32_t image_size_width, uint32_t image_size_height, struct drand48_data &rand_buf, uint64_t &out_alignment, uint64_t &out_size)
{

    // simulate the "vkGetImageMemoryRequirements"
    long int r1;
    int res1 = lrand48_r(&rand_buf, &r1);
    assert(0 == res1);
    out_alignment = 1ULL << (8ULL + static_cast<uint64_t>(r1 % 6));

    long int r2;
    int res2 = lrand48_r(&rand_buf, &r2);
    assert(0 == res2);

    long int r3;
    int res3 = lrand48_r(&rand_buf, &r3);
    assert(0 == res3);
    out_size = (r2 % out_alignment) * (r3 % image_size_height) + 4ULL * image_size_width * image_size_height;
}

inline gfx_malloc_test::gfx_malloc_test()
    : m_transfer_dst_and_sampled_image_page_memory_handle_counter(1U)
{
    // 256MB
    this->transfer_dst_and_sampled_image_init(256ULL * 1024ULL * 1024ULL);
}

inline uint64_t gfx_malloc_test::transfer_dst_and_sampled_image_alloc(uint64_t size, uint64_t align, void **out_page_handle, uint64_t *out_offset)
{
    uint64_t page_memory_handle = this->gfx_malloc::transfer_dst_and_sampled_image_alloc(size, align, transfer_dst_and_sampled_image_slob_new_pages, this, out_page_handle, out_offset);
    assert(PAGE_MEMORY_POISON != page_memory_handle);
    return page_memory_handle;
}

inline void gfx_malloc_test::transfer_dst_and_sampled_image_free(void *page_handle, uint64_t offset, uint64_t size, uint64_t page_memory_handle)
{
    return this->gfx_malloc::transfer_dst_and_sampled_image_free(page_handle, offset, size, page_memory_handle, transfer_dst_and_sampled_image_slob_free_pages, this);
}

uint64_t gfx_malloc_test::transfer_dst_and_sampled_image_slob_new_pages(void *_self)
{
    class gfx_malloc_test *self = static_cast<class gfx_malloc_test *>(_self);

    uint64_t page_memory_handle = self->m_transfer_dst_and_sampled_image_page_memory_handle_counter;
    assert(PAGE_MEMORY_POISON != page_memory_handle);
    ++self->m_transfer_dst_and_sampled_image_page_memory_handle_counter;
    auto res = self->m_transfer_dst_and_sampled_image_page_dict.insert(page_memory_handle);
    assert(res.second);
    return page_memory_handle;
}

void gfx_malloc_test::transfer_dst_and_sampled_image_slob_free_pages(uint64_t page_memory_handle, void *_self)
{
    class gfx_malloc_test *self = static_cast<class gfx_malloc_test *>(_self);

    auto iter_find = self->m_transfer_dst_and_sampled_image_page_dict.find(page_memory_handle);
    assert(self->m_transfer_dst_and_sampled_image_page_dict.end() != iter_find);
    self->m_transfer_dst_and_sampled_image_page_dict.erase(iter_find);
}