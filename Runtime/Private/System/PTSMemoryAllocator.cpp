#include "../../Public/System/PTSThread.h"
#include "../../Public/System/PTSMemoryAllocator.h"
#include <stddef.h> 
#include <assert.h>

//Richard L. Hudson, Bratin Saha, Ali-Reza Adl-Tabatabai, Benjamin C. Hertzberg. "McRT-Malloc: a scalable transactional memory allocator". Proceedings of the 5th international symposium on Memory management ACM 2006.
//Alexey Kukanov, Michael J.Voss. "The Foundations for Scalable Multi-core Software in Intel Threading Building Blocks." Intel Technology Journal, Volume11, Issue 4 2007.

//Operating System Primitive
static inline void *PTS_MemoryMap_Alloc(uint32_t size);
static inline void PTS_MemoryMap_Free(void *pVoid);

//Hudson 2006 / 3.McRT-MALLOC
//At initialization, McRT-Malloc reserves a large piece of virtual memory for its heap 
//(using an operating system primitive like the Linux mmap and munmap primitives)
//and divides the heap into 16K-byte aligned blocks that initially reside in a global block store.
//---------------------------------------------------------
//When a thread needs a block from which to allocate objects,
//it acquires ownership of one from the block store
//using a nonblocking algorithm we describe later.
//---------------------------------------------------------
//Unowned yet nonempty blocks in the block store are not owned by any thread.
//These partially filled blocks are segregated based on object size
//and managed using the same non-blocking algorithms used for the empty blocks.
//--------------------------------------------------------
//When sufficiently empty（何时进行检测？）
//a block can be distributed to another thread, 
//which then takes ownership of the block’s objects.
//--------------------------------------------------------
//Hudson 2006 / 3.McRT-MALLOC / 3.2 Non-blocking Operations
//we use the bits freed up due to the alignment of the blocks to accomplish our non-blocking properties,
//in particular to avoid the ABA problem,
struct PTS_BlockMetadata;
struct PTS_ThreadLocalBinArray;
static uint32_t const s_Block_Size = 16U * 1024U;//严格意义上16KB应当是对齐而非大小
static uint32_t const s_Bin_Number = 33U;
#define PTS_USE_QUEUEEMPTY 1
static struct PTS_BlockStore
{
	uint64_t m_QueueNonEmptyTops[s_Bin_Number];

#if PTS_USE_QUEUEEMPTY
	uint64_t m_QueueEmptyTop;
#endif

	static inline void Construct(PTS_BlockStore *pThis);

	static inline void _Unpack(uint64_t *pVersionNumber, PTS_BlockMetadata **pBlockAddress, uint64_t Value);

	static inline uint64_t _Pack(uint64_t VersionNumber, PTS_BlockMetadata *BlockAddress);

	static inline void _AtmoicPush(uint64_t *pQueueTop, PTS_BlockMetadata *pBlockToPush);
	
	static inline PTS_BlockMetadata * _AtmoicPop(uint64_t *pQueueTop);

	static inline void PushNonEmpty(PTS_BlockStore *pThis, uint32_t BinIndex, PTS_BlockMetadata *pBlockToPush);

	static inline void PushEmpty(PTS_BlockStore *pThis, PTS_BlockMetadata *pBlockToPush);

	static inline PTS_BlockMetadata *PopNonEmpty(PTS_ThreadLocalBinArray *pTLS, PTS_BlockStore *pThis, uint32_t BinIndex);

	static inline PTS_BlockMetadata *PopEmpty(PTS_ThreadLocalBinArray *pTLS, PTS_BlockStore *pThis, uint32_t BinIndex, uint32_t ObjectSize);

}s_BlockStore_Singleton;
//When a thread dies, the scheduler notifies McRT-Malloc,（作为PTSTSD_DESTRUCTOR触发）
//which then returns the dead thread’s blocks back to the block store.
PTBOOL PTCALL PTSMemoryAllocator_Initialize();

//Hudson 2006 / 3.McRT-MALLOC
//Each thread maintains a small thread-local array of bins,
//each bin containing a list of blocks holding the same sized objects.
//--------------------------------------------------------
//Hudson 2006 / 3.McRT-MALLOC / 3.3 Framework for Managing Blocks
//The blocks in any particular size bin are arranged in a circular linked list
//with one of the blocks distinguished as the bin head.
//--------------------------------------------------------
//As malloc traverses this linked list looking for a free object 
//it collects per bin statistics including how many free objects are encountered upon arriving at a block 
//and how many blocks are on the list.
//A complete traversal is noted when the thread encounters the distinguished bin head block.
//The collected statistics are used to set policy for the next traversal.
//Policy includes whether additional blocks should be allocated to the bin
//or empty blocks returned to the main store.
//The process is repeated for each full traversal of the list.
//--------------------------------------------------------
//The currently implemented policy is that 
//if less than 20 % of the objects are free and available we add blocks
//and if a block becomes completely empty it is returned to the block store.
//One final wrinkle is that if we inspect 10 blocks 
//without finding a free object 
//then we add a new block for the bin.
//This places an upper bound on the time malloc will take before returning an object.
//The take away here is that we use the distinguished bin head block
//as a hook to set policy 
//--------------------------------------------------------
//and the traversal of the blocks to collect the statistics
//used to set the policy.
//--------------------------------------------------------
//Kukanov 2007 / SCALABLE MEMORY ALLOCATION / The TBB Scalable Allocator
//At each moment, there is at most one active block 
//used to fulfill allocation requests for a given object size.
//Once the active block has no more free objects, 
//the bin is switched to use another block.
//--------------------------------------------------------
//Unlike in other allocators,
//the active block may be located in the middle of the list; 
//empty enough blocks are placed before it, 
//and full blocks are placed after it.
//--------------------------------------------------------
//This design minimizes the time to search for a new block 
//if the active one is full.
//When enough objects are freed in a block,
//the block is moved right before the active block 
//and thus becomes available for further allocation.
//A block with all its objects freed returns back to the global heap; 
//new blocks are taken from there as required.
static PTSTSD_KEY s_TLBinArray_Index;
struct PTS_BlockMetadata;
struct PTS_ThreadLocalBinArray
{
	PTS_BlockMetadata *m_Bins[s_Bin_Number];

	static inline void Construct(PTS_ThreadLocalBinArray *pThis);
//Traverse In PTSMemoryAllocator_Alloc
	static inline void Insert(PTS_ThreadLocalBinArray *pThis, uint32_t BinIndex, PTS_BlockMetadata *pBlockToPush);
	static inline void Remove(PTS_ThreadLocalBinArray *pThis, uint32_t BinIndex, PTS_BlockMetadata *pBlockToPush);
	
	static inline void Destruct(PTS_ThreadLocalBinArray *pThis);
};

