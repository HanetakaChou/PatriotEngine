#include "PTVDVKMemoryAllocator.h"
#include <assert.h>
#include <set>
#include <map>
#include <functional>
#include "../../../Public/McRT/PTSMemoryAllocator.h"
#include "../../../Public/McRT/PTSThread.h"

//Abstract GPU //Two Categories
//1.RadeonGPU //Sort Last Fragment //TiledMemory Avaliable
//AMD_Radeon NVIDIA_GTX Intel_UHD
//2.MaliGPU   //Sort-Middle Tiled  //TiledMemory Unavaliable
//ARM_Mali Qualcomm_Adreno ImgTec_PowerVR NVIDIA_Tegra

//Intel_UHD/APU(AMD_Radeon)不支持TiledMemory，应当归入第一类 
//AMD的SpecialPool统一处理 //即NonTiledGPU也可能支持SharedMemory Between CPU And GPU
//FrameBufferFetch(NonCoherent) is available on Intel GPU

// RadeonGPU

// @see Device::InitMemoryHeapProperties in PAL(https://github.com/GPUOpen-Drivers/pal)
// heapProperties with cpuWriteCombined must be cpuUncached

// @see PalGpuHeapToVkMemoryHeapFlags in XGL(https://github.com/GPUOpen-Drivers/xgl)
// @see PhysicalDevice::Initialize in XGL(https://github.com/GPUOpen-Drivers/xgl)
// MemoryHeap
// GpuHeapInvisible                        // The GPU Memory (Typically 8GB)         // DEVICE_LOCAL_BIT //May Be Unavaliable On The Integrated GPU
// GpuHeapLocal                            // The AMD Special Pool (Typically 256MB) // DEVICE_LOCAL_BIT
// GpuHeapGartUswc | GpuHeapGartCacheable  // The System Memory (Typically 32GB)     // NONE
// MemoryType
// DEVICE_LOCAL_BIT                                    //The "GpuHeapInvisible" 
// DEVICE_LOCAL_BIT|HOST_VISIBLE_BIT|HOST_COHERENT_BIT //The "GpuHeapLocal"
// HOST_VISIBLE_BIT|HOST_COHERENT_BIT                  //The "GpuHeapGartUswc"      // cpuWriteCombined
// HOST_VISIBLE_BIT|HOST_COHERENT_BIT|HOST_CACHED_BIT  //The "GpuHeapGartCacheable" // HOST_CACHED_BIT is for "readback"

// MaliGPU


static uint32_t const s_Block_Size = 1024U * 1024U * 256U; //256MB

static inline VkDeviceSize PTVD_Size_AlignDownToPowerOfTwo(VkDeviceSize Value);

