#include "PTVDInstanceImpl.h"

#include "../../../Public/McRT/PTSThread.h"
#include "../../../Public/McRT/PTSMemoryAllocator.h"

#if defined(PTWIN32)
#if defined(PTWIN32DESKTOP)
#include "../../../Public/McRT/PTSConvUTF.h"
#include <string.h>
#elif defined(PTWIN32RUNTIME)

#else
#error 未知的平台
#endif
#elif defined(PTPOSIX)
#if defined(PTPOSIXLINUXGLIBC)

#elif defined(PTPOSIXLINUXBIONIC)
#include <dlfcn.h>
#include <android/log.h>
#else
#error 未知的平台
#endif
#else
#error 未知的平台
#endif

#include <assert.h>

#include <new>

#include "../../../Public/McRT/PTSMemory.h"

#include "PTVDVKMemoryAllocator.h"

IPTVDInstance *PTCALL PTVDInstance_ForProcess()
{
	static PTVDInstanceImpl *s_InstanceImpl_Singleton_Pointer = reinterpret_cast<PTVDInstanceImpl *>(uintptr_t(0X1U)); //不可能为有效值，因为不满足Alignment
	static int32_t s_InstanceImpl_Initialize_RefCount = 0;

	PTVDInstanceImpl *pInstanceImpl;

	if (((pInstanceImpl = ::PTSAtomic_Get(&s_InstanceImpl_Singleton_Pointer)) == reinterpret_cast<PTVDInstanceImpl *>(uintptr_t(0X1U))) && (::PTSAtomic_GetAndAdd(&s_InstanceImpl_Initialize_RefCount, 1) == 0))
	{
		decltype(::vkGetInstanceProcAddr) *pFn_vkGetInstanceProcAddr = ::vkGetInstanceProcAddr;
		assert(pFn_vkGetInstanceProcAddr != NULL);

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
				"VK_LAYER_GOOGLE_unique_objects"};
			CreateInfo.ppEnabledLayerNames = EnabledLayerNames;
			char const *EnabledExtensionNames[3] = {
				"VK_KHR_surface",
				"VK_KHR_win32_surface",
				"VK_EXT_debug_report"};
			CreateInfo.enabledExtensionCount = 3U;
			CreateInfo.ppEnabledExtensionNames = EnabledExtensionNames;
#else
			CreateInfo.enabledLayerCount = 0U;
			CreateInfo.ppEnabledLayerNames = NULL;
			const char *EnabledExtensionNames[2] = {
				"VK_KHR_surface",
				"VK_KHR_win32_surface"};
			CreateInfo.enabledExtensionCount = 2U;
			CreateInfo.ppEnabledExtensionNames = EnabledExtensionNames;
#endif
#elif defined(PTWIN32RUNTIME)

#else
#error 未知的平台
#endif
#elif defined(PTPOSIX)
#if defined(PTPOSIXLINUXGLIBC)

#elif defined(PTPOSIXLINUXBIONIC)
#ifndef NDEBUG
			const char *EnabledLayerNames[5] = {
				"VK_LAYER_GOOGLE_threading",
				"VK_LAYER_LUNARG_parameter_validation",
				"VK_LAYER_LUNARG_object_tracker",
				"VK_LAYER_LUNARG_core_validation",
				"VK_LAYER_GOOGLE_unique_objects"};
			CreateInfo.enabledLayerCount = 5U;
			CreateInfo.ppEnabledLayerNames = EnabledLayerNames; // EnabledLayerNames;
			const char *EnabledExtensionNames[3] = {
				"VK_KHR_surface",
				"VK_KHR_android_surface",
				"VK_EXT_debug_report"};
			CreateInfo.enabledExtensionCount = 3U;
			CreateInfo.ppEnabledExtensionNames = EnabledExtensionNames;
#else
			CreateInfo.enabledLayerCount = 0U;
			CreateInfo.ppEnabledLayerNames = NULL;
			const char *EnabledExtensionNames[2] = {
				"VK_KHR_surface",
				"VK_KHR_android_surface"};
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
				static VKAPI_ATTR void VKAPI_CALL InternalAllocation(void *, size_t, VkInternalAllocationType, VkSystemAllocationScope){

				};
				static VKAPI_ATTR void VKAPI_CALL InternalFree(void *, size_t, VkInternalAllocationType, VkSystemAllocationScope){

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

		decltype(vkGetDeviceProcAddr) *pFn_vkGetDeviceProcAddr = reinterpret_cast<decltype(vkGetDeviceProcAddr) *>(pFn_vkGetInstanceProcAddr(hInstance, "vkGetDeviceProcAddr"));
		assert(pFn_vkGetDeviceProcAddr != NULL);

		VkInstanceWrapper hInstanceWrapper(hInstance, pFn_vkGetInstanceProcAddr);

		VkDebugReportCallbackEXT hDebugReportCallback = VK_NULL_HANDLE;
		{
#if defined(PTWIN32)
#if defined(PTWIN32DESKTOP)
#ifndef NDEBUG
			VkDebugReportCallbackCreateInfoEXT CreateInfo;
			CreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
			CreateInfo.pNext = NULL;
			CreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT; //| VK_DEBUG_REPORT_DEBUG_BIT_EXT | VK_DEBUG_REPORT_INFORMATION_BIT_EXT
			CreateInfo.pfnCallback = [](
										 VkDebugReportFlagsEXT flags,
										 VkDebugReportObjectTypeEXT,
										 uint64_t,
										 size_t,
										 int32_t,
										 const char *pLayerPrefix,
										 const char *pMessage,
										 void *) -> VkBool32 {
				wchar_t wStringOutput[0X10000];
				uint32_t Length = 0U;
				::PTS_MemoryCopy(wStringOutput + Length, L"Vulkan Debug Report: ", sizeof(wchar_t) * 21U);
				Length += 21U;

				//Flags
				::PTS_MemoryCopy(wStringOutput + Length, L"Flags: ", sizeof(wchar_t) * 7U);
				Length += 7U;
				switch (flags)
				{
				case VK_DEBUG_REPORT_ERROR_BIT_EXT:
				{
					::PTS_MemoryCopy(wStringOutput + Length, L"Error ", sizeof(wchar_t) * 6U);
					Length += 6U;
				}
				break;
				case VK_DEBUG_REPORT_WARNING_BIT_EXT:
				{
					::PTS_MemoryCopy(wStringOutput + Length, L"Warning ", sizeof(wchar_t) * 8U);
					Length += 8U;
				}
				break;
				case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
				{
					::PTS_MemoryCopy(wStringOutput + Length, L"PerformanceWarning ", sizeof(wchar_t) * 19U);
					Length += 19U;
				}
				break;
				default:
					assert(0);
				}

				//LayerPrefix
				{
					::PTS_MemoryCopy(wStringOutput + Length, L"LayerPrefix: ", sizeof(wchar_t) * 13U);
					Length += 13U;
					uint32_t InCharsLeft = static_cast<uint32_t>(::strlen(reinterpret_cast<const char *>(pLayerPrefix))); //不包括'\0'
					uint32_t OutCharsLeft = 0X10000U - 2U - Length;
					bool bResult = ::PTSConv_UTF8ToUTF16(pLayerPrefix, &InCharsLeft, reinterpret_cast<char16_t *>(wStringOutput + Length), &OutCharsLeft);
					assert(bResult);
					Length = 0X10000U - 2U - OutCharsLeft;
				}

				//Message
				{
					::PTS_MemoryCopy(wStringOutput + Length, L" Message: ", sizeof(wchar_t) * 10U);
					Length += 10U;
					uint32_t InCharsLeft = static_cast<uint32_t>(::strlen(reinterpret_cast<const char *>(pMessage))); //不包括'\0'
					uint32_t OutCharsLeft = 0X10000U - 2U - Length;
					bool bResult = ::PTSConv_UTF8ToUTF16(pMessage, &InCharsLeft, reinterpret_cast<char16_t *>(wStringOutput + Length), &OutCharsLeft);
					assert(bResult);
					Length = 0X10000U - 2U - OutCharsLeft;
				}

				wStringOutput[Length] = L'\n';
				wStringOutput[Length + 1U] = L'\0';

				::OutputDebugStringW(wStringOutput);

				return VK_FALSE;
			};
			CreateInfo.pUserData = NULL;

			VkResult eResult = hInstanceWrapper.CreateDebugReportCallbackEXT(&CreateInfo, NULL, &hDebugReportCallback);
			assert(eResult == VK_SUCCESS);
#endif
#elif defined(PTWIN32RUNTIME)

#else
#error 未知的平台
#endif
#elif defined(PTPOSIX)
#if defined(PTPOSIXLINUXGLIBC)

#elif defined(PTPOSIXLINUXBIONIC)
#ifndef NDEBUG
			struct LambdaVKAPI
			{
				static VKAPI_ATTR VkBool32 VKAPI_CALL Callback(
					VkDebugReportFlagsEXT flags,
					VkDebugReportObjectTypeEXT,
					uint64_t,
					size_t,
					int32_t,
					const char *pLayerPrefix,
					const char *pMessage,
					void *)
				{
					char StringOutput[0X10000];
					uint32_t Length = 0U;
					::PTS_MemoryCopy(StringOutput + Length, "Vulkan Debug Report: ", sizeof(char) * 21U);
					Length += 21U;

					int LogPriority;
					//Flags
					::PTS_MemoryCopy(StringOutput + Length, "Flags: ", sizeof(char) * 7U);
					Length += 7U;
					switch (flags)
					{
					case VK_DEBUG_REPORT_ERROR_BIT_EXT:
					{
						::PTS_MemoryCopy(StringOutput + Length, "Error ", sizeof(char) * 6U);
						Length += 6U;
						LogPriority = ANDROID_LOG_ERROR;
					}
					break;
					case VK_DEBUG_REPORT_WARNING_BIT_EXT:
					{
						::PTS_MemoryCopy(StringOutput + Length, "Warning ", sizeof(char) * 8U);
						Length += 8U;
						LogPriority = ANDROID_LOG_WARN;
					}
					break;
					case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
					{
						::PTS_MemoryCopy(StringOutput + Length, "PerformanceWarning ", sizeof(char) * 19U);
						Length += 19U;
						LogPriority = ANDROID_LOG_WARN;
					}
					break;
					default:
						assert(0);
					}

					//LayerPrefix
					{
						::PTS_MemoryCopy(StringOutput + Length, "LayerPrefix: ", sizeof(char) * 13U);
						Length += 13U;
						uint32_t InCharsLeft = static_cast<uint32_t>(::strlen(reinterpret_cast<const char *>(pLayerPrefix))); //不包括'\0'
						uint32_t OutCharsLeft = 0X10000U - 2U - Length;
						uint32_t NumCopy = (InCharsLeft < OutCharsLeft) ? InCharsLeft : OutCharsLeft;
						::PTS_MemoryCopy(StringOutput + Length, pLayerPrefix, NumCopy);
						Length += NumCopy;
					}

					//Message
					{
						::PTS_MemoryCopy(StringOutput + Length, " Message: ", sizeof(char) * 10U);
						Length += 10U;
						uint32_t InCharsLeft = static_cast<uint32_t>(::strlen(reinterpret_cast<const char *>(pMessage))); //不包括'\0'
						uint32_t OutCharsLeft = 0X10000U - 2U - Length;
						uint32_t NumCopy = (InCharsLeft < OutCharsLeft) ? InCharsLeft : OutCharsLeft;
						::PTS_MemoryCopy(StringOutput + Length, pMessage, NumCopy);
						Length += NumCopy;
					}

					StringOutput[Length] = L'\n';
					StringOutput[Length + 1U] = L'\0';

					::__android_log_write(LogPriority, "PTEngine", StringOutput);
					return VK_FALSE;
				}
			};

			VkDebugReportCallbackCreateInfoEXT CreateInfo;
			CreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
			CreateInfo.pNext = NULL;
			CreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT; //| VK_DEBUG_REPORT_DEBUG_BIT_EXT | VK_DEBUG_REPORT_INFORMATION_BIT_EXT
			CreateInfo.pfnCallback = LambdaVKAPI::Callback;
			CreateInfo.pUserData = NULL;

			VkResult eResult = hInstanceWrapper.CreateDebugReportCallbackEXT(&CreateInfo, NULL, &hDebugReportCallback);
			assert(eResult == VK_SUCCESS);
#endif
#else
#error 未知的平台
#endif
#else
#error 未知的平台
#endif
		}

		uint32_t PhysicalDeviceCount;
		{
			VkResult eResult = hInstanceWrapper.EnumeratePhysicalDevices(&PhysicalDeviceCount, NULL);
			assert(eResult == VK_SUCCESS);
		}

		VkPhysicalDevice *pPhysicalDevices;
		{
			pPhysicalDevices = static_cast<VkPhysicalDevice *>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(VkPhysicalDevice) * PhysicalDeviceCount, alignof(VkPhysicalDevice)));
			VkResult eResult = hInstanceWrapper.EnumeratePhysicalDevices(&PhysicalDeviceCount, pPhysicalDevices);
			assert(eResult == VK_SUCCESS);
		}

		for (uint32_t uiPhysicalDevice = 0U; uiPhysicalDevice < PhysicalDeviceCount; ++uiPhysicalDevice)
		{
			//Device--------------------------------------------
			VkDevice hDevice = VK_NULL_HANDLE;
			VkDeviceWrapper hDeviceWrapper;
			{
				VkDeviceCreateInfo CreateInfo;
				CreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
				CreateInfo.pNext = NULL;
				CreateInfo.flags = 0U;
				CreateInfo.enabledLayerCount = 0U;
				CreateInfo.ppEnabledLayerNames = NULL;
				CreateInfo.enabledExtensionCount = 1U;

				const char *EnabledExtensionNames[1] = {"VK_KHR_swapchain"};
				CreateInfo.ppEnabledExtensionNames = EnabledExtensionNames;

				VkPhysicalDeviceFeatures SupportedFeatures;
				hInstanceWrapper.GetPhysicalDeviceFeatures(pPhysicalDevices[uiPhysicalDevice], &SupportedFeatures);

				VkPhysicalDeviceFeatures EnabledFeatures = {}; //结构体中的各个成员都初始化为VK_FALSE
				assert(VK_FALSE == 0);

				//EnabledFeatures.geometryShader = VK_FALSE; //与Metal一致 //GeometryShader被认为是低效的，可以尝试用DrawInstanced取代 //同时StreamOut完全可以被ComputeShader取代
				//EnabledFeatures.tessellationShader = VK_TRUE;
#if defined(PTWIN32)
#if defined(PTWIN32DESKTOP)
				if (SupportedFeatures.textureCompressionBC != VK_TRUE)
				{
					continue;
				}
				//EnabledFeatures.textureCompressionETC2 = VK_FALSE; //For Android
				//EnabledFeatures.textureCompressionASTC_LDR = VK_FALSE; //For OSX/IOS
				EnabledFeatures.textureCompressionBC = VK_TRUE;
				//BC6H_UFLOAT : R16 G16 B16 UFLOAT (HDR)
				//BC6H_SFLOAT : R16 G16 B16 SFLOAT (HDR)
				//DXGI_FORMAT_BC7_UNORM : B4-7 G4-7 R4-7 A0-8 UNORM
				//DXGI_FORMAT_BC7_UNORM_SRGB : B4-7 G4-7 R4-7 A0-8 UNORM
#elif defined(PTWIN32RUNTIME)
				if (SupportedFeatures.textureCompressionBC != VK_TRUE)
				{
					continue;
				}
				//EnabledFeatures.textureCompressionETC2 = VK_FALSE; //For Android
				//EnabledFeatures.textureCompressionASTC_LDR = VK_FALSE; //For OSX/IOS
				EnabledFeatures.textureCompressionBC = VK_TRUE;
				//BC6H_UFLOAT : R16 G16 B16 UFLOAT (HDR)
				//BC6H_SFLOAT : R16 G16 B16 SFLOAT (HDR)
				//DXGI_FORMAT_BC7_UNORM : B4-7 G4-7 R4-7 A0-8 UNORM
				//DXGI_FORMAT_BC7_UNORM_SRGB : B4-7 G4-7 R4-7 A0-8 UNORM
#else
#error 未知的平台
#endif
#elif defined(PTPOSIX)
#if defined(PTPOSIXLINUXGLIBC)
				if (SupportedFeatures.textureCompressionBC != VK_TRUE)
				{
					continue;
				}
				//EnabledFeatures.textureCompressionETC2 = VK_FALSE; //For Android
				//EnabledFeatures.textureCompressionASTC_LDR = VK_FALSE; //For OSX/IOS
				EnabledFeatures.textureCompressionBC = VK_TRUE;
				//BC6H_UFLOAT : R16 G16 B16 UFLOAT (HDR)
				//BC6H_SFLOAT : R16 G16 B16 SFLOAT (HDR)
				//DXGI_FORMAT_BC7_UNORM : B4-7 G4-7 R4-7 A0-8 UNORM
				//DXGI_FORMAT_BC7_UNORM_SRGB : B4-7 G4-7 R4-7 A0-8 UNORM
#elif defined(PTPOSIXLINUXBIONIC)
				EnabledFeatures.textureCompressionASTC_LDR = VK_TRUE; //Mobile Standard //IOS Provides
																	  //EnabledFeatures.textureCompressionBC = VK_FALSE; //For PC
																	  //BC6H_UFLOAT : R16 G16 B16 UFLOAT (HDR)
																	  //BC6H_SFLOAT : R16 G16 B16 SFLOAT (HDR)
																	  //DXGI_FORMAT_BC7_UNORM : B4-7 G4-7 R4-7 A0-8 UNORM
																	  //DXGI_FORMAT_BC7_UNORM_SRGB : B4-7 G4-7 R4-7 A0-8 UNORM
#else
#error 未知的平台
#endif
#else
#error 未知的平台
#endif
				CreateInfo.pEnabledFeatures = &EnabledFeatures;

				uint32_t uiQueueFamilyIndex_3D = 0;
				uint32_t uiQueueFamilyIndex_Copy = 1;

				float QueuePriorities_3D[1] = {0.0f};
				float QueuePriorities_Copy[1] = {0.0f};

				VkDeviceQueueCreateInfo QueueCreateInfos[2] = {{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, NULL, 0U, uiQueueFamilyIndex_3D, 1U, QueuePriorities_3D},
															   {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, NULL, 0U, uiQueueFamilyIndex_Copy, 1U, QueuePriorities_Copy}};
				CreateInfo.queueCreateInfoCount = 1U;
				CreateInfo.pQueueCreateInfos = QueueCreateInfos;

				VkResult eResult = hInstanceWrapper.CreateDevice(pPhysicalDevices[uiPhysicalDevice], &CreateInfo, NULL, &hDevice);
				assert(eResult == VK_SUCCESS);
				if (!(eResult == VK_SUCCESS && hDevice != VK_NULL_HANDLE))
				{
					continue;
				}

				hDeviceWrapper = VkDeviceWrapper(hDevice, pFn_vkGetDeviceProcAddr);

				bool HostVector[PTVD_MEMORYTYPE_COUNT];
				VkFormat TextureDS_SR_Format;
				VkFormat TextureDS_Format;

				HWND

					PTVD_MemoryAllocator *pAllocator_PTVD = ::PTVD_MemoryAllocator_Create(hInstanceWrapper, hDeviceWrapper);
				::PTVD_MemoryAllocator_Alloc(pAllocator_PTVD, PTVD_MEMORYTYPE_TEXTURE_SHADERRESOURCE, 1024U * 8192U);

				PTVD_DeviceMemoryManager m_DeviceMemoryManager;
				m_DeviceMemoryManager.Construct(hInstanceWrapper, hDeviceWrapper, pPhysicalDevices[uiPhysicalDevice], HostVector, &TextureDS_SR_Format, &TextureDS_Format);

				VkDeviceMemory hMemory;
				VkDeviceSize BlockSize;
				void *pBlockAddress;
				m_DeviceMemoryManager.Alloc(hDeviceWrapper, &hMemory, &BlockSize, &pBlockAddress, PTVD_MEMORYTYPE_BUFFER_CONSTANT);

				m_DeviceMemoryManager.Free(hDeviceWrapper, hMemory, true);

				VkQueue hQueue_3D0 = VK_NULL_HANDLE;
				hDeviceWrapper.GetDeviceQueue(uiQueueFamilyIndex_3D, 0U, &hQueue_3D0);
				assert(hQueue_3D0 != VK_NULL_HANDLE);

				//VkQueue hQueue_Copy0 = VK_NULL_HANDLE;
				//hDeviceWrapper.GetDeviceQueue(uiQueueFamilyIndex_Copy, 0U, &hQueue_Copy0);
				//assert(hQueue_Copy0 != VK_NULL_HANDLE);
			}
		}

