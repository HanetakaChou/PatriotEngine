#include "../../Public/System/PTSThread.h"
#include "../../Public/System/PTSMemoryAllocator.h"
#include "PTSTaskSchedulerImpl.h"

#include <new>

//uint64_t mask;
//int rtval = sched_getaffinity(0, sizeof(uint64_t), reinterpret_cast<cpu_set_t *>(&mask));
//assert(rtval == 0);


#if 0

int FD_stat = ::open("/proc/thread-self/stat", O_RDONLY);
assert(FD_stat != -1);
char Str_stat[4096U];
assert(s_Page_Size == 4096U);
ssize_t ssResult;
ssResult = ::read(FD_stat, Str_stat, 4096U);
assert(ssResult != -1);
int iResult1 = ::close(FD_stat);
assert(iResult1 == 0);

long unsigned startstack;
::sscanf(Str_stat, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %*lu %*lu %*ld %*ld %*ld %*ld %*ld %*ld %*llu %*lu %*ld %*lu %*lu %*lu %lu", &startstack);

#endif

#if __TBB_WIN8UI_SUPPORT
// In Win8UI mode, TBB uses a thread creation API that does not allow to specify the stack size.
// Still, the thread stack size value, either explicit or default, is used by the scheduler.
// So here we set the default value to match the platform's default of 1MB.
const size_t ThreadStackSize = 1024 * 1024 * MByte;
#else
const size_t ThreadStackSize = 1024U * 1024U * (sizeof(uintptr_t) <= 4U ? 2U : 4U);
#endif

static inline uint32_t PTS_Info_HardwareThreadNumber();

static inline bool PTS_Size_IsPowerOfTwo(uint32_t Value);
static inline bool PTS_Size_IsAligned(size_t Value, size_t Alignment);
static inline uint32_t PTS_Size_AlignUpFrom(uint32_t Value, uint32_t Alignment);
static inline uint64_t PTS_Size_AlignUpFrom(uint64_t Value, uint64_t Alignment); 

static inline IPTSTask *PTS_Internal_Task_Alloc(size_t SizeTask, size_t AlignmentTask);
static inline void PTS_Internal_Task_Spawn(PTSArena *pArena, uint32_t Slot_Index, PTSTaskPrefixImpl *pTaskToSpawnPrefix);
static inline void PTS_Internal_Master_Main(PTSArena *pArena, uint32_t Slot_Index, PTSTaskPrefixImpl * pTaskRootPrefix, uint32_t *pHasBeenFinished);
static inline void PTS_Internal_Worker_Main(PTSArena *pArena, uint32_t Slot_Index);

static PTSMarket *s_Market_Singleton_Pointer = NULL;
static PTSTSD_KEY s_TaskScheduler_Index;

//Linear Congruential Generator
class PTS_Random_LCG
{
	static const uint32_t c = 12345U;
	static const uint32_t a = 1103515245U;
	uint32_t x;

public:
	inline PTS_Random_LCG(uint32_t Seed) :x(Seed)
	{

	}

	uint32_t Generate()
	{
		x = a * x + c;
		return (x << 1U) >> 17U;
	}
};

//------------------------------------------
inline PTSMarket::PTSMarket(uint32_t ThreadNumber) :
	m_ArenaPointerArrayMemory(new(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSArena) * 64U, alignof(PTSArena)))PTSArena *[64]{}),
	m_ArenaPointerArraySize(0U),
	m_ThreadArrayMemory(new(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSThread)*ThreadNumber, alignof(PTSThread)))PTSThread[ThreadNumber]{}),
	m_ThreadNumber(ThreadNumber),
	m_ArenaPointerArrayCapacity(64U)
{
	PTBOOL tbResult = ::PTSSemaphore_Create(0U, &m_Semaphore);
	assert(tbResult != PTFALSE);

	for (uint32_t i = 0; i < m_ThreadNumber; ++i)
	{
		tbResult = ::PTSThread_Create(Worker_Thread_Main, this, m_ThreadArrayMemory + i);
		assert(tbResult != PTFALSE);
	}
}

inline PTSArena *PTSMarket::Arena_Allocate(float fThreadNumberRatio)
{
	uint32_t ArenaCapacity = static_cast<uint32_t>(::ceilf(static_cast<float>(m_ThreadNumber) * fThreadNumberRatio));

	PTSArena *pArenaAllocated = NULL;

	//Recycle
	if (m_ArenaPointerArraySize != 0U)
	{
		PTS_Random_LCG RandomTemp(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&pArenaAllocated)));

		uint32_t IndexRandom = RandomTemp.Generate() % m_ArenaPointerArraySize;

		{
			for (uint32_t i = IndexRandom; i < m_ArenaPointerArraySize; ++i)
			{
				PTSArena *pArenaToRecycle = m_ArenaPointerArrayMemory[i];

				if (pArenaToRecycle != NULL) //数组中指针初始化为NULL
				{
					if (pArenaToRecycle->Capacity() == ArenaCapacity) //应该认为Capacity相等存在较大的可能性 以8线程为例 Capacity可能的取值有1 2 3 4 5 6 7 共7种
					{
						//MasterThread的SlotIndex一定为0
						if (pArenaToRecycle->Slot_Acquire_Master())
						{
							pArenaAllocated = pArenaToRecycle;
							break;
						}
					}
				}
			}
		}

		if (pArenaAllocated == NULL)
		{
			for (uint32_t i = 0U; i < IndexRandom; ++i)
			{
				PTSArena *pArenaToRecycle = m_ArenaPointerArrayMemory[i];

				if (pArenaToRecycle != NULL) //数组中指针初始化为NULL
				{
					if (pArenaToRecycle->Capacity() == ArenaCapacity) //应该认为Capacity相等存在较大的可能性 以8线程为例 Capacity可能的取值有1 2 3 4 5 6 7 共7种
					{
						//MasterThread的SlotIndex一定为0
						if (pArenaToRecycle->Slot_Acquire_Master())
						{
							pArenaAllocated = pArenaToRecycle;
							break;
						}
					}
				}
			}
		}
	}

	//New
	if (pArenaAllocated == NULL)
	{
		//MasterThread调用本函数时，Arena尚未在到Market中被标记为可用，不存在与WorkerThread并发访问的可能性
		//MasterThread的SlotIndex一定为0
		PTSArena *pArenaNew = new(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSArena), alignof(PTSArena)))PTSArena(ArenaCapacity);
		assert(pArenaNew != NULL);

		//只会Push 不会Push 不存在ABA //当Size为0时 空闲 可回收
		uint32_t Index = ::PTSAtomic_GetAndAdd(&m_ArenaPointerArraySize, 1U);
		assert(Index < m_ArenaPointerArrayCapacity);

		//数组中指针初始化为NULL，无害
		m_ArenaPointerArrayMemory[Index] = pArenaNew;

		pArenaAllocated = pArenaNew;
	}

	return pArenaAllocated;
}

