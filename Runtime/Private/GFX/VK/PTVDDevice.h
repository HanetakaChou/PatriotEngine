#ifndef PT_VIDEO_VULKAN_DEVICE_H
#define PT_VIDEO_VULKAN_DEVICE_H

#include "PTVDVulkanHeader.h"

//We Use Tripple Buffer Semantic
static constexpr uint32_t const s_3DContext_FrameCount = 3U;

struct IPTVDInstance;

struct PTVD_Context3D
{
	//↓Command ----------------------------------------
	uint32_t m_FrameIndex;
	VkCommandPool m_hCommandAllocator3D[s_3DContext_FrameCount];
	VkCommandBuffer m_hCommandList3D[s_3DContext_FrameCount];
	
	VkSemaphore m_hSemaphore_Allow_ReadFromFrameBuffer[s_3DContext_FrameCount]; //用于同一Queue中Submit和Present之间的同步 //Fence仅同步Submit 不保证Present完成
	VkFence m_hFence_Allow_ResetCommandList[s_3DContext_FrameCount]; //与CPU同步 以允许释放CommandList //hSemaphore_Allow_(ReadFrom)Present

	//----------------------------------------
	VkSemaphore m_hSemaphore_Allow_WriteToFrameBuffer[s_3DContext_FrameCount]; //允许写入FrameBuffer //hSemaphore_Allow_ColorAttachmentOutput 

	//↓Pipeline
	
	//CommandList Reset //Wait hFence_Allow_ResetCommandList

	//hSemaphore_Allow_(ReadFrom)Present应该只需要一个？

	//Queue Submit CommandList //Wait hSemaphore_Allow_ColorAttachmentOutput //Allow hSemaphore_Allow_(ReadFrom)Present //Allow hFence_Allow_ResetCommandList

	//Queue Present //Wait hSemaphore_Allow_(ReadFrom)Present //We Need Three？？？ //Because We Acquire Different Image From The SwapChain Using The Same API
	
	//Queue Acquire //Allow hSemaphore_Allow_ColorAttachmentOutput //并不与CPU同步 //比Fence提前返回

	//↓SwapChain ----------------------------------------
	uint32_t m_SwapChain_bLocked;
	uint32_t m_SwapChain_TimeStamp;
	//----------------------------------------
	VkSwapchainKHR m_hSwapChain;
	//----------------------------------------
	VkImage *m_pSwapChain_Image;
	VkImageView *m_pSwapChain_ImageView;
	uint32_t m_SwapChain_ImageCount;
};

struct IPTVD_Device
{
	virtual void Release() = 0;

	virtual void StartUp3D(PTVD_Context3D *pContext3D) = 0;


	//↓Present --------------------------------------
	
	//Call On UI Thread
#ifndef NDEBUG
	virtual VkBool32 SwapChain_PresentValidate(VkInstanceWrapper &rhInstanceWrapper, VkSurfaceKHR hSurface) = 0;
#endif
	virtual void SwapChain_Update(PTVD_Context3D *pContext3D, VkInstanceWrapper &rhInstanceWrapper, VkSurfaceKHR hSurface, uint32_t Width, uint32_t Height) = 0;
	virtual void SwapChain_Destroy(PTVD_Context3D *pContext3D) = 0;

	//Call On Render Thread
	//Wait When NULL
	virtual void SwapChain_Present(PTVD_Context3D *pContext3D) = 0;
};

struct IPTVD_QueueManager;
struct IPTVDHeapManager_NUMA;
struct IPTVDHeapManager_UMA;

class PTVDDevice_Base : public IPTVD_Device
{
protected:
	VkPhysicalDevice m_hPhysicalDevice;
	VkDeviceWrapper m_hDeviceWrapper;

	IPTVD_QueueManager *m_pCommandQueueManager;

	//bool bSwapChainImageSupportUAV; //A Pointer To Function //Copy By UAV Or RTV //Resemble Virtual Function
	//uint32_t m_SwapChainImageWidth;
	//uint32_t m_SwapChainImageHeight;

	inline PTVDDevice_Base(
		VkPhysicalDevice hPhysicalDevice,
		VkDeviceWrapper &hDeviceWrapper,
		IPTVD_QueueManager *pCommandQueueManager
	) 
		:
		m_hPhysicalDevice(hPhysicalDevice),
		m_hDeviceWrapper(hDeviceWrapper), 
		m_pCommandQueueManager(pCommandQueueManager)
	{

	}

	inline ~PTVDDevice_Base()
	{
		m_hDeviceWrapper.DestroyDevice(NULL);
	}

	void StartUp3D(PTVD_Context3D *pContext3D) override;
#ifndef NDEBUG
	VkBool32 SwapChain_PresentValidate(VkInstanceWrapper &rhInstanceWrapper, VkSurfaceKHR hSurface) override;
#endif
	void SwapChain_Update(PTVD_Context3D *pContext3D, VkInstanceWrapper &rhInstanceWrapper, VkSurfaceKHR hSurface, uint32_t Width, uint32_t Height) override;
	void SwapChain_Destroy(PTVD_Context3D *pContext3D) override;
	void SwapChain_Present(PTVD_Context3D *pContext3D) override;
};

class PTVDDevice_NUMA : public PTVDDevice_Base
{
	IPTVDHeapManager_NUMA *m_pHeapManager;

public:
	inline PTVDDevice_NUMA(
		VkPhysicalDevice hPhysicalDevice,
		VkDeviceWrapper &hDeviceWrapper,
		IPTVD_QueueManager *pCommandQueueManager,
		IPTVDHeapManager_NUMA *pHeapManager
	) :
		PTVDDevice_Base(hPhysicalDevice, hDeviceWrapper, pCommandQueueManager),
		m_pHeapManager(pHeapManager)
	{

	}

	inline ~PTVDDevice_NUMA()
	{

	}

	void Release() override
	{
		this->~PTVDDevice_NUMA();
	}
};

class PTVDDevice_UMA : public PTVDDevice_Base
{
	IPTVDHeapManager_UMA *m_pHeapManager;

public:
	inline PTVDDevice_UMA(
		VkPhysicalDevice hPhysicalDevice,
		VkDeviceWrapper &hDeviceWrapper,
		IPTVD_QueueManager *pCommandQueueManager,
		IPTVDHeapManager_UMA *pHeapManager
	) :
		PTVDDevice_Base(hPhysicalDevice, hDeviceWrapper, pCommandQueueManager),
		m_pHeapManager(pHeapManager)
	{

	}

	inline ~PTVDDevice_UMA()
	{

	}

	void Release() override
	{
		this->~PTVDDevice_UMA();
	}
};

#endif