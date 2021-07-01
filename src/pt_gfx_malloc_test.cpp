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

    uint64_t m_transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_page_memory_handle_counter;
    std::set<uint64_t, std::less<uint64_t>, mcrt::scalable_allocator<uint64_t>> m_transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_page_dict;
    static uint64_t transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_slob_new_pages(void *);
    static void transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_slob_free_pages(uint64_t, void *);

    uint64_t m_transfer_dst_and_sampled_image_page_memory_handle_counter;
    std::set<uint64_t, std::less<uint64_t>, mcrt::scalable_allocator<uint64_t>> m_transfer_dst_and_sampled_image_page_dict;
    static uint64_t transfer_dst_and_sampled_image_slob_new_pages(void *);
    static void transfer_dst_and_sampled_image_slob_free_pages(uint64_t, void *);

public:
    inline gfx_malloc_test();

    inline uint64_t transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_alloc(uint64_t size, uint64_t align, void **out_page_handle, uint64_t *out_offset);
    inline void transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_free(void *page_handle, uint64_t offset, uint64_t size, uint64_t page_memory_handle);

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
    uint32_t buffers_vs_images; // 0 = buffers, 1 = images
    uint32_t probability;
    uint32_t buffer_size_min;
    uint32_t buffer_size_max;
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

static inline void MainTest_GetNextAllocationSize(std::vector<struct allocation_size, mcrt::scalable_allocator<struct allocation_size>> const &config_allocation_sizes, unsigned &rand_buf, uint32_t &out_buffers_vs_images, uint32_t &out_buffer_size, uint32_t &out_image_size_width, uint32_t &out_image_size_height);
static inline void MainTest_Allocate(uint32_t buffer_size, unsigned &rand_buf, uint64_t &out_alignment, uint64_t &out_size);
static inline void MainTest_Allocate(uint32_t image_size_width, uint32_t image_size_height, unsigned &rand_buf, uint64_t &out_alignment, uint64_t &out_size);

