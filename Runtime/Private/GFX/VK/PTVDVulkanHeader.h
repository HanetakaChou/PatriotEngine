#ifndef PT_VIDEO_VULKAN_VULKANHEADER_H
#define PT_VIDEO_VULKAN_VULKANHEADER_H

#if defined(PTWIN32)
#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#if defined(PTWIN32DESKTOP)
#define VK_USE_PLATFORM_WIN32_KHR 1
#include "../../../ThirdParty/VulkanSDK/Include/vulkan/vulkan.h"
#elif defined(PTWIN32RUNTIME)
#define VK_USE_PLATFORM_WIN32_KHR 1
//#include <CoreWindow.h>
//#ifndef __ICoreWindowInterop_INTERFACE_DEFINED__
//#define __ICoreWindowInterop_INTERFACE_DEFINED__
//struct DECLSPEC_UUID("45D64A29-A63E-4CB6-B498-5781D298CB4F")ICoreWindowInterop : public IUnknown
//{
//virtual HRESULT STDMETHODCALLTYPE get_WindowHandle(HWND *hwnd) = 0;
//virtual HRESULT STDMETHODCALLTYPE put_MessageHandled(boolean value) = 0;
//};
//#endif
//ICoreWindowInterop *pCoreWindowInterop;
//pWindow->QueryInterface(IID_PPV_ARGS(&pCoreWindowInterop));
//HWND hWnd;
//pCoreWindowInterop->get_WindowHandle(&hWnd);
//HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(::GetClassLongPtrW(hWnd, GCLP_HMODULE));
#else
#error 未知的平台
#endif
#elif defined(PTPOSIX)
#if defined(PTPOSIXLINUXGLIBC)
#include <xcb/xcb.h>
#define VK_USE_PLATFORM_XCB_KHR 1
#elif defined(PTPOSIXANDROID)
#include <android/native_window.h>
#define VK_USE_PLATFORM_ANDROID_KHR 1
#include <vulkan/vulkan.h>
#else
#error 未知的平台
#endif
#else
#error 未知的平台
#endif

class VkInstanceWrapper
{
private:
	VkInstance m_hInstance;