inline PTSArena *PTSMarket::Arena_Acquire(uint32_t *pSlot_Index)
{
	PTSArena *pArenaAcquired = NULL;

	if (m_ArenaPointerArraySize != 0U)
	{
		PTS_Random_LCG RandomTemp(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&pArenaAcquired)));

		uint32_t IndexRandom = RandomTemp.Generate() % m_ArenaPointerArraySize;

		for (uint32_t i = IndexRandom; i < m_ArenaPointerArraySize; ++i)
		{
			PTSArena *pArena = m_ArenaPointerArrayMemory[i];

			if (pArena != NULL)  //数组中指针初始化为NULL
			{
				if (pArena->Size_Load_Acquire() < pArena->Capacity()) //Slot_Release中对SlotArrayMemory::HasBeenAcquired和SlotArraySize的操作并不是原子的 可能会导致无法充分调度
				{
					if (pArena->Slot_Acquire(pSlot_Index) != NULL)
					{
						pArenaAcquired = pArena;
						break;
					}
				}
			}
		}

		if (pArenaAcquired != NULL)
		{
			return pArenaAcquired;
		}

		for (uint32_t i = 0U; i < IndexRandom; ++i)
		{
			PTSArena *pArena = m_ArenaPointerArrayMemory[i];

			if (pArena != NULL)  //数组中指针初始化为NULL
			{
				if (pArena->Size_Load_Acquire() < pArena->Capacity()) //Slot_Release中对SlotArrayMemory::HasBeenAcquired和SlotArraySize的操作并不是原子的 可能会导致无法充分调度
				{
					if (pArena->Slot_Acquire(pSlot_Index) != NULL)
					{
						pArenaAcquired = pArena;
						break;
					}
				}
			}
		}
	}

	return pArenaAcquired;
}

inline unsigned __stdcall PTSMarket::Worker_Thread_Main(void *pMarketVoid)
{
	PTSTaskSchedulerWorkerImpl *pTaskScheduler = new(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSTaskSchedulerWorkerImpl), alignof(PTSTaskSchedulerWorkerImpl)))PTSTaskSchedulerWorkerImpl{};
	assert(pTaskScheduler != NULL);

	PTBOOL tbResult = ::PTSTSD_SetValue(s_TaskScheduler_Index, pTaskScheduler);
	assert(tbResult != PTFALSE);

	PTSMarket *pMarket = static_cast<PTSMarket *>(pMarketVoid);

	while (true)
	{
		tbResult = ::PTSSemaphore_Passern(&pMarket->m_Semaphore);
		assert(tbResult != PTFALSE);
		tbResult = ::PTSSemaphore_Vrijgeven(&pMarket->m_Semaphore);
		assert(tbResult != PTFALSE);

		PTSArena *pArena;
		uint32_t Slot_Index;

		pArena = pMarket->Arena_Acquire(&Slot_Index);

		if (pArena != NULL)
		{
			pTaskScheduler->m_pArena = pArena;
			pTaskScheduler->m_Slot_Index = Slot_Index;

			::PTS_Internal_Worker_Main(pArena, Slot_Index);

			pArena->Slot_Release(Slot_Index);
		}
	}

	return 0U;
}

inline void PTSMarket::Worker_Wake(uint32_t ThreadNumber)
{
	PTBOOL tbResult;
	
	for (uint32_t i = 0; i < ThreadNumber; ++i)
	{
		tbResult = ::PTSSemaphore_Vrijgeven(&m_Semaphore);
		assert(tbResult != PTFALSE);
	}
}

inline void PTSMarket::Worker_Sleep(uint32_t ThreadNumber)
{
	PTBOOL tbResult;

	for (uint32_t i = 0; i < ThreadNumber; ++i)
	{
		assert(s_Market_Singleton_Pointer != NULL);
		tbResult = ::PTSSemaphore_Passern(&s_Market_Singleton_Pointer->m_Semaphore);
		assert(tbResult != PTFALSE);
	}
}

//-----------------------------------------------------------------------------------------------------------
inline PTSArena::PTSArena(uint32_t Capacity) :
	m_SlotIndexMaximum(0U),
	m_SlotArraySize(0U),
	m_SlotArrayMemory(new(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSArenaSlot)*Capacity, alignof(PTSArenaSlot)))PTSArenaSlot[Capacity]{}),
	m_SlotArrayCapacity(Capacity)
{
	//MasterThread调用本函数时，Arena尚未在到Market中被标记为可用，不存在与WorkerThread并发访问的可能性
	//MasterThread的SlotIndex一定为0
	assert(m_SlotArraySize == 0U);
	++m_SlotArraySize;
	assert(m_SlotArrayMemory[0].m_HasBeenAcquired == 0U);
	m_SlotArrayMemory[0].m_HasBeenAcquired = 1U;
}