//Hudson 2006 / 3.McRT-MALLOC
//The array contains a bin for every multiple of four between 4 bytes and 64 bytes, 
//and a bin for every power of two between 128 bytes and 8096 bytes.
//Allocation of objects greater than 8096 bytes is handled by the operating system(e.g., on Linux we use mmap and munmap).
//Large objects are rare so we don’t discuss them further.
static inline bool PTS_Size_IsPowerOfTwo(uint32_t Value);
static inline bool PTS_Size_IsAligned(size_t Value, size_t Alignment);
static inline size_t PTS_Size_AlignDownFrom(size_t Value, size_t alignment);
static inline uint32_t PTS_Size_AlignUpFrom(uint32_t Value, uint32_t Alignment);
static inline uint64_t PTS_Size_AlignUpFrom(uint64_t Value, uint64_t Alignment);
static inline uint32_t PTS_Size_BitScanReverse(uint32_t Value);
//PartI  [ 8/16/32/48/64 ]
//PartII [ 80/96/112/128 || 60/192/224/256 || 320/384/448/512 || 640/768/896/1024 || 1280/1536/1792/2048 || 2560/3072/3584/4096 || 5120/6144/7168/8192 ]
//static uint32_t const s_Bin_Number = 33U;//5+28=33
static uint32_t const s_NonLargeObject_SizeMax = 8192U;
static inline void PTS_Size_ResolveRequestSize(uint32_t *pBinIndex, uint32_t *pObjectSize, uint32_t RequestSize);

//Hudson 2006 / 3.McRT-MALLOC
//After acquiring a block, a thread divides it into equal-sized objects
//initially placed on the block’s private free list.
//The thread owning a block also owns theobjects in the block.
//---------------------------------------------------------
//The base of each block contains a 64-byte structure holding the block's meta-data.
//With the exception of the public free list field, 
//the fields in the metadata block are strictly local to the owning thread and are therefore trivially thread safe.
//---------------------------------------------------------
//To facilitate efficient insertion and deletion the next and previous fields support the doubly-linked list of blocks in each bin.
//There is a field indicating the size of objects in the block, 
//a field tracking the number of objects in the block, 
//and a field tracking the number of objects that have been allocated but not immediately available for allocation.（FreeListPrivate和FreeListPublic中的个数？）
//---------------------------------------------------------
//Each block has fields for two linked lists of freed objects.
//A private free list field points to a linked list of objects available for allocation;
//the thread satisfies malloc requests from this list.
//A public free list field points to a linked list of objects freed by threads that do not own the block(foreign threads).
//There is a bump pointer field used to avoid explicitly initializing the private free list for an empty block.
//---------------------------------------------------------
//The bump pointer implicitly represents the free list and is used for allocation of objects in newly acquired empty blocks. 
//Once the bump pointer reaches the end of the block, the thread allocates objects using the explicit private free list.（Kukanov 2007 认为应该优先使用FreeListPrivate 缓存局部性更高）
//---------------------------------------------------------
//Per-object headers are not needed by the base McRT-Malloc
//since all relevant information is held in the block’s meta-data.
//---------------------------------------------------------
//Hudson 2006 / 3.McRT-MALLOC / 3.1 Allocating and Freeing Objects
//To allocate an object within a block, the thread first checks the block’s bump pointer.
//If the bump pointer is null, the thread allocates the object from the private free list;
//otherwise, it increments the bump pointer by the size of the object, checks for overflow, and returns the object if there is no overflow.
//On overflow, it sets the bump pointer to null and allocates from the private free list.
//The private free list is accessed only by the owning thread so this allocation algorithm is trivially thread safe.
//---------------------------------------------------------
//If the private free list is empty then the thread examines the public free list to see if it has any objects to repatriate.
//A thread repatriates the objects on the public free list of a block that it owns by moving those objects to the block’s private free list 
//using a non-blocking thread safe algorithm.
//---------------------------------------------------------
//The algorithm for freeing an object depends on whether the thread performing the free owns the freed object.
//To free an object it owns, a thread places the object on the private free list of the block containing the object, a thread safe operation.
//To free an object it does not own, a thread places the object on the public free list of the object's block using a more expensive non-blocking operation.
//--------------------------------------------------------
//Hudson 2006 / 3.McRT-MALLOC / 3.2 Non-blocking Operations
//we maintain two free lists, one private and trivially non-blocking,
//and another a public version which is managed as a single consumer, multiple-producer queue 
//and implemented as a non-blocking structure.
#if defined(PTARM) || defined(PTARM64) || defined(PTX86) || defined(PTX64)
static uint32_t const s_CacheLine_Size = 64U;
#else
#error 未知的架构
#endif
struct PTS_ObjectMetadata
{
	PTS_ObjectMetadata *m_FreeList_Next;
};
struct PTS_BlockMetadata
{
	PTS_BlockMetadata *m_BlockStore_Next;
	PTS_ObjectMetadata *m_FreeListPublic;
	
	//为避免伪共享，将可能被Foreign线程访问的Public域和只能被Owner线程访问的Private域分隔在不同的缓存行中
	uint8_t __PaddingForPublicFields[s_CacheLine_Size - sizeof(void*) * 2U];

	PTS_ThreadLocalBinArray *m_pTLS;//NULL表明Invalid TID_Owning，同时 在缓存TLS后 PTS_Internal_Free中不再需要调用PTSTSD_GetValue

	PTS_BlockMetadata *m_Bin_Next; //Full Block
	PTS_BlockMetadata *m_Bin_Previous;//Empty Enough Block

	PTS_ObjectMetadata *m_FreeListPrivate;
	PTS_ObjectMetadata *m_BumpPointer;
	
	PTSThreadID m_TID_Owning;

	uint32_t m_Object_Size;
	uint32_t m_Bin_Index;
	uint32_t m_ObjectAllocated_Number;
	
	//用于区分Block和LargeObject，但并不一定可靠
	uint32_t m_Identity;
	//uint32_t m_Identity1;
	//uint32_t m_Identity2;
	//uint32_t m_Identity3;

	//分隔Metadata和用户数据
	uint8_t __PaddingForPrivateFields[s_CacheLine_Size - sizeof(uint32_t) * 4U - sizeof(PTSThreadID) - sizeof(void*) * 4U - sizeof(void*)];

	static uint32_t const s_BlockIdentity = 0X5765C7B8U;
	//static uint32_t const s_BlockIdentity1 = 0XB7F2F907U;
	//static uint32_t const s_BlockIdentity2 = 0X48AE33F5U;
	//static uint32_t const s_BlockIdentity3 = 0XED1BAA7BU;

	static inline void Construct(PTS_ThreadLocalBinArray *pTLS, PTS_BlockMetadata *pThis, uint32_t BinIndex, uint32_t ObjectSize);

	static inline void _BumpPointerRestore(PTS_BlockMetadata *pThis, uint32_t ObjectSize);

	static inline PTS_ObjectMetadata *_BumpPointer(PTS_BlockMetadata *pThis);

	static inline void _FreeListPrivatePush(PTS_BlockMetadata *pThis, PTS_ObjectMetadata *pObjectToPush);