	decltype(::vkDestroyInstance) *m_pFn_DestroyInstance;
	decltype(::vkEnumeratePhysicalDevices) *m_pFn_EnumeratePhysicalDevices;
	decltype(::vkGetPhysicalDeviceProperties) *m_pFn_GetPhysicalDeviceProperties;
	decltype(::vkGetPhysicalDeviceFeatures) *m_pFn_GetPhysicalDeviceFeatures;
	decltype(::vkGetPhysicalDeviceQueueFamilyProperties) *m_pFn_GetPhysicalDeviceQueueFamilyProperties;
	decltype(::vkGetPhysicalDeviceMemoryProperties) *m_pFn_GetPhysicalDeviceMemoryProperties;
	decltype(::vkGetPhysicalDeviceImageFormatProperties) *m_pFn_GetPhysicalDeviceImageFormatProperties;
	decltype(::vkCreateDevice) *m_pFn_CreateDevice;

#if defined(PTWIN32)
#if defined(PTWIN32DESKTOP)
#ifndef NDEBUG
	decltype(::vkCreateDebugReportCallbackEXT) *m_pFn_CreateDebugReportCallbackEXT;
	decltype(::vkDestroyDebugReportCallbackEXT) *m_pFn_DestroyDebugReportCallbackEXT;
#endif
	decltype(::vkCreateWin32SurfaceKHR) *m_pFn_CreateWin32SurfaceKHR;
	decltype(::vkGetPhysicalDeviceWin32PresentationSupportKHR) *m_pFn_GetPhysicalDeviceWin32PresentationSupportKHR;
#elif defined(PTWIN32RUNTIME)

#else
#error 未知的平台
#endif
#elif defined(PTPOSIX)
#if defined(PTPOSIXLINUXGLIBC)

#elif defined(PTPOSIXANDROID)
#ifndef NDEBUG
	decltype(::vkCreateDebugReportCallbackEXT) *m_pFn_CreateDebugReportCallbackEXT;
	decltype(::vkDestroyDebugReportCallbackEXT) *m_pFn_DestroyDebugReportCallbackEXT;
#endif
	decltype(::vkCreateAndroidSurfaceKHR) *m_pFn_CreateAndroidSurfaceKHR;
#else
#error 未知的平台
#endif
#else
#error 未知的平台
#endif
	decltype (::vkDestroySurfaceKHR) *m_pFn_DestroySurfaceKHR;
#ifndef NDEBUG
	decltype (::vkGetPhysicalDeviceSurfaceSupportKHR) *m_pFn_GetPhysicalDeviceSurfaceSupportKHR;
#endif
	decltype (::vkGetPhysicalDeviceSurfaceCapabilitiesKHR) *m_pFn_GetPhysicalDeviceSurfaceCapabilitiesKHR;
	decltype (::vkGetPhysicalDeviceSurfaceFormatsKHR) *m_pFn_GetPhysicalDeviceSurfaceFormatsKHR;
	decltype (::vkGetPhysicalDeviceSurfacePresentModesKHR) *m_pFn_GetPhysicalDeviceSurfacePresentModesKHR;

public:
	inline VkInstanceWrapper(VkInstance hInstance, decltype(::vkGetInstanceProcAddr) *pFn_vkGetInstanceProcAddr)
	{
		m_hInstance = hInstance;

		m_pFn_DestroyInstance = reinterpret_cast<decltype(::vkDestroyInstance) *>(pFn_vkGetInstanceProcAddr(m_hInstance, "vkDestroyInstance"));

		m_pFn_EnumeratePhysicalDevices = reinterpret_cast<decltype(::vkEnumeratePhysicalDevices) *>(pFn_vkGetInstanceProcAddr(m_hInstance, "vkEnumeratePhysicalDevices"));
		m_pFn_GetPhysicalDeviceProperties = reinterpret_cast<decltype(::vkGetPhysicalDeviceProperties) *>(pFn_vkGetInstanceProcAddr(m_hInstance, "vkGetPhysicalDeviceProperties"));
		m_pFn_GetPhysicalDeviceFeatures = reinterpret_cast<decltype(::vkGetPhysicalDeviceFeatures) *>(pFn_vkGetInstanceProcAddr(m_hInstance, "vkGetPhysicalDeviceFeatures"));
		m_pFn_GetPhysicalDeviceQueueFamilyProperties = reinterpret_cast<decltype(::vkGetPhysicalDeviceQueueFamilyProperties) *>(pFn_vkGetInstanceProcAddr(m_hInstance, "vkGetPhysicalDeviceQueueFamilyProperties"));
		m_pFn_GetPhysicalDeviceMemoryProperties = reinterpret_cast<decltype(::vkGetPhysicalDeviceMemoryProperties) *>(pFn_vkGetInstanceProcAddr(m_hInstance, "vkGetPhysicalDeviceMemoryProperties"));
		m_pFn_GetPhysicalDeviceImageFormatProperties = reinterpret_cast<decltype(::vkGetPhysicalDeviceImageFormatProperties) *>(pFn_vkGetInstanceProcAddr(m_hInstance, "vkGetPhysicalDeviceImageFormatProperties"));
		m_pFn_CreateDevice = reinterpret_cast<decltype(::vkCreateDevice) *>(pFn_vkGetInstanceProcAddr(m_hInstance, "vkCreateDevice"));

#if defined(PTWIN32)
#if defined(PTWIN32DESKTOP)
#ifndef NDEBUG
		m_pFn_CreateDebugReportCallbackEXT = reinterpret_cast<decltype(::vkCreateDebugReportCallbackEXT) *>(pFn_vkGetInstanceProcAddr(m_hInstance, "vkCreateDebugReportCallbackEXT"));
		m_pFn_DestroyDebugReportCallbackEXT = reinterpret_cast<decltype(::vkDestroyDebugReportCallbackEXT) *>(pFn_vkGetInstanceProcAddr(m_hInstance, "vkDestroyDebugReportCallbackEXT"));
#endif
		m_pFn_CreateWin32SurfaceKHR = reinterpret_cast<decltype(::vkCreateWin32SurfaceKHR) *>(pFn_vkGetInstanceProcAddr(m_hInstance, "vkCreateWin32SurfaceKHR"));
		m_pFn_GetPhysicalDeviceWin32PresentationSupportKHR = reinterpret_cast<decltype(vkGetPhysicalDeviceWin32PresentationSupportKHR) *>(pFn_vkGetInstanceProcAddr(m_hInstance, "vkGetPhysicalDeviceWin32PresentationSupportKHR"));
#elif defined(PTWIN32RUNTIME)

#else
#error 未知的平台
#endif
#elif defined(PTPOSIX)
#if defined(PTPOSIXLINUXGLIBC)

#elif defined(PTPOSIXANDROID)
#ifndef NDEBUG
		m_pFn_CreateDebugReportCallbackEXT = reinterpret_cast<decltype(::vkCreateDebugReportCallbackEXT) *>(pFn_vkGetInstanceProcAddr(m_hInstance, "vkCreateDebugReportCallbackEXT"));
		m_pFn_DestroyDebugReportCallbackEXT = reinterpret_cast<decltype(::vkDestroyDebugReportCallbackEXT) *>(pFn_vkGetInstanceProcAddr(m_hInstance, "vkDestroyDebugReportCallbackEXT"));
#endif
		m_pFn_CreateAndroidSurfaceKHR = reinterpret_cast<decltype(::vkCreateAndroidSurfaceKHR) *>(pFn_vkGetInstanceProcAddr(m_hInstance, "vkCreateAndroidSurfaceKHR"));
#else
#error 未知的平台
#endif
#else
#error 未知的平台
#endif
		m_pFn_DestroySurfaceKHR = reinterpret_cast<decltype(::vkDestroySurfaceKHR) *>(pFn_vkGetInstanceProcAddr(m_hInstance, "vkDestroySurfaceKHR"));
#ifndef NDEBUG
		m_pFn_GetPhysicalDeviceSurfaceSupportKHR = reinterpret_cast<decltype(::vkGetPhysicalDeviceSurfaceSupportKHR) *>(pFn_vkGetInstanceProcAddr(m_hInstance, "vkGetPhysicalDeviceSurfaceSupportKHR"));
#endif
		m_pFn_GetPhysicalDeviceSurfaceCapabilitiesKHR = reinterpret_cast<decltype(::vkGetPhysicalDeviceSurfaceCapabilitiesKHR) *>(pFn_vkGetInstanceProcAddr(m_hInstance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
		m_pFn_GetPhysicalDeviceSurfaceFormatsKHR = reinterpret_cast<decltype(::vkGetPhysicalDeviceSurfaceFormatsKHR) *>(pFn_vkGetInstanceProcAddr(m_hInstance, "vkGetPhysicalDeviceSurfaceFormatsKHR"));
		m_pFn_GetPhysicalDeviceSurfacePresentModesKHR = reinterpret_cast<decltype(::vkGetPhysicalDeviceSurfacePresentModesKHR) *>(pFn_vkGetInstanceProcAddr(m_hInstance, "vkGetPhysicalDeviceSurfacePresentModesKHR"));

	}

	inline void DestroyInstance(const VkAllocationCallbacks *pAllocator)
	{
		m_pFn_DestroyInstance(m_hInstance, pAllocator);
	}

	inline VkResult EnumeratePhysicalDevices(uint32_t *pPhysicalDeviceCount, VkPhysicalDevice *pPhysicalDevices)
	{
		return m_pFn_EnumeratePhysicalDevices(m_hInstance, pPhysicalDeviceCount, pPhysicalDevices);
	}
	
	inline void GetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties *pProperties)
	{
		m_pFn_GetPhysicalDeviceProperties(physicalDevice, pProperties);
	}
	
	inline void GetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures *pFeatures)
	{
		m_pFn_GetPhysicalDeviceFeatures(physicalDevice, pFeatures);
	}

