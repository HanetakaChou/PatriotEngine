#include "../../Public/McRT/PTSThread.h"
#include "../../Public/McRT/PTSMemoryAllocator.h"
#include "PTSTaskSchedulerImpl.h"
#include <math.h>
#include <new>

//-----------------------------------------------------------------------------------------------------------
//Declaration
//-----------------------------------------------------------------------------------------------------------
static inline uint32_t PTS_Info_HardwareThreadNumber();

static inline bool PTS_Size_IsPowerOfTwo(uint32_t Value);
static inline bool PTS_Size_IsAligned(size_t Value, size_t Alignment);
static inline uint32_t PTS_Size_AlignUpFrom(uint32_t Value, uint32_t Alignment);
static inline uint32_t PTS_Size_BitScanReverse(uint32_t Value);
static inline uint32_t PTS_Size_BitPopCount(uint32_t Value);

static inline void PTS_Internal_Task_Spawn(PTSArena *pArena, uint32_t Slot_Index, PT_McRT_Task_Impl *pTaskToSpawn);
static inline void PTS_Internal_ExecuteAndWait(PTSArena *pArena, uint32_t Slot_Index, PT_McRT_Task_Impl *pTaskRootPrefix, void *pVoidForPredicate, bool (*pFnPredicate)(void *));
static inline void PTS_Internal_ExecuteAndWait_Main(PTSArena *pArena, uint32_t Slot_Index, PT_McRT_Task_Impl *pTaskRootPrefix, void *pVoidForPredicate, bool (*pFnPredicate)(void *));
static inline void PTS_Internal_StealAndExecute_Main(PTSArena *pArena, uint32_t Slot_Index);

static PTSMarket *s_Market_Singleton_Pointer = NULL;
static PTSTSD_KEY s_TaskScheduler_Index;

//-----------------------------------------------------------------------------------------------------------
//Linear Congruential Generator
//-----------------------------------------------------------------------------------------------------------
class PTS_Random_LCG
{
	static const uint32_t c = 12345U;
	static const uint32_t a = 1103515245U;
	uint32_t x;

public:
	inline PTS_Random_LCG(uint32_t Seed) : x(Seed)
	{
	}

	inline uint32_t Generate()
	{
		x = a * x + c;
		return (x << 1U) >> 17U;
	}
};

//-----------------------------------------------------------------------------------------------------------
//PTSMarket
//------------------------------------------
inline PTSMarket::PTSMarket(uint32_t ThreadNumber) : m_ArenaPointerArrayMemory(new (::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSArena) * 64U, alignof(PTSArena))) PTSArena *[64] {}),
													 m_ArenaPointerArraySize(0U),
													 m_HasExited(0U),
													 m_ThreadArrayMemory(new (::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSThread) * ThreadNumber, alignof(PTSThread))) PTSThread[ThreadNumber]{}),
													 m_ThreadNumber(ThreadNumber),
													 m_ArenaPointerArrayCapacity(64U)
{
	bool tbResult = ::PTSSemaphore_Create(0U, &m_Semaphore);
	assert(tbResult != false);

	for (uint32_t i = 0; i < m_ThreadNumber; ++i)
	{
		tbResult = ::PTSThread_Create(Worker_Thread_Main, this, m_ThreadArrayMemory + i);
		assert(tbResult != false);
	}
}

inline PTSArena *PTSMarket::Arena_Allocate_Master(float fThreadNumberRatio)
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
						//WorkerThread尚未Release，并不会影响新MasterThread的Acquire
						//新MasterThread可能会执行前MasterThread遗留的Task（通过Steal WorkerThread的Slot）
						//但是WorkerThread的Slot可能已经不存在Task，正处于Steal的等待循环中
						//一旦新MasterThread Spawn Task，会立即被WorkerThread Steal，相对于WorkerThread Release后再次Acquire更加高效
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
		PTSArena *pArenaNew = new (::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSArena), alignof(PTSArena))) PTSArena(ArenaCapacity);
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

inline PTSArena *PTSMarket::Arena_Attach_Worker(uint32_t *pSlot_Index)
{
	PTSArena *pArenaAttached = NULL;

	if (m_ArenaPointerArraySize != 0U)
	{
		PTS_Random_LCG RandomTemp(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&pArenaAttached)));

		uint32_t IndexRandom = RandomTemp.Generate() % m_ArenaPointerArraySize;

		{
			for (uint32_t i = IndexRandom; i < m_ArenaPointerArraySize; ++i)
			{
				PTSArena *pArena = m_ArenaPointerArrayMemory[i];

				if (pArena != NULL) //数组中指针初始化为NULL
				{
					if (pArena->Size_Load_Acquire() < pArena->Capacity()) //Slot_Release中对SlotArrayMemory::HasBeenAcquired和SlotIndexMask的操作并不是原子的 可能会导致无法充分调度
					{
						if (pArena->Slot_Acquire_Worker(pSlot_Index))
						{
							pArenaAttached = pArena;
							break;
						}
					}
				}
			}
		}

		if (pArenaAttached == NULL)
		{
			for (uint32_t i = 0U; i < IndexRandom; ++i)
			{
				PTSArena *pArena = m_ArenaPointerArrayMemory[i];

				if (pArena != NULL) //数组中指针初始化为NULL
				{
					if (pArena->Size_Load_Acquire() < pArena->Capacity()) //Slot_Release中对SlotArrayMemory::HasBeenAcquired和SlotIndexMask的操作并不是原子的 可能会导致无法充分调度
					{
						if (pArena->Slot_Acquire_Worker(pSlot_Index))
						{
							pArenaAttached = pArena;
							break;
						}
					}
				}
			}
		}
	}

	return pArenaAttached;
}

inline void PTSMarket::Worker_Wake(uint32_t ThreadNumber)
{
	bool tbResult;

	for (uint32_t i = 0; i < ThreadNumber; ++i)
	{
		tbResult = ::PTSSemaphore_Vrijgeven(&m_Semaphore);
		assert(tbResult != false);
	}
}

