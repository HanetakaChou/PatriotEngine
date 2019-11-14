#ifndef PT_VIDEO_VULKAN_QUEUE_H
#define PT_VIDEO_VULKAN_QUEUE_H

#include "PTVDVulkanHeader.h"

#include "PTVDDevice.h"

#include <assert.h>

struct IPTVD_QueueManager
{
	//Direct3D12规范
	//Multi-queue resource access
	//https://msdn.microsoft.com/en-us/library/windows/desktop/dn899217(v=vs.85).aspx#multi-queue_resource_access
	//A queue "type class" is any one of these two: Compute/3D and Copy.
	
	//DirectX 12 Graphics Samples / D3D12MemoryManagement / PagingContext
	//----------------------------------------------------------------------------
	//The PagingContext is a specialization of the Context object
	//that creates a unique D3D12 "Command Queue" and set of "Frames" (currently only one "Frame") for paging operations. 
	//The "Command Queue" is created as a "Copy "Command Queue",
	//which can map to "Paging Engines" on the graphics hardware.
	//In addition to the scheduling granularity offered by the graphics kernel scheduler,
	//being mapped to a hardware "Paging Engine" allows the paging operations to run completely in "Parallel" with 3D graphics work submitted on the "3D Engines".

	//In Vulkan
	//We assume there is One 3D Queue (The Compute/3D Type-Class) and One Copy Queue (The Copy Type-Class)
	
	//Present

#ifndef NDEBUG
	virtual VkBool32 Queue3DPresentValidate(
		VkInstanceWrapper &rhInstanceWrapper,VkPhysicalDevice hPhysicalDevice,
		VkSurfaceKHR hSurface
	) = 0;
#endif

	virtual VkResult Queue3DPresent(
		VkDeviceWrapper &rhDeviceWrapper,
		VkSwapchainKHR hSwapchain, uint32_t ImageIndex,
		//同步PostProcess的输出（UAV？？？）和呈现？？？ 
		//Vulkan不保证同一Queue中提交顺序的依赖 
		//使用Semaphore同步以与Direct3D12一致？？？
		uint32_t WaitSemaphoreCount, VkSemaphore *phWaitSemaphores 
		//VkPipelineStageFlags WaitDstStageMask, //Obviously, It Is The Stage Present That Wait
		//VkSemaphore hSignalSemaphore, //SignalSemaphore In AcquireNextImageKHR
		//VkFence hSignalFence //SignalFence In AcquireNextImageKHR
	) = 0;

	//The 3D/Compute Engine Begin ------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	//Direct ----------------------------------------------------------------------------------------------------
	virtual VkCommandPool CommandAllocator3DCreate(VkDeviceWrapper &rhDeviceWrapper) = 0;
	
	inline void CommandAllocator3DReset(VkDeviceWrapper &rhDeviceWrapper, VkCommandPool hCommandAllocator3D)
	{
		//vkResetCommandBuffer: Not Required
		//vkResetCommandPool: All CommandBuffer -> Initial State

		VkResult eResult = rhDeviceWrapper.ResetCommandPool(
			hCommandAllocator3D,
			VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT //ID3D12CommandAllocator::Reset
		);
		assert(eResult == VK_SUCCESS);
	}

	inline VkCommandBuffer CommandList3DCreate(VkDeviceWrapper &rhDeviceWrapper, VkCommandPool hCommandAllocator3D)
	{
		VkCommandBufferAllocateInfo AllocateInfo;
		AllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		AllocateInfo.pNext = NULL;
		AllocateInfo.commandPool = hCommandAllocator3D;
		AllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		AllocateInfo.commandBufferCount = 1U;

		VkCommandBuffer hCommandBuffer[1];

		VkResult eResultAllocate = rhDeviceWrapper.AllocateCommandBuffers(&AllocateInfo, hCommandBuffer);
		assert(eResultAllocate == VK_SUCCESS);

		VkCommandBufferBeginInfo BeginInfo;
		BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		BeginInfo.pNext = NULL;
		BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		BeginInfo.pInheritanceInfo = NULL;

		VkResult eResultBegin = rhDeviceWrapper.BeginCommandBuffer(hCommandBuffer[0], &BeginInfo);
		assert(eResultBegin == VK_SUCCESS);

		return hCommandBuffer[0];
	}

	inline void CommandList3DClose(VkDeviceWrapper &rhDeviceWrapper, VkCommandBuffer hCommandList3D)
	{
		VkResult eResult = rhDeviceWrapper.EndCommandBuffer(hCommandList3D);
		assert(eResult == VK_SUCCESS);
	}

	inline void CommandList3DReset(VkDeviceWrapper &rhDeviceWrapper, VkCommandBuffer hCommandList3D)
	{
		//vkResetCommandBuffer: Not Required
		//vkResetCommandPool: All CommandBuffer -> Initial State

		VkCommandBufferBeginInfo BeginInfo;
		BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		BeginInfo.pNext = NULL;
		BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		BeginInfo.pInheritanceInfo = NULL;

		VkResult eResult = rhDeviceWrapper.BeginCommandBuffer(hCommandList3D, &BeginInfo);
		assert(eResult == VK_SUCCESS);
	}