	inline void GetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice, uint32_t *pQueueFamilyPropertyCount, VkQueueFamilyProperties *pQueueFamilyProperties)
	{
		return m_pFn_GetPhysicalDeviceQueueFamilyProperties(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
	}

	inline void GetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties *pMemoryProperties)
	{
		return m_pFn_GetPhysicalDeviceMemoryProperties(physicalDevice, pMemoryProperties);
	}

	inline VkResult GetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties *pImageFormatProperties)
	{
		return m_pFn_GetPhysicalDeviceImageFormatProperties(physicalDevice, format, type, tiling, usage, flags, pImageFormatProperties);
	}

	inline VkResult CreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDevice *pDevice)
	{
		return m_pFn_CreateDevice(physicalDevice, pCreateInfo, pAllocator, pDevice);
	}

#if defined(PTWIN32)
#if defined(PTWIN32DESKTOP)
#ifndef NDEBUG
	inline VkResult CreateDebugReportCallbackEXT(const VkDebugReportCallbackCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugReportCallbackEXT *pCallback)
	{
		return m_pFn_CreateDebugReportCallbackEXT(m_hInstance, pCreateInfo, pAllocator, pCallback);
	}

	inline void DestroyDebugReportCallbackEXT(VkDebugReportCallbackEXT callback, const VkAllocationCallbacks *pAllocator)
	{
		m_pFn_DestroyDebugReportCallbackEXT(m_hInstance, callback, pAllocator);
	}