#if 0
		//↓DeviceNUMA And DeviceUMA -------------------------------------
		::PTS_CPP_Vector<::PTS_CPP_String> NUMADeviceNameS;
		::PTS_CPP_Vector<::IPTVD_Device *> NUMADevicePointerS;
		::PTS_CPP_Vector<::PTS_CPP_String> UMADeviceNameS;
		::PTS_CPP_Vector<::IPTVD_Device *> UMADevicePointerS;
		{
			uint32_t PhysicalDeviceCount;
			{
				VkResult eResult = hInstanceWrapper.EnumeratePhysicalDevices(&PhysicalDeviceCount, NULL);
				assert(eResult == VK_SUCCESS);
			}

			VkPhysicalDevice *pPhysicalDevices;
			{
				pPhysicalDevices = static_cast<VkPhysicalDevice *>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(VkPhysicalDevice)*PhysicalDeviceCount, alignof(VkPhysicalDevice)));
				VkResult eResult = hInstanceWrapper.EnumeratePhysicalDevices(&PhysicalDeviceCount, pPhysicalDevices);
				assert(eResult == VK_SUCCESS);
			}

			for (uint32_t uiPhysicalDevice = 0U; uiPhysicalDevice < PhysicalDeviceCount; ++uiPhysicalDevice)
			{
				//↓CommandQueue -------------------------------------

				//Synchronization and Multi-Engine
				//https://msdn.microsoft.com/en-us/library/windows/desktop/dn899217(v=vs.85).aspx

				//Direct3D12规范
				//Type
				//D3D12_COMMAND_LIST_TYPE_DIRECT //支持图形、计算、复制、呈现
				//D3D12_COMMAND_LIST_TYPE_COMPUTE //计算、复制
				//D3D12_COMMAND_LIST_TYPE_COPY //复制
				//Type Class
				//Compute/3D
				//Copy

				//Vulkan规范
				//All commands that are allowed on a queue that supports transfer operations
				//are also allowed on a queue that supports either graphics or compute operations.
				//Thus, if the capabilities of a queue family include VK_QUEUE_GRAPHICS_BIT or VK_QUEUE_COMPUTE_BIT,
				//then reporting the VK_QUEUE_TRANSFER_BIT capability separately for that queue family is optional.

				uint32_t uiQueueFamilyIndex_3D = VK_QUEUE_FAMILY_IGNORED;
				uint32_t uiQueueCount_3D = 0U;
				uint32_t uiQueueFamilyIndex_Compute = VK_QUEUE_FAMILY_IGNORED;
				uint32_t uiQueueCount_Compute = 0U;
				uint32_t uiQueueFamilyIndex_Copy = VK_QUEUE_FAMILY_IGNORED;
				uint32_t uiQueueCount_Copy = 0U;
				{
					uint32_t QueueFamilyPropertyCount;
					hInstanceWrapper.GetPhysicalDeviceQueueFamilyProperties(pPhysicalDevices[uiPhysicalDevice], &QueueFamilyPropertyCount, NULL);
					VkQueueFamilyProperties *pQueueFamilyProperties = static_cast<VkQueueFamilyProperties *>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(VkQueueFamilyProperties)*QueueFamilyPropertyCount, alignof(VkQueueFamilyProperties)));
					hInstanceWrapper.GetPhysicalDeviceQueueFamilyProperties(pPhysicalDevices[uiPhysicalDevice], &QueueFamilyPropertyCount, pQueueFamilyProperties);

					for (uint32_t uiQueueFamilyIndex = 0U; (uiQueueFamilyIndex_3D == VK_QUEUE_FAMILY_IGNORED || uiQueueFamilyIndex_Compute == VK_QUEUE_FAMILY_IGNORED || uiQueueCount_Copy == VK_QUEUE_FAMILY_IGNORED) && (uiQueueFamilyIndex < QueueFamilyPropertyCount); ++uiQueueFamilyIndex)
					{

						if ((pQueueFamilyProperties[uiQueueFamilyIndex].queueFlags&VK_QUEUE_GRAPHICS_BIT) && (pQueueFamilyProperties[uiQueueFamilyIndex].queueFlags&VK_QUEUE_COMPUTE_BIT))
						{
#if defined(PTWIN32)
#if defined(PTWIN32DESKTOP)
							assert(hInstanceWrapper.GetPhysicalDeviceWin32PresentationSupportKHR(pPhysicalDevices[uiPhysicalDevice], uiQueueFamilyIndex) == VK_TRUE);
							if (hInstanceWrapper.GetPhysicalDeviceWin32PresentationSupportKHR(pPhysicalDevices[uiPhysicalDevice], uiQueueFamilyIndex) == VK_TRUE)
							{
								if (uiQueueFamilyIndex_3D == VK_QUEUE_FAMILY_IGNORED)
								{
									uiQueueFamilyIndex_3D = uiQueueFamilyIndex;
									uiQueueCount_3D = pQueueFamilyProperties[uiQueueFamilyIndex].queueCount;
								}
							}
#elif defined(PTWIN32RUNTIME)

#else
#error 未知的平台
#endif
#elif defined(PTPOSIX)
#if defined(PTPOSIXLINUXGLIBC)

#elif defined(PTPOSIXLINUXBIONIC)
							//Vulkan规范
							//VK_KHR_android_surface
							//Issues
							//1) Does Android need a way to query for compatibility 
							//between a particular physical device(and queue family?) and a specific Android display?
							//RESOLVED : No.
							//Currently on Android, any physical device is expected to be able to present to the system compositor,
							//and all queue families must support the necessary image layout transitions and synchronization operations.

							//一般认为Android不应当有独显？？？
							//模拟器？？？

							if (uiQueueFamilyIndex_3D == VK_QUEUE_FAMILY_IGNORED)
							{
								uiQueueFamilyIndex_3D = uiQueueFamilyIndex;
								uiQueueCount_3D = pQueueFamilyProperties[uiQueueFamilyIndex].queueCount;
							}
#else
#error 未知的平台
#endif
#else
#error 未知的平台
#endif
						}
						else if ((pQueueFamilyProperties[uiQueueFamilyIndex].queueFlags&VK_QUEUE_COMPUTE_BIT))
						{
							if (uiQueueFamilyIndex_Compute == VK_QUEUE_FAMILY_IGNORED)
							{
								uiQueueFamilyIndex_Compute = uiQueueFamilyIndex;
								uiQueueCount_Compute = pQueueFamilyProperties[uiQueueFamilyIndex].queueCount;
							}
						}
						else if (pQueueFamilyProperties[uiQueueFamilyIndex].queueFlags&VK_QUEUE_TRANSFER_BIT)
						{
							if (uiQueueFamilyIndex_Copy == VK_QUEUE_FAMILY_IGNORED)
							{
								uiQueueFamilyIndex_Copy = uiQueueFamilyIndex;
								uiQueueCount_Copy = pQueueFamilyProperties[uiQueueFamilyIndex].queueCount;
							}
						}
					}

					::PTSMemoryAllocator_Free_Aligned(pQueueFamilyProperties);
				}

				assert(uiQueueFamilyIndex_3D != VK_QUEUE_FAMILY_IGNORED);
				if (uiQueueFamilyIndex_3D == VK_QUEUE_FAMILY_IGNORED)
				{
					//不符合Direct3D12规范
					continue;
				}

				//Device--------------------------------------------
				VkDevice hDevice = VK_NULL_HANDLE;
				VkDeviceWrapper hDeviceWrapper;
				IPTVD_QueueManager *pCommandQueueManager = NULL;
				{
					VkDeviceCreateInfo CreateInfo;
					CreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
					CreateInfo.pNext = NULL;
					CreateInfo.flags = 0U;
					CreateInfo.enabledLayerCount = 0U;
					CreateInfo.ppEnabledLayerNames = NULL;
					CreateInfo.enabledExtensionCount = 1U;

					const char *EnabledExtensionNames[1] = { "VK_KHR_swapchain" };
					CreateInfo.ppEnabledExtensionNames = EnabledExtensionNames;

					VkPhysicalDeviceFeatures SupportedFeatures;
					hInstanceWrapper.GetPhysicalDeviceFeatures(pPhysicalDevices[uiPhysicalDevice], &SupportedFeatures);

					VkPhysicalDeviceFeatures EnabledFeatures = { //结构体中的各个成员都初始化为VK_FALSE
					};
					assert(VK_FALSE == 0);

					//EnabledFeatures.geometryShader = VK_FALSE; //与Metal一致 //GeometryShader被认为是低效的，可以尝试用DrawInstanced取代 //同时StreamOut完全可以被ComputeShader取代
					//EnabledFeatures.tessellationShader = VK_TRUE;
#if defined(PTWIN32)
#if defined(PTWIN32DESKTOP)
					if (SupportedFeatures.textureCompressionBC != VK_TRUE)
					{
						continue;
					}
					//EnabledFeatures.textureCompressionETC2 = VK_FALSE; //For Android
					//EnabledFeatures.textureCompressionASTC_LDR = VK_FALSE; //For OSX/IOS
					EnabledFeatures.textureCompressionBC = VK_TRUE;
					//BC6H_UFLOAT : R16 G16 B16 UFLOAT (HDR) 
					//BC6H_SFLOAT : R16 G16 B16 SFLOAT (HDR)
					//DXGI_FORMAT_BC7_UNORM : B4-7 G4-7 R4-7 A0-8 UNORM
					//DXGI_FORMAT_BC7_UNORM_SRGB : B4-7 G4-7 R4-7 A0-8 UNORM
#elif defined(PTWIN32RUNTIME)
					if (SupportedFeatures.textureCompressionBC != VK_TRUE)
					{
						continue;
					}
					//EnabledFeatures.textureCompressionETC2 = VK_FALSE; //For Android
					//EnabledFeatures.textureCompressionASTC_LDR = VK_FALSE; //For OSX/IOS
					EnabledFeatures.textureCompressionBC = VK_TRUE;
					//BC6H_UFLOAT : R16 G16 B16 UFLOAT (HDR) 
					//BC6H_SFLOAT : R16 G16 B16 SFLOAT (HDR)
					//DXGI_FORMAT_BC7_UNORM : B4-7 G4-7 R4-7 A0-8 UNORM
					//DXGI_FORMAT_BC7_UNORM_SRGB : B4-7 G4-7 R4-7 A0-8 UNORM
#else
#error 未知的平台
#endif
#elif defined(PTPOSIX)
#if defined(PTPOSIXLINUXGLIBC)
					if (SupportedFeatures.textureCompressionBC != VK_TRUE)
					{
						continue;
					}
					//EnabledFeatures.textureCompressionETC2 = VK_FALSE; //For Android
					//EnabledFeatures.textureCompressionASTC_LDR = VK_FALSE; //For OSX/IOS
					EnabledFeatures.textureCompressionBC = VK_TRUE;
					//BC6H_UFLOAT : R16 G16 B16 UFLOAT (HDR) 
					//BC6H_SFLOAT : R16 G16 B16 SFLOAT (HDR)
					//DXGI_FORMAT_BC7_UNORM : B4-7 G4-7 R4-7 A0-8 UNORM
					//DXGI_FORMAT_BC7_UNORM_SRGB : B4-7 G4-7 R4-7 A0-8 UNORM
#elif defined(PTPOSIXLINUXBIONIC)
					EnabledFeatures.textureCompressionETC2 = VK_TRUE;
					//EnabledFeatures.textureCompressionASTC_LDR = VK_FALSE; //For OSX/IOS
					//EnabledFeatures.textureCompressionBC = VK_FALSE; //For PC
					//BC6H_UFLOAT : R16 G16 B16 UFLOAT (HDR) 
					//BC6H_SFLOAT : R16 G16 B16 SFLOAT (HDR)
					//DXGI_FORMAT_BC7_UNORM : B4-7 G4-7 R4-7 A0-8 UNORM
					//DXGI_FORMAT_BC7_UNORM_SRGB : B4-7 G4-7 R4-7 A0-8 UNORM
#else
#error 未知的平台
#endif
#else
#error 未知的平台
#endif
					CreateInfo.pEnabledFeatures = &EnabledFeatures;

					if (uiQueueCount_Copy > 0U)
					{
						//PTVD_QueueManager_3D0_Copy0

						//D3D12_COMMAND_QUEUE_PRIORITY_NORMAL
						float QueuePriorities_3D[1] = { 0.0f };
						float QueuePriorities_Copy[1] = { 0.0f };

						VkDeviceQueueCreateInfo QueueCreateInfos[2] = { { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, NULL, 0U, uiQueueFamilyIndex_3D, 1U, QueuePriorities_3D },
						{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, NULL, 0U, uiQueueFamilyIndex_Copy, 1U, QueuePriorities_Copy } };
						CreateInfo.queueCreateInfoCount = 2U;
						CreateInfo.pQueueCreateInfos = QueueCreateInfos;

						VkResult eResult = hInstanceWrapper.CreateDevice(pPhysicalDevices[uiPhysicalDevice], &CreateInfo, NULL, &hDevice);
						assert(eResult == VK_SUCCESS);
						if (!(eResult == VK_SUCCESS && hDevice != VK_NULL_HANDLE))
						{
							continue;
						}

						hDeviceWrapper = VkDeviceWrapper(hDevice, pFn_vkGetDeviceProcAddr);

						VkQueue hQueue_3D0 = VK_NULL_HANDLE;
						hDeviceWrapper.GetDeviceQueue(uiQueueFamilyIndex_3D, 0U, &hQueue_3D0);
						assert(hQueue_3D0 != VK_NULL_HANDLE);

						VkQueue hQueue_Copy0 = VK_NULL_HANDLE;
						hDeviceWrapper.GetDeviceQueue(uiQueueFamilyIndex_Copy, 0U, &hQueue_Copy0);
						assert(hQueue_Copy0 != VK_NULL_HANDLE);

						void *pCommandQueueManagerVoid = ::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTVD_QueueManager_3D0_Copy0), alignof(PTVD_QueueManager_3D0_Copy0));
						assert(pCommandQueueManagerVoid != NULL);

						pCommandQueueManager = new(pCommandQueueManagerVoid)PTVD_QueueManager_3D0_Copy0(uiQueueFamilyIndex_3D, uiQueueFamilyIndex_Copy, hQueue_3D0, hQueue_Copy0);
					}
					else
					{
						if (uiQueueCount_Compute > 0U)
						{
							//PTVD_QueueManager_3D0_Compute0

							//No Lock
							//Queue0 Only Accessed By Render Thread
							//Queue1 Only Accessed By Streaming Thread

							//D3D12_COMMAND_QUEUE_PRIORITY_NORMAL
							float QueuePriorities_3D[1] = { 0.0f };
							float QueuePriorities_Compute[1] = { 0.0f };

							VkDeviceQueueCreateInfo QueueCreateInfos[2] = { { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, NULL, 0U, uiQueueFamilyIndex_3D, 1U, QueuePriorities_3D },
							{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, NULL, 0U, uiQueueFamilyIndex_Compute, 1U, QueuePriorities_Compute } };
							CreateInfo.queueCreateInfoCount = 2U;
							CreateInfo.pQueueCreateInfos = QueueCreateInfos;

							VkResult eResult = hInstanceWrapper.CreateDevice(pPhysicalDevices[uiPhysicalDevice], &CreateInfo, NULL, &hDevice);
							assert(eResult == VK_SUCCESS);
							if (!(eResult == VK_SUCCESS && hDevice != VK_NULL_HANDLE))
							{
								continue;
							}

							hDeviceWrapper = VkDeviceWrapper(hDevice, pFn_vkGetDeviceProcAddr);

							VkQueue hQueue_3D0 = VK_NULL_HANDLE;
							hDeviceWrapper.GetDeviceQueue(uiQueueFamilyIndex_3D, 0U, &hQueue_3D0);
							assert(hQueue_3D0 != VK_NULL_HANDLE);

							VkQueue hQueue_Compute0 = VK_NULL_HANDLE;
							hDeviceWrapper.GetDeviceQueue(uiQueueFamilyIndex_Compute, 0U, &hQueue_Compute0);
							assert(hQueue_Compute0 != VK_NULL_HANDLE);

							void *pCommandQueueManagerVoid = ::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTVD_QueueManager_3D0_Compute0), alignof(PTVD_QueueManager_3D0_Compute0));
							assert(pCommandQueueManagerVoid != NULL);

							pCommandQueueManager = new(pCommandQueueManagerVoid)PTVD_QueueManager_3D0_Compute0(uiQueueFamilyIndex_3D, uiQueueFamilyIndex_Compute, hQueue_3D0, hQueue_Compute0);
						}
						else
						{
							if (uiQueueCount_3D > 1U)
							{
								//PTVD_QueueManager_3D0_3D1

								//No Lock
								//Queue0 Only Accessed By Render Thread
								//Queue1 Only Accessed By Streaming Thread

								//D3D12_COMMAND_QUEUE_PRIORITY_NORMAL
								float QueuePriorities_3D[2] = { 0.0f,0.0f };

								VkDeviceQueueCreateInfo QueueCreateInfos[1] = { { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, NULL, 0U, uiQueueFamilyIndex_3D, 2U, QueuePriorities_3D } };
								CreateInfo.queueCreateInfoCount = 1U;
								CreateInfo.pQueueCreateInfos = QueueCreateInfos;

								VkResult eResult = hInstanceWrapper.CreateDevice(pPhysicalDevices[uiPhysicalDevice], &CreateInfo, NULL, &hDevice);
								assert(eResult == VK_SUCCESS);
								if (!(eResult == VK_SUCCESS && hDevice != VK_NULL_HANDLE))
								{
									continue;
								}

								hDeviceWrapper = VkDeviceWrapper(hDevice, pFn_vkGetDeviceProcAddr);

								VkQueue hQueue_3D0 = VK_NULL_HANDLE;
								hDeviceWrapper.GetDeviceQueue(uiQueueFamilyIndex_3D, 0U, &hQueue_3D0);
								assert(hQueue_3D0 != VK_NULL_HANDLE);

								VkQueue hQueue_3D1 = VK_NULL_HANDLE;
								hDeviceWrapper.GetDeviceQueue(uiQueueFamilyIndex_3D, 1U, &hQueue_3D1);
								assert(hQueue_3D1 != VK_NULL_HANDLE);

								void *pCommandQueueManagerVoid = ::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTVD_QueueManager_3D0_3D1), alignof(PTVD_QueueManager_3D0_3D1));
								assert(pCommandQueueManagerVoid != NULL);

								pCommandQueueManager = new(pCommandQueueManagerVoid)PTVD_QueueManager_3D0_3D1(uiQueueFamilyIndex_3D, hQueue_3D0, hQueue_3D1);
							}
							else
							{
								//PTVD_QueueManager_3D0_3D0

								//Lock

								//D3D12_COMMAND_QUEUE_PRIORITY_NORMAL
								float QueuePriorities_3D[1] = { 0.0f };

								VkDeviceQueueCreateInfo QueueCreateInfos[1] = { { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, NULL, 0U, uiQueueFamilyIndex_3D, 1U, QueuePriorities_3D } };
								CreateInfo.queueCreateInfoCount = 1U;
								CreateInfo.pQueueCreateInfos = QueueCreateInfos;

								VkResult eResult = hInstanceWrapper.CreateDevice(pPhysicalDevices[uiPhysicalDevice], &CreateInfo, NULL, &hDevice);
								assert(eResult == VK_SUCCESS);
								if (!(eResult == VK_SUCCESS && hDevice != VK_NULL_HANDLE))
								{
									continue;
								}

								hDeviceWrapper = VkDeviceWrapper(hDevice, pFn_vkGetDeviceProcAddr);

								VkQueue hQueue_3D0 = VK_NULL_HANDLE;
								hDeviceWrapper.GetDeviceQueue(uiQueueFamilyIndex_3D, 0U, &hQueue_3D0);
								assert(hQueue_3D0 != VK_NULL_HANDLE);

								void *pCommandQueueManagerVoid = ::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTVD_QueueManager_3D0_3D0), alignof(PTVD_QueueManager_3D0_3D0));
								assert(pCommandQueueManagerVoid != NULL);

								pCommandQueueManager = new(pCommandQueueManagerVoid)PTVD_QueueManager_3D0_3D0(uiQueueFamilyIndex_3D, hQueue_3D0);
							}
						}
					}
				}

				//For continue
				class __Type_SafeRelease_CommandQueue
				{
					VkDevice &m_r_hDevice;
					VkDeviceWrapper &m_r_hDeviceWrapper;
					IPTVD_QueueManager * &m_r_pCommandQueueManager;

				public:
					inline __Type_SafeRelease_CommandQueue(
						VkDevice &r_pDevice,
						VkDeviceWrapper &r_hDeviceWrapper,
						IPTVD_QueueManager * &r_pCommandQueueManager
					)
						:
						m_r_hDevice(r_pDevice),
						m_r_hDeviceWrapper(r_hDeviceWrapper),
						m_r_pCommandQueueManager(r_pCommandQueueManager)
					{

					}

					inline ~__Type_SafeRelease_CommandQueue()
					{
						if (m_r_pCommandQueueManager != NULL)
						{
							//r_pCommandQueueManager->Release();
							assert(0);
						}

						if (m_r_hDevice != VK_NULL_HANDLE)
						{
							m_r_hDeviceWrapper.DestroyDevice(NULL);
							assert(0);
						}
					}
				} __Variable_SafeRelease_CommandQueue(hDevice, hDeviceWrapper, pCommandQueueManager);

				//↓Format --------------------------------------------
				VkFormat Format_GBufferColor = VK_FORMAT_UNDEFINED;
				VkFormat Format_GBufferNormal = VK_FORMAT_UNDEFINED;
				VkFormat Format_Depth = VK_FORMAT_UNDEFINED;
				{
					VkImageFormatProperties ImageFormatProperties;

					//Format_GBufferColor --------------------------------------------

					Format_GBufferColor = VK_FORMAT_A2B10G10R10_UNORM_PACK32; //HDR Display

					VkResult eResultHDR = hInstanceWrapper.GetPhysicalDeviceImageFormatProperties(
						pPhysicalDevices[uiPhysicalDevice],
						Format_GBufferColor,
						VK_IMAGE_TYPE_2D,
						VK_IMAGE_TILING_OPTIMAL,
						//VK_IMAGE_USAGE_TRANSFER_SRC_BIT | 
						//VK_IMAGE_USAGE_TRANSFER_DST_BIT |
						//VK_IMAGE_USAGE_SAMPLED_BIT | //SRV //Replaced By InputAttachment In Vulkan
						//VK_IMAGE_USAGE_STORAGE_BIT | //UAV //Direct3D11 Not Allow Both UAV And RTV //Direct3D12 Use Barrier To Avoid Track State Thus Allow
						VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | //RTV
						VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, //Replace SRV For Mobile Tile Architecture //D3D12_FEATURE_DATA_ARCHITECTURE::TileBasedRenderer
						0U,
						&ImageFormatProperties
					);

					if (eResultHDR != VK_SUCCESS)
					{
						assert(eResultHDR == VK_ERROR_FORMAT_NOT_SUPPORTED);

						Format_GBufferColor = VK_FORMAT_A8B8G8R8_UNORM_PACK32; //LDR Display

						VkResult eResultLDR_BGRA = hInstanceWrapper.GetPhysicalDeviceImageFormatProperties(
							pPhysicalDevices[uiPhysicalDevice],
							Format_GBufferColor,
							VK_IMAGE_TYPE_2D,
							VK_IMAGE_TILING_OPTIMAL,
							//VK_IMAGE_USAGE_TRANSFER_SRC_BIT | 
							//VK_IMAGE_USAGE_TRANSFER_DST_BIT |
							//VK_IMAGE_USAGE_SAMPLED_BIT | //SRV //Replaced By InputAttachment In Vulkan
							//VK_IMAGE_USAGE_STORAGE_BIT | //UAV //Direct3D11 Not Allow Both UAV And RTV //Direct3D12 Use Barrier To Avoid Track State Thus Allow
							VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | //RTV
							VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, //Replace SRV For Mobile Tile Architecture //D3D12_FEATURE_DATA_ARCHITECTURE::TileBasedRenderer
							0U,
							&ImageFormatProperties
						);

						if (eResultLDR_BGRA != VK_SUCCESS)
						{
							assert(eResultLDR_BGRA == VK_ERROR_FORMAT_NOT_SUPPORTED);

							Format_GBufferColor = VK_FORMAT_R8G8B8A8_UNORM; //LDR Display

							VkResult eResultLDR_RGBA = hInstanceWrapper.GetPhysicalDeviceImageFormatProperties(
								pPhysicalDevices[uiPhysicalDevice],
								Format_GBufferColor,
								VK_IMAGE_TYPE_2D,
								VK_IMAGE_TILING_OPTIMAL,
								//VK_IMAGE_USAGE_TRANSFER_SRC_BIT | 
								//VK_IMAGE_USAGE_TRANSFER_DST_BIT |
								//VK_IMAGE_USAGE_SAMPLED_BIT | //SRV //Replaced By InputAttachment In Vulkan
								//VK_IMAGE_USAGE_STORAGE_BIT | //UAV //Direct3D11 Not Allow Both UAV And RTV //Direct3D12 Use Barrier To Avoid Track State Thus Allow
								VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | //RTV
								VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, //Replace SRV For Mobile Tile Architecture //D3D12_FEATURE_DATA_ARCHITECTURE::TileBasedRenderer
								0U,
								&ImageFormatProperties
							);

							assert(eResultLDR_RGBA == VK_SUCCESS);
							if (eResultLDR_RGBA != VK_SUCCESS)
							{
								assert(eResultLDR_RGBA == VK_ERROR_FORMAT_NOT_SUPPORTED);
								//Not Support Format For GBufferColor
								continue;
							}
						}
					}

					//Format_GBufferNormal --------------------------------------------

					Format_GBufferNormal = VK_FORMAT_R8G8B8A8_SNORM;

					VkResult eResultNormal_SNORM = hInstanceWrapper.GetPhysicalDeviceImageFormatProperties(
						pPhysicalDevices[uiPhysicalDevice],
						Format_GBufferNormal,
						VK_IMAGE_TYPE_2D,
						VK_IMAGE_TILING_OPTIMAL,
						//VK_IMAGE_USAGE_TRANSFER_SRC_BIT | 
						//VK_IMAGE_USAGE_TRANSFER_DST_BIT |
						//VK_IMAGE_USAGE_SAMPLED_BIT | //SRV //Replaced By InputAttachment In Vulkan
						//VK_IMAGE_USAGE_STORAGE_BIT | //UAV //Direct3D11 Not Allow Both UAV And RTV //Direct3D12 Use Barrier To Avoid Track State Thus Allow
						VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | //RTV
						VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, //Replace SRV For Mobile Tile Architecture //D3D12_FEATURE_DATA_ARCHITECTURE::TileBasedRenderer
						0U,
						&ImageFormatProperties
					);

					if (eResultNormal_SNORM != VK_SUCCESS)
					{
						assert(eResultNormal_SNORM == VK_ERROR_FORMAT_NOT_SUPPORTED);

						Format_GBufferNormal = VK_FORMAT_R8G8B8A8_UNORM;

						VkResult eResultNormal_UNORM = hInstanceWrapper.GetPhysicalDeviceImageFormatProperties(
							pPhysicalDevices[uiPhysicalDevice],
							Format_GBufferNormal,
							VK_IMAGE_TYPE_2D,
							VK_IMAGE_TILING_OPTIMAL,
							//VK_IMAGE_USAGE_TRANSFER_SRC_BIT | 
							//VK_IMAGE_USAGE_TRANSFER_DST_BIT |
							//VK_IMAGE_USAGE_SAMPLED_BIT | //SRV //Replaced By InputAttachment In Vulkan
							//VK_IMAGE_USAGE_STORAGE_BIT | //UAV //Direct3D11 Not Allow Both UAV And RTV //Direct3D12 Use Barrier To Avoid Track State Thus Allow
							VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | //RTV
							VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, //Replace SRV For Mobile Tile Architecture //D3D12_FEATURE_DATA_ARCHITECTURE::TileBasedRenderer
							0U,
							&ImageFormatProperties
						);

						assert(eResultNormal_UNORM == VK_SUCCESS);
						if (eResultNormal_UNORM != VK_SUCCESS)
						{
							assert(eResultNormal_UNORM == VK_ERROR_FORMAT_NOT_SUPPORTED);
							//Not Support Format For GBufferNormal
							continue;
						}
					}

					//Format_Depth --------------------------------------------

					Format_Depth = VK_FORMAT_D32_SFLOAT;

					VkResult eResultDepth_FLOAT = hInstanceWrapper.GetPhysicalDeviceImageFormatProperties(
						pPhysicalDevices[uiPhysicalDevice],
						Format_Depth,
						VK_IMAGE_TYPE_2D,
						VK_IMAGE_TILING_OPTIMAL,
						//VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
						//VK_IMAGE_USAGE_TRANSFER_DST_BIT |
						//VK_IMAGE_USAGE_SAMPLED_BIT |  //SRV //Replaced By InputAttachment In Vulkan
						VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | //DSV
						VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, //Replace SRV For Mobile Tile Architecture //D3D12_FEATURE_DATA_ARCHITECTURE::TileBasedRenderer
						0U,
						&ImageFormatProperties
					);

					if (eResultDepth_FLOAT != VK_SUCCESS)
					{
						assert(eResultDepth_FLOAT == VK_ERROR_FORMAT_NOT_SUPPORTED);

						Format_Depth = VK_FORMAT_X8_D24_UNORM_PACK32;

						VkResult eResultDepth_UNORM24 = hInstanceWrapper.GetPhysicalDeviceImageFormatProperties(
							pPhysicalDevices[uiPhysicalDevice],
							Format_Depth,
							VK_IMAGE_TYPE_2D,
							VK_IMAGE_TILING_OPTIMAL,
							//VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
							//VK_IMAGE_USAGE_TRANSFER_DST_BIT |
							//VK_IMAGE_USAGE_SAMPLED_BIT |  //SRV //Replaced By InputAttachment In Vulkan
							VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | //DSV
							VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, //Replace SRV For Mobile Tile Architecture //D3D12_FEATURE_DATA_ARCHITECTURE::TileBasedRenderer
							0U,
							&ImageFormatProperties
						);

						if (eResultDepth_UNORM24 != VK_SUCCESS)
						{
							assert(eResultDepth_UNORM24 == VK_ERROR_FORMAT_NOT_SUPPORTED);

							Format_Depth = VK_FORMAT_D16_UNORM;

							VkResult eResultDepth_UNORM16 = hInstanceWrapper.GetPhysicalDeviceImageFormatProperties(
								pPhysicalDevices[uiPhysicalDevice],
								Format_Depth,
								VK_IMAGE_TYPE_2D,
								VK_IMAGE_TILING_OPTIMAL,
								//VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
								//VK_IMAGE_USAGE_TRANSFER_DST_BIT |
								//VK_IMAGE_USAGE_SAMPLED_BIT |  //SRV //Replaced By InputAttachment In Vulkan
								VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | //DSV
								VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, //Replace SRV For Mobile Tile Architecture //D3D12_FEATURE_DATA_ARCHITECTURE::TileBasedRenderer
								0U,
								&ImageFormatProperties
							);

							assert(eResultDepth_UNORM16 == VK_SUCCESS);
							if (eResultDepth_UNORM16 != VK_SUCCESS)
							{
								//Not Support Format For Depth
								continue;
							}
						}
					}
				}

				//↓MemoryTypeBits --------------------------------------------
				uint32_t MemoryTypeBits_Buffer;
				uint32_t MemoryTypeBits_Texture_Immutable_SRV;
				uint32_t MemoryTypeBits_Texture_Mutable_UAV;
				uint32_t MemoryTypeBits_Texture_Mutable_RTVDSV;
				{
					//Direct3D12规范
					//D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS

					//Vukan规范
					//The memoryTypeBits member is /*Identical*/ for all VkBuffer objects created 
					//with the /*Same*/ value for the /*Flags*/ and /*Usage*/ members in the VkBufferCreateInfo structure passed to vkCreateBuffer.
					//Further, if /*Usage1*/ and /*Usage2*/ of type VkBufferUsageFlags are such that the bits set in /*Usage2*/ are a subset of the bits set in /*Usage1*/, and they have the /*Same*/ /*Flags*/,
					//then the bits set in memoryTypeBits returned for /*Usage1*/ must be a /*Subset*/ of the bits set in memoryTypeBits returned for /*Usage2*/, for all values of /*Flags*/.

					//MemoryTypeBits_Buffer
					{
						VkBufferCreateInfo CreateInfo;
						CreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
						CreateInfo.pNext = NULL;
						CreateInfo.flags = 0U;
						//D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT //64KB
						CreateInfo.size = 1024U * 64U;
						CreateInfo.usage = 0U
							| VK_BUFFER_USAGE_TRANSFER_SRC_BIT //ReadBack //暂时不用
							| VK_BUFFER_USAGE_TRANSFER_DST_BIT //UpLoad
							| VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT //Buffer<float4>
							| VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT //RWBuffer<float4>
							| VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT //cbuffer
							| VK_BUFFER_USAGE_STORAGE_BUFFER_BIT //??? //暂时不用
							| VK_BUFFER_USAGE_INDEX_BUFFER_BIT //IndexBuffer
							| VK_BUFFER_USAGE_VERTEX_BUFFER_BIT //VertexBuffer
							| VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT //Argument //暂时不用
							;
						//Direct3D12规范
						//Implicit State Transitions (Promotion And Decay)
						//https://msdn.microsoft.com/en-us/library/windows/desktop/dn899226(v=vs.85).aspx#implicit_state_transitions
						//D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS 
						//Cannot be used with D3D12_RESOURCE_DIMENSION_BUFFER; but buffers always have the properties represented by this flag.
						//https://msdn.microsoft.com/en-us/library/windows/desktop/dn986742(v=vs.85).aspx
						pCommandQueueManager->BufferCreate_PopulateShareMode(&CreateInfo.sharingMode, &CreateInfo.queueFamilyIndexCount, &CreateInfo.pQueueFamilyIndices);

						VkBuffer hBuffer;

						VkResult eResult = hDeviceWrapper.CreateBuffer(&CreateInfo, NULL, &hBuffer);
						assert(eResult == VK_SUCCESS);

						VkMemoryRequirements vkMemoryRequirements;
						hDeviceWrapper.GetBufferMemoryRequirements(hBuffer, &vkMemoryRequirements);

						MemoryTypeBits_Buffer = vkMemoryRequirements.memoryTypeBits;

						hDeviceWrapper.DestroyBuffer(hBuffer, NULL);
					}

					//Direct3D12规范
					//D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES : Vulkan Usage
					//D3D12_TEXTURE_LAYOUT_ROW_MAJOR->D3D12_HEAP_FLAG_SHARED_CROSS_ADAPTER : Vulkan Tiling

					//D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES : Vulkan Usage
					//D3D12_TEXTURE_LAYOUT_ROW_MAJOR->D3D12_HEAP_FLAG_SHARED_CROSS_ADAPTER : Vulkan Tiling

					//Vukan规范
					//If the memory requirements are for a VkImage, the memoryTypeBits member
					//must not refer to a VkMemoryType with a propertyFlags that has the VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT bit set
					//if the vkGetImageMemoryRequirements::image did not have VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT bit set
					//in the /*Usage*/ member of the VkImageCreateInfo structure passed to vkCreateImage.

					//For images created with a color format,
					//the memoryTypeBits member is /*Identical*/ for all VkImage objects created 
					//with the /*Same*/ combination of values for the /*Tiling*/ member,
					//the VK_IMAGE_CREATE_SPARSE_BINDING_BIT bit of the /*Flags*/ member,
					//and the VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT of the /*Usage*/ member in the VkImageCreateInfo structure passed to vkCreateImage.

					//For images created with a depth/stencil format,
					//the memoryTypeBits member is /*Identical*/ for all VkImage objects created
					//with the /*Same*/ combination of values for 
					//the /*Format*/ member, 
					//the /*Tiling*/ member, 
					//the VK_IMAGE_CREATE_SPARSE_BINDING_BIT bit of the /*Flags*/ member, 
					//and the VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT of the /*Usage*/ member in the VkImageCreateInfo structure passed to vkCreateImage.

					//In Vulkan
					//基于上述分析，在Vulkan中，Usage并不会对Heap的类型造成影响

					//Direct3D12规范
					//Memory Management Strategies
					//https://msdn.microsoft.com/en-us/library/windows/desktop/mt613239(v=vs.85).aspx#classification_strategy

					//因此，我们采用Direct3D12的内存管理策略
					//1. D3D11_USAGE_DEFAULT	//For Re-Used Resources //由于HeapTier1的原因，GBufferPass(RTV+DSV)和TileDeferred/SSR/SSAO/PostProcesss(UAV)的输出应当存放在不同的Heap中 //理论上GeometryPipeline(VS/HS/DS/GS)和CUDACore(PS/CS)可以并行化
					//2. D3D11_USAGE_IMMUTABLE	//For Assert (Scaled/Optional Or Streaming Resources)

					//MemoryTypeBits_Texture_Immutable_SRV
					//SRV
					{
						VkImageCreateInfo CreateInfo;
						CreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
						CreateInfo.pNext = NULL;
						CreateInfo.flags = 0U;
						CreateInfo.imageType = VK_IMAGE_TYPE_2D;
#if defined(PTWIN32)
#if defined(PTWIN32DESKTOP)
						CreateInfo.format = VK_FORMAT_BC7_SRGB_BLOCK;
#elif defined(PTWIN32RUNTIME)
						CreateInfo.format = VK_FORMAT_BC7_SRGB_BLOCK;
#else
#error 未知的平台
#endif
#elif defined(PTPOSIX)
#if defined(PTPOSIXLINUXGLIBC)
						CreateInfo.format = VK_FORMAT_BC7_SRGB_BLOCK;

#elif defined(PTPOSIXLINUXBIONIC)
						CreateInfo.format = VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK;
#else
#error 未知的平台
#endif
#else
#error 未知的平台
#endif
						CreateInfo.extent.width = 128U;
						CreateInfo.extent.height = 128U;
						CreateInfo.extent.depth = 1U;
						CreateInfo.mipLevels = 1U;
						CreateInfo.arrayLayers = 1U;
						CreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
						//-------------------------------------------------------------------
						//在Direct3D12中
						//可以用ID3D12Resource::WriteToSubresource对UMA显卡Zero-Copy优化
						//https://msdn.microsoft.com/en-us/library/windows/desktop/dn914416(v=vs.85).aspx
						//-------------------------------------------------------------------
						//在Vulkan中
						//vkGetImageSubresourceLayout只适用于VK_IMAGE_TILING_LINEAR
						//-------------------------------------------------------------------
						//Vulkan-Docs
						//Synchronization Examples
						//https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples
						//Upload data from the CPU to an image sampled in a fragment shader
						//This path is universal to both UMA and discrete systems, as images should be converted to optimal tiling on upload.
						CreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
						//-------------------------------------------------------------------
						CreateInfo.usage =
							//VK_IMAGE_USAGE_TRANSFER_SRC_BIT | 
							VK_IMAGE_USAGE_TRANSFER_DST_BIT |
							//VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | //Replace SRV For Mobile Tile Architecture //D3D12_FEATURE_DATA_ARCHITECTURE::TileBasedRenderer
							VK_IMAGE_USAGE_SAMPLED_BIT; //SRV //Replaced By InputAttachment In Vulkan
						CreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
						CreateInfo.queueFamilyIndexCount = 0U;
						CreateInfo.pQueueFamilyIndices = NULL;
						CreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

						VkImageFormatProperties ImageFormatProperties;

						VkResult eResultFormat = hInstanceWrapper.GetPhysicalDeviceImageFormatProperties(
							pPhysicalDevices[uiPhysicalDevice],
							CreateInfo.format,
							CreateInfo.imageType,
							CreateInfo.tiling,
							CreateInfo.usage,
							CreateInfo.flags,
							&ImageFormatProperties
						);
						assert(eResultFormat == VK_SUCCESS);
						if (eResultFormat != VK_SUCCESS)
						{
							assert(eResultFormat == VK_ERROR_FORMAT_NOT_SUPPORTED);
							//Not Support Format For Compressed Texture
							continue;
						}

						VkImage hImage;

						VkResult eResultCreate = hDeviceWrapper.CreateImage(&CreateInfo, NULL, &hImage);
						assert(eResultCreate == VK_SUCCESS);

						VkMemoryRequirements MemoryRequirements;
						hDeviceWrapper.GetImageMemoryRequirements(hImage, &MemoryRequirements);

						MemoryTypeBits_Texture_Immutable_SRV = MemoryRequirements.memoryTypeBits;

						hDeviceWrapper.DestroyImage(hImage, NULL);
					}

					//MemoryTypeBits_Texture_Mutable_UAV
					//UAV+SRV
					{
						VkImageCreateInfo CreateInfo;
						CreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
						CreateInfo.pNext = NULL;
						CreateInfo.flags = 0U;
						CreateInfo.imageType = VK_IMAGE_TYPE_2D;
						CreateInfo.format = VK_FORMAT_R16G16B16A16_SFLOAT; //Scene-Referred //Radiance
						CreateInfo.extent.width = 128U;
						CreateInfo.extent.height = 128U;
						CreateInfo.extent.depth = 1U;
						CreateInfo.mipLevels = 1U;
						CreateInfo.arrayLayers = 1U;
						CreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
						//-------------------------------------------------------------------
						//在Direct3D12中
						//可以用ID3D12Resource::WriteToSubresource对UMA显卡Zero-Copy优化
						//https://msdn.microsoft.com/en-us/library/windows/desktop/dn914416(v=vs.85).aspx
						//-------------------------------------------------------------------
						//在Vulkan中
						//vkGetImageSubresourceLayout只适用于VK_IMAGE_TILING_LINEAR
						//-------------------------------------------------------------------
						//Vulkan-Docs
						//Synchronization Examples
						//https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples
						//Upload data from the CPU to an image sampled in a fragment shader
						//This path is universal to both UMA and discrete systems, as images should be converted to optimal tiling on upload.
						CreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
						//-------------------------------------------------------------------
						CreateInfo.usage =
							//VK_IMAGE_USAGE_TRANSFER_SRC_BIT | 
							//VK_IMAGE_USAGE_TRANSFER_DST_BIT |
							//VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | //Replace SRV For Mobile Tile Architecture //D3D12_FEATURE_DATA_ARCHITECTURE::TileBasedRenderer
							VK_IMAGE_USAGE_SAMPLED_BIT | //SRV //Replaced By InputAttachment In Vulkan
							VK_IMAGE_USAGE_STORAGE_BIT; //UAV //Direct3D11 Not Allow Both UAV And RTV //Direct3D12 Use Barrier To Avoid Track State Thus Allow				
						CreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
						CreateInfo.queueFamilyIndexCount = 0U;
						CreateInfo.pQueueFamilyIndices = NULL;
						CreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

						VkImage hImage;

						VkResult eResult = hDeviceWrapper.CreateImage(&CreateInfo, NULL, &hImage);
						assert(eResult == VK_SUCCESS);

						VkMemoryRequirements MemoryRequirements;
						hDeviceWrapper.GetImageMemoryRequirements(hImage, &MemoryRequirements);

						MemoryTypeBits_Texture_Mutable_UAV = MemoryRequirements.memoryTypeBits;

						hDeviceWrapper.DestroyImage(hImage, NULL);
					}

					//MemoryTypeBits_Texture_Mutable_RTVDSV
					//RTV+SRV //GBufferColor
					{
						VkImageCreateInfo CreateInfo;
						CreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
						CreateInfo.pNext = NULL;
						CreateInfo.flags = 0U;
						CreateInfo.imageType = VK_IMAGE_TYPE_2D;
						CreateInfo.format = Format_GBufferColor;
						CreateInfo.extent.width = 128U;
						CreateInfo.extent.height = 128U;
						CreateInfo.extent.depth = 1U;
						CreateInfo.mipLevels = 1U;
						CreateInfo.arrayLayers = 1U;
						CreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
						//-------------------------------------------------------------------
						//在Direct3D12中
						//可以用ID3D12Resource::WriteToSubresource对UMA显卡Zero-Copy优化
						//https://msdn.microsoft.com/en-us/library/windows/desktop/dn914416(v=vs.85).aspx
						//-------------------------------------------------------------------
						//在Vulkan中
						//vkGetImageSubresourceLayout只适用于VK_IMAGE_TILING_LINEAR
						//-------------------------------------------------------------------
						//Vulkan-Docs
						//Synchronization Examples
						//https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples
						//Upload data from the CPU to an image sampled in a fragment shader
						//This path is universal to both UMA and discrete systems, as images should be converted to optimal tiling on upload.
						CreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
						//-------------------------------------------------------------------
						CreateInfo.usage =
							//VK_IMAGE_USAGE_TRANSFER_SRC_BIT | 
							//VK_IMAGE_USAGE_TRANSFER_DST_BIT |
							//VK_IMAGE_USAGE_SAMPLED_BIT | //SRV //Replaced By InputAttachment In Vulkan
							//VK_IMAGE_USAGE_STORAGE_BIT | //UAV //Direct3D11 Not Allow Both UAV And RTV //Direct3D12 Use Barrier To Avoid Track State Thus Allow
							VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | //RTV
							VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT; //Replace SRV For Mobile Tile Architecture //D3D12_FEATURE_DATA_ARCHITECTURE::TileBasedRenderer
						CreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
						CreateInfo.queueFamilyIndexCount = 0U;
						CreateInfo.pQueueFamilyIndices = NULL;
						CreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

						VkImage hImage;

						VkResult eResult = hDeviceWrapper.CreateImage(&CreateInfo, NULL, &hImage);
						assert(eResult == VK_SUCCESS);

						VkMemoryRequirements MemoryRequirements;
						hDeviceWrapper.GetImageMemoryRequirements(hImage, &MemoryRequirements);

						MemoryTypeBits_Texture_Mutable_RTVDSV = MemoryRequirements.memoryTypeBits;

						hDeviceWrapper.DestroyImage(hImage, NULL);
					}

					//MemoryTypeBits_Texture_Mutable_RTVDSV
					//RTV+SRV //GBufferNormal
					{
						VkImageCreateInfo CreateInfo;
						CreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
						CreateInfo.pNext = NULL;
						CreateInfo.flags = 0U;
						CreateInfo.imageType = VK_IMAGE_TYPE_2D;
						CreateInfo.format = Format_GBufferNormal;
						CreateInfo.extent.width = 128U;
						CreateInfo.extent.height = 128U;
						CreateInfo.extent.depth = 1U;
						CreateInfo.mipLevels = 1U;
						CreateInfo.arrayLayers = 1U;
						CreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
						//-------------------------------------------------------------------
						//在Direct3D12中
						//可以用ID3D12Resource::WriteToSubresource对UMA显卡Zero-Copy优化
						//https://msdn.microsoft.com/en-us/library/windows/desktop/dn914416(v=vs.85).aspx
						//-------------------------------------------------------------------
						//在Vulkan中
						//vkGetImageSubresourceLayout只适用于VK_IMAGE_TILING_LINEAR
						//-------------------------------------------------------------------
						//Vulkan-Docs
						//Synchronization Examples
						//https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples
						//Upload data from the CPU to an image sampled in a fragment shader
						//This path is universal to both UMA and discrete systems, as images should be converted to optimal tiling on upload.
						CreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
						//-------------------------------------------------------------------
						CreateInfo.usage =
							//VK_IMAGE_USAGE_TRANSFER_SRC_BIT | 
							//VK_IMAGE_USAGE_TRANSFER_DST_BIT |
							//VK_IMAGE_USAGE_SAMPLED_BIT | //SRV //Replaced By InputAttachment In Vulkan
							//VK_IMAGE_USAGE_STORAGE_BIT | //UAV //Direct3D11 Not Allow Both UAV And RTV //Direct3D12 Use Barrier To Avoid Track State Thus Allow
							VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | //RTV
							VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT; //Replace SRV For Mobile Tile Architecture //D3D12_FEATURE_DATA_ARCHITECTURE::TileBasedRenderer
						CreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
						CreateInfo.queueFamilyIndexCount = 0U;
						CreateInfo.pQueueFamilyIndices = NULL;
						CreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

						VkImage hImage;

						VkResult eResult = hDeviceWrapper.CreateImage(&CreateInfo, NULL, &hImage);
						assert(eResult == VK_SUCCESS);

						VkMemoryRequirements MemoryRequirements;
						hDeviceWrapper.GetImageMemoryRequirements(hImage, &MemoryRequirements);

						MemoryTypeBits_Texture_Mutable_RTVDSV &= MemoryRequirements.memoryTypeBits;

						hDeviceWrapper.DestroyImage(hImage, NULL);
					}

					//MemoryTypeBits_Texture_Mutable_RTVDSV
					//DSV+SRV
					{
						VkImageCreateInfo CreateInfo;
						CreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
						CreateInfo.pNext = NULL;
						CreateInfo.flags = 0U;
						CreateInfo.imageType = VK_IMAGE_TYPE_2D;
						CreateInfo.format = Format_Depth; //VK_FORMAT_D32_SFLOAT Depth Reverse Z
						CreateInfo.extent.width = 128U;
						CreateInfo.extent.height = 128U;
						CreateInfo.extent.depth = 1U;
						CreateInfo.mipLevels = 1U;
						CreateInfo.arrayLayers = 1U;
						CreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
						//-------------------------------------------------------------------
						//在Direct3D12中
						//可以用ID3D12Resource::WriteToSubresource对UMA显卡Zero-Copy优化
						//https://msdn.microsoft.com/en-us/library/windows/desktop/dn914416(v=vs.85).aspx
						//-------------------------------------------------------------------
						//在Vulkan中
						//vkGetImageSubresourceLayout只适用于VK_IMAGE_TILING_LINEAR
						//-------------------------------------------------------------------
						//Vulkan-Docs
						//Synchronization Examples
						//https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples
						//Upload data from the CPU to an image sampled in a fragment shader
						//This path is universal to both UMA and discrete systems, as images should be converted to optimal tiling on upload.
						CreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
						//-------------------------------------------------------------------
						CreateInfo.usage =
							//VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
							//VK_IMAGE_USAGE_TRANSFER_DST_BIT |
							//VK_IMAGE_USAGE_SAMPLED_BIT |  //SRV //Replaced By InputAttachment In Vulkan
							VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | //DSV
							VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, //Replace SRV For Mobile Tile Architecture //D3D12_FEATURE_DATA_ARCHITECTURE::TileBasedRenderer
							CreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
						CreateInfo.queueFamilyIndexCount = 0U;
						CreateInfo.pQueueFamilyIndices = NULL;
						CreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

						VkImage hImage;

						VkResult eResult = hDeviceWrapper.CreateImage(&CreateInfo, NULL, &hImage);
						assert(eResult == VK_SUCCESS);

						VkMemoryRequirements MemoryRequirements;
						hDeviceWrapper.GetImageMemoryRequirements(hImage, &MemoryRequirements);

						MemoryTypeBits_Texture_Mutable_RTVDSV &= MemoryRequirements.memoryTypeBits;

						hDeviceWrapper.DestroyImage(hImage, NULL);
					}
				}

				//↓Heap -------------------------------------

				VkPhysicalDeviceProperties PhysicalDeviceProperties;
				hInstanceWrapper.GetPhysicalDeviceProperties(pPhysicalDevices[uiPhysicalDevice], &PhysicalDeviceProperties);


				//https://vulkan.gpuinfo.org

				//NCC //Flush //Unmap is still useful in CC for "Zero-Copy Optimization" //Because We can free the virual address 
				//UMA //"Zero-Copy Optimization" //There Is No WriteToSubresource Optimal In Vulkan !!! //No Optimal For Image //We Always Need StagingBuffer

				//Adreno GPU (API 1.0.3 For Some GPU) Not Has DEVICE_LOCAL Flag ???

				//Adreno GPU (API 1.1.66 For Some GPU) Has Two Memory Pool. Maybe The Second Is For Dedicated Memory ??? //Maybe Optimized For The On-Chip Memory In Tile Architect

				//PowerVR GPU is Discrete //But appears as UMA //DeviceType Is Not Reliable	 //Maybe it is a bug and fixed at API 1.0.80 ???

				//AMD RADV GPU is Discrete //But has a special pool!

				//StagingBuffer
				
				//UniformBuffer
				
				//VertexBufferAndIndexBuffer

				//SampledImage

				//FrameBuffer(Input/Color/Depth/Stencil Attachment) //Delicated Allocation //Optimized For Tiled-Architect

