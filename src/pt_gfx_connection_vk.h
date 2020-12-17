/*
 * Copyright (C) YuqiaoZhang
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

#include <pt_gfx_connection.h>
#include "pt_gfx_connection_common.h"
#include "pt_gfx_malloc_common.h"
#include <vulkan/vulkan.h>

class gfx_connection_vk : public gfx_connection_common, public gfx_malloc_common
{
    VkAllocationCallbacks m_allocator_callbacks;

    VkInstance m_instance;
    PFN_vkCreateInstance m_vkCreateInstance;
    VkPhysicalDevice m_physical_device;
    VkDeviceSize m_physical_device_limits_optimal_buffer_copy_offset_alignment;
    VkDeviceSize m_physical_device_limits_optimal_buffer_copy_row_pitch_alignment;
    VkDeviceSize m_physical_device_limits_non_coherent_atom_size;
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
    PFN_vkGetPhysicalDeviceFormatProperties m_vkGetPhysicalDeviceFormatProperties;
    PFN_vkCreateDevice m_vkCreateDevice;
    PFN_vkGetDeviceProcAddr m_vkGetDeviceProcAddr;
    PFN_vkGetDeviceQueue m_vkGetDeviceQueue;
    PFN_vkAllocateMemory m_vkAllocateMemory;
    PFN_vkFreeMemory m_vkFreeMemory;

#ifndef NDEBUG
    PFN_vkCreateDebugReportCallbackEXT m_vkCreateDebugReportCallbackEXT;
    PFN_vkDestroyDebugReportCallbackEXT m_vkDestroyDebugReportCallbackEXT;
#endif

    void *m_wsi_connection;
    void *m_visual;
    static char const *platform_surface_extension_name();
    bool platform_physical_device_presentation_support(VkPhysicalDevice physical_device, uint32_t queue_family_index);

    static void *const m_invalid_wsi_connection; // = NULL;
    static void *const m_invalid_visual;         // = ((void *)-1);

#ifndef NDEBUG
    VkDebugReportCallbackEXT m_debug_report_callback;
    VkBool32 debug_report_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char *pLayerPrefix, const char *pMessage);
#endif

    struct gfx_itexture *create_texture() override;

    ~gfx_connection_vk();
    void destroy() override;

public:
    inline gfx_connection_vk() : m_wsi_connection(m_invalid_wsi_connection), m_visual(m_invalid_visual) {}
    bool init();
    void size_change_callback(void *wsi_connection, void *visual, void *window, float width, float height);
    void draw_request_callback(void *wsi_connection, void *visual, void *window);
    inline VkDeviceSize physical_device_limits_optimal_buffer_copy_offset_alignment() { return m_physical_device_limits_optimal_buffer_copy_offset_alignment; }
    inline VkDeviceSize physical_device_limits_optimal_buffer_copy_row_pitch_alignment() { return m_physical_device_limits_optimal_buffer_copy_row_pitch_alignment; }

    // https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/vkspec.html#fundamentals-threadingbehavior
    // vkGetPhysicalDeviceFormatProperties
    // vkAllocateMemory
    // vkFreeMemory
    void get_physical_device_format_properties(VkFormat format, VkFormatProperties *out_format_properties);
    VkResult allocate_memory(VkMemoryAllocateInfo const *memory_allocate_info, VkDeviceMemory *out_device_memory);
    void free_memory(VkDeviceMemory device_memory);
};

gfx_connection_vk *gfx_connection_vk_init(struct wsi_iwindow *window);

#endif