#endif
	inline VkResult CreateWin32SurfaceKHR(const VkWin32SurfaceCreateInfoKHR *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkSurfaceKHR *pSurface)
	{
		return m_pFn_CreateWin32SurfaceKHR(m_hInstance, pCreateInfo, pAllocator, pSurface);
	}

	inline VkBool32 GetPhysicalDeviceWin32PresentationSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex)
	{
		return m_pFn_GetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, queueFamilyIndex);
	}
#elif defined(PTWIN32RUNTIME)

#else
#error 未知的平台
#endif
#elif defined(PTPOSIX)
#if defined(PTPOSIXLINUXGLIBC)

#elif defined(PTPOSIXANDROID)
#ifndef NDEBUG
	inline VkResult CreateDebugReportCallbackEXT(const VkDebugReportCallbackCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugReportCallbackEXT *pCallback)
	{
		return m_pFn_CreateDebugReportCallbackEXT(m_hInstance, pCreateInfo, pAllocator, pCallback);
	}

	inline void DestroyDebugReportCallbackEXT(VkDebugReportCallbackEXT callback, const VkAllocationCallbacks *pAllocator)
	{
		m_pFn_DestroyDebugReportCallbackEXT(m_hInstance, callback, pAllocator);
	}
#endif
	inline VkResult CreateAndroidSurfaceKHR(const VkAndroidSurfaceCreateInfoKHR *pCreateInfo,const VkAllocationCallbacks *pAllocator,VkSurfaceKHR *pSurface)
	{
		return m_pFn_CreateAndroidSurfaceKHR(m_hInstance, pCreateInfo, pAllocator, pSurface);
	}
#else
#error 未知的平台
#endif
#else
#error 未知的平台
#endif

	inline void DestroySurfaceKHR(VkSurfaceKHR surface, const VkAllocationCallbacks *pAllocator)
	{
		m_pFn_DestroySurfaceKHR(m_hInstance, surface, pAllocator);
	}

#ifndef NDEBUG
	inline VkResult GetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface, VkBool32 *pSupported)
	{
		return m_pFn_GetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, pSupported);
	}
#endif

	inline VkResult GetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR *pSurfaceCapabilities)
	{
		return m_pFn_GetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, pSurfaceCapabilities);
	}

	inline VkResult GetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t *pSurfaceFormatCount, VkSurfaceFormatKHR *pSurfaceFormats)
	{
		return m_pFn_GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, pSurfaceFormatCount, pSurfaceFormats);
	}

	inline VkResult GetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t *pPresentModeCount, VkPresentModeKHR *pPresentModes)
	{
		return m_pFn_GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, pPresentModeCount, pPresentModes);
	}

};

class VkDeviceWrapper
{
private:
	VkDevice m_hDevice;