#if 1 //Qualcomm Adreno //UMA-CacheCoherent

				//Qualcomm Adreno API 1.0.3
				//Integrated
				//Heap
				//0 DEVICE_LOCAL //POOL_L0
				//Type
				//0 DEVICE_LOCAL											//CPU_PAGE_PROPERTY_NOT_AVAILABLE 
				//1 HOST_VISIBLE+HOST_CACHED								//CPU_PAGE_PROPERTY_WRITE_BACK		//???
				//2 HOST_VISIBLE+HOST_COHERENT+HOST_CACHED					//CPU_PAGE_PROPERTY_WRITE_BACK		//???

				//Qualcomm Adreno API 1.0.3/1.0.20/1.0.31/1.0.33/1.0.38
				//Integrated
				//Heap
				//0 DEVICE_LOCAL //POOL_L0
				//Type
				//0 DEVICE_LOCAL											//CPU_PAGE_PROPERTY_NOT_AVAILABLE 
				//1 DEVICE_LOCAL+HOST_VISIBLE+HOST_CACHED					//CPU_PAGE_PROPERTY_WRITE_BACK		//UMA(DEVICE_LOCAL)-NCC(HOST_CACHED)
				//2 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT+HOST_CACHED		//CPU_PAGE_PROPERTY_WRITE_BACK		//UMA(DEVICE_LOCAL)-CC(HOST_CACHED+HOST_COHERENT)	//VertexBuffer+IndexBuffer+StartingBuffer

				//Qualcomm Adreno API 1.0.38/1.0.49/1.0.61/1.0.66/1.1.66
				//Integrated
				//Heap
				//0 DEVICE_LOCAL //POOL_L0
				//Type
				//0 DEVICE_LOCAL											//CPU_PAGE_PROPERTY_NOT_AVAILABLE 
				//1 DEVICE_LOCAL+HOST_VISIBLE+HOST_CACHED					//CPU_PAGE_PROPERTY_WRITE_BACK		//UMA(DEVICE_LOCAL)-NCC(HOST_CACHED)
				//2 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT+HOST_CACHED		//CPU_PAGE_PROPERTY_WRITE_BACK		//UMA(DEVICE_LOCAL)-CC(HOST_CACHED+HOST_COHERENT)	//VertexBuffer+IndexBuffer+StartingBuffer
				//3 DEVICE_LOCAL											//CPU_PAGE_PROPERTY_NOT_AVAILABLE	//0 is better than 3 ???
				//4 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT					//CPU_PAGE_PROPERTY_WRITE_COMBINE	//UMA(DEVICE_LOCAL)		//2 is faster than 4 //Cached

				//Qualcomm Adreno API 1.1.66
				//Integrated
				//Heap
				//0 DEVICE_LOCAL //POOL_L0
				//Type
				//0 DEVICE_LOCAL											//CPU_PAGE_PROPERTY_NOT_AVAILABLE 
				//1 DEVICE_LOCAL+HOST_VISIBLE+HOST_CACHED					//CPU_PAGE_PROPERTY_WRITE_BACK		//UMA(DEVICE_LOCAL)-NCC(HOST_CACHED)
				//2 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT+HOST_CACHED		//CPU_PAGE_PROPERTY_WRITE_BACK		//UMA(DEVICE_LOCAL)-CC(HOST_CACHED+HOST_COHERENT)	//VertexBuffer+IndexBuffer+StartingBuffer
				//3 DEVICE_LOCAL											//CPU_PAGE_PROPERTY_NOT_AVAILABLE	//0 is better than 3 ???
				//4 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT					//CPU_PAGE_PROPERTY_WRITE_COMBINE	//UMA(DEVICE_LOCAL)		//2 is faster than 4 //Cached
				//Heap
				//1 DEVICE_LOCAL //POOL_L0
				//Type
				//5 DEVICE_LOCAL											//CPU_PAGE_PROPERTY_NOT_AVAILABLE
