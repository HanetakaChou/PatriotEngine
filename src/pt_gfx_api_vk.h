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

#ifndef _PT_GFX_API_VK_H_
#define _PT_GFX_API_VK_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_gfx_connection.h>
#include <vulkan/vulkan.h>

class gfx_api_vk
{
    VkAllocationCallbacks m_allocator_callbacks;

    VkInstance m_instance;

    VkPhysicalDevice m_physical_device;
    VkDeviceSize m_physical_device_limits_buffer_image_granularity;
    VkDeviceSize m_physical_device_limits_min_uniform_buffer_offset_alignment; //Dynimac Uniform Buffer
    VkDeviceSize m_physical_device_limits_optimal_buffer_copy_offset_alignment;
    VkDeviceSize m_physical_device_limits_optimal_buffer_copy_row_pitch_alignment;
    VkDeviceSize m_physical_device_limits_non_coherent_atom_size; //we don't need non-coherent (for readback?)
    bool m_physical_device_feature_texture_compression_ASTC_LDR;
    bool m_physical_device_feature_texture_compression_BC;

    VkDevice m_device;
    bool m_has_dedicated_transfer_queue;
    uint32_t m_queue_graphics_family_index;
    uint32_t m_queue_transfer_family_index;
    uint32_t m_queue_graphics_queue_index;
    uint32_t m_queue_transfer_queue_index;
    VkQueue m_queue_graphics;
    VkQueue m_queue_transfer;

    PFN_vkGetPhysicalDeviceMemoryProperties m_vk_get_physical_device_memory_properties;
    PFN_vkGetPhysicalDeviceFormatProperties m_vk_get_physical_device_format_properties;
    PFN_vkCreateBuffer m_vk_create_buffer;
    PFN_vkCreateImage m_vk_create_image;
    PFN_vkDestroyBuffer m_vk_destroy_buffer;
    PFN_vkDestroyImage m_vk_destroy_image;
    PFN_vkGetBufferMemoryRequirements m_vk_get_buffer_memory_requirements;
    PFN_vkGetImageMemoryRequirements m_vk_get_image_memory_requirements;
    PFN_vkAllocateMemory m_vk_allocate_memory;
    PFN_vkFreeMemory m_vk_free_memory;
    PFN_vkBindBufferMemory m_vk_bind_buffer_memory;
    PFN_vkBindImageMemory m_vk_bind_image_memory;
    PFN_vkMapMemory m_vk_map_memory;
    PFN_vkUnmapMemory m_vk_unmap_memory;

    wsi_connection_ref m_wsi_connection;
    wsi_visual_ref m_wsi_visual;
    wsi_window_ref m_wsi_window;
    static char const *platform_surface_extension_name(uint32_t index);
    static uint32_t platform_surface_extension_count();
    bool platform_physical_device_presentation_support(VkPhysicalDevice physical_device, uint32_t queue_family_index);
    static char const *platform_swapchain_extension_name(uint32_t index);
    static uint32_t platform_swapchain_extension_count();
    void wsi_on_resized(wsi_window_ref wsi_window, float width, float height);
    void wsi_on_redraw_needed_acquire(wsi_window_ref wsi_window, float width, float height);
    void wsi_on_redraw_needed_release();

#ifndef NDEBUG
    VkDebugReportCallbackEXT m_debug_report_callback;
    VkBool32 debug_report_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char *pLayerPrefix, const char *pMessage);
#endif

public:
    gfx_api_vk();
    ~gfx_api_vk();
    bool init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual, wsi_window_ref wsi_window);

    inline VkDeviceSize physical_device_limits_optimal_buffer_copy_offset_alignment() { return m_physical_device_limits_optimal_buffer_copy_offset_alignment; }
    inline VkDeviceSize physical_device_limits_optimal_buffer_copy_row_pitch_alignment() { return m_physical_device_limits_optimal_buffer_copy_row_pitch_alignment; }

    // Externally Synchronized Parameters
    // The queue parameter in vkQueueSubmit

    // https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/chap4.html#fundamentals-threadingbehavior
    // vkGetPhysicalDeviceFormatProperties
    // vkCreateImage
    // vkGetBufferMemoryRequirements
    // vkGetImageMemoryRequirements
    // vkAllocateMemory
    // vkFreeMemory
    inline void get_physical_device_memory_properties(VkPhysicalDeviceMemoryProperties *memory_properties) { return m_vk_get_physical_device_memory_properties(m_physical_device, memory_properties); }
    inline void get_physical_device_format_properties(VkFormat format, VkFormatProperties *out_format_properties) { return m_vk_get_physical_device_format_properties(m_physical_device, format, out_format_properties); }
    inline VkResult create_buffer(VkBufferCreateInfo const *pCreateInfo, VkBuffer *pBuffer) { return m_vk_create_buffer(m_device, pCreateInfo, &m_allocator_callbacks, pBuffer); }
    inline VkResult create_image(VkImageCreateInfo const *pCreateInfo, VkImage *pImage) { return m_vk_create_image(m_device, pCreateInfo, &m_allocator_callbacks, pImage); }
    inline void destroy_buffer(VkBuffer buffer) { return m_vk_destroy_buffer(m_device, buffer, &m_allocator_callbacks); }
    inline void destroy_image(VkImage image) { return m_vk_destroy_image(m_device, image, &m_allocator_callbacks); }
    inline void get_buffer_memory_requirements(VkBuffer buffer, VkMemoryRequirements *memory_requirements) { return m_vk_get_buffer_memory_requirements(m_device, buffer, memory_requirements); }
    inline void get_image_memory_requirements(VkImage image, VkMemoryRequirements *memory_requirements) { return m_vk_get_image_memory_requirements(m_device, image, memory_requirements); }
    inline VkResult allocate_memory(VkMemoryAllocateInfo const *memory_allocate_info, VkDeviceMemory *out_device_memory) { return m_vk_allocate_memory(m_device, memory_allocate_info, &m_allocator_callbacks, out_device_memory); }
    inline void free_memory(VkDeviceMemory device_memory) { return m_vk_free_memory(m_device, device_memory, &m_allocator_callbacks); }
    inline VkResult bind_buffer_memory(VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset) { return m_vk_bind_buffer_memory(m_device, buffer, memory, memoryOffset); }
    inline VkResult bind_image_memory(VkImage image, VkDeviceMemory memory, VkDeviceSize memoryOffset) { return m_vk_bind_image_memory(m_device, image, memory, memoryOffset); }
    inline VkResult map_memory(VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void **ppData) { return m_vk_map_memory(m_device, memory, offset, size, flags, ppData); }
    inline void unmap_memory(VkDeviceMemory memory) { return m_vk_unmap_memory(m_device, memory); }
};

#endif
