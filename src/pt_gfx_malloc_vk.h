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
#include "pt_gfx_malloc.h"
#include <vulkan/vulkan.h>

class gfx_malloc_vk : public gfx_malloc
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

    VkDeviceMemory internal_transfer_dst_and_sampled_image_alloc(VkMemoryRequirements const *memory_requirements, uint64_t *out_offset, void **out_gfx_malloc_page);

    class slob_page_vk : public slob_page
    {
        VkDeviceMemory m_page;

    public:
        inline slob_page_vk(VkDeviceSize size, VkDeviceMemory page);
        friend VkDeviceMemory gfx_malloc_vk::internal_transfer_dst_and_sampled_image_alloc(VkMemoryRequirements const *memory_requirements, uint64_t *out_offset, void **out_gfx_malloc_page);
    };

    class slob_vk : public slob
    {
        class gfx_connection_vk *m_gfx_api_vk;
        uint32_t m_memory_index;
        class slob_page *new_pages() override;

    public:
        inline slob_vk();
        inline void init(uint64_t slob_page_size, uint32_t memory_index, class gfx_connection_vk *gfx_api_vk);
        friend VkDeviceMemory gfx_malloc_vk::internal_transfer_dst_and_sampled_image_alloc(VkMemoryRequirements const *memory_requirements, uint64_t *out_offset, void **out_gfx_malloc_page);
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
    slob_vk m_transfer_dst_and_sampled_image_slob;

    void *alloc_uniform_buffer(size_t size) override;

protected:
    gfx_malloc_vk();

    bool init(
        VkPhysicalDevice physical_device,
        PFN_vkGetPhysicalDeviceMemoryProperties vk_get_physical_device_memory_properties);

public:
    inline VkDeviceMemory transfer_dst_and_sampled_image_alloc(VkMemoryRequirements const *memory_requirements, uint64_t *out_offset, void **out_gfx_malloc_page)
    {
        return internal_transfer_dst_and_sampled_image_alloc(memory_requirements, out_offset, out_gfx_malloc_page);
    }
};

#endif