#endif

#if 1 //ARM Mali //UMA-NonCacheCoherent

				//ARM Mali API 1.0.2/1.0.11/1.0.14/1.0.26/1.0.36/1.0.47/1.0.52/1.0.53/1.0.58/1.0.61/1.0.65/1.0.66/1.0.82
				//Integrated
				//Heap
				//0 DEVICE_LOCAL //POOL_L0
				//Type
				//0 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT					//CPU_PAGE_PROPERTY_WRITE_COMBINE	//UMA(DEVICE_LOCAL)									//VertexBuffer+IndexBuffer+StartingBuffer
				//1 DEVICE_LOCAL+HOST_VISIBLE+HOST_CACHED					//CPU_PAGE_PROPERTY_WRITE_BACK		//UMA(DEVICE_LOCAL)-NCC(HOST_CACHED)				//???
				//2 DEVICE_LOCAL+LAZILY_ALLOCATED							//CPU_PAGE_PROPERTY_NOT_AVAILABLE	//???
#endif

#if 1 //ImgTec PowerVR //UMA-NonCacheCoherent

				//ImgTec PowerVR API 1.0.3
				//Discrete //???
				//Heap
				//0 DEVICE_LOCAL //POOL_L0
				//Type
				//2 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT		//For All

				//ImgTec PowerVR API 1.0.49
				//Discrete //???
				//Heap
				//0 DEVICE_LOCAL //POOL_L0
				//Type
				//0 DEVICE_LOCAL								//CPU_PAGE_PROPERTY_NOT_AVAILABLE 
				//1 DEVICE_LOCAL								//CPU_PAGE_PROPERTY_NOT_AVAILABLE //0 is better than 1 ???
				//2 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT		//CPU_PAGE_PROPERTY_WRITE_COMBINE //UMA(DEVICE_LOCAL)  //VertexBuffer+IndexBuffer+StartingBuffer
				//3 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT		//CPU_PAGE_PROPERTY_WRITE_COMBINE //UMA(DEVICE_LOCAL)  //2 is better than 3
				//4 DEVICE_LOCAL+HOST_VISIBLE+HOST_CACHED		//CPU_PAGE_PROPERTY_WRITE_BACK //NCC(HOST_CACHED+HOST_COHERENT) //UMA(DEVICE_LOCAL)

				//ImgTec PowerVR API 1.0.80
				//Integrated //The Previous Version May Be A Bug ???
				//Heap
				//0 DEVICE_LOCAL //POOL_L0
				//Type
				//0 DEVICE_LOCAL								//CPU_PAGE_PROPERTY_NOT_AVAILABLE 
				//1 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT		//CPU_PAGE_PROPERTY_WRITE_COMBINE //UMA(DEVICE_LOCAL)  //VertexBuffer+IndexBuffer+StartingBuffer
				//2 DEVICE_LOCAL+HOST_VISIBLE+HOST_CACHED		//CPU_PAGE_PROPERTY_WRITE_BACK //NCC(HOST_CACHED+HOST_COHERENT) //UMA(DEVICE_LOCAL)
