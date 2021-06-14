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

#ifndef _PT_GFX_CONNECTION_VK_H_
#define _PT_GFX_CONNECTION_VK_H_ 1

#include <stddef.h>
#include <stdint.h>
#include "pt_gfx_connection_common.h"
#include "pt_gfx_api_vk.h"
#include "pt_gfx_malloc.h"
#include "pt_gfx_malloc_vk.h"
#include <vulkan/vulkan.h>

class gfx_connection_vk : public gfx_connection_common
{
    class gfx_api_vk m_api_vk;
    class gfx_malloc_vk m_malloc;

    //deque
    struct async_resource_job_result
    {
        VkFence m_fence;
        //m_vertexbuffers
        //m_indexbuffers
        //m_textures
        uint64_t m_ringbuffer_begin;
        uint64_t m_ringbuffer_end;
    };

    // The unique uniform buffer.
    // \[Gruen 2015\] [Holger Gruen. "Constant Buffers without Constant Pain." NVIDIA GameWorks Blog 2015.](https://developer.nvidia.com/content/constant-buffers-without-constant-pain-0)
    // \[Microsoft\] [Microsoft. "Ring buffer scenario." Microsoft Docs.](https://docs.microsoft.com/en-us/windows/win32/direct3d12/fence-based-resource-management#ring-buffer-scenario)
    VkBuffer m_uniform_buffer;
    uint64_t m_uniform_buffer_ring_buffer_begin;
    uint64_t m_uniform_buffer_ring_buffer_end;

    class gfx_texture_common *create_texture() override;
    void wsi_on_resized(wsi_window_ref wsi_window, float width, float height) override;
    void wsi_on_redraw_needed_acquire(wsi_window_ref wsi_window, float width, float height) override;
    void wsi_on_redraw_needed_release() override;

    inline gfx_connection_vk();
    inline ~gfx_connection_vk();
    bool init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual);
    void destroy() override;

public:
    inline void get_physical_device_format_properties(VkFormat format, VkFormatProperties *out_format_properties) { return m_api_vk.get_physical_device_format_properties(format, out_format_properties); }
    inline VkDeviceSize physical_device_limits_optimal_buffer_copy_offset_alignment() { return m_api_vk.physical_device_limits_optimal_buffer_copy_offset_alignment(); }
    inline VkDeviceSize physical_device_limits_optimal_buffer_copy_row_pitch_alignment() { return m_api_vk.physical_device_limits_optimal_buffer_copy_row_pitch_alignment(); }
    inline VkResult create_image(VkImageCreateInfo const *pCreateInfo, VkImage *pImage) { return m_api_vk.create_image(pCreateInfo, pImage); }
    inline void get_image_memory_requirements(VkImage image, VkMemoryRequirements *memory_requirements) { return m_api_vk.get_image_memory_requirements(image, memory_requirements); }

    inline VkDeviceMemory transfer_dst_and_sampled_image_alloc(VkMemoryRequirements const *memory_requirements, void **out_page_handle, uint64_t *out_offset, uint64_t *out_size) { return m_malloc.transfer_dst_and_sampled_image_alloc(memory_requirements, out_page_handle, out_offset, out_size); }
    inline void transfer_dst_and_sampled_image_free(void *page_handle, uint64_t offset, uint64_t size, VkDeviceMemory device_memory) { return m_malloc.transfer_dst_and_sampled_image_free(page_handle, offset, size, device_memory); }

    //uniform buffer
    //assert(0==(pMemoryRequirements->alignment%m_physical_device_limits_min_uniform_buffer_offset_alignment))
};

class gfx_connection_vk *gfx_connection_vk_init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual);

#endif