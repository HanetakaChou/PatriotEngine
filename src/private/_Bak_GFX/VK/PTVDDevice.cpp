#include "PTVDDevice.h"

#include "../../../Public/McRT/PTSThread.h"
#include "../../../Public/McRT/PTSMemoryAllocator.h"

#include "PTVDQueue.h"

#include <assert.h>
#include <new>

void PTVDDevice_Base::StartUp3D(PTVD_Context3D *pContext3D)
{
	for (uint32_t iFrame = 0U; iFrame < s_3DContext_FrameCount; ++iFrame)
	{
		pContext3D->m_FrameIndex = s_3DContext_FrameCount - 1U;

		VkCommandPool hCommandPool = m_pCommandQueueManager->CommandAllocator3DCreate(m_hDeviceWrapper);
		
		VkCommandBuffer hCommandList = m_pCommandQueueManager->CommandList3DCreate(m_hDeviceWrapper, hCommandPool);
		m_pCommandQueueManager->CommandList3DClose(m_hDeviceWrapper,hCommandList);

		pContext3D->m_hCommandAllocator3D[iFrame] = hCommandPool;
		pContext3D->m_hCommandList3D[iFrame] = hCommandList;
		
		//-------------------------------------------------------------------
		VkSemaphore hSemaphoreSignal_LightPass_PostProcessPass;
		{
			VkSemaphoreCreateInfo CreateInfo;
			CreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			CreateInfo.pNext = NULL;
			CreateInfo.flags = 0U;
			VkResult eResult = m_hDeviceWrapper.CreateSemaphore(&CreateInfo, NULL, &hSemaphoreSignal_LightPass_PostProcessPass);
			assert(eResult == VK_SUCCESS);
		}
		pContext3D->m_hSemaphore_Signal_LightPassAndPostProcessPass[iFrame] = hSemaphoreSignal_LightPass_PostProcessPass;

		VkFence hFence_Signal_LightPassAndPostProcessPass;
		{
			VkFenceCreateInfo CreateInfo;
			CreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			CreateInfo.pNext = NULL;
			CreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			VkResult eResult = m_hDeviceWrapper.CreateFence(&CreateInfo, NULL, &hFence_Signal_LightPassAndPostProcessPass);
			assert(eResult == VK_SUCCESS);
		}
		pContext3D->m_hFence_Signal_LightPassAndPostProcessPass[iFrame] = hFence_Signal_LightPassAndPostProcessPass;
	}

	VkSemaphore hSemaphore_Signal_Present;
	{
		VkSemaphoreCreateInfo CreateInfo;
		CreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		CreateInfo.pNext = NULL;
		CreateInfo.flags = 0U;
		VkResult eResult = m_hDeviceWrapper.CreateSemaphore(&CreateInfo, NULL, &hSemaphore_Signal_Present);
		assert(eResult == VK_SUCCESS);
	}

	pContext3D->m_hSemaphore_Signal_Present = hSemaphore_Signal_Present;
}


#ifndef NDEBUG
VkBool32 PTVDDevice_Base::SwapChain_PresentValidate(VkInstanceWrapper &rhInstanceWrapper, VkSurfaceKHR hSurface)
{
	return m_pCommandQueueManager->Queue3DPresentValidate(rhInstanceWrapper, m_hPhysicalDevice, hSurface);
}
#endif