	static inline PTS_ObjectMetadata *_FreeListPrivatePop(PTS_BlockMetadata *pThis);

	static inline void _FreeListPublicPush(PTS_BlockMetadata *pThis, PTS_ObjectMetadata *pObjectToPush); //Foreign Thread

	static inline void _FreeListPublicRepatriate(PTS_BlockMetadata *pThis);

	static inline PTS_ObjectMetadata *Allocate(PTS_ThreadLocalBinArray *pTLS, PTS_BlockMetadata *pThis);

	static inline void Free(PTS_ThreadLocalBinArray *pTLS, PTS_BlockMetadata *pThis, PTS_ObjectMetadata *pObjectToFree);
};
static_assert(offsetof(PTS_BlockMetadata, m_pTLS) == s_CacheLine_Size, "Padding Not Correct");
static_assert(sizeof(PTS_BlockMetadata) == s_CacheLine_Size * 2U, "Padding Not Correct");

//------------------------------------------------------------------------------------------------------------

inline void PTS_BlockStore::Construct(PTS_BlockStore *pThis)
{
	for (uint32_t i = 0U; i < s_Bin_Number; ++i)
	{
		pThis->m_QueueNonEmptyTops[i] = 0U;
	}

#if PTS_USE_QUEUEEMPTY
	pThis->m_QueueEmptyTop = 0U;
#endif
}

//Hudson 2006 / 3.McRT-MALLOC / 3.2 Non-blocking Operations

//Initially each block in the heap is publicly owned and resides in the block store.
//The block store manages the blocks using a nonblocking concurrent LIFO queue data structure.
//The queue is maintained as a linked list threaded through the first word of an aligned 16K block.
//Any block in a 32 bit address space requires 18 bits to address (the lower 14 bits of a block’s address are always zero).
//To avoid ABA problems the queue uses a versioning scheme and a 64-bit CAS instruction.
//The version number uses 46 bits leaving 18 for the blocks address.
//The question then becomes whether 46 bits is sufficient to prevent rollover and the ABA problem.

//For the queue with a 64 bit CAS, 18 bits needed to address the blocks leaving 46 bits available for versioning.
//Assuming we allocate at 1 byte per cycle, which is a furious allocation rate,
//on a 3 Gigahertz machine we would allocate 3,000,000,000 bytes per second.
//This would require 181,422 blocks per second.
//Wrap around would take slightly over 12 years 
//and wrap around would only be a problem if a thread was suspended at just the right time and stayed suspended for the requisite 12 years.
//A 64 bit computer would require a 128 bit(16 byte) double wide CAS instruction.
//The locked cmpxchg16b instruction provided by the x86 is an example of such an instruction 
//and trivially provides for a full 64 bits of versioning which is sufficient.

//应用程序只可能访问地址空间中的用户空间
//x86或ARM架构下，用户空间一定不超过4GB（32位），块地址只需要18（32-14=18）位，其余46位可用作版本号，环绕需要2年（基于论文中46位需要2年计算）
//x86_64架构下
//在Windows（https://docs.microsoft.com/en-us/windows-hardware/drivers/gettingstarted/virtual-address-spaces）中，用户空间只有8TB（43位），块地址只需要29（43-14=29）位，可以使用64位CAS指令，其余35位用作版本号，环绕需要2天（基于论文中46位需要2年计算）
//在Linux（https://www.kernel.org/doc/Documentation/x86/x86_64/mm.txt）中，用户空间只有128TB（47位），块地址只需要33（47-14=33）位，可以使用64位CAS指令，其余31位用作版本号，环绕需要12小时（基于论文中46位需要2年计算）
//在ARM64架构下
//在Windows中，目前不存在支持ARM64架构的Windows发行版
//在Linux（https://www.kernel.org/doc/Documentation/arm64/memory.txt）中，用户空间只有512GB（39位），块地址只需要25（39-14=25）位，可以使用64位CAS指令，其余39位用作版本号，环绕需要8天（基于论文中46位需要2年计算）

#if defined(PTX86) || defined(PTARM)
inline void PTS_BlockStore::_Unpack(uint64_t *pVersionNumber, PTS_BlockMetadata **pBlockAddress, uint64_t Value)
{
	assert(s_Block_Size == (1U << 14U));

	//46位    //18位
	//Version //Address

	(*pVersionNumber) = ((Value) >> 18U);
	(*pBlockAddress) = reinterpret_cast<PTS_BlockMetadata *>((Value & 0X1FFFFU) << 14U);
}
inline uint64_t PTS_BlockStore::_Pack(uint64_t VersionNumber, PTS_BlockMetadata *BlockAddress)
{
	assert(s_Block_Size == (1U << 14U));
	assert((reinterpret_cast<uintptr_t>(BlockAddress) & 0X00003FFFU) == 0U);

	//47位    //17位（前1后14）
	//Version //Address

	return (VersionNumber << 17U) | (reinterpret_cast<uintptr_t>(BlockAddress) >> 14U);
}
#elif defined(PTX64)
#if defined(PTWIN32)
inline void PTS_BlockStore::_Unpack(uint64_t *pVersionNumber, PTS_BlockMetadata **pBlockAddress, uint64_t Value)
{
	//35位    //29位（前21后14）
	//Version //Address

	(*pVersionNumber) = ((Value) >> 29U);
	(*pBlockAddress) = reinterpret_cast<PTS_BlockMetadata *>((Value & 0X1FFFFFFFU) << 14U);
}

inline uint64_t PTS_BlockStore::_Pack(uint64_t VersionNumber, PTS_BlockMetadata *BlockAddress)
{
	assert(s_Block_Size == (1U << 14U));
	assert((reinterpret_cast<uintptr_t>(BlockAddress) & 0XFFFFF80000003FFFU) == 0U);

	//35位    //29位（前21后14）
	//Version //Address

	return (VersionNumber << 29U) | (reinterpret_cast<uintptr_t>(BlockAddress) >> 14U);
}
#elif defined(PTPOSIX)
inline void PTS_BlockStore::_Unpack(uint64_t *pVersionNumber, PTS_BlockMetadata **pBlockAddress, uint64_t Value)
{
	//31位    //33位（前17后14）
	//Version //Address

	(*pVersionNumber) = ((Value) >> 33U);
	(*pBlockAddress) = reinterpret_cast<PTS_BlockMetadata *>((Value & 0X1FFFFFFFFFU) << 14U);
}

inline uint64_t PTS_BlockStore::_Pack(uint64_t VersionNumber, PTS_BlockMetadata *BlockAddress)
{
	assert(s_Block_Size == (1U << 14U));
	assert((reinterpret_cast<uintptr_t>(BlockAddress) & 0XFFFF800000003FFFU) == 0U);

	//31位    //33位（前17后14）
	//Version //Address

	return (VersionNumber << 33U) | (reinterpret_cast<uintptr_t>(BlockAddress) >> 14U);
}
#else
#error 未知的平台
#endif
#elif defined(PTARM64)
#if defined(PTWIN32)
#error 尚未实现
#elif defined(PTPOSIX)
inline void PTS_BlockStore::_Unpack(uint64_t *pVersionNumber, PTS_BlockMetadata **pBlockAddress, uint64_t Value)
{
	//39位    //25位（前25后14）
	//Version //Address

	(*pVersionNumber) = ((Value) >> 25U);
	(*pBlockAddress) = reinterpret_cast<PTS_BlockMetadata *>((Value & 0X1FFFFFFU) << 14U);
}

