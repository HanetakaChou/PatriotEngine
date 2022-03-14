//
// Copyright (C) YuqiaoZhang(HanetakaChou)
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

#define VK_USE_PLATFORM_ANDROID_KHR 1
#include <stddef.h>
#include <stdint.h>
#include <pt_mcrt_atomic.h>
#include "pt_gfx_device_vk.h"
#include <android/native_window.h>
#include <vulkan/vulkan.h>
#include <assert.h>

inline ANativeWindow *unwrap(pt_gfx_wsi_window_ref wsi_window) { return reinterpret_cast<ANativeWindow *>(wsi_window); }

char const *gfx_device_vk::platform_surface_extension_name()
{
    return VK_KHR_ANDROID_SURFACE_EXTENSION_NAME;
}

bool gfx_device_vk::platform_physical_device_presentation_support(PFN_vkGetInstanceProcAddr get_instance_proc_addr, VkPhysicalDevice physical_device, uint32_t queue_family_index, pt_gfx_wsi_connection_ref wsi_connection, pt_gfx_wsi_visual_ref wsi_visual)
{
    // https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/chap33.html#platformQuerySupport_android
    // On Android, all physical devices and queue families must be capable of presentation with any native window. As a result there is no Android-specific query for these capabilities.

    return true;
}

char const *gfx_device_vk::platform_create_surface_function_name()
{
    return "vkCreateAndroidSurfaceKHR";
}

VkResult gfx_device_vk::platform_create_surface(VkSurfaceKHR *surface, pt_gfx_wsi_connection_ref wsi_connection, pt_gfx_wsi_window_ref wsi_window)
{
    PFN_vkCreateAndroidSurfaceKHR vk_create_android_surface = reinterpret_cast<PFN_vkCreateAndroidSurfaceKHR>(this->m_vk_platform_create_surface);
    assert(NULL != vk_create_android_surface);

    VkAndroidSurfaceCreateInfoKHR android_surface_create_info;
    android_surface_create_info.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    android_surface_create_info.pNext = NULL;
    android_surface_create_info.flags = 0U;
    android_surface_create_info.window = unwrap(wsi_window);

    return vk_create_android_surface(this->m_instance, &android_surface_create_info, this->m_vk_allocation_callbacks, surface);
}
