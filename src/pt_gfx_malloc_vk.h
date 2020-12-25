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

#ifndef _PT_GFX_MALLOC_VK_H_
#define _PT_GFX_MALLOC_VK_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_common.h>
#include <pt_gfx_common.h>
#include <pt_mcrt_common.h>
#include "pt_gfx_malloc_common.h"
#include <vulkan/vulkan.h>
#include <deque>

class gfx_malloc_vk : public gfx_malloc_common
{
    uint32_t m_transfer_src_buffer_memory_index;
    uint32_t m_uniform_buffer_memory_index;
    uint32_t m_color_attachment_and_input_attachment_and_transient_attachment_memory_index;
    uint32_t m_color_attachment_and_sampled_image_memory_index;
    uint32_t m_transfer_dst_and_sampled_image_memory_index;
    //uint32_t memory_index_elem[9];
    //static_assert(PT_GFX_MALLOC_USAGE_RANGE_SIZE == (sizeof(memory_index_elem) / sizeof(memory_index_elem[0])), "PT_GFX_MALLOC_USAGE_RANGE_SIZE == (sizeof(memory_index_elem) / sizeof(memory_index_elem[0]))");

    class gfx_malloc_slob_page_vk_t : public gfx_malloc_slob_page_common_t
    {
        VkDeviceMemory m_page;
    };

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

    std::deque<gfx_malloc_slob_page_vk_t, mcrt::scalable_allocator<gfx_malloc_slob_page_vk_t>> m_free_slob_small;
    std::deque<gfx_malloc_slob_page_vk_t, mcrt::scalable_allocator<gfx_malloc_slob_page_vk_t>> m_free_slob_medium;
    std::deque<gfx_malloc_slob_page_vk_t, mcrt::scalable_allocator<gfx_malloc_slob_page_vk_t>> m_free_slob_large;

    //uint32_t malloc_usage_to_memory_type_index(enum gfx_malloc_usage_t malloc_usage);

    void *alloc_uniform_buffer(size_t size) override;

protected:
    bool init(
        VkPhysicalDevice physical_device,
        PFN_vkGetPhysicalDeviceMemoryProperties vk_get_physical_device_memory_properties,
        VkDevice device,
        PFN_vkGetBufferMemoryRequirements vk_get_buffer_memory_requirements,
        PFN_vkGetImageMemoryRequirements vk_get_image_memory_requirements);
};

#endif