inline uint64_t PTS_BlockStore::_Pack(uint64_t VersionNumber, PTS_BlockMetadata *BlockAddress)
{
	assert(s_Block_Size == (1U << 14U));
	assert((reinterpret_cast<uintptr_t>(BlockAddress) & 0XFFFFFF8000003FFFU) == 0U);

	//39位    //25位（前25后14）
	//Version //Address

	return (VersionNumber << 25U) | (reinterpret_cast<uintptr_t>(BlockAddress) >> 14U);
}
#else
#error 未知的平台
#endif
#else
#error 未知的架构
#endif

inline void PTS_BlockStore::_AtmoicPush(uint64_t *pQueueTop, PTS_BlockMetadata *blockPtr)
{
	//Hudson 2006 / 3.McRT-MALLOC / 3.2 Non-blocking Operations / Figure 1 Non-blocking queues / lifoQueuePush

	uint64_t oldValue;
	uint64_t newValue;

	do {
		oldValue = (*pQueueTop);

		uint64_t oldCount;
		PTS_BlockMetadata *oldBlockPtr;
		PTS_BlockStore::_Unpack(&oldCount, &oldBlockPtr, oldValue);

		uint64_t newCount = oldCount + 1;
		blockPtr->m_BlockStore_Next = oldBlockPtr;

		newValue = PTS_BlockStore::_Pack(newCount, blockPtr);
	} while (::PTSAtomic_CompareAndSet(pQueueTop, oldValue, newValue) != oldValue);

}

inline PTS_BlockMetadata * PTS_BlockStore::_AtmoicPop(uint64_t *pQueueTop)
{
	//Hudson 2006 / 3.McRT-MALLOC / 3.2 Non-blocking Operations / Figure 1 Non-blocking queues / lifoQueuePop

	uint64_t oldQueue;
	uint64_t newQueue;
	PTS_BlockMetadata *blockPtr;

	do {
		oldQueue = (*pQueueTop);

		uint64_t oldCount;
		PTS_BlockStore::_Unpack(&oldCount, &blockPtr, oldQueue);

		if (blockPtr == NULL) //The Queue is empty
		{
			break;
		}

		uint64_t newCount = oldCount + 1;
		PTS_BlockMetadata *newBlockPtr = blockPtr->m_BlockStore_Next;

		newQueue = PTS_BlockStore::_Pack(newCount, newBlockPtr);
	} while (::PTSAtomic_CompareAndSet(pQueueTop, oldQueue, newQueue) != oldQueue);

	return blockPtr;
}

inline void PTS_BlockStore::PushNonEmpty(PTS_BlockStore *pThis, uint32_t BinIndex, PTS_BlockMetadata *pBlockToPush)
{
	assert((pBlockToPush->m_pTLS != NULL) && (::PTSThreadID_Equal(pBlockToPush->m_TID_Owning, ::PTSThreadID_Self()) != PTFALSE));
	assert(pBlockToPush->m_Bin_Next == NULL);
	assert(pBlockToPush->m_Bin_Previous == NULL);
	assert(pBlockToPush->m_Bin_Index == BinIndex);
	assert(pBlockToPush->m_Bin_Index < s_Bin_Number);
	assert(pBlockToPush->m_ObjectAllocated_Number > 0U);

	pBlockToPush->m_pTLS = NULL;
	//pBlockToPush->m_TID_Owning = ~PTSThreadID(0U);

	PTS_BlockStore::_AtmoicPush(&pThis->m_QueueNonEmptyTops[BinIndex], pBlockToPush);
}

inline void PTS_BlockStore::PushEmpty(PTS_BlockStore *pThis, PTS_BlockMetadata *pBlockToPush)
{
#if PTS_USE_QUEUEEMPTY
	//Private Field
	assert((pBlockToPush->m_pTLS != NULL) && (::PTSThreadID_Equal(pBlockToPush->m_TID_Owning, ::PTSThreadID_Self()) != PTFALSE));
	assert(pBlockToPush->m_Bin_Next == NULL);
	assert(pBlockToPush->m_Bin_Previous == NULL);
	assert(pBlockToPush->m_ObjectAllocated_Number == 0U);

	pBlockToPush->m_pTLS = NULL;
	//pBlockToPush->m_TID_Owning = ~PTSThreadID(0U);

	//pBlockToPush->m_FreeListPrivate = NULL;
	//pBlockToPush->m_BumpPointer = NULL;//BumpPointer的初始化依赖于ObjectSize

	//pBlockToPush->m_Bin_Index = ~uint32_t(0U);
	//pBlockToPush->m_Object_Size = ~uint32_t(0U);

	PTS_BlockStore::_AtmoicPush(&pThis->m_QueueEmptyTop, pBlockToPush);

	//Public Field
	assert(pBlockToPush->m_FreeListPublic == NULL);
#else
	::PTS_MemoryMap_Free(pBlockToPush);
#endif
}

inline PTS_BlockMetadata *PTS_BlockStore::PopNonEmpty(PTS_ThreadLocalBinArray *pTLS, PTS_BlockStore *pThis, uint32_t BinIndex)
{
	PTS_BlockMetadata *pBlockAdded = PTS_BlockStore::_AtmoicPop(&pThis->m_QueueNonEmptyTops[BinIndex]);

	if (pBlockAdded != NULL)
	{
		pBlockAdded->m_pTLS = pTLS;
		pBlockAdded->m_TID_Owning = ::PTSThreadID_Self();
	}

	return pBlockAdded;
}

inline PTS_BlockMetadata *PTS_BlockStore::PopEmpty(PTS_ThreadLocalBinArray *pTLS, PTS_BlockStore *pThis, uint32_t BinIndex, uint32_t ObjectSize)
{
#if PTS_USE_QUEUEEMPTY
	PTS_BlockMetadata *pBlockAdded = PTS_BlockStore::_AtmoicPop(&pThis->m_QueueEmptyTop);

	if (pBlockAdded != NULL)
	{
		pBlockAdded->m_pTLS = pTLS;
		pBlockAdded->m_TID_Owning = ::PTSThreadID_Self();

		PTS_BlockMetadata::_BumpPointerRestore(pBlockAdded, ObjectSize);

		pBlockAdded->m_Bin_Index = BinIndex;
		pBlockAdded->m_Object_Size = ObjectSize;
	}
	else
	{
		//启发式，而非在初始化时一次性分配
		pBlockAdded  = static_cast<PTS_BlockMetadata *>(::PTS_MemoryMap_Alloc(s_Block_Size));

		//严格意义上16KB应当是对齐而非大小
		assert(PTS_Size_IsAligned(reinterpret_cast<uintptr_t>(pBlockAdded), s_Block_Size));

		PTS_BlockMetadata::Construct(pTLS, pBlockAdded, BinIndex, ObjectSize);
	}

	return pBlockAdded;
#else
	PTS_BlockMetadata *pBlockAdded = static_cast<PTS_BlockMetadata *>(::PTS_MemoryMap_Alloc(s_Block_Size));

	//严格意义上16KB应当是对齐而非大小
	assert(PTS_Size_IsAligned(reinterpret_cast<uintptr_t>(pBlockAdded), s_Block_Size));

	PTS_BlockMetadata::Construct(pTLS, pBlockAdded, BinIndex, ObjectSize);

	return pBlockAdded;
#endif
}

