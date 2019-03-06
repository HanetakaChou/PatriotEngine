#include "PTVDMemoryAllocator.h"
#include "../../Public/System/PTSThread.h"
#include "../../Public/System/PTSMemoryAllocator.h"
#include "../../Public/System/PTSMemory.h"
#include <type_traits>
#include <new>

#if defined(PTVDVK)
#include "VK/PTVDVKMemoryAllocator.h"
#elif defined(PTVDD3D12)

#else
#error 未知的图形API
#endif

#if defined(PTARM) || defined(PTARM64) || defined(PTX86) || defined(PTX64)
static constexpr uint32_t const s_CacheLine_Size = 64U;
#else
#error 未知的架构
#endif

static constexpr uint32_t const s_ThresholdSizeVector[] = {
	1024U * 1024U * 1U,   //4MB   //PTVD_MEMORYTYPE_BUFFER_STAGING
	1024U * 1024U * 1U,   //4MB   //PTVD_MEMORYTYPE_BUFFER_CONSTANT
	1024U * 1024U * 1U,   //4MB   //PTVD_MEMORYTYPE_BUFFER_UNORDEREDACCESS_SHADERRESOURCE
	1024U * 1024U * 8U,   //4MB   //PTVD_MEMORYTYPE_BUFFER_VERTEX
	1024U * 1024U * 8U,   //4MB   //PTVD_MEMORYTYPE_BUFFER_INDEX
	1024U * 1024U * 4U,   //4MB   //PTVD_MEMORYTYPE_TEXTURE_RENDERTARGET_SHADERRESOURCE //Transient
	//                    //      //PTVD_MEMORYTYPE_TEXTURE_RENDERTARGET //No Need This Type. We Can Write To SwapChain Directly
	1024U * 1024U * 4U,   //4MB   //PTVD_MEMORYTYPE_TEXTURE_DEPTHSTENCIL_SHADERRESOURCE //Transient
	1024U * 1024U * 4U,   //4MB   //PTVD_MEMORYTYPE_TEXTURE_DEPTHSTENCIL //Transient
	1024U * 1024U * 4U,   //4MB   //PTVD_MEMORYTYPE_TEXTURE_UNORDEREDACCESS_SHADERRESOURCE //No Need Transient. We Can't Use ComputeShader In RenderPass.
	1024U * 1024U * 8U    //8MB   //PTVD_MEMORYTYPE_TEXTURE_SHADERRESOURCE
};
static_assert((sizeof(s_ThresholdSizeVector) / sizeof(s_ThresholdSizeVector[0])) == PTVD_MEMORYTYPE_COUNT, "Size Of s_BlockSizeVector Is Not Correct");

static constexpr uint32_t const s_BucketNumberVector[] = {
	1U,
	1U,
	1U,
	32U,
	32U,
	32U,
	32U,
	32U,
	32U,
	26U
};
static_assert((sizeof(s_BucketNumberVector) / sizeof(s_BucketNumberVector[0])) == PTVD_MEMORYTYPE_COUNT, "Size Of BucketNumberVector Is Not Correct");

static inline void PTVD_ResolveRequestSize_BufferStaging(uint32_t *pBucketIndex, uint32_t*pBucketSize, uint32_t *pObjectSize, uint32_t RequestSize);
static inline void PTVD_ResolveRequestSize_BufferConstant(uint32_t *pBucketIndex, uint32_t*pBucketSize, uint32_t *pObjectSize, uint32_t RequestSize);
static inline void PTVD_ResolveRequestSize_BufferUnorderedAccess_ShaderResource(uint32_t *pBucketIndex, uint32_t*pBucketSize, uint32_t *pObjectSize, uint32_t RequestSize);
static inline void PTVD_ResolveRequestSize_BufferVertex(uint32_t *pBucketIndex, uint32_t*pBucketSize, uint32_t *pObjectSize, uint32_t RequestSize);
static inline void PTVD_ResolveRequestSize_BufferIndex(uint32_t *pBucketIndex, uint32_t*pBucketSize, uint32_t *pObjectSize, uint32_t RequestSize);
static inline void PTVD_ResolveRequestSize_TextureRenderTarget_ShaderResource(uint32_t *pBucketIndex, uint32_t*pBucketSize, uint32_t *pObjectSize, uint32_t RequestSize);
static inline void PTVD_ResolveRequestSize_TextureDepthStencil_ShaderResource(uint32_t *pBucketIndex, uint32_t*pBucketSize, uint32_t *pObjectSize, uint32_t RequestSize);
static inline void PTVD_ResolveRequestSize_TextureDepthStencil(uint32_t *pBucketIndex, uint32_t*pBucketSize, uint32_t *pObjectSize, uint32_t RequestSize);
static inline void PTVD_ResolveRequestSize_TextureUnorderedAccess_ShaderResource(uint32_t *pBucketIndex, uint32_t*pBucketSize, uint32_t *pObjectSize, uint32_t RequestSize);
static inline void PTVD_ResolveRequestSize_TextureShaderResource(uint32_t *pBucketIndex, uint32_t*pBucketSize, uint32_t *pObjectSize, uint32_t RequestSize);