#endif

#if 1 //Intel
				
				//Intel Bay Trail //UMA-NonCacheCoherent //API 1.0.57/API 1.1.0/API 1.1.80/API 1.1.90/API 1.1.96
				//Integrated 
				//Heap
				//0 DEVICE_LOCAL //POOL_L0
				//Type
				//0 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT					//CPU_PAGE_PROPERTY_WRITE_COMBINE	//UMA(DEVICE_LOCAL)									//VertexBuffer+IndexBuffer+StartingBuffer
				//1 DEVICE_LOCAL+HOST_VISIBLE+HOST_CACHED					//CPU_PAGE_PROPERTY_WRITE_BACK		//UMA(DEVICE_LOCAL)-NCC(HOST_CACHED)				//???

				//Intel /[Braswell]-[API 1.0.57]/ /[HD 615][Kaby Lake GT2]-[API 1.0.57]/ /[HD 520][Skylake GT2]-[Driver 18.0.99]-[API 1.0.57]/ /[HD 520][Skylake GT2]-[Driver 18.1.99]-[API 1.1.76]/ /[HD 530][Skylake GT2]-[Driver 18.2.99]-[API 1.1.80]/
				//Integrated 
				//Heap
				//0 DEVICE_LOCAL //POOL_L0
				//Type
				//0 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT+HOST_CACHED		//CPU_PAGE_PROPERTY_WRITE_BACK		//UMA(DEVICE_LOCAL)-CC(HOST_CACHED+HOST_COHERENT)	//VertexBuffer+IndexBuffer+StartingBuffer
				//Heap
				//1 DEVICE_LOCAL //POOL_L0
				//Type
				//1 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT+HOST_CACHED		//CPU_PAGE_PROPERTY_WRITE_BACK		//UMA(DEVICE_LOCAL)-CC(HOST_CACHED+HOST_COHERENT)	//VertexBuffer+IndexBuffer+StartingBuffer

				//Intel /[HD 615][Kaby Lake GT2]-[API 1.0.76]/ /[HD 520]-[Skylake GT2]-[Driver 18.2.99]-[API 1.1.80]/
				//Integrated 
				//Heap
				//0 DEVICE_LOCAL //POOL_L0
				//Type
				//0 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT+HOST_CACHED		//CPU_PAGE_PROPERTY_WRITE_BACK		//UMA(DEVICE_LOCAL)-CC(HOST_CACHED+HOST_COHERENT)	//VertexBuffer+IndexBuffer+StartingBuffer
#endif

#if 1 //NVIDIA Tegra //UMA-NonCacheCoherent

				//NVIDIA Tegra API 1.0.2/1.0.3
				//Integrated
				//Heap
				//0 DEVICE_LOCAL //POOL_L0
				//Type
				//0 DEVICE_LOCAL								//CPU_PAGE_PROPERTY_NOT_AVAILABLE 
				//1 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT		//CPU_PAGE_PROPERTY_WRITE_COMBINE	//UMA(DEVICE_LOCAL)						//VertexBuffer+IndexBuffer+StartingBuffer
				//2 DEVICE_LOCAL+HOST_VISIBLE+HOST_CACHED		//CPU_PAGE_PROPERTY_WRITE_BACK		//UMA(DEVICE_LOCAL)-NCC(HOST_CACHED)

				//NVIDIA Tegra API 1.0.13/1.0.24/1.0.37/1.0.56/1.0.61
				//Integrated
				//Heap
				//0 DEVICE_LOCAL //POOL_L0
				//Type
				//0 DEVICE_LOCAL								//CPU_PAGE_PROPERTY_NOT_AVAILABLE 
				//1 DEVICE_LOCAL								//CPU_PAGE_PROPERTY_NOT_AVAILABLE	//0 is better than 1 ???
				//2 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT		//CPU_PAGE_PROPERTY_WRITE_COMBINE	//UMA(DEVICE_LOCAL)						//VertexBuffer+IndexBuffer+StartingBuffer
				//3 DEVICE_LOCAL+HOST_VISIBLE+HOST_CACHED		//CPU_PAGE_PROPERTY_WRITE_BACK		//UMA(DEVICE_LOCAL)-NCC(HOST_CACHED)

				//NVIDIA Tegra API 1.1.0
				//Integrated
				//Heap
				//0 DEVICE_LOCAL //POOL_L0
				//Type
				//0 DEVICE_LOCAL								//CPU_PAGE_PROPERTY_NOT_AVAILABLE 
				//1 DEVICE_LOCAL								//CPU_PAGE_PROPERTY_NOT_AVAILABLE	//0 is better than 1 ???
				//2 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT		//CPU_PAGE_PROPERTY_WRITE_COMBINE	//UMA(DEVICE_LOCAL)						//VertexBuffer+IndexBuffer+StartingBuffer
				//3 DEVICE_LOCAL+HOST_VISIBLE+HOST_CACHED		//CPU_PAGE_PROPERTY_WRITE_BACK		//UMA(DEVICE_LOCAL)-NCC(HOST_CACHED)
				//4 DEVICE_LOCAL								//CPU_PAGE_PROPERTY_NOT_AVAILABLE 
				//5 DEVICE_LOCAL								//CPU_PAGE_PROPERTY_NOT_AVAILABLE
#endif

#if 1 //AMD RADV

				//AMD RADV API
				//Discrete
				//Heap
				//0 DEVICE_LOCAL //POOL_L1
				//Type
				//0 DEVICE_LOCAL							//CPU_PAGE_PROPERTY_NOT_AVAILABLE												//VertexBuffer+IndexBuffer+Image
				//Heap
				//1 DEVICE_LOCAL //POOL_L1					//256MB //Special Pool Of Video Memory //Adam Sawicki. "Memory Management in Vulkan and DX12." GDC 2018.
				//Type
				//2 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT	//CPU_PAGE_PROPERTY_WRITE_COMBINE		//NUMA									//ConstantBuffer //+StartingBuffer ???
				//Heap
				//2		 		 //POOL_L0
				//Type
				//1 HOST_VISIBLE+HOST_COHERENT				//CPU_PAGE_PROPERTY_WRITE_COMBINE		//NUMA								
				//4 HOST_VISIBLE+HOST_CACHED+HOST_COHERENT	//CPU_PAGE_PROPERTY_WRITE_COMBINE		//NUMA-CC(HOST_CACHED+HOST_COHERENT)	//StartingBuffer //As Fallback If 256MB Is Not Enough