inline bool PTSArena::Slot_Acquire_Master()
{
	//MasterThread的SlotIndex一定为0
	if (::PTSAtomic_CompareAndSet(&m_SlotArraySize, 0U, 1U) == 0U)
	{
		assert(m_SlotArrayMemory[0].m_HasBeenAcquired == 0U);
		m_SlotArrayMemory[0].m_HasBeenAcquired = 1U;
		return true;
	}
	else
	{
		return false;
	}
}

inline PTSArenaSlot * PTSArena::Slot_Acquire(uint32_t *pSlot_Index)
{
	PTSArenaSlot *pSlotAcquired = NULL;

	uint32_t Slot_Index;

	PTS_Random_LCG RandomTemp(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&pSlotAcquired)));

	assert(m_SlotArrayCapacity != 0U);
	uint32_t IndexRandom = RandomTemp.Generate() % (m_SlotArrayCapacity - 1U) + 1U; //MasterThread的SlotIndex默认为0

	{
		for (uint32_t i = IndexRandom; i < m_SlotArrayCapacity; ++i)
		{
			if (::PTSAtomic_CompareAndSet(&m_SlotArrayMemory[i].m_HasBeenAcquired, 0U, 1U) == 0U)
			{
				::PTSAtomic_GetAndAdd(&m_SlotArraySize, 1U);
				Slot_Index = i;
				pSlotAcquired = m_SlotArrayMemory + i;
				break;
			}
		}
	}

	if (pSlotAcquired == NULL)
	{
		for (uint32_t i = 1U; i < IndexRandom; ++i)
		{
			if (::PTSAtomic_CompareAndSet(&m_SlotArrayMemory[i].m_HasBeenAcquired, 0U, 1U) == 0U)
			{
				::PTSAtomic_GetAndAdd(&m_SlotArraySize, 1U);
				Slot_Index = i;
				pSlotAcquired = m_SlotArrayMemory + i;
				break;
			}
		}
	}

	if (pSlotAcquired != NULL)
	{
		uint32_t SlotIndexMaximumOld = ::PTSAtomic_Get(&m_SlotIndexMaximum);

		while (Slot_Index > SlotIndexMaximumOld)
		{
			if (::PTSAtomic_CompareAndSet(&m_SlotIndexMaximum, SlotIndexMaximumOld, Slot_Index) == SlotIndexMaximumOld)
			{
				break;
			}
		}

		(*pSlot_Index) = Slot_Index;
	}

	return pSlotAcquired;
}

inline void PTSArena::Slot_Release(uint32_t Slot_Index)
{
	union
	{
		struct
		{
			uint32_t m_Head;
			uint32_t m_Tail;
		}m_OneWord;
		uint64_t m_TwoWord;
	}HeadAndTailOld;
	HeadAndTailOld.m_TwoWord = ::PTSAtomic_Get(&m_SlotArrayMemory[Slot_Index].m_HeadAndTail.m_TwoWord);
	assert(HeadAndTailOld.m_OneWord.m_Head == HeadAndTailOld.m_OneWord.m_Tail);

	uint32_t HasBeenAcquiredOld = ::PTSAtomic_GetAndSet(&m_SlotArrayMemory[Slot_Index].m_HasBeenAcquired, 0U);
	assert(HasBeenAcquiredOld == 1U);

	::PTSAtomic_GetAndAdd(&m_SlotArraySize, uint32_t(-1));
}

inline uint32_t PTSArena::SlotIndexMaximum_Load_Acquire()
{
	return ::PTSAtomic_Get(&m_SlotIndexMaximum);
}

inline uint32_t PTSArena::Size_Load_Acquire()
{
	return ::PTSAtomic_Get(&m_SlotArraySize);
}

inline PTSArenaSlot *PTSArena::Slot(uint32_t Index)
{
	return m_SlotArrayMemory + Index;
}

inline uint32_t PTSArena::Capacity()
{
	return m_SlotArrayCapacity;
}

//------------------------------------------------------------------------------------------------------------
inline PTSArenaSlot::PTSArenaSlot() :m_HeadAndTail{ 0U,0U }, m_HasBeenAcquired(0U), m_TaskDequeMemoryS{ NULL,NULL,NULL,NULL }, m_TaskDequeCapacity(0U)
{

}

inline void PTSArenaSlot::TaskDeque_Push(PTSTaskPrefixImpl *pTaskToPush)
{
	union
	{
		struct
		{
			uint32_t m_Head;
			uint32_t m_Tail;
		}m_OneWord;
		uint64_t m_TwoWord;
	}HeadAndTailOld;

	HeadAndTailOld.m_TwoWord = ::PTSAtomic_Get(&m_HeadAndTail.m_TwoWord);

	//Allocate
	if ((HeadAndTailOld.m_OneWord.m_Tail - HeadAndTailOld.m_OneWord.m_Head) + 1 >= m_TaskDequeCapacity)
	{
		//64(1) + 64(1) + 128(2)
		//0       1       2 3   
		switch (m_TaskDequeCapacity)
		{
		case 0U:
		{
			::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(&m_TaskDequeMemoryS[0]), reinterpret_cast<uintptr_t>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSTaskPrefixImpl *) * 64U, s_CacheLine_Size)));
			::PTSAtomic_Set(&m_TaskDequeCapacity, 64U);
		}
		break;
		case 64U:
		{
			::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(&m_TaskDequeMemoryS[1]), reinterpret_cast<uintptr_t>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSTaskPrefixImpl *) * 64U, s_CacheLine_Size)));
			for (uint32_t i = HeadAndTailOld.m_OneWord.m_Head; i < HeadAndTailOld.m_OneWord.m_Tail; ++i)
			{
				uint32_t indexOld = i % 64U;
				uint32_t indexNew = i % 128U;
				if (indexNew >= 64U)
				{
					::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(&m_TaskDequeMemoryS[indexOld / 64][indexOld % 64]), reinterpret_cast<uintptr_t>(m_TaskDequeMemoryS[indexNew / 64][indexNew % 64]));
				}
			}
			::PTSAtomic_Set(&m_TaskDequeCapacity, 128U);
		}
		break;
		case 128U:
		{
			::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(&m_TaskDequeMemoryS[2]), reinterpret_cast<uintptr_t>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSTaskPrefixImpl *) * 64U, s_CacheLine_Size)));
			::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(&m_TaskDequeMemoryS[3]), reinterpret_cast<uintptr_t>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSTaskPrefixImpl *) * 64U, s_CacheLine_Size)));
			for (uint32_t i = HeadAndTailOld.m_OneWord.m_Head; i < HeadAndTailOld.m_OneWord.m_Tail; ++i)
			{
				uint32_t indexOld = i % 128U;
				uint32_t indexNew = i % 256U;
				if (indexNew >= 128U)
				{
					::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(&m_TaskDequeMemoryS[indexOld / 64][indexOld % 64]), reinterpret_cast<uintptr_t>(m_TaskDequeMemoryS[indexNew / 64][indexNew % 64]));
				}
			}
			::PTSAtomic_Set(&m_TaskDequeCapacity, 256U);
		}
		break;
		default:
			assert(0);
		}
	}

	//__TBB_ASSERT(t->state() == task::allocated, "attempt to spawn task that is not in 'allocated' state");
	//t->prefix().state = task::ready;

	//Capacity只可能被Owner线程写入
	uint32_t Index = HeadAndTailOld.m_OneWord.m_Tail % m_TaskDequeCapacity;
	m_TaskDequeMemoryS[Index / 64U][Index % 64U] = pTaskToPush;

	//Tail只可能被Owner线程写入
	assert((HeadAndTailOld.m_OneWord.m_Tail + 1U) > 0U);//防止环绕
	::PTSAtomic_Set(&m_HeadAndTail.m_OneWord.m_Tail, HeadAndTailOld.m_OneWord.m_Tail + 1U);
}

