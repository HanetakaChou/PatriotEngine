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

    class slob_page_vk : public slob_page
    {
    public:
        VkDeviceMemory m_page;
        inline slob_page_vk(VkDeviceSize size, VkDeviceMemory page);
    };

    class slob_vk : public slob
    {
        class gfx_connection_vk *m_gfx_api_vk;
        VkDeviceSize m_page_size;
        uint32_t m_memory_index;
        class slob_page *new_pages() override;

    public:
        inline slob_vk(uint64_t slob_break1, uint64_t slob_break2, VkDeviceSize page_size, uint32_t memory_index, class gfx_connection_vk *gfx_api_vk);
    };

    slob_vk *m_transfer_dst_and_sampled_image_slob;

    //VkDeviceSize m_transfer_dst_and_sampled_image_slob_break1; //(page_size * 256/*SLOB_BREAK1*/) / 4096
    //VkDeviceSize m_transfer_dst_and_sampled_image_slob_break2; //(page_size * 1024/*SLOB_BREAK2*/) / 4096
    //slob_page_vk_t *m_transfer_dst_and_sampled_image_list_head_free_slob_small;
    //slob_page_vk_t *m_transfer_dst_and_sampled_image_list_head_free_slob_medium;
    //slob_page_vk_t *m_transfer_dst_and_sampled_image_list_head_free_slob_large;

#ifndef NDEBUG
    bool m_transfer_dst_and_sampled_image_slob_list_head_lock;
#endif
    static inline void transfer_dst_and_sampled_image_slob_lock_list_head(class gfx_malloc_common *self)
    {
        assert(mcrt_atomic_load(&static_cast<class gfx_malloc_vk *>(self)->m_transfer_dst_and_sampled_image_slob_list_head_lock) == false);
#ifndef NDEBUG
        mcrt_atomic_store(&static_cast<class gfx_malloc_vk *>(self)->m_transfer_dst_and_sampled_image_slob_list_head_lock, true);
#endif
    }

    static inline void transfer_dst_and_sampled_image_slob_unlock_list_head(class gfx_malloc_common *self)
    {
        assert(mcrt_atomic_load(&static_cast<class gfx_malloc_vk *>(self)->m_transfer_dst_and_sampled_image_slob_list_head_lock) == true);
#ifndef NDEBUG
        mcrt_atomic_store(&static_cast<class gfx_malloc_vk *>(self)->m_transfer_dst_and_sampled_image_slob_list_head_lock, false);
#endif
    }
    //VkDeviceSize m_transfer_dst_and_sampled_image_page_size;
    //VkDeviceSize m_transfer_dst_and_sampled_image_heap_size;
    //uint32_t m_transfer_dst_and_sampled_image_memory_index;
    //uint32_t m_transfer_dst_and_sampled_image_heap_index;

    //uint32_t malloc_usage_to_memory_type_index(enum gfx_malloc_usage_t malloc_usage);

    //routed into memory index //may be the same index
    //MT-safe???

    void *alloc_uniform_buffer(size_t size) override;

protected:
    bool init(
        VkPhysicalDevice physical_device,
        PFN_vkGetPhysicalDeviceMemoryProperties vk_get_physical_device_memory_properties);

public:
    uint64_t alloc_transfer_dst_and_sampled_image(size_t size, size_t alignment, void **out_device_memory) override;

    //inline uint64_t alloc_transfer_dst_and_sampled_image(VkMemoryRequirements const *memory_requirements, VkDeviceMemory *out_device_memory)
    //{
    //    assert(((1U << m_transfer_dst_and_sampled_image_memory_index) & memory_requirements->memoryTypeBits) != 0);
    //     static_assert(sizeof(void *) == sizeof(VkDeviceMemory), "");
    //    return this->gfx_malloc_vk::alloc_transfer_dst_and_sampled_image(memory_requirements->size, memory_requirements->alignment, reinterpret_cast<void **>(out_device_memory));
    //}
};

#endif