#endif

				//独立显卡
				if (PhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				{
					//Heap--------------------------------------------
					uint32_t uiCPUPagePropertyS_NotAvailable[VK_MAX_MEMORY_TYPES];
					uint32_t uiCPUPagePropertyCount_NotAvailable = 0U;
					uint32_t uiCPUPagePropertyS_WriteCombine[VK_MAX_MEMORY_TYPES];
					uint32_t uiCPUPagePropertyCount_WriteCombine = 0U;
					uint32_t uiCPUPagePropertyS_WriteBack_NonCacheCoherent[VK_MAX_MEMORY_TYPES];
					uint32_t uiCPUPagePropertyCount_WriteBack_NonCacheCoherent = 0U;
					uint32_t uiCPUPagePropertyS_WriteBack_CacheCoherent[VK_MAX_MEMORY_TYPES];
					uint32_t uiCPUPagePropertyCount_WriteBack_CacheCoherent = 0U;
					bool bCacheCoherent;
					//可用于确定ReadBack时是否应当调用vkInvalidateMappedMemoryRanges
					//注：Upload时始终不需要FlushMappedMemoryRanges，WriteCombine in NCC
					//注：Vulkan中InvalidateCache与Map解耦
					{
						VkPhysicalDeviceMemoryProperties MemoryProperties;
						hInstanceWrapper.GetPhysicalDeviceMemoryProperties(pPhysicalDevices[uiPhysicalDevice], &MemoryProperties);

						//Direct3D12规范
						//D3D12_MEMORY_POOL_L0
						//D3D12_MEMORY_POOL_L1

						uint32_t uiMemoryPoolCount_Video = 0U;
						uint32_t uiMemoryPoolS_Video[VK_MAX_MEMORY_HEAPS];//D3D12_MEMORY_POOL_L1
						uint32_t uiMemoryPoolCount_System = 0U;
						uint32_t uiMemoryPoolS_System[VK_MAX_MEMORY_HEAPS];//D3D12_MEMORY_POOL_L0

						for (uint32_t uiMemoryPool = 0U; uiMemoryPool < MemoryProperties.memoryHeapCount; ++uiMemoryPool)
						{
							switch (MemoryProperties.memoryHeaps[uiMemoryPool].flags)
							{
							case 0U:
							{
								uiMemoryPoolS_System[uiMemoryPoolCount_System] = uiMemoryPool;
								++uiMemoryPoolCount_System;
							}
							break;
							case VK_MEMORY_HEAP_DEVICE_LOCAL_BIT:
							{
								uiMemoryPoolS_Video[uiMemoryPoolCount_Video] = uiMemoryPool;
								++uiMemoryPoolCount_Video;
							}
							break;
							default:
								assert(0);
							}
						}

						assert(uiMemoryPoolCount_Video == 1U && uiMemoryPoolCount_System == 1U);
						//非致命???
						//if (!(uiMemoryPoolCount_Video == 1U && uiMemoryPoolCount_System == 1U))
						//{
						//	不符合Direct3D12规范
						//	continue;
						//}

						uint32_t uiMemoryPoolVideo = uiMemoryPoolS_Video[0];
						uint32_t uiMemoryPoolSystem = uiMemoryPoolS_System[0];

						//Direct3D12规范
						//D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE
						//D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE
						//D3D12_CPU_PAGE_PROPERTY_WRITE_BACK

						//NUMA的显示内存池中的内存类型的属性标志有
						//VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT //D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE
						//VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT|VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT ???

						//Direct3D12规范中不区分NUMA是否CacheCoherent

						//NCC_NUMA的系统内存池中的内存类型属性标志有以下两种
						//VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT //D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE
						//VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_CACHED_BIT //D3D12_CPU_PAGE_PROPERTY_WRITE_BACK

						//CC_NUMA的系统内存池中的内存类型属性标志有以下两种
						//VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT //D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE
						//VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT|VK_MEMORY_PROPERTY_HOST_CACHED_BIT //D3D12_CPU_PAGE_PROPERTY_WRITE_BACK

						for (uint32_t uiCPUPageProperty = 0U; uiCPUPageProperty < MemoryProperties.memoryTypeCount; ++uiCPUPageProperty)
						{
							switch (MemoryProperties.memoryTypes[uiCPUPageProperty].propertyFlags)
							{
							case 0U:
							{
								//Ignore
							}
							break;
							case VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT:
							{
								assert(MemoryProperties.memoryTypes[uiCPUPageProperty].heapIndex == uiMemoryPoolVideo);
								if (!(MemoryProperties.memoryTypes[uiCPUPageProperty].heapIndex == uiMemoryPoolVideo))
								{
									//不符合Direct3D12规范
									continue;
								}
								uiCPUPagePropertyS_NotAvailable[uiCPUPagePropertyCount_NotAvailable] = uiCPUPageProperty;
								++uiCPUPagePropertyCount_NotAvailable;
							}
							break;
							case VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT:
							{
								//Ignore

								//A memory type with this flag(VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) set
								//is only allowed to be bound to a VkImage whose usage flags include VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT.
							}
							break;
							case VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT:
							{
								assert(MemoryProperties.memoryTypes[uiCPUPageProperty].heapIndex == uiMemoryPoolSystem);
								if (!(MemoryProperties.memoryTypes[uiCPUPageProperty].heapIndex == uiMemoryPoolSystem))
								{
									//不符合Direct3D12规范
									continue;
								}
								uiCPUPagePropertyS_WriteCombine[uiCPUPagePropertyCount_WriteCombine] = uiCPUPageProperty;
								++uiCPUPagePropertyCount_WriteCombine;
							}
							break;
							case VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT:
							{
								assert(MemoryProperties.memoryTypes[uiCPUPageProperty].heapIndex == uiMemoryPoolSystem);
								if (!(MemoryProperties.memoryTypes[uiCPUPageProperty].heapIndex == uiMemoryPoolSystem))
								{
									//不符合Direct3D12规范
									continue;
								}
								uiCPUPagePropertyS_WriteBack_NonCacheCoherent[uiCPUPagePropertyCount_WriteBack_NonCacheCoherent] = uiCPUPageProperty;
								++uiCPUPagePropertyCount_WriteBack_NonCacheCoherent;
							}
							break;
							case VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT:
							{
								assert(MemoryProperties.memoryTypes[uiCPUPageProperty].heapIndex == uiMemoryPoolSystem);
								if (!(MemoryProperties.memoryTypes[uiCPUPageProperty].heapIndex == uiMemoryPoolSystem))
								{
									//不符合Direct3D12规范
									continue;
								}
								uiCPUPagePropertyS_WriteBack_CacheCoherent[uiCPUPagePropertyCount_WriteBack_CacheCoherent] = uiCPUPageProperty;
								++uiCPUPagePropertyCount_WriteBack_CacheCoherent;
							}
							break;
							default:
								assert(0);
							}
						}

						//Direct3D12规范
						//不区分NUMA是否CacheCoherent
						//D3D12_FEATURE_DATA_ARCHITECTURE::CacheCoherentUMA

						//在移动设备上测试时出现???
						//assert(iCPUPagePropertyWriteBackNonCacheCoherentCount == 0U || iCPUPagePropertyWriteBackCacheCoherentCount == 0U);
						//if (!(iCPUPagePropertyWriteBackNonCacheCoherentCount == 0U || iCPUPagePropertyWriteBackCacheCoherentCount == 0U))
						//{
						//	不符合Direct3D12规范
						//	continue;
						//}
						bCacheCoherent = (uiCPUPagePropertyCount_WriteBack_CacheCoherent != 0U);
					}

					//Heap--------------------------------------------
					//NUMA-CC
					uint32_t uiHeapIndex_CC_System_Buffer = VK_MAX_MEMORY_TYPES;
					//NUMA-NCC
					uint32_t uiHeapIndex_NCC_SystemUpload_Buffer = VK_MAX_MEMORY_TYPES;
					uint32_t uiHeapIndex_NCC_SystemReadback_Buffer = VK_MAX_MEMORY_TYPES;
					//NUMA-CC / NUMA-NCC
					uint32_t uiHeapIndex_Video_Buffer = VK_MAX_MEMORY_TYPES;
					uint32_t uiHeapIndex_Video_Texture_Immutable_SRV = VK_MAX_MEMORY_TYPES;
					uint32_t uiHeapIndex_Video_Texture_Mutable_UAV = VK_MAX_MEMORY_TYPES;
					uint32_t uiHeapIndex_Video_Texture_Mutable_RTVDSV = VK_MAX_MEMORY_TYPES;
					{
						//Vukan规范
						//For each pair of elements X and Y returned in memoryTypes, X must be placed at a lower index position than Y if:
						//either the set of bit flags returned in the propertyFlags member of X is a strict subset of the set of bit flags returned in the propertyFlags member of Y.
						//or the propertyFlags members of X and Y are equal, and X belongs to a memory heap with /*Greater Performance*/ (as determined in an implementation - specific manner).

						//Direct3D12规范
						//不区分NUMA是否CacheCoherent
						//D3D12_FEATURE_DATA_ARCHITECTURE::CacheCoherentUMA

						//iHeapVideo_Buffer
						//iHeapSystemWriteCombine_Buffer
						//iHeapSystemWriteBack_Buffer
						if (bCacheCoherent)
						{
							//uiHeapIndex_CC_System_Buffer
							for (uint32_t uiCPUPageProperty = 0U; uiCPUPageProperty < uiCPUPagePropertyCount_WriteBack_CacheCoherent; ++uiCPUPageProperty)
							{
								if (MemoryTypeBits_Buffer&(1U << uiCPUPagePropertyS_WriteBack_CacheCoherent[uiCPUPageProperty]))
								{
									uiHeapIndex_CC_System_Buffer = uiCPUPagePropertyS_WriteBack_CacheCoherent[uiCPUPageProperty];
									break;
								}
							}
							assert(uiHeapIndex_CC_System_Buffer < VK_MAX_MEMORY_TYPES);
							if (!(uiHeapIndex_CC_System_Buffer < VK_MAX_MEMORY_TYPES))
							{
								continue;
							}
						}
						else
						{
							//uiHeapIndex_NCC_SystemUpload_Buffer
							for (uint32_t uiCPUPageProperty = 0U; uiCPUPageProperty < uiCPUPagePropertyCount_WriteCombine; ++uiCPUPageProperty)
							{
								if (MemoryTypeBits_Buffer&(1U << uiCPUPagePropertyS_WriteCombine[uiCPUPageProperty]))
								{
									uiHeapIndex_NCC_SystemUpload_Buffer = uiCPUPagePropertyS_WriteCombine[uiCPUPageProperty];
									break;
								}
							}
							assert(uiHeapIndex_NCC_SystemUpload_Buffer < VK_MAX_MEMORY_TYPES);
							if (!(uiHeapIndex_NCC_SystemUpload_Buffer < VK_MAX_MEMORY_TYPES))
							{
								continue;
							}

							//uiHeapIndex_NCC_SystemReadback_Buffer
							for (uint32_t uiCPUPageProperty = 0U; uiCPUPageProperty < uiCPUPagePropertyCount_WriteBack_NonCacheCoherent; ++uiCPUPageProperty)
							{
								if (MemoryTypeBits_Buffer&(1U << uiCPUPagePropertyS_WriteBack_NonCacheCoherent[uiCPUPageProperty]))
								{
									uiHeapIndex_NCC_SystemReadback_Buffer = uiCPUPagePropertyS_WriteBack_NonCacheCoherent[uiCPUPageProperty];
									break;
								}
							}
							assert(uiHeapIndex_NCC_SystemReadback_Buffer < VK_MAX_MEMORY_TYPES);
							if (!(uiHeapIndex_NCC_SystemReadback_Buffer < VK_MAX_MEMORY_TYPES))
							{
								continue;
							}
						}


						//uiHeapIndex_Video_Buffer
						for (uint32_t uiCPUPageProperty = 0U; uiCPUPageProperty < uiCPUPagePropertyCount_NotAvailable; ++uiCPUPageProperty)
						{
							if (MemoryTypeBits_Buffer&(1U << uiCPUPagePropertyS_NotAvailable[uiCPUPageProperty]))
							{
								uiHeapIndex_Video_Buffer = uiCPUPagePropertyS_NotAvailable[uiCPUPageProperty];
								break;
							}
						}
						assert(uiHeapIndex_Video_Buffer < VK_MAX_MEMORY_TYPES);
						if (!(uiHeapIndex_Video_Buffer < VK_MAX_MEMORY_TYPES))
						{
							continue;
						}

						//uiHeapIndex_Video_Texture_Immutable_SRV
						for (uint32_t uiCPUPageProperty = 0U; uiCPUPageProperty < uiCPUPagePropertyCount_NotAvailable; ++uiCPUPageProperty)
						{
							if (MemoryTypeBits_Texture_Immutable_SRV&(1U << uiCPUPagePropertyS_NotAvailable[uiCPUPageProperty]))
							{
								uiHeapIndex_Video_Texture_Immutable_SRV = uiCPUPagePropertyS_NotAvailable[uiCPUPageProperty];
								break;
							}
						}
						assert(uiHeapIndex_Video_Texture_Immutable_SRV < VK_MAX_MEMORY_TYPES);
						if (!(uiHeapIndex_Video_Texture_Immutable_SRV < VK_MAX_MEMORY_TYPES))
						{
							continue;
						}

						//uiHeapIndex_Video_Texture_Mutable_UAV
						for (uint32_t uiCPUPageProperty = 0U; uiCPUPageProperty < uiCPUPagePropertyCount_NotAvailable; ++uiCPUPageProperty)
						{
							if (MemoryTypeBits_Texture_Mutable_UAV&(1U << uiCPUPagePropertyS_NotAvailable[uiCPUPageProperty]))
							{
								uiHeapIndex_Video_Texture_Mutable_UAV = uiCPUPagePropertyS_NotAvailable[uiCPUPageProperty];
								break;
							}
						}
						assert(uiHeapIndex_Video_Texture_Mutable_UAV < VK_MAX_MEMORY_TYPES);
						if (!(uiHeapIndex_Video_Texture_Mutable_UAV < VK_MAX_MEMORY_TYPES))
						{
							continue;
						}

						//uiHeapIndex_Video_Texture_Mutable_RTVDSV
						for (uint32_t uiCPUPageProperty = 0U; uiCPUPageProperty < uiCPUPagePropertyCount_NotAvailable; ++uiCPUPageProperty)
						{
							if (MemoryTypeBits_Texture_Mutable_RTVDSV&(1U << uiCPUPagePropertyS_NotAvailable[uiCPUPageProperty]))
							{
								uiHeapIndex_Video_Texture_Mutable_RTVDSV = uiCPUPagePropertyS_NotAvailable[uiCPUPageProperty];
								break;
							}
						}
						assert(uiHeapIndex_Video_Texture_Mutable_RTVDSV < VK_MAX_MEMORY_TYPES);
						if (!(uiHeapIndex_Video_Texture_Mutable_RTVDSV < VK_MAX_MEMORY_TYPES))
						{
							continue;
						}
					}

					//--------------------------------------------
					IPTVDHeapManager_NUMA *pHeapManager = NULL;
					if (bCacheCoherent)
					{
						void *pHeapManagerVoid = ::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTVDHeapManager_NUMA_CC), alignof(PTVDHeapManager_NUMA_CC));
						assert(pHeapManagerVoid != NULL);

						pHeapManager = new(pHeapManagerVoid)PTVDHeapManager_NUMA_CC(uiHeapIndex_CC_System_Buffer, uiHeapIndex_Video_Buffer, uiHeapIndex_Video_Texture_Immutable_SRV, uiHeapIndex_Video_Texture_Mutable_UAV, uiHeapIndex_Video_Texture_Mutable_RTVDSV);
					}
					else
					{
						void *pHeapManagerVoid = ::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTVDHeapManager_NUMA_NCC), alignof(PTVDHeapManager_NUMA_NCC));
						assert(pHeapManagerVoid != NULL);

						pHeapManager = new(pHeapManagerVoid)PTVDHeapManager_NUMA_NCC(uiHeapIndex_NCC_SystemUpload_Buffer, uiHeapIndex_NCC_SystemReadback_Buffer, uiHeapIndex_Video_Buffer, uiHeapIndex_Video_Texture_Immutable_SRV, uiHeapIndex_Video_Texture_Mutable_UAV, uiHeapIndex_Video_Texture_Mutable_RTVDSV);
					}

					//--------------------------------------------
					void *pDeviceVoid = ::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTVDDevice_NUMA), alignof(PTVDDevice_NUMA));
					assert(pDeviceVoid != NULL);

					IPTVD_Device *pDevice = new(pDeviceVoid)PTVDDevice_NUMA(pPhysicalDevices[uiPhysicalDevice], hDeviceWrapper, pCommandQueueManager, pHeapManager);

					NUMADeviceNameS.push_back(PhysicalDeviceProperties.deviceName);
					NUMADevicePointerS.push_back(pDevice);

					//For __Variable_SafeRelease_CommandQueue, We Steal Here !!!
					hDevice = VK_NULL_HANDLE;
					pCommandQueueManager = NULL;
				}
				//集成显卡
				else if (PhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
				{
					//Heap--------------------------------------------

					//在Direct3D12中
					//可以用ID3D12Resource::WriteToSubresource对UMA显卡Zero-Copy优化
					//https://msdn.microsoft.com/en-us/library/windows/desktop/dn914416(v=vs.85).aspx
					//-------------------------------------------------------------------
					//在Vulkan中
					//vkGetImageSubresourceLayout只适用于VK_IMAGE_TILING_LINEAR
					//-------------------------------------------------------------------
					//Vulkan-Docs
					//Synchronization Examples
					//https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples
					//Upload data from the CPU to an image sampled in a fragment shader
					//This path is universal to both UMA and discrete systems, as images should be converted to optimal tiling on upload.

					uint32_t uiCPUPagePropertyS_NotAvailable[VK_MAX_MEMORY_TYPES];
					uint32_t uiCPUPagePropertyCount_NotAvailable = 0U;
					uint32_t uiCPUPagePropertyS_WriteCombine[VK_MAX_MEMORY_TYPES];
					uint32_t uiCPUPagePropertyCount_WriteCombine = 0U;
					uint32_t uiCPUPagePropertyS_WriteBack_NonCacheCoherent[VK_MAX_MEMORY_TYPES];
					uint32_t uiCPUPagePropertyCount_WriteBack_NonCacheCoherent = 0U;
					uint32_t uiCPUPagePropertyS_WriteBack_CacheCoherent[VK_MAX_MEMORY_TYPES];
					uint32_t uiCPUPagePropertyCount_WriteBack_CacheCoherent = 0U;
					bool bCacheCoherent;
					//可用于确定ReadBack时是否应当调用vkInvalidateMappedMemoryRanges
					//注：Upload时始终不需要FlushMappedMemoryRanges，WriteCombine in NCC
					//注：Vulkan中InvalidateCache与Map解耦
					{
						VkPhysicalDeviceMemoryProperties MemoryProperties;
						hInstanceWrapper.GetPhysicalDeviceMemoryProperties(pPhysicalDevices[uiPhysicalDevice], &MemoryProperties);

						//Direct3D12规范
						//D3D12_MEMORY_POOL_L0
						//D3D12_MEMORY_POOL_L1

						//Vulkan规范
						//In a unified memory architecture (UMA) system
						//there is often only a /*Single*/ memory heap which is considered to be equally “local” to the /*Host*/ and to the /*Device*/,
						//and such an implementation must advertise the heap as device-local.

						uint32_t uiMemoryPoolCount_System = 0U;
						uint32_t uiMemoryPoolS_System[VK_MAX_MEMORY_HEAPS];//D3D12_MEMORY_POOL_L0

						for (uint32_t uiMemoryPool = 0U; uiMemoryPool < MemoryProperties.memoryHeapCount; ++uiMemoryPool)
						{
							switch (MemoryProperties.memoryHeaps[uiMemoryPool].flags)
							{
							case 0U:
							{
								assert(0);
							}
							break;
							case VK_MEMORY_HEAP_DEVICE_LOCAL_BIT:
							{
								uiMemoryPoolS_System[uiMemoryPoolCount_System] = uiMemoryPool;
								++uiMemoryPoolCount_System;
							}
							break;
							default:
								assert(0);
							}
						}

						assert(uiMemoryPoolCount_System == 1U);
						//非致命???
						//if (!(uiMemoryPoolCount_System == 1U))
						//{
						//	不符合Direct3D12规范
						//	continue;
						//}

						uint32_t uiMemoryPoolSystem = uiMemoryPoolS_System[0];

						//Direct3D12规范
						//D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE
						//D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE
						//D3D12_CPU_PAGE_PROPERTY_WRITE_BACK

						//NCC_UMA的系统内存池中的内存类型属性标志有以下两种
						//VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT|VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT //D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE
						//VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT|VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_CACHED_BIT //D3D12_CPU_PAGE_PROPERTY_WRITE_BACK

						//CC_UMA的系统内存池中的内存类型属性标志有以下两种
						//VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT|VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT //D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE
						//VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT|VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT|VK_MEMORY_PROPERTY_HOST_CACHED_BIT //D3D12_CPU_PAGE_PROPERTY_WRITE_BACK

						for (uint32_t uiCPUPageProperty = 0U; uiCPUPageProperty < MemoryProperties.memoryTypeCount; ++uiCPUPageProperty)
						{
							switch (MemoryProperties.memoryTypes[uiCPUPageProperty].propertyFlags)
							{
							case 0U:
							{
								//Ignore
							}
							break;
							case VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT:
							{
								//在Direct3D12中
								//可以用ID3D12Resource::WriteToSubresource对UMA显卡Zero-Copy优化
								//https://msdn.microsoft.com/en-us/library/windows/desktop/dn914416(v=vs.85).aspx
								//-------------------------------------------------------------------
								//在Vulkan中
								//vkGetImageSubresourceLayout只适用于VK_IMAGE_TILING_LINEAR
								//-------------------------------------------------------------------
								//Vulkan-Docs
								//Synchronization Examples
								//https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples
								//Upload data from the CPU to an image sampled in a fragment shader
								//This path is universal to both UMA and discrete systems, as images should be converted to optimal tiling on upload.

								assert(MemoryProperties.memoryTypes[uiCPUPageProperty].heapIndex == uiMemoryPoolSystem);
								if (!(MemoryProperties.memoryTypes[uiCPUPageProperty].heapIndex == uiMemoryPoolSystem))
								{
									//不符合Direct3D12规范
									continue;
								}
								uiCPUPagePropertyS_NotAvailable[uiCPUPagePropertyCount_NotAvailable] = uiCPUPageProperty;
								++uiCPUPagePropertyCount_NotAvailable;

							}
							break;
							case VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT:
							{
								//Ignore

								//A memory type with this flag(VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) set
								//is only allowed to be bound to a VkImage whose usage flags include VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT.
							}
							break;
							case VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT:
							{
								assert(MemoryProperties.memoryTypes[uiCPUPageProperty].heapIndex == uiMemoryPoolSystem);
								if (!(MemoryProperties.memoryTypes[uiCPUPageProperty].heapIndex == uiMemoryPoolSystem))
								{
									//不符合Direct3D12规范
									continue;
								}
								uiCPUPagePropertyS_WriteCombine[uiCPUPagePropertyCount_WriteCombine] = uiCPUPageProperty;
								++uiCPUPagePropertyCount_WriteCombine;
							}
							break;
							case VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT:
							{
								assert(MemoryProperties.memoryTypes[uiCPUPageProperty].heapIndex == uiMemoryPoolSystem);
								if (!(MemoryProperties.memoryTypes[uiCPUPageProperty].heapIndex == uiMemoryPoolSystem))
								{
									//不符合Direct3D12规范
									continue;
								}
								uiCPUPagePropertyS_WriteBack_NonCacheCoherent[uiCPUPagePropertyCount_WriteBack_NonCacheCoherent] = uiCPUPageProperty;
								++uiCPUPagePropertyCount_WriteBack_NonCacheCoherent;
							}
							break;
							case VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT:
							{
								assert(MemoryProperties.memoryTypes[uiCPUPageProperty].heapIndex == uiMemoryPoolSystem);
								if (!(MemoryProperties.memoryTypes[uiCPUPageProperty].heapIndex == uiMemoryPoolSystem))
								{
									//不符合Direct3D12规范
									continue;
								}
								uiCPUPagePropertyS_WriteBack_CacheCoherent[uiCPUPagePropertyCount_WriteBack_CacheCoherent] = uiCPUPageProperty;
								++uiCPUPagePropertyCount_WriteBack_CacheCoherent;
							}
							break;
							default:
								assert(0);
							}
						}

						//Direct3D12规范
						//D3D12_FEATURE_DATA_ARCHITECTURE::CacheCoherentUMA

						//在移动设备上测试时出现???
						//assert(iCPUPagePropertyWriteBackNonCacheCoherentCount == 0U || iCPUPagePropertyWriteBackCacheCoherentCount == 0U);
						//if (!(iCPUPagePropertyWriteBackNonCacheCoherentCount == 0U || iCPUPagePropertyWriteBackCacheCoherentCount == 0U))
						//{
						//不符合Direct3D12规范
						//continue;
						//}
						bCacheCoherent = (uiCPUPagePropertyCount_WriteBack_CacheCoherent != 0U);
					}

					//Heap--------------------------------------------

					//在Direct3D12中
					//可以用ID3D12Resource::WriteToSubresource对UMA显卡Zero-Copy优化
					//https://msdn.microsoft.com/en-us/library/windows/desktop/dn914416(v=vs.85).aspx
					//-------------------------------------------------------------------
					//在Vulkan中
					//vkGetImageSubresourceLayout只适用于VK_IMAGE_TILING_LINEAR
					//-------------------------------------------------------------------
					//Vulkan-Docs
					//Synchronization Examples
					//https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples
					//Upload data from the CPU to an image sampled in a fragment shader
					//This path is universal to both UMA and discrete systems, as images should be converted to optimal tiling on upload.

					//UMA-CC
					uint32_t uiHeapIndex_CC_System_Buffer = VK_MAX_MEMORY_TYPES;
					//UMA-NCC
					uint32_t uiHeapIndex_NCC_SystemUpload_Buffer = VK_MAX_MEMORY_TYPES;
					uint32_t uiHeapIndex_NCC_SystemReadback_Buffer = VK_MAX_MEMORY_TYPES;
					//UMA-CC / UMA-NCC
					uint32_t uiHeapIndex_SystemOptimal_Texture_Immutable_SRV = VK_MAX_MEMORY_TYPES;
					uint32_t uiHeapIndex_SystemOptimal_Texture_Mutable_UAV = VK_MAX_MEMORY_TYPES;
					uint32_t uiHeapIndex_SystemOptimal_Texture_Mutable_RTVDSV = VK_MAX_MEMORY_TYPES;
					{

						//Vukan规范
						//For each pair of elements X and Y returned in memoryTypes, X must be placed at a lower index position than Y if:
						//either the set of bit flags returned in the propertyFlags member of X is a strict subset of the set of bit flags returned in the propertyFlags member of Y.
						//or the propertyFlags members of X and Y are equal, and X belongs to a memory heap with /*Greater Performance*/ (as determined in an implementation - specific manner).

						//根据Above，较低的索引“并不一定”表示较高的性能

						//Direct3D12规范
						//D3D12_FEATURE_DATA_ARCHITECTURE::CacheCoherentUMA
						//https://msdn.microsoft.com/en-us/library/windows/desktop/dn859384(v=vs.85).aspx
						//CacheCoherentUMA means that a significant amount of the caches in the memory hierarchy are also unified or integrated between the CPU and GPU.
						//When UMA is true and CacheCoherentUMA is false, the most unique characteristic for these adapters is that /*Upload*/ heaps are still /*Write-Combined*/.
						//For CacheCoherentUMA, the most unique observable characteristic is that /*Upload*/ heaps are actually /*Write-Back*/, the usage of /*Write-Combine*/ on /*Upload*/ heaps is commonly a /*Detriment*/.

						if (bCacheCoherent)
						{
							//CacheCoherent
							//Upload和ReadBack相同

							//uiHeapIndex_CC_System_Buffer
							for (uint32_t uiCPUPageProperty = 0U; uiCPUPageProperty < uiCPUPagePropertyCount_WriteBack_CacheCoherent; ++uiCPUPageProperty)
							{
								if (MemoryTypeBits_Buffer&(1U << uiCPUPagePropertyS_WriteBack_CacheCoherent[uiCPUPageProperty]))
								{
									uiHeapIndex_CC_System_Buffer = uiCPUPagePropertyS_WriteBack_CacheCoherent[uiCPUPageProperty];
									break;
								}
							}
							assert(uiHeapIndex_CC_System_Buffer < VK_MAX_MEMORY_TYPES);
							if (!(uiHeapIndex_CC_System_Buffer < VK_MAX_MEMORY_TYPES))
							{
								continue;
							}
						}
						else
						{
							//NonCacheCoherent
							//Upload应当WriteCombine

							//uiHeapIndex_NCC_SystemUpload_Buffer
							for (uint32_t uiCPUPageProperty = 0U; uiCPUPageProperty < uiCPUPagePropertyCount_WriteCombine; ++uiCPUPageProperty)
							{
								if (MemoryTypeBits_Buffer&(1U << uiCPUPagePropertyS_WriteCombine[uiCPUPageProperty]))
								{
									uiHeapIndex_NCC_SystemUpload_Buffer = uiCPUPagePropertyS_WriteCombine[uiCPUPageProperty];
									break;
								}
							}
							assert(uiHeapIndex_NCC_SystemUpload_Buffer < VK_MAX_MEMORY_TYPES);
							if (!(uiHeapIndex_NCC_SystemUpload_Buffer < VK_MAX_MEMORY_TYPES))
							{
								continue;
							}

							//uiHeapIndex_NCC_SystemReadback_Buffer
							for (uint32_t uiCPUPageProperty = 0U; uiCPUPageProperty < uiCPUPagePropertyCount_WriteBack_NonCacheCoherent; ++uiCPUPageProperty)
							{
								if (MemoryTypeBits_Buffer&(1U << uiCPUPagePropertyS_WriteBack_NonCacheCoherent[uiCPUPageProperty]))
								{
									uiHeapIndex_NCC_SystemReadback_Buffer = uiCPUPagePropertyS_WriteBack_NonCacheCoherent[uiCPUPageProperty];
									break;
								}
							}
							assert(uiHeapIndex_NCC_SystemReadback_Buffer < VK_MAX_MEMORY_TYPES);
							if (!(uiHeapIndex_NCC_SystemReadback_Buffer < VK_MAX_MEMORY_TYPES))
							{
								continue;
							}
						}

						//uiHeapIndex_SystemOptimal_Texture_Immutable_SRV
						for (uint32_t uiCPUPageProperty = 0U; uiCPUPageProperty < uiCPUPagePropertyCount_NotAvailable; ++uiCPUPageProperty)
						{
							if (MemoryTypeBits_Texture_Immutable_SRV&(1U << uiCPUPagePropertyS_NotAvailable[uiCPUPageProperty]))
							{
								uiHeapIndex_SystemOptimal_Texture_Immutable_SRV = uiCPUPagePropertyS_NotAvailable[uiCPUPageProperty];
								break;
							}
						}
						if (!(uiHeapIndex_SystemOptimal_Texture_Immutable_SRV < VK_MAX_MEMORY_TYPES))
						{
							for (uint32_t uiCPUPageProperty = 0U; uiCPUPageProperty < uiCPUPagePropertyCount_WriteBack_CacheCoherent; ++uiCPUPageProperty)
							{
								if (MemoryTypeBits_Texture_Immutable_SRV&(1U << uiCPUPagePropertyS_WriteBack_CacheCoherent[uiCPUPageProperty]))
								{
									uiHeapIndex_SystemOptimal_Texture_Immutable_SRV = uiCPUPagePropertyS_WriteBack_CacheCoherent[uiCPUPageProperty];
									break;
								}
							}
						}
						if (!(uiHeapIndex_SystemOptimal_Texture_Immutable_SRV < VK_MAX_MEMORY_TYPES))
						{
							for (uint32_t uiCPUPageProperty = 0U; uiCPUPageProperty < uiCPUPagePropertyCount_WriteBack_NonCacheCoherent; ++uiCPUPageProperty)
							{
								if (MemoryTypeBits_Texture_Immutable_SRV&(1U << uiCPUPagePropertyS_WriteBack_NonCacheCoherent[uiCPUPageProperty]))
								{
									uiHeapIndex_SystemOptimal_Texture_Immutable_SRV = uiCPUPagePropertyS_WriteBack_NonCacheCoherent[uiCPUPageProperty];
									break;
								}
							}
						}
						if (!(uiHeapIndex_SystemOptimal_Texture_Immutable_SRV < VK_MAX_MEMORY_TYPES))
						{
							for (uint32_t uiCPUPageProperty = 0U; uiCPUPageProperty < uiCPUPagePropertyCount_WriteCombine; ++uiCPUPageProperty)
							{
								if (MemoryTypeBits_Texture_Immutable_SRV&(1U << uiCPUPagePropertyS_WriteCombine[uiCPUPageProperty]))
								{
									uiHeapIndex_SystemOptimal_Texture_Immutable_SRV = uiCPUPagePropertyS_WriteCombine[uiCPUPageProperty];
									break;
								}
							}
						}
						assert(uiHeapIndex_SystemOptimal_Texture_Immutable_SRV < VK_MAX_MEMORY_TYPES);
						if (!(uiHeapIndex_SystemOptimal_Texture_Immutable_SRV < VK_MAX_MEMORY_TYPES))
						{
							continue;
						}

						//uiHeapIndex_SystemOptimal_Texture_Mutable_UAV
						for (uint32_t uiCPUPageProperty = 0U; uiCPUPageProperty < uiCPUPagePropertyCount_NotAvailable; ++uiCPUPageProperty)
						{
							if (MemoryTypeBits_Texture_Mutable_UAV&(1U << uiCPUPagePropertyS_NotAvailable[uiCPUPageProperty]))
							{
								uiHeapIndex_SystemOptimal_Texture_Mutable_UAV = uiCPUPagePropertyS_NotAvailable[uiCPUPageProperty];
								break;
							}
						}
						if (!(uiHeapIndex_SystemOptimal_Texture_Mutable_UAV < VK_MAX_MEMORY_TYPES))
						{
							for (uint32_t uiCPUPageProperty = 0U; uiCPUPageProperty < uiCPUPagePropertyCount_WriteBack_CacheCoherent; ++uiCPUPageProperty)
							{
								if (MemoryTypeBits_Texture_Mutable_UAV&(1U << uiCPUPagePropertyS_WriteBack_CacheCoherent[uiCPUPageProperty]))
								{
									uiHeapIndex_SystemOptimal_Texture_Mutable_UAV = uiCPUPagePropertyS_WriteBack_CacheCoherent[uiCPUPageProperty];
									break;
								}
							}
						}
						if (!(uiHeapIndex_SystemOptimal_Texture_Mutable_UAV < VK_MAX_MEMORY_TYPES))
						{
							for (uint32_t uiCPUPageProperty = 0U; uiCPUPageProperty < uiCPUPagePropertyCount_WriteBack_NonCacheCoherent; ++uiCPUPageProperty)
							{
								if (MemoryTypeBits_Texture_Mutable_UAV&(1U << uiCPUPagePropertyS_WriteBack_NonCacheCoherent[uiCPUPageProperty]))
								{
									uiHeapIndex_SystemOptimal_Texture_Mutable_UAV = uiCPUPagePropertyS_WriteBack_NonCacheCoherent[uiCPUPageProperty];
									break;
								}
							}
						}
						if (!(uiHeapIndex_SystemOptimal_Texture_Mutable_UAV < VK_MAX_MEMORY_TYPES))
						{
							for (uint32_t uiCPUPageProperty = 0U; uiCPUPageProperty < uiCPUPagePropertyCount_WriteCombine; ++uiCPUPageProperty)
							{
								if (MemoryTypeBits_Texture_Mutable_UAV&(1U << uiCPUPagePropertyS_WriteCombine[uiCPUPageProperty]))
								{
									uiHeapIndex_SystemOptimal_Texture_Mutable_UAV = uiCPUPagePropertyS_WriteCombine[uiCPUPageProperty];
									break;
								}
							}
						}
						assert(uiHeapIndex_SystemOptimal_Texture_Mutable_UAV < VK_MAX_MEMORY_TYPES);
						if (!(uiHeapIndex_SystemOptimal_Texture_Mutable_UAV < VK_MAX_MEMORY_TYPES))
						{
							continue;
						}

						//uiHeapIndex_SystemOptimal_Texture_Mutable_RTVDSV
						for (uint32_t uiCPUPageProperty = 0U; uiCPUPageProperty < uiCPUPagePropertyCount_NotAvailable; ++uiCPUPageProperty)
						{
							if (MemoryTypeBits_Texture_Mutable_RTVDSV&(1U << uiCPUPagePropertyS_NotAvailable[uiCPUPageProperty]))
							{
								uiHeapIndex_SystemOptimal_Texture_Mutable_RTVDSV = uiCPUPagePropertyS_NotAvailable[uiCPUPageProperty];
								break;
							}
						}
						if (!(uiHeapIndex_SystemOptimal_Texture_Mutable_RTVDSV < VK_MAX_MEMORY_TYPES))
						{
							for (uint32_t uiCPUPageProperty = 0U; uiCPUPageProperty < uiCPUPagePropertyCount_WriteBack_CacheCoherent; ++uiCPUPageProperty)
							{
								if (MemoryTypeBits_Texture_Mutable_RTVDSV&(1U << uiCPUPagePropertyS_WriteBack_CacheCoherent[uiCPUPageProperty]))
								{
									uiHeapIndex_SystemOptimal_Texture_Mutable_RTVDSV = uiCPUPagePropertyS_WriteBack_CacheCoherent[uiCPUPageProperty];
									break;
								}
							}
						}
						if (!(uiHeapIndex_SystemOptimal_Texture_Mutable_RTVDSV < VK_MAX_MEMORY_TYPES))
						{
							for (uint32_t uiCPUPageProperty = 0U; uiCPUPageProperty < uiCPUPagePropertyCount_WriteBack_NonCacheCoherent; ++uiCPUPageProperty)
							{
								if (MemoryTypeBits_Texture_Mutable_RTVDSV&(1U << uiCPUPagePropertyS_WriteBack_NonCacheCoherent[uiCPUPageProperty]))
								{
									uiHeapIndex_SystemOptimal_Texture_Mutable_RTVDSV = uiCPUPagePropertyS_WriteBack_NonCacheCoherent[uiCPUPageProperty];
									break;
								}
							}
						}
						if (!(uiHeapIndex_SystemOptimal_Texture_Mutable_RTVDSV < VK_MAX_MEMORY_TYPES))
						{
							for (uint32_t uiCPUPageProperty = 0U; uiCPUPageProperty < uiCPUPagePropertyCount_WriteCombine; ++uiCPUPageProperty)
							{
								if (MemoryTypeBits_Texture_Mutable_RTVDSV&(1U << uiCPUPagePropertyS_WriteCombine[uiCPUPageProperty]))
								{
									uiHeapIndex_SystemOptimal_Texture_Mutable_RTVDSV = uiCPUPagePropertyS_WriteCombine[uiCPUPageProperty];
									break;
								}
							}
						}
						assert(uiHeapIndex_SystemOptimal_Texture_Mutable_RTVDSV < VK_MAX_MEMORY_TYPES);
						if (!(uiHeapIndex_SystemOptimal_Texture_Mutable_RTVDSV < VK_MAX_MEMORY_TYPES))
						{
							continue;
						}
					}

					//--------------------------------------------
					IPTVDHeapManager_UMA *pHeapManager = NULL;
					if (bCacheCoherent)
					{
						void *pHeapManagerVoid = ::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTVDHeapManager_UMA_CC), alignof(PTVDHeapManager_UMA_CC));
						assert(pHeapManagerVoid != NULL);

						pHeapManager = new(pHeapManagerVoid)PTVDHeapManager_UMA_CC(uiHeapIndex_CC_System_Buffer, uiHeapIndex_SystemOptimal_Texture_Immutable_SRV, uiHeapIndex_SystemOptimal_Texture_Mutable_UAV, uiHeapIndex_SystemOptimal_Texture_Mutable_RTVDSV);
					}
					else
					{
						void *pHeapManagerVoid = ::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTVDHeapManager_UMA_NCC), alignof(PTVDHeapManager_UMA_NCC));
						assert(pHeapManagerVoid != NULL);

						pHeapManager = new(pHeapManagerVoid)PTVDHeapManager_UMA_NCC(uiHeapIndex_NCC_SystemUpload_Buffer, uiHeapIndex_NCC_SystemReadback_Buffer, uiHeapIndex_SystemOptimal_Texture_Immutable_SRV, uiHeapIndex_SystemOptimal_Texture_Mutable_UAV, uiHeapIndex_SystemOptimal_Texture_Mutable_RTVDSV);
					}

					//--------------------------------------------
					void *pDeviceVoid = ::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTVDDevice_UMA), alignof(PTVDDevice_UMA));
					assert(pDeviceVoid != NULL);

					IPTVD_Device *pDevice = new(pDeviceVoid)PTVDDevice_UMA(pPhysicalDevices[uiPhysicalDevice], hDeviceWrapper, pCommandQueueManager, pHeapManager);

					UMADeviceNameS.push_back(PhysicalDeviceProperties.deviceName);
					UMADevicePointerS.push_back(pDevice);

					//For __Variable_SafeRelease_CommandQueue, We Steal Here !!!
					hDevice = VK_NULL_HANDLE;
					pCommandQueueManager = NULL;
				}
			}

			::PTSMemoryAllocator_Free_Aligned(pPhysicalDevices);
		}

		assert(NUMADeviceNameS.size() == NUMADevicePointerS.size());
		assert(UMADeviceNameS.size() == UMADevicePointerS.size());

		if (NUMADevicePointerS.size() > 0U || UMADevicePointerS.size() > 0U)
		{
			void *pInstanceImplVoid = ::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTVDInstanceImpl), alignof(PTVDInstanceImpl));
			assert(pInstanceImplVoid != NULL);

			pInstanceImpl = new(pInstanceImplVoid)PTVDInstanceImpl(
				hInstanceWrapper,
#ifndef NDEBUG
				hDebugReportCallback,
#endif
				NUMADeviceNameS,
				NUMADevicePointerS,
				UMADeviceNameS,
				UMADevicePointerS
			);
		}