inline PTSTaskPrefixImpl * PTSArenaSlot::TaskDeque_Pop_Private()
{
	while (true)
	{
		union
		{
			struct
			{
				uint32_t m_Head;
				uint32_t m_Tail;
			}m_OneWord;
			uint64_t m_TwoWord;
		}HeadAndTailOld;

		HeadAndTailOld.m_TwoWord = ::PTSAtomic_Get(&m_HeadAndTail.m_TwoWord);

		union
		{
			struct
			{
				uint32_t m_Head;
				uint32_t m_Tail;
			}m_OneWord;
			uint64_t m_TwoWord;
		}HeadAndTailNew;

		if (HeadAndTailOld.m_OneWord.m_Head >= HeadAndTailOld.m_OneWord.m_Tail)
		{
			//Reset //防止环绕
			HeadAndTailNew.m_OneWord.m_Head = 0;
			HeadAndTailNew.m_OneWord.m_Tail = 0;

			//只允许Owner线程Reset，保证了Tail只可能被Owner线程写入
			if (::PTSAtomic_CompareAndSet(&m_HeadAndTail.m_TwoWord, HeadAndTailOld.m_TwoWord, HeadAndTailNew.m_TwoWord) == HeadAndTailOld.m_TwoWord)
			{
				return NULL;
			}
			//else
			//{
			//	continue;
			//}
		}
		else
		{
			HeadAndTailNew.m_OneWord.m_Head = HeadAndTailOld.m_OneWord.m_Head;
			HeadAndTailNew.m_OneWord.m_Tail = HeadAndTailOld.m_OneWord.m_Tail - 1U;

			uint32_t Index = HeadAndTailNew.m_OneWord.m_Tail % m_TaskDequeCapacity;
			PTSTaskPrefixImpl * pTaskToPop = m_TaskDequeMemoryS[Index / 64U][Index % 64U];

			if (::PTSAtomic_CompareAndSet(&m_HeadAndTail.m_TwoWord, HeadAndTailOld.m_TwoWord, HeadAndTailNew.m_TwoWord) == HeadAndTailOld.m_TwoWord)
			{
				return pTaskToPop;
			}
			//else
			//{
			//	continue;
			//}
		}
	}
}

inline PTSTaskPrefixImpl * PTSArenaSlot::TaskDeque_Pop_Public()
{
	while (true)
	{
		union
		{
			struct
			{
				uint32_t m_Head;
				uint32_t m_Tail;
			}m_OneWord;
			uint64_t m_TwoWord;
		}HeadAndTailOld;

		HeadAndTailOld.m_TwoWord = ::PTSAtomic_Get(&m_HeadAndTail.m_TwoWord);

		if (HeadAndTailOld.m_OneWord.m_Head >= HeadAndTailOld.m_OneWord.m_Tail)
		{
			return NULL;
		}
		else
		{
			union
			{
				struct
				{
					uint32_t m_Head;
					uint32_t m_Tail;
				}m_OneWord;
				uint64_t m_TwoWord;
			}HeadAndTailNew;

			HeadAndTailNew.m_OneWord.m_Head = HeadAndTailOld.m_OneWord.m_Head + 1U;
			HeadAndTailNew.m_OneWord.m_Tail = HeadAndTailOld.m_OneWord.m_Tail;

			uint32_t Index = HeadAndTailOld.m_OneWord.m_Head % ::PTSAtomic_Get(&m_TaskDequeCapacity);
			PTSTaskPrefixImpl * pTaskToPop = m_TaskDequeMemoryS[Index / 64U][Index % 64U];

			if (::PTSAtomic_CompareAndSet(&m_HeadAndTail.m_TwoWord, HeadAndTailOld.m_TwoWord, HeadAndTailNew.m_TwoWord) == HeadAndTailOld.m_TwoWord)
			{
				return pTaskToPop;
			}
			//else
			//{
			//	continue;
			//}
		}

	}
}

//------------------------------------------------------------------------------------------------------------
static uint32_t const  s_TaskPrefix_Reservation_Size = ::PTS_Size_AlignUpFrom(static_cast<uint32_t>(sizeof(PTSTaskPrefixImpl)), s_TaskPrefix_Alignment);

