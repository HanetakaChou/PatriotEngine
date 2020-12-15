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

#include <stddef.h>
#include <pt_mcrt_malloc.h>
#include "pt_gfx_connection_vk.h"
#include "pt_gfx_texture_vk.h"
#include <assert.h>
#include <stdio.h>
#include <new>

gfx_connection_vk *gfx_connection_vk_init(struct wsi_iwindow *window)
{
    gfx_connection_vk *connection = new (mcrt_aligned_malloc(sizeof(gfx_connection_vk), alignof(gfx_connection_vk))) gfx_connection_vk();
    window->listen_size_change(
        [](void *connection, void *window, float width, float height, void *user_data) -> void {
            static_cast<gfx_connection_vk *>(user_data)->size_change_callback(connection, window, width, height);
        },
        connection);
    window->listen_draw_request(
        [](void *connection, void *window, void *user_data) -> void {
            static_cast<gfx_connection_vk *>(user_data)->draw_request_callback(connection, window);
        },
        connection);

    if (connection->init())
    {
        return connection;
    }
    else
    {
        mcrt_free(connection);
        return NULL;
    }
}

bool gfx_connection_vk::init()
{
    m_allocator_callbacks.pUserData = NULL;
    m_allocator_callbacks.pfnAllocation = [](void *, size_t size, size_t alignment, VkSystemAllocationScope) -> void * { return mcrt_aligned_malloc(size, alignment); };
    m_allocator_callbacks.pfnReallocation = [](void *, void *pOriginal, size_t size, size_t alignment, VkSystemAllocationScope) -> void * { return mcrt_aligned_realloc(pOriginal, size, alignment); };
    m_allocator_callbacks.pfnFree = [](void *, void *pMemory) -> void { return mcrt_free(pMemory); };
    m_allocator_callbacks.pfnInternalAllocation = [](void *, size_t, VkInternalAllocationType, VkSystemAllocationScope) -> void {};
    m_allocator_callbacks.pfnInternalFree = [](void *, size_t, VkInternalAllocationType, VkSystemAllocationScope) -> void {};

    m_vkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(vkGetInstanceProcAddr(NULL, "vkCreateInstance"));
    if (NULL == m_vkCreateInstance)
    {
        return false;
    }

    VkInstanceCreateInfo instance_create_info;
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pNext = NULL;
    instance_create_info.flags = 0U;
    VkApplicationInfo application_info;
    application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application_info.pNext = NULL;
    application_info.pApplicationName = "PatriotEngine";
    application_info.applicationVersion = 1;
    application_info.pEngineName = "PatriotEngine";
    application_info.engineVersion = 1;
    application_info.apiVersion = VK_API_VERSION_1_0;
    instance_create_info.pApplicationInfo = &application_info;
#ifndef NDEBUG
    const char *enabled_layer_names[1] = {"VK_LAYER_KHRONOS_validation"}; //VK_LAYER_LUNARG_standard_validation
    instance_create_info.enabledLayerCount = 1;
    instance_create_info.ppEnabledLayerNames = enabled_layer_names;
    const char *enabled_extension_names[3] = {VK_EXT_DEBUG_REPORT_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME, wsi_surface_extension_name()};
    instance_create_info.enabledExtensionCount = 3;
    instance_create_info.ppEnabledExtensionNames = enabled_extension_names;
#else
    instance_create_info.enabledLayerCount = 0;
    instance_create_info.ppEnabledLayerNames = NULL;
    instance_create_info.enabledExtensionCount = 2;
    instance_create_info.ppEnabledExtensionNames = {VK_KHR_SURFACE_EXTENSION_NAME, wsi_surface_extension_name()};
#endif

    VkResult res = m_vkCreateInstance(&instance_create_info, &m_allocator_callbacks, &m_instance);
    if (VK_SUCCESS != res)
    {
        return false;
    }

#ifndef NDEBUG
    m_vkCreateDebugReportCallbackEXT = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(m_instance, "vkCreateDebugReportCallbackEXT"));
    if (NULL == m_vkCreateDebugReportCallbackEXT)
    {
        return false;
    }

    m_vkDestroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(m_instance, "vkDestroyDebugReportCallbackEXT"));
    if (NULL == m_vkDestroyDebugReportCallbackEXT)
    {
        return false;
    }

    VkDebugReportCallbackCreateInfoEXT debug_report_callback_create_info;
    debug_report_callback_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    debug_report_callback_create_info.pNext = NULL;
    debug_report_callback_create_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
                                              VK_DEBUG_REPORT_WARNING_BIT_EXT |
                                              VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
    debug_report_callback_create_info.pfnCallback = [](VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char *pLayerPrefix, const char *pMessage, void *pUserData) -> VkBool32 {
        return static_cast<gfx_connection_vk *>(pUserData)->debug_report_callback(flags, objectType, object, location, messageCode, pLayerPrefix, pMessage);
    };
    debug_report_callback_create_info.pUserData = this;

    res = m_vkCreateDebugReportCallbackEXT(m_instance, &debug_report_callback_create_info, &m_allocator_callbacks, &m_debug_report_callback);
    if (VK_SUCCESS != res)
    {
        return false;
    }
#endif

    m_vkEnumeratePhysicalDevices = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(vkGetInstanceProcAddr(m_instance, "vkEnumeratePhysicalDevices"));
    if (NULL == m_vkEnumeratePhysicalDevices)
    {
        return false;
    }

    uint32_t physical_device_count;
    res = m_vkEnumeratePhysicalDevices(m_instance, &physical_device_count, NULL);
    if (VK_SUCCESS != res)
    {
        return false;
    }

    uint32_t physical_device_count_res;
    VkPhysicalDevice *physical_devices = static_cast<VkPhysicalDevice *>(mcrt_malloc(sizeof(VkPhysicalDevice) * physical_device_count));
    res = m_vkEnumeratePhysicalDevices(m_instance, &physical_device_count_res, physical_devices);
    assert(physical_device_count == physical_device_count_res);
    if (VK_SUCCESS != res)
    {
        return false;
    }
    
    //choose proper device

    mcrt_free(physical_devices);

    return true;
}

VkBool32 gfx_connection_vk::debug_report_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char *pLayerPrefix, const char *pMessage)
{
    printf("[%s] : %s", pLayerPrefix, pMessage);
    return VK_FALSE;
}

struct gfx_itexture *gfx_connection_vk::create_texture()
{
    gfx_texture_vk *texture = new (mcrt_aligned_malloc(sizeof(gfx_texture_vk), alignof(gfx_texture_vk))) gfx_texture_vk(this);
    return texture;
}

gfx_connection_vk::~gfx_connection_vk()
{
#ifndef NDEBUG
    m_vkDestroyDebugReportCallbackEXT(m_instance, m_debug_report_callback, &m_allocator_callbacks);
#endif
}

void gfx_connection_vk::destroy()
{
    this->~gfx_connection_vk();
    mcrt_free(this);
}