#endif

		::PTSAtomic_Set(&s_InstanceImpl_Singleton_Pointer, pInstanceImpl);
	}
	else
	{
		//Wait
		while (pInstanceImpl == reinterpret_cast<PTVDInstanceImpl *>(uintptr_t(0X1U)))
		{
			::PTS_Yield();
			pInstanceImpl = ::PTSAtomic_Get(&s_InstanceImpl_Singleton_Pointer);
		}
	}

	return pInstanceImpl;
}

void PTVDInstanceImpl::Release()
{
#ifndef NDEBUG
	m_hInstanceWrapper.DestroyDebugReportCallbackEXT(m_hDebugReportCallback, NULL);
#endif
	m_hInstanceWrapper.DestroyInstance(NULL);
}

#if 0

char const * PTVDInstanceImpl::EnumNUMA(uint32_t Index)
{
	return (Index < m_NUMADeviceName.size()) ? m_NUMADeviceName[Index].c_str() : NULL;
}

char const * PTVDInstanceImpl::EnumUMA(uint32_t Index)
{
	return (Index < m_UMADeviceName.size()) ? m_UMADeviceName[Index].c_str() : NULL;
}

void PTVDInstanceImpl::Initialize()
{
	//按照以下策略
	//1:同时存在独显和集显（一般为Desktop游戏笔记本或Desktop台式机（需要在BIOS中启用核心显卡））
	//Device3D为独显
	//DeviceCompute为集显
	//2:只存在独显（一般为Desktop台式机）
	//Device3D为独显
	//DeviceCompute为NULL
	//3:只存在集显（一般为Desktop轻薄笔记本或Mobile）
	//Device3D为集显
	//DeviceCompute为NULL

	assert(m_NUMADevicePointer.size() > 0U || m_UMADevicePointer.size() > 0U);
	if (m_UMADevicePointer.size() == 0U)
	{
		//只存在独显（一般为Desktop台式机）
		this->Initialize(true, 0U, false, bool(-1), -1); //Compute为NULL
	}
	else if (m_NUMADevicePointer.size() == 0U)
	{
		//只存在集显（一般为Desktop轻薄笔记本或Mobile）
		this->Initialize(false, 0U, false, bool(-1), -1); //Compute为NULL
	}
	else
	{
		//同时存在独显和集显（一般为Desktop游戏笔记本或Desktop台式机（需要在BIOS中启用核心显卡））
		this->Initialize(true, 0U, true, false, 0U);
	}
}