	//Vulkan专有-RenderPass支持
	//VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT
	inline VkCommandBuffer CommandList3DRenderPassInsideCreate(VkCommandPool hCommandAllocator3D) 
	{ 
		return VK_NULL_HANDLE;
	}

	inline void CommandList3DRenderPassInsideClose(VkCommandBuffer hCommandList3DRenderPassInside)
	{

	}

	inline void CommandList3DRenderPassInsideReset(VkCommandBuffer hCommandList3DRenderPassInside) 
	{
	}

	//Image Upload Begin -------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//Direct3D12规范
	//Implicit State Transitions / Common state promotion
	//https://msdn.microsoft.com/en-us/library/windows/desktop/dn899226(v=vs.85).aspx#Common_state_promotion

	//Vulkan规范
	//Queue Family Ownership Transfer
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//A "Queue Family Ownership Transfer" consists of two distinct parts: 1. Release exclusive ownership from the source queue family 2. Acquire exclusive ownership for the destination queue family
	//The "Acquire Operation" happens "Before" the "Visibility Operation".
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//A "Layout Transition" specified in this way happens-"After" the "Release Operation" and happens-"Before" the "Acquire Operation".

	//To Do !!!
	//Vulkan并不保证同一Queue中提交顺序的执行依赖
	//一般我们使用与交换链中的Buffer个数相同的CommandAllocator
	//显然，Acquire操作只能进行一次，我们只能在某一帧中执行Acquire操作，从而无法保证其余N-1帧的正确性

	//可选方案1：使用Semaphore同步不同帧 //确保同一Queue中提交顺序的执行依赖，以与Direct3D12一致？？？ //显然第一帧和第二帧不应当同时写入同一份GBuffer？？？ //同步应当分离Render和Present？？？
	//可选方案2：使用Fence将Acquire操作与CPU同步？？？

	//由于HeapTier1的原因，GBufferPass(RTV+DSV)和TileDeferred/SSR/SSAO/PostProcesss(UAV)的输出应当存放在不同的Heap中
	//理论上GeometryPipeline(VS/HS/DS/GS)和CUDACore(PS/CS)可以并行化，恰好与上述行为相符

	//In Vulkan
	//PipelineStage: 0 / Can Be TopOfPipe ??? -> FragmentShader|ComputeShader /是否应当拆分成不同的函数???
	//Access: TransferWrite -> ShaderWrite
	//ImageLayout: TransferDstOptimal -> ShaderReadOnlyOptimal
	virtual void CommandList3D_RecordImageUploadAcquire(VkDeviceWrapper &rhDeviceWrapper, VkCommandBuffer hCommandListCopy, VkImage hImage, VkImageSubresourceRange const &rSubresourceRange) = 0;

	//Image Upload End ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//Buffer ReadBack Begin -------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//PipelineStage: ComputeShader -> Host
	//Access: ShaderWrite -> HostRead
	virtual void CommandList3DRecord_BufferReadBack_Barrier(VkCommandBuffer hCommandList3D, VkBuffer hBuffer, uint64_t uiOffset, uint64_t uiSize) = 0;

	//Bundle ---------------------------------------------------------------------------------------------------

	//Record Once And 
	//For example, the PostProcess 

	virtual VkCommandPool CommandAllocatorBundleCreate() = 0;
	
	inline void CommandAllocatorBundleReset(VkDeviceWrapper &rhDeviceWrapper, VkCommandPool hCommandAllocatorBundle)
	{
		//vkResetCommandBuffer: Not Required
		//vkResetCommandPool: All CommandBuffer -> Initial State

		VkResult eResult = rhDeviceWrapper.ResetCommandPool(
			hCommandAllocatorBundle,
			VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT //ID3D12CommandAllocator::Reset
		);
		assert(eResult == VK_SUCCESS);
	}

	//Direct3D12规范
	//Bundle restrictions
	//https://msdn.microsoft.com/en-us/library/windows/desktop/dn899205(v=vs.85).aspx#bundle_restrictions
	//D3D12_COMMAND_LIST_TYPE_BUNDLE
	//-----------------------------------------------------------------
	//In Vulkan
	//VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT
	//-----------------------------------------------------------------
	inline VkCommandBuffer CommandListBundleCreate(VkCommandPool hCommandAllocatorBundle)
	{ 
		return VK_NULL_HANDLE; 
	}

	inline void CommandListBundleClose(VkCommandBuffer hCommandList3DBundle)
	{

	}

	inline void CommandListBundleReset(VkCommandBuffer hCommandList3DBundle)
	{

	}

