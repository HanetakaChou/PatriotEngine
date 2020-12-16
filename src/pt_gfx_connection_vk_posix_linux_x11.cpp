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
#include "pt_gfx_connection_vk.h"
#include <xcb/xcb.h>
#include <vulkan/vulkan.h>

static_assert(sizeof(xcb_window_t) <= sizeof(void *), "sizeof(xcb_window_t) <= sizeof(void *)");

void gfx_connection_vk::size_change_callback(void *_wsi_connection, void *_visual, void *_window, float width, float height)
{
    assert(m_wsi_connection == NULL || _wsi_connection == m_wsi_connection);
    assert(m_visual == reinterpret_cast<void *>(intptr_t(-1)) || m_visual == _visual);
    m_wsi_connection = _wsi_connection;
    m_visual = _visual;

    xcb_connection_t *wsi_connection = static_cast<xcb_connection_t *>(m_wsi_connection);
    xcb_visualid_t visual = reinterpret_cast<uintptr_t>(m_visual);
    xcb_window_t window = reinterpret_cast<uintptr_t>(_window);
}

void gfx_connection_vk::draw_request_callback(void *_wsi_connection, void *_visual, void *_window)
{
    assert(m_wsi_connection == NULL || _wsi_connection == m_wsi_connection);
    assert(m_visual == reinterpret_cast<void *>(intptr_t(-1)) || m_visual == _visual);
    m_wsi_connection = _wsi_connection;
    m_visual = _visual;

    xcb_connection_t *wsi_connection = static_cast<xcb_connection_t *>(m_wsi_connection);
    xcb_visualid_t visual = reinterpret_cast<uintptr_t>(m_visual);
    xcb_window_t window = reinterpret_cast<uintptr_t>(_window);
}

char const *gfx_connection_vk::platform_surface_extension_name()
{
    return VK_KHR_XCB_SURFACE_EXTENSION_NAME;
}

bool gfx_connection_vk::platform_physical_device_presentation_support(VkPhysicalDevice physical_device, uint32_t queue_family_index)
{
    PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR m_vkGetPhysicalDeviceXcbPresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR>(vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceXcbPresentationSupportKHR"));

    xcb_connection_t *wsi_connection = NULL;
    xcb_visualid_t visual = intptr_t(-1);
    while ((NULL == (wsi_connection = static_cast<xcb_connection_t *>(m_wsi_connection))) || (static_cast<xcb_visualid_t>(intptr_t(-1)) == (visual = reinterpret_cast<uintptr_t>(m_visual))))
    {
        mcrt_os_yield();
    }

    VkBool32 res = m_vkGetPhysicalDeviceXcbPresentationSupportKHR(physical_device, queue_family_index, wsi_connection, visual);

    return (VK_FALSE != res);
}