inline void PTSMarket::Worker_Sleep(uint32_t ThreadNumber)
{
	bool tbResult;

	for (uint32_t i = 0; i < ThreadNumber; ++i)
	{
		assert(s_Market_Singleton_Pointer != NULL);
		tbResult = ::PTSSemaphore_Passern(&s_Market_Singleton_Pointer->m_Semaphore);
		assert(tbResult != false);
	}
}

//-----------------------------------------------------------------------------------------------------------
//PTSArena
//-----------------------------------------------------------------------------------------------------------
inline PTSArena::PTSArena(uint32_t Capacity) : m_SlotIndexAffinityMask(0U),
											   m_SlotArrayMemory(new (::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSArenaSlot) * Capacity, alignof(PTSArenaSlot))) PTSArenaSlot[Capacity]{}),
											   m_SlotArrayCapacity(Capacity)
{
	//由于SlotIndexMask的约束，SlotArrayCapacity不得大于32
	assert(Capacity <= 32U);
	//MasterThread调用本函数时，Arena尚未在到Market中被标记为可用，不存在与WorkerThread并发访问的可能性
	//MasterThread的SlotIndex一定为0
	assert(m_SlotArrayMemory[0].m_HasBeenAcquired == 0U);
	m_SlotArrayMemory[0].m_HasBeenAcquired = 1U;
	assert(m_SlotIndexAffinityMask == 0U);
	m_SlotIndexAffinityMask |= (1U << 0U);
}

inline bool PTSArena::Slot_Acquire_Master()
{
	bool HasAcquired = false;

	//MasterThread的SlotIndex一定为0
	if (::PTSAtomic_CompareAndSet(&m_SlotArrayMemory[0].m_HasBeenAcquired, 0U, 1U) == 0U)
	{
		uint32_t SlotIndexMask = 1U << 0U;
		uint32_t SlotIndexAffinityMaskOld;
		do
		{
			SlotIndexAffinityMaskOld = ::PTSAtomic_Get(&m_SlotIndexAffinityMask);
			//WorkerThread尚未Release，并不会影响新MasterThread的Acquire
			//新MasterThread可能会执行前MasterThread遗留的Task（通过Steal WorkerThread的Slot）
			//但是WorkerThread的Slot可能已经不存在Task，正处于Steal的等待循环中
			//一旦新MasterThread Spawn Task，会立即被WorkerThread Steal，相对于WorkerThread Release后再次Acquire更加高效
			assert((SlotIndexAffinityMaskOld & SlotIndexMask) == 0U);
		} while (::PTSAtomic_CompareAndSet(&m_SlotIndexAffinityMask, SlotIndexAffinityMaskOld, (SlotIndexAffinityMaskOld | SlotIndexMask)) != SlotIndexAffinityMaskOld);

		HasAcquired = true;
	}

	return HasAcquired;
}

inline bool PTSArena::Slot_Acquire_Worker(uint32_t *pSlot_Index)
{
	uint32_t Slot_Index;

	PTS_Random_LCG RandomTemp(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&Slot_Index)));

	bool HasAcquired = false;

	//-----HasBeenAcquired---------------------------------------------

	assert(m_SlotArrayCapacity != 0U);
	//MasterThread的SlotIndex一定为0
	//将0从IndexRandom中的取值范围中排除
	uint32_t IndexRandom = RandomTemp.Generate() % (m_SlotArrayCapacity - 1U) + 1U;
	{
		for (uint32_t i = IndexRandom; i < m_SlotArrayCapacity; ++i)
		{
			if (::PTSAtomic_CompareAndSet(&m_SlotArrayMemory[i].m_HasBeenAcquired, 0U, 1U) == 0U)
			{
				Slot_Index = i;
				HasAcquired = true;
				break;
			}
		}
	}

	if (!HasAcquired)
	{
		//MasterThread的SlotIndex一定为0
		//将0从迭代中排除
		for (uint32_t i = 1U; i < IndexRandom; ++i)
		{
			if (::PTSAtomic_CompareAndSet(&m_SlotArrayMemory[i].m_HasBeenAcquired, 0U, 1U) == 0U)
			{
				Slot_Index = i;
				HasAcquired = true;
				break;
			}
		}
	}

	//-----SlotIndexAffinityMask---------------------------------------------

	if (HasAcquired)
	{
		//由于SlotIndexMask的约束，SlotArrayCapacity不得大于32
		assert(Slot_Index < 32U);

		uint32_t SlotIndexMask = 1U << Slot_Index;
		uint32_t SlotIndexAffinityMaskOld;
		do
		{
			SlotIndexAffinityMaskOld = ::PTSAtomic_Get(&m_SlotIndexAffinityMask);
			assert((SlotIndexAffinityMaskOld & SlotIndexMask) == 0U);
		} while (::PTSAtomic_CompareAndSet(&m_SlotIndexAffinityMask, SlotIndexAffinityMaskOld, (SlotIndexAffinityMaskOld | SlotIndexMask)) != SlotIndexAffinityMaskOld);

		(*pSlot_Index) = Slot_Index;
	}

	return HasAcquired;
}