int main(int argc, char **argv)
{
    // PerformMainTests
    // 0 = buffers, 1 = images, 2 = buffers and images
    size_t buffers_vs_images_count = 2;
    // 0 = small, 1 = large, 2 = small and large
    uint32_t const small_vs_large_count = 3U;
    // 0 = varying sizes min...max, 1 = set of constant sizes
    uint32_t const constant_sizes_count = 1U;
    // 0 = 100%, additional_operations = 0, 1 = 50%, 2 = 5%, 3 = 95% additional_operations = a lot
    uint32_t const begin_bytes_to_allocate_count = 4U;
    for (uint32_t buffers_vs_images_index = 0U; buffers_vs_images_index < buffers_vs_images_count; ++buffers_vs_images_index)
    {
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

                    // Buffers present
                    if (buffers_vs_images_index == 0 || buffers_vs_images_index == 2)
                    {
                        // Small
                        if (0U == small_vs_large_index || 2U == small_vs_large_index)
                        {
                            // Varying size
                            if (0U == constant_sizes_index)
                            {
                                config_allocation_sizes.push_back({0, 4, 16, 1024, 0, 0});
                            }
                            // Constant sizes
                            else if (1U == constant_sizes_index)
                            {
                                config_allocation_sizes.push_back({0, 1, 16, 16, 0, 0});
                                config_allocation_sizes.push_back({0, 1, 64, 64, 0, 0});
                                config_allocation_sizes.push_back({0, 1, 256, 256, 0, 0});
                                config_allocation_sizes.push_back({0, 1, 1024, 1024, 0, 0});
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
                                config_allocation_sizes.push_back({0, 4, 0x10000, 0xA00000, 0, 0}); // 64 KB ... 10 MB
                            }
                            // Constant sizes
                            else if (1U == constant_sizes_index)
                            {
                                config_allocation_sizes.push_back({0, 1, 0x10000, 0x10000, 0, 0});
                                config_allocation_sizes.push_back({0, 1, 0x80000, 0x80000, 0, 0});
                                config_allocation_sizes.push_back({0, 1, 0x200000, 0x200000, 0, 0});
                                config_allocation_sizes.push_back({0, 1, 0xA00000, 0xA00000, 0, 0});
                            }
                            else
                            {
                                assert(false);
                            }
                        }
                    }

                    // Images present
                    if (buffers_vs_images_index == 1 || buffers_vs_images_index == 2)
                    {
                        // Small
                        if (0U == small_vs_large_index || 2U == small_vs_large_index)
                        {
                            // Varying size
                            if (0U == constant_sizes_index)
                            {
                                config_allocation_sizes.push_back({1, 4, 0, 0, 4, 32});
                            }
                            // Constant sizes
                            else if (1U == constant_sizes_index)
                            {
                                config_allocation_sizes.push_back({1, 1, 0, 0, 4, 4});
                                config_allocation_sizes.push_back({1, 1, 0, 0, 8, 8});
                                config_allocation_sizes.push_back({1, 1, 0, 0, 16, 16});
                                config_allocation_sizes.push_back({1, 1, 0, 0, 32, 32});
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
                                config_allocation_sizes.push_back({1, 4, 0, 0, 256, 2048});
                            }
                            // Constant sizes
                            else if (1U == constant_sizes_index)
                            {
                                config_allocation_sizes.push_back({1, 1, 0, 0, 256, 256});
                                config_allocation_sizes.push_back({1, 1, 0, 0, 512, 512});
                                config_allocation_sizes.push_back({1, 1, 0, 0, 1024, 1024});
                                config_allocation_sizes.push_back({1, 1, 0, 0, 2048, 2048});
                            }
                            else
                            {
                                assert(false);
                            }
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
                    unsigned rand_buf = (unsigned)time(NULL);

                    std::vector<struct test_allocation_result, mcrt::scalable_allocator<struct test_allocation_result>> test_buffer_allocation_results;
                    std::vector<struct test_allocation_result, mcrt::scalable_allocator<struct test_allocation_result>> test_image_allocation_results;

                    // BEGIN ALLOCATIONS
                    uint64_t total_allocated_bytes = 0U;
                    while (total_allocated_bytes < config_begin_bytes_to_allocate)
                    {
                        // MainTest_GetNextAllocationSize
                        uint32_t buffers_vs_images;
                        uint32_t buffer_size;
                        uint32_t image_size_width;
                        uint32_t image_size_height;
                        MainTest_GetNextAllocationSize(config_allocation_sizes, rand_buf, buffers_vs_images, buffer_size, image_size_width, image_size_height);

                        if (0U == buffers_vs_images)
                        {
                            // MainTest_Allocate
                            uint64_t alignment;
                            uint64_t size;
                            MainTest_Allocate(buffer_size, rand_buf, alignment, size);

                            void *page_handle;
                            uint64_t offset;
                            uint64_t page_memory_handle = malloc_test.transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_alloc(size, alignment, &page_handle, &offset);
                            test_buffer_allocation_results.push_back({page_handle, offset, size, page_memory_handle});
                            total_allocated_bytes += size;
                        }
                        else if (1U == buffers_vs_images)
                        {
                            // MainTest_Allocate
                            uint64_t alignment;
                            uint64_t size;
                            MainTest_Allocate(image_size_width, image_size_height, rand_buf, alignment, size);

                            void *page_handle;
                            uint64_t offset;
                            uint64_t page_memory_handle = malloc_test.transfer_dst_and_sampled_image_alloc(size, alignment, &page_handle, &offset);
                            test_image_allocation_results.push_back({page_handle, offset, size, page_memory_handle});
                            total_allocated_bytes += size;
                        }
                        else
                        {
                            assert(false);
                        }
                    }

                    // ADDITIONAL ALLOCATIONS AND FREES
                    uint32_t do_nothing_operation_count = 0U;
                    for (size_t i = 0; i < config_additional_operation_count; ++i)
                    {
                        // true = allocate, false = free
                        long int r1 = rand_r(&rand_buf);
                        bool allocate = ((r1 % 2) != 0);

                        if (allocate && (total_allocated_bytes < config_max_bytes_to_allocate))
                        {
                            uint32_t buffers_vs_images;
                            uint32_t buffer_size;
                            uint32_t image_size_width;
                            uint32_t image_size_height;
                            MainTest_GetNextAllocationSize(config_allocation_sizes, rand_buf, buffers_vs_images, buffer_size, image_size_width, image_size_height);

                            if (0U == buffers_vs_images)
                            {
                                // MainTest_Allocate
                                uint64_t alignment;
                                uint64_t size;
                                MainTest_Allocate(buffer_size, rand_buf, alignment, size);

                                void *page_handle;
                                uint64_t offset;
                                uint64_t page_memory_handle = malloc_test.transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_alloc(size, alignment, &page_handle, &offset);
                                test_buffer_allocation_results.push_back({page_handle, offset, size, page_memory_handle});
                                total_allocated_bytes += size;
                            }
                            else if (1U == buffers_vs_images)
                            {
                                // MainTest_Allocate
                                uint64_t alignment;
                                uint64_t size;
                                MainTest_Allocate(image_size_width, image_size_height, rand_buf, alignment, size);

                                void *page_handle;
                                uint64_t offset;
                                uint64_t page_memory_handle = malloc_test.transfer_dst_and_sampled_image_alloc(size, alignment, &page_handle, &offset);
                                test_image_allocation_results.push_back({page_handle, offset, size, page_memory_handle});
                                total_allocated_bytes += size;
                            }
                            else
                            {
                                assert(false);
                            }
                        }
                        else if (!test_buffer_allocation_results.empty() || !test_image_allocation_results.empty())
                        {
                            uint32_t buffers_vs_images = uint32_t(-1);
                            if (!test_buffer_allocation_results.empty() && !test_image_allocation_results.empty())
                            {
                                long int r3 = rand_r(&rand_buf);
                                buffers_vs_images = (r3 % 2);
                            }
                            else if (!test_buffer_allocation_results.empty())
                            {
                                buffers_vs_images = 0U;
                            }
                            else if (!test_image_allocation_results.empty())
                            {
                                buffers_vs_images = 1U;
                            }
                            else
                            {
                                assert(false);
                            }

                            if (0U == buffers_vs_images)
                            {
                                long int r2 = rand_r(&rand_buf);
                                size_t index_to_free = (r2 % test_buffer_allocation_results.size());

                                struct test_allocation_result const &allocation_result = test_buffer_allocation_results[index_to_free];
                                malloc_test.transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_free(allocation_result.page_handle, allocation_result.offset, allocation_result.size, allocation_result.page_memory_handle);
                                std::swap(test_buffer_allocation_results[index_to_free], test_buffer_allocation_results.back());
                                test_buffer_allocation_results.pop_back();
                            }
                            else if (1U == buffers_vs_images)
                            {
                                long int r2 = rand_r(&rand_buf);
                                size_t index_to_free = (r2 % test_image_allocation_results.size());

                                struct test_allocation_result const &allocation_result = test_image_allocation_results[index_to_free];
                                malloc_test.transfer_dst_and_sampled_image_free(allocation_result.page_handle, allocation_result.offset, allocation_result.size, allocation_result.page_memory_handle);
                                std::swap(test_image_allocation_results[index_to_free], test_image_allocation_results.back());
                                test_image_allocation_results.pop_back();
                            }
                            else
                            {
                                assert(false);
                            }
                        }
                        else
                        {
                            ++do_nothing_operation_count;
                        }
                    }

                    // DEALLOCATION
                    while (!test_buffer_allocation_results.empty())
                    {
                        long int r2 = rand_r(&rand_buf);
                        size_t index_to_free = (r2 % test_buffer_allocation_results.size());

                        struct test_allocation_result const &allocation_result = test_buffer_allocation_results[index_to_free];
                        malloc_test.transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_free(allocation_result.page_handle, allocation_result.offset, allocation_result.size, allocation_result.page_memory_handle);
                        std::swap(test_buffer_allocation_results[index_to_free], test_buffer_allocation_results.back());
                        test_buffer_allocation_results.pop_back();
                    }

                    while (!test_image_allocation_results.empty())
                    {
                        long int r1 = rand_r(&rand_buf);
                        size_t index_to_free = (r1 % test_image_allocation_results.size());

                        struct test_allocation_result const &allocation_result = test_image_allocation_results[index_to_free];
                        malloc_test.transfer_dst_and_sampled_image_free(allocation_result.page_handle, allocation_result.offset, allocation_result.size, allocation_result.page_memory_handle);
                        std::swap(test_image_allocation_results[index_to_free], test_image_allocation_results.back());
                        test_image_allocation_results.pop_back();
                    }

                    // implicit
                    // malloc_test.~gfx_malloc_test();
                }
            }
        }
    }

    return 0;
}

