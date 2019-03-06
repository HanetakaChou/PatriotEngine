#include "PTDeviceImpl.h"
#include "../../../../Public/System/Win32/PTConvUTF.h" 

#ifdef PTDEBUG
inline VkBool32 VKAPI_PTR PTDebugReportCallbackEXT(VkDebugReportFlagsEXT , VkDebugReportObjectTypeEXT , uint64_t , size_t , int32_t , const char *, const char *pMessage, void *)
{
	wchar_t wMessage[4096];
	size_t InCharsLeft = ::strlen(reinterpret_cast<const char *>(pMessage)) + 1U;//包括'\0'
	size_t OutCharsLeft = 4096;
	::PTConv_UTF8ToUTF16(pMessage, &InCharsLeft, wMessage, &OutCharsLeft);
	::OutputDebugStringW(wMessage);
	return VK_FALSE;
}
#endif

inline void* VKAPI_PTR PTAllocationFunction(void *pUserData, size_t size, size_t alignment, VkSystemAllocationScope)
{
	IPTHeap *pHeap = static_cast<IPTHeap *>(pUserData);
	return pHeap->AlignedAlloc(size, alignment);
}

inline void* VKAPI_PTR PTReallocationFunction(void *pUserData, void *pOriginal, size_t size, size_t alignment, VkSystemAllocationScope)
{
	IPTHeap *pHeap = static_cast<IPTHeap *>(pUserData);
	return pHeap->AlignedRealloc(pOriginal, size, alignment);
}

inline void VKAPI_PTR PTFreeFunction(void *pUserData, void *pMemory)
{
	IPTHeap *pHeap = static_cast<IPTHeap *>(pUserData);
	pHeap->AlignedFree(pMemory);
}

#include "../../Common/Device/PTSubAlloc.h"

#include <assert.h>
#include <new>