static inline PTSTaskPrefixImpl * PTS_Internal_Task_Prefix(IPTSTask *pTask)
{
	return reinterpret_cast<PTSTaskPrefixImpl *>(reinterpret_cast<uintptr_t>(pTask) - s_TaskPrefix_Reservation_Size);
}

inline PTSTaskPrefixImpl::PTSTaskPrefixImpl() :m_Parent(NULL), m_RefCount(0U), m_pFn_Execute(NULL), m_State(Allocated)
{

}

IPTSTaskPrefix *PTSTaskPrefixImpl::Parent()
{
	return m_Parent;
}

void PTSTaskPrefixImpl::ParentSet(IPTSTaskPrefix *pParent)
{
	m_Parent = static_cast<PTSTaskPrefixImpl *>(pParent);
}

void PTSTaskPrefixImpl::Recycle_AsChildOf(IPTSTaskPrefix *pParent)
{
	assert(m_State == Executing || m_State == Allocated); //execute not running, or already recycled
	assert(m_RefCount == 0U); //no child tasks allowed when recycled as a child
	assert(m_Parent == NULL); //parent must be null //使用allocate_continuation
	assert(static_cast<PTSTaskPrefixImpl *>(pParent)->m_State != Freed); //parent already freed

	m_State = Allocated;
	m_Parent = static_cast<PTSTaskPrefixImpl *>(pParent);
}

void PTSTaskPrefixImpl::RefCount_Set(uint32_t RefCount)
{
	assert((m_State&Debug_RefCount_InUse) == 0); //应当在SpawnTask之前SetRefCount
	m_RefCount = RefCount;
}

void PTSTaskPrefixImpl::OverrideExecute(IPTSTask *(*pFn_Execute)(IPTSTask *pTaskThisVoid))
{
	m_pFn_Execute = pFn_Execute;
}

inline PTSTaskPrefixImpl *PTSTaskPrefixImpl::Execute()
{
	assert(m_pFn_Execute != NULL);
	IPTSTask *pTask = reinterpret_cast<IPTSTask *>(reinterpret_cast<uintptr_t>(this) + s_TaskPrefix_Reservation_Size);
	IPTSTask *pTaskByPass = static_cast<IPTSTask *>(m_pFn_Execute(pTask));
	return (pTaskByPass != NULL) ? (::PTS_Internal_Task_Prefix(pTaskByPass)) : NULL;
}



//------------------------------------------------------------------------------------------------------------
inline PTSTaskSchedulerMasterImpl::PTSTaskSchedulerMasterImpl(PTSArena *pArena, uint32_t Slot_Index) :m_pArena(pArena), m_Slot_Index(Slot_Index)
{

}

void PTSTaskSchedulerMasterImpl::Worker_Wake()
{
	s_Market_Singleton_Pointer->Worker_Wake(m_pArena->m_SlotArrayCapacity);
}

void PTSTaskSchedulerMasterImpl::Worker_Sleep()
{
	s_Market_Singleton_Pointer->Worker_Sleep(m_pArena->m_SlotArrayCapacity);
}

IPTSTask *PTSTaskSchedulerMasterImpl::Task_Allocate(size_t SizeTask, size_t AlignmentTask)
{	
	return ::PTS_Internal_Task_Alloc(SizeTask, AlignmentTask);
}

void PTSTaskSchedulerMasterImpl::Task_Spawn(IPTSTask *pTask)
{
	::PTS_Internal_Task_Spawn(m_pArena, m_Slot_Index, ::PTS_Internal_Task_Prefix(pTask));
}


void PTSTaskSchedulerMasterImpl::Task_Spawn_Root_And_Wait(IPTSTask *pTaskRoot)
{
	assert(pTaskRoot != NULL);

	uint32_t HasBeenFinished = 0U;

	class PTSTaskWait :public IPTSTask
	{
		uint32_t *m_pHasBeenFinished;
	public:
		inline PTSTaskWait(uint32_t *pHasBeenFinished) :m_pHasBeenFinished(pHasBeenFinished)
		{

		}

		static IPTSTask * Execute(IPTSTask *pTaskThisVoid)
		{
			PTSTaskWait *pTaskThis = static_cast<PTSTaskWait *>(pTaskThisVoid);
			::PTSAtomic_Set(pTaskThis->m_pHasBeenFinished, 1U);
			return NULL;
		}
	};

	PTSTaskWait *pTaskWait = new(this->Task_Allocate(sizeof(PTSTaskWait), alignof(PTSTaskWait)))PTSTaskWait(&HasBeenFinished);
	PTSTaskPrefixImpl *pTaskWaitPrefix = ::PTS_Internal_Task_Prefix(pTaskWait);
	pTaskWaitPrefix->m_pFn_Execute = PTSTaskWait::Execute;

	PTSTaskPrefixImpl *pTaskRootPrefix = ::PTS_Internal_Task_Prefix(pTaskRoot);
	
	pTaskWaitPrefix->m_RefCount = 1U;
	assert(pTaskRootPrefix->m_Parent == NULL); //Root
	pTaskRootPrefix->m_Parent = pTaskWaitPrefix;

	assert(pTaskRootPrefix->m_State == PTSTaskPrefixImpl::Allocated); //attempt to spawn task that is not in 'allocated' state
	pTaskRootPrefix->m_State = PTSTaskPrefixImpl::Ready;

	assert(pTaskRootPrefix->m_pFn_Execute != NULL);
	::PTS_Internal_Master_Main(m_pArena, m_Slot_Index, pTaskRootPrefix, &HasBeenFinished);
}

//------------------------------------------------------------------------------------------------------------
inline PTSTaskSchedulerWorkerImpl::PTSTaskSchedulerWorkerImpl()
{

}

void PTSTaskSchedulerWorkerImpl::Worker_Wake()
{
	assert(0);
}

void PTSTaskSchedulerWorkerImpl::Worker_Sleep()
{
	assert(0);
}


