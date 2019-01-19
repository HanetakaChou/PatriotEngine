#include "../../Public/System/PTSThread.h"
#include "../../Public/System/PTSMemoryAllocator.h"
#include "../../Public/System/PTSMemory.h"
#include <stddef.h> 
#include <assert.h>
#include <string.h>
#include <type_traits>

//Richard L. Hudson, Bratin Saha, Ali-Reza Adl-Tabatabai, Benjamin C. Hertzberg. "McRT-Malloc: a scalable transactional memory allocator". Proceedings of the 5th international symposium on Memory management ACM 2006.
//Alexey Kukanov, Michael J.Voss. "The Foundations for Scalable Multi-core Software in Intel Threading Building Blocks." Intel Technology Journal, Volume11, Issue 4 2007.

//Operating System Primitive
static inline void *PTS_MemoryMap_Alloc(uint32_t size);
static inline void PTS_MemoryMap_Free(void *pVoid);
//To Do: 不同Map可能连续 在Windows中得到的结果可能大于实际分配的值 在多线程环境下 线程B可能在线程A读取（Calloc）时释放
static inline uint32_t PTS_MemoryMap_Size(void *pVoid);

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
//in particular to avoid the ABA problem


//Intel TBB
//Front-End McRT Malloc (Nature Is A Bucket Allocator)
//Back-End Slab Allocator (Nature A Buddy Allocator) //We Use System Call "FileMemoryMap" Directly As The Paper "McRT Malloc" Suggests

#define PTS_USE_QUEUEEMPTY 1

//When a thread dies, the scheduler notifies McRT-Malloc,（作为PTSTSD_DESTRUCTOR触发）
//which then returns the dead thread’s blocks back to the block store.

bool PTCALL PTSMemoryAllocator_Initialize();

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

struct PTS_BucketObjectHeader;
struct PTS_BucketBlockHeader;
class PTS_BucketBlockManager;
class PTS_ThreadLocalBucketAllocator;

//Hudson 2006 / 3.McRT-MALLOC
//The array contains a bin for every multiple of four between 4 bytes and 64 bytes, 
//and a bin for every power of two between 128 bytes and 8096 bytes.
//Allocation of objects greater than 8096 bytes is handled by the operating system(e.g., on Linux we use mmap and munmap).
//Large objects are rare so we don’t discuss them further.

//严格意义上16KB应当是对齐而非大小
static uint32_t const s_Block_Size = 16U * 1024U;

//Part1 [ 8/16/32/48/64 ]
//Part2 [ 80/96/112/128 || 60/192/224/256 || 320/384/448/512 || 640/768/896/1024 || 1280/1536/1792/2048 || 2560/3072/3584/4096 || 5120/6144/7168/8192 ]
//static uint32_t const s_Bucket_Number = 32U; //5+27=32
static uint32_t const s_Bucket_Number = 32U;
static uint32_t const s_NonLargeObject_SizeMax = 7168U;

static inline bool PTS_Size_IsPowerOfTwo(uint32_t Value);
static inline bool PTS_Size_IsAligned(size_t Value, size_t Alignment);
static inline size_t PTS_Size_AlignDownFrom(size_t Value, size_t alignment);
static inline uint32_t PTS_Size_AlignUpFrom(uint32_t Value, uint32_t Alignment);
static inline uint64_t PTS_Size_AlignUpFrom(uint64_t Value, uint64_t Alignment);
static inline uint32_t PTS_Size_BitScanReverse(uint32_t Value);
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

static class PTS_BucketBlockManager
{
	uint64_t m_QueueNonEmptyTops[s_Bucket_Number];

#if PTS_USE_QUEUEEMPTY
	uint64_t m_QueueEmptyTop;
#endif

private:
	static inline void _Unpack(uint64_t *pVersionNumber, PTS_BucketBlockHeader **pBlockAddress, uint64_t Value);

	static inline uint64_t _Pack(uint64_t VersionNumber, PTS_BucketBlockHeader *BlockAddress);

	static inline void _AtomicPush(uint64_t *pQueueTop, PTS_BucketBlockHeader *blockPtr);

	static inline PTS_BucketBlockHeader *_AtomicPop(uint64_t *pQueueTop, uint32_t BinIndex);

	friend struct PTS_BucketBlockHeader;
public:
	inline void Construct();

	inline void PushEmpty(PTS_BucketBlockHeader *pBlockToPush);

	inline void PushNonEmpty(PTS_BucketBlockHeader *pBlockToPush);

	inline PTS_BucketBlockHeader *PopNonEmpty(uint32_t BinIndex);

	inline PTS_BucketBlockHeader *PopEmpty(uint32_t BinIndex);

}s_BlockStore_Singleton;
//static_assert(std::is_pod<PTS_BucketBlockManager>::value, "PTS_BucketBlockManager Is Not A POD");

class PTS_ThreadLocalBucketAllocator
{
	PTS_BucketBlockHeader *m_BucketVector[s_Bucket_Number]; //Bucket=BlockDoublyLinkedList
public:
	inline void Construct();

	inline PTS_BucketObjectHeader *Alloc(uint32_t Size);

	inline void Insert(PTS_BucketBlockHeader *pBlockToAdd, uint32_t BinIndex);

	inline void Remove(PTS_BucketBlockHeader *pBlockToRemove, uint32_t BinIndex);

	inline void OwnEmpty(PTS_BucketBlockHeader *pBlockToOwn, uint32_t BinIndex, uint32_t ObjectSize);

	inline void OwnNonEmpty(PTS_BucketBlockHeader *pBlockToOwn);

	inline void LoseEmpty(PTS_BucketBlockHeader *pBlockToOwn);

	inline void LoseNonEmpty(PTS_BucketBlockHeader *pBlockToOwn);

	inline void Destruct();
};
//static_assert(std::is_pod<PTS_ThreadLocalBucketAllocator>::value, "PTS_ThreadLocalBucketAllocator Is Not A POD");

struct PTS_BucketBlockHeader
{
private:
	PTS_BucketBlockHeader *m_pNext_InBlockManager;
	friend void PTS_BucketBlockManager::_AtomicPush(uint64_t *pQueueTop, PTS_BucketBlockHeader *blockPtr);
	friend PTS_BucketBlockHeader *PTS_BucketBlockManager::_AtomicPop(uint64_t *pQueueTop, uint32_t BinIndex);
	friend void PTS_BucketBlockManager::PushEmpty(PTS_BucketBlockHeader *pBlockToPush);
	friend void PTS_BucketBlockManager::PushNonEmpty(PTS_BucketBlockHeader *pBlockToPush);

	PTS_BucketObjectHeader *m_ObjectFreeVector_Public; //ObjectVector=FreeList
	
	//To Avoid False Sharing，将可能被Foreign线程访问的Public域和只能被Owner线程访问的Private域分隔在不同的缓存行中
	uint8_t __PaddingForPublicFields[s_CacheLine_Size - sizeof(void*) * 2U];

	//NULL Means The "m_ThreadID_Owner" Is Invalid
	//同时 在缓存TLS后 PTS_Internal_Free中不再需要调用PTSTSD_GetValue
	PTS_ThreadLocalBucketAllocator *m_pAllocator_Owner;

	PTS_BucketBlockHeader *m_pNext_InBucket; //Full Block //Bucket=BlockDoublyLinkedList
	PTS_BucketBlockHeader *m_pPrevious_InBucket; //Empty Enough Block //Bucket=BlockDoublyLinkedList
	friend void PTS_ThreadLocalBucketAllocator::Insert(PTS_BucketBlockHeader *pBlockToAdd, uint32_t BinIndex);
	friend void PTS_ThreadLocalBucketAllocator::Remove(PTS_BucketBlockHeader *pBlockToRemove, uint32_t BinIndex);
	friend PTS_BucketObjectHeader *PTS_ThreadLocalBucketAllocator::Alloc(uint32_t Size);
	friend void PTS_ThreadLocalBucketAllocator::Destruct();

	PTS_BucketObjectHeader *m_ObjectFreeVector_Private; //ObjectVector=FreeList
	PTS_BucketObjectHeader *m_BumpPointer;
	
	//Valid Only When "m_pAllocator_Owner" Is Not NULL
	PTSThreadID m_ThreadID_Owner;