	//Vulkan规范
	//同步
	//------------------------------------------
	//Fence
	//GPU-CPU间同步
	//------------------------------------------
	//Semaphore
	//GPU内Queue间同步
	//-------------------------------------------
	//Vulkan不保证同一Queue中提交顺序的依赖
	//Fence限于Submit
	//Semaphore限于Batch
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	virtual void Queue3DExecuteCommandList(
		VkDeviceWrapper &rhDeviceWrapper,
		uint32_t Submit0_CommandList3DCount, const VkCommandBuffer *Submit0_phCommandList3D,
		//物理模拟中计算流水线的输出可以用Barrier和图形流水线同步？？？ //Vulkan不保证同一Queue中提交顺序的依赖 //使用Semaphore同步以与Direct3D12一致？？？
		uint32_t Submit0_WaitSemaphoreCount, VkSemaphore *Submit0_phWaitSemaphore, VkPipelineStageFlags *Submit0_pWaitDstStageMask,
		uint32_t Submit0_SignalSemaphoreCount, VkSemaphore *Submit0_phSignalSemaphore,
		VkFence hSignalFence
	) = 0;

	virtual void Queue3DExecuteCommandList(
		VkDeviceWrapper &rhDeviceWrapper,
		uint32_t Submit0_CommandList3DCount, const VkCommandBuffer *Submit0_phCommandList3D,
		uint32_t Submit0_WaitSemaphoreCount, VkSemaphore *Submit0_phWaitSemaphore, VkPipelineStageFlags *Submit0_pWaitDstStageMask,
		uint32_t Submit0_SignalSemaphoreCount, VkSemaphore *Submit0_phSignalSemaphore,
		uint32_t Submit1_CommandList3DCount, const VkCommandBuffer *Submit1_phCommandList3D,
		uint32_t Submit1_WaitSemaphoreCount, VkSemaphore *Submit1_phWaitSemaphore, VkPipelineStageFlags *Submit1_pWaitDstStageMask,
		uint32_t Submit1_SignalSemaphoreCount, VkSemaphore *Submit1_phSignalSemaphore,
		VkFence hSignalFence
	) = 0;

	virtual void Queue3DExecuteCommandList(
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
	) = 0;

	//The 3D/Compute Engine End --------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//The Copy Engine Begin ----------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//Copy命令队列一般不需要二级命令列表 //在Direct3D12中仅3D命令列表允许ExecuteBundle

	virtual VkCommandPool CommandAllocatorCopyCreate(VkDeviceWrapper &rhDeviceWrapper) = 0;
	
	inline void CommandAllocatorCopyReset(VkDeviceWrapper &rhDeviceWrapper, VkCommandPool hCommandAllocatorCopy)
	{
		//vkResetCommandBuffer: Not Required
		//vkResetCommandPool: All CommandBuffer -> Initial State

		VkResult eResult = rhDeviceWrapper.ResetCommandPool(
			hCommandAllocatorCopy,
			VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT //ID3D12CommandAllocator::Reset
		);
		assert(eResult == VK_SUCCESS);
	}

	inline VkCommandBuffer CommandListCopyCreate(VkDeviceWrapper &rhDeviceWrapper, VkCommandPool hCommandAllocatorCopy)
	{
		VkCommandBufferAllocateInfo AllocateInfo;
		AllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		AllocateInfo.pNext = NULL;
		AllocateInfo.commandPool = hCommandAllocatorCopy;
		AllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		AllocateInfo.commandBufferCount = 1U;

		VkCommandBuffer hCommandBuffer[1];

		VkResult eResultAllocate = rhDeviceWrapper.AllocateCommandBuffers(&AllocateInfo, hCommandBuffer);
		assert(eResultAllocate == VK_SUCCESS);

		VkCommandBufferBeginInfo BeginInfo;
		BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		BeginInfo.pNext = NULL;
		BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		BeginInfo.pInheritanceInfo = NULL;

		VkResult eResultBegin = rhDeviceWrapper.BeginCommandBuffer(hCommandBuffer[0], &BeginInfo);
		assert(eResultBegin == VK_SUCCESS);

		return hCommandBuffer[0];
	}

	inline void CommandListCopyClose(VkDeviceWrapper &rhDeviceWrapper, VkCommandBuffer hCommandListCopy)
	{
		VkResult eResult = rhDeviceWrapper.EndCommandBuffer(hCommandListCopy);
		assert(eResult == VK_SUCCESS);
	}

	inline void CommandListCopyReset(VkDeviceWrapper &rhDeviceWrapper, VkCommandBuffer hCommandListCopy)
	{
		//vkResetCommandBuffer: Not Required
		//vkResetCommandPool: All CommandBuffer -> Initial State

		VkCommandBufferBeginInfo BeginInfo;
		BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		BeginInfo.pNext = NULL;
		BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		BeginInfo.pInheritanceInfo = NULL;

		VkResult eResult = rhDeviceWrapper.BeginCommandBuffer(hCommandListCopy, &BeginInfo);
		assert(eResult == VK_SUCCESS);
	}

	//Buffer Create

	//Direct3D12规范
	//Implicit State Transitions (Promotion And Decay)
	//https://msdn.microsoft.com/en-us/library/windows/desktop/dn899226(v=vs.85).aspx#implicit_state_transitions
	//D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS 
	//Cannot be used with D3D12_RESOURCE_DIMENSION_BUFFER; but buffers always have the properties represented by this flag.
	virtual void BufferCreate_PopulateShareMode(VkSharingMode *pSharingMode, uint32_t *pQueueFamilyIndexCount, const uint32_t **ppQueueFamilyIndices) = 0;

