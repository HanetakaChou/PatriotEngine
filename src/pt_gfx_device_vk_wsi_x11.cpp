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

#define VK_USE_PLATFORM_XCB_KHR 1
#include <stddef.h>
#include <stdint.h>
#include <pt_mcrt_atomic.h>
#include "pt_gfx_device_vk.h"
#include <xcb/xcb.h>
#include <vulkan/vulkan.h>
#include <assert.h>

inline xcb_connection_t *unwrap(wsi_connection_ref wsi_connection) { return reinterpret_cast<xcb_connection_t *>(wsi_connection); }
inline xcb_window_t unwrap(wsi_window_ref wsi_window) { return reinterpret_cast<uintptr_t>(wsi_window); }
inline xcb_visualid_t unwrap(wsi_visual_ref wsi_visual) { return reinterpret_cast<uintptr_t>(wsi_visual); }

char const *gfx_device_vk::platform_surface_extension_name()
{
    return VK_KHR_XCB_SURFACE_EXTENSION_NAME;
}

bool gfx_device_vk::platform_physical_device_presentation_support(PFN_vkGetInstanceProcAddr get_instance_proc_addr, VkPhysicalDevice physical_device, uint32_t queue_family_index, wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual)
{
    PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR vk_get_physical_device_xcb_presentation_support = reinterpret_cast<PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR>(get_instance_proc_addr(m_instance, "vkGetPhysicalDeviceXcbPresentationSupportKHR"));

    VkBool32 res_get_physical_device_xcb_presentation_support = vk_get_physical_device_xcb_presentation_support(physical_device, queue_family_index, unwrap(wsi_connection), unwrap(wsi_visual));

    return (VK_FALSE != res_get_physical_device_xcb_presentation_support);
}

char const *gfx_device_vk::platform_create_surface_function_name()
{
    return "vkCreateXcbSurfaceKHR";
}

VkResult gfx_device_vk::platform_create_surface(VkSurfaceKHR *surface, wsi_connection_ref wsi_connection, wsi_window_ref wsi_window)
{
    PFN_vkCreateXcbSurfaceKHR vk_create_xcb_surface = reinterpret_cast<PFN_vkCreateXcbSurfaceKHR>(this->m_vk_platform_create_surface);
    assert(NULL != vk_create_xcb_surface);

    VkXcbSurfaceCreateInfoKHR xcb_surface_create_info;
    xcb_surface_create_info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    xcb_surface_create_info.pNext = NULL;
    xcb_surface_create_info.flags = 0U;
    xcb_surface_create_info.connection = unwrap(wsi_connection);
    xcb_surface_create_info.window = unwrap(wsi_window);

    return vk_create_xcb_surface(this->m_instance, &xcb_surface_create_info, this->m_vk_allocation_callbacks, surface);
}
