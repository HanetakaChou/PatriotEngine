#include "../../../../Public/Graphics/Device/PTGDevice.h"

#if defined(PTWIN32)
#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#if defined(PTWIN32DESKTOP)
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#elif defined(PTPOSIX)
#if defined(PTPOSIXXCB)
#define VK_USE_PLATFORM_XCB_KHR
#elif defined(PTPOSIXANDROID)
#define VK_USE_PLATFORM_ANDROID_KHR
#else
#error 未知的架构
#endif
#else

#endif

#include "../../../../ThirdParty/vulkan/Include/vulkan/vulkan.h"
#include "PTVulkanWrapper.inl"

class PTGraphicDeviceImpl : public IPTGDevice
{
	void Destory(IPTHeap *pHeap) override;

	VkInstanceWrapper m_hInstanceCpp;
#ifdef PTDEBUG
	VkDebugReportCallbackEXT m_hDebugReportCallback;
#endif
	VkDeviceWrapper m_hDeviceCpp;
	VkQueue m_hCommandQueueDirect;
	VkQueue m_hCommandQueueCompute;
	VkQueue m_hCommandQueueCopy;
	VkCommandPool m_hCommandAllocatorDirect;
	VkCommandPool m_hCommandAllocatorCompute;
	VkCommandPool m_hCommandAllocatorCopy;
	VkDeviceMemory m_hHeapBuffer;
	VkDeviceMemory m_hHeapTextureNonRTDS;
	VkDeviceMemory m_hHeapTextureRTDS;
	inline ~PTGraphicDeviceImpl();

public:
	inline PTGraphicDeviceImpl(
		VkInstanceWrapper hInstanceCpp,
#ifdef PTDEBUG
		VkDebugReportCallbackEXT hDebugReportCallback,
#endif
		VkDeviceWrapper hDeviceCpp,
		VkQueue hCommandQueueGraphics,
		VkQueue hCommandQueueCompute,
		VkQueue hCommandQueueCopy,
		VkCommandPool hCommandAllocatorGraphics,
		VkCommandPool hCommandAllocatorCompute,
		VkCommandPool hCommandAllocatorCopy);
};