//------------------------------------------------------------------------------------------------------------

inline void PTS_ThreadLocalBinArray::Construct(PTS_ThreadLocalBinArray *pThis)
{
	for (uint32_t i = 0U; i < s_Bin_Number; ++i)
	{
		pThis->m_Bins[i] = NULL;
	}
}

inline void PTS_ThreadLocalBinArray::Insert(PTS_ThreadLocalBinArray *pThis, uint32_t BinIndex, PTS_BlockMetadata *pBlockToPush)
{
	assert((pBlockToPush->m_pTLS != NULL) && (::PTSThreadID_Equal(pBlockToPush->m_TID_Owning, ::PTSThreadID_Self()) != PTFALSE));
	assert(pBlockToPush->m_Bin_Next == NULL);
	assert(pBlockToPush->m_Bin_Previous == NULL);
	assert(pBlockToPush->m_Bin_Index == BinIndex);
	assert(pBlockToPush->m_Bin_Index < s_Bin_Number);

	PTS_BlockMetadata * const pBlockActive = pThis->m_Bins[BinIndex];
	assert(pBlockActive == NULL || ((pBlockActive->m_pTLS != NULL) && (::PTSThreadID_Equal(pBlockActive->m_TID_Owning, ::PTSThreadID_Self()) != PTFALSE)));
	assert(pBlockActive == NULL || pBlockActive->m_Bin_Index == pBlockToPush->m_Bin_Index);
	if (pBlockActive == NULL)
	{
		pThis->m_Bins[BinIndex] = pBlockToPush;//Active初始化
	}

	pBlockToPush->m_Bin_Next = pBlockActive;
	if (pBlockActive != NULL)
	{
		pBlockToPush->m_Bin_Previous = pBlockActive->m_Bin_Previous;
		pBlockActive->m_Bin_Previous = pBlockToPush;
		if (pBlockToPush->m_Bin_Previous != NULL)
		{
			pBlockToPush->m_Bin_Previous->m_Bin_Next = pBlockToPush;
		}
	}
}

inline void PTS_ThreadLocalBinArray::Remove(PTS_ThreadLocalBinArray *pThis, uint32_t BinIndex, PTS_BlockMetadata *pBlockToPush)
{
	assert((pBlockToPush->m_pTLS != NULL) && (::PTSThreadID_Equal(pBlockToPush->m_TID_Owning, ::PTSThreadID_Self()) != PTFALSE));
	assert(pBlockToPush->m_Bin_Index == BinIndex);
	assert(pBlockToPush->m_Bin_Index < s_Bin_Number);

	PTS_BlockMetadata * const pBlockActive = pThis->m_Bins[BinIndex];
	assert(pBlockActive == NULL || ((pBlockActive->m_pTLS != NULL) && (::PTSThreadID_Equal(pBlockActive->m_TID_Owning, ::PTSThreadID_Self()) != PTFALSE)));
	assert(pBlockActive == NULL || pBlockActive->m_Bin_Index == pBlockToPush->m_Bin_Index);

	if (pBlockActive == pBlockToPush)
	{
		//Previous（即EmptyEnough）优先于Next（即Full）
		pThis->m_Bins[BinIndex] = (pBlockToPush->m_Bin_Previous != NULL) ? pBlockToPush->m_Bin_Previous : pBlockToPush->m_Bin_Next;
	}

	if (pBlockToPush->m_Bin_Previous!=NULL) 
	{
		assert(pBlockToPush->m_Bin_Previous->m_Bin_Next == pBlockToPush);
		pBlockToPush->m_Bin_Previous->m_Bin_Next = pBlockToPush->m_Bin_Next;
	}
	if (pBlockToPush->m_Bin_Next != NULL) 
	{
		assert(pBlockToPush->m_Bin_Next->m_Bin_Previous == pBlockToPush);
		pBlockToPush->m_Bin_Next->m_Bin_Previous = pBlockToPush->m_Bin_Previous;
	}
	pBlockToPush->m_Bin_Next = NULL;
	pBlockToPush->m_Bin_Previous = NULL;
}

inline void PTS_ThreadLocalBinArray::Destruct(PTS_ThreadLocalBinArray *pThis)
{
	for (uint32_t i = 0U; i < s_Bin_Number; ++i)
	{
		PTS_BlockMetadata * const pBlockActive = pThis->m_Bins[i];
		PTS_BlockMetadata * const pBlockFull = (pBlockActive != NULL) ? (pBlockActive->m_Bin_Next) : NULL;

		//Previous: EmptyEnough Block
		PTS_BlockMetadata *pBlockToRemove = pBlockActive;

		while (pBlockToRemove != NULL)
		{
			PTS_BlockMetadata::_FreeListPublicRepatriate(pBlockToRemove);

			PTS_BlockMetadata *pTemp = pBlockToRemove->m_Bin_Previous;
			//简化PTS_ThreadLocalBinArray::Remove
			pBlockToRemove->m_Bin_Next = NULL;
			pBlockToRemove->m_Bin_Previous = NULL;

			//Empty
			if (pBlockToRemove->m_ObjectAllocated_Number == 0U)
			{
				PTS_BlockStore::PushEmpty(&s_BlockStore_Singleton, pBlockToRemove);
			}
			else
			{
				PTS_BlockStore::PushNonEmpty(&s_BlockStore_Singleton, pBlockToRemove->m_Bin_Index, pBlockToRemove);
			}

			pBlockToRemove = pTemp;
		}

		//Next: Full Block
		pBlockToRemove = pBlockFull;

		while (pBlockToRemove != NULL)
		{
			PTS_BlockMetadata::_FreeListPublicRepatriate(pBlockToRemove);

			PTS_BlockMetadata *pTemp = pBlockToRemove->m_Bin_Next;
			//简化PTS_ThreadLocalBinArray::Remove
			pBlockToRemove->m_Bin_Next = NULL;
			pBlockToRemove->m_Bin_Previous = NULL;
			
			//Empty
			if (pBlockToRemove->m_ObjectAllocated_Number == 0U)
			{
				PTS_BlockStore::PushEmpty(&s_BlockStore_Singleton, pBlockToRemove);
			}
			else
			{
				PTS_BlockStore::PushNonEmpty(&s_BlockStore_Singleton, pBlockToRemove->m_Bin_Index, pBlockToRemove);
			}

			pBlockToRemove = pTemp;
		}
	}
}