inline void PTSArena::Slot_Release(uint32_t Slot_Index)
{
	union {
		struct
		{
			uint32_t m_Head;
			uint32_t m_Tail;
		} m_OneWord;
		uint64_t m_TwoWord;
	} HeadAndTailOld;
	HeadAndTailOld.m_TwoWord = ::PTSAtomic_Get(&m_SlotArrayMemory[Slot_Index].m_HeadAndTail.m_TwoWord);
	//Local Task Deque Should Be Empty !!!
	assert(HeadAndTailOld.m_OneWord.m_Head == HeadAndTailOld.m_OneWord.m_Tail);

	uint32_t SlotIndexMask = 1U << Slot_Index;
	uint32_t SlotIndexMaskInverse = ~SlotIndexMask;
	uint32_t SlotIndexAffinityMaskOld;
	do
	{
		SlotIndexAffinityMaskOld = ::PTSAtomic_Get(&m_SlotIndexAffinityMask);
		assert((SlotIndexAffinityMaskOld & SlotIndexMask) != 0U);
	} while (::PTSAtomic_CompareAndSet(&m_SlotIndexAffinityMask, SlotIndexAffinityMaskOld, (SlotIndexAffinityMaskOld & SlotIndexMaskInverse)) != SlotIndexAffinityMaskOld);

	//关于ABA
	//在Acquire时，SlotIndexAffinityMask的更新在HasBeenAcquired之后
	//在Release时，SlotIndexAffinityMask的更新在HasBeenAcquired之前
	//SlotIndexAffinityMask中的Index始终不大于HasBeenAcquired的真实Index，不存在发生死锁的可能性

	uint32_t HasBeenAcquiredOld = ::PTSAtomic_GetAndSet(&m_SlotArrayMemory[Slot_Index].m_HasBeenAcquired, 0U);
	assert(HasBeenAcquiredOld == 1U);
}

inline uint32_t PTSArena::SlotIndexMaximum_Load_Acquire()
{
	return ::PTS_Size_BitScanReverse(::PTSAtomic_Get(&m_SlotIndexAffinityMask));
}

inline uint32_t PTSArena::Size_Load_Acquire()
{
	return ::PTS_Size_BitPopCount(::PTSAtomic_Get(&m_SlotIndexAffinityMask));
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
//PTSArenaSlot
//------------------------------------------------------------------------------------------------------------
inline PTSArenaSlot::PTSArenaSlot() : m_HeadAndTail{{0U, 0U}},
									  m_HasBeenAcquired(0U),
									  m_TaskDequeMemoryS{NULL, NULL, NULL, NULL},
									  m_TaskDequeCapacity(0U)
{
}

inline void PTSArenaSlot::TaskDeque_Push(PT_McRT_Task_Impl *pTaskToPush)
{
	union {
		struct
		{
			uint32_t m_Head;
			uint32_t m_Tail;
		} m_OneWord;
		uint64_t m_TwoWord;
	} HeadAndTailOld;

	HeadAndTailOld.m_TwoWord = ::PTSAtomic_Get(&m_HeadAndTail.m_TwoWord);

	//Allocate
	if ((HeadAndTailOld.m_OneWord.m_Tail - HeadAndTailOld.m_OneWord.m_Head) + 1 >= m_TaskDequeCapacity)
	{
		//64(1) + 64(1) + 128(2) + 256(4) + 512(8)
		//0       1       2 3      4 5 6 7  8 9 10 11 12 13 14 15
		switch (m_TaskDequeCapacity)
		{
		case 0U:
		{
			::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(&m_TaskDequeMemoryS[0]), reinterpret_cast<uintptr_t>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PT_McRT_Task_Impl *) * 64U, s_CacheLine_Size)));
			::PTSAtomic_Set(&m_TaskDequeCapacity, 64U);
		}
		break;
		case 64U:
		{
			::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(&m_TaskDequeMemoryS[1]), reinterpret_cast<uintptr_t>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PT_McRT_Task_Impl *) * 64U, s_CacheLine_Size)));
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
			::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(&m_TaskDequeMemoryS[2]), reinterpret_cast<uintptr_t>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PT_McRT_Task_Impl *) * 64U, s_CacheLine_Size)));
			::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(&m_TaskDequeMemoryS[3]), reinterpret_cast<uintptr_t>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PT_McRT_Task_Impl *) * 64U, s_CacheLine_Size)));
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
		case 256U:
		{
			::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(&m_TaskDequeMemoryS[4]), reinterpret_cast<uintptr_t>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PT_McRT_Task_Impl *) * 64U, s_CacheLine_Size)));
			::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(&m_TaskDequeMemoryS[5]), reinterpret_cast<uintptr_t>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PT_McRT_Task_Impl *) * 64U, s_CacheLine_Size)));
			::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(&m_TaskDequeMemoryS[6]), reinterpret_cast<uintptr_t>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PT_McRT_Task_Impl *) * 64U, s_CacheLine_Size)));
			::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(&m_TaskDequeMemoryS[7]), reinterpret_cast<uintptr_t>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PT_McRT_Task_Impl *) * 64U, s_CacheLine_Size)));
			for (uint32_t i = HeadAndTailOld.m_OneWord.m_Head; i < HeadAndTailOld.m_OneWord.m_Tail; ++i)
			{
				uint32_t indexOld = i % 256U;
				uint32_t indexNew = i % 512U;
				if (indexNew >= 256U)
				{
					::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(&m_TaskDequeMemoryS[indexOld / 64][indexOld % 64]), reinterpret_cast<uintptr_t>(m_TaskDequeMemoryS[indexNew / 64][indexNew % 64]));
				}
			}
			::PTSAtomic_Set(&m_TaskDequeCapacity, 512U);
		}
		break;
		case 512U:
		{
			::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(&m_TaskDequeMemoryS[8]), reinterpret_cast<uintptr_t>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PT_McRT_Task_Impl *) * 64U, s_CacheLine_Size)));
			::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(&m_TaskDequeMemoryS[9]), reinterpret_cast<uintptr_t>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PT_McRT_Task_Impl *) * 64U, s_CacheLine_Size)));
			::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(&m_TaskDequeMemoryS[10]), reinterpret_cast<uintptr_t>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PT_McRT_Task_Impl *) * 64U, s_CacheLine_Size)));
			::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(&m_TaskDequeMemoryS[11]), reinterpret_cast<uintptr_t>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PT_McRT_Task_Impl *) * 64U, s_CacheLine_Size)));
			::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(&m_TaskDequeMemoryS[12]), reinterpret_cast<uintptr_t>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PT_McRT_Task_Impl *) * 64U, s_CacheLine_Size)));
			::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(&m_TaskDequeMemoryS[13]), reinterpret_cast<uintptr_t>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PT_McRT_Task_Impl *) * 64U, s_CacheLine_Size)));
			::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(&m_TaskDequeMemoryS[14]), reinterpret_cast<uintptr_t>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PT_McRT_Task_Impl *) * 64U, s_CacheLine_Size)));
			::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(&m_TaskDequeMemoryS[15]), reinterpret_cast<uintptr_t>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PT_McRT_Task_Impl *) * 64U, s_CacheLine_Size)));
			for (uint32_t i = HeadAndTailOld.m_OneWord.m_Head; i < HeadAndTailOld.m_OneWord.m_Tail; ++i)
			{
				uint32_t indexOld = i % 512U;
				uint32_t indexNew = i % 1024U;
				if (indexNew >= 512U)
				{
					::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(&m_TaskDequeMemoryS[indexOld / 64][indexOld % 64]), reinterpret_cast<uintptr_t>(m_TaskDequeMemoryS[indexNew / 64][indexNew % 64]));
				}
			}
			::PTSAtomic_Set(&m_TaskDequeCapacity, 1024U);
		}
		break;
		default:
			assert(0);
		}
	}

	//Capacity只可能被Owner线程写入
	uint32_t Index = HeadAndTailOld.m_OneWord.m_Tail % m_TaskDequeCapacity;
	m_TaskDequeMemoryS[Index / 64U][Index % 64U] = pTaskToPush;

	//Tail只可能被Owner线程写入
	assert((HeadAndTailOld.m_OneWord.m_Tail + 1U) > 0U); //防止环绕
	::PTSAtomic_Set(&m_HeadAndTail.m_OneWord.m_Tail, HeadAndTailOld.m_OneWord.m_Tail + 1U);
}