	uint32_t m_ObjectSize; //Free时用到
	uint32_t Pm_BucketIndex_InAllocator;
	uint32_t m_ObjectAllocated_Number;
	
#ifndef NDEBUG
	//不一定可靠，但很大概率可以用于区分Block和LargeObject
	uint32_t m_Identity_Block;
#endif

	//To Fill In Cache Line To 分隔Metadata和用户数据
#ifndef NDEBUG
	uint8_t __PaddingForPrivateFields[s_CacheLine_Size - sizeof(uint32_t) * 4U - sizeof(PTSThreadID) - sizeof(void*) * 4U - sizeof(void*)];
#else
	uint8_t __PaddingForPrivateFields[s_CacheLine_Size - sizeof(uint32_t) * 3U - sizeof(PTSThreadID) - sizeof(void*) * 4U - sizeof(void*)];
#endif

	friend void PTS_ThreadLocalBucketAllocator::OwnEmpty(PTS_BucketBlockHeader *pBlockToOwn, uint32_t BinIndex, uint32_t ObjectSize);
	friend void PTS_ThreadLocalBucketAllocator::OwnNonEmpty(PTS_BucketBlockHeader *pBlockToOwn);
	friend void PTS_ThreadLocalBucketAllocator::LoseEmpty(PTS_BucketBlockHeader *pBlockToOwn);
	friend void PTS_ThreadLocalBucketAllocator::LoseNonEmpty(PTS_BucketBlockHeader *pBlockToOwn);

#ifndef NDEBUG
	static uint32_t const s_Identity_Block = 0X5765C7B8U;
	static PTSThreadID const s_ThreadID_Invalid = PTSThreadID(5201314U);
#endif

private:
	inline void _BumpPointerRestore(uint32_t ObjectSize);

	inline PTS_BucketObjectHeader *_BumpPointer();

	inline void _ObjectFreeVector_Private_Push(PTS_BucketObjectHeader *pObjectToPush);

	inline PTS_BucketObjectHeader *_ObjectFreeVector_Private_Pop();
	
	//Foreign Thread
	inline void _ObjectFreeVector_Public_Push(PTS_BucketObjectHeader *pObjectToPush); 

	friend struct PTS_BucketObjectHeader;

public:
	inline void Construct();

	inline void LostByOwner();

	inline void RemoveFromOwner();

	inline bool IsEmpty();

	inline bool IsEmptyEnough();

	inline uint32_t ObjectSize();

	inline void ObjectFreeVector_Public_Repatriate();

	inline PTS_BucketObjectHeader *Alloc();

	inline void Free(PTS_BucketObjectHeader *pObjectToFree);

#ifndef NDEBUG
	inline bool IsBlock();
#endif
	static inline constexpr bool StaticAssertPaddingForPublicFields()
	{
		return offsetof(PTS_BucketBlockHeader, m_pAllocator_Owner) == s_CacheLine_Size;
	}

};
//static_assert(std::is_pod<PTS_BucketBlockHeader>::value, "PTS_BucketBlockHeader Is Not A POD");
static_assert(PTS_BucketBlockHeader::StaticAssertPaddingForPublicFields(), "Padding For Public Fields Of PTS_BucketBlockHeader Is Not Correct");
static_assert(sizeof(PTS_BucketBlockHeader) == s_CacheLine_Size * 2U, "Padding For Private Fields Of PTS_BucketBlockHeader Is Not Correct");

struct PTS_BucketObjectHeader
{
private:
	PTS_BucketObjectHeader *m_pNext_InFreeVector;
	friend void PTS_BucketBlockHeader::_ObjectFreeVector_Private_Push(PTS_BucketObjectHeader *pObjectToPush);
	friend PTS_BucketObjectHeader *PTS_BucketBlockHeader::_ObjectFreeVector_Private_Pop();
	friend void PTS_BucketBlockHeader::_ObjectFreeVector_Public_Push(PTS_BucketObjectHeader *pObjectToPush);
	friend void PTS_BucketBlockHeader::ObjectFreeVector_Public_Repatriate();
};
//static_assert(std::is_pod<PTS_BucketObjectHeader>::value, "PTS_BucketObjectHeader Is Not A POD");


//********************************************************************************************************************************************************************************************************************************************************************
//PTS_BucketBlockManager
//********************************************************************************************************************************************************************************************************************************************************************

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
//在Linux（https://www.kernel.org/doc/Documentation/arm64/memory.txt）中，用户空间只有512GB（39位）/256TB（48位）/4TB（42位）/*目前全部按照48位处理*/，块地址只需要34（48-14=34）位，可以使用64位CAS指令，其余30位用作版本号，环绕需要6小时（基于论文中46位需要2年计算）

#if defined(PTX86) || defined(PTARM)
inline void PTS_BucketBlockManager::_Unpack(uint64_t *pVersionNumber, PTS_BucketBlockHeader **pBlockAddress, uint64_t Value)
{
	assert(s_Block_Size == (1U << 14U));

	//46位    //18位
	//Version //Address

	(*pVersionNumber) = ((Value) >> 18U);
	(*pBlockAddress) = reinterpret_cast<PTS_BucketBlockHeader *>((Value & 0X1FFFFU) << 14U);
}
inline uint64_t PTS_BucketBlockManager::_Pack(uint64_t VersionNumber, PTS_BucketBlockHeader *BlockAddress)
{
	assert(s_Block_Size == (1U << 14U));
	assert((reinterpret_cast<uintptr_t>(BlockAddress) & 0X00003FFFU) == 0U);

	//47位    //17位（前1后14）
	//Version //Address

	return (VersionNumber << 17U) | (reinterpret_cast<uintptr_t>(BlockAddress) >> 14U);
}
#elif defined(PTX64)
#if defined(PTWIN32)
inline void PTS_BucketBlockManager::_Unpack(uint64_t *pVersionNumber, PTS_BucketBlockHeader **pBlockAddress, uint64_t Value)
{
	//35位    //29位
	//Version //Address

	(*pVersionNumber) = ((Value) >> 29U);
	(*pBlockAddress) = reinterpret_cast<PTS_BucketBlockHeader *>((Value & 0X1FFFFFFFU) << 14U);
}

inline uint64_t PTS_BucketBlockManager::_Pack(uint64_t VersionNumber, PTS_BucketBlockHeader *BlockAddress)
{
	assert(s_Block_Size == (1U << 14U));
	assert((reinterpret_cast<uintptr_t>(BlockAddress) & 0XFFFFF80000003FFFU) == 0U);

	//35位    //29位（前21后14）
	//Version //Address

	return (VersionNumber << 29U) | (reinterpret_cast<uintptr_t>(BlockAddress) >> 14U);
}
#elif defined(PTPOSIX)
inline void PTS_BucketBlockManager::_Unpack(uint64_t *pVersionNumber, PTS_BucketBlockHeader **pBlockAddress, uint64_t Value)
{
	//31位    //33位
	//Version //Address

	(*pVersionNumber) = ((Value) >> 33U);
	(*pBlockAddress) = reinterpret_cast<PTS_BucketBlockHeader *>((Value & 0X1FFFFFFFFU) << 14U);
}

inline uint64_t PTS_BucketBlockManager::_Pack(uint64_t VersionNumber, PTS_BucketBlockHeader *BlockAddress)
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
inline void PTS_BucketBlockManager::_Unpack(uint64_t *pVersionNumber, PTS_BucketBlockHeader **pBlockAddress, uint64_t Value)
{
	//30位    //34位
	//Version //Address

	(*pVersionNumber) = ((Value) >> 34U);
	(*pBlockAddress) = reinterpret_cast<PTS_BucketBlockHeader *>((Value & 0X3FFFFFFFFU) << 14U);
}

inline uint64_t PTS_BucketBlockManager::_Pack(uint64_t VersionNumber, PTS_BucketBlockHeader *BlockAddress)
{
	assert(s_Block_Size == (1U << 14U));
	assert((reinterpret_cast<uintptr_t>(BlockAddress) & 0XFFFF000000003FFFU) == 0U);

	//30位    //34位（前16后14）
	//Version //Address

	return (VersionNumber << 34U) | (reinterpret_cast<uintptr_t>(BlockAddress) >> 14U);
}
#else
#error 未知的平台
#endif
#else
#error 未知的架构
#endif

