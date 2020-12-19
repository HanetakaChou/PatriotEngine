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

#include <stdint.h>
#include <pt_mcrt_thread.h>
#include <pt_mcrt_atomic.h>
#include "pt_gfx_connection_vk.h"
#include <vulkan/vulkan.h>

void gfx_connection_vk::size_change_callback(void *_wsi_connection, void *_visual, void *_window, float width, float height)
{
    assert(m_wsi_connection == m_invalid_wsi_connection || _wsi_connection == m_wsi_connection);
    assert(m_visual == m_invalid_visual || m_visual == _visual);
    m_wsi_connection = _wsi_connection;
    m_visual = _visual;

    void *bitmap_data = static_cast<void *>(m_wsi_connection);
}

void gfx_connection_vk::draw_request_callback(void *_wsi_connection, void *_visual, void *_window)
{
    assert(m_wsi_connection == m_invalid_wsi_connection || _wsi_connection == m_wsi_connection);
    assert(m_visual == m_invalid_visual || m_visual == _visual);
    m_wsi_connection = _wsi_connection;
    m_visual = _visual;

    void *bitmap_data = static_cast<void *>(m_wsi_connection);
}

char const *gfx_connection_vk::platform_surface_extension_name(uint32_t index)
{
    return NULL;
}

uint32_t gfx_connection_vk::platform_surface_extension_count()
{
    return 0;
}

bool gfx_connection_vk::platform_physical_device_presentation_support(VkPhysicalDevice physical_device, uint32_t queue_family_index)
{
    return true;
}


char const *gfx_connection_vk::platform_swapchain_extension_name(uint32_t index)
{
    return NULL;
}

uint32_t gfx_connection_vk::platform_swapchain_extension_count()
{
    return 0;
}