static constexpr void(*(s_ResolveRequestSizeVector[]))(uint32_t *pBucketIndex, uint32_t*pBucketSize, uint32_t *pObjectSize, uint32_t RequestSize) = {
	PTVD_ResolveRequestSize_BufferStaging,
	PTVD_ResolveRequestSize_BufferConstant,
	PTVD_ResolveRequestSize_BufferUnorderedAccess_ShaderResource,
	PTVD_ResolveRequestSize_BufferVertex,
	PTVD_ResolveRequestSize_BufferIndex,
	PTVD_ResolveRequestSize_TextureRenderTarget_ShaderResource,
	PTVD_ResolveRequestSize_TextureDepthStencil_ShaderResource,
	PTVD_ResolveRequestSize_TextureDepthStencil,
	PTVD_ResolveRequestSize_TextureUnorderedAccess_ShaderResource,
	PTVD_ResolveRequestSize_TextureShaderResource,
};


static inline uint32_t PTS_Size_BitScanReverse(uint32_t Value)
{
	DWORD Index;
	BOOL wbResult = ::_BitScanReverse(&Index, Value);
	assert(wbResult != FALSE);
	return Index;
}

static inline uint32_t PTS_Size_AlignUpFrom(uint32_t Value, uint32_t Alignment)
{
	return ((Value - 1U) | (Alignment - 1U)) + 1U;
}

//Alignment
//Buffer不小于256B
//D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT 256
//Texture不小于4096B
//D3D12_SMALL_RESOURCE_PLACEMENT_ALIGNMENT 4096

//D3D12 Use Overwrite //Alloc A Constant Buffer Bigger Than All Objects In The Scene Consume

//maxMemoryAllocationCount >= 4096
//Even 1MB One Block Not Exceed

//256
//512
//768
//1024

static inline void PTVD_ResolveRequestSize_BufferStaging(uint32_t *pBucketIndex, uint32_t*pBucketSize, uint32_t *pObjectSize, uint32_t RequestSize)
{
	//Buffer_Staging Is Too Small!
	//You May Need To Improve Your Usage!
	assert(0);
}

static inline void PTVD_ResolveRequestSize_BufferConstant(uint32_t *pBucketIndex, uint32_t*pBucketSize, uint32_t *pObjectSize, uint32_t RequestSize)
{
	//We Don't Need To Write For Static Object?

	//Buffer_Constant Is Too Small!
	//You May Need To Improve Your Usage!
	assert(0);
}

static inline void PTVD_ResolveRequestSize_BufferUnorderedAccess_ShaderResource(uint32_t *pBucketIndex, uint32_t*pBucketSize, uint32_t *pObjectSize, uint32_t RequestSize)
{
	//Buffer_UnorderedAccess Is Too Small!
	//You May Need To Improve Your Usage!
	assert(0);
}

static inline void PTVD_ResolveRequestSize_BufferVertex(uint32_t *pBucketIndex, uint32_t*pBucketSize, uint32_t *pObjectSize, uint32_t RequestSize)
{
	//Use VBV??? //由外部决定

	//4M
	//  0.5K
	//  1K
	//  2K
	//0 4K
	//1 8K
	//Part2↓------------------------------
	//2 12K
	//3 16K
	//4 24K
	//5 32K
	//6 48K
	//7 64K

	//16M
	//  96K
	//9 128K
	//Part2↓---------------------------------------------
	//10 192K
	//11 256K

	//64M
	//Part2↓--------------------------------------------
	//12 384K
	//13 512K
	//14 768K
	//15 1024K
	//Warning--------------------------------------------
	//17 4096K
}

static inline void PTVD_ResolveRequestSize_BufferIndex(uint32_t *pBucketIndex, uint32_t*pBucketSize, uint32_t *pObjectSize, uint32_t RequestSize)
{

}

static inline void PTVD_ResolveRequestSize_TextureRenderTarget_ShaderResource(uint32_t *pBucketIndex, uint32_t*pBucketSize, uint32_t *pObjectSize, uint32_t RequestSize)
{

}

static inline void PTVD_ResolveRequestSize_TextureDepthStencil_ShaderResource(uint32_t *pBucketIndex, uint32_t*pBucketSize, uint32_t *pObjectSize, uint32_t RequestSize)
{

}

static inline void PTVD_ResolveRequestSize_TextureDepthStencil(uint32_t *pBucketIndex, uint32_t*pBucketSize, uint32_t *pObjectSize, uint32_t RequestSize)
{

}

static inline void PTVD_ResolveRequestSize_TextureUnorderedAccess_ShaderResource(uint32_t *pBucketIndex, uint32_t*pBucketSize, uint32_t *pObjectSize, uint32_t RequestSize)
{

}