bool PTVD_DeviceMemoryManager::Construct(VkInstanceWrapper &rInstanceWrapper, VkDeviceWrapper &rDeviceWrapper, VkPhysicalDevice hPhysicalDevice, bool HostVector[PTVD_MEMORYTYPE_COUNT], VkFormat *pTextureDS_SR_Format, VkFormat *pTextureDS_Format)
{
	//https://vulkan.gpuinfo.org

	//NCC //Flush //Unmap is still useful in CC for "Zero-Copy Optimization" //Because We can free the virual address 
	//UMA //"Zero-Copy Optimization" //There Is No WriteToSubresource Optimal In Vulkan !!! //No Optimal For Image //We Always Need StagingBuffer

	//Adreno GPU (API 1.0.3 For Some GPU) The Memory Type Does Not Have The DEVICE_LOCAL Flag ??? //We Don't Support These GPU.

	//Adreno GPU (API 1.1.66 For Some GPU) Has Two Memory Pool. Maybe The Second Is For Dedicated Memory ??? //Maybe Optimized For The On-Chip Memory In Tile Architect

	//Mali GPU Lazily Alllocated

	//PowerVR GPU is Discrete //But appears as UMA //DeviceType Is Not Reliable	 //Maybe it is a bug and fixed at API 1.0.80 ???

	//Intel GPU (For Some) Has Two Memory Pool

	//AMD RADV GPU is Discrete //But has a special pool!

	//StagingBuffer

	//UniformBuffer

	//VertexBufferAndIndexBuffer

	//SampledImage

	//FrameBuffer(Input/Color/Depth/Stencil Attachment) //Delicated Allocation //Optimized For Tiled-Architect

#if 1	//Mobile

#if 1	//Qualcomm Adreno //UMA-CC

	//Qualcomm Adreno API 1.0.3
	//***************************************************************************************************************************************************************************************************************************************************************
	//DeviceType
	//Integrated
	//***************************************************************************************************************************************************************************************************************************************************************
	//MemoryHeap
	//0 DEVICE_LOCAL //POOL_L0
	//MemoryType
	//0 DEVICE_LOCAL												//CPU_PAGE_PROPERTY_NOT_AVAILABLE 
	//1 HOST_VISIBLE+HOST_CACHED									//CPU_PAGE_PROPERTY_WRITE_BACK					//The Memory Type Does Not Have The DEVICE_LOCAL Flag ???
	//2 HOST_VISIBLE+HOST_COHERENT+HOST_CACHED						//CPU_PAGE_PROPERTY_WRITE_BACK		
	//***************************************************************************************************************************************************************************************************************************************************************

	//Qualcomm Adreno API 1.0.3/1.0.20/1.0.31/1.0.33/1.0.38
	//***************************************************************************************************************************************************************************************************************************************************************
	//DeviceType
	//Integrated
	//***************************************************************************************************************************************************************************************************************************************************************
	//MemoryHeap
	//0 DEVICE_LOCAL //POOL_L0
	//***************************************************************************************************************************************************************************************************************************************************************
	//MemoryType
	//0 DEVICE_LOCAL												//CPU_PAGE_PROPERTY_NOT_AVAILABLE																				//TextureAll
	//1 DEVICE_LOCAL+HOST_VISIBLE+HOST_CACHED						//CPU_PAGE_PROPERTY_WRITE_BACK					//UMA(DEVICE_LOCAL)-NCC(HOST_CACHED)
	//2 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT+HOST_CACHED			//CPU_PAGE_PROPERTY_WRITE_BACK					//UMA(DEVICE_LOCAL)-CC(HOST_CACHED+HOST_COHERENT)				//BufferStaging+BufferVertex+BufferIndex+BufferConstant
	//***************************************************************************************************************************************************************************************************************************************************************

	//Qualcomm Adreno API 1.0.38/1.0.49/1.0.61/1.0.66/1.1.66
	//***************************************************************************************************************************************************************************************************************************************************************
	//DeviceType
	//Integrated
	//***************************************************************************************************************************************************************************************************************************************************************
	//MemoryHeap
	//0 DEVICE_LOCAL //POOL_L0
	//MemoryType
	//0 DEVICE_LOCAL												//CPU_PAGE_PROPERTY_NOT_AVAILABLE																				//TextureAll 
	//1 DEVICE_LOCAL+HOST_VISIBLE+HOST_CACHED						//CPU_PAGE_PROPERTY_WRITE_BACK					//UMA(DEVICE_LOCAL)-NCC(HOST_CACHED)
	//2 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT+HOST_CACHED			//CPU_PAGE_PROPERTY_WRITE_BACK					//UMA(DEVICE_LOCAL)-CC(HOST_CACHED+HOST_COHERENT)				//BufferStaging+BufferVertex+BufferIndex+BufferConstant
	//3 DEVICE_LOCAL												//CPU_PAGE_PROPERTY_NOT_AVAILABLE																				//1 And 4 Are The Same //MayBe One For TILING_OPTIMAL And One For LILING_LINEAR(SHARED In D3D12)
	//4 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT						//CPU_PAGE_PROPERTY_WRITE_COMBINE				//UMA(DEVICE_LOCAL)
	//***************************************************************************************************************************************************************************************************************************************************************

	//Qualcomm Adreno API 1.1.66
	//***************************************************************************************************************************************************************************************************************************************************************
	//DeviceType
	//Integrated
	//***************************************************************************************************************************************************************************************************************************************************************
	//MemoryHeap
	//0 DEVICE_LOCAL //POOL_L0
	//MemoryType
	//0 DEVICE_LOCAL												//CPU_PAGE_PROPERTY_NOT_AVAILABLE																				//TextureAll
	//1 DEVICE_LOCAL+HOST_VISIBLE+HOST_CACHED						//CPU_PAGE_PROPERTY_WRITE_BACK					//UMA(DEVICE_LOCAL)-NCC(HOST_CACHED)
	//2 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT+HOST_CACHED			//CPU_PAGE_PROPERTY_WRITE_BACK					//UMA(DEVICE_LOCAL)-CC(HOST_CACHED+HOST_COHERENT)				//BufferStaging+BufferVertex+BufferIndex+BufferConstant
	//3 DEVICE_LOCAL												//CPU_PAGE_PROPERTY_NOT_AVAILABLE																				//1 And 4 Are The Same //MayBe One For TILING_OPTIMAL And One For LILING_LINEAR(SHARED In D3D12)
	//4 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT						//CPU_PAGE_PROPERTY_WRITE_COMBINE				//UMA(DEVICE_LOCAL)
	//***************************************************************************************************************************************************************************************************************************************************************
	//MemoryHeap
	//1 DEVICE_LOCAL //POOL_L0
	//MemoryType
	//5 DEVICE_LOCAL												//CPU_PAGE_PROPERTY_NOT_AVAILABLE																				//HeapSize Is Smaller //MayBe For Delicated Memory ???
	//***************************************************************************************************************************************************************************************************************************************************************

#endif

#if 1 //ARM Mali //UMA-NCC

	//ARM Mali API 1.0.2/1.0.11/1.0.14/1.0.26/1.0.36/1.0.47/1.0.52/1.0.53/1.0.58/1.0.61/1.0.65/1.0.66/1.0.82
	//***************************************************************************************************************************************************************************************************************************************************************
	//DeviceType
	//Integrated
	//***************************************************************************************************************************************************************************************************************************************************************
	//MemoryHeap
	//0 DEVICE_LOCAL //POOL_L0
	//***************************************************************************************************************************************************************************************************************************************************************
	//MemoryType
	//0 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT						//CPU_PAGE_PROPERTY_WRITE_COMBINE				//UMA(DEVICE_LOCAL)												//BufferStaging+BufferVertex+BufferIndex+BufferConstant +Texture
	//1 DEVICE_LOCAL+HOST_VISIBLE+HOST_CACHED						//CPU_PAGE_PROPERTY_WRITE_BACK					//UMA(DEVICE_LOCAL)-NCC(HOST_CACHED)							
	//2 DEVICE_LOCAL+LAZILY_ALLOCATED								//CPU_PAGE_PROPERTY_NOT_AVAILABLE																				//TextureRT/DS //TransientAttachment //For Example, In Deferred, RTV To SRV
	//***************************************************************************************************************************************************************************************************************************************************************

#endif

#if 1 //ImgTec PowerVR //UMA-NCC

	//ImgTec PowerVR API 1.0.3
	//***************************************************************************************************************************************************************************************************************************************************************
	//DeviceType
	//Discrete //This May Be A Bug In The Driver ???
	//***************************************************************************************************************************************************************************************************************************************************************
	//MemoryHeap
	//0 DEVICE_LOCAL //POOL_L0
	//MemoryType
	//0 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT						//CPU_PAGE_PROPERTY_WRITE_COMBINE				//UMA(DEVICE_LOCAL)												//BufferStaging+BufferVertex+BufferIndex+BufferConstant+TextureAll
	//***************************************************************************************************************************************************************************************************************************************************************

	//ImgTec PowerVR API 1.0.49
	//***************************************************************************************************************************************************************************************************************************************************************
	//DeviceType
	//Discrete //This May Be A Bug In The Driver ???
	//***************************************************************************************************************************************************************************************************************************************************************
	//MemoryHeap
	//0 DEVICE_LOCAL //POOL_L0
	//MemoryType
	//0 DEVICE_LOCAL												//CPU_PAGE_PROPERTY_NOT_AVAILABLE																				//TextureAll
	//1 DEVICE_LOCAL												//CPU_PAGE_PROPERTY_NOT_AVAILABLE																				//0 And 1 Are The Same //MayBe One For TILING_OPTIMAL And One For LILING_LINEAR(SHARED In D3D12)
	//2 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT						//CPU_PAGE_PROPERTY_WRITE_COMBINE				//UMA(DEVICE_LOCAL)												//BufferStaging+BufferVertex+BufferIndex+BufferConstant
	//3 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT						//CPU_PAGE_PROPERTY_WRITE_COMBINE																				//2 And 3 Are The Same ???
	//4 DEVICE_LOCAL+HOST_VISIBLE+HOST_CACHED						//CPU_PAGE_PROPERTY_WRITE_BACK					//UMA(DEVICE_LOCAL)-NCC(HOST_CACHED)
	//***************************************************************************************************************************************************************************************************************************************************************

	//ImgTec PowerVR API 1.0.80
	//***************************************************************************************************************************************************************************************************************************************************************
	//DeviceType
	//Integrated //The Previous Version May Be A Bug ???
	//***************************************************************************************************************************************************************************************************************************************************************
	//MemoryHeap
	//0 DEVICE_LOCAL //POOL_L0
	//MemoryType
	//0 DEVICE_LOCAL												//CPU_PAGE_PROPERTY_NOT_AVAILABLE 
	//1 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT						//CPU_PAGE_PROPERTY_WRITE_COMBINE				//UMA(DEVICE_LOCAL)												//BufferStaging+BufferVertex+BufferIndex+BufferConstant
	//2 DEVICE_LOCAL+HOST_VISIBLE+HOST_CACHED						//CPU_PAGE_PROPERTY_WRITE_BACK					//UMA(DEVICE_LOCAL)-NCC(HOST_CACHED)
	//***************************************************************************************************************************************************************************************************************************************************************

#endif

#if 1 //Intel

	//Intel Bay Trail //UMA-NCC //API 1.0.57/API 1.1.0/API 1.1.80/API 1.1.90/API 1.1.96
	//***************************************************************************************************************************************************************************************************************************************************************
	//DeviceType
	//Integrated
	//***************************************************************************************************************************************************************************************************************************************************************
	//MemoryHeap
	//0 DEVICE_LOCAL //POOL_L0
	//***************************************************************************************************************************************************************************************************************************************************************
	//MemoryType
	//0 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT						//CPU_PAGE_PROPERTY_WRITE_COMBINE				//UMA(DEVICE_LOCAL)												//BufferStaging+BufferVertex+BufferIndex+BufferConstant+TextureAll
	//1 DEVICE_LOCAL+HOST_VISIBLE+HOST_CACHED						//CPU_PAGE_PROPERTY_WRITE_BACK					//UMA(DEVICE_LOCAL)-NCC(HOST_CACHED)
	//***************************************************************************************************************************************************************************************************************************************************************

	//Intel-[Braswell]-[API 1.0.57]  //UMA-CC
	//Intel-[HD 615]-[Kaby Lake GT2]-[API 1.0.57]
	//Intel-[HD 520]-[Skylake GT2]-[Driver 18.0.99]-[API 1.0.57]
	//Intel-[HD 520]-[Skylake GT2]-[Driver 18.1.99]-[API 1.1.76]
	//Intel-[HD 530]-[Skylake GT2]-[Driver 18.2.99]-[API 1.1.80]
	//***************************************************************************************************************************************************************************************************************************************************************
	//DeviceType
	//Integrated
	//***************************************************************************************************************************************************************************************************************************************************************
	//MemoryHeap
	//0 DEVICE_LOCAL //POOL_L0
	//MemoryType
	//0 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT+HOST_CACHED			//CPU_PAGE_PROPERTY_WRITE_BACK					//UMA(DEVICE_LOCAL)-CC(HOST_CACHED+HOST_COHERENT)				//BufferStaging+BufferVertex+BufferIndex+BufferConstant+TextureAll
	//***************************************************************************************************************************************************************************************************************************************************************
	//MemoryHeap
	//1 DEVICE_LOCAL //POOL_L0
	//MemoryType
	//1 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT+HOST_CACHED			//CPU_PAGE_PROPERTY_WRITE_BACK					//UMA(DEVICE_LOCAL)-CC(HOST_CACHED+HOST_COHERENT)				//HeapSize Is Smaller //MayBe For Delicated Memory ???
	//***************************************************************************************************************************************************************************************************************************************************************

	//Intel-[HD 615][Kaby Lake GT2]-[API 1.0.76] //UMA-CC
	//Intel-[HD 520]-[Skylake GT2]-[Driver 18.2.99]-[API 1.1.80]
	//***************************************************************************************************************************************************************************************************************************************************************
	//DeviceType
	//Integrated
	//***************************************************************************************************************************************************************************************************************************************************************
	//MemoryHeap
	//0 DEVICE_LOCAL //POOL_L0
	//MemoryType
	//0 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT+HOST_CACHED			//CPU_PAGE_PROPERTY_WRITE_BACK					//UMA(DEVICE_LOCAL)-CC(HOST_CACHED+HOST_COHERENT)				//BufferStaging+BufferVertex+BufferIndex+BufferConstant+TextureAll
	//***************************************************************************************************************************************************************************************************************************************************************

#endif

#if 1 //NVIDIA Tegra //UMA-NCC

	//NVIDIA Tegra API 1.0.2/1.0.3
	//***************************************************************************************************************************************************************************************************************************************************************
	//DeviceType
	//Integrated
	//***************************************************************************************************************************************************************************************************************************************************************
	//MemoryHeap
	//0 DEVICE_LOCAL //POOL_L0
	//Type
	//0 DEVICE_LOCAL												//CPU_PAGE_PROPERTY_NOT_AVAILABLE																				//TextureAll
	//1 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT						//CPU_PAGE_PROPERTY_WRITE_COMBINE				//UMA(DEVICE_LOCAL)												//BufferStaging+BufferVertex+BufferIndex+BufferConstant
	//2 DEVICE_LOCAL+HOST_VISIBLE+HOST_CACHED						//CPU_PAGE_PROPERTY_WRITE_BACK					//UMA(DEVICE_LOCAL)-NCC(HOST_CACHED)
	//***************************************************************************************************************************************************************************************************************************************************************

	//NVIDIA Tegra API 1.0.13/1.0.24/1.0.37/1.0.56/1.0.61
	//***************************************************************************************************************************************************************************************************************************************************************
	//DeviceType
	//Integrated
	//***************************************************************************************************************************************************************************************************************************************************************
	//MemoryHeap
	//0 DEVICE_LOCAL //POOL_L0
	//MemoryType
	//0 DEVICE_LOCAL												//CPU_PAGE_PROPERTY_NOT_AVAILABLE																				//TextureAll
	//1 DEVICE_LOCAL												//CPU_PAGE_PROPERTY_NOT_AVAILABLE																				//0 And 1 Are The Same //MayBe One For TILING_OPTIMAL And One For LILING_LINEAR(SHARED In D3D12)
	//2 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT						//CPU_PAGE_PROPERTY_WRITE_COMBINE				//UMA(DEVICE_LOCAL)												//BufferStaging+BufferVertex+BufferIndex+BufferConstant
	//3 DEVICE_LOCAL+HOST_VISIBLE+HOST_CACHED						//CPU_PAGE_PROPERTY_WRITE_BACK					//UMA(DEVICE_LOCAL)-NCC(HOST_CACHED)
	//***************************************************************************************************************************************************************************************************************************************************************

	//NVIDIA Tegra API 1.1.0
	//***************************************************************************************************************************************************************************************************************************************************************
	//DeviceType
	//Integrated
	//***************************************************************************************************************************************************************************************************************************************************************
	//MemoryHeap
	//0 DEVICE_LOCAL //POOL_L0
	//MemoryType
	//0 DEVICE_LOCAL												//CPU_PAGE_PROPERTY_NOT_AVAILABLE																				//Texture
	//1 DEVICE_LOCAL												//CPU_PAGE_PROPERTY_NOT_AVAILABLE																				//0 ,1 ,4 And 5 Are The Same //MayBe For Different TILING (OPTIMAL And LINEAR(SHARED In D3D12)) And LAYOUT (NON_RT_DS/RT_DS In D3D12)
	//2 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT						//CPU_PAGE_PROPERTY_WRITE_COMBINE				//UMA(DEVICE_LOCAL)												//BufferStaging+BufferVertex+BufferIndex+BufferConstant
	//3 DEVICE_LOCAL+HOST_VISIBLE+HOST_CACHED						//CPU_PAGE_PROPERTY_WRITE_BACK					//UMA(DEVICE_LOCAL)-NCC(HOST_CACHED)
	//4 DEVICE_LOCAL												//CPU_PAGE_PROPERTY_NOT_AVAILABLE 
	//5 DEVICE_LOCAL												//CPU_PAGE_PROPERTY_NOT_AVAILABLE
	//***************************************************************************************************************************************************************************************************************************************************************

#endif

#if 1 //AMD RADV //NUMA-CC(With Special Pool)

	//AMD RADV API 1.0.57/1.0.68/1.1.90
	//***************************************************************************************************************************************************************************************************************************************************************
	//DeviceType
	//Discrete
	//***************************************************************************************************************************************************************************************************************************************************************
	//MemoryHeap
	//0 DEVICE_LOCAL //POOL_L1
	//MemoryType
	//0 DEVICE_LOCAL												//CPU_PAGE_PROPERTY_NOT_AVAILABLE				//NUMA															//BufferVertex+BufferIndex+TextureAll
	//***************************************************************************************************************************************************************************************************************************************************************
	//MemoryHeap
	//1 DEVICE_LOCAL //POOL_L1 //256MB //Special Pool Of Video Memory //Adam Sawicki. "Memory Management in Vulkan and DX12." GDC 2018.
	//MemoryType
	//2 DEVICE_LOCAL+HOST_VISIBLE+HOST_COHERENT						//CPU_PAGE_PROPERTY_WRITE_COMBINE				//AMD Special Pool												//ConstantBuffer //256MB Is Not Big Enough //So We Don't Use It As Staging Buffer
	//***************************************************************************************************************************************************************************************************************************************************************
	//MemoryHeap
	//2 //POOL_L0
	//MemoryType
	//1 HOST_VISIBLE+HOST_COHERENT									//CPU_PAGE_PROPERTY_WRITE_COMBINE				//NUMA								
	//3 HOST_VISIBLE+HOST_CACHED+HOST_COHERENT						//CPU_PAGE_PROPERTY_WRITE_BACK					//NUMA-CC(HOST_CACHED+HOST_COHERENT)							//BufferStarting //The Special Pool Is Not Big Enough ///So We Use This Pool As Fallback
	//***************************************************************************************************************************************************************************************************************************************************************

#endif

#endif

#if 1	//PC

#if 1 //NVIDIA //NUMA-CC

	//NVIDIA 410/600/830M/840M API 1.0.3
	//NVIDIA 840M API 1.0.5
	//NVIDIA 940M API 1.0.4
	//***************************************************************************************************************************************************************************************************************************************************************
	//DeviceType
	//Discrete
	//***************************************************************************************************************************************************************************************************************************************************************
	//MemoryHeap
	//0 DEVICE_LOCAL //POOL_L1
	//MemoryType
	//1 DEVICE_LOCAL												//CPU_PAGE_PROPERTY_NOT_AVAILABLE				//NUMA															//TextureAll
	//***************************************************************************************************************************************************************************************************************************************************************
	//MemoryHeap
	//1 //POOL_L0
	//MemoryType
	//0																//We Simplely Ignore These Memory Type ???
	//1 HOST_VISIBLE+HOST_COHERENT									//CPU_PAGE_PROPERTY_WRITE_COMBINE				//NUMA		
	//2 HOST_VISIBLE+HOST_CACHED+HOST_COHERENT						//CPU_PAGE_PROPERTY_WRITE_BACK					//NUMA-CC(HOST_CACHED+HOST_COHERENT)							//BufferStarting //The Special Pool Is Not Big Enough ///So We Use This Pool As Fallback
	//***************************************************************************************************************************************************************************************************************************************************************


	//NVIDIA 840M API 1.0.8/1.0.13/1.0.24
	//NVIDIA 920M API 1.0.8/1.0.24
	//NVIDIA 920MX API 1.0.13/1.0.24
	//NVIDIA 940MX API 1.0.47/1.0.42/1.0.46/1.0.49/1.0.56/1.0.65/1.1.70/1.1.84
	//NVIDIA GeForce GT 1030 API 1.0.56/1.0.65/1.1.70/1.1.82/1.1.84
	//NVIDIA GeForce GT 630 API 1.0.56/1.0.65/1.1.70/1.1.73/1.1.82
	//***************************************************************************************************************************************************************************************************************************************************************
	//DeviceType
	//Discrete
	//***************************************************************************************************************************************************************************************************************************************************************
	//MemoryHeap
	//1 DEVICE_LOCAL //POOL_L1
	//MemoryType
	//7 DEVICE_LOCAL												//CPU_PAGE_PROPERTY_NOT_AVAILABLE				//NUMA															//TextureAll
	//8 DEVICE_LOCAL												//CPU_PAGE_PROPERTY_NOT_AVAILABLE				//NUMA															//7 And 8 Are The Same //MayBe One For TILING_OPTIMAL And One For LILING_LINEAR(SHARED In D3D12)
	//***************************************************************************************************************************************************************************************************************************************************************
	//MemoryHeap
	//0 //POOL_L0
	//MemoryType
	//0																//We Simplely Ignore These Memory Type ???
	//1
	//2
	//3
	//4
	//5
	//6
	//9 HOST_VISIBLE+HOST_COHERENT									//CPU_PAGE_PROPERTY_WRITE_COMBINE				//NUMA		
	//A HOST_VISIBLE+HOST_CACHED+HOST_COHERENT						//CPU_PAGE_PROPERTY_WRITE_BACK					//NUMA-CC(HOST_CACHED+HOST_COHERENT)							//BufferStarting //The Special Pool Is Not Big Enough ///So We Use This Pool As Fallback
	//***************************************************************************************************************************************************************************************************************************************************************

#endif

#endif

	VkPhysicalDeviceMemoryProperties MemoryProperties;
	rInstanceWrapper.GetPhysicalDeviceMemoryProperties(hPhysicalDevice, &MemoryProperties);

	if (MemoryProperties.memoryHeapCount > 3U)
	{
		assert(0); //"m_MemoryHeapSizeRemainVector" Is Not Big Enough
		return false;
	}

	for (uint32_t MemoryHeapIndex = 0U; MemoryHeapIndex < MemoryProperties.memoryHeapCount; ++MemoryHeapIndex)
	{
		m_MemoryHeapSizeRemainVector[MemoryHeapIndex] = MemoryProperties.memoryHeaps[MemoryHeapIndex].size;
	}

	//�Ժ��仯
	constexpr VkDeviceSize const s_BufferStaging_HeapSizeThreshold = VkDeviceSize(1024U * 1024U * 1024U); //1GB //The AMD Special Pool Is 256MB //The Second Pool Of Some GPU Is ???
	constexpr VkDeviceSize const s_BufferConstant_HeapSizeThreshold = VkDeviceSize(1024U * 1024U * 128U); //128MB
	constexpr VkDeviceSize const s_BufferUnorderedAccess_HeapSizeThreshold = VkDeviceSize(1024U * 1024U * 256U); //256MB
	constexpr VkDeviceSize const s_BufferVertex_HeapSizeThreshold = VkDeviceSize(1024U * 1024U * 1024U);
	constexpr VkDeviceSize const s_BufferIndex_HeapSizeThreshold = VkDeviceSize(1024U * 1024U * 1024U);
	constexpr VkDeviceSize const s_TextureRenderTarget_HeapSizeThreshold = VkDeviceSize(1024U * 1024U * 1024U);
	constexpr VkDeviceSize const s_TextureDepthStencil_HeapSizeThreshold = VkDeviceSize(1024U * 1024U * 1024U);
	constexpr VkDeviceSize const s_TextureUnorderedAccess_HeapSizeThreshold = VkDeviceSize(1024U * 1024U * 1024U);
	constexpr VkDeviceSize const s_TextureShadowResource_HeapSizeThreshold = VkDeviceSize(1024U * 1024U * 1024U);

	//The memoryTypeBits member is [Identical] for all VkBuffer objects created 
	//with the [Same] value 
	//for the [Flags]
	//and [Usage] members in the VkBufferCreateInfo structure passed to vkCreateBuffer.
	//Further, if [Usage1] and [Usage2] of type VkBufferUsageFlags are such that 
	//the bits set in [Usage2] are a subset of the bits set in [Usage1],
	//and they have the [Same] [Flags],
	//then the bits set in memoryTypeBits returned for [Usage1] must be a [Subset] of the bits set in memoryTypeBits returned for [Usage2],
	//for all values of [Flags].

	//BufferStaging
	{
		VkBufferCreateInfo CreateInfo;
		CreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		CreateInfo.pNext = NULL;
		CreateInfo.flags = 0U;
		//******************************************************************************************************************
		CreateInfo.size = 256U;
		//******************************************************************************************************************
		CreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT/*UpLoad*/;
		//******************************************************************************************************************
		CreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		CreateInfo.queueFamilyIndexCount = 0U;
		CreateInfo.pQueueFamilyIndices = NULL;
		//******************************************************************************************************************

		VkBuffer hBuffer;

		VkResult eResult = rDeviceWrapper.CreateBuffer(&CreateInfo, NULL, &hBuffer);
		assert(eResult == VK_SUCCESS);

		VkMemoryRequirements MemoryRequirements;
		rDeviceWrapper.GetBufferMemoryRequirements(hBuffer, &MemoryRequirements);

		rDeviceWrapper.DestroyBuffer(hBuffer, NULL);

		VkDeviceSize HeapFoundSize = VkDeviceSize(0U);
		bool HasFound = false;

		std::pair<VkMemoryPropertyFlags, bool> const TargetPropertyFlagsAndBoolVector[] = {
			{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true }, //For AMD Special Pool
			{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true },
			{ VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true },
			{ VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true },
		};

		for (std::pair<VkMemoryPropertyFlags, bool> const &TargetPropertyFlagsAndBool : TargetPropertyFlagsAndBoolVector)
		{
			if ((!HasFound) || (HasFound && (HeapFoundSize < s_BufferStaging_HeapSizeThreshold)))
			{
				for (uint32_t MemoryTypeIndex = 0U, MemoryTypeBit = 1U; MemoryTypeIndex < MemoryProperties.memoryTypeCount; ++MemoryTypeIndex, MemoryTypeBit <<= 1U)
				{
					if ((MemoryRequirements.memoryTypeBits&MemoryTypeBit) && (MemoryProperties.memoryTypes[MemoryTypeIndex].propertyFlags == TargetPropertyFlagsAndBool.first))
					{
						if ((!HasFound) || (HeapFoundSize < MemoryProperties.memoryHeaps[MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex].size))
						{
							if (!HasFound)
							{
								HasFound = true;
							}

							m_MemoryHeapIndexVector[PTVD_MEMORYTYPE_BUFFER_STAGING] = MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex;
							m_MemoryTypeIndexVector[PTVD_MEMORYTYPE_BUFFER_STAGING] = MemoryTypeIndex;
							HostVector[PTVD_MEMORYTYPE_BUFFER_STAGING] = TargetPropertyFlagsAndBool.second;
							HeapFoundSize = MemoryProperties.memoryHeaps[MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex].size;

							if (HeapFoundSize >= s_BufferStaging_HeapSizeThreshold)
							{
								//Heap Is Big Enough To Use It Even If It Is The Special Pool Or The Second Small Pool
								break;
							}
						}
					}
				}
			}
		}

		if (!HasFound)
		{
			return false;
		}

		HostVector[PTVD_MEMORYTYPE_BUFFER_STAGING] = true;
	}

	//BufferConstant
	{
		VkBufferCreateInfo CreateInfo;
		CreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		CreateInfo.pNext = NULL;
		CreateInfo.flags = 0U;
		//******************************************************************************************************************
		CreateInfo.size = 256U;
		CreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		//******************************************************************************************************************
		CreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		CreateInfo.queueFamilyIndexCount = 0U;
		CreateInfo.pQueueFamilyIndices = NULL;
		//******************************************************************************************************************

		VkBuffer hBuffer;

		VkResult eResult = rDeviceWrapper.CreateBuffer(&CreateInfo, NULL, &hBuffer);
		assert(eResult == VK_SUCCESS);

		VkMemoryRequirements MemoryRequirements;
		rDeviceWrapper.GetBufferMemoryRequirements(hBuffer, &MemoryRequirements);

		rDeviceWrapper.DestroyBuffer(hBuffer, NULL);

		VkDeviceSize HeapFoundSize = VkDeviceSize(0U);
		bool HasFound = false;

		std::pair<VkMemoryPropertyFlags, bool> const TargetPropertyFlagsAndBoolVector[] = {
			{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true }, //For AMD Special Pool
			{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true },
			{ VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true },
			{ VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true },
		};

		for (std::pair<VkMemoryPropertyFlags, bool> const &TargetPropertyFlagsAndBool : TargetPropertyFlagsAndBoolVector)
		{
			if ((!HasFound) || (HasFound && (HeapFoundSize < s_BufferConstant_HeapSizeThreshold)))
			{
				for (uint32_t MemoryTypeIndex = 0U, MemoryTypeBit = 1U; MemoryTypeIndex < MemoryProperties.memoryTypeCount; ++MemoryTypeIndex, MemoryTypeBit <<= 1U)
				{
					if ((MemoryRequirements.memoryTypeBits&MemoryTypeBit) && (MemoryProperties.memoryTypes[MemoryTypeIndex].propertyFlags == TargetPropertyFlagsAndBool.first))
					{
						if ((!HasFound) || (HeapFoundSize < MemoryProperties.memoryHeaps[MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex].size))
						{
							if (!HasFound)
							{
								HasFound = true;
							}

							m_MemoryHeapIndexVector[PTVD_MEMORYTYPE_BUFFER_CONSTANT] = MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex;
							m_MemoryTypeIndexVector[PTVD_MEMORYTYPE_BUFFER_CONSTANT] = MemoryTypeIndex;
							HostVector[PTVD_MEMORYTYPE_BUFFER_CONSTANT] = TargetPropertyFlagsAndBool.second;
							HeapFoundSize = MemoryProperties.memoryHeaps[MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex].size;

							if (HeapFoundSize >= s_BufferConstant_HeapSizeThreshold)
							{
								//Heap Is Big Enough To Use It Even If It Is The Special Pool Or The Second Small Pool
								break;
							}
						}
					}
				}
			}
		}

		if (!HasFound)
		{
			return false;
		}

		HostVector[PTVD_MEMORYTYPE_BUFFER_CONSTANT] = true;
	}


	//BufferUAV_SRV
	{
		VkBufferCreateInfo CreateInfo;
		CreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		CreateInfo.pNext = NULL;
		CreateInfo.flags = 0U;
		//******************************************************************************************************************
		CreateInfo.size = 256U;
		//******************************************************************************************************************
		CreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
		//******************************************************************************************************************
		CreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		CreateInfo.queueFamilyIndexCount = 0U;
		CreateInfo.pQueueFamilyIndices = NULL;
		//******************************************************************************************************************

		VkBuffer hBuffer;

		VkResult eResult = rDeviceWrapper.CreateBuffer(&CreateInfo, NULL, &hBuffer);
		assert(eResult == VK_SUCCESS);

		VkMemoryRequirements MemoryRequirements;
		rDeviceWrapper.GetBufferMemoryRequirements(hBuffer, &MemoryRequirements);

		rDeviceWrapper.DestroyBuffer(hBuffer, NULL);

		VkDeviceSize HeapFoundSize = VkDeviceSize(0U);
		bool HasFound = false;

		std::pair<VkMemoryPropertyFlags, bool> const TargetPropertyFlagsAndBoolVector[] = {
			{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false },
			{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true },
			{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT, true },
			{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true }
		};

		for (std::pair<VkMemoryPropertyFlags, bool> const &TargetPropertyFlagsAndBool : TargetPropertyFlagsAndBoolVector)
		{
			if ((!HasFound) || (HasFound && (HeapFoundSize < s_BufferUnorderedAccess_HeapSizeThreshold)))
			{
				for (uint32_t MemoryTypeIndex = 0U, MemoryTypeBit = 1U; MemoryTypeIndex < MemoryProperties.memoryTypeCount; ++MemoryTypeIndex, MemoryTypeBit <<= 1U)
				{
					if ((MemoryRequirements.memoryTypeBits&MemoryTypeBit) && (MemoryProperties.memoryTypes[MemoryTypeIndex].propertyFlags == TargetPropertyFlagsAndBool.first))
					{
						if ((!HasFound) || (HeapFoundSize < MemoryProperties.memoryHeaps[MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex].size))
						{
							if (!HasFound)
							{
								HasFound = true;
							}

							m_MemoryHeapIndexVector[PTVD_MEMORYTYPE_BUFFER_UNORDEREDACCESS_SHADERRESOURCE] = MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex;
							m_MemoryTypeIndexVector[PTVD_MEMORYTYPE_BUFFER_UNORDEREDACCESS_SHADERRESOURCE] = MemoryTypeIndex;
							HostVector[PTVD_MEMORYTYPE_BUFFER_UNORDEREDACCESS_SHADERRESOURCE] = TargetPropertyFlagsAndBool.second;
							HeapFoundSize = MemoryProperties.memoryHeaps[MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex].size;

							if (HeapFoundSize >= s_BufferUnorderedAccess_HeapSizeThreshold)
							{
								//Heap Is Big Enough To Use It Even If It Is The Special Pool Or The Second Small Pool
								break;
							}
						}
					}
				}
			}
		}

		if (!HasFound)
		{
			return false;
		}
	}

	//BufferVertex
	{
		VkBufferCreateInfo CreateInfo;
		CreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		CreateInfo.pNext = NULL;
		//******************************************************************************************************************
		CreateInfo.flags = 0U;
		//******************************************************************************************************************
		CreateInfo.size = 256U;
		//******************************************************************************************************************
		CreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		//******************************************************************************************************************
		CreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		CreateInfo.queueFamilyIndexCount = 0U;
		CreateInfo.pQueueFamilyIndices = NULL;
		//******************************************************************************************************************

		VkBuffer hBuffer;

		VkResult eResult = rDeviceWrapper.CreateBuffer(&CreateInfo, NULL, &hBuffer);
		assert(eResult == VK_SUCCESS);

		VkMemoryRequirements MemoryRequirements;
		rDeviceWrapper.GetBufferMemoryRequirements(hBuffer, &MemoryRequirements);

		rDeviceWrapper.DestroyBuffer(hBuffer, NULL);

		VkDeviceSize HeapFoundSize = VkDeviceSize(0U);
		bool HasFound = false;

		std::pair<VkMemoryPropertyFlags, bool> const TargetPropertyFlagsAndBoolVector[] = {
			{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true }, //Zero-Copy Optimazation //We Need Deal With AMD Special Pool
			{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true },
			{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false }
			//{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT, true },
		};

		for (std::pair<VkMemoryPropertyFlags, bool> const &TargetPropertyFlagsAndBool : TargetPropertyFlagsAndBoolVector)
		{
			if ((!HasFound) || (HasFound && (HeapFoundSize < s_BufferVertex_HeapSizeThreshold)))
			{
				for (uint32_t MemoryTypeIndex = 0U, MemoryTypeBit = 1U; MemoryTypeIndex < MemoryProperties.memoryTypeCount; ++MemoryTypeIndex, MemoryTypeBit <<= 1U)
				{
					if ((MemoryRequirements.memoryTypeBits&MemoryTypeBit) && (MemoryProperties.memoryTypes[MemoryTypeIndex].propertyFlags == TargetPropertyFlagsAndBool.first))
					{
						if ((!HasFound) || (HeapFoundSize < MemoryProperties.memoryHeaps[MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex].size))
						{
							if (!HasFound)
							{
								HasFound = true;
							}

							m_MemoryHeapIndexVector[PTVD_MEMORYTYPE_BUFFER_VERTEX] = MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex;
							m_MemoryTypeIndexVector[PTVD_MEMORYTYPE_BUFFER_VERTEX] = MemoryTypeIndex;
							HostVector[PTVD_MEMORYTYPE_BUFFER_VERTEX] = TargetPropertyFlagsAndBool.second;
							HeapFoundSize = MemoryProperties.memoryHeaps[MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex].size;

							if (HeapFoundSize >= s_BufferVertex_HeapSizeThreshold)
							{
								//Heap Is Big Enough To Use It Even If It Is The Special Pool Or The Second Small Pool
								break;
							}
						}
					}
				}
			}
		}

		if (!HasFound)
		{
			return false;
		}
	}

	//BufferIndex
	{
		VkBufferCreateInfo CreateInfo;
		CreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		CreateInfo.pNext = NULL;
		CreateInfo.flags = 0U;
		//******************************************************************************************************************
		CreateInfo.size = 256U;
		//******************************************************************************************************************
		CreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		//******************************************************************************************************************
		CreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		CreateInfo.queueFamilyIndexCount = 0U;
		CreateInfo.pQueueFamilyIndices = NULL;
		//******************************************************************************************************************

		VkBuffer hBuffer;

		VkResult eResult = rDeviceWrapper.CreateBuffer(&CreateInfo, NULL, &hBuffer);
		assert(eResult == VK_SUCCESS);

		VkMemoryRequirements MemoryRequirements;
		rDeviceWrapper.GetBufferMemoryRequirements(hBuffer, &MemoryRequirements);

		rDeviceWrapper.DestroyBuffer(hBuffer, NULL);

		VkDeviceSize HeapFoundSize = VkDeviceSize(0U);
		bool HasFound = false;

		std::pair<VkMemoryPropertyFlags, bool> const TargetPropertyFlagsAndBoolVector[] = {
			{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true }, //Zero-Copy Optimazation //We Need Deal With AMD Special Pool
			{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true },
			{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false }
			//{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT, true },
		};

		for (std::pair<VkMemoryPropertyFlags, bool> const &TargetPropertyFlagsAndBool : TargetPropertyFlagsAndBoolVector)
		{
			if ((!HasFound) || (HasFound && (HeapFoundSize < s_BufferIndex_HeapSizeThreshold)))
			{
				for (uint32_t MemoryTypeIndex = 0U, MemoryTypeBit = 1U; MemoryTypeIndex < MemoryProperties.memoryTypeCount; ++MemoryTypeIndex, MemoryTypeBit <<= 1U)
				{
					if ((MemoryRequirements.memoryTypeBits&MemoryTypeBit) && (MemoryProperties.memoryTypes[MemoryTypeIndex].propertyFlags == TargetPropertyFlagsAndBool.first))
					{
						if ((!HasFound) || (HeapFoundSize < MemoryProperties.memoryHeaps[MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex].size))
						{
							if (!HasFound)
							{
								HasFound = true;
							}

							m_MemoryHeapIndexVector[PTVD_MEMORYTYPE_BUFFER_INDEX] = MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex;
							m_MemoryTypeIndexVector[PTVD_MEMORYTYPE_BUFFER_INDEX] = MemoryTypeIndex;
							HostVector[PTVD_MEMORYTYPE_BUFFER_INDEX] = TargetPropertyFlagsAndBool.second;
							HeapFoundSize = MemoryProperties.memoryHeaps[MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex].size;

							if (HeapFoundSize >= s_BufferIndex_HeapSizeThreshold)
							{
								//Heap Is Big Enough To Use It Even If It Is The Special Pool Or The Second Small Pool
								break;
							}
						}
					}
				}
			}
		}

		if (!HasFound)
		{
			return false;
		}
	}

	//If the memory requirements are for a VkImage, 
	//the memoryTypeBits member must not refer to a VkMemoryType with a propertyFlags that has the [VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT] bit set 
	//if the vkGetImageMemoryRequirements::image did not have [VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT] bit set in the [Usage] member of the VkImageCreateInfo structure passed to vkCreateImage.

	//For images created with a [color] format,
	//the memoryTypeBits member is [Identical] for all VkImage objects created 
	//with the [Same] combination of values for 
	//the [Tiling] member,
	//the [VK_IMAGE_CREATE_SPARSE_BINDING_BIT] bit of the [Flags] member,
	//and the [VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT] of the [Usage] member in the VkImageCreateInfo structure passed to vkCreateImage.

	//For images created with a [depth/stencil] format,
	//the memoryTypeBits member is [Identical] for all VkImage objects created
	//with the [Same] combination of values for 
	//the [Format] member, 
	//the [Tiling] member, 
	//the [VK_IMAGE_CREATE_SPARSE_BINDING_BIT] bit of the [Flags] member, 
	//and the [VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT] of the [Usage] member in the VkImageCreateInfo structure passed to vkCreateImage.				

	//If usage includes VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, 
	//then bits other than VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 
	//VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
	//and VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT must not be set. //We Should Use INPUT_ATTACHMENT_BIT Instead Of SAMPLED_BIT

	//Texture_RTV_SRV
	{
		VkImageCreateInfo CreateInfo;
		CreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		CreateInfo.pNext = NULL;
		CreateInfo.flags = 0U;
		CreateInfo.imageType = VK_IMAGE_TYPE_2D;
		CreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM; //Nothing To Do With [Format] For [Color] //We Use R8G8B8A8_UNORM To Make CreateImage Success
		//******************************************************************************************************************
		CreateInfo.extent.width = 256U;
		CreateInfo.extent.height = 256U;
		CreateInfo.extent.depth = 1U;
		CreateInfo.mip_levels = 1U;
		CreateInfo.array_layers = 1U;
		CreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		//******************************************************************************************************************
		CreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		CreateInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT; //Nothing To Do With [Bit other than TRANSIENT_ATTACHMENT] of [Usage]. //We Use COLOR_ATTACHMENT_BIT To Make CreateImage Success
		//******************************************************************************************************************
		CreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		CreateInfo.queueFamilyIndexCount = 0U;
		CreateInfo.pQueueFamilyIndices = NULL;
		CreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		//******************************************************************************************************************

		VkImageFormatProperties ImageFormatProperties;

		VkResult eResultFormat = rInstanceWrapper.GetPhysicalDeviceImageFormatProperties(
			hPhysicalDevice,
			CreateInfo.format,
			CreateInfo.imageType,
			CreateInfo.tiling,
			CreateInfo.usage,
			CreateInfo.flags,
			&ImageFormatProperties
		);
		assert(eResultFormat == VK_SUCCESS);

		assert(CreateInfo.extent.width <= ImageFormatProperties.maxExtent.width); //VkPhysicalDeviceLimits::maxImageDimension2D
		assert(CreateInfo.extent.height <= ImageFormatProperties.maxExtent.height); //VkPhysicalDeviceLimits::maxImageDimension2D
		assert(CreateInfo.extent.depth <= ImageFormatProperties.maxExtent.depth);
		assert(CreateInfo.mip_levels <= ImageFormatProperties.maxMipLevels);
		assert(CreateInfo.array_layers <= ImageFormatProperties.maxArrayLayers);
		assert(CreateInfo.samples&ImageFormatProperties.sampleCounts); //VkPhysicalDeviceLimits::framebufferColorSampleCounts

		VkImage hImage;

		VkResult eResultCreate = rDeviceWrapper.CreateImage(&CreateInfo, NULL, &hImage);
		assert(eResultCreate == VK_SUCCESS);

		VkMemoryRequirements MemoryRequirements;
		rDeviceWrapper.GetImageMemoryRequirements(hImage, &MemoryRequirements);

		rDeviceWrapper.DestroyImage(hImage, NULL);

		VkDeviceSize HeapFoundSize = VkDeviceSize(0U);
		bool HasFound = false;

		for (uint32_t MemoryTypeIndex = 0U, MemoryTypeBit = 1U; MemoryTypeIndex < MemoryProperties.memoryTypeCount; ++MemoryTypeIndex, MemoryTypeBit <<= 1U)
		{
			if (MemoryRequirements.memoryTypeBits&MemoryTypeBit)
			{
				if (MemoryProperties.memoryTypes[MemoryTypeIndex].propertyFlags == (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT))
				{
					if ((!HasFound) || (HeapFoundSize < MemoryProperties.memoryHeaps[MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex].size))
					{
						if (!HasFound)
						{
							HasFound = true;
						}

						m_MemoryHeapIndexVector[PTVD_MEMORYTYPE_TEXTURE_RENDERTARGET_SHADERRESOURCE] = MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex;
						m_MemoryTypeIndexVector[PTVD_MEMORYTYPE_TEXTURE_RENDERTARGET_SHADERRESOURCE] = MemoryTypeIndex;
						HostVector[PTVD_MEMORYTYPE_TEXTURE_RENDERTARGET_SHADERRESOURCE] = false;
						HeapFoundSize = MemoryProperties.memoryHeaps[MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex].size;

						if (HeapFoundSize >= s_TextureRenderTarget_HeapSizeThreshold)
						{
							//Heap Is Big Enough To Use It Even If It Is The Special Pool Or The Second Small Pool
							break;
						}
					}
				}
			}
		}

		if (!HasFound)
		{
			std::pair<VkMemoryPropertyFlags, bool> const TargetPropertyFlagsAndBoolVector[] = {
				{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false },
				{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true },
				{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT, true },
				{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true }
			};

			for (std::pair<VkMemoryPropertyFlags, bool> const &TargetPropertyFlagsAndBool : TargetPropertyFlagsAndBoolVector)
			{
				if ((!HasFound) || (HasFound && (HeapFoundSize < s_TextureRenderTarget_HeapSizeThreshold)))
				{
					for (uint32_t MemoryTypeIndex = 0U, MemoryTypeBit = 1U; MemoryTypeIndex < MemoryProperties.memoryTypeCount; ++MemoryTypeIndex, MemoryTypeBit <<= 1U)
					{
						if ((MemoryRequirements.memoryTypeBits&MemoryTypeBit) && (MemoryProperties.memoryTypes[MemoryTypeIndex].propertyFlags == TargetPropertyFlagsAndBool.first))
						{
							if ((!HasFound) || (HeapFoundSize < MemoryProperties.memoryHeaps[MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex].size))
							{
								if (!HasFound)
								{
									HasFound = true;
								}

								m_MemoryHeapIndexVector[PTVD_MEMORYTYPE_TEXTURE_RENDERTARGET_SHADERRESOURCE] = MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex;
								m_MemoryTypeIndexVector[PTVD_MEMORYTYPE_TEXTURE_RENDERTARGET_SHADERRESOURCE] = MemoryTypeIndex;
								HostVector[PTVD_MEMORYTYPE_TEXTURE_RENDERTARGET_SHADERRESOURCE] = TargetPropertyFlagsAndBool.second;
								HeapFoundSize = MemoryProperties.memoryHeaps[MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex].size;

								if (HeapFoundSize >= s_TextureRenderTarget_HeapSizeThreshold)
								{
									//Heap Is Big Enough To Use It Even If It Is The Special Pool Or The Second Small Pool
									break;
								}
							}
						}
					}
				}
			}
		}

		if (!HasFound)
		{
			return false;
		}
	}

	//Texture_DSV_SRV
	{
		VkFormat FormatFound = VK_FORMAT_UNDEFINED;
		VkDeviceSize HeapFoundSize = VkDeviceSize(0U);
		bool HasFound = false;
		bool IsLazilyAlllocated = false;

		VkFormat const TargetFormatVector[] = {
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_X8_D24_UNORM_PACK32,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM,
			VK_FORMAT_D16_UNORM_S8_UINT
		};

		std::pair<VkMemoryPropertyFlags, bool> const TargetPropertyFlagsAndBoolVector[] = {
			{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false	},
			{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true },
			{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT, true },
			{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true }
		};

		for (VkFormat const TargetFormat : TargetFormatVector)
		{
			VkImageCreateInfo CreateInfo;
			CreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			CreateInfo.pNext = NULL;
			CreateInfo.flags = 0U;
			CreateInfo.imageType = VK_IMAGE_TYPE_2D;
			CreateInfo.format = VK_FORMAT_D16_UNORM; //Be To Do With [Format] For [Depth/Stencil] 
			//******************************************************************************************************************
			CreateInfo.extent.width = 256U;
			CreateInfo.extent.height = 256U;
			CreateInfo.extent.depth = 1U;
			CreateInfo.mip_levels = 1U;
			CreateInfo.array_layers = 1U;
			CreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			//******************************************************************************************************************
			CreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			CreateInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT; //Although Nothing To Do With [Bit other than TRANSIENT_ATTACHMENT] of [Usage]. //We Need DEPTH_STENCIL_ATTACHMENT_BIT|SAMPLED_BIT To Choose Which Format To Use
			//******************************************************************************************************************
			CreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			CreateInfo.queueFamilyIndexCount = 0U;
			CreateInfo.pQueueFamilyIndices = NULL;
			CreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			//******************************************************************************************************************

			VkImageFormatProperties ImageFormatProperties;

			VkResult eResultFormat = rInstanceWrapper.GetPhysicalDeviceImageFormatProperties(
				hPhysicalDevice,
				CreateInfo.format,
				CreateInfo.imageType,
				CreateInfo.tiling,
				CreateInfo.usage,
				CreateInfo.flags,
				&ImageFormatProperties
			);
			if (eResultFormat != VK_SUCCESS)
			{
				assert(eResultFormat == VK_ERROR_FORMAT_NOT_SUPPORTED);
				continue;
			}

			assert(CreateInfo.extent.width <= ImageFormatProperties.maxExtent.width); //VkPhysicalDeviceLimits::maxImageDimension2D
			assert(CreateInfo.extent.height <= ImageFormatProperties.maxExtent.height); //VkPhysicalDeviceLimits::maxImageDimension2D
			assert(CreateInfo.extent.depth <= ImageFormatProperties.maxExtent.depth);
			assert(CreateInfo.mip_levels <= ImageFormatProperties.maxMipLevels);
			assert(CreateInfo.array_layers <= ImageFormatProperties.maxArrayLayers);
			assert(CreateInfo.samples&ImageFormatProperties.sampleCounts); //VkPhysicalDeviceLimits::framebufferDepthSampleCounts //VkPhysicalDeviceLimits::framebufferStencilSampleCounts

			VkImage hImage;

			VkResult eResultCreate = rDeviceWrapper.CreateImage(&CreateInfo, NULL, &hImage);
			assert(eResultCreate == VK_SUCCESS);

			VkMemoryRequirements MemoryRequirements;
			rDeviceWrapper.GetImageMemoryRequirements(hImage, &MemoryRequirements);

			rDeviceWrapper.DestroyImage(hImage, NULL);

			for (uint32_t MemoryTypeIndex = 0U, MemoryTypeBit = 1U; MemoryTypeIndex < MemoryProperties.memoryTypeCount; ++MemoryTypeIndex, MemoryTypeBit <<= 1U)
			{
				if (MemoryRequirements.memoryTypeBits&MemoryTypeBit)
				{
					if (MemoryProperties.memoryTypes[MemoryTypeIndex].propertyFlags == (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT))
					{
						if ((!HasFound) || (HeapFoundSize < MemoryProperties.memoryHeaps[MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex].size))
						{
							if (!HasFound)
							{
								HasFound = true;
							}

							m_MemoryHeapIndexVector[PTVD_MEMORYTYPE_TEXTURE_DEPTHSTENCIL_SHADERRESOURCE] = MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex;
							m_MemoryTypeIndexVector[PTVD_MEMORYTYPE_TEXTURE_DEPTHSTENCIL_SHADERRESOURCE] = MemoryTypeIndex;
							HostVector[PTVD_MEMORYTYPE_TEXTURE_DEPTHSTENCIL_SHADERRESOURCE] = false;
							HeapFoundSize = MemoryProperties.memoryHeaps[MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex].size;
							FormatFound = TargetFormat;
							IsLazilyAlllocated = true;

							if (HeapFoundSize >= s_TextureDepthStencil_HeapSizeThreshold)
							{
								//Heap Is Big Enough To Use It Even If It Is The Special Pool Or The Second Small Pool
								break;
							}
						}
					}
				}
			}

			if (IsLazilyAlllocated)
			{
				break;
			}

			for (std::pair<VkMemoryPropertyFlags, bool> const &TargetPropertyFlagsAndBool : TargetPropertyFlagsAndBoolVector)
			{
				if ((!HasFound) || (HasFound && (HeapFoundSize < s_TextureDepthStencil_HeapSizeThreshold)))
				{
					for (uint32_t MemoryTypeIndex = 0U, MemoryTypeBit = 1U; MemoryTypeIndex < MemoryProperties.memoryTypeCount; ++MemoryTypeIndex, MemoryTypeBit <<= 1U)
					{
						if ((MemoryRequirements.memoryTypeBits&MemoryTypeBit) && (MemoryProperties.memoryTypes[MemoryTypeIndex].propertyFlags == TargetPropertyFlagsAndBool.first))
						{
							if ((!HasFound) || (HeapFoundSize < MemoryProperties.memoryHeaps[MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex].size))
							{
								if (!HasFound)
								{
									HasFound = true;
								}

								m_MemoryHeapIndexVector[PTVD_MEMORYTYPE_TEXTURE_DEPTHSTENCIL_SHADERRESOURCE] = MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex;
								m_MemoryTypeIndexVector[PTVD_MEMORYTYPE_TEXTURE_DEPTHSTENCIL_SHADERRESOURCE] = MemoryTypeIndex;
								HostVector[PTVD_MEMORYTYPE_TEXTURE_DEPTHSTENCIL_SHADERRESOURCE] = TargetPropertyFlagsAndBool.second;
								HeapFoundSize = MemoryProperties.memoryHeaps[MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex].size;
								FormatFound = TargetFormat;

								if (HeapFoundSize >= s_TextureDepthStencil_HeapSizeThreshold)
								{
									//Heap Is Big Enough To Use It Even If It Is The Special Pool Or The Second Small Pool
									break;
								}
							}
						}
					}
				}
			}

		}

		if (!HasFound)
		{
			return false;
		}

		(*pTextureDS_SR_Format) = FormatFound;
	}

	//Texture_DSV
	{
		VkFormat FormatFound = VK_FORMAT_UNDEFINED;
		VkDeviceSize HeapFoundSize = VkDeviceSize(0U);
		bool HasFound = false;
		bool IsLazilyAlllocated = false;

		VkFormat const TargetFormatVector[] = {
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_X8_D24_UNORM_PACK32,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM,
			VK_FORMAT_D16_UNORM_S8_UINT
		};

		std::pair<VkMemoryPropertyFlags, bool> const TargetPropertyFlagsAndBoolVector[] = {
			{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false	},
			{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true },
			{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT, true },
			{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true }
		};

		for (VkFormat const TargetFormat : TargetFormatVector)
		{
			VkImageCreateInfo CreateInfo;
			CreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			CreateInfo.pNext = NULL;
			CreateInfo.flags = 0U;
			CreateInfo.imageType = VK_IMAGE_TYPE_2D;
			CreateInfo.format = VK_FORMAT_D16_UNORM; //Be To Do With [Format] For [Depth/Stencil] 
			//******************************************************************************************************************
			CreateInfo.extent.width = 256U;
			CreateInfo.extent.height = 256U;
			CreateInfo.extent.depth = 1U;
			CreateInfo.mip_levels = 1U;
			CreateInfo.array_layers = 1U;
			CreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			//******************************************************************************************************************
			CreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			CreateInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT /* VK_IMAGE_USAGE_SAMPLED_BIT //We No Need SRV */; //Although Nothing To Do With [Bit other than TRANSIENT_ATTACHMENT] of [Usage]. //We Need DEPTH_STENCIL_ATTACHMENT_BIT To Choose Which Format To Use
			//******************************************************************************************************************
			CreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			CreateInfo.queueFamilyIndexCount = 0U;
			CreateInfo.pQueueFamilyIndices = NULL;
			CreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			//******************************************************************************************************************

			VkImageFormatProperties ImageFormatProperties;

			VkResult eResultFormat = rInstanceWrapper.GetPhysicalDeviceImageFormatProperties(
				hPhysicalDevice,
				CreateInfo.format,
				CreateInfo.imageType,
				CreateInfo.tiling,
				CreateInfo.usage,
				CreateInfo.flags,
				&ImageFormatProperties
			);
			if (eResultFormat != VK_SUCCESS)
			{
				assert(eResultFormat == VK_ERROR_FORMAT_NOT_SUPPORTED);
				continue;
			}

			assert(CreateInfo.extent.width <= ImageFormatProperties.maxExtent.width); //VkPhysicalDeviceLimits::maxImageDimension2D
			assert(CreateInfo.extent.height <= ImageFormatProperties.maxExtent.height); //VkPhysicalDeviceLimits::maxImageDimension2D
			assert(CreateInfo.extent.depth <= ImageFormatProperties.maxExtent.depth);
			assert(CreateInfo.mip_levels <= ImageFormatProperties.maxMipLevels);
			assert(CreateInfo.array_layers <= ImageFormatProperties.maxArrayLayers);
			assert(CreateInfo.samples&ImageFormatProperties.sampleCounts); //VkPhysicalDeviceLimits::framebufferDepthSampleCounts //VkPhysicalDeviceLimits::framebufferStencilSampleCounts

			VkImage hImage;

			VkResult eResultCreate = rDeviceWrapper.CreateImage(&CreateInfo, NULL, &hImage);
			assert(eResultCreate == VK_SUCCESS);

			VkMemoryRequirements MemoryRequirements;
			rDeviceWrapper.GetImageMemoryRequirements(hImage, &MemoryRequirements);

			rDeviceWrapper.DestroyImage(hImage, NULL);

			for (uint32_t MemoryTypeIndex = 0U, MemoryTypeBit = 1U; MemoryTypeIndex < MemoryProperties.memoryTypeCount; ++MemoryTypeIndex, MemoryTypeBit <<= 1U)
			{
				if (MemoryRequirements.memoryTypeBits&MemoryTypeBit)
				{
					if (MemoryProperties.memoryTypes[MemoryTypeIndex].propertyFlags == (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT))
					{
						if ((!HasFound) || (HeapFoundSize < MemoryProperties.memoryHeaps[MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex].size))
						{
							if (!HasFound)
							{
								HasFound = true;
							}
							
							m_MemoryHeapIndexVector[PTVD_MEMORYTYPE_TEXTURE_DEPTHSTENCIL] = MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex;
							m_MemoryTypeIndexVector[PTVD_MEMORYTYPE_TEXTURE_DEPTHSTENCIL] = MemoryTypeIndex;
							HostVector[PTVD_MEMORYTYPE_TEXTURE_DEPTHSTENCIL] = false;
							HeapFoundSize = MemoryProperties.memoryHeaps[MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex].size;
							FormatFound = TargetFormat;
							IsLazilyAlllocated = true;

							if (HeapFoundSize >= s_TextureDepthStencil_HeapSizeThreshold)
							{
								//Heap Is Big Enough To Use It Even If It Is The Special Pool Or The Second Small Pool
								break;
							}
						}
					}
				}
			}

			if (IsLazilyAlllocated)
			{
				break;
			}

			for (std::pair<VkMemoryPropertyFlags, bool> const &TargetPropertyFlagsAndBool : TargetPropertyFlagsAndBoolVector)
			{
				if ((!HasFound) || (HasFound && (HeapFoundSize < s_TextureDepthStencil_HeapSizeThreshold)))
				{
					for (uint32_t MemoryTypeIndex = 0U, MemoryTypeBit = 1U; MemoryTypeIndex < MemoryProperties.memoryTypeCount; ++MemoryTypeIndex, MemoryTypeBit <<= 1U)
					{
						if ((MemoryRequirements.memoryTypeBits&MemoryTypeBit) && (MemoryProperties.memoryTypes[MemoryTypeIndex].propertyFlags == TargetPropertyFlagsAndBool.first))
						{
							if ((!HasFound) || (HeapFoundSize < MemoryProperties.memoryHeaps[MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex].size))
							{
								if (!HasFound)
								{
									HasFound = true;
								}

								m_MemoryHeapIndexVector[PTVD_MEMORYTYPE_TEXTURE_DEPTHSTENCIL] = MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex;
								m_MemoryTypeIndexVector[PTVD_MEMORYTYPE_TEXTURE_DEPTHSTENCIL] = MemoryTypeIndex;
								HostVector[PTVD_MEMORYTYPE_TEXTURE_DEPTHSTENCIL] = TargetPropertyFlagsAndBool.second;
								HeapFoundSize = MemoryProperties.memoryHeaps[MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex].size;
								FormatFound = TargetFormat;

								if (HeapFoundSize >= s_TextureDepthStencil_HeapSizeThreshold)
								{
									//Heap Is Big Enough To Use It Even If It Is The Special Pool Or The Second Small Pool
									break;
								}
							}
						}
					}
				}
			}
		}

		if (!HasFound)
		{
			return false;
		}

		(*pTextureDS_Format) = FormatFound;
	}



	//Texture_UAV_SRV
	{
		VkImageCreateInfo CreateInfo;
		CreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		CreateInfo.pNext = NULL;
		CreateInfo.flags = 0U;
		CreateInfo.imageType = VK_IMAGE_TYPE_2D;
		CreateInfo.format = VK_FORMAT_R32_SFLOAT; //Nothing To Do With [Format] For [Color] //We Use VK_FORMAT_R32_SFLOAT To Make CreateImage Success
		//******************************************************************************************************************
		CreateInfo.extent.width = 256U;
		CreateInfo.extent.height = 256U;
		CreateInfo.extent.depth = 1U;
		CreateInfo.mip_levels = 1U;
		CreateInfo.array_layers = 1U;
		CreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		//******************************************************************************************************************
		CreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		CreateInfo.usage = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT; /* We Can't Use Compute Shader In RenderPass //We No Need TRANSIENT */ //Nothing To Do With [Bit other than TRANSIENT_ATTACHMENT] of [Usage]. //We Use STORAGE_BIT|SAMPLED_BIT To Make CreateImage Success
		//******************************************************************************************************************
		CreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		CreateInfo.queueFamilyIndexCount = 0U;
		CreateInfo.pQueueFamilyIndices = NULL;
		CreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		//******************************************************************************************************************

		VkImageFormatProperties ImageFormatProperties;

		VkResult eResultFormat = rInstanceWrapper.GetPhysicalDeviceImageFormatProperties(
			hPhysicalDevice,
			CreateInfo.format,
			CreateInfo.imageType,
			CreateInfo.tiling,
			CreateInfo.usage,
			CreateInfo.flags,
			&ImageFormatProperties
		);
		assert(eResultFormat == VK_SUCCESS);

		assert(CreateInfo.extent.width <= ImageFormatProperties.maxExtent.width); //VkPhysicalDeviceLimits::maxImageDimension2D
		assert(CreateInfo.extent.height <= ImageFormatProperties.maxExtent.height); //VkPhysicalDeviceLimits::maxImageDimension2D
		assert(CreateInfo.extent.depth <= ImageFormatProperties.maxExtent.depth);
		assert(CreateInfo.mip_levels <= ImageFormatProperties.maxMipLevels);
		assert(CreateInfo.array_layers <= ImageFormatProperties.maxArrayLayers);
		assert(CreateInfo.samples&ImageFormatProperties.sampleCounts); //VkPhysicalDeviceLimits::framebufferColorSampleCounts

		VkImage hImage;

		VkResult eResultCreate = rDeviceWrapper.CreateImage(&CreateInfo, NULL, &hImage);
		assert(eResultCreate == VK_SUCCESS);

		VkMemoryRequirements MemoryRequirements;
		rDeviceWrapper.GetImageMemoryRequirements(hImage, &MemoryRequirements);

		rDeviceWrapper.DestroyImage(hImage, NULL);

		VkDeviceSize HeapFoundSize = VkDeviceSize(0U);
		bool HasFound = false;

		std::pair<VkMemoryPropertyFlags, bool> const TargetPropertyFlagsAndBoolVector[] = {
				{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false },
				{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true },
				{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT, true },
				{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true }
		};

		for (std::pair<VkMemoryPropertyFlags, bool> const &TargetPropertyFlagsAndBool : TargetPropertyFlagsAndBoolVector)
		{
			if ((!HasFound) || (HasFound && (HeapFoundSize < s_TextureUnorderedAccess_HeapSizeThreshold)))
			{
				for (uint32_t MemoryTypeIndex = 0U, MemoryTypeBit = 1U; MemoryTypeIndex < MemoryProperties.memoryTypeCount; ++MemoryTypeIndex, MemoryTypeBit <<= 1U)
				{
					if ((MemoryRequirements.memoryTypeBits&MemoryTypeBit) && (MemoryProperties.memoryTypes[MemoryTypeIndex].propertyFlags == TargetPropertyFlagsAndBool.first))
					{
						if ((!HasFound) || (HeapFoundSize < MemoryProperties.memoryHeaps[MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex].size))
						{
							if (!HasFound)
							{
								HasFound = true;
							}

							m_MemoryHeapIndexVector[PTVD_MEMORYTYPE_TEXTURE_UNORDEREDACCESS_SHADERRESOURCE] = MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex;
							m_MemoryTypeIndexVector[PTVD_MEMORYTYPE_TEXTURE_UNORDEREDACCESS_SHADERRESOURCE] = MemoryTypeIndex;
							HostVector[PTVD_MEMORYTYPE_TEXTURE_UNORDEREDACCESS_SHADERRESOURCE] = TargetPropertyFlagsAndBool.second;
							HeapFoundSize = MemoryProperties.memoryHeaps[MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex].size;

							if (HeapFoundSize >= s_TextureUnorderedAccess_HeapSizeThreshold)
							{
								//Heap Is Big Enough To Use It Even If It Is The Special Pool Or The Second Small Pool
								break;
							}
						}
					}
				}
			}
		}

		if (!HasFound)
		{
			return false;
		}
	}

	//Texture_SRV
	{
		VkImageCreateInfo CreateInfo;
		CreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		CreateInfo.pNext = NULL;
		CreateInfo.flags = 0U;
		CreateInfo.imageType = VK_IMAGE_TYPE_2D;
		CreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM; //Nothing To Do With [Format] For [Color] //We Use VK_FORMAT_R8G8B8A8_UNORM To Make CreateImage Success
		//******************************************************************************************************************
		CreateInfo.extent.width = 256U;
		CreateInfo.extent.height = 256U;
		CreateInfo.extent.depth = 1U;
		CreateInfo.mip_levels = 1U;
		CreateInfo.array_layers = 1U;
		CreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		//******************************************************************************************************************
		CreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		CreateInfo.usage =  VK_IMAGE_USAGE_SAMPLED_BIT; /* We Can't Use Compute Shader In RenderPass //We No Need TRANSIENT */ //Nothing To Do With [Bit other than TRANSIENT_ATTACHMENT] of [Usage]. //We Use SAMPLED_BIT To Make CreateImage Success
		//******************************************************************************************************************
		CreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		CreateInfo.queueFamilyIndexCount = 0U;
		CreateInfo.pQueueFamilyIndices = NULL;
		CreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		//******************************************************************************************************************

		VkImageFormatProperties ImageFormatProperties;

		VkResult eResultFormat = rInstanceWrapper.GetPhysicalDeviceImageFormatProperties(
			hPhysicalDevice,
			CreateInfo.format,
			CreateInfo.imageType,
			CreateInfo.tiling,
			CreateInfo.usage,
			CreateInfo.flags,
			&ImageFormatProperties
		);
		assert(eResultFormat == VK_SUCCESS);

		assert(CreateInfo.extent.width <= ImageFormatProperties.maxExtent.width); //VkPhysicalDeviceLimits::maxImageDimension2D
		assert(CreateInfo.extent.height <= ImageFormatProperties.maxExtent.height); //VkPhysicalDeviceLimits::maxImageDimension2D
		assert(CreateInfo.extent.depth <= ImageFormatProperties.maxExtent.depth);
		assert(CreateInfo.mip_levels <= ImageFormatProperties.maxMipLevels);
		assert(CreateInfo.array_layers <= ImageFormatProperties.maxArrayLayers);
		assert(CreateInfo.samples&ImageFormatProperties.sampleCounts); //VkPhysicalDeviceLimits::framebufferColorSampleCounts

		VkImage hImage;

		VkResult eResultCreate = rDeviceWrapper.CreateImage(&CreateInfo, NULL, &hImage);
		assert(eResultCreate == VK_SUCCESS);

		VkMemoryRequirements MemoryRequirements;
		rDeviceWrapper.GetImageMemoryRequirements(hImage, &MemoryRequirements);

		rDeviceWrapper.DestroyImage(hImage, NULL);

		VkDeviceSize HeapFoundSize = VkDeviceSize(0U);
		bool HasFound = false;

		std::pair<VkMemoryPropertyFlags, bool> const TargetPropertyFlagsAndBoolVector[] = {
				{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false },
				{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true },
				{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT, true },
				{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, true }
		};

		for (std::pair<VkMemoryPropertyFlags, bool> const &TargetPropertyFlagsAndBool : TargetPropertyFlagsAndBoolVector)
		{
			if ((!HasFound) || (HasFound && (HeapFoundSize < s_TextureShadowResource_HeapSizeThreshold)))
			{
				for (uint32_t MemoryTypeIndex = 0U, MemoryTypeBit = 1U; MemoryTypeIndex < MemoryProperties.memoryTypeCount; ++MemoryTypeIndex, MemoryTypeBit <<= 1U)
				{
					if ((MemoryRequirements.memoryTypeBits&MemoryTypeBit) && (MemoryProperties.memoryTypes[MemoryTypeIndex].propertyFlags == TargetPropertyFlagsAndBool.first))
					{
						if ((!HasFound) || (HeapFoundSize < MemoryProperties.memoryHeaps[MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex].size))
						{
							if (!HasFound)
							{
								HasFound = true;
							}

							m_MemoryHeapIndexVector[PTVD_MEMORYTYPE_TEXTURE_SHADERRESOURCE] = MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex;
							m_MemoryTypeIndexVector[PTVD_MEMORYTYPE_TEXTURE_SHADERRESOURCE] = MemoryTypeIndex;
							HostVector[PTVD_MEMORYTYPE_TEXTURE_SHADERRESOURCE] = TargetPropertyFlagsAndBool.second;
							HeapFoundSize = MemoryProperties.memoryHeaps[MemoryProperties.memoryTypes[MemoryTypeIndex].heapIndex].size;

							if (HeapFoundSize >= s_TextureShadowResource_HeapSizeThreshold)
							{
								//Heap Is Big Enough To Use It Even If It Is The Special Pool Or The Second Small Pool
								break;
							}
						}
					}
				}
			}
		}

		if (!HasFound)
		{
			return false;
		}
	}

	return true;
}