	decltype(::vkDestroyDevice) *m_pFn_DestroyDevice;
	decltype(::vkGetDeviceQueue) *m_pFn_GetDeviceQueue;
	decltype(::vkQueueSubmit) *m_pFn_QueueSubmit;
	decltype(::vkCreateSemaphore) *m_pFn_CreateSemaphore;
	decltype(::vkCreateFence) *m_pFn_CreateFence;
	decltype(::vkGetFenceStatus) *m_pFn_GetFenceStatus;
	decltype(::vkResetFences) *m_pFn_ResetFences;
	decltype(::vkCreateCommandPool) *m_pFn_CreateCommandPool;
	decltype(::vkResetCommandPool) *m_pFn_ResetCommandPool;
	decltype(::vkDestroyCommandPool) *m_pFn_DestroyCommandPool;
	decltype(::vkAllocateCommandBuffers) *m_pFn_AllocateCommandBuffers;
	//decltype(::vkResetCommandBuffer) *m_pFn_ResetCommandBuffer;
	decltype(::vkFreeCommandBuffers) *m_pFn_FreeCommandBuffers;
	decltype(::vkBeginCommandBuffer) *m_pFn_BeginCommandBuffer;
	decltype(::vkEndCommandBuffer) *m_pFn_EndCommandBuffer;
	decltype(::vkCmdPipelineBarrier) *m_pFn_CmdPipelineBarrier;
	decltype(::vkAllocateMemory) *m_pFn_AllocateMemory;
	decltype(::vkFreeMemory) *m_pFn_FreeMemory;
	decltype(::vkMapMemory) *m_pFn_MapMemory;
	decltype(::vkUnmapMemory) *m_pFn_UnmapMemory;
	decltype(::vkInvalidateMappedMemoryRanges) *m_pFn_InvalidateMappedMemoryRanges;
	decltype(::vkCreateBuffer) *m_pFn_CreateBuffer;
	decltype(::vkDestroyBuffer) *m_pFn_DestroyBuffer;
	decltype(::vkGetBufferMemoryRequirements) *m_pFn_GetBufferMemoryRequirements;
	decltype(::vkCreateImage) *m_pFn_CreateImage;
	decltype(::vkDestroyImage) *m_pFn_DestroyImage;
	decltype(::vkGetImageMemoryRequirements) *m_pFn_GetImageMemoryRequirements;
	decltype(::vkCreateImageView) *m_pFn_CreateImageView;
	decltype(::vkDestroyImageView) *m_pFn_DestroyImageView;
	decltype(::vkCreateSwapchainKHR) *m_pFn_CreateSwapchainKHR;
	decltype(::vkDestroySwapchainKHR) *m_pFn_DestroySwapchainKHR;
	decltype(::vkGetSwapchainImagesKHR) *m_pFn_GetSwapchainImagesKHR;
	decltype(::vkAcquireNextImageKHR) *m_pFn_AcquireNextImageKHR;
	decltype(::vkQueuePresentKHR) *m_pFn_QueuePresentKHR;

public:
	inline VkDeviceWrapper()
		:
		m_hDevice(VK_NULL_HANDLE),
		m_pFn_DestroyDevice(NULL),
		m_pFn_GetDeviceQueue(NULL),
		m_pFn_QueueSubmit(NULL),
		m_pFn_CreateSemaphore(NULL),
		m_pFn_CreateFence(NULL),
		m_pFn_GetFenceStatus(NULL),
		m_pFn_ResetFences(NULL),
		m_pFn_CreateCommandPool(NULL),
		m_pFn_ResetCommandPool(NULL),
		m_pFn_DestroyCommandPool(NULL),
		m_pFn_AllocateCommandBuffers(NULL),
		//m_pFn_ResetCommandBuffer(NULL),
		m_pFn_FreeCommandBuffers(NULL),
		m_pFn_BeginCommandBuffer(NULL),
		m_pFn_EndCommandBuffer(NULL),
		m_pFn_CmdPipelineBarrier(NULL),
		m_pFn_AllocateMemory(NULL),
		m_pFn_FreeMemory(NULL),
		m_pFn_MapMemory(NULL),
		m_pFn_UnmapMemory(NULL),
		m_pFn_InvalidateMappedMemoryRanges(NULL),
		m_pFn_CreateBuffer(NULL),
		m_pFn_DestroyBuffer(NULL),
		m_pFn_GetBufferMemoryRequirements(NULL),
		m_pFn_CreateImage(NULL),
		m_pFn_DestroyImage(NULL),
		m_pFn_GetImageMemoryRequirements(NULL),
		m_pFn_CreateImageView(NULL),
		m_pFn_DestroyImageView(NULL),
		m_pFn_CreateSwapchainKHR(NULL),
		m_pFn_DestroySwapchainKHR(NULL),
		m_pFn_GetSwapchainImagesKHR(NULL),
		m_pFn_AcquireNextImageKHR(NULL),
		m_pFn_QueuePresentKHR(NULL)
	{

	}