void PTVDDevice_Base::SwapChain_Update(PTVD_Context3D *pContext3D, VkInstanceWrapper &rhInstanceWrapper, VkSurfaceKHR hSurface, uint32_t Width, uint32_t Height)
{
	uint32_t MinImageCount;
	uint32_t ImageWidth;
	uint32_t ImageHeight;
	uint32_t ImageArrayLayers;
	bool bImageSupportUAV;
	uint32_t ImageUsage;
	VkSurfaceTransformFlagBitsKHR SurfaceCurrentTransform;
	VkCompositeAlphaFlagBitsKHR SurfaceCompositeAlpha;
	{
		VkSurfaceCapabilitiesKHR SurfaceCapabilities;
		{
			VkResult eResult = rhInstanceWrapper.GetPhysicalDeviceSurfaceCapabilitiesKHR(m_hPhysicalDevice, hSurface, &SurfaceCapabilities);
			assert(eResult == VK_SUCCESS);
		}

		//We Use Tripple Buffer Semantic
		MinImageCount = (s_3DContext_FrameCount > SurfaceCapabilities.minImageCount) ? s_3DContext_FrameCount : SurfaceCapabilities.minImageCount;
		MinImageCount = (SurfaceCapabilities.maxImageCount == 0U) ? MinImageCount : ((MinImageCount < SurfaceCapabilities.maxImageCount) ? MinImageCount : SurfaceCapabilities.maxImageCount);

		ImageWidth = (SurfaceCapabilities.currentExtent.width == 0XFFFFFFFFU) ? SurfaceCapabilities.currentExtent.width : Width;
		ImageHeight = (SurfaceCapabilities.currentExtent.height == 0XFFFFFFFFU) ? SurfaceCapabilities.currentExtent.height : Height;

		assert(1U <= SurfaceCapabilities.maxImageArrayLayers);
		ImageArrayLayers = 1U;

		bImageSupportUAV = (SurfaceCapabilities.supportedUsageFlags&VK_IMAGE_USAGE_STORAGE_BIT);
		assert(SurfaceCapabilities.supportedUsageFlags&VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
		ImageUsage = bImageSupportUAV ? VK_IMAGE_USAGE_STORAGE_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		SurfaceCurrentTransform = SurfaceCapabilities.currentTransform;

		if (SurfaceCapabilities.supportedCompositeAlpha&VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR)
		{
			SurfaceCompositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
		}
		else if (SurfaceCapabilities.supportedCompositeAlpha&VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)
		{
			SurfaceCompositeAlpha = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
		}
		else if (SurfaceCapabilities.supportedCompositeAlpha&VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
		{
			SurfaceCompositeAlpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
		}
		else
		{
			assert(SurfaceCapabilities.supportedCompositeAlpha&VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR);
			SurfaceCompositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		}
	}
	VkFormat ImageFormat;
	VkColorSpaceKHR ImageColorSpace;
	{
		uint32_t SurfaceFormatCount;
		{
			VkResult eResult = rhInstanceWrapper.GetPhysicalDeviceSurfaceFormatsKHR(m_hPhysicalDevice, hSurface, &SurfaceFormatCount, NULL);
			assert(eResult == VK_SUCCESS);
		}

		VkSurfaceFormatKHR *pSurfaceFormats;
		{
			pSurfaceFormats = static_cast<VkSurfaceFormatKHR *>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(VkSurfaceFormatKHR)*SurfaceFormatCount, alignof(VkSurfaceFormatKHR)));
			VkResult eResult = rhInstanceWrapper.GetPhysicalDeviceSurfaceFormatsKHR(m_hPhysicalDevice, hSurface, &SurfaceFormatCount, pSurfaceFormats);
			assert(eResult == VK_SUCCESS);
		}

		ImageFormat = pSurfaceFormats[0].format;
		ImageColorSpace = pSurfaceFormats[0].colorSpace;

		::PTSMemoryAllocator_Free_Aligned(pSurfaceFormats);
	}
	VkPresentModeKHR SurfacePresentMode;
	{
		uint32_t PresentModeCount;
		{
			VkResult eResult = rhInstanceWrapper.GetPhysicalDeviceSurfacePresentModesKHR(m_hPhysicalDevice, hSurface, &PresentModeCount, NULL);
			assert(eResult == VK_SUCCESS);
		}

		VkPresentModeKHR *pPresentModes;
		{
			pPresentModes = static_cast<VkPresentModeKHR *>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(VkPresentModeKHR)*PresentModeCount, alignof(VkPresentModeKHR)));
			VkResult eResult = rhInstanceWrapper.GetPhysicalDeviceSurfacePresentModesKHR(m_hPhysicalDevice, hSurface, &PresentModeCount, pPresentModes);
			assert(eResult == VK_SUCCESS);
		}

		//We Use Tripple Buffer Semantic
		if ([PresentModeCount, pPresentModes]()->bool
		{
			for (uint32_t iPresentMode = 0U; iPresentMode < PresentModeCount; ++iPresentMode)
			{
				if (pPresentModes[iPresentMode] == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					return true;
				}
			}
			return false;
		}())
		{
			SurfacePresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
		}
		else
		{
			assert(([PresentModeCount, pPresentModes]()->bool
			{
				for (uint32_t iPresentMode = 0U; iPresentMode < PresentModeCount; ++iPresentMode)
				{
					if (pPresentModes[iPresentMode] == VK_PRESENT_MODE_FIFO_KHR)
					{
						return true;
					}
				}
				return false;
			}()));
			SurfacePresentMode = VK_PRESENT_MODE_FIFO_KHR;
		}

		::PTSMemoryAllocator_Free_Aligned(pPresentModes);
	}

	VkSwapchainCreateInfoKHR __SwapChainCreateInfo;
	__SwapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	__SwapChainCreateInfo.pNext = NULL;
	__SwapChainCreateInfo.flags = 0U;
	__SwapChainCreateInfo.surface = hSurface;
	__SwapChainCreateInfo.minImageCount = MinImageCount;
	__SwapChainCreateInfo.imageFormat = ImageFormat;
	__SwapChainCreateInfo.imageColorSpace = ImageColorSpace;
	__SwapChainCreateInfo.imageExtent.width = ImageWidth;
	__SwapChainCreateInfo.imageExtent.height = ImageHeight;
	__SwapChainCreateInfo.imageArrayLayers = ImageArrayLayers;
	__SwapChainCreateInfo.imageUsage = ImageUsage;
	__SwapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	__SwapChainCreateInfo.queueFamilyIndexCount = 0U;
	__SwapChainCreateInfo.pQueueFamilyIndices = NULL;
	__SwapChainCreateInfo.preTransform = SurfaceCurrentTransform;
	__SwapChainCreateInfo.compositeAlpha = SurfaceCompositeAlpha;
	__SwapChainCreateInfo.presentMode = SurfacePresentMode;
	__SwapChainCreateInfo.clipped = VK_TRUE;
	__SwapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE; //If We Retire Then It Can Not Be VK_NULL_HANDLE //[Validation Layer] "surface has an existing swapchain other than oldSwapchain"

	//ReadBarrier Not Need 
	//Only Write By Current Thread

	VkSwapchainKHR hSwapChainPrevious = pContext3D->m_hSwapChain;
	VkImage *pSwapChainImagePrevious = pContext3D->m_pSwapChain_Image;
	VkImageView *pSwapChainImageViewsPrevious = pContext3D->m_pSwapChain_ImageView;
	uint32_t SwapChainImageCountPrevious = pContext3D->m_SwapChain_ImageCount;

	//Lock-Begin
	while (::PTSAtomic_CompareAndSet(&pContext3D->m_SwapChain_bLocked, 0U, 1U) != 0U)
	{
		//Write-HighPriority
		::PTS_Pause();
	}

	//Release The Old SwapChain
	//We Don't Retire Because It Will Crash When We Test
	if (pSwapChainImageViewsPrevious != NULL)
	{
		for (uint32_t iImage = 0U; iImage < SwapChainImageCountPrevious; ++iImage)
		{
			m_hDeviceWrapper.DestroyImageView(pSwapChainImageViewsPrevious[iImage], NULL);
		}
		::PTSMemoryAllocator_Free_Aligned(pSwapChainImageViewsPrevious);
	}
	if (pSwapChainImagePrevious != NULL)
	{
		::PTSMemoryAllocator_Free_Aligned(pSwapChainImagePrevious);
	}
	if (hSwapChainPrevious != VK_NULL_HANDLE)
	{
		m_hDeviceWrapper.DestroySwapchainKHR(hSwapChainPrevious, NULL);
	}

	VkSwapchainKHR hSwapChainNew;
	{
		VkResult eResult = m_hDeviceWrapper.CreateSwapchainKHR(&__SwapChainCreateInfo, NULL, &hSwapChainNew);
		assert(eResult == VK_SUCCESS);
	}
	uint32_t SwapChainImageCountNew;
	{
		SwapChainImageCountNew = 0U;
		VkResult eResult = m_hDeviceWrapper.GetSwapchainImagesKHR(hSwapChainNew, &SwapChainImageCountNew, NULL);
		assert(eResult == VK_SUCCESS);
	}
	VkImage *pSwapChainImagesNew;
	{
		pSwapChainImagesNew = static_cast<VkImage *>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(VkImage)*SwapChainImageCountNew, alignof(VkImage)));
		VkResult eResult = m_hDeviceWrapper.GetSwapchainImagesKHR(hSwapChainNew, &SwapChainImageCountNew, pSwapChainImagesNew);
		assert(eResult == VK_SUCCESS);
	}
	VkImageView *pSwapChainImageViewsNew;
	{
		pSwapChainImageViewsNew = static_cast<VkImageView *>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(VkImageView)*SwapChainImageCountNew, alignof(VkImageView)));
		for (uint32_t iImage = 0U; iImage < SwapChainImageCountNew; ++iImage)
		{
			VkImageViewCreateInfo CreateInfo;
			CreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			CreateInfo.pNext = NULL;
			CreateInfo.flags = 0U;
			CreateInfo.image = pSwapChainImagesNew[iImage];
			CreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			CreateInfo.format = ImageFormat;
			CreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
			CreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
			CreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
			CreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
			CreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			CreateInfo.subresourceRange.baseMipLevel = 0U;
			CreateInfo.subresourceRange.levelCount = 1U;
			CreateInfo.subresourceRange.baseArrayLayer = 0U;
			CreateInfo.subresourceRange.layerCount = ImageArrayLayers;
			VkResult eResult = m_hDeviceWrapper.CreateImageView(&CreateInfo, NULL, pSwapChainImageViewsNew + iImage);
			assert(eResult == VK_SUCCESS);
		}
	}

	pContext3D->m_hSwapChain = hSwapChainNew;
	pContext3D->m_pSwapChain_Image = pSwapChainImagesNew;
	pContext3D->m_pSwapChain_ImageView = pSwapChainImageViewsNew;
	pContext3D->m_SwapChain_ImageCount = SwapChainImageCountNew;
	++pContext3D->m_SwapChain_TimeStamp;

	//To Ensure That We Acquire The Previous TimeStamp Before We Release The Lock
	//We Need The Both Acquire And Release Semantic
	//The ++ Opearation Above Need Acquire Semantic
	//But The PTSAtomic_Set Below Only Has Release Semantic
	::PTS_AcquireReleaseBarrier();

	//Lock-End
	::PTSAtomic_Set(&pContext3D->m_SwapChain_bLocked, 0U);
}

