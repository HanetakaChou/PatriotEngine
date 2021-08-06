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

#include <stddef.h>
#include <stdint.h>
#include "pt_gfx_device_vk.h"

char const *gfx_device_vk::platform_surface_extension_name(uint32_t index)
{
    return NULL;
}

uint32_t gfx_device_vk::platform_surface_extension_count()
{
    return 0;
}

char const *gfx_device_vk::platform_swapchain_extension_name(uint32_t index)
{
    return NULL;
}

uint32_t gfx_device_vk::platform_swapchain_extension_count()
{
    return 0;
}

bool gfx_device_vk::platform_physical_device_presentation_support(VkPhysicalDevice physical_device, uint32_t queue_family_index, wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual, wsi_window_ref wsi_window)
{
    return true;
}

VkResult gfx_device_vk::platform_create_surface(VkSurfaceKHR *surface, wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual, wsi_window_ref wsi_window)
{
    return VK_SUCCESS;
}