inline void PTS_BucketBlockManager::_AtomicPush(uint64_t *pQueueTop, PTS_BucketBlockHeader *blockPtr)
{
	//Hudson 2006 / 3.McRT-MALLOC / 3.2 Non-blocking Operations / Figure 1 Non-blocking queues / lifoQueuePush

	uint64_t oldValue;
	uint64_t newValue;

	do {
		oldValue = (*pQueueTop);

		uint64_t oldCount;
		PTS_BucketBlockHeader *oldBlockPtr;
		_Unpack(&oldCount, &oldBlockPtr, oldValue);

		uint64_t newCount = oldCount + 1;
		blockPtr->m_pNext_InBlockManager = oldBlockPtr;

		newValue = _Pack(newCount, blockPtr);
	} while (::PTSAtomic_CompareAndSet(pQueueTop, oldValue, newValue) != oldValue);
}

inline PTS_BucketBlockHeader *PTS_BucketBlockManager::_AtomicPop(uint64_t *pQueueTop, uint32_t BinIndex)
{
	//Hudson 2006 / 3.McRT-MALLOC / 3.2 Non-blocking Operations / Figure 1 Non-blocking queues / lifoQueuePop

	uint64_t oldQueue;
	uint64_t newQueue;
	PTS_BucketBlockHeader *blockPtr;

	do {
		oldQueue = (*pQueueTop);

		uint64_t oldCount;
		_Unpack(&oldCount, &blockPtr, oldQueue);

		if (blockPtr == NULL) //The Queue is empty
		{
			break;
		}

		uint64_t newCount = oldCount + 1;
		PTS_BucketBlockHeader *newBlockPtr = blockPtr->m_pNext_InBlockManager;

		newQueue = _Pack(newCount, newBlockPtr);
	} while (::PTSAtomic_CompareAndSet(pQueueTop, oldQueue, newQueue) != oldQueue);

	return blockPtr;
}

inline void PTS_BucketBlockManager::Construct()
{
	for (uint32_t i = 0U; i < s_Bucket_Number; ++i)
	{
		m_QueueNonEmptyTops[i] = 0U;
	}

#if PTS_USE_QUEUEEMPTY
	m_QueueEmptyTop = 0U;
#endif
}

inline void PTS_BucketBlockManager::PushEmpty(PTS_BucketBlockHeader *pBlockToPush)
{
#if PTS_USE_QUEUEEMPTY
	//Private Field
	assert(pBlockToPush->m_pAllocator_Owner == NULL);
	assert(pBlockToPush->m_ObjectFreeVector_Private == NULL);
	assert(pBlockToPush->m_BumpPointer == NULL);
	assert(pBlockToPush->m_ThreadID_Owner == PTS_BucketBlockHeader::s_ThreadID_Invalid);
	assert(pBlockToPush->m_ObjectSize == ~uint32_t(0U));
	assert(pBlockToPush->Pm_BucketIndex_InAllocator == ~uint32_t(0U));
	assert(pBlockToPush->m_pNext_InBucket == NULL);
	assert(pBlockToPush->m_pPrevious_InBucket == NULL);
	assert(pBlockToPush->m_ObjectAllocated_Number == 0U);

	//Public Field
	assert(pBlockToPush->m_ObjectFreeVector_Public == NULL);
	_AtomicPush(&m_QueueEmptyTop, pBlockToPush);
#else
	::PTS_MemoryMap_Free(pBlockToPush);
#endif
}

inline void PTS_BucketBlockManager::PushNonEmpty(PTS_BucketBlockHeader *pBlockToPush)
{
	assert(pBlockToPush->m_pAllocator_Owner == NULL);
	assert(pBlockToPush->m_ThreadID_Owner == PTS_BucketBlockHeader::s_ThreadID_Invalid);
	assert(pBlockToPush->m_pNext_InBucket == NULL);
	assert(pBlockToPush->m_pPrevious_InBucket == NULL);
	assert(pBlockToPush->m_ObjectAllocated_Number != 0U);

	_AtomicPush(&m_QueueNonEmptyTops[pBlockToPush->Pm_BucketIndex_InAllocator], pBlockToPush);
}

inline PTS_BucketBlockHeader *PTS_BucketBlockManager::PopNonEmpty(uint32_t BinIndex)
{
	PTS_BucketBlockHeader *pBlockAdded = _AtomicPop(&m_QueueNonEmptyTops[BinIndex], BinIndex);

	return pBlockAdded;
}

inline PTS_BucketBlockHeader *PTS_BucketBlockManager::PopEmpty(uint32_t BinIndex)
{
#if PTS_USE_QUEUEEMPTY
	PTS_BucketBlockHeader *pBlockAdded = _AtomicPop(&m_QueueEmptyTop, BinIndex);

	if (pBlockAdded == NULL)
	{
		//启发式，而非在初始化时一次性分配
		pBlockAdded = static_cast<PTS_BucketBlockHeader *>(::PTS_MemoryMap_Alloc(s_Block_Size));
		assert(pBlockAdded != NULL);

		//严格意义上16KB应当是对齐而非大小
		assert(PTS_Size_IsAligned(reinterpret_cast<uintptr_t>(pBlockAdded), s_Block_Size));

		pBlockAdded->Construct();
	}
	return pBlockAdded;
#else
	PTS_BucketBlockHeader *pBlockAdded = static_cast<PTS_BucketBlockHeader *>(::PTS_MemoryMap_Alloc(s_Block_Size));

	//严格意义上16KB应当是对齐而非大小
	assert(PTS_Size_IsAligned(reinterpret_cast<uintptr_t>(pBlockAdded), s_Block_Size));

	PTS_BucketBlockHeader::Construct();

	return pBlockAdded;
#endif
}






//********************************************************************************************************************************************************************************************************************************************************************
//PTS_ThreadLocalBucketAllocator
//********************************************************************************************************************************************************************************************************************************************************************