static inline void PTVD_ResolveRequestSize_TextureShaderResource(uint32_t *pBucketIndex, uint32_t*pBucketSize, uint32_t *pObjectSize, uint32_t RequestSize)
{
	assert(pBucketIndex != NULL);
	assert(pBucketSize != NULL);
	assert(pObjectSize != NULL);

	//Bucket Allocator 是可行的
	//游戏中的Asset的大小大多数是相同的

	//4M
	//0 4K
	//1 8K
	//Part2↓------------------------------
	//2 12K
	//3 16K
	//4 24K
	//5 32K
	//6 48K
	//7 64K
	//8 96K
	//9 128K

	//16M
	//10 192K
	//11 256K
	//12 384K
	//13 512K
	//14 768K
	//15 1024K
	//16 1536K
	//17 2048K

	//64M
	//Part3↓-----------------------------
	//18 2560K
	//19 3072K
	//20 3584K
	//21 4096K
	//22 5120K
	//23 6144K
	//24 7168K
	//25 8192K

#if 0
	//PC Only ???
	//256M
	//-----------------------------
	//18 10M
	//19 12M
	//20 14M
	//21 16M
	//22 20M
	//23 24M
	//24 28M
	//25 32M
#endif
	assert(RequestSize > 0U);

	static const uint32_t s_Part1_BucketNumber = 2U;
	static const uint32_t s_Part1_ObjectSizeTotal = 1024U * 8U;
	static const uint32_t s_Part1_ObjectSizeTotal_Order = 13U;
	assert(s_Part1_ObjectSizeTotal == (1U << s_Part1_ObjectSizeTotal_Order));

	static const uint32_t s_Part2_BucketNumber = 18U;
	static const uint32_t s_Part2_ObjectSizeTotal = 1024U * 2048U;
	static const uint32_t s_Part2_ObjectSizeTotal_Order = 21U;
	static const uint32_t s_Part2_GroupSize = 2U;
	static const uint32_t s_Part2_GroupSize_Order = 1U;
	assert(s_Part2_ObjectSizeTotal == (1U << s_Part2_ObjectSizeTotal_Order));
	assert(s_Part2_GroupSize == (1U << s_Part2_GroupSize_Order));

	static const uint32_t s_Part3_BucketNumber = 26U;
	static const uint32_t s_Part3_ObjectSizeTotal = 1024U * 8192U;
	static const uint32_t s_Part3_ObjectSizeTotal_Order = 23U;
	static const uint32_t s_Part3_GroupSize = 4U;
	static const uint32_t s_Part3_GroupSize_Order = 2U;
	assert(s_Part3_ObjectSizeTotal == (1U << s_Part3_ObjectSizeTotal_Order));
	assert(s_Part3_GroupSize == (1U << s_Part3_GroupSize_Order));

	if (RequestSize <= s_Part1_ObjectSizeTotal)
	{
		static uint32_t const s_IndexArray[] = { 0U,1U };
		assert((sizeof(s_IndexArray) / sizeof(s_IndexArray[0])) == s_Part1_BucketNumber);
		(*pBucketIndex) = s_IndexArray[(RequestSize - 1U) >> 12U];

		static uint32_t const s_SizeArray[] = { 1024U * 4U,1024U * 8U };
		assert((sizeof(s_SizeArray) / sizeof(s_SizeArray[0])) == s_Part1_BucketNumber);
		assert(s_SizeArray[1] == s_Part1_ObjectSizeTotal);
		(*pObjectSize) = s_SizeArray[(*pBucketIndex)];

		(*pBucketSize) = 1024U * 1024U * 4U; //4M
	}
	else if (RequestSize <= s_Part2_ObjectSizeTotal)
	{
		uint32_t Order = ::PTS_Size_BitScanReverse(RequestSize - 1U);
		assert(Order >= s_Part1_ObjectSizeTotal_Order && Order < s_Part2_ObjectSizeTotal_Order);

		(*pBucketIndex) = s_Part1_BucketNumber
			+ s_Part2_GroupSize * (Order - s_Part1_ObjectSizeTotal_Order)
			+ (((RequestSize - 1U) >> (Order - s_Part2_GroupSize_Order)) - s_Part2_GroupSize);

		uint32_t AlignmentMin = 1U << (Order - s_Part2_GroupSize_Order);
		(*pObjectSize) = ::PTS_Size_AlignUpFrom(RequestSize, AlignmentMin);

		if ((*pBucketIndex) < 10U)
		{
			(*pBucketSize) = 1024U * 1024U * 4U; //4M
		}
		else
		{
			assert(Order < s_Part2_ObjectSizeTotal_Order);
			(*pBucketSize) = 1024U * 1024U * 16U; //16M
		}
	}
	else if (RequestSize <= s_Part3_ObjectSizeTotal)
	{
		uint32_t Order = ::PTS_Size_BitScanReverse(RequestSize - 1U);
		assert(Order >= s_Part2_ObjectSizeTotal_Order && Order < s_Part3_ObjectSizeTotal_Order);

		(*pBucketIndex) = s_Part2_BucketNumber
			+ s_Part3_GroupSize * (Order - s_Part2_ObjectSizeTotal_Order)
			+ (((RequestSize - 1U) >> (Order - s_Part3_GroupSize_Order)) - s_Part3_GroupSize);

		uint32_t AlignmentMin = 1U << (Order - s_Part3_GroupSize_Order);
		(*pObjectSize) = ::PTS_Size_AlignUpFrom(RequestSize, AlignmentMin);
	}
	else
	{
		//>8096K 直接分配
		assert(0);
	}

	assert(s_ThresholdSizeVector[PTVD_MEMORYTYPE_TEXTURE_SHADERRESOURCE] == s_Part3_ObjectSizeTotal);
	assert(s_BucketNumberVector[PTVD_MEMORYTYPE_TEXTURE_SHADERRESOURCE] == s_Part3_BucketNumber);
}