inline PT_McRT_Task_Impl *PTSArenaSlot::TaskDeque_Pop_Private()
{
	while (true)
	{
		union {
			struct
			{
				uint32_t m_Head;
				uint32_t m_Tail;
			} m_OneWord;
			uint64_t m_TwoWord;
		} HeadAndTailOld;

		HeadAndTailOld.m_TwoWord = ::PTSAtomic_Get(&m_HeadAndTail.m_TwoWord);

		union {
			struct
			{
				uint32_t m_Head;
				uint32_t m_Tail;
			} m_OneWord;
			uint64_t m_TwoWord;
		} HeadAndTailNew;

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

			if (::PTSAtomic_CompareAndSet(&m_HeadAndTail.m_TwoWord, HeadAndTailOld.m_TwoWord, HeadAndTailNew.m_TwoWord) == HeadAndTailOld.m_TwoWord)
			{
				PT_McRT_Task_Impl *pTaskToPop = m_TaskDequeMemoryS[Index / 64U][Index % 64U];
				return pTaskToPop;
			}
			//else
			//{
			//	continue;
			//}
		}
	}
}

inline PT_McRT_Task_Impl *PTSArenaSlot::TaskDeque_Pop_Public()
{
	while (true)
	{
		union {
			struct
			{
				uint32_t m_Head;
				uint32_t m_Tail;
			} m_OneWord;
			uint64_t m_TwoWord;
		} HeadAndTailOld;

		HeadAndTailOld.m_TwoWord = ::PTSAtomic_Get(&m_HeadAndTail.m_TwoWord);

		if (HeadAndTailOld.m_OneWord.m_Head >= HeadAndTailOld.m_OneWord.m_Tail)
		{
			return NULL;
		}
		else
		{
			union {
				struct
				{
					uint32_t m_Head;
					uint32_t m_Tail;
				} m_OneWord;
				uint64_t m_TwoWord;
			} HeadAndTailNew;

			HeadAndTailNew.m_OneWord.m_Head = HeadAndTailOld.m_OneWord.m_Head + 1U;
			HeadAndTailNew.m_OneWord.m_Tail = HeadAndTailOld.m_OneWord.m_Tail;

			uint32_t Index = HeadAndTailOld.m_OneWord.m_Head % ::PTSAtomic_Get(&m_TaskDequeCapacity);

			if (::PTSAtomic_CompareAndSet(&m_HeadAndTail.m_TwoWord, HeadAndTailOld.m_TwoWord, HeadAndTailNew.m_TwoWord) == HeadAndTailOld.m_TwoWord)
			{
				PT_McRT_Task_Impl *pTaskToPop = m_TaskDequeMemoryS[Index / 64U][Index % 64U];

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
//PT_McRT_Task_Impl
//------------------------------------------------------------------------------------------------------------

inline PT_McRT_Task_Impl::PT_McRT_Task_Impl()
	:
#ifndef NDEBUG
	  m_pTaskInner(PT_McRT_Task_Impl_m_pTaskInner_Undefined),
#endif
	  m_pTaskSuccessor(NULL),
	  m_PredecessorCount(0U)
#ifndef NDEBUG
	  ,
	  m_PredecessorCountMayAtomicAdd(false), m_PredecessorCount_Verification(0U), m_TrackState(Allocated)
#endif
{
}

inline void PT_McRT_Task_Impl::SetInner(PT_McRT_ITask_Inner *pTaskInner)
{
	assert(m_pTaskInner == PT_McRT_Task_Impl_m_pTaskInner_Undefined);
	assert(pTaskInner != NULL);
	m_pTaskInner = pTaskInner;
}

inline void PT_McRT_Task_Impl::SetSuccessor(PT_McRT_Task_Impl *pTaskSuccessor)
{
	assert(m_pTaskSuccessor == NULL); //如果successor不为NULL，那么改变Successor会导致原Successor的Ref_Count改变 //这是TBB所不允许的 //在successor非NULL时，必须先EscapeSuccessor
	m_pTaskSuccessor = pTaskSuccessor;
}

inline void PT_McRT_Task_Impl::EscapeSuccessor(PT_McRT_Task_Impl *pTaskNewChild)
{
	pTaskNewChild->SetSuccessor(m_pTaskSuccessor); //维持successor的ref_count不变 //否则会违反m_PredecessorCountMayAtomicAdd语义
	m_pTaskSuccessor = NULL;
}

inline PT_McRT_Task_Impl *PT_McRT_Task_Impl::PT_McRT_Internal_Alloc_Task(void *pUserData, PT_McRT_ITask_Inner *(*pFn_CreateTaskInnerInstance)(void *pUserData, PT_McRT_ITask *pTaskOuter))
{
	PT_McRT_Task_Impl *pTaskOuter = new (PT_McRT_Aligned_Malloc(sizeof(PT_McRT_Task_Impl), alignof(PT_McRT_Task_Impl))) PT_McRT_Task_Impl();
	PT_McRT_ITask_Inner *pTaskInner = pFn_CreateTaskInnerInstance(pUserData, pTaskOuter);
	assert(pTaskInner != NULL);
	pTaskOuter->SetInner(pTaskInner);
	return pTaskOuter;
}

extern "C" PTMCRTAPI PT_McRT_ITask *PTCALL PT_McRT_ITask_Allocate_Root(void *pUserData, PT_McRT_ITask_Inner *(*pFn_CreateTaskInnerInstance)(void *pUserData, PT_McRT_ITask *pTaskOuter))
{
	PT_McRT_Task_Impl *pTaskNew = PT_McRT_Task_Impl::PT_McRT_Internal_Alloc_Task(pUserData, pFn_CreateTaskInnerInstance);
	pTaskNew->SetSuccessor(NULL);
	return pTaskNew;
}

PT_McRT_ITask *PT_McRT_Task_Impl::Allocate_Child(void *pUserData, PT_McRT_ITask_Inner *(*pFn_CreateTaskInnerInstance)(void *pUserData, PT_McRT_ITask *pTaskOuter))
{
	assert(m_PredecessorCountMayAtomicAdd == false); //显然，接下来会对"this"Task调用Set_Ref_Count修改引用计数

#ifndef NDEBUG
	++m_PredecessorCount_Verification;
#endif

	PT_McRT_Task_Impl *pTaskNew = PT_McRT_Internal_Alloc_Task(pUserData, pFn_CreateTaskInnerInstance);
	pTaskNew->SetSuccessor(this);

	return pTaskNew;
}

PT_McRT_ITask *PT_McRT_Task_Impl::Allocate_Continuation(void *pUserData, PT_McRT_ITask_Inner *(*pFn_CreateTaskInnerInstance)(void *pUserData, PT_McRT_ITask *pTaskOuter))
{
	PT_McRT_Task_Impl *pTaskNew = PT_McRT_Internal_Alloc_Task(pUserData, pFn_CreateTaskInnerInstance);

	//Parent的引用计数维持不变
	this->EscapeSuccessor(pTaskNew);

	return pTaskNew;
}

void PT_McRT_Task_Impl::Set_Ref_Count(uint32_t RefCount)
{
	assert(m_PredecessorCount == 0U);
	assert(m_PredecessorCountMayAtomicAdd == false); //应当在SpawnTask之前SetRefCount
	assert(m_PredecessorCount_Verification == RefCount);

	m_PredecessorCount = RefCount;
}

inline void PT_McRT_Task_Impl::Spawn_PreProcess()
{
#ifndef NDEBUG
	assert(m_PredecessorCount == 0U);	 //只有叶节点才可能被Spawn
	m_PredecessorCount_Verification = 0U; //以原子操作减至0的同步点
	if (m_pTaskSuccessor != NULL)
	{
		m_pTaskSuccessor->m_PredecessorCountMayAtomicAdd = true;
		assert(m_pTaskSuccessor->m_PredecessorCount > 0U); //防止忘记调用Set_Ref_Count
	}								   //在DAG中被执行的部分，且不是叶节点
	assert(m_TrackState == Allocated); //attempt to spawn task that is not in 'allocated' state
	m_TrackState = Ready;
#endif
}

inline void PT_McRT_Task_Impl::Execute_PreProcess()
{
	m_RecycleState = Not_Recycle_T;

#ifndef NDEBUG
	assert(m_TrackState == Ready);
	m_TrackState = Executing;
#endif
}

inline PT_McRT_Task_Impl *PT_McRT_Task_Impl::Execute()
{
	assert(m_RecycleState == Not_Recycle_T);
	assert(m_TrackState == Executing);

	PT_McRT_Task_Impl *pTaskByPass = static_cast<PT_McRT_Task_Impl *>(m_pTaskInner->Execute());
	return pTaskByPass;
}

void PT_McRT_Task_Impl::Recycle_As_Child_Of(PT_McRT_ITask *pParent)
{
	assert(m_PredecessorCount == 0U); //no child tasks allowed when recycled as a child //Otherwise //Race Condition

	assert(m_RecycleState == Not_Recycle_T);
	m_RecycleState = Recycle_As_Child_Of_T;

	assert(m_TrackState == Executing); //Being Executing
#ifndef NDEBUG
	m_TrackState = Allocated;
#endif

	PT_McRT_Task_Impl *pTaskParent = static_cast<PT_McRT_Task_Impl *>(pParent);
	assert(pTaskParent->m_PredecessorCountMayAtomicAdd == false); //显然，接下来会对Parent调用Set_Ref_Count修改引用计数
#ifndef NDEBUG
	++pTaskParent->m_PredecessorCount_Verification;
#endif
	this->SetSuccessor(pTaskParent);
}

void PT_McRT_Task_Impl::Recycle_As_Safe_Continuation()
{
	assert(m_RecycleState == Not_Recycle_T);
	m_RecycleState = Recycle_As_Safe_Continuation_T;

	assert(m_TrackState == Executing); //Being Executing
#ifndef NDEBUG
	m_TrackState = Allocated;
#endif

	assert(m_PredecessorCountMayAtomicAdd == false); //显然，接下来会对自身调用Set_Ref_Count修改引用计数
#ifndef NDEBUG
	++m_PredecessorCount_Verification;
#endif
}

inline PT_McRT_Task_Impl::~PT_McRT_Task_Impl()
{
	m_pTaskInner->Dispose();
}

inline bool PT_McRT_Task_Impl::TestPredecessorCompletion(PT_McRT_Task_Impl *pTaskToTest)
{
	assert(pTaskToTest->m_PredecessorCountMayAtomicAdd == true);

	bool bCompletion;

	if (::PTSAtomic_GetAndAdd(&pTaskToTest->m_PredecessorCount, uint32_t(-1)) == 1U)
	{
#ifndef NDEBUG
		pTaskToTest->m_PredecessorCountMayAtomicAdd = false;
#endif
		bCompletion = true;
	}
	else
	{
		bCompletion = false;
	}

	return bCompletion;
}

inline PT_McRT_Task_Impl *PT_McRT_Task_Impl::FreeAndTestSuccessor()
{
	//Task (Which Not Recycled) still has children after it has been executed
	//往往是错误的Allocate_Child导致
	assert(m_PredecessorCount == 0U);

	assert(m_RecycleState == Not_Recycle_T);

	PT_McRT_Task_Impl *pTaskSuccessor = m_pTaskSuccessor;

	//Free
	this->~PT_McRT_Task_Impl();
	::PT_McRT_Aligned_Free(this);

	PT_McRT_Task_Impl *pTaskToSpawn = NULL;

	if (pTaskSuccessor != NULL)
	{
		if (TestPredecessorCompletion(pTaskSuccessor))
		{
			pTaskToSpawn = pTaskSuccessor;
		}
	}

	return pTaskToSpawn;
}

inline PT_McRT_Task_Impl *PT_McRT_Task_Impl::TestSelf()
{
	assert(m_RecycleState == Recycle_As_Safe_Continuation_T);

	PT_McRT_Task_Impl *pTaskToSpawn = NULL;

	if (TestPredecessorCompletion(this))
	{
		pTaskToSpawn = this;
	}

	return pTaskToSpawn;
}

//------------------------------------------------------------------------------------------------------------
//PTSTaskSchedulerMasterImpl
//------------------------------------------------------------------------------------------------------------
inline PTSTaskSchedulerMasterImpl::PTSTaskSchedulerMasterImpl(PTSArena *pArena)
	: m_pArena(pArena),
	  m_HasWaked(0U)
{
}

inline PTSTaskSchedulerMasterImpl::~PTSTaskSchedulerMasterImpl()
{
	assert(m_pArena != NULL);
	assert(m_HasWaked == 0U);
	//MasterThread的SlotIndex一定为0
	m_pArena->Slot_Release(0U);
}

uint32_t PTSTaskSchedulerMasterImpl::Warp_Size()
{
	return m_pArena->Capacity();
}

uint32_t PTSTaskSchedulerMasterImpl::Warp_ThreadID()
{
	//MasterThread的SlotIndex一定为0
	return 0U;
}

void PTSTaskSchedulerMasterImpl::Task_Spawn(PT_McRT_ITask *pTask)
{
	::PTS_Internal_Task_Spawn(m_pArena, 0U, static_cast<PT_McRT_Task_Impl *>(pTask));
}

void PTSTaskSchedulerMasterImpl::Task_ExecuteAndWait(PT_McRT_ITask *pTask, void *pVoidForPredicate, bool (*pFnPredicate)(void *))
{
	::PTS_Internal_ExecuteAndWait(m_pArena, 0U, static_cast<PT_McRT_Task_Impl *>(pTask), pVoidForPredicate, pFnPredicate);
}

void PTSTaskSchedulerMasterImpl::Worker_Wake()
{
	//TaskScheduler中的方法只可能被唯一的MasterThread串行访问
	assert((m_HasWaked == 0U) ? (m_HasWaked = 1U, 1) : 0);
	s_Market_Singleton_Pointer->Worker_Wake(m_pArena->Capacity());
}

void PTSTaskSchedulerMasterImpl::Worker_Sleep()
{
	//TaskScheduler中的方法只可能被唯一的MasterThread串行访问
	assert((m_HasWaked == 1U) ? (m_HasWaked = 0U, 1) : 0);
	s_Market_Singleton_Pointer->Worker_Sleep(m_pArena->Capacity());
}

//------------------------------------------------------------------------------------------------------------
//PTSTaskSchedulerWorkerImpl
//------------------------------------------------------------------------------------------------------------
inline PTSTaskSchedulerWorkerImpl::PTSTaskSchedulerWorkerImpl()
{
}

uint32_t PTSTaskSchedulerWorkerImpl::Warp_Size()
{
	return m_pArena->Capacity();
}

uint32_t PTSTaskSchedulerWorkerImpl::Warp_ThreadID()
{
	return m_Slot_Index;
}

void PTSTaskSchedulerWorkerImpl::Worker_Wake()
{
	assert(0);
}

void PTSTaskSchedulerWorkerImpl::Worker_Sleep()
{
	assert(0);
}

void PTSTaskSchedulerWorkerImpl::Task_Spawn(PT_McRT_ITask *pTask)
{
	::PTS_Internal_Task_Spawn(m_pArena, m_Slot_Index, static_cast<PT_McRT_Task_Impl *>(pTask));
}

void PTSTaskSchedulerWorkerImpl::Task_ExecuteAndWait(PT_McRT_ITask *pTask, void *pVoidForPredicate, bool (*pFnPredicate)(void *))
{
	::PTS_Internal_ExecuteAndWait(m_pArena, m_Slot_Index, static_cast<PT_McRT_Task_Impl *>(pTask), pVoidForPredicate, pFnPredicate);
}

//------------------------------------------------------------------------------------------------------------
//PTS_Size
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

//------------------------------------------------------------------------------------------------------------
//PTS_Internal
//------------------------------------------------------------------------------------------------------------

static inline void PTS_Internal_Task_Spawn(PTSArena *pArena, uint32_t Slot_Index, PT_McRT_Task_Impl *pTaskToSpawn)
{
	pTaskToSpawn->Spawn_PreProcess();

	PTSArenaSlot *pArenaSlot = pArena->Slot(Slot_Index);
	pArenaSlot->TaskDeque_Push(pTaskToSpawn);
}

static inline void PTS_Internal_ExecuteAndWait(PTSArena *pArena, uint32_t Slot_Index, PT_McRT_Task_Impl *pTaskRootPrefix, void *pVoidForPredicate, bool (*pFnPredicate)(void *))
{
	pTaskRootPrefix->Spawn_PreProcess();

	PTS_Internal_ExecuteAndWait_Main(pArena, Slot_Index, pTaskRootPrefix, pVoidForPredicate, pFnPredicate);
}

static inline void PTS_Internal_ExecuteAndWait_Main(PTSArena *pArena, uint32_t Slot_Index, PT_McRT_Task_Impl *pTaskRootPrefix, void *pVoidForPredicate, bool (*pFnPredicate)(void *))
{
	assert(pTaskRootPrefix != NULL);

	PT_McRT_Task_Impl *pTaskExecuting = pTaskRootPrefix;
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

				pTaskExecuting->m_RecycleState = PT_McRT_Task_Impl::Not_Recycle_T;
				pTaskExecuting->Execute_PreProcess();

				//ByPass
				PT_McRT_Task_Impl *pTaskByPass = pTaskExecuting->Execute();

				//Bypass的语义：在功能上与Spawn相同
				//assert(pTaskByPass->IsLeaf());

				//Spawn: Push Queue
				//Steal循环: Pop Queue
				//ByPass省去Push和Pop的过程，直接执行Task

				//优化：ByPass Parent

				switch (pTaskExecuting->m_RecycleState)
				{
				case PT_McRT_Task_Impl::Not_Recycle_T:
				{
					PT_McRT_Task_Impl *pTaskToSpawn = pTaskExecuting->FreeAndTestSuccessor();

					if (pTaskToSpawn != NULL)
					{
						//Spawn Successor
						if (pTaskByPass == NULL)
						{
							pTaskByPass = pTaskToSpawn;
						}
						else if (pTaskByPass != pTaskToSpawn)
						{
							//在Spawn Parent的情况下，ByPass反而会变慢
							PTS_Internal_Task_Spawn(pArena, Slot_Index, pTaskToSpawn);
						}
					}
				}
				break;
				case PT_McRT_Task_Impl::Recycle_As_Child_Of_T:
				{
					assert(pTaskExecuting->m_RecycleState == PT_McRT_Task_Impl::Recycle_As_Child_Of_T);

					//Recycle要求Parent为NULL //必定先进行Allocate_Continuation
					//而Allocate_Continuation保持Parent的ChildCount不变 //一定不可能发生Spawn_Parent的情况

					//Recycle等同于Allocate //语义上与Spawn无关

					//Recycle_As_Child_Of比Recycle_As_Safe_Continuation更常见，因为可以将同一个Task用于Bypass，有利于提高缓存的命中率 //同一个Task有利于提高程序局部性
				}
				break;
				case PT_McRT_Task_Impl::Recycle_As_Safe_Continuation_T:
				{
					assert(pTaskExecuting->m_RecycleState == PT_McRT_Task_Impl::Recycle_As_Safe_Continuation_T);

					//Recycle_As_Safe_Continuation //Parent的Predecessor个数不变，不可能Spawn_Parent

					PT_McRT_Task_Impl *pTaskToSpawn = pTaskExecuting->TestSelf();

					if (pTaskToSpawn != NULL)
					{
						//Spawn Self
						if (pTaskByPass == NULL)
						{
							pTaskByPass = pTaskToSpawn;
						}
						else if (pTaskByPass != pTaskToSpawn)
						{
							//在Spawn Self的情况下，ByPass反而会变慢
							PTS_Internal_Task_Spawn(pArena, Slot_Index, pTaskToSpawn);
						}
					}
				}
				break;
				default:
					assert(0);
				}

				if (pTaskByPass != NULL)
				{
					pTaskByPass->Spawn_PreProcess(); //Bypass的语义：在功能上与Spawn相同
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

		while (!pFnPredicate(pVoidForPredicate)) //Stop Only When HasBeenFinished
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

static inline void PTS_Internal_StealAndExecute_Main(PTSArena *pArena, uint32_t Slot_Index)
{
	PT_McRT_Task_Impl *pTaskExecuting = NULL;

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
				pTaskExecuting->Execute_PreProcess();

				//ByPass
				PT_McRT_Task_Impl *pTaskByPass = pTaskExecuting->Execute();

				//Bypass的语义：在功能上与Spawn相同
				//assert(pTaskByPass->IsLeaf());

				//Spawn: Push Queue
				//Steal循环: Pop Queue
				//ByPass省去Push和Pop的过程，直接执行Task

				//优化：ByPass Parent / ByPass Recyle

				switch (pTaskExecuting->m_RecycleState)
				{
				case PT_McRT_Task_Impl::Not_Recycle_T:
				{
					PT_McRT_Task_Impl *pTaskToSpawn = pTaskExecuting->FreeAndTestSuccessor();

					if (pTaskToSpawn != NULL)
					{
						//Spawn Successor
						if (pTaskByPass == NULL)
						{
							pTaskByPass = pTaskToSpawn;
						}
						else if (pTaskByPass != pTaskToSpawn)
						{
							//在Spawn Parent的情况下，ByPass反而会变慢
							PTS_Internal_Task_Spawn(pArena, Slot_Index, pTaskToSpawn);
						}
					}
				}
				break;
				case PT_McRT_Task_Impl::Recycle_As_Child_Of_T:
				{
					assert(pTaskExecuting->m_RecycleState == PT_McRT_Task_Impl::Recycle_As_Child_Of_T);

					//Recycle要求Parent为NULL //必定先进行Allocate_Continuation
					//而Allocate_Continuation保持Parent的ChildCount不变 //一定不可能发生Spawn_Parent的情况

					//Recycle等同于Allocate //语义上与Spawn无关

					//Recycle_As_Child_Of比Recycle_As_Safe_Continuation更常见，因为可以将同一个Task用于Bypass，有利于提高缓存的命中率 //同一个Task有利于提高程序局部性
				}
				break;
				case PT_McRT_Task_Impl::Recycle_As_Safe_Continuation_T:
				{
					assert(pTaskExecuting->m_RecycleState == PT_McRT_Task_Impl::Recycle_As_Safe_Continuation_T);

					//Recycle_As_Safe_Continuation //Parent的Predecessor个数不变，不可能Spawn_Parent

					PT_McRT_Task_Impl *pTaskToSpawn = pTaskExecuting->TestSelf();

					if (pTaskToSpawn != NULL)
					{
						//Spawn Self
						if (pTaskByPass == NULL)
						{
							pTaskByPass = pTaskToSpawn;
						}
						else if (pTaskByPass != pTaskToSpawn)
						{
							//在Spawn Self的情况下，ByPass反而会变慢
							PTS_Internal_Task_Spawn(pArena, Slot_Index, pTaskToSpawn);
						}
					}
				}
				break;
				default:
					assert(0);
				}

				if (pTaskByPass != NULL)
				{
					pTaskByPass->Spawn_PreProcess(); //Bypass的语义：在功能上与Spawn相同
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

#ifdef PTWIN32
inline unsigned __stdcall PTSMarket::Worker_Thread_Main(void *pMarketVoid)
#elif defined(PTPOSIX)
inline void *PTSMarket::Worker_Thread_Main(void *pMarketVoid)
#else
#error 未知的平台
#endif
{
	PTSTaskSchedulerWorkerImpl *pTaskScheduler = new (
		::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSTaskSchedulerWorkerImpl), alignof(PTSTaskSchedulerWorkerImpl))) PTSTaskSchedulerWorkerImpl{};
	assert(pTaskScheduler != NULL);

	bool tbResult = ::PTSTSD_SetValue(s_TaskScheduler_Index, pTaskScheduler);
	assert(tbResult != false);

	PTSMarket *pMarket = static_cast<PTSMarket *>(pMarketVoid);

	while (!pMarket->m_HasExited)
	{
		tbResult = ::PTSSemaphore_Passern(&pMarket->m_Semaphore);
		assert(tbResult != false);
		tbResult = ::PTSSemaphore_Vrijgeven(&pMarket->m_Semaphore);
		assert(tbResult != false);

		PTSArena *pArena;
		uint32_t Slot_Index;

		pArena = pMarket->Arena_Attach_Worker(&Slot_Index);

		if (pArena != NULL)
		{
			pTaskScheduler->m_pArena = pArena;
			pTaskScheduler->m_Slot_Index = Slot_Index;

			::PTS_Internal_StealAndExecute_Main(pArena, Slot_Index);

			pArena->Slot_Release(Slot_Index);
		}
	}

	::PTSMemoryAllocator_Free_Aligned(pTaskScheduler);
	::PTSTSD_SetValue(s_TaskScheduler_Index, NULL);

#ifdef PTWIN32
	return 0U;
#elif defined(PTPOSIX)
	return NULL;
#else
#error 未知的平台
#endif
}

//------------------------------------------------------------------------------------------------------------
//PTMCRTAPI
//------------------------------------------------------------------------------------------------------------
static int32_t s_TaskScheduler_Initialize_RefCount = 0;
bool PTCALL PTSTaskScheduler_Initialize(uint32_t ThreadNumber)
{
	if (::PTSAtomic_GetAndAdd(&s_TaskScheduler_Initialize_RefCount, 1) == 0)
	{
		bool tbResult;
		tbResult = ::PTSTSD_Create(
			&s_TaskScheduler_Index,
			[](void *pVoid) -> void {
				//Destructtor
				//WorkThread在退出前已将TSD设置为NULL
				//一定是MasterThread
				PTSTaskSchedulerMasterImpl *pTaskScheduler = static_cast<PTSTaskSchedulerMasterImpl *>(pVoid);
				pTaskScheduler->~PTSTaskSchedulerMasterImpl();
				::PTSMemoryAllocator_Free_Aligned(pVoid);
			});
		assert(tbResult != false);

		assert(s_Market_Singleton_Pointer == NULL);
		if (ThreadNumber == 0U)
		{
			ThreadNumber = ::PTS_Info_HardwareThreadNumber() - 1U; //默认值
		}
		s_Market_Singleton_Pointer = new (::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSMarket), alignof(PTSMarket))) PTSMarket(ThreadNumber);
		assert(s_Market_Singleton_Pointer != NULL);

		return tbResult;
	}
	else
	{
		return true;
	}
}

bool PTCALL PTSTaskScheduler_Initialize_ForThread(float fThreadNumberRatio)
{
	assert(::PTSAtomic_Get(&s_TaskScheduler_Initialize_RefCount) > 0);
	assert(::PTSTSD_GetValue(s_TaskScheduler_Index) == NULL);

	PTSArena *pArena = s_Market_Singleton_Pointer->Arena_Allocate_Master(fThreadNumberRatio);
	assert(pArena != NULL);

	PTSTaskSchedulerMasterImpl *pTaskScheduler = new (
		::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSTaskSchedulerMasterImpl), alignof(PTSTaskSchedulerMasterImpl))) PTSTaskSchedulerMasterImpl(pArena); //MasterThread的SlotIndex一定为0
	assert(pTaskScheduler != NULL);

	bool tbResult = ::PTSTSD_SetValue(s_TaskScheduler_Index, pTaskScheduler);
	assert(tbResult != false);

	return tbResult;
}

IPTSTaskScheduler *PTCALL PTSTaskScheduler_ForThread()
{
	PTSTaskSchedulerMasterImpl *pTaskScheduler = static_cast<PTSTaskSchedulerMasterImpl *>(::PTSTSD_GetValue(s_TaskScheduler_Index));
	assert(pTaskScheduler != NULL);

	return pTaskScheduler;
}

#if defined PTWIN32
#include "Win32/PTSTaskSchedulerImpl.inl"
#elif defined PTPOSIX
#include "Posix/PTSTaskSchedulerImpl.inl"
#else
#error 未知的平台
#endif