static inline void MainTest_GetNextAllocationSize(std::vector<struct allocation_size, mcrt::scalable_allocator<struct allocation_size>> const &config_allocation_sizes, unsigned &rand_buf, uint32_t &out_buffers_vs_images, uint32_t &out_buffer_size, uint32_t &out_image_size_width, uint32_t &out_image_size_height)
{
    uint32_t alloc_size_index = 0U;
    {

        uint32_t allocation_size_probability_sum = std::accumulate(
            config_allocation_sizes.begin(),
            config_allocation_sizes.end(),
            static_cast<uint32_t>(0U),
            [](uint32_t sum, struct allocation_size const &alloc_size)
            {
                return sum + alloc_size.probability;
            });

        long int r1 = rand_r(&rand_buf);
        r1 %= allocation_size_probability_sum;
        while (r1 >= config_allocation_sizes[alloc_size_index].probability)
        {
            r1 -= config_allocation_sizes[alloc_size_index].probability;
            ++alloc_size_index;
        }
    }

    struct allocation_size const &alloc_size = config_allocation_sizes[alloc_size_index];
    out_buffers_vs_images = alloc_size.buffers_vs_images;
    if (0U == alloc_size.buffers_vs_images)
    {
        if (alloc_size.buffer_size_max == alloc_size.buffer_size_min)
        {
            out_buffer_size = alloc_size.buffer_size_max;
        }
        else
        {
            long int r1 = rand_r(&rand_buf);
            out_buffer_size = alloc_size.buffer_size_min + (r1 % (alloc_size.buffer_size_max - alloc_size.buffer_size_min));
        }

        assert(0 != out_buffer_size);
    }
    else if (1U == alloc_size.buffers_vs_images)
    {
        if (alloc_size.image_size_max == alloc_size.image_size_min)
        {
            out_image_size_width = alloc_size.image_size_max;
            out_image_size_height = alloc_size.image_size_max;
        }
        else
        {
            long int r1 = rand_r(&rand_buf);
            out_image_size_width = alloc_size.image_size_min + (r1 % (alloc_size.image_size_max - alloc_size.image_size_min));

            long int r2 = rand_r(&rand_buf);
            out_image_size_height = alloc_size.image_size_min + (r1 % (alloc_size.image_size_max - alloc_size.image_size_min));
        }
    }
    else
    {
        assert(false);
    }
}