#if 0
static inline void PTS_Size_ResolveRequestSize(uint32_t *pBinIndex, uint32_t *pObjectSize, uint32_t RequestSize)
{


	static uint32_t const s_PartI_SizeMax = 64U;
	static uint32_t const s_PartI_BinNumber = 5U;

	if (RequestSize <= s_PartI_SizeMax)
	{
		//[8/16/24/32/40/48/56/64] -> [8/16/32/48/64]
		static uint32_t const s_IndexArray[8] = { 0U,1U,2U,2U,3U,3U,4U,4U };
		(*pBinIndex) = s_IndexArray[(RequestSize - 1U) >> 3U];

		static uint32_t const s_SizeArray[5] = { 8U,16U,32U,48U,64U };
		(*pObjectSize) = s_SizeArray[(*pBinIndex)];
	}
	else if (RequestSize <= s_NonLargeObject_SizeMax)
	{
			                                                                                                                                              //
		                                                                                                                           //64                64  1000000
		                                                                                                                           //80    01010000
		                                                                                                                           //96    01100000‬
		                                                                                                                           //112   01110000‬
		                                                                                                         //128               128   10000000
		                                                                                                         //160   010100000‬
		                                                                                                         //192   ‭011000000‬
		                                                                                                         //224   011100000   
	                                                                                          //256                256   100000000                   
		                                                                                      //320   0101000000
		                                                                                      //384   ‭0110000000‬
						                                                                      //448   ‭0111000000‬
						                                                  //512                 512   1000000000
						                                                  //640   01010000000
						                                                  //768   01100000000
		                                                                  //896   01110000000
		                                             //1024                 1024  10000000000
		                                             //1280  010100000000
		                                             //1536  011000000000
		                                             //1792  011100000000
		                       //2048                  2048  100000000000
		                       //2560  0101000000000
		                       //3072  0110000000000
		                       //3584  0111000000000
		//4096                   4096  1000000000000
		//5120  01010000000000
		//6144  01100000000000
		//7168  01110000000000
		//8192  10000000000000

		uint32_t Order = ::PTS_Size_BitScanReverse(RequestSize - 1U);
		assert(Order >= 6U && Order <= 12U);

		(*pBinIndex) = s_PartI_BinNumber +
			4 * (Order - 6U) +  // 80/96/112/128 -> 4*(6-6)=0 || 60/192/224/256 -> 4*(7-6)=4 || 320/384/448/512 -> 4*(8-6)=8 || 640/768/896/1024 -> 4*(9-6)=12 || 1280/1536/1792/2048 -> 4*(10-6)=16 || 2560/3072/3584/4096 -> 4*(11-6)=20 || 5120/6144/7168/8192 -> 4*(12-6)=24
			((RequestSize - 1U) >> (Order - 2U)) - 4U; //0100 -> 4-4=0 || 0101 -> 5-4=1 || 0110 -> 6-4=2 || 0111 -> 7-4=3
		
		uint32_t AlignmentMin = 1024U >> (12U - Order);
		assert(AlignmentMin == 16U || AlignmentMin == 32U || AlignmentMin == 64U || AlignmentMin == 128U || AlignmentMin == 256U || AlignmentMin == 512U || AlignmentMin == 1024U);
		(*pObjectSize) = ::PTS_Size_AlignUpFrom(RequestSize, AlignmentMin);
	}
	else
	{
		assert(RequestSize <= s_NonLargeObject_SizeMax);
		(*pBinIndex) = ~uint32_t(0U);
		(*pObjectSize) = ~uint32_t(0U);
	}
}
#endif

struct PTVD_MemoryObject;
struct PTVD_MemoryBlock;
class PTVD_ThreadLocalAllocator;

class PTVD_BlockManager
{
	PTVD_MemoryBlock **m_BlockNonEmptyVector;//BucketNumber
	PTVD_MemoryBlock *m_QueueEmptyTop;
public:
	inline void PushNonEmpty(uint32_t BucketIndex_InAllocator, PTVD_MemoryBlock *pBlockToPush);

	inline PTVD_MemoryBlock *PopNonEmpty(PTVD_ThreadLocalAllocator *pAllocator_Owner, uint32_t BinIndex, uint32_t ObjectSize/*For Validation*/);

