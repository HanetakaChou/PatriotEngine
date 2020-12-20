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

#define VK_USE_PLATFORM_XCB_KHR
#include <stdint.h>
#include <pt_mcrt_thread.h>
#include <pt_mcrt_atomic.h>
#include "pt_gfx_connection_vk.h"
#include <xcb/xcb.h>
#include <vulkan/vulkan.h>

inline xcb_connection_t *unwrap(wsi_connection_ref wsi_connection) { return reinterpret_cast<xcb_connection_t *>(wsi_connection); }
inline xcb_visualid_t unwrap(wsi_visual_ref wsi_visual) { return reinterpret_cast<uintptr_t>(wsi_visual); }

//static_assert(sizeof(xcb_window_t) <= sizeof(void *), "sizeof(xcb_window_t) <= sizeof(void *)");

void gfx_connection_vk::wsi_on_resized(wsi_window_ref wsi_window, float width, float height)
{
    //m_wsi_window = wsi_window;

    //xcb_connection_t *wsi_connection = static_cast<xcb_connection_t *>(m_wsi_connection);
    //xcb_visualid_t visual = reinterpret_cast<uintptr_t>(m_visual);
    //xcb_window_t window = reinterpret_cast<uintptr_t>(_window);
}

void gfx_connection_vk::wsi_on_redraw_needed_acquire(wsi_window_ref wsi_window, float width, float height)
{
#if 0
    assert(m_wsi_connection == m_invalid_wsi_connection || _wsi_connection == m_wsi_connection);
    assert(m_visual == m_invalid_visual || m_visual == _visual);
    m_wsi_connection = _wsi_connection;
    m_visual = _visual;

    xcb_connection_t *wsi_connection = static_cast<xcb_connection_t *>(m_wsi_connection);
    xcb_visualid_t visual = reinterpret_cast<uintptr_t>(m_visual);
    xcb_window_t window = reinterpret_cast<uintptr_t>(_window);
#endif
}

void gfx_connection_vk::wsi_on_redraw_needed_release()
{
}

char const *gfx_connection_vk::platform_surface_extension_name(uint32_t index)
{
    if (0 == index)
    {
        return VK_KHR_SURFACE_EXTENSION_NAME;
    }
    else if (1 == index)
    {
        return VK_KHR_XCB_SURFACE_EXTENSION_NAME;
    }
    else
    {
        return NULL;
    }
}

uint32_t gfx_connection_vk::platform_surface_extension_count()
{
    return 2;
}

bool gfx_connection_vk::platform_physical_device_presentation_support(VkPhysicalDevice physical_device, uint32_t queue_family_index)
{
    PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR m_vkGetPhysicalDeviceXcbPresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR>(vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceXcbPresentationSupportKHR"));

    xcb_connection_t *wsi_connection = unwrap(m_wsi_connection);
    xcb_visualid_t wsi_visual = unwrap(m_wsi_visual);

    VkBool32 res = m_vkGetPhysicalDeviceXcbPresentationSupportKHR(physical_device, queue_family_index, wsi_connection, wsi_visual);

    return (VK_FALSE != res);
}

char const *gfx_connection_vk::platform_swapchain_extension_name(uint32_t index)
{
    if (0 == index)
    {
        return VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    }
    else
    {
        return NULL;
    }
}

uint32_t gfx_connection_vk::platform_swapchain_extension_count()
{
    return 1;
}
