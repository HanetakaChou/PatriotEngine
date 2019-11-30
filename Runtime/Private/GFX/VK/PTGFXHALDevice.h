#ifndef PT_PRIVATE_GFX_HAL_H
#define PT_PRIVATE_GFX_HAL_H 1

#include "../../../Public/GFX/PTGFXHAL.h"

#if defined(PTWIN32DESKTOP)
#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR 1
#include "../../../ThirdParty/vulkansdk_windows/include/vulkan/vulkan.h"
#elif defined(PTWIN32RUNTIME)
#error Win32 Runtime Not Support Vulkan
#elif defined(PTPOSIXLINUXBIONIC)
#include <android/native_window.h>
#define VK_USE_PLATFORM_ANDROID_KHR 1
#include <vulkan/vulkan.h>
#elif defined(PTPOSIXLINUXGLIBC)
#include <xcb/xcb.h>
#define VK_USE_PLATFORM_XCB_KHR 1
#include "../../../../ThirdParty/PosixLinuxGlibc/include/vulkan/vulkan.h"
#else
#error 未知的平台
#endif

struct PT_GFX_HAL_Device_VK :public PT_GFX_HAL_IDevice
{
	VkInstance m_hInstance;

	decltype(::vkDestroyInstance) *m_pFn_DestroyInstance;
	decltype(::vkEnumeratePhysicalDevices) *m_pFn_EnumeratePhysicalDevices;
	decltype(::vkGetPhysicalDeviceProperties) *m_pFn_GetPhysicalDeviceProperties;
	decltype(::vkGetPhysicalDeviceFeatures) *m_pFn_GetPhysicalDeviceFeatures;
	decltype(::vkGetPhysicalDeviceQueueFamilyProperties) *m_pFn_GetPhysicalDeviceQueueFamilyProperties;
	decltype(::vkGetPhysicalDeviceMemoryProperties) *m_pFn_GetPhysicalDeviceMemoryProperties;
	decltype(::vkGetPhysicalDeviceImageFormatProperties) *m_pFn_GetPhysicalDeviceImageFormatProperties;
	
	decltype(::vkCreateDevice) *m_pFn_CreateDevice;
};



#endif