	inline PTVD_MemoryBlock *PopEmpty(PTVD_ThreadLocalAllocator *pAllocator_Owner, uint32_t BinIndex, uint32_t ObjectSize);
};

struct IPTVD_MemoryObject
{
#if defined(PTVDVK)
	virtual VkDeviceMemory Memory() const;
	virtual VkBuffer Buffer() const;  //For BufferVertex And BufferIndex
	virtual VkDeviceSize MemoryOffset() const;
#ifndef NDEBUG
	virtual VkDeviceSize MemorySize() const;
#endif
#elif defined(PTVDD3D12)

#else
#error 未知的图形API
#endif
};


struct PTVD_MemoryObject
{
	//Metadata
	//Header
private:
	PTVD_MemoryObject *m_pNext_InBlock;
	PTVD_MemoryBlock *m_pBlock_Owner;
	//We Don't Need VkDeviceSize m_MemoryOffset //We Can Calulate MemoryOffset By "(this - m_pBlock_Owner->m_ObjectVector)*m_pBlock_Owner->m_Object_Size"
public:
#if defined(PTVDVK)
	inline VkDeviceMemory Memory() const;
	inline VkBuffer Buffer() const;  //For BufferVertex And BufferIndex
	inline VkDeviceSize MemoryOffset() const;
#ifndef NDEBUG
	inline VkDeviceSize MemorySize() const;
#endif
#elif defined(PTVDD3D12)

#else
#error 未知的图形API
#endif
};
static_assert(std::is_pod<PTVD_MemoryObject>::value, "PTVD_MemoryObject Is Not A POD");

struct PTVD_MemoryBlock
{
	//Metadata
	//Header
private:
	//Public域
	PTVD_MemoryBlock *m_pNext_InBlockManager;
	PTVD_MemoryObject *m_ObjectFreeVector_Public; //ObjectVector=FreeList

	//To Avoid False Sharing，将可能被Foreign线程访问的Public域和只能被Owner线程访问的Private域分隔在不同的缓存行中
	uint8_t __PaddingForPublicFields[s_CacheLine_Size - sizeof(void*) * 2U];

	PTVD_ThreadLocalAllocator *m_pAllocator_Owner;//NULL Means The "m_ThreadID_Owner" Is Invalid ，同时 在缓存TLS后 PTS_Internal_Free中不再需要调用PTSTSD_GetValue
	
	PTVD_MemoryBlock *m_pNext_InBucket; //Full Block //Bucket=BlockDoublyLinkedList
	PTVD_MemoryBlock *m_pPrevious_InBucket; //Empty Enough Block //Bucket=BlockDoublyLinkedList

	PTVD_MemoryObject *m_ObjectFreeVector_Private; //ObjectVector=FreeList

	PTVD_MemoryObject *m_ObjectVector; //Vector For All Object

#if defined(PTVDVK)
	VkDeviceMemory m_hMemory;
	VkBuffer m_hBuffer; //For BufferVertex And BufferIndex
	VkDeviceSize m_ObjectSize;
#ifndef NDEBUG
	VkDeviceSize m_BlockSize; //For Debug Use
#endif
	VkDeviceSize m_BumpMemoryOffset;
	friend VkDeviceMemory PTVD_MemoryObject::Memory() const;
	friend VkBuffer PTVD_MemoryObject::Buffer() const;
	friend VkDeviceSize PTVD_MemoryObject::MemoryOffset() const;
	friend VkDeviceSize PTVD_MemoryObject::MemorySize() const;
#elif defined(PTVDD3D12)

#else
#error 未知的图形API
#endif

	PTSThreadID m_ThreadID_Owner; //Valid Only When "m_pAllocator_Owner" Is Not NULL
	
	uint32_t m_BucketIndex_InAllocator;
	
	uint32_t m_ObjectAllocated_Number;

	//To Fill In Cache Line
	//uint8_t __PaddingForPrivateFields[s_CacheLine_Size - sizeof(uint32_t) * 4U - sizeof(PTSThreadID) - sizeof(void*) * 4U - sizeof(void*)];

public:
	static inline constexpr bool StaticAssertPadding()
	{
		return (offsetof(PTVD_MemoryBlock, m_pAllocator_Owner) == s_CacheLine_Size);
	}

#if defined(PTVDVK)
	inline void Construct(PTVD_ThreadLocalAllocator *pAllocator_Owner, VkDeviceMemory hMemory, VkDeviceSize BlockSize, VkDeviceSize ObjectSize, uint32_t BucketIndex_InAllocator);
#elif defined(PTVDD3D12)

#else
#error 未知的图形API
#endif

};
static_assert(std::is_pod<PTVD_MemoryBlock>::value, "PTVD_MemoryBlock Is Not A POD");
static_assert(PTVD_MemoryBlock::StaticAssertPadding(), "Padding Of PTVD_MemoryBlock Is Not Correct");