//------------------------------------------------------------------------------------------------------------
static inline bool PTS_Size_IsPowerOfTwo(uint32_t Value)
{
	return (Value != 0U) && ((Value & (Value - 1U)) == 0U);
}

static bool PTS_Size_IsAligned(size_t Value, size_t Alignment)
{
	return (Value & (Alignment - 1U)) == 0U;
}

static inline size_t PTS_Size_AlignDownFrom(size_t Value, size_t alignment)
{
	return (Value)&(~(alignment - 1U));
}

static inline uint32_t PTS_Size_AlignUpFrom(uint32_t Value, uint32_t Alignment)
{
	return ((Value - 1U) | (Alignment - 1U)) + 1U;
}

static inline uint64_t PTS_Size_AlignUpFrom(uint64_t Value, uint64_t Alignment)
{
	return ((Value - 1U) | (Alignment - 1U)) + 1U;
}

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

//------------------------------------------------------------------------------------------------------------

inline void PTS_BlockMetadata::Construct(PTS_ThreadLocalBinArray *pTLS, PTS_BlockMetadata *pThis, uint32_t BinIndex, uint32_t ObjectSize)
{
	//Private Field
	pThis->m_pTLS = pTLS;
	pThis->m_TID_Owning = ::PTSThreadID_Self();

	pThis->m_Identity = s_BlockIdentity;

	pThis->m_Bin_Next = NULL;
	pThis->m_Bin_Previous = NULL;

	PTS_BlockMetadata::_BumpPointerRestore(pThis, ObjectSize);

	pThis->m_Bin_Index = BinIndex;
	pThis->m_Object_Size = ObjectSize;
	pThis->m_ObjectAllocated_Number = 0U;

	//Public Field
	pThis->m_FreeListPublic = NULL;
}

inline void PTS_BlockMetadata::_BumpPointerRestore(PTS_BlockMetadata *pThis, uint32_t ObjectSize)
{
	assert(pThis->m_ObjectAllocated_Number == 0U);
	//assert(pThis->m_FreeListPublic == NULL);

	pThis->m_FreeListPrivate = NULL;

	pThis->m_BumpPointer = reinterpret_cast<PTS_ObjectMetadata *>(reinterpret_cast<uintptr_t>(pThis) + (s_Block_Size - ObjectSize * ((s_Block_Size - sizeof(PTS_BlockMetadata)) / ObjectSize)));
	assert(reinterpret_cast<uintptr_t>(pThis->m_BumpPointer) >= (reinterpret_cast<uintptr_t>(pThis) + sizeof(PTS_BlockMetadata)));
	assert((reinterpret_cast<uintptr_t>(pThis->m_BumpPointer) + ObjectSize) <= (reinterpret_cast<uintptr_t>(pThis) + s_Block_Size));
}

inline PTS_ObjectMetadata *PTS_BlockMetadata::_BumpPointer(PTS_BlockMetadata *pThis)
{
	if (pThis->m_BumpPointer == NULL)
		return NULL;

	PTS_ObjectMetadata *pObjectToAlloc = pThis->m_BumpPointer;
	
	assert(pThis->m_ObjectAllocated_Number < (s_Block_Size - sizeof(PTS_BlockMetadata)) / pThis->m_Object_Size);
	++pThis->m_ObjectAllocated_Number;

	//Increment
	pThis->m_BumpPointer = reinterpret_cast<PTS_ObjectMetadata *>(reinterpret_cast<uintptr_t>(pThis->m_BumpPointer) + pThis->m_Object_Size);
	
	//Check For OverFlow
	if ((reinterpret_cast<uintptr_t>(pThis->m_BumpPointer) + pThis->m_Object_Size) > (reinterpret_cast<uintptr_t>(pThis) + s_Block_Size))
	{
		pThis->m_BumpPointer = NULL;
	}

	return pObjectToAlloc;
}

inline void PTS_BlockMetadata::_FreeListPrivatePush(PTS_BlockMetadata *pThis, PTS_ObjectMetadata *pObjectToPush)
{
	pObjectToPush->m_FreeList_Next = pThis->m_FreeListPrivate;
	pThis->m_FreeListPrivate = pObjectToPush;
}

inline PTS_ObjectMetadata *PTS_BlockMetadata::_FreeListPrivatePop(PTS_BlockMetadata *pThis)
{
	if (pThis->m_FreeListPrivate == NULL)
		return NULL;

	PTS_ObjectMetadata *pObjectToPop = pThis->m_FreeListPrivate;

	pThis->m_FreeListPrivate = pObjectToPop->m_FreeList_Next;

	assert(pThis->m_ObjectAllocated_Number < (s_Block_Size - sizeof(PTS_BlockMetadata)) / pThis->m_Object_Size);
	++pThis->m_ObjectAllocated_Number;

	return pObjectToPop;
}

inline void PTS_BlockMetadata::_FreeListPublicPush(PTS_BlockMetadata *pThis, PTS_ObjectMetadata *pObjectToPush)
{
	//Foreign Thread
	assert((pThis->m_pTLS != NULL) || (::PTSThreadID_Equal(pThis->m_TID_Owning, ::PTSThreadID_Self()) == PTFALSE));

	//Hudson 2006 / 3.McRT-MALLOC / 3.2 Non-blocking Operations / Figure 2 Public Free List / freeListPush
	//---------------------------------------------------------
	//The public free list holds objects that were freed by foreign threads.
	//Adding an object to a public free list is expensive 
	//since it requires an atomic CAS instruction on a cache line that is likely in another processor’s cache.
	//---------------------------------------------------------
	//To push an object onto the public free list, load the public free list from the blocks metadata,
	//assign the next field in the object to this and do a CAS.
	//If the CAS succeeds we are done, if not, we repeat the process as many times as is required.

	PTS_ObjectMetadata *oldPublicFreeList;
	do
	{
		oldPublicFreeList = pThis->m_FreeListPublic;
		pObjectToPush->m_FreeList_Next = oldPublicFreeList;
	} while (::PTSAtomic_CompareAndSet(reinterpret_cast<uintptr_t *>(&pThis->m_FreeListPublic), reinterpret_cast<uintptr_t>(oldPublicFreeList), reinterpret_cast<uintptr_t>(pObjectToPush)) != reinterpret_cast<uintptr_t>(oldPublicFreeList));
}