	//Buffer Upload Begin -------------------------------------------------------------------------------------------------------------------------------------------------------------
#if 1
	//The vkQueueSubmit command automatically guarantees that host writes flushed to VK_ACCESS_HOST_WRITE_BIT are made available
	//Host -> Transfer
	//virtual void CommandListCopyRecord_BufferUpload_Barrier(VkCommandBuffer hCommandListCopy, VkBuffer hBuffer, uint64_t uiOffset, uint64_t uiSize) = 0;

	//CommandListCopyRecord_CopyBufferToBuffer
	//......

	//Vulkan规范
	//Pipeline Stages
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT and VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT are useful 
	//for accomplishing "Layout Transitions" and "Queue Ownership Operations"
	//when the required "Execution Dependency" is satisfied by "Other Means" - for example, "Semaphore" operations between queues. //在BufferUpload中使用Fence而不使用Semaphore
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//一般我们不希望GPU在执行Draw/Compute操作时等待Copy操作（从而使帧率下降），使用Fence与CPU同步（CPU在确定Copy操作完成后才提交相应的Draw/Compute操作），而不使用Semaphore（不可能通过Barrier同步（因为不可能在Copy命令列表中提交Draw操作））
	//因此在BufferUpload中使用Fence而不使用Semaphore
	
	//Direct3D12规范
	//Implicit State Transitions / State decay to common
	//https://msdn.microsoft.com/en-us/library/windows/desktop/dn899226(v=vs.85).aspx#state_decay_to_common

	//PipelineStage: Transfer -> BottomOfPipe
	//Access: TransferWrite -> VertexAttributeRead
	//------------------------------------------------------------------------------------
	//virtual void CommandListCopyRecord_BufferVertexUpload_Barrier(VkCommandBuffer hCommandListCopy, VkBuffer hBuffer, uint64_t uiOffset, uint64_t uiSize) = 0;
	
	//PipelineStage: Transfer -> BottomOfPipe
	//Access: TransferWrite -> IndexRead
	//virtual void CommandListCopyRecord_BufferIndexUpload_Barrier(VkCommandBuffer hCommandListCopy, VkBuffer hBuffer, uint64_t uiOffset, uint64_t uiSize) = 0;

	//In Vulkan
	//For Buffer, there is no concept of "Layout Transition" //因此以上Barrier是没有必要的，只需要使用Fence即可完成同步

#endif
	//Buffer Upload End -------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//Image Upload Begin -------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//CommandListCopyRecord_CopyBufferToImage
	//......

#if 1
	//Vulkan规范
	//Pipeline Stages
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT and VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT are useful 
	//for accomplishing "Layout Transitions" and "Queue Ownership Operations"
	//when the required "Execution Dependency" is satisfied by "Other Means" - for example, "Semaphore" operations between queues. //在ImageUpload中使用Fence而不使用Semaphore
	
	//Vulkan规范
	//Queue Family Ownership Transfer
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//An application must ensure that these operations occur in the correct order by defining an execution dependency between them, e.g. using a "Semaphore". //在ImageUpload中使用Fence而不使用Semaphore

	//In Vulkan
	//一般我们不希望GPU在执行Draw/Compute操作时等待Copy操作（从而使帧率下降），使用Fence与CPU同步（CPU在确定Copy操作完成后才提交相应的Draw/Compute操作），而不使用Semaphore（不可能通过Barrier同步（因为不可能在Copy命令列表中提交Draw操作））
	//因此在ImageUpload中使用Fence而不使用Semaphore

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//Direct3D12规范
	//Implicit State Transitions / State decay to common
	//https://msdn.microsoft.com/en-us/library/windows/desktop/dn899226(v=vs.85).aspx#state_decay_to_common

	//Vulkan规范
	//Queue Family Ownership Transfer
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Release Operation
	//dstStageMask is ignored for such a barrier, such that no visibility operation is executed - the value of this mask does not affect the validity of the barrier.
	//The release operation happens - after the availability operation.
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Acquire Operation
	//srcStageMask is ignored for such a barrier, such that no availability operation is executed - the value of this mask does not affect the validity of the barrier.
	//The acquire operation happens-before the visibility operation.
	//---------------------------------------------------------------------------------------------------
	//Whilst it is not invalid to provide destination or source access masks for memory barriers used for release or acquire operations,
	//respectively, they have no practical effect.
	//Access after a release operation has undefined results, and so visibility for those accesses has no practical effect.
	//Similarly, write access before an acquire operation will produce undefined results for future access, so availability of those writes has no practical use.
	//In an earlier version of the specification, these were required to match on both sides - but this was subsequently relaxed.
	//These masks should be set to 0.
#endif