template<uint32_t const MemoryType_PTVD>
class PTVD_BucketAllocator
{
	PTVD_MemoryBlock *m_BucketVector[s_BucketNumberVector[MemoryType_PTVD]]; //Bucket=BlockDoublyLinkedList //BucketNumber
public:
	inline void Construct();
	inline PTVD_MemoryObject *Alloc(uint32_t Size);
	inline void Destruct();
};
static_assert(std::is_pod<PTVD_BucketAllocator<PTVD_MEMORYTYPE_BUFFER_STAGING>>::value, "PTVD_BucketAllocator Is Not A POD");
static_assert(std::is_pod<PTVD_BucketAllocator<PTVD_MEMORYTYPE_BUFFER_CONSTANT>>::value, "PTVD_BucketAllocator Is Not A POD");
static_assert(std::is_pod<PTVD_BucketAllocator<PTVD_MEMORYTYPE_BUFFER_UNORDEREDACCESS_SHADERRESOURCE>>::value, "PTVD_BucketAllocator Is Not A POD");
static_assert(std::is_pod<PTVD_BucketAllocator<PTVD_MEMORYTYPE_BUFFER_VERTEX>>::value, "PTVD_BucketAllocator Is Not A POD");
static_assert(std::is_pod<PTVD_BucketAllocator<PTVD_MEMORYTYPE_BUFFER_INDEX>>::value, "PTVD_BucketAllocator Is Not A POD");
static_assert(std::is_pod<PTVD_BucketAllocator<PTVD_MEMORYTYPE_TEXTURE_RENDERTARGET_SHADERRESOURCE>>::value, "PTVD_BucketAllocator Is Not A POD");
static_assert(std::is_pod<PTVD_BucketAllocator<PTVD_MEMORYTYPE_TEXTURE_DEPTHSTENCIL_SHADERRESOURCE>>::value, "PTVD_BucketAllocator Is Not A POD");
static_assert(std::is_pod<PTVD_BucketAllocator<PTVD_MEMORYTYPE_TEXTURE_DEPTHSTENCIL>>::value, "PTVD_BucketAllocator Is Not A POD");
static_assert(std::is_pod<PTVD_BucketAllocator<PTVD_MEMORYTYPE_TEXTURE_UNORDEREDACCESS_SHADERRESOURCE>>::value, "PTVD_BucketAllocator Is Not A POD");
static_assert(std::is_pod<PTVD_BucketAllocator<PTVD_MEMORYTYPE_TEXTURE_SHADERRESOURCE>>::value, "PTVD_BucketAllocator Is Not A POD");

class PTVD_ThreadLocalRouter
{
	PTVD_BucketAllocator<PTVD_MEMORYTYPE_BUFFER_STAGING> m_Bucket_BufferStaging;
	PTVD_BucketAllocator<PTVD_MEMORYTYPE_BUFFER_CONSTANT> m_Bucket_BufferConstant;
	PTVD_BucketAllocator<PTVD_MEMORYTYPE_BUFFER_UNORDEREDACCESS_SHADERRESOURCE> m_Bucket_BufferUnorderedAccess_ShaderResource;
	PTVD_BucketAllocator<PTVD_MEMORYTYPE_BUFFER_VERTEX> m_Bucket_BufferVertex;
	PTVD_BucketAllocator<PTVD_MEMORYTYPE_BUFFER_INDEX> m_Bucket_BufferIndex;
	PTVD_BucketAllocator<PTVD_MEMORYTYPE_TEXTURE_RENDERTARGET_SHADERRESOURCE> m_Bucket_TextureRenderTarget_ShaderResource;
	PTVD_BucketAllocator<PTVD_MEMORYTYPE_TEXTURE_DEPTHSTENCIL_SHADERRESOURCE> m_Bucket_TextureDepthStencil_ShaderResource;
	PTVD_BucketAllocator<PTVD_MEMORYTYPE_TEXTURE_DEPTHSTENCIL> m_Bucket_TextureDepthStencil;
	PTVD_BucketAllocator<PTVD_MEMORYTYPE_TEXTURE_UNORDEREDACCESS_SHADERRESOURCE> m_Bucket_TextureUnorderedAccess_ShaderResource;
	PTVD_BucketAllocator<PTVD_MEMORYTYPE_TEXTURE_SHADERRESOURCE> m_Bucket_TextureShaderResource;

public:
	inline void Construct();
	inline PTVD_MemoryObject *Alloc(uint32_t MemoryType, uint32_t Size);
	inline void Destruct();
};
static_assert(std::is_pod<PTVD_ThreadLocalRouter>::value, "PTVD_ThreadLocalRouter Is Not A POD");

#if defined(PTVDVK)
inline VkDeviceMemory PTVD_MemoryObject::Memory() const
{
	return m_pBlock_Owner->m_hMemory;
}

inline VkBuffer PTVD_MemoryObject::Buffer() const
{
	return m_pBlock_Owner->m_hBuffer;
}