inline void PTS_ThreadLocalBucketAllocator::Construct()
{
	for (uint32_t i = 0U; i < s_Bucket_Number; ++i)
	{
		m_BucketVector[i] = NULL;
	}
}

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
	//Part1↓-----------------------------
	//8                 1000

	//Part2↓-----------------------------
	//16              10|000
	//24              11|000
	//32             100|000
	//48             110|000   

	//Part3↓-----------------------------
	//64            100|0000
	//80           0101|0000
	//96           0110|0000‬
	//112          0111|0000‬

	//128          100|00000
	//160         0101|00000‬
	//192    ‭     0110|00000‬
	//224         0111|00000

	//256         100|000000                   
	//320        0101|000000
	//384       ‭ 0110|000000‬
	//448       ‭ 0111|000000‬

	//512        100|0000000
	//640       0101|0000000
	//768       0110|0000000
	//896       0111|0000000

	//1024      100|00000000
	//1280     0101|00000000
	//1536     0110|00000000
	//1792     0111|00000000

	//2048     100|000000000
	//2560    0101|000000000
	//3072    0110|000000000
	//3584    0111|000000000

	//4096    100|0000000000
	//5120   0101|0000000000
	//6144   0110|0000000000
	//7168   0111|0000000000

	//8192   100|00000000000		

	static uint32_t const s_PartI_SizeMax = 64U;
	static uint32_t const s_PartI_BinNumber = 5U;

	if (RequestSize <= s_PartI_SizeMax)
	{
		static uint32_t const s_IndexArray[8] = { 0U,1U,2U,2U,3U,3U,4U,4U };
		(*pBinIndex) = s_IndexArray[(RequestSize - 1U) >> 3U];

		static uint32_t const s_SizeArray[5] = { 8U,16U,32U,48U,64U };
		(*pObjectSize) = s_SizeArray[(*pBinIndex)];
	}
	else if (RequestSize <= s_NonLargeObject_SizeMax)
	{


		uint32_t Order = ::PTS_Size_BitScanReverse(RequestSize - 1U);
		assert(Order >= 6U && Order <= 12U);

		static const uint32_t s_Order_PartITotal = 6U;
		static const uint32_t s_Order_PartIIGroup = 2U;

		(*pBinIndex) = s_PartI_BinNumber +
			(1U << s_Order_PartIIGroup) * (Order - s_Order_PartITotal) +  //64:[Order=6] 80/96/112/128 -> 4*(6-6)=0 || 60/192/224/256 -> 4*(7-6)=4 || 320/384/448/512 -> 4*(8-6)=8 || 640/768/896/1024 -> 4*(9-6)=12 || 1280/1536/1792/2048 -> 4*(10-6)=16 || 2560/3072/3584/4096 -> 4*(11-6)=20 || 5120/6144/7168/8192 -> 4*(12-6)=24
			((RequestSize - 1U) >> (Order - s_Order_PartIIGroup)) - (1U << s_Order_PartIIGroup); //最高三位 100 -> 4-4=0 || 101 -> 5-4=1 || 110 -> 6-4=2 || 111 -> 7-4=3

		uint32_t AlignmentMin = 1U << (Order - s_Order_PartIIGroup);
		assert(AlignmentMin == (1024U >> (12U - Order)));
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

inline PTS_BucketObjectHeader *PTS_ThreadLocalBucketAllocator::Alloc(uint32_t Size)
{
	uint32_t BinIndex;
	uint32_t ObjectSize;
	::PTS_Size_ResolveRequestSize(&BinIndex, &ObjectSize, Size);

	PTS_BucketObjectHeader *pObjectToAlloc = NULL;

	//Test Current Active
	PTS_BucketBlockHeader ** const ppBlockActive = &m_BucketVector[BinIndex];
	PTS_BucketBlockHeader *pBlockEmptyEnough = NULL;
	PTS_BucketBlockHeader *pBlockFull = NULL;
	if ((*ppBlockActive) != NULL)
	{
		pBlockEmptyEnough = (*ppBlockActive)->m_pPrevious_InBucket;
		pBlockFull = (*ppBlockActive)->m_pNext_InBucket;

		//assert(m_BucketVector[(*ppBlockActive)->Pm_BucketIndex_InAllocator] == (*ppBlockActive));
		pObjectToAlloc = (*ppBlockActive)->Alloc();
		if (pObjectToAlloc != NULL)
		{
			return pObjectToAlloc;
		}
	}

	//Traverse Previous: EmptyEnough Block
	while (pBlockEmptyEnough != NULL)
	{
		(*ppBlockActive) = pBlockEmptyEnough; //Reset Active 

		//assert(m_BucketVector[pBlockEmptyEnough->Pm_BucketIndex_InAllocator] == pBlockEmptyEnough);
		pObjectToAlloc = pBlockEmptyEnough->Alloc();

		if (pObjectToAlloc != NULL)
		{
			return pObjectToAlloc;
		}

		pBlockEmptyEnough = pBlockEmptyEnough->m_pPrevious_InBucket;
	}

	//Traverse Next: Full Block
	while (pBlockFull != NULL)
	{
		//Be Here Or Be In Free ???
		pBlockFull->ObjectFreeVector_Public_Repatriate();

		PTS_BucketBlockHeader *Temp = pBlockFull->m_pNext_InBucket;

		//EmptyEnough
		if (pBlockFull->IsEmptyEnough())
		{
			//Move From "Next" To "Previous"
			Remove(pBlockFull, BinIndex);
			Insert(pBlockFull, BinIndex);

			(*ppBlockActive) = pBlockFull; //Reset Active 

			//assert(m_BucketVector[pBlockFull->Pm_BucketIndex_InAllocator] == pBlockFull);
			pObjectToAlloc = pBlockFull->Alloc();
			assert(pObjectToAlloc != NULL);
			return pObjectToAlloc;
		}

		pBlockFull = Temp;
	}

	//NoOwner-NonEmpty Block
	PTS_BucketBlockHeader *pBlockAdded = s_BlockStore_Singleton.PopNonEmpty(BinIndex);
	if (pBlockAdded != NULL)
	{
		OwnNonEmpty(pBlockAdded);
		Insert(pBlockAdded, BinIndex);

		(*ppBlockActive) = pBlockAdded; //Active前移

		//assert(m_BucketVector[pBlockAdded->Pm_BucketIndex_InAllocator] == pBlockAdded);
		pObjectToAlloc = pBlockAdded->Alloc();
		if (pObjectToAlloc != NULL)
		{
			return pObjectToAlloc;
		}
	}

	//(NoOwner-)Empty Block
	pBlockAdded = s_BlockStore_Singleton.PopEmpty(BinIndex);
	//内存不足???
	assert(pBlockAdded != NULL);

	if (pBlockAdded != NULL)
	{
		OwnEmpty(pBlockAdded, BinIndex, ObjectSize);
		Insert(pBlockAdded, BinIndex);

		(*ppBlockActive) = pBlockAdded; //Active前移

		//assert(m_BucketVector[pBlockAdded->Pm_BucketIndex_InAllocator] == pBlockAdded);
		pObjectToAlloc = pBlockAdded->Alloc();
		//The Block Is Empty???
		assert(pObjectToAlloc != NULL);

		if (pObjectToAlloc != NULL)
		{
			return pObjectToAlloc;
		}
	}

	return NULL;
}

inline void PTS_ThreadLocalBucketAllocator::Insert(PTS_BucketBlockHeader *pBlockToAdd, uint32_t BinIndex)
{
	//The Block Is Owned By Allocator And Current Thread 
	assert((pBlockToAdd->m_pAllocator_Owner = this) && (::PTSThreadID_Equal(pBlockToAdd->m_ThreadID_Owner, ::PTSThreadID_Self()) != false));

	//Size Consistent
	assert(pBlockToAdd->m_pNext_InBucket == NULL);
	assert(pBlockToAdd->m_pPrevious_InBucket == NULL);
	assert(pBlockToAdd->Pm_BucketIndex_InAllocator == BinIndex);
	assert(pBlockToAdd->Pm_BucketIndex_InAllocator < s_Bucket_Number);

	PTS_BucketBlockHeader *const pBlockActive = m_BucketVector[BinIndex];

	//The Block Is Owned By Allocator And Current Thread 
	assert(pBlockActive == NULL || ((pBlockActive->m_pAllocator_Owner == this) && (::PTSThreadID_Equal(pBlockActive->m_ThreadID_Owner, ::PTSThreadID_Self()) != false)));

	//Size Consistent
	assert(pBlockActive == NULL || pBlockActive->Pm_BucketIndex_InAllocator == pBlockToAdd->Pm_BucketIndex_InAllocator);

	if (pBlockActive == NULL)
	{
		m_BucketVector[BinIndex] = pBlockToAdd; //Active初始化
	}

	//pBlockToAdd位于pBlockActive的Previous
	pBlockToAdd->m_pNext_InBucket = pBlockActive;
	if (pBlockActive != NULL)
	{
		pBlockToAdd->m_pPrevious_InBucket = pBlockActive->m_pPrevious_InBucket;
		pBlockActive->m_pPrevious_InBucket = pBlockToAdd;

		if (pBlockToAdd->m_pPrevious_InBucket != NULL)
		{
			pBlockToAdd->m_pPrevious_InBucket->m_pNext_InBucket = pBlockToAdd;
		}
	}
}

inline void PTS_ThreadLocalBucketAllocator::Remove(PTS_BucketBlockHeader *pBlockToRemove, uint32_t BinIndex)
{
	//The Block Is Owned By Allocator And Current Thread 
	assert((pBlockToRemove->m_pAllocator_Owner == this) && (::PTSThreadID_Equal(pBlockToRemove->m_ThreadID_Owner, ::PTSThreadID_Self()) != false));

	//Size Consistent
	assert(pBlockToRemove->Pm_BucketIndex_InAllocator == BinIndex);
	assert(pBlockToRemove->Pm_BucketIndex_InAllocator < s_Bucket_Number);

	PTS_BucketBlockHeader * const pBlockActive = m_BucketVector[BinIndex];

	//The Block Is Owned By Allocator And Current Thread 
	assert(pBlockActive == NULL || ((pBlockActive->m_pAllocator_Owner == this) && (::PTSThreadID_Equal(pBlockActive->m_ThreadID_Owner, ::PTSThreadID_Self()) != false)));

	//Size Consistent
	assert(pBlockActive == NULL || pBlockActive->Pm_BucketIndex_InAllocator == BinIndex);

	if (pBlockActive == pBlockToRemove)
	{
		//Previous（即EmptyEnough）优先于Next（即Full）
		pBlockToRemove->m_pAllocator_Owner->m_BucketVector[BinIndex] = (pBlockToRemove->m_pPrevious_InBucket != NULL) ? pBlockToRemove->m_pPrevious_InBucket : pBlockToRemove->m_pNext_InBucket;
	}

	if (pBlockToRemove->m_pPrevious_InBucket != NULL)
	{
		assert(pBlockToRemove->m_pPrevious_InBucket->m_pNext_InBucket == pBlockToRemove);
		pBlockToRemove->m_pPrevious_InBucket->m_pNext_InBucket = pBlockToRemove->m_pNext_InBucket;
	}

	if (pBlockToRemove->m_pNext_InBucket != NULL)
	{
		assert(pBlockToRemove->m_pNext_InBucket->m_pPrevious_InBucket == pBlockToRemove);
		pBlockToRemove->m_pNext_InBucket->m_pPrevious_InBucket = pBlockToRemove->m_pPrevious_InBucket;
	}

	pBlockToRemove->m_pNext_InBucket = NULL;
	pBlockToRemove->m_pPrevious_InBucket = NULL;
}

inline void PTS_ThreadLocalBucketAllocator::OwnEmpty(PTS_BucketBlockHeader *pBlockToOwn, uint32_t BinIndex, uint32_t ObjectSize)
{
	//Private Field
	assert(pBlockToOwn->m_pAllocator_Owner == NULL);
	assert(pBlockToOwn->m_ObjectFreeVector_Private == NULL);
	assert(pBlockToOwn->m_BumpPointer == NULL);
	assert(pBlockToOwn->m_ThreadID_Owner == PTS_BucketBlockHeader::s_ThreadID_Invalid);
	assert(pBlockToOwn->m_ObjectSize == ~uint32_t(0U));
	assert(pBlockToOwn->Pm_BucketIndex_InAllocator == ~uint32_t(0U));
	assert(pBlockToOwn->m_pNext_InBucket == NULL);
	assert(pBlockToOwn->m_pPrevious_InBucket == NULL);
	assert(pBlockToOwn->m_ObjectAllocated_Number == 0U);

	pBlockToOwn->m_pAllocator_Owner = this;

	pBlockToOwn->m_ThreadID_Owner = ::PTSThreadID_Self();

	pBlockToOwn->m_ObjectSize = ObjectSize;

	pBlockToOwn->Pm_BucketIndex_InAllocator = BinIndex;

	pBlockToOwn->_BumpPointerRestore(ObjectSize);

	//Public Field
	assert(pBlockToOwn->m_ObjectFreeVector_Public == NULL);
}

inline void PTS_ThreadLocalBucketAllocator::OwnNonEmpty(PTS_BucketBlockHeader *pBlockToOwn)
{
	assert(pBlockToOwn->m_pAllocator_Owner == NULL);
	assert(pBlockToOwn->m_ThreadID_Owner == PTS_BucketBlockHeader::s_ThreadID_Invalid);
	assert(pBlockToOwn->m_pNext_InBucket == NULL);
	assert(pBlockToOwn->m_pPrevious_InBucket == NULL);
	assert(pBlockToOwn->m_ObjectAllocated_Number != 0U);

	pBlockToOwn->m_pAllocator_Owner = this;
	pBlockToOwn->m_ThreadID_Owner = ::PTSThreadID_Self();
}

inline void PTS_ThreadLocalBucketAllocator::LoseEmpty(PTS_BucketBlockHeader *pBlockToOwn)
{
	//The Block Is Owned By Current Thread 
	assert((pBlockToOwn->m_pAllocator_Owner == this) && (::PTSThreadID_Equal(pBlockToOwn->m_ThreadID_Owner, ::PTSThreadID_Self()) != false));
	
	//Private Field
	assert(pBlockToOwn->m_pNext_InBucket == NULL);
	assert(pBlockToOwn->m_pPrevious_InBucket == NULL);
	assert(pBlockToOwn->m_ObjectAllocated_Number == 0U);

	pBlockToOwn->m_pAllocator_Owner = NULL;

#ifndef NDEBUG
	pBlockToOwn->m_ThreadID_Owner = PTS_BucketBlockHeader::s_ThreadID_Invalid;
	pBlockToOwn->m_ObjectFreeVector_Private = NULL;
	pBlockToOwn->m_BumpPointer = NULL;
	pBlockToOwn->m_ObjectSize = ~uint32_t(0U);
	pBlockToOwn->Pm_BucketIndex_InAllocator = ~uint32_t(0U);
#endif

	//Public Field
	assert(pBlockToOwn->m_ObjectFreeVector_Public == NULL);
}

inline void PTS_ThreadLocalBucketAllocator::LoseNonEmpty(PTS_BucketBlockHeader *pBlockToOwn)
{
	//The Block Is Owned By Current Thread 
	assert((pBlockToOwn->m_pAllocator_Owner == this) && (::PTSThreadID_Equal(pBlockToOwn->m_ThreadID_Owner, ::PTSThreadID_Self()) != false));

	//Private Field
	assert(pBlockToOwn->m_pNext_InBucket == NULL);
	assert(pBlockToOwn->m_pPrevious_InBucket == NULL);
	assert(pBlockToOwn->m_ObjectAllocated_Number != 0U);

	pBlockToOwn->m_pAllocator_Owner = NULL;
#ifndef NDEBUG
	pBlockToOwn->m_ThreadID_Owner = PTS_BucketBlockHeader::s_ThreadID_Invalid;
#endif
}

inline void PTS_ThreadLocalBucketAllocator::Destruct()
{
	for (uint32_t BinIndex = 0U; BinIndex < s_Bucket_Number; ++BinIndex)
	{
		PTS_BucketBlockHeader * const pBlockActive = m_BucketVector[BinIndex];
		PTS_BucketBlockHeader * const pBlockFull = (pBlockActive != NULL) ? (pBlockActive->m_pNext_InBucket) : NULL;

		//Previous: EmptyEnough Block
		PTS_BucketBlockHeader *pBlockToRemove = pBlockActive;

		while (pBlockToRemove != NULL)
		{
			pBlockToRemove->ObjectFreeVector_Public_Repatriate();

			PTS_BucketBlockHeader *pTemp = pBlockToRemove->m_pPrevious_InBucket;
			
			Remove(pBlockToRemove, BinIndex);

			if (!pBlockToRemove->IsEmpty())
			{
				LoseNonEmpty(pBlockToRemove);
				s_BlockStore_Singleton.PushNonEmpty(pBlockToRemove);
			}
			else
			{
				LoseEmpty(pBlockToRemove);
				s_BlockStore_Singleton.PushEmpty(pBlockToRemove);
			}

			pBlockToRemove = pTemp;
		}

		//Next: Full Block
		pBlockToRemove = pBlockFull;

		while (pBlockToRemove != NULL)
		{
			pBlockToRemove->ObjectFreeVector_Public_Repatriate();

			PTS_BucketBlockHeader *pTemp = pBlockToRemove->m_pNext_InBucket;
			
			pBlockToRemove->RemoveFromOwner();

			if (!pBlockToRemove->IsEmpty())
			{
				LoseNonEmpty(pBlockToRemove);
				s_BlockStore_Singleton.PushNonEmpty(pBlockToRemove);
			}
			else
			{
				LoseEmpty(pBlockToRemove);
				s_BlockStore_Singleton.PushEmpty(pBlockToRemove);
			}


			pBlockToRemove = pTemp;
		}
	}
}





//********************************************************************************************************************************************************************************************************************************************************************
//PTS_BucketBlockHeader
//********************************************************************************************************************************************************************************************************************************************************************

inline void PTS_BucketBlockHeader::_BumpPointerRestore(uint32_t ObjectSize)
{
	assert(m_ObjectAllocated_Number == 0U);
	assert(m_ObjectFreeVector_Public == NULL);

	m_ObjectFreeVector_Private = NULL;

	m_BumpPointer = reinterpret_cast<PTS_BucketObjectHeader *>(reinterpret_cast<uintptr_t>(this) + (s_Block_Size - m_ObjectSize * ((s_Block_Size - sizeof(PTS_BucketBlockHeader)) / m_ObjectSize)));
	assert(reinterpret_cast<uintptr_t>(m_BumpPointer) >= (reinterpret_cast<uintptr_t>(this) + sizeof(PTS_BucketBlockHeader)));
	assert((reinterpret_cast<uintptr_t>(m_BumpPointer) + ObjectSize) <= (reinterpret_cast<uintptr_t>(this) + s_Block_Size));
}

inline PTS_BucketObjectHeader *PTS_BucketBlockHeader::_BumpPointer()
{
	//The Block Is Owned By Current Thread 
	assert((m_pAllocator_Owner != NULL) && (::PTSThreadID_Equal(m_ThreadID_Owner, ::PTSThreadID_Self()) != false));

	if (m_BumpPointer == NULL)
	{
		//OverFlow
		return NULL;
	}

	PTS_BucketObjectHeader *pObjectToAlloc = m_BumpPointer;

	++m_ObjectAllocated_Number;
	assert(m_ObjectAllocated_Number <= (s_Block_Size - sizeof(PTS_BucketBlockHeader)) / m_ObjectSize);

	//Increment
	m_BumpPointer = reinterpret_cast<PTS_BucketObjectHeader *>(reinterpret_cast<uintptr_t>(m_BumpPointer) + m_ObjectSize);

	//Check For OverFlow
	if ((reinterpret_cast<uintptr_t>(m_BumpPointer) + m_ObjectSize) > (reinterpret_cast<uintptr_t>(this) + s_Block_Size))
	{
		m_BumpPointer = NULL;
	}

	return pObjectToAlloc;
}

inline void PTS_BucketBlockHeader::_ObjectFreeVector_Private_Push(PTS_BucketObjectHeader *pObjectToPush)
{
	//The Block Is Owned By Current Thread 
	assert((m_pAllocator_Owner != NULL) && (::PTSThreadID_Equal(m_ThreadID_Owner, ::PTSThreadID_Self()) != false));

	pObjectToPush->m_pNext_InFreeVector = m_ObjectFreeVector_Private;
	m_ObjectFreeVector_Private = pObjectToPush;
}

inline PTS_BucketObjectHeader *PTS_BucketBlockHeader::_ObjectFreeVector_Private_Pop()
{
	//The Block Is Owned By Current Thread 
	assert((m_pAllocator_Owner != NULL) && (::PTSThreadID_Equal(m_ThreadID_Owner, ::PTSThreadID_Self()) != false));

	if (m_ObjectFreeVector_Private == NULL)
	{
		return NULL;
	}

	PTS_BucketObjectHeader *pObjectToPop = m_ObjectFreeVector_Private;

	m_ObjectFreeVector_Private = pObjectToPop->m_pNext_InFreeVector;

	++m_ObjectAllocated_Number;
	assert(m_ObjectAllocated_Number <= (s_Block_Size - sizeof(PTS_BucketBlockHeader)) / m_ObjectSize);

	return pObjectToPop;
}

inline void PTS_BucketBlockHeader::_ObjectFreeVector_Public_Push(PTS_BucketObjectHeader *pObjectToPush)
{
	//Foreign Thread
	assert((m_pAllocator_Owner == NULL) || (::PTSThreadID_Equal(m_ThreadID_Owner, ::PTSThreadID_Self()) == false));

	//Hudson 2006 / 3.McRT-MALLOC / 3.2 Non-blocking Operations / Figure 2 Public Free List / freeListPush
	//---------------------------------------------------------
	//The public free list holds objects that were freed by foreign threads.
	//Adding an object to a public free list is expensive 
	//since it requires an atomic CAS instruction on a cache line that is likely in another processor’s cache.
	//---------------------------------------------------------
	//To push an object onto the public free list, load the public free list from the blocks metadata,
	//assign the next field in the object to this and do a CAS.
	//If the CAS succeeds we are done, if not, we repeat the process as many times as is required.

	PTS_BucketObjectHeader *oldPublicFreeList;
	do
	{
		oldPublicFreeList = m_ObjectFreeVector_Public;
		pObjectToPush->m_pNext_InFreeVector = oldPublicFreeList;
	} while (::PTSAtomic_CompareAndSet(reinterpret_cast<uintptr_t *>(&m_ObjectFreeVector_Public), reinterpret_cast<uintptr_t>(oldPublicFreeList), reinterpret_cast<uintptr_t>(pObjectToPush)) != reinterpret_cast<uintptr_t>(oldPublicFreeList));
}

inline void PTS_BucketBlockHeader::Construct()
{
	//Private Field
	m_pAllocator_Owner = NULL;
#ifndef NDEBUG
	m_ObjectFreeVector_Private = NULL;
	m_BumpPointer = NULL;
	m_ThreadID_Owner = s_ThreadID_Invalid;
	m_ObjectSize = ~uint32_t(0U);
	Pm_BucketIndex_InAllocator = ~uint32_t(0U);
#endif
	m_pNext_InBucket = NULL;
	m_pPrevious_InBucket = NULL;
	m_ObjectAllocated_Number = 0U;
#ifndef NDEBUG
	m_Identity_Block = s_Identity_Block;
#endif
	//Public Field
	m_pNext_InBlockManager = NULL;
	m_ObjectFreeVector_Public = NULL;
}

inline void PTS_BucketBlockHeader::LostByOwner()
{
	//The Block Is Owned By Current Thread 
	assert((m_pAllocator_Owner != NULL) && (::PTSThreadID_Equal(m_ThreadID_Owner, ::PTSThreadID_Self()) != false));

	if (!IsEmpty())
	{
		m_pAllocator_Owner->LoseNonEmpty(this);
	}
	else
	{
		m_pAllocator_Owner->LoseEmpty(this);
	}
}

inline void PTS_BucketBlockHeader::RemoveFromOwner()
{
	m_pAllocator_Owner->Remove(this, Pm_BucketIndex_InAllocator);
}

inline bool PTS_BucketBlockHeader::IsEmpty()
{
	return m_ObjectAllocated_Number == 0U;
}

inline bool PTS_BucketBlockHeader::IsEmptyEnough()
{
	return ((m_ObjectSize*m_ObjectAllocated_Number) <= (((s_Block_Size - sizeof(PTS_BucketBlockHeader)) / 5U) * 4U)) && ((m_ObjectSize*(m_ObjectAllocated_Number + 1U)) <= (s_Block_Size - sizeof(PTS_BucketBlockHeader)));
}

inline uint32_t PTS_BucketBlockHeader::ObjectSize()
{
	return m_ObjectSize;
}

inline void PTS_BucketBlockHeader::ObjectFreeVector_Public_Repatriate()
{
	//The Block Is Owned By Current Thread 
	assert((m_pAllocator_Owner != NULL) && (::PTSThreadID_Equal(m_ThreadID_Owner, ::PTSThreadID_Self()) != false));

	//In PTS_ThreadLocalBucketAllocator::Destruct When A Thead Dies
	//assert(pThis->m_ObjectFreeVector_Private == NULL);

	//Hudson 2006 / 3.McRT-MALLOC / 3.2 Non-blocking Operations / Figure 2 Public Free List / repatriatePublicFreeList
	//---------------------------------------------------------
	//To repatriate the public free list, a thread atomically swaps the public free list field with null (using the IA32 locked exchange instruction), 
	//and sets the private free list field to the loaded public free list value.
	//---------------------------------------------------------
	//This is ABA safe without concern for versioning because the concurrent data structure is a single consumer(the owning thread) and multiple producers.
	//Since neither the consumer nor the producer rely on the values in the objects the faulty assumptions associated with, the ABA problem are avoided.


	PTS_BucketObjectHeader *FreeListPublic_Begin = reinterpret_cast<PTS_BucketObjectHeader *>(::PTSAtomic_GetAndSet(reinterpret_cast<uintptr_t *>(&m_ObjectFreeVector_Public), static_cast<uintptr_t>(NULL)));

	if (FreeListPublic_Begin != NULL)
	{
		--m_ObjectAllocated_Number;
		assert(m_ObjectAllocated_Number <= (s_Block_Size - sizeof(PTS_BucketBlockHeader)) / m_ObjectSize);

		PTS_BucketObjectHeader **FreeListPublic_End = &FreeListPublic_Begin->m_pNext_InFreeVector;
		while ((*FreeListPublic_End))
		{
			--m_ObjectAllocated_Number;
			assert(m_ObjectAllocated_Number <= (s_Block_Size - sizeof(PTS_BucketBlockHeader)) / m_ObjectSize);

			FreeListPublic_End = &((*FreeListPublic_End)->m_pNext_InFreeVector);
		}

		(*FreeListPublic_End) = m_ObjectFreeVector_Private;
		m_ObjectFreeVector_Private = FreeListPublic_Begin;
	}
}

inline PTS_BucketObjectHeader *PTS_BucketBlockHeader::Alloc()
{
	//The Block Is Owned By Current Thread 
	assert((m_pAllocator_Owner != NULL) && (::PTSThreadID_Equal(m_ThreadID_Owner, ::PTSThreadID_Self()) != false));

	PTS_BucketObjectHeader *pObjectToAlloc;

	//Bump Pointer First
	pObjectToAlloc = _BumpPointer();
	if (pObjectToAlloc != NULL)
	{
		return pObjectToAlloc;
	}

	//Private FreeList Next //Private FreeList Should Be First For Better Cache Locality (Kukanov 2007) ???
	pObjectToAlloc = _ObjectFreeVector_Private_Pop();
	if (pObjectToAlloc != NULL)
	{
		return pObjectToAlloc;
	}

	//Repatriate Then
	assert(m_ObjectFreeVector_Private == NULL);
	ObjectFreeVector_Public_Repatriate();

	//Empty
	if (m_ObjectAllocated_Number == 0U)
	{
		_BumpPointerRestore(this->m_ObjectSize);

		//Bump Pointer Again
		pObjectToAlloc = _BumpPointer();
		assert(pObjectToAlloc != NULL);
		return pObjectToAlloc;
	}
	else
	{
		//Private FreeList Again
		pObjectToAlloc = _ObjectFreeVector_Private_Pop();
		if (pObjectToAlloc != NULL)
		{
			return pObjectToAlloc;
		}

		return NULL;
	}
}

inline void PTS_BucketBlockHeader::Free(PTS_BucketObjectHeader *pObjectToFree)
{
	if ((m_pAllocator_Owner != NULL) && (::PTSThreadID_Equal(m_ThreadID_Owner, ::PTSThreadID_Self()) != false))
	{
		//Owning Thread
		--m_ObjectAllocated_Number;
		assert(m_ObjectAllocated_Number <= (s_Block_Size - sizeof(PTS_BucketBlockHeader)) / m_ObjectSize);

		//It Can Save Memory But Also Will Take Time 
		//PTS_BucketBlockHeader::ObjectFreeVector_Public_Repatriate(pThis);

		if (m_ObjectAllocated_Number == 0U)
		{
			//Empty

			//FreeListPrivate清零，不再有Push的必要
			//assert(pThis->m_ObjectFreeVector_Public == NULL);
			//pThis->PTS_BucketBlockHeader::_BumpPointerRestore(pThis->m_ObjectSize);

			RemoveFromOwner();

			LostByOwner();

			s_BlockStore_Singleton.PushEmpty(this);

			//如果不Repatriate
			//那么实际中出现概率会较低

			//一般认为Free是一个轻量级的操作？
			//不Push到BlockStore
		}
		else
		{
			//Non_Empty

			_ObjectFreeVector_Private_Push(pObjectToFree);

			//检查 EmptyEnough Here (Kukanov 2007) ???
			//目前在PTS_Internal_Alloc中进行（Hudson 2006）
		}

	}
	else
	{
		//Foreign Thread
		_ObjectFreeVector_Public_Push(pObjectToFree);
	}
}

#ifndef NDEBUG
inline bool PTS_BucketBlockHeader::IsBlock()
{
	return m_Identity_Block == s_Identity_Block;
}

#endif

//********************************************************************************************************************************************************************************************************************************************************************
//API Entry
//********************************************************************************************************************************************************************************************************************************************************************

static PTSTSD_KEY s_ThreadLocalBucketAllocator_Index;
static int32_t s_MemoryAllocator_Initialize_RefCount = 0;
bool PTCALL PTSMemoryAllocator_Initialize()
{
	if (::PTSAtomic_GetAndAdd(&s_MemoryAllocator_Initialize_RefCount, 1) == 0)
	{
		s_BlockStore_Singleton.Construct();

		bool tbResult = ::PTSTSD_Create(
			&s_ThreadLocalBucketAllocator_Index,
			[](void *pVoid)->void {
			PTS_ThreadLocalBucketAllocator *pThreadLocalBucketAllocator = static_cast<PTS_ThreadLocalBucketAllocator *>(pVoid);
			pThreadLocalBucketAllocator->Destruct();
			::PTS_MemoryMap_Free(pVoid);
		}
		);
		
		return tbResult;
	}
	else
	{
		return true;
	}
}

static inline void * PTS_Internal_Alloc(uint32_t Size)
{
	PTS_ThreadLocalBucketAllocator *pThreadLocalBucketAllocator = static_cast<PTS_ThreadLocalBucketAllocator *>(::PTSTSD_GetValue(s_ThreadLocalBucketAllocator_Index));

	if (pThreadLocalBucketAllocator == NULL)
	{
		pThreadLocalBucketAllocator = static_cast<PTS_ThreadLocalBucketAllocator *>(::PTS_MemoryMap_Alloc(sizeof(PTS_ThreadLocalBucketAllocator)));
		assert(pThreadLocalBucketAllocator != NULL);

		pThreadLocalBucketAllocator->Construct();

		bool tbResult = ::PTSTSD_SetValue(s_ThreadLocalBucketAllocator_Index, pThreadLocalBucketAllocator);
		assert(tbResult != false);
	}
	
	if (Size <= s_NonLargeObject_SizeMax)
	{
		return pThreadLocalBucketAllocator->Alloc(Size);		
	}
	else
	{
		PTS_BucketObjectHeader *pObjectToAlloc = static_cast<PTS_BucketObjectHeader *>(::PTS_MemoryMap_Alloc(Size));
		assert(pObjectToAlloc != NULL);

		//确保LargeObject一定对齐到16KB，以确保在PTS_Internal_Free中访问BlockAssumed（假定的）一定不会发生冲突
		assert(PTS_Size_IsAligned(reinterpret_cast<uintptr_t>(pObjectToAlloc), s_Block_Size));

		return pObjectToAlloc;
	}
}

static inline void PTS_Internal_Free(void *pVoid)
{
	//在PTS_Internal_Alloc中确保LargeObject一定对齐到16KB，因此访问BlockAssumed（假定的）一定不会发生冲突
	PTS_BucketBlockHeader *pBlockAssumed = reinterpret_cast<PTS_BucketBlockHeader *>(PTS_Size_AlignDownFrom(reinterpret_cast<uintptr_t>(pVoid), static_cast<size_t>(s_Block_Size)));

	//根据PTS_BlockMetadata::_BumpPointerRestore
	if (reinterpret_cast<uintptr_t>(pBlockAssumed)!= reinterpret_cast<uintptr_t>(pVoid))
	{
		assert(pBlockAssumed->IsBlock());

		//并不存在偏移，见PTSMemoryAllocator_Alloc_Aligned
		PTS_BucketObjectHeader *pObjectToFree = static_cast<PTS_BucketObjectHeader *>(pVoid);

		pBlockAssumed->Free(pObjectToFree);
	}
	else //LargeObject
	{
		//并不一定可靠
		//但相等的概率理论上只有1/(2^32-1)
		assert(!pBlockAssumed->IsBlock());

		::PTS_MemoryMap_Free(pVoid);
	}
}

static inline void * PTS_Internal_Alloc_Aligned(uint32_t Size, uint32_t Alignment)
{
	void *pVoidToAlloc;

	if (Size <= s_NonLargeObject_SizeMax && Alignment <= s_NonLargeObject_SizeMax)
	{
		//并不存在偏移
		//we just align the size up, and request this amount, 
		//because for every size aligned to some power of 2, 
		//the allocated object is at least that aligned.
		//见PTS_Size_ResolveRequestSize
		pVoidToAlloc = ::PTS_Internal_Alloc(::PTS_Size_AlignUpFrom(Size, Alignment));
	}
	else
	{
		//一般情况下极少发生
		pVoidToAlloc = ::PTS_Internal_Alloc(Size);
		if (pVoidToAlloc != NULL && !::PTS_Size_IsAligned(reinterpret_cast<uintptr_t>(pVoidToAlloc), static_cast<size_t>(Alignment)))
		{
			PTS_Internal_Free(pVoidToAlloc);
			pVoidToAlloc = NULL;
		}
	}

	return pVoidToAlloc;
}

void * PTCALL PTSMemoryAllocator_Alloc(uint32_t Size)
{
	assert(::PTSAtomic_Get(&s_MemoryAllocator_Initialize_RefCount) > 0);

	if (Size != 0U)
	{
		return ::PTS_Internal_Alloc(Size);
	}
	else
	{
		return NULL;
	}
}

void PTCALL PTSMemoryAllocator_Free(void *pVoid)
{
	assert(::PTSAtomic_Get(&s_MemoryAllocator_Initialize_RefCount) > 0);

	if (pVoid != NULL)
	{
		::PTS_Internal_Free(pVoid);
	}
}

void * PTCALL PTSMemoryAllocator_Realloc(void *pVoidOld, uint32_t SizeNew)
{
	assert(::PTSAtomic_Get(&s_MemoryAllocator_Initialize_RefCount) > 0);

	if (pVoidOld != NULL && SizeNew != 0U)
	{
		//在PTS_Internal_Alloc中确保LargeObject一定对齐到16KB，因此访问BlockAssumed（假定的）一定不会发生冲突
		PTS_BucketBlockHeader *pBlockAssumed = reinterpret_cast<PTS_BucketBlockHeader *>(PTS_Size_AlignDownFrom(reinterpret_cast<uintptr_t>(pVoidOld), static_cast<size_t>(s_Block_Size)));

		//根据PTS_BlockMetadata::_BumpPointerRestore
		if (reinterpret_cast<uintptr_t>(pBlockAssumed) != reinterpret_cast<uintptr_t>(pVoidOld))
		{
			//Block

			assert(pBlockAssumed->IsBlock());

			uint32_t SizeOld = pBlockAssumed->ObjectSize();
			if (SizeOld < SizeNew)
			{
				void *pVoidNew = ::PTS_Internal_Alloc(SizeNew);
				assert(pVoidNew != NULL);
				if (pVoidNew != NULL)
				{
					uint32_t SizeCopy = (SizeNew > SizeOld) ? SizeOld : SizeNew;
					::PTS_MemoryCopy(pVoidNew, pVoidOld, SizeCopy);

					//并不存在偏移，见PTSMemoryAllocator_Alloc_Aligned
					PTS_BucketObjectHeader *pObjectToFree = static_cast<PTS_BucketObjectHeader *>(pVoidOld);
					pBlockAssumed->Free(pObjectToFree);
				}
				return pVoidNew;
			}
			else
			{
				return pVoidOld;
			}
		}
		else 
		{
			//LargeObject

			//并不一定可靠
			//但相等的概率理论上只有1/(2^32-1)
			assert(!pBlockAssumed->IsBlock());

			uint32_t SizeOld = ::PTS_MemoryMap_Size(pVoidOld);
			if (SizeOld < SizeNew)
			{
				void *pVoidNew = ::PTS_Internal_Alloc(SizeNew);
				assert(pVoidNew != NULL);
				if (pVoidNew != NULL)
				{
					uint32_t SizeCopy = (SizeNew > SizeOld) ? SizeOld : SizeNew;
					::PTS_MemoryCopy(pVoidNew, pVoidOld, SizeCopy);

					::PTS_MemoryMap_Free(pVoidOld);
				}
				return pVoidNew;
			}
			else
			{
				return pVoidOld;
			}
		}
	}
	else if(SizeNew != 0U)
	{
		return ::PTS_Internal_Alloc(SizeNew);
	}
	else if (pVoidOld != NULL)
	{
		::PTS_Internal_Free(pVoidOld);
		return NULL;
	}
	else
	{
		return NULL;
	}
}

void * PTCALL PTSMemoryAllocator_Alloc_Aligned(uint32_t Size, uint32_t Alignment)
{
	assert(::PTSAtomic_Get(&s_MemoryAllocator_Initialize_RefCount) > 0);

	assert(::PTS_Size_IsPowerOfTwo(Alignment));

	if (Size != 0U)
	{
		return ::PTS_Internal_Alloc_Aligned(Size, Alignment);
	}
	else
	{
		return NULL;
	}
}

void PTCALL PTSMemoryAllocator_Free_Aligned(void *pVoid)
{
	assert(::PTSAtomic_Get(&s_MemoryAllocator_Initialize_RefCount) > 0);
	
	if (pVoid != NULL)
	{
		::PTS_Internal_Free(pVoid);
	}
}

void * PTCALL PTSMemoryAllocator_Realloc_Aligned(void *pVoidOld, uint32_t SizeNew, uint32_t AlignmentNew)
{
	assert(::PTSAtomic_Get(&s_MemoryAllocator_Initialize_RefCount) > 0);

	assert(::PTS_Size_IsPowerOfTwo(AlignmentNew));

	if (pVoidOld != NULL && SizeNew != 0U)
	{
		//在PTS_Internal_Alloc中确保LargeObject一定对齐到16KB，因此访问BlockAssumed（假定的）一定不会发生冲突
		PTS_BucketBlockHeader *pBlockAssumed = reinterpret_cast<PTS_BucketBlockHeader *>(PTS_Size_AlignDownFrom(reinterpret_cast<uintptr_t>(pVoidOld), static_cast<size_t>(s_Block_Size)));
		
		if (reinterpret_cast<uintptr_t>(pBlockAssumed) != reinterpret_cast<uintptr_t>(pVoidOld))
		{
			//Block

			assert(pBlockAssumed->IsBlock());

			uint32_t SizeOld = pBlockAssumed->ObjectSize();
			if (SizeOld < SizeNew || !::PTS_Size_IsAligned(reinterpret_cast<uintptr_t>(pVoidOld), static_cast<size_t>(AlignmentNew)))
			{
				void *pVoidNew = ::PTS_Internal_Alloc_Aligned(SizeNew, AlignmentNew);
				assert((pVoidNew != NULL));
				if(pVoidNew != NULL)
				{
					uint32_t SizeCopy = (SizeNew > SizeOld) ? SizeOld : SizeNew;
					::PTS_MemoryCopy(pVoidNew, pVoidOld, SizeCopy);

					//并不存在偏移，见PTSMemoryAllocator_Alloc_Aligned
					PTS_BucketObjectHeader *pObjectToFree = static_cast<PTS_BucketObjectHeader *>(pVoidOld);
					pBlockAssumed->Free(pObjectToFree);
				}
				return pVoidNew;
			}
			else
			{
				return pVoidOld;
			}
		}
		else
		{
			//LargeObject

			//并不一定可靠
			//但相等的概率理论上只有1/(2^32-1)
			assert(!pBlockAssumed->IsBlock());

			uint32_t SizeOld = ::PTS_MemoryMap_Size(pVoidOld);
			if (SizeOld < SizeNew || !::PTS_Size_IsAligned(reinterpret_cast<uintptr_t>(pVoidOld), static_cast<size_t>(AlignmentNew)))
			{
				void *pVoidNew = ::PTS_Internal_Alloc_Aligned(SizeNew, AlignmentNew);
				assert(pVoidNew != NULL);
				if (pVoidNew != NULL)
				{
					uint32_t SizeCopy = (SizeNew > SizeOld) ? SizeOld : SizeNew;
					::PTS_MemoryCopy(pVoidNew, pVoidOld, SizeCopy);

					::PTS_MemoryMap_Free(pVoidOld);
				}
				return pVoidNew;
			}
			else
			{
				return pVoidOld;
			}
		}
	}
	else if (SizeNew != 0U)
	{
		return ::PTS_Internal_Alloc_Aligned(SizeNew, AlignmentNew);
	}
	else if (pVoidOld != NULL)
	{
		::PTS_Internal_Free(pVoidOld);
		return NULL;
	}
	else
	{
		return NULL;
	}
}

#if defined PTWIN32
#include "Win32/PTSMemoryAllocator.inl"
#elif defined PTPOSIX
#include "Posix/PTSMemoryAllocator.inl"
#else
#error 未知的平台
#endif