void PTVDDevice_Base::SwapChain_Destroy(PTVD_Context3D *pContext3D)
{
	//ReadBarrier Not Need 
	//Only Write By Current Thread

	VkSwapchainKHR hSwapChainPrevious = pContext3D->m_hSwapChain;
	VkImage *pSwapChainImagePrevious = pContext3D->m_pSwapChain_Image;
	VkImageView *pSwapChainImageViewsPrevious = pContext3D->m_pSwapChain_ImageView;
	uint32_t pSwapChainImageCountPrevious = pContext3D->m_SwapChain_ImageCount;

	//Lock-Begin
	while (::PTSAtomic_CompareAndSet(&pContext3D->m_SwapChain_bLocked, 0U, 1U) != 0U)
	{
		//Write-HighPriority
		::PTS_Pause();
	}

	//Release The Previous SwapChain
	if (pSwapChainImageViewsPrevious != NULL)
	{
		for (uint32_t iImage = 0U; iImage < pSwapChainImageCountPrevious; ++iImage)
		{
			m_hDeviceWrapper.DestroyImageView(pSwapChainImageViewsPrevious[iImage], NULL);
		}
		::PTSMemoryAllocator_Free_Aligned(pSwapChainImageViewsPrevious);
	}
	if (pSwapChainImagePrevious != NULL)
	{
		::PTSMemoryAllocator_Free_Aligned(pSwapChainImagePrevious);
	}
	if (hSwapChainPrevious != VK_NULL_HANDLE)
	{
		m_hDeviceWrapper.DestroySwapchainKHR(hSwapChainPrevious, NULL);
	}

	pContext3D->m_hSwapChain = VK_NULL_HANDLE;
	pContext3D->m_pSwapChain_Image = NULL;
	pContext3D->m_pSwapChain_ImageView = NULL;
	pContext3D->m_SwapChain_ImageCount = 0U;

	//WriteBarrier Not Need
	//There Exists Approach WriteBarrier In the PTSAtomic_Set(m_SwapChain_bLocked) Below

	//Lock-End
	::PTSAtomic_Set(&pContext3D->m_SwapChain_bLocked, 0U);
}


