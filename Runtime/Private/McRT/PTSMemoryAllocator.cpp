#include "../../Public/McRT/PTSThread.h"
#include "../../Public/McRT/PTSMemoryAllocator.h"
#include "../../Public/McRT/PTSMemory.h"
#include <stddef.h> 
#include <assert.h>
#include <string.h>
//#include <type_traits>

//Richard L. Hudson, Bratin Saha, Ali-Reza Adl-Tabatabai, Benjamin C. Hertzberg. "McRT-Malloc: a scalable transactional memory allocator". Proceedings of the 5th international symposium on Memory management ACM 2006.
//Alexey Kukanov, Michael J.Voss. "The Foundations for Scalable Multi-core Software in Intel Threading Building Blocks." Intel Technology Journal, Volume11, Issue 4 2007.

//Platform-Dependent OS Primitive
static inline void *PTS_MemoryMap_Alloc(uint32_t size); //一定对齐到16KB
static inline void PTS_MemoryMap_Free(void *pVoid);
static inline uint32_t PTS_MemoryMap_Size(void *pVoid);

//Intel TBB
//Front-End McRT Malloc (Nature Is A Bucket Allocator)
//Back-End Slab Allocator (Nature A Buddy Allocator) //We Use System Call "FileMemoryMap" Directly As The Paper "McRT Malloc" Suggests

struct PTS_BucketObjectHeader;
struct PTS_BucketBlockHeader;
class PTS_ThreadLocalBucketAllocator;
class PTS_BucketBlockManager;

//Hudson 2006 / 3.McRT-MALLOC

//PTS_BucketBlockManager
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
//--------------------------------------------------------
//作为PTSTSD_DESTRUCTOR触发
//When a thread dies, the scheduler notifies McRT-Malloc,
//which then returns the dead thread’s blocks back to the block store.

//PTS_ThreadLocalBucketAllocator
//--------------------------------------------------------
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
//--------------------------------------------------------
//The array contains a bin for every multiple of four between 4 bytes and 64 bytes, 
//and a bin for every power of two between 128 bytes and 8096 bytes.
//Allocation of objects greater than 8096 bytes is handled by the operating system(e.g., on Linux we use mmap and munmap).
//Large objects are rare so we don’t discuss them further.

//PTS_BucketBlockHeader
//--------------------------------------------------------
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

//严格意义上16KB应当是对齐而非大小
static constexpr uint32_t const s_Block_Size = 16U * 1024U;

//See Funtion "PTS_Size_ResolveRequestSize"
static constexpr uint32_t const s_Bucket_Number = 32U;
static constexpr uint32_t const s_NonLargeObject_SizeMax = 7168U;

static inline bool PTS_Size_IsPowerOfTwo(uint32_t Value);
static inline bool PTS_Size_IsAligned(size_t Value, size_t Alignment);
static inline size_t PTS_Size_AlignDownFrom(size_t Value, size_t alignment);
static inline uint32_t PTS_Size_AlignUpFrom(uint32_t Value, uint32_t Alignment);
static inline uint32_t PTS_Size_BitScanReverse(uint32_t Value);
static inline void PTS_Size_ResolveRequestSize(uint32_t *pBucketIndex, uint32_t *pObjectSize, uint32_t RequestSize);

#if defined(PTARM) || defined(PTARM64) || defined(PTX86) || defined(PTX64)
static constexpr uint32_t const s_CacheLine_Size = 64U;
#else
#error 未知的架构
#endif

static inline void PTS_AtomicQueue_Unpack(uint64_t *pVersionNumber, PTS_BucketBlockHeader **pBlockAddress, uint64_t Value);

static inline uint64_t PTS_AtomicQueue_Pack(uint64_t VersionNumber, PTS_BucketBlockHeader *BlockAddress);

template<typename TypeBlock, typename TypeBlockNextAccessor>
static inline void PTS_AtomicQueue_Push(uint64_t *pQueueTop, TypeBlock *blockPtr);

template<typename TypeBlock, typename TypeBlockNextAccessor>
static inline TypeBlock *PTS_AtomicQueue_Pop(uint64_t *pQueueTop);

static constexpr uint32_t const s_BucketBlockManager_QueueEmpty_CountThreshold = 64U;
static class PTS_BucketBlockManager
{
	uint64_t m_QueueNonEmptyTops[s_Bucket_Number];

	uint64_t m_QueueEmptyTop;
	uint32_t m_QueueEmpty_Count;

private:
	class BucketBlockHeader_Next_InBlockManager
	{
		static inline PTS_BucketBlockHeader **Access(PTS_BucketBlockHeader *pBlockToAccess);
		
		template<typename TypeBlock, typename TypeBlockNextAccessor>
		friend void PTS_AtomicQueue_Push(uint64_t *pQueueTop, TypeBlock *blockPtr);
		
		template<typename TypeBlock, typename TypeBlockNextAccessor>
		friend TypeBlock *PTS_AtomicQueue_Pop(uint64_t *pQueueTop);

		friend struct PTS_BucketBlockHeader;
	};

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
	PTS_BucketBlockHeader *m_BlockActiveVector[s_Bucket_Number]; //Bucket=BlockDoublyLinkedList
	PTS_BucketBlockHeader *m_BlockLastNextVector[s_Bucket_Number]; //Record Last Try In Traverse Next

	inline void _LoseEmpty(PTS_BucketBlockHeader *pBlockToLose);

	inline void _LoseNonEmpty(PTS_BucketBlockHeader *pBlockToLose);

	inline void _RemoveInDestruct(PTS_BucketBlockHeader *pBlockToRemove, uint32_t BucketIndex);

	friend struct PTS_BucketBlockHeader;
	friend struct PTS_BucketObjectHeader;
public:
	inline void Construct();

	inline PTS_BucketObjectHeader *Alloc(uint32_t Size);

	inline void InsertToPrevious(PTS_BucketBlockHeader *pBlockToAdd, uint32_t BinIndex);

	inline void InsertToNext(PTS_BucketBlockHeader *pBlockToAdd, uint32_t BinIndex);

	inline void Remove(PTS_BucketBlockHeader *pBlockToRemove, uint32_t BinIndex);

	inline void OwnEmpty(PTS_BucketBlockHeader *pBlockToOwn, uint32_t BinIndex, uint32_t ObjectSize);

	inline void OwnNonEmpty(PTS_BucketBlockHeader *pBlockToOwn);

	inline void Destruct();
};
//static_assert(std::is_pod<PTS_ThreadLocalBucketAllocator>::value, "PTS_ThreadLocalBucketAllocator Is Not A POD");

#ifndef NDEBUG
static constexpr uint32_t const s_BucketBlockHeader_Identity_Block = 0X5765C7B8U;
static PTSThreadID const s_BucketBlockHeader_ThreadID_Invalid = PTSThreadID(5201314U); 
#endif

struct PTS_BucketBlockHeader
{
private:
	PTS_BucketBlockHeader *m_pNext_InBlockManager;
	friend PTS_BucketBlockHeader **PTS_BucketBlockManager::BucketBlockHeader_Next_InBlockManager::Access(PTS_BucketBlockHeader *pBlockToAccess);
	friend void PTS_BucketBlockManager::PushEmpty(PTS_BucketBlockHeader *pBlockToPush);
	friend void PTS_BucketBlockManager::PushNonEmpty(PTS_BucketBlockHeader *pBlockToPush);
	
	PTS_BucketObjectHeader *m_ObjectFreeVector_Public; //ObjectVector=FreeList
	
	uint32_t m_ObjectFreeVector_Public_Number;
	
	//To Avoid False Sharing，将可能被Foreign线程访问的Public域和只能被Owner线程访问的Private域分隔在不同的缓存行中
	uint8_t __PaddingForPublicFields[s_CacheLine_Size - sizeof(void*) * 2U - sizeof(uint32_t)];

	//NULL Means The "m_ThreadID_Owner" Is Invalid
	//同时 在缓存TLS后 PTS_Internal_Free中不再需要调用PTSTSD_GetValue
	PTS_ThreadLocalBucketAllocator *m_pAllocator_Owner;

	PTS_BucketBlockHeader *m_pPrevious_InBucket; //Empty Enough Block //Bucket=BlockDoublyLinkedList
	PTS_BucketBlockHeader *m_pNext_InBucket; //Full Block //Bucket=BlockDoublyLinkedList
	friend void PTS_ThreadLocalBucketAllocator::InsertToPrevious(PTS_BucketBlockHeader *pBlockToAdd, uint32_t BinIndex);
	friend void PTS_ThreadLocalBucketAllocator::InsertToNext(PTS_BucketBlockHeader *pBlockToAdd, uint32_t BinIndex);
	friend void PTS_ThreadLocalBucketAllocator::Remove(PTS_BucketBlockHeader *pBlockToRemove, uint32_t BinIndex);
	friend PTS_BucketObjectHeader *PTS_ThreadLocalBucketAllocator::Alloc(uint32_t Size);
	friend void PTS_ThreadLocalBucketAllocator::Destruct();

	PTS_BucketObjectHeader *m_ObjectFreeVector_Private; //ObjectVector=FreeList
	PTS_BucketObjectHeader *m_BumpPointer;
	
	//Valid Only When "m_pAllocator_Owner" Is Not NULL
	PTSThreadID m_ThreadID_Owner;