	inline VkDeviceWrapper(VkDevice hDevice, decltype(vkGetDeviceProcAddr) *pFn_vkGetDeviceProcAddr)
	{
		m_hDevice = hDevice;
		
		m_pFn_DestroyDevice= reinterpret_cast<decltype(::vkDestroyDevice) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkDestroyDevice"));
		m_pFn_GetDeviceQueue = reinterpret_cast<decltype(::vkGetDeviceQueue) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkGetDeviceQueue"));
		m_pFn_QueueSubmit = reinterpret_cast<decltype(::vkQueueSubmit) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkQueueSubmit"));
		m_pFn_CreateSemaphore = reinterpret_cast<decltype(::vkCreateSemaphore) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkCreateSemaphore"));
		m_pFn_CreateFence = reinterpret_cast<decltype(::vkCreateFence) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkCreateFence"));
		m_pFn_GetFenceStatus = reinterpret_cast<decltype(::vkGetFenceStatus) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkGetFenceStatus"));
		m_pFn_ResetFences = reinterpret_cast<decltype(::vkResetFences) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkResetFences"));
		m_pFn_CreateCommandPool = reinterpret_cast<decltype(::vkCreateCommandPool) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkCreateCommandPool"));
		m_pFn_ResetCommandPool = reinterpret_cast<decltype(::vkResetCommandPool) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkResetCommandPool"));
		m_pFn_DestroyCommandPool = reinterpret_cast<decltype(::vkDestroyCommandPool) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkDestroyCommandPool"));
		m_pFn_AllocateCommandBuffers = reinterpret_cast<decltype(::vkAllocateCommandBuffers) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkAllocateCommandBuffers"));
		//m_pFn_ResetCommandBuffer = reinterpret_cast<decltype(::vkResetCommandBuffer) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkResetCommandBuffer"));
		m_pFn_FreeCommandBuffers = reinterpret_cast<decltype(::vkFreeCommandBuffers) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkFreeCommandBuffers"));
		m_pFn_BeginCommandBuffer = reinterpret_cast<decltype(::vkBeginCommandBuffer) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkBeginCommandBuffer"));
		m_pFn_EndCommandBuffer = reinterpret_cast<decltype(::vkEndCommandBuffer) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkEndCommandBuffer"));
		m_pFn_CmdPipelineBarrier = reinterpret_cast<decltype(::vkCmdPipelineBarrier) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkCmdPipelineBarrier"));
		m_pFn_AllocateMemory = reinterpret_cast<decltype(::vkAllocateMemory) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkAllocateMemory"));
		m_pFn_FreeMemory = reinterpret_cast<decltype(::vkFreeMemory) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkFreeMemory"));
		m_pFn_MapMemory = reinterpret_cast<decltype(::vkMapMemory) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkMapMemory"));
		m_pFn_UnmapMemory = reinterpret_cast<decltype(::vkUnmapMemory) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkUnmapMemory"));
		m_pFn_InvalidateMappedMemoryRanges = reinterpret_cast<decltype(::vkInvalidateMappedMemoryRanges) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkInvalidateMappedMemoryRanges"));
		m_pFn_CreateBuffer = reinterpret_cast<decltype(::vkCreateBuffer) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkCreateBuffer"));
		m_pFn_DestroyBuffer = reinterpret_cast<decltype(::vkDestroyBuffer) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkDestroyBuffer"));
		m_pFn_GetBufferMemoryRequirements = reinterpret_cast<decltype(::vkGetBufferMemoryRequirements) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkGetBufferMemoryRequirements"));
		m_pFn_CreateImage = reinterpret_cast<decltype(::vkCreateImage) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkCreateImage"));
		m_pFn_DestroyImage = reinterpret_cast<decltype(::vkDestroyImage) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkDestroyImage"));
		m_pFn_GetImageMemoryRequirements = reinterpret_cast<decltype(::vkGetImageMemoryRequirements) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkGetImageMemoryRequirements"));
		m_pFn_CreateImageView = reinterpret_cast<decltype(::vkCreateImageView) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkCreateImageView"));
		m_pFn_DestroyImageView = reinterpret_cast<decltype(::vkDestroyImageView) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkDestroyImageView"));
		m_pFn_CreateSwapchainKHR = reinterpret_cast<decltype(::vkCreateSwapchainKHR) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkCreateSwapchainKHR"));
		m_pFn_DestroySwapchainKHR = reinterpret_cast<decltype(::vkDestroySwapchainKHR) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkDestroySwapchainKHR"));
		m_pFn_GetSwapchainImagesKHR = reinterpret_cast<decltype(::vkGetSwapchainImagesKHR) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkGetSwapchainImagesKHR"));
		m_pFn_AcquireNextImageKHR = reinterpret_cast<decltype(::vkAcquireNextImageKHR) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkAcquireNextImageKHR"));
		m_pFn_QueuePresentKHR = reinterpret_cast<decltype(::vkQueuePresentKHR) *>(pFn_vkGetDeviceProcAddr(m_hDevice, "vkQueuePresentKHR"));

	}