	//PipelineStage: Transfer -> 0 / Can Be BottomOfPipe ???
	//Access: TransferWrite -> ShaderRead
	//ImageLayout: TransferDstOptimal -> ShaderReadOnlyOptimal
	virtual void CommandListCopy_RecordImageUploadRelease(VkDeviceWrapper &rhDeviceWrapper, VkCommandBuffer hCommandListCopy, VkImage hImage, VkImageSubresourceRange const &rSubresourceRange) = 0;

	//Image Upload End -------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//Vulkan规范
	//同步
	//------------------------------------------
	//Fence
	//GPU-CPU间同步
	//------------------------------------------
	//Semaphore
	//GPU内Queue间同步
	//-------------------------------------------
	//Vulkan不保证同一Queue中提交顺序的执行依赖
	//Fence限于Submit
	//Semaphore限于Batch
	//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//一般我们不希望GPU在执行Draw/Compute操作时等待Copy操作（从而使帧率下降），使用Fence与CPU同步（CPU在确定Copy操作完成后才提交相应的Draw/Compute操作），而不使用Semaphore（不可能通过Barrier同步（因为不可能在Copy命令列表中提交Draw操作））

	virtual void CommandQueueCopy_CommandListsExecute(
		VkDeviceWrapper &rhDeviceWrapper,
		uint32_t CommandListCopyCount, const VkCommandBuffer *pCommandListCopys,
		//VkSemaphore hWaitSemaphore = VK_NULL_HANDLE, VkPipelineStageFlags WaitDstStageMask = 0U, //Copy Queue Hardly Wait For Others
		//VkSemaphore hSignalSemaphore = VK_NULL_HANDLE, //在BufferUpload/ImageUpload中使用Fence而不使用Semaphore
		VkFence hSignalFence
	) = 0;

	//The Copy Engine End ----------------------------------------------------------------------------------------------------------------------------------------------------------------------
};

class PTVD_QueueManager_3D0_Copy0 : public IPTVD_QueueManager
{
	VkQueue m_hQueue3D0;
	VkQueue m_hQueueCopy0;
	uint32_t m_Queue3D_FamilyIndex;
	uint32_t m_QueueCopy_FamilyIndex;
#ifndef NDEBUG
	bool m_Queue3D0_bLocked;
	bool m_QueueCopy0_bLocked;
#endif

public:
	inline PTVD_QueueManager_3D0_Copy0(
		uint32_t Queue3D_FamilyIndex,
		uint32_t QueueCopy_FamilyIndex,
		VkQueue hQueue3D0,
		VkQueue hQueueCopy0
	) :
		m_hQueue3D0(hQueue3D0),
		m_hQueueCopy0(hQueueCopy0),
		m_Queue3D_FamilyIndex(Queue3D_FamilyIndex),
		m_QueueCopy_FamilyIndex(QueueCopy_FamilyIndex)
#ifndef NDEBUG
		, m_Queue3D0_bLocked(false)
		, m_QueueCopy0_bLocked(false)
#endif
	{

	}

#ifndef NDEBUG
	VkBool32 Queue3DPresentValidate(VkInstanceWrapper &rhInstanceWrapper, VkPhysicalDevice hPhysicalDevice, VkSurfaceKHR hSurface) override;
#endif

	VkResult Queue3DPresent(
		VkDeviceWrapper &rhDeviceWrapper,
		VkSwapchainKHR hSwapchain, uint32_t ImageIndex,
		uint32_t WaitSemaphoreCount, VkSemaphore *phWaitSemaphores
	) override;

	VkCommandPool CommandAllocator3DCreate(VkDeviceWrapper &rhDeviceWrapper) override;

	void CommandList3D_RecordImageUploadAcquire(VkDeviceWrapper &rhDeviceWrapper, VkCommandBuffer hCommandListCopy, VkImage hImage, VkImageSubresourceRange const &rSubresourceRange) override;
	void CommandList3DRecord_BufferReadBack_Barrier(VkCommandBuffer hCommandList3D, VkBuffer hBuffer, uint64_t uiOffset, uint64_t uiSize) override {}
	
	VkCommandPool CommandAllocatorBundleCreate() override { return VK_NULL_HANDLE; }
	
	void Queue3DExecuteCommandList(
		VkDeviceWrapper &rhDeviceWrapper,
		uint32_t Submit0_CommandList3DCount, const VkCommandBuffer *Submit0_phCommandList3D,
		uint32_t Submit0_WaitSemaphoreCount, VkSemaphore *Submit0_phWaitSemaphore, VkPipelineStageFlags *Submit0_pWaitDstStageMask,
		uint32_t Submit0_SignalSemaphoreCount, VkSemaphore *Submit0_phSignalSemaphore,
		VkFence hSignalFence
	) override;