	uint32_t m_ObjectSize; //Free/Realloc时用到
	uint32_t m_BucketIndex_InAllocator;
	uint32_t m_ObjectAllocated_Number;
	
#ifndef NDEBUG
	//不一定可靠，但很大概率可以用于区分Block和LargeObject
	uint32_t m_Identity_Block;
#endif

	bool m_bInPrevious;

	//To Fill In Cache Line To 分隔Metadata和用户数据
#ifndef NDEBUG
	uint8_t __PaddingForPrivateFields[s_CacheLine_Size*2U - sizeof(uint32_t) * 4U - sizeof(PTSThreadID) - sizeof(void*) * 4U - sizeof(void*) - sizeof(bool) * 1U];
#else
	uint8_t __PaddingForPrivateFields[s_CacheLine_Size - sizeof(uint32_t) * 3U - sizeof(PTSThreadID) - sizeof(void*) * 4U - sizeof(void*) - sizeof(bool) * 1U];
#endif

	friend void PTS_ThreadLocalBucketAllocator::OwnEmpty(PTS_BucketBlockHeader *pBlockToOwn, uint32_t BinIndex, uint32_t ObjectSize);
	friend void PTS_ThreadLocalBucketAllocator::OwnNonEmpty(PTS_BucketBlockHeader *pBlockToOwn);
	friend void PTS_ThreadLocalBucketAllocator::_LoseEmpty(PTS_BucketBlockHeader *pBlockToLose);
	friend void PTS_ThreadLocalBucketAllocator::_LoseNonEmpty(PTS_BucketBlockHeader *pBlockToLose);
	friend void PTS_ThreadLocalBucketAllocator::_RemoveInDestruct(PTS_BucketBlockHeader *pBlockToRemove, uint32_t BinIndex);

private:
	inline void _BumpPointerRestore();

	inline PTS_BucketObjectHeader *_BumpPointer();

	inline void _ObjectFreeVector_Private_Push(PTS_BucketObjectHeader *pObjectToPush);

	inline PTS_BucketObjectHeader *_ObjectFreeVector_Private_Pop();
	
	//Foreign Thread
	//return previous head pointer
	inline void _ObjectFreeVector_Public_Push(PTS_BucketObjectHeader *pObjectToPush);

	inline bool _IsEmpty() const;

	inline bool _IsInPrevious() const;

	friend struct PTS_BucketObjectHeader;
public:
	inline void Construct();

	inline bool IsReadyInPrevious() const;