	inline void DestroyDevice(const VkAllocationCallbacks *pAllocator)
	{
		return m_pFn_DestroyDevice(m_hDevice, pAllocator);
	}

	inline void GetDeviceQueue(uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue *pQueue)
	{
		return m_pFn_GetDeviceQueue(m_hDevice, queueFamilyIndex, queueIndex, pQueue);
	}

	inline VkResult QueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo *pSubmits, VkFence fence)
	{
		return m_pFn_QueueSubmit(queue, submitCount, pSubmits, fence);
	}

	inline VkResult CreateSemaphore(const VkSemaphoreCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkSemaphore *pSemaphore)
	{
		return m_pFn_CreateSemaphore(m_hDevice, pCreateInfo, pAllocator, pSemaphore);
	}

	inline VkResult CreateFence(const VkFenceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkFence *pFence)
	{
		return m_pFn_CreateFence(m_hDevice, pCreateInfo, pAllocator, pFence);
	}

	inline VkResult GetFenceStatus(VkFence fence)
	{
		return m_pFn_GetFenceStatus(m_hDevice, fence);
	}

	inline VkResult ResetFences(uint32_t fenceCount, const VkFence *pFences)
	{
		return m_pFn_ResetFences(m_hDevice, fenceCount, pFences);
	}

	inline VkResult CreateCommandPool(const VkCommandPoolCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkCommandPool *pCommandPool)
	{
		return m_pFn_CreateCommandPool(m_hDevice, pCreateInfo, pAllocator, pCommandPool);
	}

	inline VkResult ResetCommandPool(VkCommandPool commandPool, VkCommandPoolResetFlags flags)
	{
		return m_pFn_ResetCommandPool(m_hDevice, commandPool, flags);
	}

	inline void DestroyCommandPool(VkCommandPool commandPool, const VkAllocationCallbacks *pAllocator)
	{
		return m_pFn_DestroyCommandPool(m_hDevice, commandPool, pAllocator);
	}

	inline VkResult AllocateCommandBuffers(const VkCommandBufferAllocateInfo *pAllocateInfo, VkCommandBuffer *pCommandBuffers)
	{
		return m_pFn_AllocateCommandBuffers(m_hDevice, pAllocateInfo, pCommandBuffers);
	}

	//inline VkResult ResetCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags)
	//{
	//	return m_pFn_ResetCommandBuffer(commandBuffer, flags);
	//}

	inline void FreeCommandBuffers(VkCommandPool commandPool, uint32_t commandBufferCount, const VkCommandBuffer *pCommandBuffers)
	{
		return m_pFn_FreeCommandBuffers(m_hDevice, commandPool, commandBufferCount, pCommandBuffers);
	}

	inline VkResult BeginCommandBuffer(VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo *pBeginInfo)
	{
		return m_pFn_BeginCommandBuffer(commandBuffer, pBeginInfo);
	}

	inline VkResult EndCommandBuffer(VkCommandBuffer commandBuffer)
	{
		return m_pFn_EndCommandBuffer(commandBuffer);
	}

	inline void CmdPipelineBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount, const VkMemoryBarrier *pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier *pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier *pImageMemoryBarriers)
	{
		return m_pFn_CmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
	}

	inline VkResult AllocateMemory(const VkMemoryAllocateInfo *pAllocateInfo, const VkAllocationCallbacks *pAllocator, VkDeviceMemory *pMemory)
	{
		
		return m_pFn_AllocateMemory(m_hDevice, pAllocateInfo, pAllocator, pMemory);
	}

	inline void FreeMemory(VkDeviceMemory memory, const VkAllocationCallbacks *pAllocator)
	{
		return m_pFn_FreeMemory(m_hDevice, memory, pAllocator);
	}

	inline VkResult MapMemory(VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void **ppData)
	{
		return m_pFn_MapMemory(m_hDevice, memory, offset, size, flags, ppData);
	}

	inline void UnmapMemory(VkDeviceMemory memory)
	{
		return m_pFn_UnmapMemory(m_hDevice, memory);
	}

	inline VkResult InvalidateMappedMemoryRanges(uint32_t memoryRangeCount, const VkMappedMemoryRange *pMemoryRanges)
	{
		return m_pFn_InvalidateMappedMemoryRanges(m_hDevice, memoryRangeCount, pMemoryRanges);
	}

	inline VkResult CreateBuffer(const VkBufferCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkBuffer *pBuffer)
	{
		return m_pFn_CreateBuffer(m_hDevice, pCreateInfo, pAllocator, pBuffer);
	}

	inline void DestroyBuffer(VkBuffer buffer, const VkAllocationCallbacks *pAllocator)
	{
		return m_pFn_DestroyBuffer(m_hDevice, buffer, pAllocator);
	}

	inline void GetBufferMemoryRequirements(VkBuffer buffer, VkMemoryRequirements *pMemoryRequirements)
	{
		return m_pFn_GetBufferMemoryRequirements(m_hDevice, buffer, pMemoryRequirements);
	}

	inline VkResult CreateImage(const VkImageCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkImage *pImage)
	{
		return m_pFn_CreateImage(m_hDevice, pCreateInfo, pAllocator, pImage);
	}

	inline void DestroyImage(VkImage image, const VkAllocationCallbacks *pAllocator)
	{
		return m_pFn_DestroyImage(m_hDevice, image, pAllocator);
	}

	inline void GetImageMemoryRequirements(VkImage image, VkMemoryRequirements *pMemoryRequirements)
	{
		return m_pFn_GetImageMemoryRequirements(m_hDevice, image, pMemoryRequirements);
	}

	inline VkResult CreateImageView(const VkImageViewCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkImageView *pView)
	{
		return m_pFn_CreateImageView(m_hDevice, pCreateInfo, pAllocator, pView);
	}

	inline void DestroyImageView(VkImageView imageView, const VkAllocationCallbacks *pAllocator)
	{
		return m_pFn_DestroyImageView(m_hDevice, imageView, pAllocator);
	}

	inline VkResult CreateSwapchainKHR(const VkSwapchainCreateInfoKHR *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkSwapchainKHR *pSwapchain)
	{
		return m_pFn_CreateSwapchainKHR(m_hDevice, pCreateInfo, pAllocator, pSwapchain);
	}

	inline void DestroySwapchainKHR(VkSwapchainKHR swapchain, const VkAllocationCallbacks *pAllocator)
	{
		return m_pFn_DestroySwapchainKHR(m_hDevice, swapchain, pAllocator);
	}

	inline VkResult GetSwapchainImagesKHR(VkSwapchainKHR swapchain, uint32_t *pSwapchainImageCount, VkImage *pSwapchainImages)
	{
		return m_pFn_GetSwapchainImagesKHR(m_hDevice, swapchain, pSwapchainImageCount, pSwapchainImages);
	}

	inline VkResult AcquireNextImageKHR(VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t *pImageIndex)
	{
		return m_pFn_AcquireNextImageKHR(m_hDevice, swapchain, timeout, semaphore, fence, pImageIndex);
	}

	inline VkResult QueuePresentKHR(VkQueue queue, const VkPresentInfoKHR *pPresentInfo)
	{
		return m_pFn_QueuePresentKHR(queue, pPresentInfo);
	}

};

#endif