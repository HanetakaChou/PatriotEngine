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
#include <pt_common.h>
#include <pt_gfx_common.h>
#include <pt_gfx_connection.h>
#include "pt_gfx_connection_common.h"
#include "pt_gfx_malloc_common.h"
#include "pt_gfx_malloc_vk.h"
#include <vulkan/vulkan.h>

class gfx_connection_vk : public gfx_connection_common, public gfx_malloc_vk
{
    VkAllocationCallbacks m_allocator_callbacks;

    VkInstance m_instance;
    VkPhysicalDevice m_physical_device;
    VkDeviceSize m_physical_device_limits_buffer_image_granularity;
    VkDeviceSize m_physical_device_limits_min_uniform_buffer_offset_alignment;
    VkDeviceSize m_physical_device_limits_optimal_buffer_copy_offset_alignment;
    VkDeviceSize m_physical_device_limits_optimal_buffer_copy_row_pitch_alignment;
    VkDeviceSize m_physical_device_limits_non_coherent_atom_size; //we don't need non-coherent (for readback?)
    bool m_queue_GP_diff_queue_T;
    uint32_t m_queue_GP_family_index;
    uint32_t m_queue_T_family_index;
    uint32_t m_queue_GP_queue_index;
    uint32_t m_queue_T_queue_index;
    VkDevice m_device;
    VkQueue m_queue_GP;
    VkQueue m_queue_T;
    bool m_physical_device_feature_texture_compression_ASTC_LDR;
    bool m_physical_device_feature_texture_compression_BC;

    PFN_vkEnumeratePhysicalDevices m_vkEnumeratePhysicalDevices;
    PFN_vkGetPhysicalDeviceProperties m_vkGetPhysicalDeviceProperties;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties m_vkGetPhysicalDeviceQueueFamilyProperties;
    PFN_vkGetPhysicalDeviceFeatures m_vkGetPhysicalDeviceFeatures;
    PFN_vkCreateDevice m_vkCreateDevice;
    PFN_vkGetDeviceQueue m_vkGetDeviceQueue;
    PFN_vkGetPhysicalDeviceFormatProperties m_vk_get_physical_device_format_properties;
    PFN_vkCreateBuffer m_vk_create_buffer;
    PFN_vkCreateImage m_vk_create_image;
    PFN_vkAllocateMemory m_vkAllocateMemory;
    PFN_vkFreeMemory m_vkFreeMemory;

    wsi_connection_ref m_wsi_connection;
    wsi_visual_ref m_wsi_visual;
    static char const *platform_surface_extension_name(uint32_t index);
    static uint32_t platform_surface_extension_count();
    bool platform_physical_device_presentation_support(VkPhysicalDevice physical_device, uint32_t queue_family_index);
    static char const *platform_swapchain_extension_name(uint32_t index);
    static uint32_t platform_swapchain_extension_count();

#ifndef NDEBUG
    VkDebugReportCallbackEXT m_debug_report_callback;
    VkBool32 debug_report_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char *pLayerPrefix, const char *pMessage);
#endif

    struct async_resource_job_result
    {
        VkFence m_fence;
        //m_vertexbuffers
        //m_indexbuffers
        //m_textures
        uint64_t m_ringbuffer_begin;
        uint64_t m_ringbuffer_end;
    };

    //deque


    class gfx_texture_common *create_texture() override;
    void wsi_on_resized(wsi_window_ref wsi_window, float width, float height) override;
    void wsi_on_redraw_needed_acquire(wsi_window_ref wsi_window, float width, float height) override;
    void wsi_on_redraw_needed_release() override;

    inline gfx_connection_vk(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual) : m_wsi_connection(wsi_connection), m_wsi_visual(wsi_visual) {}
    ~gfx_connection_vk();
    bool init();
    void destroy() override;

public:
    static class gfx_connection_vk *init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual);

    inline VkDeviceSize physical_device_limits_optimal_buffer_copy_offset_alignment() { return m_physical_device_limits_optimal_buffer_copy_offset_alignment; }
    inline VkDeviceSize physical_device_limits_optimal_buffer_copy_row_pitch_alignment() { return m_physical_device_limits_optimal_buffer_copy_row_pitch_alignment; }

    // Externally Synchronized Parameters
    // The queue parameter in vkQueueSubmit

    // https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/chap4.html#fundamentals-threadingbehavior
    // vkGetPhysicalDeviceFormatProperties
    // vkCreateImage
    // vkAllocateMemory
    // vkFreeMemory
    inline void get_physical_device_format_properties(VkFormat format, VkFormatProperties *out_format_properties) { return m_vk_get_physical_device_format_properties(m_physical_device, format, out_format_properties); }

    inline VkResult create_buffer(VkBufferCreateInfo const *pCreateInfo, VkBuffer *pBuffer) { return m_vk_create_buffer(m_device, pCreateInfo, &m_allocator_callbacks, pBuffer); }
    inline VkResult create_image(VkImageCreateInfo const *pCreateInfo, VkImage *pImage) { return m_vk_create_image(m_device, pCreateInfo, &m_allocator_callbacks, pImage); }

    //uniform buffer
    //assert(0==(pMemoryRequirements->alignment%m_physical_device_limits_min_uniform_buffer_offset_alignment))

    inline VkResult allocate_memory(VkMemoryAllocateInfo const *memory_allocate_info, VkDeviceMemory *out_device_memory) { return m_vkAllocateMemory(m_device, memory_allocate_info, &m_allocator_callbacks, out_device_memory); }
    inline void free_memory(VkDeviceMemory device_memory) { return m_vkFreeMemory(m_device, device_memory, &m_allocator_callbacks); }
};

#endif