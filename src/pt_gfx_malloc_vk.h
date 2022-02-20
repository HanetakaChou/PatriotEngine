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
#include "pt_gfx_device_vk.h"
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

struct gfx_malloc_allocation_vk
{
    VmaAllocation m_vma_allocation;
};

class gfx_malloc_vk
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

    // asset
    VmaAllocator m_asset_allocator;
    uint32_t m_asset_vertex_buffer_memory_index;
    uint32_t m_asset_index_buffer_memory_index;
    uint32_t m_asset_image_memory_index;

public:
    gfx_malloc_vk();
    bool init(class gfx_device_vk *gfx_device);
    void destroy(class gfx_device_vk *gfx_device);
    ~gfx_malloc_vk();

    inline void *uniform_buffer_pointer() { return this->m_uniform_buffer_device_memory_pointer; }

    inline void *transfer_src_buffer_pointer() { return this->m_transfer_src_buffer_device_memory_pointer; }
    inline VkDeviceSize transfer_src_buffer_size() { return this->m_transfer_src_buffer_size; }
    inline VkBuffer transfer_src_buffer() { return this->m_transfer_src_buffer; }

    inline VkFormat format_depth() { return this->m_format_depth; }
    inline VkFormat format_depth_stencil() { return this->m_format_depth_stencil; }
    inline uint32_t depth_stencil_attachment_and_transient_attachment_memory_index() { return this->m_depth_stencil_attachment_and_transient_attachment_memory_index; }

    bool asset_vertex_buffer_alloc(class gfx_device_vk* gfx_device, VkBufferCreateInfo const* buffer_create_info, VkBuffer* buffer, gfx_malloc_allocation_vk* allocation);
    void asset_vertex_buffer_free(class gfx_device_vk* gfx_device, VkBuffer buffer, struct gfx_malloc_allocation_vk const* allocation);

    bool asset_index_buffer_alloc(class gfx_device_vk* gfx_device, VkBufferCreateInfo const* buffer_create_info, VkBuffer* buffer, gfx_malloc_allocation_vk* allocation);
    void asset_index_buffer_free(class gfx_device_vk* gfx_device, VkBuffer buffer, struct gfx_malloc_allocation_vk const* allocation);

    bool asset_image_alloc(class gfx_device_vk* gfx_device, VkImageCreateInfo const* image_create_info, VkImage* image, struct gfx_malloc_allocation_vk* allocation);
    void asset_image_free(class gfx_device_vk* gfx_device, VkImage image, struct gfx_malloc_allocation_vk const* allocation);
};

#endif