	inline uint32_t ObjectSize() const;

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
#ifndef NDEBUG
static_assert(sizeof(PTS_BucketBlockHeader) == s_CacheLine_Size * 3U, "Padding For Private Fields Of PTS_BucketBlockHeader Is Not Correct");
#else
static_assert(sizeof(PTS_BucketBlockHeader) == s_CacheLine_Size * 2U, "Padding For Private Fields Of PTS_BucketBlockHeader Is Not Correct");
#endif

struct PTS_BucketObjectHeader
{
private:
	PTS_BucketObjectHeader *m_pNext_InFreeVector;
	friend PTS_BucketObjectHeader *PTS_BucketBlockHeader::_BumpPointer();
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
#if defined(PTWIN32)
//在Windows（https://docs.microsoft.com/en-us/windows-hardware/drivers/gettingstarted/virtual-address-spaces）中
//x86架构下，用户空间只有2GB(31位）或3GB（32位） /*目前全部按照32位处理*/，块地址只需要18（32-14=18）位，可以使用64位CAS指令，其余46位可用作版本号，环绕需要2年（基于论文中46位需要2年计算）
//x64架构下，用户空间只有8TB（43位），块地址只需要29（43-14=29）位，可以使用64位CAS指令，其余35位用作版本号，环绕需要2天（基于论文中46位需要2年计算）
//-----------------------------
//在ARM架构下，虽然没有文档说明，但是用户空间显然不会超过4GB（32位），块地址只需要18（32-14=18）位，可以使用64位CAS指令，其余46位可用作版本号，环绕需要2年（基于论文中46位需要2年计算）
//-----------------------------
//目前不存在ARM64架构的Windows发行版
#if defined(PTARM) || defined(PTX86)
static inline void PTS_AtomicQueue_Unpack(uint64_t *pVersionNumber, PTS_BucketBlockHeader **pBlockAddress, uint64_t Value)
{
	assert(s_Block_Size == (1U << 14U));

	//46位    //18位
	//Version //Address

	(*pVersionNumber) = ((Value) >> 18U);
	(*pBlockAddress) = reinterpret_cast<PTS_BucketBlockHeader *>((Value & 0X3FFFFU) << 14U);
}
static inline uint64_t PTS_AtomicQueue_Pack(uint64_t VersionNumber, PTS_BucketBlockHeader *BlockAddress)
{
	assert(s_Block_Size == (1U << 14U));
	assert((reinterpret_cast<uintptr_t>(BlockAddress) & 0X00003FFFU) == 0U);

	//46位    //18位（后14）
	//Version //Address

	return (VersionNumber << 18U) | (reinterpret_cast<uintptr_t>(BlockAddress) >> 14U);
}
#elif defined(PTARM64)
#error 目前不存在ARM64架构的Windows发行版
#elif defined(PTX64)
static inline void PTS_AtomicQueue_Unpack(uint64_t *pVersionNumber, PTS_BucketBlockHeader **pBlockAddress, uint64_t Value)
{
	//35位    //29位
	//Version //Address

	(*pVersionNumber) = ((Value) >> 29U);
	(*pBlockAddress) = reinterpret_cast<PTS_BucketBlockHeader *>((Value & 0X1FFFFFFFU) << 14U);
}

static inline uint64_t PTS_AtomicQueue_Pack(uint64_t VersionNumber, PTS_BucketBlockHeader *BlockAddress)
{
	assert(s_Block_Size == (1U << 14U));
	assert((reinterpret_cast<uintptr_t>(BlockAddress) & 0XFFFFF80000003FFFU) == 0U);

	//35位    //29位（前21后14）
	//Version //Address

	return (VersionNumber << 29U) | (reinterpret_cast<uintptr_t>(BlockAddress) >> 14U);
}
#else
#error 未知的架构
#endif
#elif defined(PTPOSIX)
//在Linux中
//在ARM(https://www.kernel.org/doc/Documentation/arm/memory.txt)或x86架构下，用户空间一定不会超过4GB（32位）/*具体的值取决于宏定义TASK_SIZE*/，块地址只需要18（32-14=18）位，可以使用64位CAS指令，其余46位可用作版本号，环绕需要2年（基于论文中46位需要2年计算）
//在ARM64（https://www.kernel.org/doc/Documentation/arm64/memory.txt）架构下，用户空间只有512GB（39位）/256TB（48位）/4TB（42位）/*目前全部按照48位处理*/，块地址只需要34（48-14=34）位，可以使用64位CAS指令，其余30位用作版本号，环绕需要6小时（基于论文中46位需要2年计算）
//在x64（https://www.kernel.org/doc/Documentation/x86/x86_64/mm.txt）架构下，用户空间只有128TB（47位），块地址只需要33（47-14=33）位，可以使用64位CAS指令，其余31位用作版本号，环绕需要12小时（基于论文中46位需要2年计算）
#if defined(PTARM) || defined(PTX86)
static inline void PTS_AtomicQueue_Unpack(uint64_t *pVersionNumber, PTS_BucketBlockHeader **pBlockAddress, uint64_t Value)
{
	assert(s_Block_Size == (1U << 14U));

	//46位    //18位
	//Version //Address

	(*pVersionNumber) = ((Value) >> 18U);
	(*pBlockAddress) = reinterpret_cast<PTS_BucketBlockHeader *>((Value & 0X3FFFFU) << 14U);
}
static inline uint64_t PTS_AtomicQueue_Pack(uint64_t VersionNumber, PTS_BucketBlockHeader *BlockAddress)
{
	assert(s_Block_Size == (1U << 14U));
	assert((reinterpret_cast<uintptr_t>(BlockAddress) & 0X00003FFFU) == 0U);

	//46位    //18位（后14）
	//Version //Address

	return (VersionNumber << 18U) | (reinterpret_cast<uintptr_t>(BlockAddress) >> 14U);
}
#elif defined(PTARM64)
static inline void PTS_AtomicQueue_Unpack(uint64_t *pVersionNumber, PTS_BucketBlockHeader **pBlockAddress, uint64_t Value)
{
	//30位    //34位
	//Version //Address

	(*pVersionNumber) = ((Value) >> 34U);
	(*pBlockAddress) = reinterpret_cast<PTS_BucketBlockHeader *>((Value & 0X3FFFFFFFFU) << 14U);
}

static inline uint64_t PTS_AtomicQueue_Pack(uint64_t VersionNumber, PTS_BucketBlockHeader *BlockAddress)
{
	assert(s_Block_Size == (1U << 14U));
	assert((reinterpret_cast<uintptr_t>(BlockAddress) & 0XFFFF000000003FFFU) == 0U);

	//30位    //34位（前16后14）
	//Version //Address

	return (VersionNumber << 34U) | (reinterpret_cast<uintptr_t>(BlockAddress) >> 14U);
}
#elif defined(PTX64)
static inline void PTS_AtomicQueue_Unpack(uint64_t *pVersionNumber, PTS_BucketBlockHeader **pBlockAddress, uint64_t Value)
{
	//31位    //33位
	//Version //Address

	(*pVersionNumber) = ((Value) >> 33U);
	(*pBlockAddress) = reinterpret_cast<PTS_BucketBlockHeader *>((Value & 0X1FFFFFFFFU) << 14U);
}

static inline uint64_t PTS_AtomicQueue_Pack(uint64_t VersionNumber, PTS_BucketBlockHeader *BlockAddress)
{
	assert(s_Block_Size == (1U << 14U));
	assert((reinterpret_cast<uintptr_t>(BlockAddress) & 0XFFFF800000003FFFU) == 0U);

	//31位    //33位（前17后14）
	//Version //Address

	return (VersionNumber << 33U) | (reinterpret_cast<uintptr_t>(BlockAddress) >> 14U);
}
#else
#error 未知的架构 
#endif
#else
#error 未知的平台
#endif

//Hudson 2006 / 3.McRT-MALLOC / 3.2 Non-blocking Operations / Figure 1 Non-blocking queues / lifoQueuePush

template<typename TypeBlock,typename TypeBlockNextAccessor>
static inline void PTS_AtomicQueue_Push(uint64_t *pQueueTop, TypeBlock *blockPtr)
{
	uint64_t oldValue;
	uint64_t newValue;

	do {
		oldValue = (*pQueueTop);

		uint64_t oldCount;
		TypeBlock *oldBlockPtr;
		::PTS_AtomicQueue_Unpack(&oldCount, &oldBlockPtr, oldValue);

		uint64_t newCount = oldCount + 1;
		(*TypeBlockNextAccessor::Access(blockPtr)) = oldBlockPtr;

		newValue = ::PTS_AtomicQueue_Pack(newCount, blockPtr);
	} while (::PTSAtomic_CompareAndSet(pQueueTop, oldValue, newValue) != oldValue);
}

//Hudson 2006 / 3.McRT-MALLOC / 3.2 Non-blocking Operations / Figure 1 Non-blocking queues / lifoQueuePop

template<typename TypeBlock, typename TypeBlockNextAccessor>
static inline TypeBlock *PTS_AtomicQueue_Pop(uint64_t *pQueueTop)
{
	uint64_t oldQueue;
	uint64_t newQueue;
	TypeBlock *blockPtr;

	do {
		oldQueue = (*pQueueTop);

		uint64_t oldCount;
		::PTS_AtomicQueue_Unpack(&oldCount, &blockPtr, oldQueue);

		if (blockPtr == NULL) //The Queue is empty
		{
			break;
		}

		uint64_t newCount = oldCount + 1;
		TypeBlock *newBlockPtr = (*TypeBlockNextAccessor::Access(blockPtr));

		newQueue = ::PTS_AtomicQueue_Pack(newCount, newBlockPtr);
	} while (::PTSAtomic_CompareAndSet(pQueueTop, oldQueue, newQueue) != oldQueue);

	return blockPtr;
}

//----------------------------------------------------------------------------------------------------------------
inline PTS_BucketBlockHeader **PTS_BucketBlockManager::BucketBlockHeader_Next_InBlockManager::Access(PTS_BucketBlockHeader *pBlockToAccess)
{
	return &pBlockToAccess->m_pNext_InBlockManager;
}

inline void PTS_BucketBlockManager::Construct()
{
	for (uint32_t i = 0U; i < s_Bucket_Number; ++i)
	{
		m_QueueNonEmptyTops[i] = 0U;
	}

	m_QueueEmptyTop = 0U;
	m_QueueEmpty_Count = 0U;
}

inline void PTS_BucketBlockManager::PushEmpty(PTS_BucketBlockHeader *pBlockToPush)
{
	//Private Field
	assert(pBlockToPush->m_pAllocator_Owner == NULL);
	assert(pBlockToPush->m_ObjectFreeVector_Private == NULL);
	assert(pBlockToPush->m_BumpPointer == NULL);
	assert(pBlockToPush->m_ThreadID_Owner == s_BucketBlockHeader_ThreadID_Invalid);
	assert(pBlockToPush->m_ObjectSize == ~uint32_t(0U));
	assert(pBlockToPush->m_BucketIndex_InAllocator == ~uint32_t(0U));
	assert(pBlockToPush->m_pNext_InBucket == NULL);
	assert(pBlockToPush->m_pPrevious_InBucket == NULL);
	assert(pBlockToPush->_IsEmpty());

	//Public Field
	assert(pBlockToPush->m_ObjectFreeVector_Public == NULL);
	assert(pBlockToPush->m_ObjectFreeVector_Public_Number == 0U);

	if (::PTSAtomic_Get(&m_QueueEmpty_Count) <= s_BucketBlockManager_QueueEmpty_CountThreshold)
	{
		::PTSAtomic_GetAndAdd(&m_QueueEmpty_Count, 1U); //队列中实际的个数和m_QueueEmpty_Count并不严格相等
		::PTS_AtomicQueue_Push<PTS_BucketBlockHeader, BucketBlockHeader_Next_InBlockManager>(&m_QueueEmptyTop, pBlockToPush);
	}
	else
	{
		::PTS_MemoryMap_Free(pBlockToPush);
	}
}

inline void PTS_BucketBlockManager::PushNonEmpty(PTS_BucketBlockHeader *pBlockToPush)
{
	assert(pBlockToPush->m_pAllocator_Owner == NULL);
	assert(pBlockToPush->m_ThreadID_Owner == s_BucketBlockHeader_ThreadID_Invalid);
	assert(pBlockToPush->m_pNext_InBucket == NULL);
	assert(pBlockToPush->m_pPrevious_InBucket == NULL);
	assert(!pBlockToPush->_IsEmpty());

	::PTS_AtomicQueue_Push<PTS_BucketBlockHeader, BucketBlockHeader_Next_InBlockManager>(&m_QueueNonEmptyTops[pBlockToPush->m_BucketIndex_InAllocator], pBlockToPush);
}

inline PTS_BucketBlockHeader *PTS_BucketBlockManager::PopNonEmpty(uint32_t BinIndex)
{
	PTS_BucketBlockHeader *pBlockAdded = ::PTS_AtomicQueue_Pop<PTS_BucketBlockHeader, BucketBlockHeader_Next_InBlockManager>(&m_QueueNonEmptyTops[BinIndex]);

	return pBlockAdded;
}

inline PTS_BucketBlockHeader *PTS_BucketBlockManager::PopEmpty(uint32_t BinIndex)
{
	PTS_BucketBlockHeader *pBlockAdded;
	
	pBlockAdded = ::PTS_AtomicQueue_Pop<PTS_BucketBlockHeader, BucketBlockHeader_Next_InBlockManager>(&m_QueueEmptyTop);
	if (pBlockAdded != NULL)
	{
		::PTSAtomic_GetAndAdd(&m_QueueEmpty_Count, uint32_t(-1)); //队列中实际的个数和m_QueueEmpty_Count并不严格相等
		return pBlockAdded;
	}

	//启发式，而非在初始化时一次性分配
	pBlockAdded = static_cast<PTS_BucketBlockHeader *>(::PTS_MemoryMap_Alloc(s_Block_Size));
	//内存不足？？？
	assert(pBlockAdded != NULL);
	if (pBlockAdded != NULL)
	{		
		//严格意义上16KB应当是对齐而非大小
		assert(PTS_Size_IsAligned(reinterpret_cast<uintptr_t>(pBlockAdded), s_Block_Size));
		pBlockAdded->Construct();
		return pBlockAdded;
	}

	return NULL;
}






//********************************************************************************************************************************************************************************************************************************************************************
//PTS_ThreadLocalBucketAllocator
//********************************************************************************************************************************************************************************************************************************************************************

inline void PTS_ThreadLocalBucketAllocator::Construct()
{
	for (uint32_t BucketIndex = 0U; BucketIndex < s_Bucket_Number; ++BucketIndex)
	{
		m_BlockActiveVector[BucketIndex] = NULL;
		m_BlockLastNextVector[BucketIndex] = NULL;
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

static inline void PTS_Size_ResolveRequestSize(uint32_t *pBucketIndex, uint32_t *pObjectSize, uint32_t RequestSize)
{
	//Part1↓-----------------------------
	//------------------------------------
	//8                 1000
	//16              10|000
	//32             100|000
	//------------------------------------
	//48             11|0000   
	//64            100|0000

	//Part2↓-----------------------------
	//80           0101|0000
	//96           0110|0000‬
	//112          0111|0000‬
	//128          1000|0000

	//160         0101|00000‬
	//192    ‭     0110|00000‬
	//224         0111|00000
	//256         1000|00000  

	//320        0101|000000
	//384       ‭ 0110|000000‬
	//448       ‭ 0111|000000‬
	//512        1000|000000

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

	//End↑-----------------------------
	
	//8192   1000|0000000000		

	static constexpr uint32_t const s_Part1_BucketNumber = 5U;
	static constexpr uint32_t const s_Part1_ObjectSizeTotal = 64U;
	static constexpr uint32_t const s_Part1_ObjectSizeTotal_Order = 6U;
	assert(s_Part1_ObjectSizeTotal == (1U << s_Part1_ObjectSizeTotal_Order));

	static constexpr uint32_t const s_Part2_BucketNumber = 32U; //5 + 27 = 32
	static constexpr uint32_t const s_Part2_ObjectSizeTotal = 8192U;
	static constexpr uint32_t const s_Part2_ObjectSizeTotal_Order = 13U;
	static constexpr uint32_t const s_Part2_GroupSize = 4U;
	static constexpr uint32_t const s_Part2_GroupSize_Order = 2U;
	assert(s_Part2_ObjectSizeTotal == (1U << s_Part2_ObjectSizeTotal_Order));
	assert(s_Part2_GroupSize == (1U << s_Part2_GroupSize_Order));

	assert(s_Part2_BucketNumber == s_Bucket_Number);

#ifndef NDEBUG
	if (RequestSize <= 0U)
	{
		assert(RequestSize > 0U);
		(*pBucketIndex) = ~uint32_t(0U);
		(*pObjectSize) = ~uint32_t(0U);
	}
	else
#endif
	if (RequestSize <= s_Part1_ObjectSizeTotal)
	{
		static constexpr uint32_t const s_BinIndexVector[] = { 0U,1U,2U,2U,3U,3U,4U,4U };
		//8  -> 7  -> 0 -> 0:8
		//16 -> 15 -> 1 -> 1:16
		//24 -> 23 -> 2 -> 2:32
		//32 -> 31 -> 3 -> 2:32
		//40 -> 39 -> 4 -> 3:48
		//48 -> 47 -> 5 -> 3:48
		//56 -> 55 -> 6 -> 4:64
		//64 -> 63 -> 7 -> 4:64
		static constexpr uint32_t const s_ObjectSizeVector[] = { 8U,16U,32U,48U,64U };
		assert((sizeof(s_ObjectSizeVector) / sizeof(s_ObjectSizeVector[0])) == s_Part1_BucketNumber);
		assert(s_ObjectSizeVector[s_Part1_BucketNumber - 1U] == s_Part1_ObjectSizeTotal);

		(*pBucketIndex) = s_BinIndexVector[(RequestSize - 1U) >> 3U]; //== (RequestSize - 1U)/8U
		(*pObjectSize) = s_ObjectSizeVector[(*pBucketIndex)];
	}
	else if (RequestSize <= s_NonLargeObject_SizeMax)
	{
		uint32_t Order = ::PTS_Size_BitScanReverse(RequestSize - 1U);
		assert(Order >= s_Part1_ObjectSizeTotal_Order && Order < s_Part2_ObjectSizeTotal_Order);

		(*pBucketIndex) = s_Part1_BucketNumber
			+ s_Part2_GroupSize * (Order - s_Part1_ObjectSizeTotal_Order)
			+ (((RequestSize - 1U) >> (Order - s_Part2_GroupSize_Order)/*最高三位*/) - s_Part2_GroupSize); // 100 -> 4-4=0 || 101 -> 5-4=1 || 110 -> 6-4=2 || 111 -> 7-4=3

		uint32_t GroupAlignment_Min = 1U << (Order - s_Part2_GroupSize_Order)/*最高三位以外的其余位*/;
		(*pObjectSize) = ::PTS_Size_AlignUpFrom(RequestSize, GroupAlignment_Min);
	}
#ifndef NDEBUG
	else
	{
		assert(RequestSize <= s_NonLargeObject_SizeMax);
		(*pBucketIndex) = ~uint32_t(0U);
		(*pObjectSize) = ~uint32_t(0U);
	}
#endif
}

inline PTS_BucketObjectHeader *PTS_ThreadLocalBucketAllocator::Alloc(uint32_t Size)
{
	uint32_t BucketIndex;
	uint32_t ObjectSize;
	::PTS_Size_ResolveRequestSize(&BucketIndex, &ObjectSize, Size);

	//Test Current Active
	PTS_BucketBlockHeader ** const ppBlockActive = &m_BlockActiveVector[BucketIndex];
	PTS_BucketBlockHeader *pBlockEmptyEnoughInit = NULL;
	PTS_BucketBlockHeader *pBlockFullInit = NULL;
	if ((*ppBlockActive) != NULL)
	{
		pBlockEmptyEnoughInit = (*ppBlockActive)->m_pPrevious_InBucket;
		pBlockFullInit = (*ppBlockActive)->m_pNext_InBucket;

		assert(m_BlockActiveVector[(*ppBlockActive)->m_BucketIndex_InAllocator] == (*ppBlockActive));
		PTS_BucketObjectHeader *pObjectToAlloc = (*ppBlockActive)->Alloc();
		if (pObjectToAlloc != NULL)
		{
			return pObjectToAlloc;
		}
	}

	//Traverse Previous: EmptyEnough Block
	for (
		PTS_BucketBlockHeader *pBlockEmptyEnough = pBlockEmptyEnoughInit;
		pBlockEmptyEnough != NULL;
		pBlockEmptyEnough = pBlockEmptyEnough->m_pPrevious_InBucket
		)
	{
		//Reset Active
		(*ppBlockActive) = pBlockEmptyEnough;

		assert(m_BlockActiveVector[pBlockEmptyEnough->m_BucketIndex_InAllocator] == pBlockEmptyEnough);
		PTS_BucketObjectHeader *pObjectToAlloc = pBlockEmptyEnough->Alloc();
		if (pObjectToAlloc != NULL)
		{
			return pObjectToAlloc;
		}

		//This Block Becomes Full
		//We Try Next
	}

	//Traverse Next: Full Block

	//There Exists Race Condition In TBB's Method.
	//So We Choose [Hudson 2006]'s Method

	//[Hudson 2006] / 3.McRT-MALLOC / 3.3 Framework for Managing Blocks
	//One final wrinkle is that if we inspect 10 blocks without finding a free object then we add a new block for the bin.
	//This places an upper bound on the time malloc will take before returning an object.

	//We Try 8 Times Not 10 Times

	PTS_BucketBlockHeader ** const ppBlockLastNext = &m_BlockLastNextVector[BucketIndex];

	for (
		struct { uint32_t iTryTime; PTS_BucketBlockHeader *pBlockFull; }__Z = { 0U,((*ppBlockLastNext) != NULL) ? (((*ppBlockLastNext)->m_pNext_InBucket != NULL) ? (*ppBlockLastNext)->m_pNext_InBucket : pBlockFullInit) : pBlockFullInit };
		__Z.iTryTime < 8U && ((*ppBlockLastNext) = __Z.pBlockFull/*ResetLastNext*/) != NULL;
		++__Z.iTryTime, __Z.pBlockFull = __Z.pBlockFull->m_pNext_InBucket
		)
	{
		if (__Z.pBlockFull->IsReadyInPrevious())
		{
			//EmptyEnough
			
			//Move From "Next" To "Previous"
			Remove(__Z.pBlockFull, BucketIndex);
			InsertToPrevious(__Z.pBlockFull, BucketIndex);

			//Reset Active
			(*ppBlockActive) = __Z.pBlockFull;

			assert(m_BlockActiveVector[__Z.pBlockFull->m_BucketIndex_InAllocator] == (*ppBlockActive));
			PTS_BucketObjectHeader *pObjectToAlloc = __Z.pBlockFull->Alloc();
			
			//EmptyEnough Must Not NULL
			assert(pObjectToAlloc != NULL); 
			return pObjectToAlloc;
		}
	}

	//NoOwner-NonEmpty Block
	for (
		PTS_BucketBlockHeader *pBlockAdded = s_BlockStore_Singleton.PopNonEmpty(BucketIndex);
		pBlockAdded != NULL;
		pBlockAdded = s_BlockStore_Singleton.PopNonEmpty(BucketIndex)
		)
	{
		OwnNonEmpty(pBlockAdded);

		if (pBlockAdded->IsReadyInPrevious())
		{
			InsertToPrevious(pBlockAdded, BucketIndex);
			
			//Active前移
			(*ppBlockActive) = pBlockAdded; 
		}
		else
		{
			InsertToNext(pBlockAdded, BucketIndex);

			//Active不变
			//其它线程终止时添加的块
		}

		assert(m_BlockActiveVector[pBlockAdded->m_BucketIndex_InAllocator] == pBlockAdded);
		PTS_BucketObjectHeader *pObjectToAlloc = pBlockAdded->Alloc();
		if (pObjectToAlloc != NULL)
		{
			return pObjectToAlloc;
		}
	}

	//(NoOwner-)Empty Block
	{
		PTS_BucketBlockHeader *pBlockAdded = s_BlockStore_Singleton.PopEmpty(BucketIndex);
		//内存不足???
		assert(pBlockAdded != NULL);

		if (pBlockAdded != NULL)
		{
			OwnEmpty(pBlockAdded, BucketIndex, ObjectSize);
			InsertToPrevious(pBlockAdded, BucketIndex);

			(*ppBlockActive) = pBlockAdded; //Active前移

			assert(m_BlockActiveVector[pBlockAdded->m_BucketIndex_InAllocator] == pBlockAdded);
			PTS_BucketObjectHeader *pObjectToAlloc = pBlockAdded->Alloc();
			assert(pObjectToAlloc != NULL); //The Block Is Empty???
			if (pObjectToAlloc != NULL)
			{
				return pObjectToAlloc;
			}
		}
	}

	return NULL;
}

inline void PTS_ThreadLocalBucketAllocator::InsertToPrevious(PTS_BucketBlockHeader *pBlockToAdd, uint32_t BinIndex)
{
	//The Block Is Owned By Allocator And Current Thread 
	assert((pBlockToAdd->m_pAllocator_Owner = this) && (::PTSThreadID_Equal(pBlockToAdd->m_ThreadID_Owner, ::PTSThreadID_Self()) != false));

	//Only Empty Enough Block Can Insert To Previous
	assert(pBlockToAdd->IsReadyInPrevious());

	//Link Consistent
	assert(!pBlockToAdd->m_bInPrevious);
	assert(pBlockToAdd->m_pNext_InBucket == NULL);
	assert(pBlockToAdd->m_pPrevious_InBucket == NULL);

	//Size Consistent
	assert(pBlockToAdd->m_BucketIndex_InAllocator == BinIndex);
	assert(pBlockToAdd->m_BucketIndex_InAllocator < s_Bucket_Number);

	PTS_BucketBlockHeader *const pBlockActive = m_BlockActiveVector[BinIndex];

	//The Block Is Owned By Allocator And Current Thread 
	assert(pBlockActive == NULL || ((pBlockActive->m_pAllocator_Owner == this) && (::PTSThreadID_Equal(pBlockActive->m_ThreadID_Owner, ::PTSThreadID_Self()) != false)));

	//Size Consistent
	assert(pBlockActive == NULL || pBlockActive->m_BucketIndex_InAllocator == pBlockToAdd->m_BucketIndex_InAllocator);

	if (pBlockActive == NULL)
	{
		m_BlockActiveVector[BinIndex] = pBlockToAdd; //Active初始化
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

	//Link Consistent
	pBlockToAdd->m_bInPrevious = true;
}

inline void PTS_ThreadLocalBucketAllocator::InsertToNext(PTS_BucketBlockHeader *pBlockToAdd, uint32_t BinIndex)
{
	//The Block Is Owned By Allocator And Current Thread 
	assert((pBlockToAdd->m_pAllocator_Owner = this) && (::PTSThreadID_Equal(pBlockToAdd->m_ThreadID_Owner, ::PTSThreadID_Self()) != false));

	//Only Not Empty Enough Block Can Insert To Next
	assert(!pBlockToAdd->IsReadyInPrevious());

	//Link Consistent
	assert(!pBlockToAdd->m_bInPrevious);
	assert(pBlockToAdd->m_pNext_InBucket == NULL);
	assert(pBlockToAdd->m_pPrevious_InBucket == NULL);

	//Size Consistent	
	assert(pBlockToAdd->m_BucketIndex_InAllocator == BinIndex);
	assert(pBlockToAdd->m_BucketIndex_InAllocator < s_Bucket_Number);

	PTS_BucketBlockHeader *const pBlockActive = m_BlockActiveVector[BinIndex];

	//The Block Is Owned By Allocator And Current Thread 
	assert(pBlockActive == NULL || ((pBlockActive->m_pAllocator_Owner == this) && (::PTSThreadID_Equal(pBlockActive->m_ThreadID_Owner, ::PTSThreadID_Self()) != false)));

	//Size Consistent
	assert(pBlockActive == NULL || pBlockActive->m_BucketIndex_InAllocator == pBlockToAdd->m_BucketIndex_InAllocator);

	if (pBlockActive == NULL)
	{
		m_BlockActiveVector[BinIndex] = pBlockToAdd; //Active初始化
	}

	//pBlockToAdd位于pBlockActive的Next
	pBlockToAdd->m_pPrevious_InBucket = pBlockActive;
	if (pBlockActive != NULL)
	{
		pBlockToAdd->m_pNext_InBucket = pBlockActive->m_pNext_InBucket;
		pBlockActive->m_pNext_InBucket = pBlockToAdd;

		if (pBlockToAdd->m_pNext_InBucket != NULL)
		{
			pBlockToAdd->m_pNext_InBucket->m_pPrevious_InBucket = pBlockToAdd;
		}
	}
}

inline void PTS_ThreadLocalBucketAllocator::Remove(PTS_BucketBlockHeader *pBlockToRemove, uint32_t BinIndex)
{
	//The Block Is Owned By Allocator And Current Thread 
	assert((pBlockToRemove->m_pAllocator_Owner == this) && (::PTSThreadID_Equal(pBlockToRemove->m_ThreadID_Owner, ::PTSThreadID_Self()) != false));

	//Size Consistent
	assert(pBlockToRemove->m_BucketIndex_InAllocator == BinIndex);
	assert(pBlockToRemove->m_BucketIndex_InAllocator < s_Bucket_Number);

	//The Block Is Owned By Allocator And Current Thread 
	assert(m_BlockActiveVector[BinIndex] == NULL || ((m_BlockActiveVector[BinIndex]->m_pAllocator_Owner == this) && (::PTSThreadID_Equal(m_BlockActiveVector[BinIndex]->m_ThreadID_Owner, ::PTSThreadID_Self()) != false)));
	assert(m_BlockLastNextVector[BinIndex] == NULL || ((m_BlockLastNextVector[BinIndex]->m_pAllocator_Owner == this) && (::PTSThreadID_Equal(m_BlockLastNextVector[BinIndex]->m_ThreadID_Owner, ::PTSThreadID_Self()) != false)));

	//Size Consistent
	assert(m_BlockActiveVector[BinIndex] == NULL || m_BlockActiveVector[BinIndex]->m_BucketIndex_InAllocator == BinIndex);
	assert(m_BlockLastNextVector[BinIndex] == NULL || m_BlockLastNextVector[BinIndex]->m_BucketIndex_InAllocator == BinIndex);

	//Reset Active
	if (m_BlockActiveVector[BinIndex] == pBlockToRemove)
	{
		//Previous(Means EmptyEnough) Prior To Next(Means Full)
		pBlockToRemove->m_pAllocator_Owner->m_BlockActiveVector[BinIndex] = (pBlockToRemove->m_pPrevious_InBucket != NULL) ? pBlockToRemove->m_pPrevious_InBucket : pBlockToRemove->m_pNext_InBucket;
	}

	//Reset Last Next
	if (m_BlockLastNextVector[BinIndex] == pBlockToRemove)
	{
		//Only Next(Means Full) Acceptable
		pBlockToRemove->m_pAllocator_Owner->m_BlockLastNextVector[BinIndex] = (pBlockToRemove->m_pNext_InBucket != NULL) ? pBlockToRemove->m_pNext_InBucket : NULL;
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

	//Link Consistent
	pBlockToRemove->m_bInPrevious = false;
}

inline void PTS_ThreadLocalBucketAllocator::OwnEmpty(PTS_BucketBlockHeader *pBlockToOwn, uint32_t BinIndex, uint32_t ObjectSize)
{
	//Private Field
	assert(pBlockToOwn->m_pAllocator_Owner == NULL);
	assert(pBlockToOwn->m_ObjectFreeVector_Private == NULL);
	assert(pBlockToOwn->m_BumpPointer == NULL);
	assert(pBlockToOwn->m_ThreadID_Owner == s_BucketBlockHeader_ThreadID_Invalid);
	assert(pBlockToOwn->m_ObjectSize == ~uint32_t(0U));
	assert(pBlockToOwn->m_BucketIndex_InAllocator == ~uint32_t(0U));
	assert(pBlockToOwn->m_pNext_InBucket == NULL);
	assert(pBlockToOwn->m_pPrevious_InBucket == NULL);
	assert(pBlockToOwn->_IsEmpty());
	assert(!pBlockToOwn->m_bInPrevious);

	pBlockToOwn->m_pAllocator_Owner = this;
	pBlockToOwn->m_ThreadID_Owner = ::PTSThreadID_Self();
	pBlockToOwn->m_ObjectSize = ObjectSize;
	pBlockToOwn->m_BucketIndex_InAllocator = BinIndex;
	pBlockToOwn->_BumpPointerRestore();

	//Public Field
	assert(pBlockToOwn->m_ObjectFreeVector_Public == NULL);
	assert(pBlockToOwn->m_ObjectFreeVector_Public_Number == 0U);
}

inline void PTS_ThreadLocalBucketAllocator::OwnNonEmpty(PTS_BucketBlockHeader *pBlockToOwn)
{
	//Private Field
	assert(pBlockToOwn->m_pAllocator_Owner == NULL);
	assert(pBlockToOwn->m_ThreadID_Owner == s_BucketBlockHeader_ThreadID_Invalid);
	assert(pBlockToOwn->m_pNext_InBucket == NULL);
	assert(pBlockToOwn->m_pPrevious_InBucket == NULL);
	assert(!pBlockToOwn->_IsEmpty());
	assert(!pBlockToOwn->m_bInPrevious);

	pBlockToOwn->m_pAllocator_Owner = this;
	pBlockToOwn->m_ThreadID_Owner = ::PTSThreadID_Self();
}

inline void PTS_ThreadLocalBucketAllocator::_LoseEmpty(PTS_BucketBlockHeader *pBlockToLose)
{
	//Private Field
	
	//The Block Is Owned By Allocator And Current Thread 
	assert((pBlockToLose->m_pAllocator_Owner == this) && (::PTSThreadID_Equal(pBlockToLose->m_ThreadID_Owner, ::PTSThreadID_Self()) != false));
	
	assert(pBlockToLose->m_pNext_InBucket == NULL);
	assert(pBlockToLose->m_pPrevious_InBucket == NULL);
	assert(pBlockToLose->_IsEmpty());
	assert(!pBlockToLose->m_bInPrevious);

	pBlockToLose->m_pAllocator_Owner = NULL;

#ifndef NDEBUG
	pBlockToLose->m_ThreadID_Owner = s_BucketBlockHeader_ThreadID_Invalid;
	pBlockToLose->m_ObjectFreeVector_Private = NULL;
	pBlockToLose->m_BumpPointer = NULL;
	pBlockToLose->m_ObjectSize = ~uint32_t(0U);
	pBlockToLose->m_BucketIndex_InAllocator = ~uint32_t(0U);
#endif

	//Public Field
	assert(pBlockToLose->m_ObjectFreeVector_Public == NULL);
	assert(pBlockToLose->m_ObjectFreeVector_Public_Number == 0U);
}

inline void PTS_ThreadLocalBucketAllocator::_LoseNonEmpty(PTS_BucketBlockHeader *pBlockToLose)
{
	//Private Field

	//The Block Is Owned By Current Thread 
	assert((pBlockToLose->m_pAllocator_Owner == this) && (::PTSThreadID_Equal(pBlockToLose->m_ThreadID_Owner, ::PTSThreadID_Self()) != false));

	assert(pBlockToLose->m_pNext_InBucket == NULL);
	assert(pBlockToLose->m_pPrevious_InBucket == NULL);
	assert(!pBlockToLose->_IsEmpty());
	assert(!pBlockToLose->m_bInPrevious);

	pBlockToLose->m_pAllocator_Owner = NULL;
#ifndef NDEBUG
	pBlockToLose->m_ThreadID_Owner = s_BucketBlockHeader_ThreadID_Invalid;
#endif
}

inline void PTS_ThreadLocalBucketAllocator::_RemoveInDestruct(PTS_BucketBlockHeader *pBlockToRemove, uint32_t BucketIndex)
{
	//The Block Is Owned By Allocator And Current Thread 
	assert((pBlockToRemove->m_pAllocator_Owner == this) && (::PTSThreadID_Equal(pBlockToRemove->m_ThreadID_Owner, ::PTSThreadID_Self()) != false));

	//Size Consistent
	assert(pBlockToRemove->m_BucketIndex_InAllocator == BucketIndex);
	assert(pBlockToRemove->m_BucketIndex_InAllocator < s_Bucket_Number);

	//Verify Link
	assert(pBlockToRemove->m_pPrevious_InBucket == NULL || pBlockToRemove->m_pPrevious_InBucket->m_pNext_InBucket == NULL || pBlockToRemove->m_pPrevious_InBucket->m_pNext_InBucket == pBlockToRemove);

	assert(pBlockToRemove->m_pNext_InBucket == NULL || pBlockToRemove->m_pNext_InBucket->m_pPrevious_InBucket == NULL || pBlockToRemove->m_pNext_InBucket->m_pPrevious_InBucket == pBlockToRemove);

	pBlockToRemove->m_pNext_InBucket = NULL;
	pBlockToRemove->m_pPrevious_InBucket = NULL;

	//Link Consistent
	pBlockToRemove->m_bInPrevious = false;
}

inline void PTS_ThreadLocalBucketAllocator::Destruct()
{
	for (uint32_t BucketIndex = 0U; BucketIndex < s_Bucket_Number; ++BucketIndex)
	{
		PTS_BucketBlockHeader * const pBlockActive = m_BlockActiveVector[BucketIndex];
		PTS_BucketBlockHeader * const pBlockFull = (pBlockActive != NULL) ? (pBlockActive->m_pNext_InBucket) : NULL;

		//Previous: EmptyEnough Block
		PTS_BucketBlockHeader *pBlockToRemove = pBlockActive;

		while (pBlockToRemove != NULL)
		{
			PTS_BucketBlockHeader *pTemp = pBlockToRemove->m_pPrevious_InBucket;
			
			pBlockToRemove->ObjectFreeVector_Public_Repatriate();

			_RemoveInDestruct(pBlockToRemove, BucketIndex);

			if (!pBlockToRemove->_IsEmpty())
			{
				_LoseNonEmpty(pBlockToRemove);
				s_BlockStore_Singleton.PushNonEmpty(pBlockToRemove);
			}
			else
			{
				_LoseEmpty(pBlockToRemove);
				s_BlockStore_Singleton.PushEmpty(pBlockToRemove);
			}

			pBlockToRemove = pTemp;
		}

		//Next: Full Block
		pBlockToRemove = pBlockFull;

		while (pBlockToRemove != NULL)
		{
			PTS_BucketBlockHeader *pTemp = pBlockToRemove->m_pNext_InBucket;
			
			pBlockToRemove->ObjectFreeVector_Public_Repatriate();

			_RemoveInDestruct(pBlockToRemove, BucketIndex);

			if (!pBlockToRemove->_IsEmpty())
			{
				_LoseNonEmpty(pBlockToRemove);
				s_BlockStore_Singleton.PushNonEmpty(pBlockToRemove);
			}
			else
			{
				_LoseEmpty(pBlockToRemove);
				s_BlockStore_Singleton.PushEmpty(pBlockToRemove);
			}


			pBlockToRemove = pTemp;
		}
	}
}





//********************************************************************************************************************************************************************************************************************************************************************
//PTS_BucketBlockHeader
//********************************************************************************************************************************************************************************************************************************************************************

inline void PTS_BucketBlockHeader::_BumpPointerRestore()
{
	assert(m_ObjectFreeVector_Public == NULL);
	assert(_IsEmpty());

	m_ObjectFreeVector_Private = NULL;

	//We Align To The End Of The Block To Maximum The Alignment

	m_BumpPointer = reinterpret_cast<PTS_BucketObjectHeader *>(reinterpret_cast<uintptr_t>(this) + (s_Block_Size - m_ObjectSize * ((s_Block_Size - sizeof(PTS_BucketBlockHeader)) / m_ObjectSize)));
	assert(reinterpret_cast<uintptr_t>(m_BumpPointer) >= (reinterpret_cast<uintptr_t>(this) + sizeof(PTS_BucketBlockHeader)));
	assert((reinterpret_cast<uintptr_t>(m_BumpPointer) + m_ObjectSize) <= (reinterpret_cast<uintptr_t>(this) + s_Block_Size));
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
	pObjectToAlloc->m_pNext_InFreeVector = NULL;

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

	PTS_BucketObjectHeader *Old_ObjectFreeVector_Public;
	do
	{
		Old_ObjectFreeVector_Public = m_ObjectFreeVector_Public;
		pObjectToPush->m_pNext_InFreeVector = Old_ObjectFreeVector_Public;
	} while (::PTSAtomic_CompareAndSet(&m_ObjectFreeVector_Public, Old_ObjectFreeVector_Public, pObjectToPush) != Old_ObjectFreeVector_Public);
}

inline bool PTS_BucketBlockHeader::_IsEmpty() const
{
	return m_ObjectAllocated_Number == 0U;
}

inline bool PTS_BucketBlockHeader::_IsInPrevious() const
{
	return m_bInPrevious;
}

inline void PTS_BucketBlockHeader::Construct()
{
	//Public Field
	//m_pNext_InBlockManager = NULL; //We Will Set The Pointer To The Correct Value In Atomic Queue Push. We Don't Need To Set It To NULL Here
	m_ObjectFreeVector_Public = NULL;
	m_ObjectFreeVector_Public_Number = 0U;

	//Private Field
	m_pAllocator_Owner = NULL;
#ifndef NDEBUG
	m_ObjectFreeVector_Private = NULL;
	m_BumpPointer = NULL;
	m_ThreadID_Owner = s_BucketBlockHeader_ThreadID_Invalid;
	m_ObjectSize = ~uint32_t(0U);
	m_BucketIndex_InAllocator = ~uint32_t(0U);
#endif
	m_pNext_InBucket = NULL;
	m_pPrevious_InBucket = NULL;
	m_ObjectAllocated_Number = 0U;
#ifndef NDEBUG
	m_Identity_Block = s_BucketBlockHeader_Identity_Block;
#endif
	m_bInPrevious = false;
}

inline bool PTS_BucketBlockHeader::IsReadyInPrevious() const
{
	//The Block Is Owned By Current Thread 
	assert((m_pAllocator_Owner != NULL) && (::PTSThreadID_Equal(m_ThreadID_Owner, ::PTSThreadID_Self()) != false));

	uint32_t ObjectFreeVector_Public_Number = ::PTSAtomic_Get(&m_ObjectFreeVector_Public_Number);
	
	assert(m_ObjectAllocated_Number >= ObjectFreeVector_Public_Number);
	uint32_t ObjectAllocated_Number_SubtractPublic = m_ObjectAllocated_Number - ObjectFreeVector_Public_Number;

	return (
		(m_ObjectSize*ObjectAllocated_Number_SubtractPublic) <= (((s_Block_Size - sizeof(PTS_BucketBlockHeader)) / 5U) * 4U)) //Empty Enough
		&& 
		((m_ObjectSize*(ObjectAllocated_Number_SubtractPublic + 1U)) <= (s_Block_Size - sizeof(PTS_BucketBlockHeader)) //Not Full
			);
}

inline uint32_t PTS_BucketBlockHeader::ObjectSize() const
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

	PTS_BucketObjectHeader *ObjectFreeVectorPublic_Begin = ::PTSAtomic_GetAndSet(&m_ObjectFreeVector_Public, static_cast<PTS_BucketObjectHeader *>(NULL));

	uint32_t ObjectFreeRepatriated_Number = 0U;
	if (ObjectFreeVectorPublic_Begin != NULL)
	{
		++ObjectFreeRepatriated_Number;

		PTS_BucketObjectHeader **ObjectFreeVectorPublic_End = &ObjectFreeVectorPublic_Begin->m_pNext_InFreeVector;
		while ((*ObjectFreeVectorPublic_End) != NULL)
		{
			++ObjectFreeRepatriated_Number;
			ObjectFreeVectorPublic_End = &((*ObjectFreeVectorPublic_End)->m_pNext_InFreeVector);
		}

		(*ObjectFreeVectorPublic_End) = m_ObjectFreeVector_Private;
		m_ObjectFreeVector_Private = ObjectFreeVectorPublic_Begin;
	}


	m_ObjectAllocated_Number -= ObjectFreeRepatriated_Number;
	assert(m_ObjectAllocated_Number <= (s_Block_Size - sizeof(PTS_BucketBlockHeader)) / m_ObjectSize);

	::PTSAtomic_GetAndAdd(&m_ObjectFreeVector_Public_Number, (uint32_t(0U) - ObjectFreeRepatriated_Number));
}

inline PTS_BucketObjectHeader *PTS_BucketBlockHeader::Alloc()
{
	//The Block Is Owned By Current Thread 
	assert((m_pAllocator_Owner != NULL) && (::PTSThreadID_Equal(m_ThreadID_Owner, ::PTSThreadID_Self()) != false));

	//We Choose [Kukanov 2007] For Better Cache Locality
	//[Hudson 2006] First: BumpPointer | Next: PrivateObject
	//[Kukanov 2007] First: PrivateObject | Next: BumpPointer

	//First: PrivateObject 
	{
		PTS_BucketObjectHeader *pObjectToAlloc = _ObjectFreeVector_Private_Pop();
		if (pObjectToAlloc != NULL)
		{
			return pObjectToAlloc;
		}
	}


	//Next: BumpPointer
	{
		PTS_BucketObjectHeader *pObjectToAlloc = _BumpPointer();
		if (pObjectToAlloc != NULL)
		{
			return pObjectToAlloc;
		}
	}


	//[Hudson 2006] Then: Repatriate	
	ObjectFreeVector_Public_Repatriate();
	
	if (_IsEmpty())
	{
		_BumpPointerRestore();

		PTS_BucketObjectHeader *pObjectToAlloc = _BumpPointer();
		assert(pObjectToAlloc != NULL); //Empty???
		return pObjectToAlloc;
	}
	else
	{
		PTS_BucketObjectHeader *pObjectToAlloc = _ObjectFreeVector_Private_Pop();
		if (pObjectToAlloc != NULL)
		{
			return pObjectToAlloc;
		}
		else
		{
			return NULL;
		}
	}
}

inline void PTS_BucketBlockHeader::Free(PTS_BucketObjectHeader *pObjectToFree)
{
#ifndef NDEBUG
	{
		PTS_BucketObjectHeader *BumpPointer = m_BumpPointer; 
		assert(BumpPointer == NULL || pObjectToFree < BumpPointer);
	}
#endif

	if ((m_pAllocator_Owner != NULL) && (::PTSThreadID_Equal(m_ThreadID_Owner, ::PTSThreadID_Self()) != false))
	{
		//Owning Thread

		//[Hudson 2006] / 2.RELATED WORK / 2.1 Concurrent Malloc/Free
		//Hoard uses one CAS in its common path for malloc and Michael uses two.
		//CAS instructions on current commercially available processors can be from one to two orders of magnitude slower than similar non-atomic operations.
		//Our algorithm avoids atomic operations in the common paths of the malloc and free routines, and uses them in a non-blocking way in the non-common path code.

		//We Should Not RepatriatePublicObject Or PushToBlockManager Here

		//IsReadyInPrevious Has PTSAtomic_Get //But It Just Means A Memory Barrier At Most Time

		--m_ObjectAllocated_Number;
		assert(m_ObjectAllocated_Number <= (s_Block_Size - sizeof(PTS_BucketBlockHeader)) / m_ObjectSize);

		if (_IsEmpty())
		{
			//Empty

			//FreeListPrivate清零，不再有Push的必要
			_BumpPointerRestore();

			if (!_IsInPrevious())
			{
				assert(IsReadyInPrevious()); //Be Empty Must Be Empty Enough

				m_pAllocator_Owner->Remove(this, m_BucketIndex_InAllocator);
				m_pAllocator_Owner->InsertToPrevious(this, m_BucketIndex_InAllocator);
			}
		}
		else
		{
			//Non-Empty

			_ObjectFreeVector_Private_Push(pObjectToFree);

			if ((!_IsInPrevious()) && IsReadyInPrevious())
			{
				m_pAllocator_Owner->Remove(this, m_BucketIndex_InAllocator);
				m_pAllocator_Owner->InsertToPrevious(this, m_BucketIndex_InAllocator);
			}
		}

	}
	else
	{
		//Foreign Thread
		
		_ObjectFreeVector_Public_Push(pObjectToFree);
		
		::PTSAtomic_GetAndAdd(&m_ObjectFreeVector_Public_Number, 1U);
	}
}

#ifndef NDEBUG
inline bool PTS_BucketBlockHeader::IsBlock()
{
	return m_Identity_Block == s_BucketBlockHeader_Identity_Block;
}

#endif


//********************************************************************************************************************************************************************************************************************************************************************
//PT Internal
//********************************************************************************************************************************************************************************************************************************************************************

//不区分Free和Free_Aligned

static PTSTSD_KEY s_ThreadLocalBucketAllocator_Index;

static inline void * PTS_Internal_Alloc(uint32_t Size)
{
	if (Size != 0U)
	{
		if (Size <= s_NonLargeObject_SizeMax)
		{
			PTS_ThreadLocalBucketAllocator *pThreadLocalBucketAllocator = static_cast<PTS_ThreadLocalBucketAllocator *>(::PTSTSD_GetValue(s_ThreadLocalBucketAllocator_Index));

			if (pThreadLocalBucketAllocator == NULL)
			{
				//To Do：
				//Map在大多数平台上的实现是至少分配一页（4096字节）内存
				//而PTS_ThreadLocalBucketAllocator不到500字节
				//整个进程共浪费3500×线程数的字节
				//是否需要调整代码进行节省？
				pThreadLocalBucketAllocator = static_cast<PTS_ThreadLocalBucketAllocator *>(::PTS_MemoryMap_Alloc(sizeof(PTS_ThreadLocalBucketAllocator)));
				assert(pThreadLocalBucketAllocator != NULL);

				pThreadLocalBucketAllocator->Construct();

				bool bResult = ::PTSTSD_SetValue(s_ThreadLocalBucketAllocator_Index, pThreadLocalBucketAllocator);
				assert(bResult != false);
			}

			return pThreadLocalBucketAllocator->Alloc(Size);
		}
		else
		{
			void *pVoidToAlloc = static_cast<PTS_BucketObjectHeader *>(::PTS_MemoryMap_Alloc(Size));
			assert(pVoidToAlloc != NULL);

			//PTS_MemoryMap_Alloc的实现一定是对齐到16KB的
			assert(::PTS_Size_IsAligned(reinterpret_cast<uintptr_t>(pVoidToAlloc), s_Block_Size));

			return pVoidToAlloc;
		}
	}
	else
	{
		return NULL;
	}
}

static inline void * PTS_Internal_Alloc_Aligned(uint32_t Size, uint32_t Alignment)
{
	if (::PTS_Size_IsPowerOfTwo(Alignment))
	{
		if (Size != 0U)
		{
			if (Alignment <= s_NonLargeObject_SizeMax)
			{
				if (Size <= s_NonLargeObject_SizeMax)
				{
					PTS_ThreadLocalBucketAllocator *pThreadLocalBucketAllocator = static_cast<PTS_ThreadLocalBucketAllocator *>(::PTSTSD_GetValue(s_ThreadLocalBucketAllocator_Index));

					if (pThreadLocalBucketAllocator == NULL)
					{
						//To Do：
						//Map在大多数平台上的实现是至少分配一页（4096字节）内存
						//而PTS_ThreadLocalBucketAllocator不到500字节
						//整个进程共浪费3500×线程数的字节
						//是否需要调整代码进行节省？
						pThreadLocalBucketAllocator = static_cast<PTS_ThreadLocalBucketAllocator *>(::PTS_MemoryMap_Alloc(sizeof(PTS_ThreadLocalBucketAllocator)));
						assert(pThreadLocalBucketAllocator != NULL);

						pThreadLocalBucketAllocator->Construct();

						bool bResult = ::PTSTSD_SetValue(s_ThreadLocalBucketAllocator_Index, pThreadLocalBucketAllocator);
						assert(bResult != false);
					}

					//并不存在偏移
					//见PTS_Size_ResolveRequestSize
					//we just align the size up, and request this amount, 
					//because for every size aligned to some power of 2, 
					//the allocated object is at least that aligned.
					void *pVoidToAlloc = pThreadLocalBucketAllocator->Alloc(::PTS_Size_AlignUpFrom(Size, Alignment));

					//Verify Alignment		
					assert(::PTS_Size_IsAligned(reinterpret_cast<uintptr_t>(pVoidToAlloc), static_cast<size_t>(Alignment)));

					return pVoidToAlloc;
				}
				else
				{
					void *pVoidToAlloc = static_cast<PTS_BucketObjectHeader *>(::PTS_MemoryMap_Alloc(Size));
					//内存不足???
					assert(pVoidToAlloc != NULL);

					//PTS_MemoryMap_Alloc的实现一定是对齐到16KB的
					assert(::PTS_Size_IsAligned(reinterpret_cast<uintptr_t>(pVoidToAlloc), s_Block_Size));

					//For Alignment <= s_NonLargeObject_SizeMax
					//Max Of Alignment Is 4K (Which Is Page Size)
					assert(::PTS_Size_IsAligned(reinterpret_cast<uintptr_t>(pVoidToAlloc), static_cast<size_t>(Alignment)));

					return pVoidToAlloc;
				}
			}
			else if (Alignment <= s_Block_Size)
			{
				void *pVoidToAlloc = static_cast<PTS_BucketObjectHeader *>(::PTS_MemoryMap_Alloc(Size));
				//内存不足???
				assert(pVoidToAlloc != NULL);

				//PTS_MemoryMap_Alloc的实现一定是对齐到16KB的
				assert(::PTS_Size_IsAligned(reinterpret_cast<uintptr_t>(pVoidToAlloc), s_Block_Size));

				//Alignment <= 16KB
				assert(::PTS_Size_IsAligned(reinterpret_cast<uintptr_t>(pVoidToAlloc), static_cast<size_t>(Alignment)));

				return pVoidToAlloc;
			}
			else
			{
				//极少发生???
				//很可能是用法上出现了错误???
				assert(0);

				void *pVoidToAlloc = static_cast<PTS_BucketObjectHeader *>(::PTS_MemoryMap_Alloc(::PTS_Size_AlignUpFrom(Size, Alignment)));
				//内存不足???
				assert(pVoidToAlloc != NULL);

				if (pVoidToAlloc != NULL && !::PTS_Size_IsAligned(reinterpret_cast<uintptr_t>(pVoidToAlloc), static_cast<size_t>(Alignment)))
				{
					::PTS_MemoryMap_Free(pVoidToAlloc);
					pVoidToAlloc = NULL;
				}

				return pVoidToAlloc;
			}
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

static inline void PTS_Internal_Free(void *pVoid)
{
	if (pVoid != NULL)
	{
		PTS_BucketBlockHeader *pBlockAssumed = reinterpret_cast<PTS_BucketBlockHeader *>(PTS_Size_AlignDownFrom(reinterpret_cast<uintptr_t>(pVoid), static_cast<size_t>(s_Block_Size)));

		//PTS_BucketObjectHeader一定不是16KB对齐 //见PTS_BucketBlockHeader::_BumpPointerRestore
		//而PTS_MemoryMap_Alloc一定是16KB对齐的

		if (reinterpret_cast<uintptr_t>(pBlockAssumed) != reinterpret_cast<uintptr_t>(pVoid))
		{
			//Block
			assert(pBlockAssumed->IsBlock());

			//并不存在偏移，见PTSMemoryAllocator_Alloc_Aligned
			PTS_BucketObjectHeader *pObjectToFree = static_cast<PTS_BucketObjectHeader *>(pVoid);

			pBlockAssumed->Free(pObjectToFree);
		}
		else
		{
			//LargeObject

			//并不一定可靠
			//但相等的概率理论上只有1/(2^32-1)
			assert(!pBlockAssumed->IsBlock());

			::PTS_MemoryMap_Free(pVoid);
		}
	}
}

static inline uint32_t PTS_Internal_Size(void *pVoid)
{
	if (pVoid != NULL)
	{
		PTS_BucketBlockHeader *pBlockAssumed = reinterpret_cast<PTS_BucketBlockHeader *>(PTS_Size_AlignDownFrom(reinterpret_cast<uintptr_t>(pVoid), static_cast<size_t>(s_Block_Size)));

		//PTS_BucketObjectHeader一定不是16KB对齐 //见PTS_BucketBlockHeader::_BumpPointerRestore
		//而PTS_MemoryMap_Alloc一定是16KB对齐的

		if (reinterpret_cast<uintptr_t>(pBlockAssumed) != reinterpret_cast<uintptr_t>(pVoid))
		{
			//Block
			assert(pBlockAssumed->IsBlock());

			//ObjectSize For Bucket
			return pBlockAssumed->ObjectSize();
		}
		else
		{
			//LargeObject

			//并不一定可靠
			//但相等的概率理论上只有1/(2^32-1)
			assert(!pBlockAssumed->IsBlock());

			return ::PTS_MemoryMap_Size(pVoid);
		}
	}
	else
	{
		return 0U;
	}
}

static inline void * PTS_Internal_Realloc(void *pVoidOld, uint32_t SizeNew)
{
	if (pVoidOld != NULL && SizeNew > 0U)
	{
		PTS_BucketBlockHeader *pBlockAssumed = reinterpret_cast<PTS_BucketBlockHeader *>(PTS_Size_AlignDownFrom(reinterpret_cast<uintptr_t>(pVoidOld), static_cast<size_t>(s_Block_Size)));

		//PTS_BucketObjectHeader一定不是16KB对齐 //见PTS_BucketBlockHeader::_BumpPointerRestore
		//而PTS_MemoryMap_Alloc一定是16KB对齐的

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
	else if (SizeNew > 0U)
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

static inline void * PTS_Internal_Realloc_Aligned(void *pVoidOld, uint32_t SizeNew, uint32_t AlignmentNew)
{
	if (::PTS_Size_IsPowerOfTwo(AlignmentNew))
	{
		if (pVoidOld != NULL && SizeNew > 0U)
		{
			PTS_BucketBlockHeader *pBlockAssumed = reinterpret_cast<PTS_BucketBlockHeader *>(PTS_Size_AlignDownFrom(reinterpret_cast<uintptr_t>(pVoidOld), static_cast<size_t>(s_Block_Size)));

			//PTS_BucketObjectHeader一定不是16KB对齐 //见PTS_BucketBlockHeader::_BumpPointerRestore
			//而PTS_MemoryMap_Alloc一定是16KB对齐的

			if (reinterpret_cast<uintptr_t>(pBlockAssumed) != reinterpret_cast<uintptr_t>(pVoidOld))
			{
				//Block

				assert(pBlockAssumed->IsBlock());

				uint32_t SizeOld = pBlockAssumed->ObjectSize();
				if (SizeOld < SizeNew || !::PTS_Size_IsAligned(reinterpret_cast<uintptr_t>(pVoidOld), static_cast<size_t>(AlignmentNew)))
				{
					void *pVoidNew = ::PTS_Internal_Alloc_Aligned(SizeNew, AlignmentNew);
					assert((pVoidNew != NULL));
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
		else if (SizeNew > 0U)
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
	else
	{
		return NULL;
	}
}


//********************************************************************************************************************************************************************************************************************************************************************
//API Entry
//********************************************************************************************************************************************************************************************************************************************************************

static int32_t s_MemoryAllocator_Initialize_RefCount = 0;
bool PTCALL PTSMemoryAllocator_Initialize()
{
	if (::PTSAtomic_GetAndAdd(&s_MemoryAllocator_Initialize_RefCount, 1) == 0)
	{
		s_BlockStore_Singleton.Construct();

		for (uint32_t i = 0U; i < s_BucketBlockManager_QueueEmpty_CountThreshold; ++i)
		{
			//PTS_MemoryMap_Alloc会串行化
			//预先分配Block以提高并行度
			s_BlockStore_Singleton.PushEmpty(s_BlockStore_Singleton.PopEmpty(0U));
		}

		bool bResult = ::PTSTSD_Create(
			&s_ThreadLocalBucketAllocator_Index,
			[](void *pVoid)->void {
			PTS_ThreadLocalBucketAllocator *pThreadLocalBucketAllocator = static_cast<PTS_ThreadLocalBucketAllocator *>(pVoid);
			pThreadLocalBucketAllocator->Destruct();
			::PTS_MemoryMap_Free(pVoid);
		}
		);
		
		return bResult;
	}
	else
	{
		return true;
	}
}

void * PTCALL PTSMemoryAllocator_Alloc(uint32_t Size)
{
	assert(::PTSAtomic_Get(&s_MemoryAllocator_Initialize_RefCount) > 0);

	return ::PTS_Internal_Alloc(Size);
}

void * PTCALL PTSMemoryAllocator_Alloc_Aligned(uint32_t Size, uint32_t Alignment)
{
	assert(::PTSAtomic_Get(&s_MemoryAllocator_Initialize_RefCount) > 0);

	return ::PTS_Internal_Alloc_Aligned(Size, Alignment);
}

void PTCALL PTSMemoryAllocator_Free(void *pVoid)
{
	assert(::PTSAtomic_Get(&s_MemoryAllocator_Initialize_RefCount) > 0);

	::PTS_Internal_Free(pVoid);
}

void PTCALL PTSMemoryAllocator_Free_Aligned(void *pVoid)
{
	assert(::PTSAtomic_Get(&s_MemoryAllocator_Initialize_RefCount) > 0);
	
	::PTS_Internal_Free(pVoid);
}

uint32_t PTCALL PTSMemoryAllocator_Size(void *pVoid)
{
	assert(::PTSAtomic_Get(&s_MemoryAllocator_Initialize_RefCount) > 0);

	return ::PTS_Internal_Size(pVoid);
}

void * PTCALL PTSMemoryAllocator_Realloc(void *pVoidOld, uint32_t SizeNew)
{
	assert(::PTSAtomic_Get(&s_MemoryAllocator_Initialize_RefCount) > 0);

	return PTS_Internal_Realloc(pVoidOld, SizeNew);
}

void * PTCALL PTSMemoryAllocator_Realloc_Aligned(void *pVoidOld, uint32_t SizeNew, uint32_t AlignmentNew)
{
	assert(::PTSAtomic_Get(&s_MemoryAllocator_Initialize_RefCount) > 0);

	return ::PTS_Internal_Realloc_Aligned(pVoidOld, SizeNew, AlignmentNew);
}

#if defined PTWIN32
#include "Win32/PTSMemoryAllocator.inl"
#elif defined PTPOSIX
#include "Posix/PTSMemoryAllocator.inl"
#else
#error 未知的平台
#endif
