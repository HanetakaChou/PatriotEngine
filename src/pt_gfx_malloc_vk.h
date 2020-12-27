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
#include <pt_mcrt_atomic.h>
#include "pt_gfx_malloc_common.h"
#include <vulkan/vulkan.h>

class gfx_malloc_vk : public gfx_malloc_common
{
    class gfx_connection_vk *m_gfx_api_vk;

    enum VkFormat m_format_depth;
    enum VkFormat m_format_depth_stencil;

    uint32_t m_transfer_src_buffer_memory_index;
    //VkDeviceSize m_transfer_src_buffer_ringbuffer_begin;
    //VkDeviceSize m_transfer_src_buffer_ringbuffer_end;

    uint32_t m_uniform_buffer_memory_index;
    //VkDeviceSize m_uniform_buffer_ringbuffer_begin;
    //VkDeviceSize m_uniform_buffer_ringbuffer_end;

    uint32_t m_color_attachment_and_input_attachment_and_transient_attachment_memory_index;
    uint32_t m_color_attachment_and_sampled_image_memory_index;
    uint32_t m_depth_stencil_attachment_and_transient_attachment_memory_index;

    uint32_t m_transfer_dst_and_vertex_buffer_memory_index;
    uint32_t m_transfer_dst_and_index_buffer_memory_index;

    //uint32_t memory_index_elem[9];
    //static_assert(PT_GFX_MALLOC_USAGE_RANGE_SIZE == (sizeof(memory_index_elem) / sizeof(memory_index_elem[0])), "PT_GFX_MALLOC_USAGE_RANGE_SIZE == (sizeof(memory_index_elem) / sizeof(memory_index_elem[0]))");

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

    class slob_page_vk_t : public slob_page_t
    {
        VkDeviceMemory m_page;

    public:
        inline slob_page_vk_t(VkDeviceSize size, VkDeviceMemory page) : slob_page_t(size), m_page(page)
        {
        }
    };

    VkDeviceSize m_transfer_dst_and_sampled_image_slob_break1; //(page_size * 256/*SLOB_BREAK1*/) / 4096
    VkDeviceSize m_transfer_dst_and_sampled_image_slob_break2; //(page_size * 1024/*SLOB_BREAK2*/) / 4096
    slob_page_vk_t *m_transfer_dst_and_sampled_image_list_head_free_slob_small;
    slob_page_vk_t *m_transfer_dst_and_sampled_image_list_head_free_slob_medium;
    slob_page_vk_t *m_transfer_dst_and_sampled_image_list_head_free_slob_large;

#ifndef NDEBUG
    bool m_transfer_dst_and_sampled_image_slob_list_head_lock;
#endif
    inline void transfer_dst_and_sampled_image_slob_lock_list_head()
    {
        assert(mcrt_atomic_load(&m_transfer_dst_and_sampled_image_slob_list_head_lock) == false);
#ifndef NDEBUG
        mcrt_atomic_store(&m_transfer_dst_and_sampled_image_slob_list_head_lock, true);
#endif
    }

    inline void transfer_dst_and_sampled_image_slob_unlock_list_head()
    {
        assert(mcrt_atomic_load(&m_transfer_dst_and_sampled_image_slob_list_head_lock) == true);
#ifndef NDEBUG
        mcrt_atomic_store(&m_transfer_dst_and_sampled_image_slob_list_head_lock, false);
#endif
    }
    VkDeviceSize m_transfer_dst_and_sampled_image_page_size;
    //VkDeviceSize m_transfer_dst_and_sampled_image_heap_size;
    uint32_t m_transfer_dst_and_sampled_image_memory_index;
    //uint32_t m_transfer_dst_and_sampled_image_heap_index;

    //uint32_t malloc_usage_to_memory_type_index(enum gfx_malloc_usage_t malloc_usage);

    //routed into memory index //may be the same index
    //MT-safe???
    static class slob_page_t *transfer_dst_and_sampled_image_slob_new_pages(class gfx_malloc_common *self);

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