IPTSTask *PTSTaskSchedulerWorkerImpl::Task_Allocate(size_t SizeTask, size_t AlignmentTask)
{
	return ::PTS_Internal_Task_Alloc(SizeTask, AlignmentTask);
}

void PTSTaskSchedulerWorkerImpl::Task_Spawn(IPTSTask *pTask)
{
	::PTS_Internal_Task_Spawn(m_pArena, m_Slot_Index, ::PTS_Internal_Task_Prefix(pTask));
}

void PTSTaskSchedulerWorkerImpl::Task_Spawn_Root_And_Wait(IPTSTask *pTask)
{
	assert(0);
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

static inline uint32_t PTS_Size_AlignUpFrom(uint32_t Value, uint32_t Alignment)
{
	return ((Value - 1U) | (Alignment - 1U)) + 1U;
}

static inline uint64_t PTS_Size_AlignUpFrom(uint64_t Value, uint64_t Alignment)
{
	return ((Value - 1U) | (Alignment - 1U)) + 1U;
}

//------------------------------------------------------------------------------------------------------------
static inline IPTSTask *PTS_Internal_Task_Alloc(size_t SizeTask, size_t AlignmentTask)
{
	IPTSTaskPrefix *pTaskPrefix;
	IPTSTask *pTask;

	assert(::PTS_Size_IsPowerOfTwo(static_cast<uint32_t>(AlignmentTask)));
	assert(::PTS_Size_IsAligned(s_TaskPrefix_Alignment, AlignmentTask));

	pTaskPrefix = ::new(::PTSMemoryAllocator_Alloc_Aligned(s_TaskPrefix_Reservation_Size + static_cast<uint32_t>(SizeTask), s_TaskPrefix_Alignment))PTSTaskPrefixImpl{};
	assert(pTaskPrefix != NULL);

	pTask = reinterpret_cast<IPTSTask *>(reinterpret_cast<uintptr_t>(pTaskPrefix) + s_TaskPrefix_Reservation_Size);


	assert(::PTS_Size_IsAligned(reinterpret_cast<uintptr_t>(pTaskPrefix), s_TaskPrefix_Alignment));
	assert(::PTS_Size_IsAligned(reinterpret_cast<uintptr_t>(pTask), AlignmentTask));

	return pTask;
}

static inline void PTS_Internal_Task_Spawn(PTSArena *pArena, uint32_t Slot_Index, PTSTaskPrefixImpl *pTaskToSpawnPrefix)
{
	assert(pTaskToSpawnPrefix->m_State == PTSTaskPrefixImpl::Allocated); //attempt to spawn task that is not in 'allocated' state
	pTaskToSpawnPrefix->m_State = PTSTaskPrefixImpl::Ready;

	//Debug
	//pTaskToSpawnPrefix->m_State |= PTSTaskPrefixImpl::Debug_RefCount_InUse;

	PTSArenaSlot *pArenaSlot = pArena->Slot(Slot_Index);
	pArenaSlot->TaskDeque_Push(pTaskToSpawnPrefix);
}

static inline void PTS_Internal_Master_Main(PTSArena *pArena, uint32_t Slot_Index, PTSTaskPrefixImpl *pTaskRootPrefix, uint32_t *pHasBeenFinished)
{
	assert(pTaskRootPrefix != NULL);

	PTSTaskPrefixImpl *pTaskExecuting = pTaskRootPrefix;
	PTS_Random_LCG RandomTemp(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&pTaskExecuting)));

	//Victim Task Deque
	while (pTaskExecuting)
	{
		//Local Task Deque
		while (pTaskExecuting)
		{
			//ByPass + Recycle
			while (pTaskExecuting)
			{
				assert(pTaskExecuting->m_State == PTSTaskPrefixImpl::Ready);
				pTaskExecuting->m_State = PTSTaskPrefixImpl::Executing;

				//ByPass
				PTSTaskPrefixImpl *pTaskByPass = pTaskExecuting->Execute();

				//Recycle
				switch (pTaskExecuting->m_State)
				{
				case PTSTaskPrefixImpl::Executing://Not Recycle
				{
					assert(pTaskExecuting->m_RefCount == 0U); //Task still has children after it has been executed
					
					PTSTaskPrefixImpl *pTaskParent = pTaskExecuting->m_Parent;
					
					//Free
					pTaskExecuting->~PTSTaskPrefixImpl();
					::PTSMemoryAllocator_Free_Aligned(pTaskExecuting);
					
					//Parent
					if (pTaskParent != NULL)
					{
						uintptr_t RefCountOld = ::PTSAtomic_GetAndAdd(&pTaskParent->m_RefCount, uint32_t(-1));
						if (RefCountOld == 1)
						{
							//pTaskParent->m_State &= ~Debug_RefCount_InUse;

							if (pTaskByPass == NULL)
							{
								//ByPass

								assert(pTaskParent->m_State == PTSTaskPrefixImpl::Allocated); //attempt to spawn task that is not in 'allocated' state
								pTaskParent->m_State = PTSTaskPrefixImpl::Ready;

								pTaskByPass = pTaskParent;
							}
							else
							{
								PTS_Internal_Task_Spawn(pArena, Slot_Index, pTaskParent);
							}
						}
					}
				}
				break;
				case PTSTaskPrefixImpl::Allocated://Recycle As Child Or Continuation
				{
					pTaskExecuting->m_State = PTSTaskPrefixImpl::Ready;
				}
				break;
				default:
					assert(0); //illegal state 
				}

				pTaskExecuting = pTaskByPass;
			}

			//Local Task Deque
			pTaskExecuting = pArena->Slot(Slot_Index)->TaskDeque_Pop_Private();
		}

		//Victim Task Deque
		uint32_t CountSlot = pArena->SlotIndexMaximum_Load_Acquire() + 1U; //Acquire
		int32_t CountFailure = 0;
		int32_t CountYield = 0;

		while (!::PTSAtomic_Get(pHasBeenFinished)) //Stop Only When HasBeenFinished
		{
			uint32_t IndexVictim = RandomTemp.Generate() % CountSlot;
			if (IndexVictim != Slot_Index)
			{
				PTSArenaSlot *pVictim = pArena->Slot(IndexVictim);
				pTaskExecuting = pVictim->TaskDeque_Pop_Public();
				if (pTaskExecuting != NULL)
				{
					break;
				}
			}

			for (int32_t i = 0; i < 1000; i++) //#define STEALING_PAUSE_MASTER
			{
				::PTS_Pause();
			}

			const int32_t CountFailureThreshold = 2 * int32_t(CountSlot);
			if (CountFailure >= CountFailureThreshold)
			{
				::PTS_Yield();
				++CountYield;

				if (CountYield >= 100) //#define YIELD_COUNT_THRESHOLD_MASTER
				{
					for (uint32_t i = 0U; i < IndexVictim; ++i)
					{
						if (i != Slot_Index)
						{
							PTSArenaSlot *pVictim = pArena->Slot(i);
							pTaskExecuting = pVictim->TaskDeque_Pop_Public();
							if (pTaskExecuting != NULL)
							{
								break;
							}
						}
					}

					if (pTaskExecuting != NULL)
					{
						break;
					}

					for (uint32_t i = IndexVictim + 1U; i < CountSlot; ++i)
					{
						if (i != Slot_Index)
						{
							PTSArenaSlot *pVictim = pArena->Slot(i);
							pTaskExecuting = pVictim->TaskDeque_Pop_Public();
							if (pTaskExecuting != NULL)
							{
								break;
							}
						}
					}

					if (pTaskExecuting != NULL)
					{
						break;
					}

					CountSlot = pArena->SlotIndexMaximum_Load_Acquire() + 1U; //Acquire
				}
			}

			++CountFailure;
		}
	}

}