static inline void MainTest_Allocate(uint32_t buffer_size, unsigned &rand_buf, uint64_t &out_alignment, uint64_t &out_size)
{
    // simulate the "vkGetBufferMemoryRequirements"
    long int r1 = rand_r(&rand_buf);
    out_alignment = 1ULL << (8ULL + static_cast<uint64_t>(r1 % 6));

    long int r2 = rand_r(&rand_buf);
    out_size = (r2 % out_alignment) + buffer_size;
}

static inline void MainTest_Allocate(uint32_t image_size_width, uint32_t image_size_height, unsigned &rand_buf, uint64_t &out_alignment, uint64_t &out_size)
{

    // simulate the "vkGetImageMemoryRequirements"
    long int r1 = rand_r(&rand_buf);
    out_alignment = 1ULL << (8ULL + static_cast<uint64_t>(r1 % 6));

    long int r2 = rand_r(&rand_buf);
    long int r3 = rand_r(&rand_buf);
    out_size = (r2 % out_alignment) * (r3 % image_size_height) + 4ULL * image_size_width * image_size_height;
}

uint64_t gfx_malloc_test::transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_slob_new_pages(void *_self)
{
    class gfx_malloc_test *self = static_cast<class gfx_malloc_test *>(_self);

    uint64_t page_memory_handle = self->m_transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_page_memory_handle_counter;
    assert(PAGE_MEMORY_HANDLE_POISON != page_memory_handle);
    ++self->m_transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_page_memory_handle_counter;
    auto res = self->m_transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_page_dict.insert(page_memory_handle);
    assert(res.second);
    return page_memory_handle;
}

