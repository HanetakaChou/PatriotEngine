#include "PTVDQueue.h"

#include "../../../Public/McRT/PTSThread.h"

#include <assert.h>

#ifndef NDEBUG
VkBool32 PTVD_QueueManager_3D0_Copy0::Queue3DPresentValidate(
	VkInstanceWrapper &rhInstanceWrapper, VkPhysicalDevice hPhysicalDevice,
	VkSurfaceKHR hSurface
)
{
	VkBool32 Supported;

	VkResult eResult = rhInstanceWrapper.GetPhysicalDeviceSurfaceSupportKHR(hPhysicalDevice, m_Queue3D_FamilyIndex, hSurface, &Supported);
	assert(eResult == VK_SUCCESS);
	
	return Supported;
}
#endif

VkResult PTVD_QueueManager_3D0_Copy0::Queue3DPresent(
	VkDeviceWrapper &rhDeviceWrapper,
	VkSwapchainKHR hSwapchain, uint32_t ImageIndex,
	uint32_t WaitSemaphoreCount, VkSemaphore *phWaitSemaphores
)
{
	VkPresentInfoKHR PresentInfo;
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	PresentInfo.pNext = NULL;
	//----------------------------------------------------------------
	//Vulkan不保证同一Queue中提交顺序的依赖 //显然应当等待PostProcess（后处理）
	PresentInfo.waitSemaphoreCount = WaitSemaphoreCount;
	PresentInfo.pWaitSemaphores = phWaitSemaphores;
	//----------------------------------------------------------------
	PresentInfo.swapchainCount = 1U;
	PresentInfo.pSwapchains = &hSwapchain;
	PresentInfo.pImageIndices = &ImageIndex;
	//----------------------------------------------------------------
	VkResult eResultIndividual;
	PresentInfo.pResults = &eResultIndividual;
	
#ifndef NDEBUG
	//Lock-Begin
	assert(m_Queue3D0_bLocked == false);
	m_Queue3D0_bLocked = true;
#endif

	VkResult eResult = rhDeviceWrapper.QueuePresentKHR(m_hQueue3D0, &PresentInfo);
	
#ifndef NDEBUG
	//Lock-End
	assert(m_Queue3D0_bLocked == true);
	m_Queue3D0_bLocked = false;
#endif

	assert(eResultIndividual == eResult);
	return eResultIndividual;
}

VkCommandPool PTVD_QueueManager_3D0_Copy0::CommandAllocator3DCreate(VkDeviceWrapper &rhDeviceWrapper)
{
	VkCommandPool hCommandPool = VK_NULL_HANDLE;

	VkCommandPoolCreateInfo CreateInfo;
	CreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	CreateInfo.pNext = NULL;
	//vkResetCommandBuffer: Not Required
	//vkResetCommandPool: All CommandBuffer -> Initial State
	CreateInfo.flags = 0U; //VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	CreateInfo.queueFamilyIndex = m_Queue3D_FamilyIndex;

	VkResult eResult = rhDeviceWrapper.CreateCommandPool(&CreateInfo, NULL, &hCommandPool);
	assert(eResult == VK_SUCCESS);
	return hCommandPool;
}