void PTVDDevice_Base::SwapChain_Present/*Render*/(PTVD_Context3D *pContext3D)
{

	//Index ---------------------------------------
	++pContext3D->m_FrameIndex;
	pContext3D->m_FrameIndex %= s_3DContext_FrameCount;

	//For Current Frame
	uint32_t FrameIndex = pContext3D->m_FrameIndex;

	VkCommandPool hCommandAllocator3D = pContext3D->m_hCommandAllocator3D[FrameIndex];
	VkCommandBuffer hCommandList3D = pContext3D->m_hCommandList3D[FrameIndex];

	VkFence hFence_Allow_ResetCommandList = pContext3D->m_hFence_Allow_ResetCommandList[FrameIndex];
	VkSemaphore hSemaphore_Allow_WriteToImmediateBuffer; //For Example //The Constant Buffer //The G-Buffer //The PostProcessBuffer
	VkSemaphore hSemaphore_Allow_ReadFromFinalImage; //Need Three
	VkSemaphore hSemaphore_Allow_WriteToFrameBuffer = pContext3D->m_hSemaphore_Allow_WriteToFrameBuffer[FrameIndex];
	VkSemaphore hSemaphore_Allow_ReadFromFrameBuffer = pContext3D->m_hSemaphore_Allow_ReadFromFrameBuffer[FrameIndex];

	//↓The Rendering Process

	//CommandList Reset And Record //Wait hFence_Allow_ResetCommandList //Fence会与CPU同步

	//Queue Submit CommandList (Record Render Command) //Wait hSemaphore_Allow_WriteToImmediateBuffer 
	                                                   //Signal hSemaphore_Allow_WriteToImmediateBuffer[FrameIndex+1]
	                                                   //Signal hSemaphore_Allow_ReadFromFinalImage[

	//忽略Present失败？？？ 小概率事件？？？ //通过循环尝试

	//Queue Acquire //Means The Queue Present Last Frame //Allow hSemaphore_Allow_WriteToFrameBuffer //Semaphore只在GPU内部同步并不与CPU同步，比Fence高效

	//由于Vulkan中交换链处理存在问题

	//Queue Submit CommandList //Wait hSemaphore_Allow_WriteToFrameBuffer //Allow hSemaphore_Allow_ReadFromFrameBuffer //Allow hFence_Allow_ResetCommandList

	//Queue Present //Wait hSemaphore_Allow_ReadFromFrameBuffer //We Need Three Semaphore //Because We Acquire Different Image From The SwapChain Using The Same API


	//Fence ---------------------------------------
	//Wait For CommandList Complete
	//Then We Can Reset CommandList And CommandPool
	for (VkResult eResult;
		(eResult = m_hDeviceWrapper.GetFenceStatus(hFence_Allow_ResetCommandList)) != VK_SUCCESS;
		assert(eResult == VK_NOT_READY))
	{
		::PTS_Yield();
	}
	//Unsignal
	{
		VkResult eResult = m_hDeviceWrapper.ResetFences(1U, &hFence_Allow_ResetCommandList);
		assert(eResult == VK_SUCCESS);
	}

	//Reset CommandList ---------------------------------------
	m_pCommandQueueManager->CommandAllocator3DReset(m_hDeviceWrapper, hCommandAllocator3D);
	m_pCommandQueueManager->CommandList3DReset(m_hDeviceWrapper, hCommandList3D);

	//We Record Render Command Here
	//..................

	//We Need The BackBufferIndex To Record The Memory Barrier Command
	uint32_t BackBufferIndex;

	for (bool bPresentSuccess = false; !bPresentSuccess; NULL)
	{
		//Lock-Begin
		for (uint32_t uiWaitCount = 0U; ::PTSAtomic_CompareAndSet(&pContext3D->m_SwapChain_bLocked, 0U, 1U) != 0U; ++uiWaitCount)
		{
			//Read-LowPriority
			uint32_t uiWaitMS = uiWaitCount / 100U;
			uiWaitMS = (uiWaitMS < 30U) ? uiWaitMS : 30U;
			(!(uiWaitMS > 0U)) ? ::PTS_Yield() : ::PTS_Sleep(uiWaitMS);
		}


		//ReadBarrier Not Need
		//There Exists Approach ReadBarrier In the PTSAtomic_CompareAndSet(m_SwapChain_bLocked) Above //Really ???

		VkSwapchainKHR hSwapChainCurrent = pContext3D->m_hSwapChain;
		VkImage *SwapChainImagesCurrent = pContext3D->m_pSwapChain_Image;
		VkImageView *pSwapChainImageViewsCurrent = pContext3D->m_pSwapChain_ImageView;
		uint32_t SwapChainImageCountCurrent = pContext3D->m_SwapChain_ImageCount;
		
		//ReadBarrier Not Need
		//Only Read By Current Thread

		if (hSwapChainCurrent == VK_NULL_HANDLE)
		{
			//Lock-End
			::PTSAtomic_Set(&pContext3D->m_SwapChain_bLocked, 0U);

			//Wait
			for (uint32_t iWaitCount = 1U; ::PTSAtomic_Get(&pContext3D->m_hSwapChain) == VK_NULL_HANDLE; ++iWaitCount)
			{
				::PTS_Sleep((iWaitCount < 30U) ? iWaitCount : 30U);
			}

			//Try Again
			continue;
		}

		//Semaphore ---------------------------------------
		{
			VkResult eResult = m_hDeviceWrapper.AcquireNextImageKHR(hSwapChainCurrent, UINT64_MAX, hSemaphore_Allow_WriteToFrameBuffer, VK_NULL_HANDLE, &BackBufferIndex);
			if (eResult != VK_SUCCESS)
			{
				assert(eResult == VK_SUBOPTIMAL_KHR || eResult == VK_ERROR_OUT_OF_DATE_KHR);

				//We Acquire The Previous TimeStamp Before We Release The Lock
				uint32_t SwapChainTimeStampPrevious = ::PTSAtomic_Get(&pContext3D->m_SwapChain_TimeStamp);

				//To Ensure That We Acquire The Previous TimeStamp Before We Release The Lock
				//We Need The Both Acquire And Release Semantic
				//The PTSAtomic_Get Above Only Has Acquire Semantic
				//And The PTSAtomic_Set Below Only Has Release Semantic
				::PTS_AcquireReleaseBarrier();

				//Lock-End
				::PTSAtomic_Set(&pContext3D->m_SwapChain_bLocked, 0U);

				//Wait
				for (uint32_t uiWaitCount = 1U; ::PTSAtomic_Get(&pContext3D->m_SwapChain_TimeStamp) == SwapChainTimeStampPrevious; ++uiWaitCount)
				{
					::PTS_Sleep((uiWaitCount < 30U) ? uiWaitCount : 30U);
				}

				//Try Again
				continue;
			}
		}

		//Wait For hSemaphore_Signal_Present
		//Then We Can Write To The SwapChian Image
		//Layout Transition ---------------------------------------
		{
			VkImageMemoryBarrier ImageMemoryBarrier[1];
			ImageMemoryBarrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			ImageMemoryBarrier[0].pNext = NULL;
			ImageMemoryBarrier[0].srcAccessMask = 0U;
			ImageMemoryBarrier[0].dstAccessMask = 0U;
			ImageMemoryBarrier[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;//ID3D12GraphicsCommandList::DiscardResource
			ImageMemoryBarrier[0].newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			ImageMemoryBarrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			ImageMemoryBarrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			ImageMemoryBarrier[0].image = pContext3D->m_pSwapChain_Image[uiIndexImageCurrent];
			ImageMemoryBarrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			ImageMemoryBarrier[0].subresourceRange.baseMipLevel = 0U;
			ImageMemoryBarrier[0].subresourceRange.levelCount = 1U;
			ImageMemoryBarrier[0].subresourceRange.baseArrayLayer = 0U;
			ImageMemoryBarrier[0].subresourceRange.layerCount = 1U;

			m_hDeviceWrapper.CmdPipelineBarrier(
				hCommandList3D,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT/* PipelineStage_ColorAttachmentOutput*/,
				0U,
				0U, NULL,
				0U, NULL,
				1U, ImageMemoryBarrier);
		}

		m_pCommandQueueManager->CommandList3DClose(m_hDeviceWrapper, hCommandList3D);

		//Pipeline Stages //Vulkan规范
		//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//When defining a memory dependency,
		//using only VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT or VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
		//would never make any accesses available and/or visible
		//because these stages do not access memory.
		//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT and VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT are useful 
		//for accomplishing "Layout Transitions" and "Queue Ownership Operations"
		//when the required "Execution Dependency" is satisfied 
		//by "Other Means" - for example, "Semaphore" operations between queues. //Here Is The Semaphore hSemaphore_Signal_Present

		//The Execution Dependency Chain  
		
		//QueuePresent -Semaphore-> QueueSubmit(Stage_Top/NoMemoryAccess) -MemoryBarrier-> (ColorAttachmentOutput)

		VkPipelineStageFlags WaitDstStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		m_pCommandQueueManager->Queue3DExecuteCommandList(
			m_hDeviceWrapper,
			1U, &hCommandList3D,
			1U, &hSemaphore_Signal_Present, &WaitDstStageMask,
			1U, &hSemaphore_Signal_LightPassAndPostProcessPass,
			hFence_Signal_LightPassAndPostProcessPass);

		//Wait For hSemaphore_Signal_LightPassAndPostProcessPass
		//Then We Can Write To The SwapChian Image
		//Present ---------------------------------------
		{
			VkResult eResult = m_pCommandQueueManager->Queue3DPresent(
				m_hDeviceWrapper,
				hSwapChainCurrent, uiIndexImageCurrent,
				1U,&pContext3D->m_hSemaphore_Signal_LightPassAndPostProcessPass[uiCommand3D_FrameIndex]
			);

			if (eResult != VK_SUCCESS)
			{
				assert(eResult == VK_SUBOPTIMAL_KHR || eResult == VK_ERROR_OUT_OF_DATE_KHR);

				//We Acquire The Previous TimeStamp Before We Release The Lock
				uint32_t SwapChainTimeStampPrevious = ::PTSAtomic_Get(&pContext3D->m_SwapChain_TimeStamp);

				//To Ensure That We Acquire The Previous TimeStamp Before We Release The Lock
				//We Need The Both Acquire And Release Semantic
				//The PTSAtomic_Get Above Only Has Acquire Semantic
				//And The PTSAtomic_Set Below Only Has Release Semantic
				::PTS_AcquireReleaseBarrier();

				//Lock-End
				::PTSAtomic_Set(&pContext3D->m_SwapChain_bLocked, 0U);

				//Wait For The UI Thread Update The SwapChain 
				for (uint32_t iWaitCount = 1U; ::PTSAtomic_Get(&pContext3D->m_SwapChain_TimeStamp) == SwapChainTimeStampPrevious; ++iWaitCount)
				{
					::PTS_Sleep((iWaitCount < 30U) ? iWaitCount : 30U);
				}

				//Try Again
				continue;
			}
		}
		
		bPresentSuccess = true;

		//Lock-End
		::PTSAtomic_Set(&pContext3D->m_SwapChain_bLocked, 0U);	
	}
}