	void Queue3DExecuteCommandList(
		VkDeviceWrapper &rhDeviceWrapper,
		uint32_t Submit0_CommandList3DCount, const VkCommandBuffer *Submit0_phCommandList3D,
		uint32_t Submit0_WaitSemaphoreCount, VkSemaphore *Submit0_phWaitSemaphore, VkPipelineStageFlags *Submit0_pWaitDstStageMask,
		uint32_t Submit0_SignalSemaphoreCount, VkSemaphore *Submit0_phSignalSemaphore,
		uint32_t Submit1_CommandList3DCount, const VkCommandBuffer *Submit1_phCommandList3D,
		uint32_t Submit1_WaitSemaphoreCount, VkSemaphore *Submit1_phWaitSemaphore, VkPipelineStageFlags *Submit1_pWaitDstStageMask,
		uint32_t Submit1_SignalSemaphoreCount, VkSemaphore *Submit1_phSignalSemaphore,
		VkFence hSignalFence
	) override;

	void Queue3DExecuteCommandList(
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
	) override;

	VkCommandPool CommandAllocatorCopyCreate(VkDeviceWrapper &rhDeviceWrapper) override;

	void BufferCreate_PopulateShareMode(VkSharingMode *pSharingMode, uint32_t *pQueueFamilyIndexCount, const uint32_t **ppQueueFamilyIndices) override;

	void CommandListCopy_RecordImageUploadRelease(VkDeviceWrapper &rhDeviceWrapper, VkCommandBuffer hCommandListCopy, VkImage hImage, VkImageSubresourceRange const &rSubresourceRange) override;

	void CommandQueueCopy_CommandListsExecute(
		VkDeviceWrapper &rhDeviceWrapper,
		uint32_t CommandListCopyCount, const VkCommandBuffer *pCommandListCopys,
		VkFence hSignalFence
	) override;
};

class PTVD_QueueManager_3D0_Compute0 : public IPTVD_QueueManager
{
	VkQueue m_hQueue3D0;
	VkQueue m_hQueueCompute0;
	uint32_t m_Queue3D_FamilyIndex;
	uint32_t m_QueueCompute_FamilyIndex;
#ifndef NDEBUG
	bool m_Queue3D0_bLocked;
	bool m_QueueCompute0_bLocked;
#endif

public:
	inline PTVD_QueueManager_3D0_Compute0(
		uint32_t Queue3D_FamilyIndex,
		uint32_t QueueCompute_FamilyIndex,
		VkQueue hQueue3D0,
		VkQueue hQueueCompute0
	) :
		m_hQueue3D0(hQueue3D0),
		m_hQueueCompute0(hQueueCompute0),
		m_Queue3D_FamilyIndex(Queue3D_FamilyIndex),
		m_QueueCompute_FamilyIndex(QueueCompute_FamilyIndex)
#ifndef NDEBUG
		, m_Queue3D0_bLocked(false)
		, m_QueueCompute0_bLocked(false)
#endif
	{

	}

#ifndef NDEBUG
	VkBool32 Queue3DPresentValidate(
		VkInstanceWrapper &rhInstanceWrapper, VkPhysicalDevice hPhysicalDevice,
		VkSurfaceKHR hSurface
	) override;
#endif

	VkResult Queue3DPresent(
		VkDeviceWrapper &rhDeviceWrapper,
		VkSwapchainKHR hSwapchain, uint32_t ImageIndex,
		uint32_t WaitSemaphoreCount, VkSemaphore *phWaitSemaphores
	) override
	{
		return VK_SUCCESS;
	}

	VkCommandPool CommandAllocator3DCreate(VkDeviceWrapper &rhDeviceWrapper) override;

	void CommandList3D_RecordImageUploadAcquire(VkDeviceWrapper &rhDeviceWrapper, VkCommandBuffer hCommandListCopy, VkImage hImage, VkImageSubresourceRange const &rSubresourceRange) override {}
	void CommandList3DRecord_BufferReadBack_Barrier(VkCommandBuffer hCommandList3D, VkBuffer hBuffer, uint64_t uiOffset, uint64_t uiSize) override {}

	VkCommandPool CommandAllocatorBundleCreate() override { return VK_NULL_HANDLE; }

	void Queue3DExecuteCommandList(
		VkDeviceWrapper &rhDeviceWrapper,
		uint32_t Submit0_CommandList3DCount, const VkCommandBuffer *Submit0_phCommandList3D,
		uint32_t Submit0_WaitSemaphoreCount, VkSemaphore *Submit0_phWaitSemaphore, VkPipelineStageFlags *Submit0_pWaitDstStageMask,
		uint32_t Submit0_SignalSemaphoreCount, VkSemaphore *Submit0_phSignalSemaphore,
		VkFence hSignalFence
	) override;

	void Queue3DExecuteCommandList(
		VkDeviceWrapper &rhDeviceWrapper,
		uint32_t Submit0_CommandList3DCount, const VkCommandBuffer *Submit0_phCommandList3D,
		uint32_t Submit0_WaitSemaphoreCount, VkSemaphore *Submit0_phWaitSemaphore, VkPipelineStageFlags *Submit0_pWaitDstStageMask,
		uint32_t Submit0_SignalSemaphoreCount, VkSemaphore *Submit0_phSignalSemaphore,
		uint32_t Submit1_CommandList3DCount, const VkCommandBuffer *Submit1_phCommandList3D,
		uint32_t Submit1_WaitSemaphoreCount, VkSemaphore *Submit1_phWaitSemaphore, VkPipelineStageFlags *Submit1_pWaitDstStageMask,
		uint32_t Submit1_SignalSemaphoreCount, VkSemaphore *Submit1_phSignalSemaphore,
		VkFence hSignalFence
	) override;