extern "C" PTGRAPHICSAPI IPTGDevice * PTCALL PTGDevice_Construct(PTGDeviceStorage *Storage)
{
	decltype(::vkGetInstanceProcAddr) *pFn_vkGetInstanceProcAddr = ::vkGetInstanceProcAddr;
	decltype(::vkCreateInstance) *pFn_vkCreateInstance = reinterpret_cast<decltype(::vkCreateInstance) *>(pFn_vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkCreateInstance"));

	VkResult Result;

	VkAllocationCallbacks PTAllocator;
	PTAllocator.pUserData = static_cast<void *>(pHeap);
	PTAllocator.pfnAllocation = PTAllocationFunction;
	PTAllocator.pfnReallocation = PTReallocationFunction;
	PTAllocator.pfnFree = PTFreeFunction;
	PTAllocator.pfnInternalAllocation = NULL;
	PTAllocator.pfnInternalFree = NULL;

	VkInstance hInstance;
	{
		VkInstanceCreateInfo InstanceInfo;
		InstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		InstanceInfo.pNext = NULL;
		InstanceInfo.flags = 0U;
		InstanceInfo.pApplicationInfo = NULL;
#ifdef PTDEBUG
		const char *InstanceEnabledLayerNames[1] = { "VK_LAYER_LUNARG_standard_validation" };
		InstanceInfo.enabledLayerCount = 1U;
		InstanceInfo.ppEnabledLayerNames = InstanceEnabledLayerNames;
		const char *InstanceEnabledExtensionNames[3] = {
			VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
			VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef VK_KHR_win32_surface
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif defined(VK_KHR_xcb_surface)
			VK_KHR_XCB_SURFACE_EXTENSION_NAME
#elif defined(VK_KHR_android_surface)
			VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
#endif
		};
		InstanceInfo.enabledExtensionCount = 3U;
		InstanceInfo.ppEnabledExtensionNames = InstanceEnabledExtensionNames;
#else
		InstanceInfo.enabledLayerCount = 0U;
		InstanceInfo.ppEnabledLayerNames = NULL;
		const char *InstanceEnabledExtensionNames[2] = {
			VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef VK_KHR_win32_surface
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif defined(VK_KHR_xcb_surface)
			VK_KHR_XCB_SURFACE_EXTENSION_NAME
#elif defined(VK_KHR_android_surface)
			VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
#endif
		};
		InstanceInfo.enabledExtensionCount = 2U;
		InstanceInfo.ppEnabledExtensionNames = InstanceEnabledExtensionNames;
#endif
		Result = pFn_vkCreateInstance(&InstanceInfo, &PTAllocator, &hInstance);
		assert(Result == VK_SUCCESS);
	}
	
	if (hInstance == VK_NULL_HANDLE)
		return NULL;

	VkInstanceCpp hInstanceCpp;
	hInstanceCpp.Construct(hInstance, pFn_vkGetInstanceProcAddr);

#ifdef PTDEBUG
	VkDebugReportCallbackEXT hDebugReportCallback;
	{
		VkDebugReportCallbackCreateInfoEXT DebugReportCallbackInfo;
		DebugReportCallbackInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
		DebugReportCallbackInfo.pNext = NULL;
		DebugReportCallbackInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
		DebugReportCallbackInfo.pfnCallback = PTDebugReportCallbackEXT;
		DebugReportCallbackInfo.pUserData = reinterpret_cast<void*>(-1);
		hInstanceCpp.CreateDebugReportCallbackEXT(&DebugReportCallbackInfo, &PTAllocator, &hDebugReportCallback);
	}
#endif

	//PhysicalDevice
	VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
	VkPhysicalDeviceType PhysicalDeviceType;
	{
		uint32_t PhysicalDeviceCount;
		hInstanceCpp.EnumeratePhysicalDevices(&PhysicalDeviceCount, NULL);
		VkPhysicalDevice *pPhysicalDevices;
		pPhysicalDevices = static_cast<VkPhysicalDevice *>(pHeap->AlignedAlloc(sizeof(VkPhysicalDevice)*PhysicalDeviceCount, alignof(VkPhysicalDevice)));
		hInstanceCpp.EnumeratePhysicalDevices(&PhysicalDeviceCount, pPhysicalDevices);
		for (uint32_t i = 0U; i < PhysicalDeviceCount; ++i)
		{
			VkPhysicalDeviceProperties PhysicalDeviceProperties;
			hInstanceCpp.GetPhysicalDeviceProperties(pPhysicalDevices[i], &PhysicalDeviceProperties);
			PhysicalDevice = pPhysicalDevices[i];
			PhysicalDeviceType = PhysicalDeviceProperties.deviceType;
			if (PhysicalDeviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				break;//优先使用独显
			}	
		}
	}

	if (PhysicalDevice == VK_NULL_HANDLE)
		return NULL;

	//DeviceInfo
	enum :uint32_t
	{
		PTARCH_UNKNOWN,
		PTARCH_NUMA_NCC,
		PTARCH_NUMA_CC,
		PTARCH_UMA_NCC,
		PTARCH_UMA_CC
	}PTArchitecture = PTARCH_UNKNOWN;//对应于D3D12_FEATURE_DATA_ARCHITECTURE
	uint32_t DirectQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;//对应于D3D12_COMMAND_LIST_TYPE_DIRECT
	uint32_t ComputeQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;//对应于D3D12_COMMAND_LIST_TYPE_COMPUTE
	uint32_t CopyQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;//对应于D3D12_COMMAND_LIST_TYPE_COPY
	VkDeviceSize SystemMemorySize;//对应于D3D12_MEMORY_POOL_L0
	uint32_t SystemMemoryNormalIndexArray[VK_MAX_MEMORY_TYPES];//对应于D3D12_CPU_PAGE_PROPERTY_WRITE_BACK
	uint32_t SystemMemoryNormalIndexCount = 0U;
	uint32_t SystemMemoryWriteCombineIndexArray[VK_MAX_MEMORY_TYPES];//对应于D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE
	uint32_t SystemMemoryWriteCombineIndexCount = 0U;
	VkDeviceSize VideoMemorySize;//对应于D3D12_MEMORY_POOL_L1
	uint32_t VideoMemoryIndexArray[VK_MAX_MEMORY_TYPES];//对应于D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE
	uint32_t VideoMemoryIndexCount = 0U;
	{
		//QueueFamilyProperties-------------------------------------------------------------------------
		uint32_t QueueFamilyPropertyCount;
		hInstanceCpp.GetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyPropertyCount, NULL);
		VkQueueFamilyProperties *pQueueFamilyProperties = static_cast<VkQueueFamilyProperties *>(pHeap->AlignedAlloc(sizeof(VkQueueFamilyProperties)*QueueFamilyPropertyCount, alignof(VkQueueFamilyProperties)));
		hInstanceCpp.GetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyPropertyCount, pQueueFamilyProperties);
		for (uint32_t iQF = 0U; (DirectQueueFamilyIndex == VK_QUEUE_FAMILY_IGNORED || ComputeQueueFamilyIndex == VK_QUEUE_FAMILY_IGNORED || CopyQueueFamilyIndex == VK_QUEUE_FAMILY_IGNORED) && iQF < QueueFamilyPropertyCount; ++iQF)
		{
			if ((pQueueFamilyProperties[iQF].queueFlags&VK_QUEUE_GRAPHICS_BIT) && (pQueueFamilyProperties[iQF].queueFlags&VK_QUEUE_COMPUTE_BIT))//支持图形、计算和传输操作
			{
#ifdef VK_KHR_win32_surface
				if (hInstanceCpp.GetPhysicalDeviceWin32PresentationSupportKHR(PhysicalDevice, iQF) == VK_TRUE)//在D3D12中，支持图形和计算操作的直接命令队列同时支持呈现
				{
					if (DirectQueueFamilyIndex == VK_QUEUE_FAMILY_IGNORED)
						DirectQueueFamilyIndex = iQF;
				}
#elif defined(VK_KHR_xcb_surface)

#elif defined(VK_KHR_android_surface)
				if (DirectQueueFamilyIndex == VK_QUEUE_FAMILY_IGNORED)//Android平台一定支持呈现
					DirectQueueFamilyIndex = iQF;
#endif
			}
			else if ((!(pQueueFamilyProperties[iQF].queueFlags&VK_QUEUE_GRAPHICS_BIT)) && (pQueueFamilyProperties[iQF].queueFlags&VK_QUEUE_COMPUTE_BIT))//支持计算和传输操作
			{
				if (ComputeQueueFamilyIndex == VK_QUEUE_FAMILY_IGNORED)
					ComputeQueueFamilyIndex = iQF;
			}
			else if ((!(pQueueFamilyProperties[iQF].queueFlags&(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT))) && (pQueueFamilyProperties[iQF].queueFlags&VK_QUEUE_TRANSFER_BIT))//支持传输操作
			{
				if (CopyQueueFamilyIndex == VK_QUEUE_FAMILY_IGNORED)
					CopyQueueFamilyIndex = iQF;
			}
		}
		pHeap->AlignedFree(pQueueFamilyProperties);

		//MemoryProperties-------------------------------------------------------------------------
		VkPhysicalDeviceMemoryProperties MemoryProperties;
		hInstanceCpp.GetPhysicalDeviceMemoryProperties(PhysicalDevice, &MemoryProperties);

		//vulkan规定，至少有一个堆设置了VK_MEMORY_HEAP_DEVICE_LOCAL_BIT标志，如果未找到，那么表明发生了错误
		uint32_t iHeapDeviceLocal = 0U;
		while (iHeapDeviceLocal < MemoryProperties.memoryHeapCount)
		{
			if (MemoryProperties.memoryHeaps[iHeapDeviceLocal].flags&VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
				break;
			++iHeapDeviceLocal;
		}
		assert(iHeapDeviceLocal < MemoryProperties.memoryHeapCount);

		switch (PhysicalDeviceType)
		{
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU://独显（即NUMA）
		{
			//NUMA通常有两个堆，显示内存堆是设备局部的而系统内存堆不是设备局部的
			uint32_t iHeapNonDeviceLocal = 0U;
			while (iHeapNonDeviceLocal < MemoryProperties.memoryHeapCount)
			{
				if (!(MemoryProperties.memoryHeaps[iHeapNonDeviceLocal].flags&VK_MEMORY_HEAP_DEVICE_LOCAL_BIT))
					break;
				++iHeapNonDeviceLocal;
			}
			assert(iHeapNonDeviceLocal < MemoryProperties.memoryHeapCount);

			SystemMemorySize = MemoryProperties.memoryHeaps[iHeapNonDeviceLocal].size;
			VideoMemorySize = MemoryProperties.memoryHeaps[iHeapDeviceLocal].size;

			//显示内存堆通常有以下一种内存类型
			//VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			for (uint32_t i = 0U; i < MemoryProperties.memoryTypeCount; ++i)
			{
				if ((MemoryProperties.memoryTypes[i].heapIndex == iHeapDeviceLocal) && (MemoryProperties.memoryTypes[i].propertyFlags == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
				{
					VideoMemoryIndexArray[VideoMemoryIndexCount] = i;
					++VideoMemoryIndexCount;
				}
			}
			assert(VideoMemoryIndexCount > 0U);

			//NCC_NUMA的系统内存堆通常有以下两种内存类型
			//VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			//VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_CACHED_BIT
			//CC_NUMA的系统内存堆通常有以下两种内存类型
			//VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			//VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_CACHED_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT

			//根据是否存在内存类型VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_CACHED_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT来区分NCC_NUMA和CC_NUMA
			for (uint32_t i = 0U; i < MemoryProperties.memoryTypeCount; ++i)
			{
				if ((MemoryProperties.memoryTypes[i].heapIndex == iHeapNonDeviceLocal) && (MemoryProperties.memoryTypes[i].propertyFlags == (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)))
				{
					SystemMemoryNormalIndexArray[SystemMemoryNormalIndexCount] = i;
					++SystemMemoryNormalIndexCount;
				}
			}

			//NUMA_CC
			if (SystemMemoryNormalIndexArray > 0U)
			{
				PTArchitecture = PTARCH_NUMA_CC;
			}
			//NUMA_NCC
			else
			{
				PTArchitecture = PTARCH_NUMA_NCC;
				for (uint32_t i = 0U; i < MemoryProperties.memoryTypeCount; ++i)
				{
					if ((MemoryProperties.memoryTypes[i].heapIndex == iHeapNonDeviceLocal) && (MemoryProperties.memoryTypes[i].propertyFlags == (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT)))
					{
						SystemMemoryNormalIndexArray[SystemMemoryNormalIndexCount] = i;
						++SystemMemoryNormalIndexCount;
					}
				}
				assert(SystemMemoryNormalIndexArray > 0U);
			}

			//NUMA_CC或NUMA_NCC
			for (uint32_t i = 0U; i < MemoryProperties.memoryTypeCount; ++i)
			{
				if ((MemoryProperties.memoryTypes[i].heapIndex == iHeapNonDeviceLocal) && (MemoryProperties.memoryTypes[i].propertyFlags == (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)))
				{
					SystemMemoryWriteCombineIndexArray[SystemMemoryWriteCombineIndexCount] = i;
					++SystemMemoryWriteCombineIndexCount;
				}
			}
		}
		break;
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU://集显（即UMA）
		{
			//UMA通常只有一个堆，即系统内存堆，既是设备局部的，又可以被CPU访问
			SystemMemorySize = MemoryProperties.memoryHeaps[iHeapDeviceLocal].size;

			//NCC_UMA的系统内存堆通常有以下两种内存类型
			//VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT|VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			//VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT|VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_CACHED_BIT

			//CC_UMA的系统内存堆通常有以下两种内存类型
			//VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT|VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			//VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT|VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_CACHED_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT

			//根据是否存在内存类型VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT|VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_CACHED_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT来区分NCC_NUMA和CC_NUMA
			for (uint32_t i = 0U; i < MemoryProperties.memoryTypeCount; ++i)
			{
				if ((MemoryProperties.memoryTypes[i].heapIndex == iHeapDeviceLocal) && (MemoryProperties.memoryTypes[i].propertyFlags == (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)))
				{
					SystemMemoryNormalIndexArray[SystemMemoryNormalIndexCount] = i;
					++SystemMemoryNormalIndexCount;
				}
			}

			//UMA_CC
			if (SystemMemoryNormalIndexCount > 0U)
			{
				PTArchitecture = PTARCH_UMA_CC;
			}
			//UMA_NCC
			else
			{
				PTArchitecture = PTARCH_UMA_NCC;
				for (uint32_t i = 0U; i < MemoryProperties.memoryTypeCount; ++i)
				{
					if ((MemoryProperties.memoryTypes[i].heapIndex == iHeapDeviceLocal) && (MemoryProperties.memoryTypes[i].propertyFlags == (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT)))
					{
						SystemMemoryNormalIndexArray[SystemMemoryNormalIndexCount] = i;
						++SystemMemoryNormalIndexCount;
					}
				}
				assert(SystemMemoryNormalIndexArray > 0U);
			}

			//UMA_CC或UMA_NCC
			for (uint32_t i = 0U; i < MemoryProperties.memoryTypeCount; ++i)
			{
				if ((MemoryProperties.memoryTypes[i].heapIndex == iHeapDeviceLocal) && (MemoryProperties.memoryTypes[i].propertyFlags == (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)))
				{
					SystemMemoryWriteCombineIndexArray[SystemMemoryWriteCombineIndexCount] = i;
					++SystemMemoryWriteCombineIndexCount;
				}
			}
		}
		break;
		default:
			assert(0);
		}
	}

	assert(PTArchitecture != PTARCH_UNKNOWN);
	assert(DirectQueueFamilyIndex != VK_QUEUE_FAMILY_IGNORED);

	//Device
	VkDevice hDevice;
	{

		const char *DeviceEnabledExtensionNames[1] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		VkDeviceCreateInfo DeviceInfo;
		DeviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		DeviceInfo.pNext = NULL;
		DeviceInfo.flags = 0U;
		DeviceInfo.enabledLayerCount = 0U;
		DeviceInfo.ppEnabledLayerNames = NULL;
		DeviceInfo.enabledExtensionCount = 1U;
		DeviceInfo.ppEnabledExtensionNames = DeviceEnabledExtensionNames;
		VkPhysicalDeviceFeatures EnabledFeatures = {};//结构体中的各个成员都初始化为VK_FALSE
		DeviceInfo.pEnabledFeatures = &EnabledFeatures;

		VkDeviceQueueCreateInfo QueueInfos[3];
		DeviceInfo.queueCreateInfoCount = 0;
		DeviceInfo.pQueueCreateInfos = QueueInfos;

		//对应于Direct3D12中的直接命令队列
		{
			QueueInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			QueueInfos[0].pNext = NULL;
			QueueInfos[0].flags = 0U;
			QueueInfos[0].queueFamilyIndex = DirectQueueFamilyIndex;
			float QueuePriorities[1] = { 0.0f };//D3D12_COMMAND_QUEUE_PRIORITY_NORMAL
			QueueInfos[0].queueCount = 1U;
			QueueInfos[0].pQueuePriorities = QueuePriorities;
			++DeviceInfo.queueCreateInfoCount;
		}

		//对应于D3D12中的计算命令队列
		if (ComputeQueueFamilyIndex != VK_QUEUE_FAMILY_IGNORED)
		{
			QueueInfos[DeviceInfo.queueCreateInfoCount].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			QueueInfos[DeviceInfo.queueCreateInfoCount].pNext = NULL;
			QueueInfos[DeviceInfo.queueCreateInfoCount].flags = 0U;
			QueueInfos[DeviceInfo.queueCreateInfoCount].queueFamilyIndex = ComputeQueueFamilyIndex;
			float QueuePriorities[1] = { 0.0f };//D3D12_COMMAND_QUEUE_PRIORITY_NORMAL
			QueueInfos[DeviceInfo.queueCreateInfoCount].queueCount = 1U;
			QueueInfos[DeviceInfo.queueCreateInfoCount].pQueuePriorities = QueuePriorities;
			++DeviceInfo.queueCreateInfoCount;
		}

		if ((PTArchitecture == PTARCH_NUMA_NCC) || (PTArchitecture == PTARCH_NUMA_CC))
		{
			//对应于D3D12中的复制命令队列
			if (CopyQueueFamilyIndex != VK_QUEUE_FAMILY_IGNORED)
			{
				QueueInfos[DeviceInfo.queueCreateInfoCount].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				QueueInfos[DeviceInfo.queueCreateInfoCount].pNext = NULL;
				QueueInfos[DeviceInfo.queueCreateInfoCount].flags = 0U;
				QueueInfos[DeviceInfo.queueCreateInfoCount].queueFamilyIndex = CopyQueueFamilyIndex;
				float QueuePriorities[1] = { 1.0f };//D3D12_COMMAND_QUEUE_PRIORITY_HIGH
				QueueInfos[DeviceInfo.queueCreateInfoCount].queueCount = 1U;
				QueueInfos[DeviceInfo.queueCreateInfoCount].pQueuePriorities = QueuePriorities;
				++DeviceInfo.queueCreateInfoCount;
			}
		}	

		Result = hInstanceCpp.CreateDevice(PhysicalDevice, &DeviceInfo, &PTAllocator, &hDevice);
		assert(Result == VK_SUCCESS);
	}

	if (hDevice == VK_NULL_HANDLE)
		return NULL;

	decltype(vkGetDeviceProcAddr) *pFn_vkGetDeviceProcAddr = reinterpret_cast<decltype(vkGetDeviceProcAddr) *>(pFn_vkGetInstanceProcAddr(hInstance, "vkGetDeviceProcAddr"));

	VkDeviceCpp hDeviceCpp;
	hDeviceCpp.Construct(hDevice, pFn_vkGetDeviceProcAddr);

	VkQueue hCommandQueueDirect = VK_NULL_HANDLE;
	VkQueue hCommandQueueCompute = VK_NULL_HANDLE;
	VkQueue hCommandQueueCopy = VK_NULL_HANDLE;
	{
		hDeviceCpp.GetDeviceQueue(DirectQueueFamilyIndex, 0U, &hCommandQueueDirect);
		
		if (ComputeQueueFamilyIndex != VK_QUEUE_FAMILY_IGNORED)
		{
			hDeviceCpp.GetDeviceQueue(ComputeQueueFamilyIndex, 0U, &hCommandQueueCompute);
		}

		if ((PTArchitecture == PTARCH_NUMA_NCC) || (PTArchitecture == PTARCH_NUMA_CC))
		{
			if (CopyQueueFamilyIndex != VK_QUEUE_FAMILY_IGNORED)
			{
				hDeviceCpp.GetDeviceQueue(CopyQueueFamilyIndex, 0U, &hCommandQueueCopy);
			}
		}
	}

	VkCommandPool hCommandAllocatorDirect = VK_NULL_HANDLE;
	VkCommandPool hCommandAllocatorCompute = VK_NULL_HANDLE;
	VkCommandPool hCommandAllocatorCopy = VK_NULL_HANDLE;
	{
		VkCommandPoolCreateInfo CommandPoolInfo;
		CommandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		CommandPoolInfo.pNext = NULL;
		CommandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;//ID3D12GraphicsCommandList::Reset
		
		CommandPoolInfo.queueFamilyIndex = DirectQueueFamilyIndex;
		hDeviceCpp.CreateCommandPool(&CommandPoolInfo, &PTAllocator, &hCommandAllocatorDirect);

		if (ComputeQueueFamilyIndex != VK_QUEUE_FAMILY_IGNORED)
		{
			CommandPoolInfo.queueFamilyIndex = ComputeQueueFamilyIndex;
			hDeviceCpp.CreateCommandPool(&CommandPoolInfo, &PTAllocator, &hCommandAllocatorCompute);
		}

		if ((PTArchitecture == PTARCH_NUMA_NCC) || (PTArchitecture == PTARCH_NUMA_CC))
		{
			if (CopyQueueFamilyIndex != VK_QUEUE_FAMILY_IGNORED)
			{
				CommandPoolInfo.queueFamilyIndex = CopyQueueFamilyIndex;
				hDeviceCpp.CreateCommandPool(&CommandPoolInfo, &PTAllocator, &hCommandAllocatorCopy);
			}
		}
	}


	//tiling
	//	D3D12_HEAP_FLAG_SHARED_CROSS_ADAPTER(D3D12_TEXTURE_LAYOUT_ROW_MAJOR)
	//usage
	//	D3D12_HEAP_FLAG_DENY_BUFFERS
	//	D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES
	//	D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES

	//Memory Management Strategies
	//http://msdn.microsoft.com/en-us/library/mt613239
	//Critical //Scaled/Optional //Streaming
	uint64_t MaxSizeAssetVBVIBV = 256ULL * 1024ULL * 1024ULL;
	uint64_t MaxSizeAssetSRV = 256ULL * 1024ULL * 1024ULL;
	//Re-used 
	uint64_t MaxSizeFrameCBV = 1024ULL;
	uint64_t MaxSizeFrameUAVSRV = 1024ULL;
	uint64_t MaxSizeFrameRTVDSVSRV = 1024ULL;
	//Streaming 
	uint64_t MaxSizeStreamCPSC = 1024ULL;

	//Memory Management Strategies
	//http://msdn.microsoft.com/en-us/library/mt613239
	//Critical //Scaled/Optional //Streaming
	VkDeviceMemory hHeapAssetVBVIBV = VK_NULL_HANDLE;
	VkDeviceMemory hHeapAssetSRV = VK_NULL_HANDLE;
	//Re-used 
	VkDeviceMemory hHeapFrameCBV = VK_NULL_HANDLE;
	VkDeviceMemory hHeapFrameUAVSRV = VK_NULL_HANDLE;
	VkDeviceMemory hHeapFrameRTVDSVSRV = VK_NULL_HANDLE;
	//Streaming 
	VkDeviceMemory hHeapStreamCPSC = VK_NULL_HANDLE;

	{
		switch (PTArchitecture)
		{
		case PTARCH_NUMA_NCC:
		{
			VkBufferCreateInfo BufferInfo;
			BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			BufferInfo.pNext = NULL;
			BufferInfo.flags = 0U;
			BufferInfo.size = 65536U;//对应于D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT
			BufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			BufferInfo.queueFamilyIndexCount = 0U;
			BufferInfo.pQueueFamilyIndices = NULL;

			VkBuffer hBufferVTID;
			Result = hDeviceCpp.CreateBuffer(&BufferInfo, &PTAllocator, &hBufferVTID);
			assert(Result == VK_SUCCESS);

			VkMemoryRequirements MemoryRequirements;
			hDeviceCpp.GetBufferMemoryRequirements(hBufferVTID, &MemoryRequirements);

			for(uint32_t i=0U;i<VideoMemoryIndexCount;++i)
			{
				if (MemoryRequirements.memoryTypeBits&(1U << VideoMemoryIndexArray[i]))
				{
					VkMemoryAllocateInfo AllocateInfo;
					AllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
					AllocateInfo.pNext = NULL;
					AllocateInfo.allocationSize = MaxSizeAssetVBVIBV;
					AllocateInfo.memoryTypeIndex = VideoMemoryIndexArray[i];
					hDeviceCpp.AllocateMemory(&AllocateInfo, &PTAllocator, &hHeapAssetVBVIBV);
					break;
				}
			}

		}
		break;
		case PTARCH_UMA_CC:
		{
			VkMemoryRequirements MemoryRequirements;

			VkBufferCreateInfo BufferInfo;
			BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			BufferInfo.pNext = NULL;
			BufferInfo.flags = 0U;
			BufferInfo.size = 65536U;//对应于D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT
			BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			BufferInfo.queueFamilyIndexCount = 0U;
			BufferInfo.pQueueFamilyIndices = NULL;

			//AssetVBVIBV---------------------------------------------------------------
			BufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

			VkBuffer hBufferAssetVBVIBV;
			Result = hDeviceCpp.CreateBuffer(&BufferInfo, &PTAllocator, &hBufferAssetVBVIBV);
			assert(Result == VK_SUCCESS);
			hDeviceCpp.GetBufferMemoryRequirements(hBufferAssetVBVIBV, &MemoryRequirements);
			hDeviceCpp.DestroyBuffer(hBufferAssetVBVIBV, &PTAllocator);

			for (uint32_t i = 0U; i<SystemMemoryNormalIndexCount; ++i)
			{
				if (MemoryRequirements.memoryTypeBits&(1U << SystemMemoryNormalIndexArray[i]))
				{
					VkMemoryAllocateInfo AllocateInfo;
					AllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
					AllocateInfo.pNext = NULL;
					AllocateInfo.allocationSize = MaxSizeAssetVBVIBV;
					AllocateInfo.memoryTypeIndex = SystemMemoryNormalIndexArray[i];
					Result = hDeviceCpp.AllocateMemory(&AllocateInfo, &PTAllocator, &hHeapAssetVBVIBV);
					assert(Result == VK_SUCCESS);
					break;
				}
			}

			//FrameCBV---------------------------------------------------------------

			BufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

			VkBuffer hBufferFrameCBV;
			Result = hDeviceCpp.CreateBuffer(&BufferInfo, &PTAllocator, &hBufferFrameCBV);
			assert(Result == VK_SUCCESS);
			hDeviceCpp.GetBufferMemoryRequirements(hBufferFrameCBV, &MemoryRequirements);
			hDeviceCpp.DestroyBuffer(hBufferFrameCBV, &PTAllocator);

			for (uint32_t i = 0U; i<SystemMemoryNormalIndexCount; ++i)
			{
				if (MemoryRequirements.memoryTypeBits&(1U << SystemMemoryNormalIndexArray[i]))
				{
					VkMemoryAllocateInfo AllocateInfo;
					AllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
					AllocateInfo.pNext = NULL;
					AllocateInfo.allocationSize = MaxSizeFrameCBV;
					AllocateInfo.memoryTypeIndex = SystemMemoryNormalIndexArray[i];
					Result = hDeviceCpp.AllocateMemory(&AllocateInfo, &PTAllocator, &hHeapAssetVBVIBV);
					assert(Result == VK_SUCCESS);
					break;
				}
			}
		}
		break;
		default:
			break;
		}
	}

	//VkPhysicalDeviceLimits::bufferImageGranularity

	void *pMemory = pHeap->AlignedAlloc(sizeof(PTGraphicDeviceImpl), alignof(PTGraphicDeviceImpl));
	assert(pMemory != NULL);

	return static_cast<IPTGraphicDevice *>(
		new(pMemory)PTGraphicDeviceImpl(
			hInstanceCpp,
#ifdef PTDEBUG
			hDebugReportCallback,
#endif
			hDeviceCpp,
			hCommandQueueDirect,
			hCommandQueueCompute,
			hCommandQueueCopy,
			hCommandAllocatorDirect,
			hCommandAllocatorCompute,
			hCommandAllocatorCopy
		)
		);
}

void PTGraphicDeviceImpl::Destory(IPTHeap *pHeap)
{
	VkAllocationCallbacks PTAllocator;
	PTAllocator.pUserData = static_cast<void *>(pHeap);
	PTAllocator.pfnAllocation = PTAllocationFunction;
	PTAllocator.pfnReallocation = PTReallocationFunction;
	PTAllocator.pfnFree = PTFreeFunction;
	PTAllocator.pfnInternalAllocation = NULL;
	PTAllocator.pfnInternalFree = NULL;

	if (m_hCommandAllocatorCopy != VK_NULL_HANDLE)
		m_hDeviceCpp.DestroyCommandPool(m_hCommandAllocatorCopy, &PTAllocator);
	if (m_hCommandAllocatorCompute != VK_NULL_HANDLE)
		m_hDeviceCpp.DestroyCommandPool(m_hCommandAllocatorCompute, &PTAllocator);
	m_hDeviceCpp.DestroyCommandPool(m_hCommandAllocatorDirect, &PTAllocator);
	m_hDeviceCpp.DestroyDevice(&PTAllocator);
#ifdef PTDEBUG
	m_hInstanceCpp.DestroyDebugReportCallbackEXT(m_hDebugReportCallback, &PTAllocator);
#endif
	m_hInstanceCpp.DestroyInstance(&PTAllocator);

	pHeap->AlignedFree(this);
}


inline PTGraphicDeviceImpl::PTGraphicDeviceImpl(
	VkInstanceWrapper hInstanceCpp,
#ifdef PTDEBUG
	VkDebugReportCallbackEXT hDebugReportCallback,
#endif
	VkDeviceWrapper hDeviceCpp,
	VkQueue hCommandQueueDirect,
	VkQueue hCommandQueueCompute,
	VkQueue hCommandQueueCopy,
	VkCommandPool hCommandAllocatorDirect,
	VkCommandPool hCommandAllocatorCompute,
	VkCommandPool hCommandAllocatorCopy)
	:
	m_hInstanceCpp(hInstanceCpp),
#ifdef PTDEBUG
	m_hDebugReportCallback(hDebugReportCallback),
#endif
	m_hDeviceCpp(hDeviceCpp),
	m_hCommandQueueDirect(hCommandQueueDirect),
	m_hCommandQueueCompute(hCommandQueueCompute),
	m_hCommandQueueCopy(hCommandQueueCopy),
	m_hCommandAllocatorDirect(hCommandAllocatorDirect),
	m_hCommandAllocatorCompute(hCommandAllocatorCompute),
	m_hCommandAllocatorCopy(hCommandAllocatorCopy)
{

}