void gfx_malloc_test::transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_slob_free_pages(uint64_t page_memory_handle, void *_self)
{
    class gfx_malloc_test *self = static_cast<class gfx_malloc_test *>(_self);

    auto iter_find = self->m_transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_page_dict.find(page_memory_handle);
    assert(self->m_transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_page_dict.end() != iter_find);
    self->m_transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_page_dict.erase(iter_find);
}

uint64_t gfx_malloc_test::transfer_dst_and_sampled_image_slob_new_pages(void *_self)
{
    class gfx_malloc_test *self = static_cast<class gfx_malloc_test *>(_self);

    uint64_t page_memory_handle = self->m_transfer_dst_and_sampled_image_page_memory_handle_counter;
    assert(PAGE_MEMORY_HANDLE_POISON != page_memory_handle);
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

inline gfx_malloc_test::gfx_malloc_test()
    : m_transfer_dst_and_sampled_image_page_memory_handle_counter(1U)
{
    // 256MB
    this->transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_init(256ULL * 1024ULL * 1024ULL);

    // 256MB
    this->transfer_dst_and_sampled_image_init(256ULL * 1024ULL * 1024ULL);
}

inline uint64_t gfx_malloc_test::transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_alloc(uint64_t size, uint64_t align, void **out_page_handle, uint64_t *out_offset)
{
    assert(0 != size);
    uint64_t page_memory_handle = this->gfx_malloc::transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_alloc(size, align, transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_slob_new_pages, this, out_page_handle, out_offset);
    assert(PAGE_MEMORY_HANDLE_POISON != page_memory_handle);
    return page_memory_handle;
}

inline void gfx_malloc_test::transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_free(void *page_handle, uint64_t offset, uint64_t size, uint64_t page_memory_handle)
{
    return this->gfx_malloc::transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_free(page_handle, offset, size, page_memory_handle, transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_slob_free_pages, this);
}

inline uint64_t gfx_malloc_test::transfer_dst_and_sampled_image_alloc(uint64_t size, uint64_t align, void **out_page_handle, uint64_t *out_offset)
{
    uint64_t page_memory_handle = this->gfx_malloc::transfer_dst_and_sampled_image_alloc(size, align, transfer_dst_and_sampled_image_slob_new_pages, this, out_page_handle, out_offset);
    assert(PAGE_MEMORY_HANDLE_POISON != page_memory_handle);
    return page_memory_handle;
}

inline void gfx_malloc_test::transfer_dst_and_sampled_image_free(void *page_handle, uint64_t offset, uint64_t size, uint64_t page_memory_handle)
{
    return this->gfx_malloc::transfer_dst_and_sampled_image_free(page_handle, offset, size, page_memory_handle, transfer_dst_and_sampled_image_slob_free_pages, this);
}