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

#define VK_USE_PLATFORM_WIN32_KHR 1
#include <stddef.h>
#include <stdint.h>
// https://docs.microsoft.com/en-us/windows/win32/winprog/using-the-windows-headers#faster-builds-with-smaller-header-files
#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN 1
#define NOGDICAPMASKS 1
#define NOVIRTUALKEYCODES 1
#define NOWINMESSAGES 1
#define NOWINSTYLES 1
#define NOSYSMETRICS 1
#define NOMENUS 1
#define NOICONS 1
#define NOKEYSTATES 1
#define NOSYSCOMMANDS 1
#define NORASTEROPS 1
#define NOSHOWWINDOW 1
#define NOATOM 1
#define NOCLIPBOARD 1
#define NOCOLOR 1
#define NOCTLMGR 1
#define NODRAWTEXT 1
#define NOGDI 1
#define NOKERNEL 1
// #define NOUSER 1
#define NONLS 1
#define NOMB 1
#define NOMEMMGR 1
#define NOMETAFILE 1
#define NOMINMAX 1
#define NOMSG 1
#define NOOPENFILE 1
#define NOSCROLL 1
#define NOSERVICE 1
#define NOSOUND 1
#define NOTEXTMETRIC 1
#define NOWH 1
// #define NOWINOFFSETS 1
#define NOCOMM 1
#define NOKANJI 1
#define NOHELP 1
#define NOPROFILER 1
#define NODEFERWINDOWPOS 1
#define NOMCX 1
#include <Windows.h>
#include <pt_mcrt_atomic.h>
#include "pt_gfx_device_vk.h"
#include <vulkan/vulkan.h>
#include <assert.h>

inline HWND unwrap(pt_gfx_wsi_window_ref wsi_window) { return reinterpret_cast<HWND>(wsi_window); }

char const *gfx_device_vk::platform_surface_extension_name()
{
    return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
}

bool gfx_device_vk::platform_physical_device_presentation_support(PFN_vkGetInstanceProcAddr get_instance_proc_addr, VkPhysicalDevice physical_device, uint32_t queue_family_index, pt_gfx_wsi_connection_ref wsi_connection, pt_gfx_wsi_visual_ref wsi_visual)
{
    PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR vk_get_physical_device_win32_presentation_support = reinterpret_cast<PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR>(get_instance_proc_addr(this->m_instance, "vkGetPhysicalDeviceWin32PresentationSupportKHR"));

    VkBool32 res_get_physical_device_win32_presentation_support = vk_get_physical_device_win32_presentation_support(physical_device, queue_family_index);

    return (VK_FALSE != res_get_physical_device_win32_presentation_support);
}

char const *gfx_device_vk::platform_create_surface_function_name()
{
    return "vkCreateWin32SurfaceKHR";
}

VkResult gfx_device_vk::platform_create_surface(VkSurfaceKHR *surface, pt_gfx_wsi_connection_ref wsi_connection, pt_gfx_wsi_window_ref wsi_window)
{
    PFN_vkCreateWin32SurfaceKHR vk_create_win32_surface = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(this->m_vk_platform_create_surface);
    assert(NULL != vk_create_win32_surface);

    VkWin32SurfaceCreateInfoKHR win32_surface_create_info;
    win32_surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    win32_surface_create_info.pNext = NULL;
    win32_surface_create_info.flags = 0U;
    win32_surface_create_info.hinstance = reinterpret_cast<HINSTANCE>(GetClassLongPtrW(unwrap(wsi_window), GCLP_HMODULE));
    win32_surface_create_info.hwnd = unwrap(wsi_window);

    return vk_create_win32_surface(this->m_instance, &win32_surface_create_info, this->m_vk_allocation_callbacks, surface);
}