inline VkDeviceSize PTVD_MemoryObject::MemoryOffset() const
{
	return (m_pBlock_Owner->m_ObjectSize)*(this - m_pBlock_Owner->m_ObjectVector);
}

#ifndef NDEBUG
inline VkDeviceSize PTVD_MemoryObject::MemorySize() const
{
	return m_pBlock_Owner->m_BlockSize;
}
#endif

#elif defined(PTVDD3D12)

#else
#error 未知的图形API
#endif

#if defined(PTVDVK)
inline void PTVD_MemoryBlock::Construct(PTVD_ThreadLocalAllocator *pAllocator_Owner, VkDeviceMemory hMemory, VkDeviceSize BlockSize, VkDeviceSize ObjectSize, uint32_t BucketIndex_InAllocator)
{
	//Private Fields
	m_pAllocator_Owner = pAllocator_Owner;
	m_ThreadID_Owner = ::PTSThreadID_Self();

	m_pNext_InBucket = NULL;
	m_pPrevious_InBucket = NULL;

	m_ObjectFreeVector_Private = NULL;

	assert((BlockSize % ObjectSize) == VkDeviceSize(0U));
	m_ObjectVector = static_cast<PTVD_MemoryObject *>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTVD_MemoryObject)*static_cast<uint32_t>((BlockSize / ObjectSize)), alignof(PTVD_MemoryObject)));
	m_hMemory = hMemory;
	m_ObjectSize = ObjectSize;
#ifndef NDEBUG
	m_BlockSize = BlockSize;
#endif
	m_BumpMemoryOffset = 0U;

	m_BucketIndex_InAllocator = BucketIndex_InAllocator;
	m_ObjectAllocated_Number = 0U;

	//Public Fields
	m_ObjectFreeVector_Public = NULL;
}
#elif defined(PTVDD3D12)

#else
#error 未知的图形API
#endif

template<uint32_t const MemoryType_PTVD>
inline void PTVD_BucketAllocator<MemoryType_PTVD>::Construct()
{
	::PTS_MemoryZero(m_BucketVector, sizeof(m_BucketVector));
}

template<uint32_t const MemoryType_PTVD>
inline PTVD_MemoryObject *PTVD_BucketAllocator<MemoryType_PTVD>::Alloc(uint32_t Size)
{
	uint32_t BucketIndex;
	uint32_t BucketSize;
	uint32_t ObjectSize;
	s_ResolveRequestSizeVector[MemoryType_PTVD](&BucketIndex, &BucketSize, &ObjectSize, Size);
	return NULL;
}

template<uint32_t const MemoryType_PTVD>
inline void PTVD_BucketAllocator<MemoryType_PTVD>::Destruct()
{

}

inline void PTVD_ThreadLocalRouter::Construct()
{
	m_Bucket_BufferStaging.Construct();
	m_Bucket_BufferConstant.Construct();
	m_Bucket_BufferUnorderedAccess_ShaderResource.Construct();
	m_Bucket_BufferVertex.Construct();
	m_Bucket_BufferIndex.Construct();
	m_Bucket_TextureRenderTarget_ShaderResource.Construct();
	m_Bucket_TextureDepthStencil_ShaderResource.Construct();
	m_Bucket_TextureDepthStencil.Construct();
	m_Bucket_TextureUnorderedAccess_ShaderResource.Construct();
	m_Bucket_TextureShaderResource.Construct();
}

inline PTVD_MemoryObject *PTVD_ThreadLocalRouter::Alloc(uint32_t MemoryType, uint32_t Size)
{
	PTVD_MemoryObject *pObject = NULL;

	switch (MemoryType)
	{
	case PTVD_MEMORYTYPE_BUFFER_STAGING: pObject = m_Bucket_BufferStaging.Alloc(Size); break;
	case PTVD_MEMORYTYPE_BUFFER_CONSTANT: pObject = m_Bucket_BufferConstant.Alloc(Size); break;
	case PTVD_MEMORYTYPE_BUFFER_UNORDEREDACCESS_SHADERRESOURCE: pObject = m_Bucket_BufferUnorderedAccess_ShaderResource.Alloc(Size); break;
	case PTVD_MEMORYTYPE_BUFFER_VERTEX: pObject = m_Bucket_BufferVertex.Alloc(Size); break;
	case PTVD_MEMORYTYPE_BUFFER_INDEX: pObject = m_Bucket_BufferIndex.Alloc(Size); break;
	case PTVD_MEMORYTYPE_TEXTURE_RENDERTARGET_SHADERRESOURCE: pObject = m_Bucket_TextureRenderTarget_ShaderResource.Alloc(Size); break;
	case PTVD_MEMORYTYPE_TEXTURE_DEPTHSTENCIL_SHADERRESOURCE: pObject = m_Bucket_TextureDepthStencil_ShaderResource.Alloc(Size); break;
	case PTVD_MEMORYTYPE_TEXTURE_DEPTHSTENCIL: pObject = m_Bucket_TextureDepthStencil.Alloc(Size); break;
	case PTVD_MEMORYTYPE_TEXTURE_UNORDEREDACCESS_SHADERRESOURCE: pObject = m_Bucket_TextureUnorderedAccess_ShaderResource.Alloc(Size); break;
	case PTVD_MEMORYTYPE_TEXTURE_SHADERRESOURCE: pObject = m_Bucket_TextureShaderResource.Alloc(Size); break;
	default:assert(0);
	}

	return pObject;
}