bool PTVD_DeviceMemoryManager::Alloc(VkDeviceWrapper &rDeviceWrapper, VkDeviceMemory *phMemory, VkDeviceSize *pBlockSize, void **ppBlockAddress, uint32_t MemoryType_PTVD)
{
	VkDeviceSize AllocationSize;
	{
		//ABA�������޺���???
		VkDeviceSize MemoryHeapSizeRemainOld;
		VkDeviceSize MemoryHeapSizeRemainNew;
		do
		{
			MemoryHeapSizeRemainOld = ::PTSAtomic_Get(&m_MemoryHeapSizeRemainVector[m_MemoryHeapIndexVector[MemoryType_PTVD]]);

			AllocationSize = (MemoryHeapSizeRemainOld >= s_Block_Size) ? s_Block_Size : PTVD_Size_AlignDownToPowerOfTwo(MemoryHeapSizeRemainOld);
			assert(MemoryHeapSizeRemainOld >= AllocationSize);

			MemoryHeapSizeRemainNew = MemoryHeapSizeRemainOld - AllocationSize;
		} while (::PTSAtomic_CompareAndSet(&m_MemoryHeapSizeRemainVector[m_MemoryHeapIndexVector[MemoryType_PTVD]], MemoryHeapSizeRemainOld, MemoryHeapSizeRemainNew) != MemoryHeapSizeRemainOld);

	}
	
	VkMemoryAllocateInfo AllocateInfo;
	AllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	AllocateInfo.pNext = NULL;
	AllocateInfo.allocationSize = AllocationSize;
	AllocateInfo.memoryTypeIndex = m_MemoryTypeIndexVector[MemoryType_PTVD];

	VkDeviceMemory hMemory;
	VkResult eResult = rDeviceWrapper.AllocateMemory(&AllocateInfo, NULL, &hMemory);
	//assert(eResult == VK_SUCCESS);
	if (eResult != VK_SUCCESS)
	{
		//����
		VkDeviceSize MemoryHeapSizeRemainOld;
		VkDeviceSize MemoryHeapSizeRemainNew;
		do{
			MemoryHeapSizeRemainOld = ::PTSAtomic_Get(&m_MemoryHeapSizeRemainVector[m_MemoryHeapIndexVector[MemoryType_PTVD]]);
			MemoryHeapSizeRemainNew = MemoryHeapSizeRemainOld + AllocationSize;
		} while (::PTSAtomic_CompareAndSet(&m_MemoryHeapSizeRemainVector[m_MemoryHeapIndexVector[MemoryType_PTVD]], MemoryHeapSizeRemainOld, MemoryHeapSizeRemainNew) != MemoryHeapSizeRemainOld);

		return false;
	}

	assert(phMemory != NULL);
	(*phMemory) = hMemory;

	assert(pBlockSize != NULL);
	(*pBlockSize) = AllocationSize;

	if (ppBlockAddress != NULL)
	{
		eResult = rDeviceWrapper.MapMemory(hMemory, 0U, VK_WHOLE_SIZE, 0U, ppBlockAddress);
		assert(eResult == VK_SUCCESS);
	}

	return true;
}