static inline void PTS_Internal_Worker_Main(PTSArena *pArena, uint32_t Slot_Index)
{
	PTSTaskPrefixImpl *pTaskExecuting = NULL;
	
	PTS_Random_LCG RandomTemp(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&pTaskExecuting)));

	//Steal Task / Victim Task Deque
	{
		uint32_t CountSlot = pArena->SlotIndexMaximum_Load_Acquire() + 1U; //Acquire
		int32_t CountFailure = 0;
		int32_t CountYield = 0;

		while (CountFailure < 1500) //#define STEALING_COUNT_WORKER
		{
			uint32_t IndexVictim = RandomTemp.Generate() % CountSlot;
			if (IndexVictim != Slot_Index)
			{
				PTSArenaSlot *pVictim = pArena->Slot(IndexVictim);
				pTaskExecuting = pVictim->TaskDeque_Pop_Public();
				if (pTaskExecuting != NULL)
				{
					break;
				}
			}

			for (int32_t i = 0; i < 1500; i++) //#define STEALING_PAUSE_WORKER
			{
				::PTS_Pause();
			}

			const int32_t CountFailureThreshold = 2 * int32_t(CountSlot);
			if (CountFailure >= CountFailureThreshold)
			{
				::PTS_Yield();
				++CountYield;

				if (CountYield >= 100) //#define YIELD_COUNT_THRESHOLD_MASTER
				{
					for (uint32_t i = 0U; i < IndexVictim; ++i)
					{
						if (i != Slot_Index)
						{
							PTSArenaSlot *pVictim = pArena->Slot(i);
							pTaskExecuting = pVictim->TaskDeque_Pop_Public();
							if (pTaskExecuting != NULL)
							{
								break;
							}
						}
					}

					if (pTaskExecuting != NULL)
					{
						break;
					}

					for (uint32_t i = IndexVictim + 1U; i < CountSlot; ++i)
					{
						if (i != Slot_Index)
						{
							PTSArenaSlot *pVictim = pArena->Slot(i);
							pTaskExecuting = pVictim->TaskDeque_Pop_Public();
							if (pTaskExecuting != NULL)
							{
								break;
							}
						}
					}

					if (pTaskExecuting != NULL)
					{
						break;
					}

					CountSlot = pArena->SlotIndexMaximum_Load_Acquire() + 1U; //Acquire
				}
			}

			++CountFailure;
		}
	}


	//Victim Task Deque
	while (pTaskExecuting)
	{
		//Local Task Deque
		while (pTaskExecuting)
		{
			//ByPass + Recycle
			while (pTaskExecuting)
			{
				assert(pTaskExecuting->m_State == PTSTaskPrefixImpl::Ready);
				pTaskExecuting->m_State = PTSTaskPrefixImpl::Executing;

				//ByPass
				PTSTaskPrefixImpl *pTaskByPass = pTaskExecuting->Execute();

				//Recycle
				switch (pTaskExecuting->m_State)
				{
				case PTSTaskPrefixImpl::Executing://Not Recycle
				{
					assert(pTaskExecuting->m_RefCount == 0U); //Task still has children after it has been executed

					PTSTaskPrefixImpl *pTaskParent = pTaskExecuting->m_Parent;

					//Free
					pTaskExecuting->~PTSTaskPrefixImpl();
					::PTSMemoryAllocator_Free_Aligned(pTaskExecuting);

					//Parent
					if (pTaskParent != NULL)
					{
						uintptr_t RefCountOld = ::PTSAtomic_GetAndAdd(&pTaskParent->m_RefCount, uint32_t(-1));
						if (RefCountOld == 1)
						{
							//pTaskParent->m_State &= ~Debug_RefCount_InUse;

							if (pTaskByPass == NULL)
							{
								//ByPass

								assert(pTaskParent->m_State == PTSTaskPrefixImpl::Allocated); //attempt to spawn task that is not in 'allocated' state
								pTaskParent->m_State = PTSTaskPrefixImpl::Ready;

								pTaskByPass = pTaskParent;
							}
							else
							{
								PTS_Internal_Task_Spawn(pArena, Slot_Index, pTaskParent);
							}
						}
					}
				}
				break;
				case PTSTaskPrefixImpl::Allocated://Recycle As Child Or Continuation
				{
					pTaskExecuting->m_State = PTSTaskPrefixImpl::Ready;
				}
				break;
				default:
					assert(0); //illegal state 
				}

				pTaskExecuting = pTaskByPass;
			}

			//Local Task Deque
			pTaskExecuting = pArena->Slot(Slot_Index)->TaskDeque_Pop_Private();
		}

		//Victim Task Deque
		uint32_t CountSlot = pArena->SlotIndexMaximum_Load_Acquire() + 1U; //Acquire
		int32_t CountFailure = 0;
		int32_t CountYield = 0;

		while (CountFailure < 1500) //#define STEALING_COUNT_WORKER
		{
			uint32_t IndexVictim = RandomTemp.Generate() % CountSlot;
			if (IndexVictim != Slot_Index)
			{
				PTSArenaSlot *pVictim = pArena->Slot(IndexVictim);
				pTaskExecuting = pVictim->TaskDeque_Pop_Public();
				if (pTaskExecuting != NULL)
				{
					break;
				}
			}

			for (int32_t i = 0; i < 1500; i++) //#define STEALING_PAUSE_WORKER
			{
				::PTS_Pause();
			}

			const int32_t CountFailureThreshold = 2 * int32_t(CountSlot);
			if (CountFailure >= CountFailureThreshold)
			{
				::PTS_Yield();
				++CountYield;

				if (CountYield >= 100) //#define YIELD_COUNT_THRESHOLD_MASTER
				{
					for (uint32_t i = 0U; i < IndexVictim; ++i)
					{
						if (i != Slot_Index)
						{
							PTSArenaSlot *pVictim = pArena->Slot(i);
							pTaskExecuting = pVictim->TaskDeque_Pop_Public();
							if (pTaskExecuting != NULL)
							{
								break;
							}
						}
					}

					if (pTaskExecuting != NULL)
					{
						break;
					}

					for (uint32_t i = IndexVictim + 1U; i < CountSlot; ++i)
					{
						if (i != Slot_Index)
						{
							PTSArenaSlot *pVictim = pArena->Slot(i);
							pTaskExecuting = pVictim->TaskDeque_Pop_Public();
							if (pTaskExecuting != NULL)
							{
								break;
							}
						}
					}

					if (pTaskExecuting != NULL)
					{
						break;
					}

					CountSlot = pArena->SlotIndexMaximum_Load_Acquire() + 1U; //Acquire
				}
			}

			++CountFailure;
		}
	}
}