inline void PTS_BlockMetadata::_FreeListPublicRepatriate(PTS_BlockMetadata *pThis)
{
	//Owning Thread
	assert((pThis->m_pTLS != NULL) && (::PTSThreadID_Equal(pThis->m_TID_Owning, ::PTSThreadID_Self()) != PTFALSE));

	//In PTS_ThreadLocalBinArray::Destruct When A Thead Dies
	//assert(pThis->m_FreeListPrivate == NULL);

	//Hudson 2006 / 3.McRT-MALLOC / 3.2 Non-blocking Operations / Figure 2 Public Free List / repatriatePublicFreeList
	//---------------------------------------------------------
	//To repatriate the public free list, a thread atomically swaps the public free list field with null (using the IA32 locked exchange instruction), 
	//and sets the private free list field to the loaded public free list value.
	//---------------------------------------------------------
	//This is ABA safe without concern for versioning because the concurrent data structure is a single consumer(the owning thread) and multiple producers.
	//Since neither the consumer nor the producer rely on the values in the objects the faulty assumptions associated with, the ABA problem are avoided.

	pThis->m_FreeListPrivate = reinterpret_cast<PTS_ObjectMetadata *>(::PTSAtomic_GetAndSet(reinterpret_cast<uintptr_t *>(&pThis->m_FreeListPublic), static_cast<uintptr_t>(NULL)));

	PTS_ObjectMetadata *pTemp = pThis->m_FreeListPrivate;
	while (pTemp)
	{
		--pThis->m_ObjectAllocated_Number;
		pTemp = pTemp->m_FreeList_Next;
	}
}

inline PTS_ObjectMetadata *PTS_BlockMetadata::Allocate(PTS_ThreadLocalBinArray *pTLS, PTS_BlockMetadata *pThis)
{
	PTS_ObjectMetadata *pObjectToAlloc;

	//Bump Pointer First
	pObjectToAlloc = PTS_BlockMetadata::_BumpPointer(pThis);
	if (pObjectToAlloc != NULL)
	{
		return pObjectToAlloc;
	}

	//Private FreeList Next //Private FreeList Should Be First For Better Cache Locality (Kukanov 2007) ???
	pObjectToAlloc = PTS_BlockMetadata::_FreeListPrivatePop(pThis);
	if (pObjectToAlloc != NULL)
	{
		return pObjectToAlloc;
	}

	//Repatriate Then
	PTS_BlockMetadata::_FreeListPublicRepatriate(pThis);

	//Empty
	if (pThis->m_ObjectAllocated_Number == 0U)
	{
		assert(pTLS->m_Bins[pThis->m_Bin_Index] /*pBlockActive*/ == pThis);

		PTS_BlockMetadata::_BumpPointerRestore(pThis, pThis->m_Object_Size);

		//Bump Pointer Again
		pObjectToAlloc = PTS_BlockMetadata::_BumpPointer(pThis);
		assert(pObjectToAlloc != NULL);
		return pObjectToAlloc;
	}
	else
	{
		//Private FreeList Again
		pObjectToAlloc = PTS_BlockMetadata::_FreeListPrivatePop(pThis);
		if (pObjectToAlloc != NULL)
		{
			return pObjectToAlloc;
		}

		return NULL;
	}
}

inline void PTS_BlockMetadata::Free(PTS_ThreadLocalBinArray *pTLS, PTS_BlockMetadata *pThis, PTS_ObjectMetadata *pObjectToFree)
{
	if((pThis->m_pTLS != NULL) && (::PTSThreadID_Equal(pThis->m_TID_Owning, ::PTSThreadID_Self()) != PTFALSE)) //Owning Thread
	{
		--pThis->m_ObjectAllocated_Number;
		assert(pThis->m_ObjectAllocated_Number < (s_Block_Size - sizeof(PTS_BlockMetadata)) / pThis->m_Object_Size);

		//Empty
		if (pThis->m_ObjectAllocated_Number == 0U)
		{
			//FreeListPrivate清零，不再有Push的必要
			PTS_BlockMetadata::_BumpPointerRestore(pThis,pThis->m_Object_Size); 

			if (pTLS->m_Bins[pThis->m_Bin_Index] /*pBlockActive*/ != pThis) 
			{
				PTS_ThreadLocalBinArray::Remove(pTLS, pThis->m_Bin_Index, pThis);
				PTS_BlockStore::PushEmpty(&s_BlockStore_Singleton, pThis);
			}
		}
		else
		{
			PTS_BlockMetadata::_FreeListPrivatePush(pThis, pObjectToFree);

			//检查 EmptyEnough Here (Kukanov 2007) ???
			//目前在PTS_Internal_Alloc中进行（Hudson 2006）
		}

	}
	else //Foreign Thread
	{
		PTS_BlockMetadata::_FreeListPublicPush(pThis, pObjectToFree);

	}
}

//------------------------------------------------------------------------------------------------------------
static int32_t s_MemoryAllocator_Initialize_RefCount = 0;
PTBOOL PTCALL PTSMemoryAllocator_Initialize()
{
	if (::PTSAtomic_GetAndAdd(&s_MemoryAllocator_Initialize_RefCount, 1) == 0)
	{
		PTS_BlockStore::Construct(&s_BlockStore_Singleton);

		PTBOOL tbResult = ::PTSTSD_Create(
			&s_TLBinArray_Index,
			[](void *pVoid)->void {
			PTS_ThreadLocalBinArray *pTLBinArray = static_cast<PTS_ThreadLocalBinArray *>(pVoid);
			PTS_ThreadLocalBinArray::Destruct(pTLBinArray);
			::PTS_MemoryMap_Free(pVoid);
		}
		);
		
		return tbResult;
	}
	else
	{
		return PTTRUE;
	}
	
}