bool PTVD_DeviceMemoryManager::Free(VkDeviceWrapper &rDeviceWrapper, VkDeviceMemory hMemory, bool bUnmap)
{
	if (bUnmap)
	{
		rDeviceWrapper.UnmapMemory(hMemory);
	}

	rDeviceWrapper.FreeMemory(hMemory, NULL);

	return true;
}

#if defined PTWIN32
static inline VkDeviceSize PTVD_Size_AlignDownToPowerOfTwo(VkDeviceSize Value)
{
	VkDeviceSize ValueAligned;

	if (Value != VkDeviceSize(0U))
	{
		DWORD Index;
		BOOL wbResult = ::_BitScanReverse64(&Index, Value);
		assert(wbResult != FALSE);

		ValueAligned = (VkDeviceSize(1U) << VkDeviceSize(Index));
	}
	else
	{
		ValueAligned = 0U;
	}
	
	return ValueAligned;
}
#elif defined PTPOSIX
static inline VkDeviceSize PTVD_Size_AlignDownToPowerOfTwo(VkDeviceSize Value)
{
	VkDeviceSize ValueAligned;

	if (Value != VkDeviceSize(0U))
	{
		int Index = 63 - ::__builtin_clzl(Value);
		ValueAligned = (VkDeviceSize(1U) << VkDeviceSize(Index));
	}
	else
	{
		ValueAligned = 0U;
	}

	return ValueAligned;
}
#else
#error δ֪��ƽ̨
#endif


