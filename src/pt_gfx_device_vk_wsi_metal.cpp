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

#define VK_USE_PLATFORM_METAL_EXT 1
#include <stddef.h>
#include <stdint.h>
#include <pt_mcrt_atomic.h>
#include "pt_gfx_device_vk.h"
#include <vulkan/vulkan.h>

inline CAMetalLayer *unwrap(wsi_window_ref wsi_window) { return reinterpret_cast<CAMetalLayer *>(wsi_window); }

char const *gfx_device_vk::platform_surface_extension_name()
{
    return VK_EXT_METAL_SURFACE_EXTENSION_NAME;
}

bool gfx_device_vk::platform_physical_device_presentation_support(PFN_vkGetInstanceProcAddr get_instance_proc_addr, VkPhysicalDevice physical_device, uint32_t queue_family_index, wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual)
{
    // [33.4.9. iOS Platform](https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/chap33.html#platformQuerySupport_ios)
    // On iOS, all physical devices and queue families must be capable of presentation with any layer. As a result there is no iOS-specific query for these capabilities.

    // [33.4.10. macOS Platform](https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/chap33.html#platformQuerySupport_macos)
    // On macOS, all physical devices and queue families must be capable of presentation with any layer. As a result there is no macOS-specific query for these capabilities.

    return true;
}

char const *gfx_device_vk::platform_create_surface_function_name()
{
    return "vkCreateMetalSurfaceEXT";
}

VkResult gfx_device_vk::platform_create_surface(VkSurfaceKHR *surface, wsi_connection_ref wsi_connection, wsi_window_ref wsi_window)
{
    PFN_vkCreateMetalSurfaceEXT vk_create_metal_surface = reinterpret_cast<PFN_vkCreateMetalSurfaceEXT>(this->m_vk_platform_create_surface);
    assert(NULL != vk_create_metal_surface);

    VkMetalSurfaceCreateInfoEXT metal_surface_create_info;
    metal_surface_create_info.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
    metal_surface_create_info.pNext = NULL;
    metal_surface_create_info.flags = 0U;
    metal_surface_create_info.pLayer = unwrap(wsi_window);

    return vk_create_metal_surface(this->m_instance, &metal_surface_create_info, this->m_vk_allocation_callbacks, surface);
}
