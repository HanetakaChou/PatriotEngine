#include "PTGFXHALDevice.h"

#include <assert.h>

#include "../../../Public/System/PTSMemoryAllocator.h"

extern "C" PTGFXAPI PT_GFX_HAL_IDevice  * PTCALL PT_GFX_HAL_Device_Init()
{
	decltype(::vkGetInstanceProcAddr) *pFn_vkGetInstanceProcAddr = ::vkGetInstanceProcAddr;

	decltype(::vkCreateInstance) *pFn_vkCreateInstance = reinterpret_cast<decltype(::vkCreateInstance) *>(pFn_vkGetInstanceProcAddr(NULL, "vkCreateInstance"));
	assert(pFn_vkCreateInstance != NULL);

	VkInstance hInstance = VK_NULL_HANDLE;
	{
		VkInstanceCreateInfo CreateInfo;
		CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		CreateInfo.pNext = NULL;
		CreateInfo.flags = 0U;
		VkApplicationInfo ApplicationInfo;
		ApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		ApplicationInfo.pNext = NULL;
		ApplicationInfo.pApplicationName = "PTLauncher";
		ApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		ApplicationInfo.pEngineName = "PTEngine";
		ApplicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		ApplicationInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);
		CreateInfo.pApplicationInfo = &ApplicationInfo;
#if defined(PTWIN32)
#if defined(PTWIN32DESKTOP)
#ifndef NDEBUG
		//const char *EnabledLayerNames[1] = {
		//	"VK_LAYER_LUNARG_standard_validation"
		//};
		//CreateInfo.enabledLayerCount = 1U;
		char const *EnabledLayerNames[5] = {
			"VK_LAYER_GOOGLE_threading",
			"VK_LAYER_LUNARG_parameter_validation",
			"VK_LAYER_LUNARG_object_tracker",
			"VK_LAYER_LUNARG_core_validation",
			"VK_LAYER_GOOGLE_unique_objects" };
		CreateInfo.enabledLayerCount = 5U;
		CreateInfo.ppEnabledLayerNames = EnabledLayerNames;
		char const *EnabledExtensionNames[3] = {
			"VK_KHR_surface",
			"VK_KHR_win32_surface",
			"VK_EXT_debug_report" };
		CreateInfo.enabledExtensionCount = 3U;
		CreateInfo.ppEnabledExtensionNames = EnabledExtensionNames;
#else
		CreateInfo.enabledLayerCount = 0U;
		CreateInfo.ppEnabledLayerNames = NULL;
		const char *EnabledExtensionNames[2] = {
			"VK_KHR_surface",
			"VK_KHR_win32_surface" };
		CreateInfo.enabledExtensionCount = 2U;
		CreateInfo.ppEnabledExtensionNames = EnabledExtensionNames;
#endif
#elif defined(PTWIN32RUNTIME)

#else
#error 未知的平台
#endif
#elif defined(PTPOSIX)
#if defined(PTPOSIXXCB)
#ifndef NDEBUG
		//const char *EnabledLayerNames[1] = {
		//	"VK_LAYER_LUNARG_standard_validation"
		//};
		//CreateInfo.enabledLayerCount = 1U;
		char const *EnabledLayerNames[5] = {
			"VK_LAYER_GOOGLE_threading",
			"VK_LAYER_LUNARG_parameter_validation",
			"VK_LAYER_LUNARG_object_tracker",
			"VK_LAYER_LUNARG_core_validation",
			"VK_LAYER_GOOGLE_unique_objects" };
		CreateInfo.enabledLayerCount = 5U;
		CreateInfo.ppEnabledLayerNames = EnabledLayerNames;
		char const *EnabledExtensionNames[3] = {
			"VK_KHR_surface",
			"VK_KHR_xcb_surface",
			"VK_EXT_debug_report" };
		CreateInfo.enabledExtensionCount = 3U;
		CreateInfo.ppEnabledExtensionNames = EnabledExtensionNames;
#else
		CreateInfo.enabledLayerCount = 0U;
		CreateInfo.ppEnabledLayerNames = NULL;
		const char *EnabledExtensionNames[2] = {
			"VK_KHR_surface",
			"VK_KHR_xcb_surface" };
		CreateInfo.enabledExtensionCount = 2U;
		CreateInfo.ppEnabledExtensionNames = EnabledExtensionNames;
#endif
#elif defined(PTPOSIXANDROID)
#ifndef NDEBUG
		char const *EnabledLayerNames[5] = {
			"VK_LAYER_GOOGLE_threading",
			"VK_LAYER_LUNARG_parameter_validation",
			"VK_LAYER_LUNARG_object_tracker",
			"VK_LAYER_LUNARG_core_validation",
			"VK_LAYER_GOOGLE_unique_objects" };
		CreateInfo.enabledLayerCount = 5U;
		CreateInfo.ppEnabledLayerNames = EnabledLayerNames; // EnabledLayerNames;
		char const *EnabledExtensionNames[3] = {
			"VK_KHR_surface",
			"VK_KHR_android_surface",
			"VK_EXT_debug_report" };
		CreateInfo.enabledExtensionCount = 3U;
		CreateInfo.ppEnabledExtensionNames = EnabledExtensionNames;
#else
		CreateInfo.enabledLayerCount = 0U;
		CreateInfo.ppEnabledLayerNames = NULL;
		char const *EnabledExtensionNames[2] = {
			"VK_KHR_surface",
			"VK_KHR_android_surface" };
		CreateInfo.enabledExtensionCount = 2U;
		CreateInfo.ppEnabledExtensionNames = EnabledExtensionNames;
#endif
#else
#error 未知的平台
#endif
#else
#error 未知的平台
#endif

		struct LambdaVKAPI
		{
			static VKAPI_ATTR void *VKAPI_CALL Allocation(void *, size_t size, size_t alignment, VkSystemAllocationScope)
			{
				return ::PTSMemoryAllocator_Alloc_Aligned(static_cast<uint32_t>(size), static_cast<uint32_t>(alignment));
			}
			static VKAPI_ATTR void *VKAPI_CALL Reallocation(void *, void *pOriginal, size_t size, size_t alignment, VkSystemAllocationScope)
			{
				return ::PTSMemoryAllocator_Realloc_Aligned(pOriginal, static_cast<uint32_t>(size), static_cast<uint32_t>(alignment));
			}
			static VKAPI_ATTR void VKAPI_CALL Free(void *, void *pMemory)
			{
				return ::PTSMemoryAllocator_Free_Aligned(pMemory);
			}
			static VKAPI_ATTR void VKAPI_CALL InternalAllocation(void *, size_t, VkInternalAllocationType, VkSystemAllocationScope) {

			};
			static VKAPI_ATTR void VKAPI_CALL InternalFree(void *, size_t, VkInternalAllocationType, VkSystemAllocationScope) {

			};
		};

		VkAllocationCallbacks Allocator;
		Allocator.pUserData = NULL;
		Allocator.pfnAllocation = LambdaVKAPI::Allocation;
		Allocator.pfnReallocation = LambdaVKAPI::Reallocation;
		Allocator.pfnFree = LambdaVKAPI::Free;
		Allocator.pfnInternalAllocation = LambdaVKAPI::InternalAllocation;
		Allocator.pfnInternalFree = LambdaVKAPI::InternalFree;

		VkResult eResult = pFn_vkCreateInstance(&CreateInfo, &Allocator, &hInstance);
		assert(eResult == VK_SUCCESS);
	}

	return 0;
}


//Texture ----------------------------------------------------------------------------

#include"../PTGFXAssetDDS.h"

extern "C" PTGFXAPI PT_GFX_HAL_ITexture * PTCALL PT_GFX_HAL_Texture_Asset_LoadDDS(
	PT_GFX_HAL_IDevice *pDevice,
	void *fd_pUserData, intptr_t(PTPTR *fd_read)(void *fd_pUserData, void *buf, uintptr_t count), int64_t(PTPTR *fd_lseek64)(void *fd_pUserData, int64_t offset, int32_t whence), int32_t(PTPTR *fd_stat64)(void *fd_pUserData, int64_t *st_size)
)
{
	//PT_GFX_Asset_LoadDDS

	return NULL;
}