	void Queue3DExecuteCommandList(
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
	) override;


	VkCommandPool CommandAllocatorCopyCreate(VkDeviceWrapper &rhDeviceWrapper) override { return VK_NULL_HANDLE; }

	void BufferCreate_PopulateShareMode(VkSharingMode *pSharingMode, uint32_t *pQueueFamilyIndexCount, const uint32_t **ppQueueFamilyIndices) override {}

	void CommandListCopy_RecordImageUploadRelease(VkDeviceWrapper &rhDeviceWrapper, VkCommandBuffer hCommandListCopy, VkImage hImage, VkImageSubresourceRange const &rSubresourceRange) override {}

	void CommandQueueCopy_CommandListsExecute(
		VkDeviceWrapper &rhDeviceWrapper,
		uint32_t CommandListCopyCount, const VkCommandBuffer *pCommandListCopys,
		VkFence hSignalFence
	) override {}
};

class PTVD_QueueManager_3D0_3D1 : public IPTVD_QueueManager
{
	VkQueue m_hQueue3D0;
	VkQueue m_hQueue3D1;
	uint32_t m_Queue3D_FamilyIndex;
#ifndef NDEBUG
	bool m_Queue3D0_bLocked;
	bool m_Queue3D1_bLocked;
#endif


public:
	inline PTVD_QueueManager_3D0_3D1(
		uint32_t Queue3D_FamilyIndex,
		VkQueue hQueue3D0,
		VkQueue hQueue3D1
	) :
		m_hQueue3D0(hQueue3D0),
		m_hQueue3D1(hQueue3D1),
		m_Queue3D_FamilyIndex(Queue3D_FamilyIndex)
#ifndef NDEBUG
		, m_Queue3D0_bLocked(false)
		, m_Queue3D1_bLocked(false)
#endif
	{

	}

#ifndef NDEBUG
	VkBool32 Queue3DPresentValidate(
		VkInstanceWrapper &rhInstanceWrapper, VkPhysicalDevice hPhysicalDevice, 
		VkSurfaceKHR hSurface
	) override;
#endif

	VkResult Queue3DPresent(
		VkDeviceWrapper &rhDeviceWrapper,
		VkSwapchainKHR hSwapchain, uint32_t ImageIndex,
		uint32_t WaitSemaphoreCount, VkSemaphore *phWaitSemaphores
	) override;

	VkCommandPool CommandAllocator3DCreate(VkDeviceWrapper &rhDeviceWrapper) override;

	void CommandList3D_RecordImageUploadAcquire(VkDeviceWrapper &rhDeviceWrapper, VkCommandBuffer hCommandListCopy, VkImage hImage, VkImageSubresourceRange const &rSubresourceRange) override {}
	void CommandList3DRecord_BufferReadBack_Barrier(VkCommandBuffer hCommandList3D, VkBuffer hBuffer, uint64_t uiOffset, uint64_t uiSize) override {}

	VkCommandPool CommandAllocatorBundleCreate() override { return VK_NULL_HANDLE; }

	void Queue3DExecuteCommandList(
		VkDeviceWrapper &rhDeviceWrapper,
		uint32_t Submit0_CommandList3DCount, const VkCommandBuffer *Submit0_phCommandList3D,
		uint32_t Submit0_WaitSemaphoreCount, VkSemaphore *Submit0_phWaitSemaphore, VkPipelineStageFlags *Submit0_pWaitDstStageMask,
		uint32_t Submit0_SignalSemaphoreCount, VkSemaphore *Submit0_phSignalSemaphore,
		VkFence hSignalFence
	) override;

	void Queue3DExecuteCommandList(
		VkDeviceWrapper &rhDeviceWrapper,
		uint32_t Submit0_CommandList3DCount, const VkCommandBuffer *Submit0_phCommandList3D,
		uint32_t Submit0_WaitSemaphoreCount, VkSemaphore *Submit0_phWaitSemaphore, VkPipelineStageFlags *Submit0_pWaitDstStageMask,
		uint32_t Submit0_SignalSemaphoreCount, VkSemaphore *Submit0_phSignalSemaphore,
		uint32_t Submit1_CommandList3DCount, const VkCommandBuffer *Submit1_phCommandList3D,
		uint32_t Submit1_WaitSemaphoreCount, VkSemaphore *Submit1_phWaitSemaphore, VkPipelineStageFlags *Submit1_pWaitDstStageMask,
		uint32_t Submit1_SignalSemaphoreCount, VkSemaphore *Submit1_phSignalSemaphore,
		VkFence hSignalFence
	) override;

	void Queue3DExecuteCommandList(
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
	) override;


