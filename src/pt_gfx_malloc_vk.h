//
// Copyright (C) YuqiaoZhang(HanetakaYuminaga)
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#ifndef _PT_GFX_MALLOC_VK_H_
#define _PT_GFX_MALLOC_VK_H_ 1

#include <stddef.h>
#include <stdint.h>
#include "pt_gfx_malloc.h"
#include "pt_gfx_device_vk.h"
#include <vulkan/vulkan.h>

class gfx_malloc_vk : public gfx_malloc
{
    // constant buffer
    VkDeviceSize m_uniform_buffer_size;
    VkBuffer m_uniform_buffer;
    VkDeviceMemory m_uniform_buffer_device_memory;
    void *m_uniform_buffer_device_memory_pointer;

    // staging buffer
    VkDeviceSize m_transfer_src_buffer_size;
    VkBuffer m_transfer_src_buffer;
    VkDeviceMemory m_transfer_src_buffer_device_memory;
    void *m_transfer_src_buffer_device_memory_pointer;

    // framebuffer attachment
    uint32_t m_color_attachment_and_input_attachment_and_transient_attachment_memory_index;
    uint32_t m_color_attachment_and_sampled_image_memory_index;
    VkFormat m_format_depth;
    VkFormat m_format_depth_stencil;
    uint32_t m_depth_stencil_attachment_and_transient_attachment_memory_index;

    // We never use a buffer as both the vertex buffer and the index buffer
    uint32_t m_transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_memory_index;
    VkDeviceSize m_transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_page_size;

    struct transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_slob_new_pages_callback_data
    {
        class gfx_device_vk *m_gfx_device;
        uint32_t m_transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_memory_index;
        VkDeviceSize m_transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_page_size;
    };
    static uint64_t transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_slob_new_pages(void *);
    static void transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_slob_free_pages(uint64_t, void *);

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
    uint32_t m_transfer_dst_and_sampled_image_memory_index;
    VkDeviceSize m_transfer_dst_and_sampled_image_page_size;

    struct transfer_dst_and_sampled_image_slob_new_pages_callback_data
    {
        class gfx_device_vk *m_gfx_device;
        uint32_t m_transfer_dst_and_sampled_image_memory_index;
        VkDeviceSize m_transfer_dst_and_sampled_image_page_size;
    };
    static uint64_t transfer_dst_and_sampled_image_slob_new_pages(void *);
    static void transfer_dst_and_sampled_image_slob_free_pages(uint64_t, void *);

public:
    gfx_malloc_vk();
    bool init(class gfx_device_vk *gfx_device);
    void destroy();
    ~gfx_malloc_vk();

    inline void *uniform_buffer_pointer() { return this->m_uniform_buffer_device_memory_pointer; }

    inline void *transfer_src_buffer_pointer() { return this->m_transfer_src_buffer_device_memory_pointer; }
    inline VkDeviceSize transfer_src_buffer_size() { return this->m_transfer_src_buffer_size; }
    inline VkBuffer transfer_src_buffer() { return this->m_transfer_src_buffer; }

    inline VkFormat format_depth() { return this->m_format_depth; }
    inline VkFormat format_depth_stencil() { return this->m_format_depth_stencil; }
    inline uint32_t depth_stencil_attachment_and_transient_attachment_memory_index() { return this->m_depth_stencil_attachment_and_transient_attachment_memory_index; }

    VkDeviceMemory transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_alloc(class gfx_device_vk *gfx_device, VkMemoryRequirements const *memory_requirements, void **out_page_handle, uint64_t *out_offset, uint64_t *out_size);
    void transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_free(class gfx_device_vk *gfx_device, void *page_handle, uint64_t offset, uint64_t size, VkDeviceMemory device_memory);

    VkDeviceMemory transfer_dst_and_sampled_image_alloc(class gfx_device_vk *gfx_device, VkMemoryRequirements const *memory_requirements, void **out_page_handle, uint64_t *out_offset, uint64_t *out_size);
    void transfer_dst_and_sampled_image_free(class gfx_device_vk *gfx_device, void *page_handle, uint64_t offset, uint64_t size, VkDeviceMemory device_memory);
};

#endif