static inline void * PTS_Internal_Alloc(uint32_t size)
{
	PTS_ThreadLocalBinArray *pTLBinArray = static_cast<PTS_ThreadLocalBinArray *>(::PTSTSD_GetValue(s_TLBinArray_Index));

	if (pTLBinArray == NULL)
	{
		pTLBinArray = static_cast<PTS_ThreadLocalBinArray *>(::PTS_MemoryMap_Alloc(sizeof(PTS_ThreadLocalBinArray)));
		assert(pTLBinArray != NULL);

		PTS_ThreadLocalBinArray::Construct(pTLBinArray);

		PTBOOL tbResult = ::PTSTSD_SetValue(s_TLBinArray_Index, pTLBinArray);
		assert(tbResult != PTFALSE);
	}

	if (size <= s_NonLargeObject_SizeMax)
	{
		uint32_t BinIndex;
		uint32_t ObjectSize;
		::PTS_Size_ResolveRequestSize(&BinIndex, &ObjectSize, size);

		PTS_ObjectMetadata *pObjectToAlloc = NULL;

		//Bin
		//Traverse
		PTS_BlockMetadata ** const ppBlockActive = &pTLBinArray->m_Bins[BinIndex];
		PTS_BlockMetadata *pBlockEmptyEnough = NULL;
		PTS_BlockMetadata *pBlockFull = NULL;
		if ((*ppBlockActive) != NULL)
		{
			pBlockEmptyEnough = (*ppBlockActive)->m_Bin_Previous;
			pBlockFull = (*ppBlockActive)->m_Bin_Next;

		//Active
			pObjectToAlloc = PTS_BlockMetadata::Allocate(pTLBinArray, (*ppBlockActive));
			if (pObjectToAlloc != NULL)
			{
				return pObjectToAlloc;
			}
		}

		//Previous: EmptyEnough Block
		while (pBlockEmptyEnough != NULL)
		{
			(*ppBlockActive) = pBlockEmptyEnough;//Active前移

			pObjectToAlloc = PTS_BlockMetadata::Allocate(pTLBinArray, pBlockEmptyEnough);
			
			if (pObjectToAlloc != NULL)
			{
				return pObjectToAlloc;
			}

			pBlockEmptyEnough = pBlockEmptyEnough->m_Bin_Previous;
		}

		//Next: Full Block
		while (pBlockFull != NULL)
		{
			PTS_BlockMetadata::_FreeListPublicRepatriate(pBlockFull); //Be Here Or Be In Free ???

			//EmptyEnough
			if ((pBlockFull->m_Object_Size*pBlockFull->m_ObjectAllocated_Number) <= (((s_Block_Size - sizeof(PTS_BlockMetadata)) / 5U) * 4U))
			{
				//Move From "Next" To "Previous"
				PTS_ThreadLocalBinArray::Remove(pTLBinArray, pBlockFull->m_Bin_Index, pBlockFull);
				PTS_ThreadLocalBinArray::Insert(pTLBinArray, pBlockFull->m_Bin_Index, pBlockFull);

				(*ppBlockActive) = pBlockFull;//Active前移

				pObjectToAlloc = PTS_BlockMetadata::Allocate(pTLBinArray, pBlockFull);
				assert(pObjectToAlloc != NULL);
				return pObjectToAlloc;
			}

			pBlockFull = pBlockFull->m_BlockStore_Next;
		}

		//UnOwned Yet NonEmpty Block
		PTS_BlockMetadata *pBlockAdded = PTS_BlockStore::PopNonEmpty(pTLBinArray, &s_BlockStore_Singleton, BinIndex);
		while (pBlockAdded != NULL)
		{
			PTS_ThreadLocalBinArray::Insert(pTLBinArray, pBlockAdded->m_Bin_Index, pBlockAdded);

			(*ppBlockActive) = pBlockAdded;//Active前移

			pObjectToAlloc = PTS_BlockMetadata::Allocate(pTLBinArray, pBlockAdded);
			if (pObjectToAlloc != NULL)
			{
				return pObjectToAlloc;
			}

			pBlockAdded = PTS_BlockStore::PopNonEmpty(pTLBinArray, &s_BlockStore_Singleton, BinIndex);
		}

		//Empty Block
		pBlockAdded = PTS_BlockStore::PopEmpty(pTLBinArray, &s_BlockStore_Singleton, BinIndex, ObjectSize);
		assert(pBlockAdded != NULL);

		PTS_ThreadLocalBinArray::Insert(pTLBinArray, pBlockAdded->m_Bin_Index, pBlockAdded);
		
		(*ppBlockActive) = pBlockAdded;//Active前移

		pObjectToAlloc = PTS_BlockMetadata::Allocate(pTLBinArray, pBlockAdded);
		assert(pObjectToAlloc != NULL);
		return pObjectToAlloc;
	}
	else
	{
		PTS_ObjectMetadata *pObjectToAlloc = static_cast<PTS_ObjectMetadata *>(::PTS_MemoryMap_Alloc(size));

		//确保LargeObject一定对齐到16KB，以确保在PTS_Internal_Free中访问BlockAssumed（假定的）一定不会发生冲突
		assert(PTS_Size_IsAligned(reinterpret_cast<uintptr_t>(pObjectToAlloc), s_Block_Size));

		return pObjectToAlloc;
	}
}

static inline void PTS_Internal_Free(void *pVoid)
{
	if (pVoid != NULL)
	{
		//在PTS_Internal_Alloc中确保LargeObject一定对齐到16KB，因此访问BlockAssumed（假定的）一定不会发生冲突
		PTS_BlockMetadata *pBlockAssumed = reinterpret_cast<PTS_BlockMetadata *>(PTS_Size_AlignDownFrom(reinterpret_cast<uintptr_t>(pVoid), static_cast<size_t>(s_Block_Size)));

		//区分Block和LargeObject，并不一定可靠
		if (pBlockAssumed->m_Identity == PTS_BlockMetadata::s_BlockIdentity) //Block
		{
			//并不存在偏移，见PTSMemoryAllocator_Alloc_Aligned
			PTS_ObjectMetadata *pObjectToFree = static_cast<PTS_ObjectMetadata *>(pVoid);

			PTS_BlockMetadata::Free(pBlockAssumed->m_pTLS, pBlockAssumed, pObjectToFree);
		}
		else //LargeObject
		{
			::PTS_MemoryMap_Free(pVoid);
		}
	}
}

void * PTCALL PTSMemoryAllocator_Alloc(uint32_t size)
{
	assert(::PTSAtomic_Get(&s_MemoryAllocator_Initialize_RefCount) > 0);

	return ::PTS_Internal_Alloc(size);
}

void PTCALL PTSMemoryAllocator_Free(void *pVoid)
{
	assert(::PTSAtomic_Get(&s_MemoryAllocator_Initialize_RefCount) > 0);

	return ::PTS_Internal_Free(pVoid);
}

void * PTCALL PTSMemoryAllocator_Alloc_Aligned(uint32_t size, uint32_t alignment)
{
	assert(::PTSAtomic_Get(&s_MemoryAllocator_Initialize_RefCount) > 0);

	assert(::PTS_Size_IsPowerOfTwo(alignment));

	void *pVoidToAlloc;

	if (size <= s_NonLargeObject_SizeMax && alignment <= s_NonLargeObject_SizeMax)
	{
		//并不存在偏移
		//we just align the size up, and request this amount, 
		//because for every size aligned to some power of 2, 
		//the allocated object is at least that aligned.
		pVoidToAlloc = ::PTS_Internal_Alloc(::PTS_Size_AlignUpFrom(size, alignment));
	}
	else
	{
		//一般情况下极少发生
		pVoidToAlloc = ::PTS_Internal_Alloc(size);
		if (pVoidToAlloc != NULL && !PTS_Size_IsAligned(reinterpret_cast<uintptr_t>(pVoidToAlloc), static_cast<size_t>(alignment)))
		{
			PTS_Internal_Free(pVoidToAlloc);
			pVoidToAlloc = NULL;
		}
	}
	
	return pVoidToAlloc;
}

void PTCALL PTSMemoryAllocator_Free_Aligned(void *pVoid)
{
	assert(::PTSAtomic_Get(&s_MemoryAllocator_Initialize_RefCount) > 0);

	return ::PTS_Internal_Free(pVoid);
}

#if defined PTWIN32
#include "Win32/PTSMemoryAllocator.inl"
#elif defined PTPOSIX
#include "Posix/PTSMemoryAllocator.inl"
#else
#error 未知的平台
#endif