void PTVDInstanceImpl::Initialize(bool XD_IsNUMA, uint32_t XD_Index, bool Compute_IsGPU, bool Compute_IsNUMA, uint32_t Compute_Index)
{
	m_pDevice3D = (XD_IsNUMA) ? ((XD_Index < m_NUMADevicePointer.size()) ? m_NUMADevicePointer[XD_Index] : NULL) : ((XD_Index < m_UMADevicePointer.size()) ? m_UMADevicePointer[XD_Index] : NULL);
	m_pDeviceCompute = (Compute_IsGPU) ? ((Compute_IsNUMA) ? ((Compute_Index < m_NUMADevicePointer.size()) ? m_NUMADevicePointer[Compute_Index] : NULL) : ((Compute_Index < m_UMADevicePointer.size()) ? m_UMADevicePointer[Compute_Index] : NULL)) : NULL;

	//不允许3DDevice为NULL
	assert(m_pDevice3D != NULL);

	m_pDevice3D->StartUp3D(&m_Context3D);
}

#include "../../../Public/Window/PTWWindow.h"
void PTVDInstanceImpl::WindowEventOutputCallback(void *pEventDataVoid)
{
#if defined(PTWIN32)
#if defined(PTWIN32DESKTOP)
	PT_WSI_IWindow::EventOutput *pEventDataGeneric = static_cast<PT_WSI_IWindow::EventOutput *>(pEventDataVoid);

	switch (pEventDataGeneric->m_Type)
	{
	case PT_WSI_IWindow::EventOutput::Type_WindowCreated:
	{
		PT_WSI_IWindow::EventOutput_WindowCreated *pEventDataSpecific = static_cast<PT_WSI_IWindow::EventOutput_WindowCreated *>(pEventDataGeneric);
		
		VkSurfaceKHR hSurface;
		{
			VkWin32SurfaceCreateInfoKHR CreateInfo;
			CreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			CreateInfo.pNext = NULL;
			CreateInfo.flags = 0U;
			CreateInfo.hinstance = pEventDataSpecific->m_hDisplay;
			CreateInfo.hwnd = pEventDataSpecific->m_hWindow;
			VkResult eResult = m_hInstanceWrapper.CreateWin32SurfaceKHR(&CreateInfo, NULL, &hSurface);
			assert(eResult == VK_SUCCESS);
		}

		assert(m_hSurface == VK_NULL_HANDLE);
		m_hSurface = hSurface;

#ifndef NDEBUG
		VkBool32 vbResult = m_pDevice3D->SwapChain_PresentValidate(m_hInstanceWrapper, hSurface);
		assert(vbResult == VK_TRUE);
#endif
	}
	break;
	case PT_WSI_IWindow::EventOutput::Type_WindowResized:
	{
		PT_WSI_IWindow::EventOutput_WindowResized *pEventDataSpecific = static_cast<PT_WSI_IWindow::EventOutput_WindowResized *>(pEventDataGeneric);

		m_pDevice3D->SwapChain_Update(&m_Context3D, m_hInstanceWrapper, m_hSurface, pEventDataSpecific->m_Width, pEventDataSpecific->m_Height);
	}
	break;
	case PT_WSI_IWindow::EventOutput::Type_WindowDestroyed:
	{
		m_pDevice3D->SwapChain_Destroy(&m_Context3D);
		
		m_hInstanceWrapper.DestroySurfaceKHR(m_hSurface, NULL);
		m_hSurface = VK_NULL_HANDLE;
	}
	break;
	default:
		assert(0);
	}
#elif defined(PTWIN32RUNTIME)

#else
#error 未知的平台
#endif
#elif defined(PTPOSIX)
#if defined(PTPOSIXLINUXGLIBC)

#elif defined(PTPOSIXLINUXBIONIC)
	PT_WSI_IWindow::EventOutput *pEventDataGeneric = static_cast<PT_WSI_IWindow::EventOutput *>(pEventDataVoid);

	switch (pEventDataGeneric->m_Type)
	{
	case PT_WSI_IWindow::EventOutput::Type_WindowCreated:
	{
		PT_WSI_IWindow::EventOutput_WindowCreated *pEventDataSpecific = static_cast<PT_WSI_IWindow::EventOutput_WindowCreated *>(pEventDataGeneric);

		VkSurfaceKHR hSurface;
		{
			VkAndroidSurfaceCreateInfoKHR CreateInfo;
			CreateInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
			CreateInfo.pNext = NULL;
			CreateInfo.flags = 0U;
			CreateInfo.window = pEventDataSpecific->m_hWindow;
			VkResult eResult = m_hInstanceWrapper.CreateAndroidSurfaceKHR(&CreateInfo, NULL, &hSurface);
			assert(eResult == VK_SUCCESS);
		}

		assert(m_hSurface == VK_NULL_HANDLE);
		m_hSurface = hSurface;

#ifndef NDEBUG
		VkBool32 vbResult = m_pDevice3D->SwapChain_PresentValidate(m_hInstanceWrapper, hSurface);
		assert(vbResult == VK_TRUE);
#endif
	}
	break;
	case PT_WSI_IWindow::EventOutput::Type_WindowResized:
	{
		PT_WSI_IWindow::EventOutput_WindowResized *pEventDataSpecific = static_cast<PT_WSI_IWindow::EventOutput_WindowResized *>(pEventDataGeneric);

		m_pDevice3D->SwapChain_Update(&m_Context3D, m_hInstanceWrapper, m_hSurface, pEventDataSpecific->m_Width, pEventDataSpecific->m_Height);

	}
	break;
	case PT_WSI_IWindow::EventOutput::Type_WindowDestroyed:
	{
		m_pDevice3D->SwapChain_Destroy(&m_Context3D);

		m_hInstanceWrapper.DestroySurfaceKHR(m_hSurface, NULL);
		m_hSurface = VK_NULL_HANDLE;
	}
	break;
	default:
		assert(0);
	}
#else
#error 未知的平台
#endif
#else
#error 未知的平台
#endif
}

void PTVDInstanceImpl::Render(
	PTGCamera *pCamera,
	IPTVDXBar *pSceneTree,
	IPTGImage *pImageOut
)
{
	//Fence/Semaphores -> D3D12Fence
	//Events -> N/A
	//PipelineBarriers/RenderPasses -> D3D12Barrier

	//PC GPU Performance Hot Spots
	//https://developer.nvidia.com/pc-gpu-performance-hot-spots

	//Constant Buffers without Constant Pain
	//https://developer.nvidia.com/content/constant-buffers-without-constant-pain-0

	for (int i = 0; ; ++i)
	{
		m_pDevice3D->SwapChain_Present(&m_Context3D);
	}
}

#endif