//------------------------------------------------------------------------------------------------------------
static int32_t s_TaskScheduler_Initialize_RefCount = 0;
PTBOOL PTCALL PTSTaskScheduler_Initialize(uint32_t ThreadNumber)
{
	if (::PTSAtomic_GetAndAdd(&s_TaskScheduler_Initialize_RefCount, 1) == 0)
	{
		PTBOOL tbResult;
		tbResult = ::PTSTSD_Create(&s_TaskScheduler_Index, NULL);
		assert(tbResult != PTFALSE);
		
		if (ThreadNumber == 0U)
		{
			ThreadNumber = ::PTS_Info_HardwareThreadNumber() - 1U; //默认值
		}
		s_Market_Singleton_Pointer = new(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSMarket), alignof(PTSMarket)))PTSMarket(ThreadNumber);
		assert(s_Market_Singleton_Pointer != NULL);
		
		return tbResult;
	}
	else
	{
		return PTTRUE;
	}
}

PTBOOL PTCALL PTSTaskScheduler_Uninitialize()
{
	if (::PTSAtomic_GetAndAdd(&s_TaskScheduler_Initialize_RefCount, -1) == 1)
	{
		return PTTRUE;
	}
	else
	{
		return PTTRUE;
	}
}

PTBOOL PTCALL PTSTaskScheduler_Local_Initialize(float fThreadNumberRatio)
{
	assert(::PTSAtomic_Get(&s_TaskScheduler_Initialize_RefCount) > 0);

	PTSTaskSchedulerMasterImpl *pTaskScheduler = static_cast<PTSTaskSchedulerMasterImpl *>(::PTSTSD_GetValue(s_TaskScheduler_Index));

	if (pTaskScheduler == NULL)
	{
		PTSArena *pArena = s_Market_Singleton_Pointer->Arena_Allocate(fThreadNumberRatio);
		assert(pArena != NULL);

		pTaskScheduler = new(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSTaskSchedulerMasterImpl), alignof(PTSTaskSchedulerMasterImpl)))PTSTaskSchedulerMasterImpl(pArena, 0U); //MasterThread的ArenaSlotIndex默认为0
		assert(pTaskScheduler != NULL);

		PTBOOL tbResult = ::PTSTSD_SetValue(s_TaskScheduler_Index, pTaskScheduler);
		assert(tbResult != PTFALSE);

		return tbResult;
	}
	else
	{
		return PTTRUE;
	}
}

PTBOOL PTCALL PTSTaskScheduler_Local_Uninitialize()
{
	assert(::PTSAtomic_Get(&s_TaskScheduler_Initialize_RefCount) > 0);
	return PTTRUE;
}

IPTSTaskScheduler * PTCALL PTSTaskScheduler_Local()
{
	assert(::PTSAtomic_Get(&s_TaskScheduler_Initialize_RefCount) > 0);

	PTSTaskSchedulerMasterImpl *pTaskScheduler = static_cast<PTSTaskSchedulerMasterImpl *>(::PTSTSD_GetValue(s_TaskScheduler_Index));
	assert(pTaskScheduler != NULL);

	return pTaskScheduler;
}

IPTSTaskPrefix * PTCALL PTSTaskScheduler_Task_Prefix(IPTSTask *pTask)
{
	return ::PTS_Internal_Task_Prefix(pTask);
}


#if defined PTWIN32
#include "Win32/PTSTaskSchedulerImpl.inl"
#elif defined PTPOSIX
#include "Posix/PTSMemoryAllocator.inl"
#else
#error 未知的平台
#endif