	VkCommandPool CommandAllocatorCopyCreate(VkDeviceWrapper &rhDeviceWrapper) override { return VK_NULL_HANDLE; }

	void BufferCreate_PopulateShareMode(VkSharingMode *pSharingMode, uint32_t *pQueueFamilyIndexCount, const uint32_t **ppQueueFamilyIndices) override;

	void CommandListCopy_RecordImageUploadRelease(VkDeviceWrapper &rhDeviceWrapper, VkCommandBuffer hCommandListCopy, VkImage hImage, VkImageSubresourceRange const &rSubresourceRange) override {}

	void CommandQueueCopy_CommandListsExecute(
		VkDeviceWrapper &rhDeviceWrapper,
		uint32_t CommandListCopyCount, const VkCommandBuffer *pCommandListCopys,
		VkFence hSignalFence
	) override {}
};

class PTVD_QueueManager_3D0_3D0 :public IPTVD_QueueManager
{
	uint32_t m_Queue3D_FamilyIndex;
	uint32_t m_Queue3D0_bLocked;
	VkQueue m_hQueue3D0;

public:
	inline PTVD_QueueManager_3D0_3D0(
		uint32_t Queue3D_FamilyIndex,
		VkQueue hQueue3D0
	) :
		m_Queue3D_FamilyIndex(Queue3D_FamilyIndex),
		m_Queue3D0_bLocked(0U),
		m_hQueue3D0(hQueue3D0)
	{

	}

#ifndef NDEBUG
	VkBool32 Queue3DPresentValidate(
		VkInstanceWrapper &rhInstanceWrapper, VkPhysicalDevice hPhysicalDevice,
		VkSurfaceKHR hSurface
	) override;
#endif

	VkResult Queue3DPresent(
		VkDeviceWrapper &rhDeviceWrapper,
		VkSwapchainKHR hSwapchain, uint32_t ImageIndex,
		uint32_t WaitSemaphoreCount, VkSemaphore *phWaitSemaphores
	) override;

	VkCommandPool CommandAllocator3DCreate(VkDeviceWrapper &rhDeviceWrapper) override;

	void CommandList3D_RecordImageUploadAcquire(VkDeviceWrapper &rhDeviceWrapper, VkCommandBuffer hCommandListCopy, VkImage hImage, VkImageSubresourceRange const &rSubresourceRange) override {}
	void CommandList3DRecord_BufferReadBack_Barrier(VkCommandBuffer hCommandList3D, VkBuffer hBuffer, uint64_t uiOffset, uint64_t uiSize) override {}

	VkCommandPool CommandAllocatorBundleCreate() override { return VK_NULL_HANDLE; }

	void Queue3DExecuteCommandList(
		VkDeviceWrapper &rhDeviceWrapper,
		uint32_t Submit0_CommandList3DCount, const VkCommandBuffer *Submit0_phCommandList3D,
		uint32_t Submit0_WaitSemaphoreCount, VkSemaphore *Submit0_phWaitSemaphore, VkPipelineStageFlags *Submit0_pWaitDstStageMask,
		uint32_t Submit0_SignalSemaphoreCount, VkSemaphore *Submit0_phSignalSemaphore,
		VkFence hSignalFence
	) override;

	void Queue3DExecuteCommandList(
		VkDeviceWrapper &rhDeviceWrapper,
		uint32_t Submit0_CommandList3DCount, const VkCommandBuffer *Submit0_phCommandList3D,
		uint32_t Submit0_WaitSemaphoreCount, VkSemaphore *Submit0_phWaitSemaphore, VkPipelineStageFlags *Submit0_pWaitDstStageMask,
		uint32_t Submit0_SignalSemaphoreCount, VkSemaphore *Submit0_phSignalSemaphore,
		uint32_t Submit1_CommandList3DCount, const VkCommandBuffer *Submit1_phCommandList3D,
		uint32_t Submit1_WaitSemaphoreCount, VkSemaphore *Submit1_phWaitSemaphore, VkPipelineStageFlags *Submit1_pWaitDstStageMask,
		uint32_t Submit1_SignalSemaphoreCount, VkSemaphore *Submit1_phSignalSemaphore,
		VkFence hSignalFence
	) override;

	void Queue3DExecuteCommandList(
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
	) override;


	VkCommandPool CommandAllocatorCopyCreate(VkDeviceWrapper &rhDeviceWrapper) override { return VK_NULL_HANDLE; }

	void BufferCreate_PopulateShareMode(VkSharingMode *pSharingMode, uint32_t *pQueueFamilyIndexCount, const uint32_t **ppQueueFamilyIndices) override;

	void CommandListCopy_RecordImageUploadRelease(VkDeviceWrapper &rhDeviceWrapper, VkCommandBuffer hCommandListCopy, VkImage hImage, VkImageSubresourceRange const &rSubresourceRange) override {}

	void CommandQueueCopy_CommandListsExecute(
		VkDeviceWrapper &rhDeviceWrapper,
		uint32_t CommandListCopyCount, const VkCommandBuffer *pCommandListCopys,
		VkFence hSignalFence
	) override {}
};

#endif