//NVIDIA 410/600/830M/840M API 1.0.3
//NVIDIA 840M API 1.0.5
//NVIDIA 940M API 1.0.4
//QueueFamily
//0 //GRAPHICS_BIT+COMPUTE_BIT+TRANSFER_BIT+SPARSE_BINDING_BIT //16

//NVIDIA 840M API 1.0.8/1.0.13/1.0.24
//NVIDIA 920M API 1.0.8/1.0.24
//NVIDIA 920MX API 1.0.13/1.0.24
//NVIDIA 930MX API 1.0.8
//NVIDIA 940MX API 1.0.47/1.0.42/1.0.46/1.0.49/1.0.56/1.0.65/1.1.70/1.1.84
//NVIDIA GeForce GT 630 API 1.0.56/1.0.65/1.1.70/1.1.73/1.1.82
//QueueFamily
//0 //GRAPHICS_BIT+COMPUTE_BIT+TRANSFER_BIT+SPARSE_BINDING_BIT //16
//QueueFamily
//1 //TRANSFER_BIT //1



//NVIDIA GeForce GT 1030 API 1.0.56/1.0.65/1.1.70/1.1.82/1.1.84
//QueueFamily
//0 //GRAPHICS_BIT+COMPUTE_BIT+TRANSFER_BIT+SPARSE_BINDING_BIT //16
//QueueFamily
//1 //TRANSFER_BIT //1
//QueueFamily
//2 //COMPUTE_BIT //8