inline void PTVD_ThreadLocalRouter::Destruct()
{
	m_Bucket_BufferStaging.Destruct();
	m_Bucket_BufferConstant.Destruct();
	m_Bucket_BufferUnorderedAccess_ShaderResource.Destruct();
	m_Bucket_BufferVertex.Destruct();
	m_Bucket_BufferIndex.Destruct();
	m_Bucket_TextureRenderTarget_ShaderResource.Destruct();
	m_Bucket_TextureDepthStencil_ShaderResource.Destruct();
	m_Bucket_TextureDepthStencil.Destruct();
	m_Bucket_TextureUnorderedAccess_ShaderResource.Destruct();
	m_Bucket_TextureShaderResource.Destruct();
}

//***********************************************************************************************************************************************************************
//API Entry

class PTVD_MemoryAllocator
{
	PTVD_BlockManager m_BlockManager;
	PTSTSD_KEY m_ThreadLocalRouter_Index;
public:
#if defined(PTVDVK)
	inline bool Construct(VkInstanceWrapper &rInstanceWrapper, VkDeviceWrapper &rDeviceWrapper);
#elif defined(PTVDD3D12)

#else
#error 未知的图形API
#endif
	inline PTVD_MemoryObject *Alloc(uint32_t MemoryType, uint32_t Size);
};

#if defined(PTVDVK)
inline bool PTVD_MemoryAllocator::Construct(VkInstanceWrapper &rInstanceWrapper, VkDeviceWrapper &rDeviceWrapper)
{
	PTBOOL tbResult = ::PTSTSD_Create(
		&m_ThreadLocalRouter_Index,
		[](void *pVoid)->void {
		PTVD_ThreadLocalRouter *pThreadLocalRouter = static_cast<PTVD_ThreadLocalRouter *>(pVoid);
		pThreadLocalRouter->Destruct();
		::PTSMemoryAllocator_Free_Aligned(pThreadLocalRouter);
	}
	);

	return (tbResult != PTFALSE) ? true : false;
}
#elif defined(PTVDD3D12)

#else
#error 未知的图形API
#endif

#if defined(PTVDVK)
inline PTVD_MemoryObject *PTVD_MemoryAllocator::Alloc(uint32_t MemoryType, uint32_t Size)
{
	PTVD_ThreadLocalRouter *pThreadLocalRouter = static_cast<PTVD_ThreadLocalRouter *>(::PTSTSD_GetValue(m_ThreadLocalRouter_Index));

	if (pThreadLocalRouter == NULL)
	{
		pThreadLocalRouter = static_cast<PTVD_ThreadLocalRouter *>(new(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTVD_ThreadLocalRouter), alignof(PTVD_ThreadLocalRouter)))PTVD_ThreadLocalRouter{});
		assert(pThreadLocalRouter != NULL);

		pThreadLocalRouter->Construct();

		PTBOOL tbResult = ::PTSTSD_SetValue(m_ThreadLocalRouter_Index, pThreadLocalRouter);
		assert(tbResult != PTFALSE);
	}

	return pThreadLocalRouter->Alloc(MemoryType, Size);
}
#elif defined(PTVDD3D12)

#else
#error 未知的图形API
#endif


PTVD_MemoryAllocator *PTVD_MemoryAllocator_Create(VkInstanceWrapper &rInstanceWrapper, VkDeviceWrapper &rDeviceWrapper)
{
	PTVD_MemoryAllocator *pMemoryAllocator_PTVD = static_cast<PTVD_MemoryAllocator *>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTVD_ThreadLocalRouter), alignof(PTVD_ThreadLocalRouter)));
	assert(pMemoryAllocator_PTVD != NULL);

	bool bResult = pMemoryAllocator_PTVD->Construct(rInstanceWrapper, rDeviceWrapper);
	assert(bResult != false);

	return pMemoryAllocator_PTVD;
}

PTVD_MemoryObject *PTVD_MemoryAllocator_Alloc(PTVD_MemoryAllocator *pMemoryAllocator_PTVD, uint32_t MemoryType, uint32_t Size)
{
	return pMemoryAllocator_PTVD->Alloc(MemoryType, Size);
}


#if defined(PTVDVK)
VkDeviceMemory PTVD_MemoryObject_Memory(PTVD_MemoryObject *pMemoryObject)
{
	return pMemoryObject->Memory();
}

VkDeviceSize PTVD_MemoryObject_MemoryOffset(PTVD_MemoryObject *pMemoryObject)
{
	return pMemoryObject->MemoryOffset();
}

#elif defined(PTVDD3D12)

#else
#error 未知的图形API
#endif