void PTVD_QueueManager_3D0_Copy0::CommandList3D_RecordImageUploadAcquire(VkDeviceWrapper &rhDeviceWrapper, VkCommandBuffer hCommandListCopy, VkImage hImage, VkImageSubresourceRange const &rSubresourceRange)
{
	VkImageMemoryBarrier ImageMemoryBarrier[1];
	ImageMemoryBarrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	ImageMemoryBarrier[0].pNext = NULL;
	ImageMemoryBarrier[0].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	ImageMemoryBarrier[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	ImageMemoryBarrier[0].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	ImageMemoryBarrier[0].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	ImageMemoryBarrier[0].srcQueueFamilyIndex = m_QueueCopy_FamilyIndex;
	ImageMemoryBarrier[0].dstQueueFamilyIndex = m_Queue3D_FamilyIndex;
	ImageMemoryBarrier[0].image = hImage;
	ImageMemoryBarrier[0].subresourceRange = rSubresourceRange;

	rhDeviceWrapper.CmdPipelineBarrier(hCommandListCopy,
		0U, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT| VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		0U,
		0U, NULL,
		0U, NULL,
		1U, ImageMemoryBarrier
	);
}

void PTVD_QueueManager_3D0_Copy0::Queue3DExecuteCommandList(
	VkDeviceWrapper &rhDeviceWrapper,
	uint32_t Submit0_CommandList3DCount, const VkCommandBuffer *Submit0_phCommandList3D,
	uint32_t Submit0_WaitSemaphoreCount, VkSemaphore *Submit0_phWaitSemaphore, VkPipelineStageFlags *Submit0_pWaitDstStageMask,
	uint32_t Submit0_SignalSemaphoreCount, VkSemaphore *Submit0_phSignalSemaphore,
	VkFence hSignalFence
) 
{
	VkSubmitInfo Submit[1];

	Submit[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Submit[0].pNext = NULL;
	Submit[0].waitSemaphoreCount = Submit0_WaitSemaphoreCount;
	Submit[0].pWaitSemaphores = Submit0_phWaitSemaphore;
	Submit[0].pWaitDstStageMask = Submit0_pWaitDstStageMask;
	Submit[0].commandBufferCount = Submit0_CommandList3DCount;
	Submit[0].pCommandBuffers = Submit0_phCommandList3D;
	Submit[0].signalSemaphoreCount = Submit0_SignalSemaphoreCount;
	Submit[0].pSignalSemaphores = Submit0_phSignalSemaphore;

#ifndef NDEBUG
	//Lock-Begin
	assert(m_Queue3D0_bLocked == false);
	m_Queue3D0_bLocked = true;
#endif

	VkResult eResult = rhDeviceWrapper.QueueSubmit(m_hQueue3D0, 1U, Submit, hSignalFence);
	assert(eResult == VK_SUCCESS);

#ifndef NDEBUG
	//Lock-End
	assert(m_Queue3D0_bLocked == true);
	m_Queue3D0_bLocked = false;
#endif

}

void PTVD_QueueManager_3D0_Copy0::Queue3DExecuteCommandList(
	VkDeviceWrapper &rhDeviceWrapper,
	uint32_t Submit0_CommandList3DCount, const VkCommandBuffer *Submit0_phCommandList3D,
	uint32_t Submit0_WaitSemaphoreCount, VkSemaphore *Submit0_phWaitSemaphore, VkPipelineStageFlags *Submit0_pWaitDstStageMask,
	uint32_t Submit0_SignalSemaphoreCount, VkSemaphore *Submit0_phSignalSemaphore,
	uint32_t Submit1_CommandList3DCount, const VkCommandBuffer *Submit1_phCommandList3D,
	uint32_t Submit1_WaitSemaphoreCount, VkSemaphore *Submit1_phWaitSemaphore, VkPipelineStageFlags *Submit1_pWaitDstStageMask,
	uint32_t Submit1_SignalSemaphoreCount, VkSemaphore *Submit1_phSignalSemaphore,
	VkFence hSignalFence
)
{
	VkSubmitInfo Submit[2];

	Submit[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Submit[0].pNext = NULL;
	Submit[0].waitSemaphoreCount = Submit0_WaitSemaphoreCount;
	Submit[0].pWaitSemaphores = Submit0_phWaitSemaphore;
	Submit[0].pWaitDstStageMask = Submit0_pWaitDstStageMask;
	Submit[0].commandBufferCount = Submit0_CommandList3DCount;
	Submit[0].pCommandBuffers = Submit0_phCommandList3D;
	Submit[0].signalSemaphoreCount = Submit0_SignalSemaphoreCount;
	Submit[0].pSignalSemaphores = Submit0_phSignalSemaphore;

	Submit[1].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Submit[1].pNext = NULL;
	Submit[1].waitSemaphoreCount = Submit1_WaitSemaphoreCount;
	Submit[1].pWaitSemaphores = Submit1_phWaitSemaphore;
	Submit[1].pWaitDstStageMask = Submit1_pWaitDstStageMask;
	Submit[1].commandBufferCount = Submit1_CommandList3DCount;
	Submit[1].pCommandBuffers = Submit1_phCommandList3D;
	Submit[1].signalSemaphoreCount = Submit1_SignalSemaphoreCount;
	Submit[1].pSignalSemaphores = Submit1_phSignalSemaphore;

#ifndef NDEBUG
	//Lock-Begin
	assert(m_Queue3D0_bLocked == false);
	m_Queue3D0_bLocked = true;
#endif

	VkResult eResult = rhDeviceWrapper.QueueSubmit(m_hQueue3D0, 2U, Submit, hSignalFence);
	assert(eResult == VK_SUCCESS);

#ifndef NDEBUG
	//Lock-End
	assert(m_Queue3D0_bLocked == true);
	m_Queue3D0_bLocked = false;
#endif

}

void PTVD_QueueManager_3D0_Copy0::Queue3DExecuteCommandList(
	VkDeviceWrapper &rhDeviceWrapper,
	uint32_t Submit0_CommandList3DCount, const VkCommandBuffer *Submit0_phCommandList3D,
	uint32_t Submit0_WaitSemaphoreCount, VkSemaphore *Submit0_phWaitSemaphore, VkPipelineStageFlags *Submit0_pWaitDstStageMask,
	uint32_t Submit0_SignalSemaphoreCount, VkSemaphore *Submit0_phSignalSemaphore,
	uint32_t Submit1_CommandList3DCount, const VkCommandBuffer *Submit1_phCommandList3D,
	uint32_t Submit1_WaitSemaphoreCount, VkSemaphore *Submit1_phWaitSemaphore, VkPipelineStageFlags *Submit1_pWaitDstStageMask,
	uint32_t Submit1_SignalSemaphoreCount, VkSemaphore *Submit1_phSignalSemaphore,
	uint32_t Submit2_CommandList3DCount, const VkCommandBuffer *Submit2_phCommandList3D,
	uint32_t Submit2_WaitSemaphoreCount, VkSemaphore *Submit2_phWaitSemaphore, VkPipelineStageFlags *Submit2_pWaitDstStageMask,
	uint32_t Submit2_SignalSemaphoreCount, VkSemaphore *Submit2_phSignalSemaphore,
	VkFence hSignalFence
)
{
	VkSubmitInfo Submit[3];

	Submit[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Submit[0].pNext = NULL;
	Submit[0].waitSemaphoreCount = Submit0_WaitSemaphoreCount;
	Submit[0].pWaitSemaphores = Submit0_phWaitSemaphore;
	Submit[0].pWaitDstStageMask = Submit0_pWaitDstStageMask;
	Submit[0].commandBufferCount = Submit0_CommandList3DCount;
	Submit[0].pCommandBuffers = Submit0_phCommandList3D;
	Submit[0].signalSemaphoreCount = Submit0_SignalSemaphoreCount;
	Submit[0].pSignalSemaphores = Submit0_phSignalSemaphore;

	Submit[1].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Submit[1].pNext = NULL;
	Submit[1].waitSemaphoreCount = Submit1_WaitSemaphoreCount;
	Submit[1].pWaitSemaphores = Submit1_phWaitSemaphore;
	Submit[1].pWaitDstStageMask = Submit1_pWaitDstStageMask;
	Submit[1].commandBufferCount = Submit1_CommandList3DCount;
	Submit[1].pCommandBuffers = Submit1_phCommandList3D;
	Submit[1].signalSemaphoreCount = Submit1_SignalSemaphoreCount;
	Submit[1].pSignalSemaphores = Submit1_phSignalSemaphore;

	Submit[2].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Submit[2].pNext = NULL;
	Submit[2].waitSemaphoreCount = Submit2_WaitSemaphoreCount;
	Submit[2].pWaitSemaphores = Submit2_phWaitSemaphore;
	Submit[2].pWaitDstStageMask = Submit2_pWaitDstStageMask;
	Submit[2].commandBufferCount = Submit2_CommandList3DCount;
	Submit[2].pCommandBuffers = Submit2_phCommandList3D;
	Submit[2].signalSemaphoreCount = Submit2_SignalSemaphoreCount;
	Submit[2].pSignalSemaphores = Submit2_phSignalSemaphore;

#ifndef NDEBUG
	//Lock-Begin
	assert(m_Queue3D0_bLocked == false);
	m_Queue3D0_bLocked = true;
#endif

	VkResult eResult = rhDeviceWrapper.QueueSubmit(m_hQueue3D0, 3U, Submit, hSignalFence);
	assert(eResult == VK_SUCCESS);

#ifndef NDEBUG
	//Lock-End
	assert(m_Queue3D0_bLocked == true);
	m_Queue3D0_bLocked = false;
#endif

}

VkCommandPool PTVD_QueueManager_3D0_Copy0::CommandAllocatorCopyCreate(VkDeviceWrapper &rhDeviceWrapper)
{
	VkCommandPool hCommandPool = VK_NULL_HANDLE;

	VkCommandPoolCreateInfo CreateInfo;
	CreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	CreateInfo.pNext = NULL;
	//vkResetCommandBuffer: Not Required
	//vkResetCommandPool: All CommandBuffer -> Initial State
	CreateInfo.flags = 0U;
	CreateInfo.queueFamilyIndex = m_QueueCopy_FamilyIndex;

	VkResult eResult = rhDeviceWrapper.CreateCommandPool(&CreateInfo, NULL, &hCommandPool);
	assert(eResult == VK_SUCCESS);
	return hCommandPool;
}

void PTVD_QueueManager_3D0_Copy0::BufferCreate_PopulateShareMode(VkSharingMode *pSharingMode, uint32_t *pQueueFamilyIndexCount, const uint32_t **ppQueueFamilyIndices)
{
	(*pSharingMode) = VK_SHARING_MODE_CONCURRENT;
	(*pQueueFamilyIndexCount) = 2U;
	(*ppQueueFamilyIndices) = &m_Queue3D_FamilyIndex;
	assert((*ppQueueFamilyIndices)[1] == m_QueueCopy_FamilyIndex);
}

void PTVD_QueueManager_3D0_Copy0::CommandListCopy_RecordImageUploadRelease(VkDeviceWrapper &rhDeviceWrapper, VkCommandBuffer hCommandListCopy, VkImage hImage, VkImageSubresourceRange const &rSubresourceRange)
{
	VkImageMemoryBarrier ImageMemoryBarrier[1];
	ImageMemoryBarrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	ImageMemoryBarrier[0].pNext = NULL;
	ImageMemoryBarrier[0].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	ImageMemoryBarrier[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	ImageMemoryBarrier[0].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	ImageMemoryBarrier[0].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	ImageMemoryBarrier[0].srcQueueFamilyIndex = m_QueueCopy_FamilyIndex;
	ImageMemoryBarrier[0].dstQueueFamilyIndex = m_Queue3D_FamilyIndex;
	ImageMemoryBarrier[0].image = hImage;
	ImageMemoryBarrier[0].subresourceRange = rSubresourceRange;

	rhDeviceWrapper.CmdPipelineBarrier(hCommandListCopy,
		VK_PIPELINE_STAGE_TRANSFER_BIT, 0U,
		0U,
		0U, NULL,
		0U, NULL,
		1U, ImageMemoryBarrier
	);

}

void PTVD_QueueManager_3D0_Copy0::CommandQueueCopy_CommandListsExecute(
	VkDeviceWrapper &rhDeviceWrapper,
	uint32_t CommandListCopyCount, const VkCommandBuffer *pCommandListCopys,
	VkFence hSignalFence
)
{
	VkSubmitInfo Submit[1];
	Submit[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Submit[0].pNext = NULL;
	Submit[0].waitSemaphoreCount = 0U;
	Submit[0].pWaitSemaphores = NULL;
	Submit[0].pWaitDstStageMask = NULL;
	Submit[0].commandBufferCount = CommandListCopyCount;
	Submit[0].pCommandBuffers = pCommandListCopys;
	Submit[0].signalSemaphoreCount = 0U;
	Submit[0].pSignalSemaphores = NULL;

	VkResult eResult = rhDeviceWrapper.QueueSubmit(m_hQueueCopy0, 1U, Submit, hSignalFence);
	assert(eResult == VK_SUCCESS);
}

#ifndef NDEBUG
VkBool32 PTVD_QueueManager_3D0_Compute0::Queue3DPresentValidate(
	VkInstanceWrapper &rhInstanceWrapper, VkPhysicalDevice hPhysicalDevice,
	VkSurfaceKHR hSurface
)
{
	VkBool32 Supported;

	VkResult eResult = rhInstanceWrapper.GetPhysicalDeviceSurfaceSupportKHR(hPhysicalDevice, m_Queue3D_FamilyIndex, hSurface, &Supported);
	assert(eResult == VK_SUCCESS);

	return Supported;
}
#endif

VkCommandPool PTVD_QueueManager_3D0_Compute0::CommandAllocator3DCreate(VkDeviceWrapper &rhDeviceWrapper)
{
	VkCommandPool hCommandPool = VK_NULL_HANDLE;

	VkCommandPoolCreateInfo CreateInfo;
	CreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	CreateInfo.pNext = NULL;
	//vkResetCommandBuffer: Not Required
	//vkResetCommandPool: All CommandBuffer -> Initial State
	CreateInfo.flags = 0U; //VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	CreateInfo.queueFamilyIndex = m_Queue3D_FamilyIndex;

	VkResult eResult = rhDeviceWrapper.CreateCommandPool(&CreateInfo, NULL, &hCommandPool);
	assert(eResult == VK_SUCCESS);
	return hCommandPool;
}

void PTVD_QueueManager_3D0_Compute0::Queue3DExecuteCommandList(
	VkDeviceWrapper &rhDeviceWrapper,
	uint32_t Submit0_CommandList3DCount, const VkCommandBuffer *Submit0_phCommandList3D,
	uint32_t Submit0_WaitSemaphoreCount, VkSemaphore *Submit0_phWaitSemaphore, VkPipelineStageFlags *Submit0_pWaitDstStageMask,
	uint32_t Submit0_SignalSemaphoreCount, VkSemaphore *Submit0_phSignalSemaphore,
	VkFence hSignalFence
)
{
	VkSubmitInfo Submit[1];

	Submit[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Submit[0].pNext = NULL;
	Submit[0].waitSemaphoreCount = Submit0_WaitSemaphoreCount;
	Submit[0].pWaitSemaphores = Submit0_phWaitSemaphore;
	Submit[0].pWaitDstStageMask = Submit0_pWaitDstStageMask;
	Submit[0].commandBufferCount = Submit0_CommandList3DCount;
	Submit[0].pCommandBuffers = Submit0_phCommandList3D;
	Submit[0].signalSemaphoreCount = Submit0_SignalSemaphoreCount;
	Submit[0].pSignalSemaphores = Submit0_phSignalSemaphore;

#ifndef NDEBUG
	//Lock-Begin
	assert(m_Queue3D0_bLocked == false);
	m_Queue3D0_bLocked = true;
#endif

	VkResult eResult = rhDeviceWrapper.QueueSubmit(m_hQueue3D0, 1U, Submit, hSignalFence);
	assert(eResult == VK_SUCCESS);

#ifndef NDEBUG
	//Lock-End
	assert(m_Queue3D0_bLocked == true);
	m_Queue3D0_bLocked = false;
#endif

}

void PTVD_QueueManager_3D0_Compute0::Queue3DExecuteCommandList(
	VkDeviceWrapper &rhDeviceWrapper,
	uint32_t Submit0_CommandList3DCount, const VkCommandBuffer *Submit0_phCommandList3D,
	uint32_t Submit0_WaitSemaphoreCount, VkSemaphore *Submit0_phWaitSemaphore, VkPipelineStageFlags *Submit0_pWaitDstStageMask,
	uint32_t Submit0_SignalSemaphoreCount, VkSemaphore *Submit0_phSignalSemaphore,
	uint32_t Submit1_CommandList3DCount, const VkCommandBuffer *Submit1_phCommandList3D,
	uint32_t Submit1_WaitSemaphoreCount, VkSemaphore *Submit1_phWaitSemaphore, VkPipelineStageFlags *Submit1_pWaitDstStageMask,
	uint32_t Submit1_SignalSemaphoreCount, VkSemaphore *Submit1_phSignalSemaphore,
	VkFence hSignalFence
)
{
	VkSubmitInfo Submit[2];

	Submit[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Submit[0].pNext = NULL;
	Submit[0].waitSemaphoreCount = Submit0_WaitSemaphoreCount;
	Submit[0].pWaitSemaphores = Submit0_phWaitSemaphore;
	Submit[0].pWaitDstStageMask = Submit0_pWaitDstStageMask;
	Submit[0].commandBufferCount = Submit0_CommandList3DCount;
	Submit[0].pCommandBuffers = Submit0_phCommandList3D;
	Submit[0].signalSemaphoreCount = Submit0_SignalSemaphoreCount;
	Submit[0].pSignalSemaphores = Submit0_phSignalSemaphore;

	Submit[1].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Submit[1].pNext = NULL;
	Submit[1].waitSemaphoreCount = Submit1_WaitSemaphoreCount;
	Submit[1].pWaitSemaphores = Submit1_phWaitSemaphore;
	Submit[1].pWaitDstStageMask = Submit1_pWaitDstStageMask;
	Submit[1].commandBufferCount = Submit1_CommandList3DCount;
	Submit[1].pCommandBuffers = Submit1_phCommandList3D;
	Submit[1].signalSemaphoreCount = Submit1_SignalSemaphoreCount;
	Submit[1].pSignalSemaphores = Submit1_phSignalSemaphore;

#ifndef NDEBUG
	//Lock-Begin
	assert(m_Queue3D0_bLocked == false);
	m_Queue3D0_bLocked = true;
#endif

	VkResult eResult = rhDeviceWrapper.QueueSubmit(m_hQueue3D0, 2U, Submit, hSignalFence);
	assert(eResult == VK_SUCCESS);

#ifndef NDEBUG
	//Lock-End
	assert(m_Queue3D0_bLocked == true);
	m_Queue3D0_bLocked = false;
#endif

}

void PTVD_QueueManager_3D0_Compute0::Queue3DExecuteCommandList(
	VkDeviceWrapper &rhDeviceWrapper,
	uint32_t Submit0_CommandList3DCount, const VkCommandBuffer *Submit0_phCommandList3D,
	uint32_t Submit0_WaitSemaphoreCount, VkSemaphore *Submit0_phWaitSemaphore, VkPipelineStageFlags *Submit0_pWaitDstStageMask,
	uint32_t Submit0_SignalSemaphoreCount, VkSemaphore *Submit0_phSignalSemaphore,
	uint32_t Submit1_CommandList3DCount, const VkCommandBuffer *Submit1_phCommandList3D,
	uint32_t Submit1_WaitSemaphoreCount, VkSemaphore *Submit1_phWaitSemaphore, VkPipelineStageFlags *Submit1_pWaitDstStageMask,
	uint32_t Submit1_SignalSemaphoreCount, VkSemaphore *Submit1_phSignalSemaphore,
	uint32_t Submit2_CommandList3DCount, const VkCommandBuffer *Submit2_phCommandList3D,
	uint32_t Submit2_WaitSemaphoreCount, VkSemaphore *Submit2_phWaitSemaphore, VkPipelineStageFlags *Submit2_pWaitDstStageMask,
	uint32_t Submit2_SignalSemaphoreCount, VkSemaphore *Submit2_phSignalSemaphore,
	VkFence hSignalFence
)
{
	VkSubmitInfo Submit[3];

	Submit[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Submit[0].pNext = NULL;
	Submit[0].waitSemaphoreCount = Submit0_WaitSemaphoreCount;
	Submit[0].pWaitSemaphores = Submit0_phWaitSemaphore;
	Submit[0].pWaitDstStageMask = Submit0_pWaitDstStageMask;
	Submit[0].commandBufferCount = Submit0_CommandList3DCount;
	Submit[0].pCommandBuffers = Submit0_phCommandList3D;
	Submit[0].signalSemaphoreCount = Submit0_SignalSemaphoreCount;
	Submit[0].pSignalSemaphores = Submit0_phSignalSemaphore;

	Submit[1].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Submit[1].pNext = NULL;
	Submit[1].waitSemaphoreCount = Submit1_WaitSemaphoreCount;
	Submit[1].pWaitSemaphores = Submit1_phWaitSemaphore;
	Submit[1].pWaitDstStageMask = Submit1_pWaitDstStageMask;
	Submit[1].commandBufferCount = Submit1_CommandList3DCount;
	Submit[1].pCommandBuffers = Submit1_phCommandList3D;
	Submit[1].signalSemaphoreCount = Submit1_SignalSemaphoreCount;
	Submit[1].pSignalSemaphores = Submit1_phSignalSemaphore;

	Submit[2].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Submit[2].pNext = NULL;
	Submit[2].waitSemaphoreCount = Submit2_WaitSemaphoreCount;
	Submit[2].pWaitSemaphores = Submit2_phWaitSemaphore;
	Submit[2].pWaitDstStageMask = Submit2_pWaitDstStageMask;
	Submit[2].commandBufferCount = Submit2_CommandList3DCount;
	Submit[2].pCommandBuffers = Submit2_phCommandList3D;
	Submit[2].signalSemaphoreCount = Submit2_SignalSemaphoreCount;
	Submit[2].pSignalSemaphores = Submit2_phSignalSemaphore;

#ifndef NDEBUG
	//Lock-Begin
	assert(m_Queue3D0_bLocked == false);
	m_Queue3D0_bLocked = true;
#endif

	VkResult eResult = rhDeviceWrapper.QueueSubmit(m_hQueue3D0, 3U, Submit, hSignalFence);
	assert(eResult == VK_SUCCESS);


#ifndef NDEBUG
	//Lock-End
	assert(m_Queue3D0_bLocked == true);
	m_Queue3D0_bLocked = false;
#endif

}

#ifndef NDEBUG
VkBool32 PTVD_QueueManager_3D0_3D1::Queue3DPresentValidate(
	VkInstanceWrapper &rhInstanceWrapper, VkPhysicalDevice hPhysicalDevice,
	VkSurfaceKHR hSurface
)
{
	VkBool32 Supported;

	VkResult eResult = rhInstanceWrapper.GetPhysicalDeviceSurfaceSupportKHR(hPhysicalDevice, m_Queue3D_FamilyIndex, hSurface, &Supported);
	assert(eResult == VK_SUCCESS);

	return Supported;
}
#endif

VkResult PTVD_QueueManager_3D0_3D1::Queue3DPresent(
	VkDeviceWrapper &rhDeviceWrapper,
	VkSwapchainKHR hSwapchain, uint32_t ImageIndex,
	uint32_t WaitSemaphoreCount, VkSemaphore *phWaitSemaphores
)
{
	VkPresentInfoKHR PresentInfo;
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	PresentInfo.pNext = NULL;
	//----------------------------------------------------------------
	//Vulkan不保证同一Queue中提交顺序的依赖 //显然应当等待PostProcess（后处理）
	PresentInfo.waitSemaphoreCount = WaitSemaphoreCount;
	PresentInfo.pWaitSemaphores = phWaitSemaphores;
	//----------------------------------------------------------------
	PresentInfo.swapchainCount = 1U;
	PresentInfo.pSwapchains = &hSwapchain;
	PresentInfo.pImageIndices = &ImageIndex;
	//----------------------------------------------------------------
	VkResult eResultIndividual;
	PresentInfo.pResults = &eResultIndividual;

#ifndef NDEBUG
	//Lock-Begin
	assert(m_Queue3D0_bLocked == false);
	m_Queue3D0_bLocked = true;
#endif

	VkResult eResult = rhDeviceWrapper.QueuePresentKHR(m_hQueue3D0, &PresentInfo);

#ifndef NDEBUG
	//Lock-End
	assert(m_Queue3D0_bLocked == true);
	m_Queue3D0_bLocked = false;
#endif

	assert(eResultIndividual == eResult);
	return eResultIndividual;
}

VkCommandPool PTVD_QueueManager_3D0_3D1::CommandAllocator3DCreate(VkDeviceWrapper &rhDeviceWrapper)
{
	VkCommandPool hCommandPool = VK_NULL_HANDLE;

	VkCommandPoolCreateInfo CreateInfo;
	CreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	CreateInfo.pNext = NULL;
	//vkResetCommandBuffer: Not Required
	//vkResetCommandPool: All CommandBuffer -> Initial State
	CreateInfo.flags = 0U; //VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	CreateInfo.queueFamilyIndex = m_Queue3D_FamilyIndex;

	VkResult eResult = rhDeviceWrapper.CreateCommandPool(&CreateInfo, NULL, &hCommandPool);
	assert(eResult == VK_SUCCESS);
	return hCommandPool;
}

void PTVD_QueueManager_3D0_3D1::Queue3DExecuteCommandList(
	VkDeviceWrapper &rhDeviceWrapper,
	uint32_t Submit0_CommandList3DCount, const VkCommandBuffer *Submit0_phCommandList3D,
	uint32_t Submit0_WaitSemaphoreCount, VkSemaphore *Submit0_phWaitSemaphore, VkPipelineStageFlags *Submit0_pWaitDstStageMask,
	uint32_t Submit0_SignalSemaphoreCount, VkSemaphore *Submit0_phSignalSemaphore,
	VkFence hSignalFence
)
{
	VkSubmitInfo Submit[1];

	Submit[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Submit[0].pNext = NULL;
	Submit[0].waitSemaphoreCount = Submit0_WaitSemaphoreCount;
	Submit[0].pWaitSemaphores = Submit0_phWaitSemaphore;
	Submit[0].pWaitDstStageMask = Submit0_pWaitDstStageMask;
	Submit[0].commandBufferCount = Submit0_CommandList3DCount;
	Submit[0].pCommandBuffers = Submit0_phCommandList3D;
	Submit[0].signalSemaphoreCount = Submit0_SignalSemaphoreCount;
	Submit[0].pSignalSemaphores = Submit0_phSignalSemaphore;

#ifndef NDEBUG
	//Lock-Begin
	assert(m_Queue3D0_bLocked == false);
	m_Queue3D0_bLocked = true;
#endif

	VkResult eResult = rhDeviceWrapper.QueueSubmit(m_hQueue3D0, 1U, Submit, hSignalFence);
	assert(eResult == VK_SUCCESS);

#ifndef NDEBUG
	//Lock-End
	assert(m_Queue3D0_bLocked == true);
	m_Queue3D0_bLocked = false;
#endif

}

void PTVD_QueueManager_3D0_3D1::Queue3DExecuteCommandList(
	VkDeviceWrapper &rhDeviceWrapper,
	uint32_t Submit0_CommandList3DCount, const VkCommandBuffer *Submit0_phCommandList3D,
	uint32_t Submit0_WaitSemaphoreCount, VkSemaphore *Submit0_phWaitSemaphore, VkPipelineStageFlags *Submit0_pWaitDstStageMask,
	uint32_t Submit0_SignalSemaphoreCount, VkSemaphore *Submit0_phSignalSemaphore,
	uint32_t Submit1_CommandList3DCount, const VkCommandBuffer *Submit1_phCommandList3D,
	uint32_t Submit1_WaitSemaphoreCount, VkSemaphore *Submit1_phWaitSemaphore, VkPipelineStageFlags *Submit1_pWaitDstStageMask,
	uint32_t Submit1_SignalSemaphoreCount, VkSemaphore *Submit1_phSignalSemaphore,
	VkFence hSignalFence
)
{
	VkSubmitInfo Submit[2];

	Submit[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Submit[0].pNext = NULL;
	Submit[0].waitSemaphoreCount = Submit0_WaitSemaphoreCount;
	Submit[0].pWaitSemaphores = Submit0_phWaitSemaphore;
	Submit[0].pWaitDstStageMask = Submit0_pWaitDstStageMask;
	Submit[0].commandBufferCount = Submit0_CommandList3DCount;
	Submit[0].pCommandBuffers = Submit0_phCommandList3D;
	Submit[0].signalSemaphoreCount = Submit0_SignalSemaphoreCount;
	Submit[0].pSignalSemaphores = Submit0_phSignalSemaphore;

	Submit[1].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Submit[1].pNext = NULL;
	Submit[1].waitSemaphoreCount = Submit1_WaitSemaphoreCount;
	Submit[1].pWaitSemaphores = Submit1_phWaitSemaphore;
	Submit[1].pWaitDstStageMask = Submit1_pWaitDstStageMask;
	Submit[1].commandBufferCount = Submit1_CommandList3DCount;
	Submit[1].pCommandBuffers = Submit1_phCommandList3D;
	Submit[1].signalSemaphoreCount = Submit1_SignalSemaphoreCount;
	Submit[1].pSignalSemaphores = Submit1_phSignalSemaphore;

#ifndef NDEBUG
	//Lock-Begin
	assert(m_Queue3D0_bLocked == false);
	m_Queue3D0_bLocked = true;
#endif

	VkResult eResult = rhDeviceWrapper.QueueSubmit(m_hQueue3D0, 2U, Submit, hSignalFence);
	assert(eResult == VK_SUCCESS);

#ifndef NDEBUG
	//Lock-End
	assert(m_Queue3D0_bLocked == true);
	m_Queue3D0_bLocked = false;
#endif

}

void PTVD_QueueManager_3D0_3D1::Queue3DExecuteCommandList(
	VkDeviceWrapper &rhDeviceWrapper,
	uint32_t Submit0_CommandList3DCount, const VkCommandBuffer *Submit0_phCommandList3D,
	uint32_t Submit0_WaitSemaphoreCount, VkSemaphore *Submit0_phWaitSemaphore, VkPipelineStageFlags *Submit0_pWaitDstStageMask,
	uint32_t Submit0_SignalSemaphoreCount, VkSemaphore *Submit0_phSignalSemaphore,
	uint32_t Submit1_CommandList3DCount, const VkCommandBuffer *Submit1_phCommandList3D,
	uint32_t Submit1_WaitSemaphoreCount, VkSemaphore *Submit1_phWaitSemaphore, VkPipelineStageFlags *Submit1_pWaitDstStageMask,
	uint32_t Submit1_SignalSemaphoreCount, VkSemaphore *Submit1_phSignalSemaphore,
	uint32_t Submit2_CommandList3DCount, const VkCommandBuffer *Submit2_phCommandList3D,
	uint32_t Submit2_WaitSemaphoreCount, VkSemaphore *Submit2_phWaitSemaphore, VkPipelineStageFlags *Submit2_pWaitDstStageMask,
	uint32_t Submit2_SignalSemaphoreCount, VkSemaphore *Submit2_phSignalSemaphore,
	VkFence hSignalFence
)
{
	VkSubmitInfo Submit[3];

	Submit[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Submit[0].pNext = NULL;
	Submit[0].waitSemaphoreCount = Submit0_WaitSemaphoreCount;
	Submit[0].pWaitSemaphores = Submit0_phWaitSemaphore;
	Submit[0].pWaitDstStageMask = Submit0_pWaitDstStageMask;
	Submit[0].commandBufferCount = Submit0_CommandList3DCount;
	Submit[0].pCommandBuffers = Submit0_phCommandList3D;
	Submit[0].signalSemaphoreCount = Submit0_SignalSemaphoreCount;
	Submit[0].pSignalSemaphores = Submit0_phSignalSemaphore;

	Submit[1].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Submit[1].pNext = NULL;
	Submit[1].waitSemaphoreCount = Submit1_WaitSemaphoreCount;
	Submit[1].pWaitSemaphores = Submit1_phWaitSemaphore;
	Submit[1].pWaitDstStageMask = Submit1_pWaitDstStageMask;
	Submit[1].commandBufferCount = Submit1_CommandList3DCount;
	Submit[1].pCommandBuffers = Submit1_phCommandList3D;
	Submit[1].signalSemaphoreCount = Submit1_SignalSemaphoreCount;
	Submit[1].pSignalSemaphores = Submit1_phSignalSemaphore;

	Submit[2].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Submit[2].pNext = NULL;
	Submit[2].waitSemaphoreCount = Submit2_WaitSemaphoreCount;
	Submit[2].pWaitSemaphores = Submit2_phWaitSemaphore;
	Submit[2].pWaitDstStageMask = Submit2_pWaitDstStageMask;
	Submit[2].commandBufferCount = Submit2_CommandList3DCount;
	Submit[2].pCommandBuffers = Submit2_phCommandList3D;
	Submit[2].signalSemaphoreCount = Submit2_SignalSemaphoreCount;
	Submit[2].pSignalSemaphores = Submit2_phSignalSemaphore;

#ifndef NDEBUG
	//Lock-Begin
	assert(m_Queue3D0_bLocked == false);
	m_Queue3D0_bLocked = true;
#endif

	VkResult eResult = rhDeviceWrapper.QueueSubmit(m_hQueue3D0, 3U, Submit, hSignalFence);
	assert(eResult == VK_SUCCESS);


#ifndef NDEBUG
	//Lock-End
	assert(m_Queue3D0_bLocked == true);
	m_Queue3D0_bLocked = false;
#endif

}

void PTVD_QueueManager_3D0_3D1::BufferCreate_PopulateShareMode(VkSharingMode *pSharingMode, uint32_t *pQueueFamilyIndexCount, const uint32_t **ppQueueFamilyIndices)
{
	(*pSharingMode) = VK_SHARING_MODE_EXCLUSIVE;
	(*pQueueFamilyIndexCount) = 0U;
	(*ppQueueFamilyIndices) = NULL;
}

#ifndef NDEBUG
VkBool32 PTVD_QueueManager_3D0_3D0::Queue3DPresentValidate(
	VkInstanceWrapper &rhInstanceWrapper, VkPhysicalDevice hPhysicalDevice,
	VkSurfaceKHR hSurface
)
{
	VkBool32 Supported;

	VkResult eResult = rhInstanceWrapper.GetPhysicalDeviceSurfaceSupportKHR(hPhysicalDevice, m_Queue3D_FamilyIndex, hSurface, &Supported);
	assert(eResult == VK_SUCCESS);

	return Supported;
}
#endif

VkResult PTVD_QueueManager_3D0_3D0::Queue3DPresent(
	VkDeviceWrapper &rhDeviceWrapper,
	VkSwapchainKHR hSwapchain, uint32_t ImageIndex,
	uint32_t WaitSemaphoreCount, VkSemaphore *phWaitSemaphores
)
{
	VkPresentInfoKHR PresentInfo;
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	PresentInfo.pNext = NULL;
	//----------------------------------------------------------------
	//Vulkan不保证同一Queue中提交顺序的依赖 //显然应当等待PostProcess（后处理）
	PresentInfo.waitSemaphoreCount = WaitSemaphoreCount;
	PresentInfo.pWaitSemaphores = phWaitSemaphores;
	//----------------------------------------------------------------
	PresentInfo.swapchainCount = 1U;
	PresentInfo.pSwapchains = &hSwapchain;
	PresentInfo.pImageIndices = &ImageIndex;
	//----------------------------------------------------------------
	VkResult eResultIndividual;
	PresentInfo.pResults = &eResultIndividual;

	//ReadBarrier Not Need
	//There Exists Approach ReadBarrier In the PTSAtomic_CompareAndSet(m_SwapChain_bLocked) Below //Really ???

	//Lock-Begin
	while (::PTSAtomic_CompareAndSet(&m_Queue3D0_bLocked, 0U, 1U) != 0U)
	{
		//Write-HighPriority
		::PTS_Pause();
	}

	VkResult eResult = rhDeviceWrapper.QueuePresentKHR(m_hQueue3D0, &PresentInfo);

	assert(eResultIndividual == eResult);

	//WriteBarrier Not Need
	//There Exists Approach WriteBarrier In the PTSAtomic_Set(m_SwapChain_bLocked) Below

	//Lock-End
	::PTSAtomic_Set(&m_Queue3D0_bLocked, 0U);

	return eResultIndividual;
}

VkCommandPool PTVD_QueueManager_3D0_3D0::CommandAllocator3DCreate(VkDeviceWrapper &rhDeviceWrapper)
{
	VkCommandPool hCommandPool = VK_NULL_HANDLE;

	VkCommandPoolCreateInfo CreateInfo;
	CreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	CreateInfo.pNext = NULL;
	//vkResetCommandBuffer: Not Required
	//vkResetCommandPool: All CommandBuffer -> Initial State
	CreateInfo.flags = 0U; //VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	CreateInfo.queueFamilyIndex = m_Queue3D_FamilyIndex;

	VkResult eResult = rhDeviceWrapper.CreateCommandPool(&CreateInfo, NULL, &hCommandPool);
	assert(eResult == VK_SUCCESS);
	return hCommandPool;
}

void PTVD_QueueManager_3D0_3D0::BufferCreate_PopulateShareMode(VkSharingMode *pSharingMode, uint32_t *pQueueFamilyIndexCount, const uint32_t **ppQueueFamilyIndices)
{
	(*pSharingMode) = VK_SHARING_MODE_EXCLUSIVE;
	(*pQueueFamilyIndexCount) = 0U;
	(*ppQueueFamilyIndices) = NULL;
}

void PTVD_QueueManager_3D0_3D0::Queue3DExecuteCommandList(
	VkDeviceWrapper &rhDeviceWrapper,
	uint32_t Submit0_CommandList3DCount, const VkCommandBuffer *Submit0_phCommandList3D,
	uint32_t Submit0_WaitSemaphoreCount, VkSemaphore *Submit0_phWaitSemaphore, VkPipelineStageFlags *Submit0_pWaitDstStageMask,
	uint32_t Submit0_SignalSemaphoreCount, VkSemaphore *Submit0_phSignalSemaphore,
	VkFence hSignalFence
)
{
	VkSubmitInfo Submit[1];

	Submit[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Submit[0].pNext = NULL;
	Submit[0].waitSemaphoreCount = Submit0_WaitSemaphoreCount;
	Submit[0].pWaitSemaphores = Submit0_phWaitSemaphore;
	Submit[0].pWaitDstStageMask = Submit0_pWaitDstStageMask;
	Submit[0].commandBufferCount = Submit0_CommandList3DCount;
	Submit[0].pCommandBuffers = Submit0_phCommandList3D;
	Submit[0].signalSemaphoreCount = Submit0_SignalSemaphoreCount;
	Submit[0].pSignalSemaphores = Submit0_phSignalSemaphore;

	//ReadBarrier Not Need
	//There Exists Approach ReadBarrier In the PTSAtomic_CompareAndSet(m_SwapChain_bLocked) Below //Really ???

	//Lock-Begin
	while (::PTSAtomic_CompareAndSet(&m_Queue3D0_bLocked, 0U, 1U) != 0U)
	{
		//Write-HighPriority
		::PTS_Pause();
	}

	VkResult eResult = rhDeviceWrapper.QueueSubmit(m_hQueue3D0, 1U, Submit, hSignalFence);
	assert(eResult == VK_SUCCESS);

	//Lock-End
	::PTSAtomic_Set(&m_Queue3D0_bLocked, 0U);
}

void PTVD_QueueManager_3D0_3D0::Queue3DExecuteCommandList(
	VkDeviceWrapper &rhDeviceWrapper,
	uint32_t Submit0_CommandList3DCount, const VkCommandBuffer *Submit0_phCommandList3D,
	uint32_t Submit0_WaitSemaphoreCount, VkSemaphore *Submit0_phWaitSemaphore, VkPipelineStageFlags *Submit0_pWaitDstStageMask,
	uint32_t Submit0_SignalSemaphoreCount, VkSemaphore *Submit0_phSignalSemaphore,
	uint32_t Submit1_CommandList3DCount, const VkCommandBuffer *Submit1_phCommandList3D,
	uint32_t Submit1_WaitSemaphoreCount, VkSemaphore *Submit1_phWaitSemaphore, VkPipelineStageFlags *Submit1_pWaitDstStageMask,
	uint32_t Submit1_SignalSemaphoreCount, VkSemaphore *Submit1_phSignalSemaphore,
	VkFence hSignalFence
)
{
	VkSubmitInfo Submit[2];

	Submit[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Submit[0].pNext = NULL;
	Submit[0].waitSemaphoreCount = Submit0_WaitSemaphoreCount;
	Submit[0].pWaitSemaphores = Submit0_phWaitSemaphore;
	Submit[0].pWaitDstStageMask = Submit0_pWaitDstStageMask;
	Submit[0].commandBufferCount = Submit0_CommandList3DCount;
	Submit[0].pCommandBuffers = Submit0_phCommandList3D;
	Submit[0].signalSemaphoreCount = Submit0_SignalSemaphoreCount;
	Submit[0].pSignalSemaphores = Submit0_phSignalSemaphore;

	Submit[1].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Submit[1].pNext = NULL;
	Submit[1].waitSemaphoreCount = Submit1_WaitSemaphoreCount;
	Submit[1].pWaitSemaphores = Submit1_phWaitSemaphore;
	Submit[1].pWaitDstStageMask = Submit1_pWaitDstStageMask;
	Submit[1].commandBufferCount = Submit1_CommandList3DCount;
	Submit[1].pCommandBuffers = Submit1_phCommandList3D;
	Submit[1].signalSemaphoreCount = Submit1_SignalSemaphoreCount;
	Submit[1].pSignalSemaphores = Submit1_phSignalSemaphore;

	//ReadBarrier Not Need
	//There Exists Approach ReadBarrier In the PTSAtomic_CompareAndSet(m_SwapChain_bLocked) Below //Really ???

	//Lock-Begin
	while (::PTSAtomic_CompareAndSet(&m_Queue3D0_bLocked, 0U, 1U) != 0U)
	{
		//Write-HighPriority
		::PTS_Pause();
	}

	VkResult eResult = rhDeviceWrapper.QueueSubmit(m_hQueue3D0, 2U, Submit, hSignalFence);
	assert(eResult == VK_SUCCESS);

	//Lock-End
	::PTSAtomic_Set(&m_Queue3D0_bLocked, 0U);
}

void PTVD_QueueManager_3D0_3D0::Queue3DExecuteCommandList(
	VkDeviceWrapper &rhDeviceWrapper,
	uint32_t Submit0_CommandList3DCount, const VkCommandBuffer *Submit0_phCommandList3D,
	uint32_t Submit0_WaitSemaphoreCount, VkSemaphore *Submit0_phWaitSemaphore, VkPipelineStageFlags *Submit0_pWaitDstStageMask,
	uint32_t Submit0_SignalSemaphoreCount, VkSemaphore *Submit0_phSignalSemaphore,
	uint32_t Submit1_CommandList3DCount, const VkCommandBuffer *Submit1_phCommandList3D,
	uint32_t Submit1_WaitSemaphoreCount, VkSemaphore *Submit1_phWaitSemaphore, VkPipelineStageFlags *Submit1_pWaitDstStageMask,
	uint32_t Submit1_SignalSemaphoreCount, VkSemaphore *Submit1_phSignalSemaphore,
	uint32_t Submit2_CommandList3DCount, const VkCommandBuffer *Submit2_phCommandList3D,
	uint32_t Submit2_WaitSemaphoreCount, VkSemaphore *Submit2_phWaitSemaphore, VkPipelineStageFlags *Submit2_pWaitDstStageMask,
	uint32_t Submit2_SignalSemaphoreCount, VkSemaphore *Submit2_phSignalSemaphore,
	VkFence hSignalFence
)
{
	VkSubmitInfo Submit[3];

	Submit[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Submit[0].pNext = NULL;
	Submit[0].waitSemaphoreCount = Submit0_WaitSemaphoreCount;
	Submit[0].pWaitSemaphores = Submit0_phWaitSemaphore;
	Submit[0].pWaitDstStageMask = Submit0_pWaitDstStageMask;
	Submit[0].commandBufferCount = Submit0_CommandList3DCount;
	Submit[0].pCommandBuffers = Submit0_phCommandList3D;
	Submit[0].signalSemaphoreCount = Submit0_SignalSemaphoreCount;
	Submit[0].pSignalSemaphores = Submit0_phSignalSemaphore;

	Submit[1].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Submit[1].pNext = NULL;
	Submit[1].waitSemaphoreCount = Submit1_WaitSemaphoreCount;
	Submit[1].pWaitSemaphores = Submit1_phWaitSemaphore;
	Submit[1].pWaitDstStageMask = Submit1_pWaitDstStageMask;
	Submit[1].commandBufferCount = Submit1_CommandList3DCount;
	Submit[1].pCommandBuffers = Submit1_phCommandList3D;
	Submit[1].signalSemaphoreCount = Submit1_SignalSemaphoreCount;
	Submit[1].pSignalSemaphores = Submit1_phSignalSemaphore;

	Submit[2].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Submit[2].pNext = NULL;
	Submit[2].waitSemaphoreCount = Submit2_WaitSemaphoreCount;
	Submit[2].pWaitSemaphores = Submit2_phWaitSemaphore;
	Submit[2].pWaitDstStageMask = Submit2_pWaitDstStageMask;
	Submit[2].commandBufferCount = Submit2_CommandList3DCount;
	Submit[2].pCommandBuffers = Submit2_phCommandList3D;
	Submit[2].signalSemaphoreCount = Submit2_SignalSemaphoreCount;
	Submit[2].pSignalSemaphores = Submit2_phSignalSemaphore;

	//ReadBarrier Not Need
	//There Exists Approach ReadBarrier In the PTSAtomic_CompareAndSet(m_SwapChain_bLocked) Below //Really ???

	//Lock-Begin
	while (::PTSAtomic_CompareAndSet(&m_Queue3D0_bLocked, 0U, 1U) != 0U)
	{
		//Write-HighPriority
		::PTS_Pause();
	}

	VkResult eResult = rhDeviceWrapper.QueueSubmit(m_hQueue3D0, 3U, Submit, hSignalFence);
	assert(eResult == VK_SUCCESS);

	//Lock-End
	::PTSAtomic_Set(&m_Queue3D0_bLocked, 0U);
}