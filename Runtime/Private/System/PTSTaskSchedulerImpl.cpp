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

static inline void PTS_Internal_Spawn(PTSArena *pArena, uint32_t ArenaSlot_Index, PTSTaskPrefixImpl *pTaskToSpawnPrefix);
static inline void PTS_Internal_Master_Main(PTSArena *pArena, uint32_t ArenaSlot_Index, PTSTaskPrefixImpl * pTaskRootPrefix, uint32_t *pHasBeenFinished);

//------------------------------------------
inline PTSMarket::PTSMarket(uint32_t ThreadNumber) :m_ArenaQueueTop(NULL),m_ThreadNumber(ThreadNumber)
{

}

inline PTSArena *PTSMarket::Arena_Acquire(float fThreadNumberRatio)
{
	uint32_t Capacity = static_cast<uint32_t>(::ceilf(static_cast<float>(m_ThreadNumber) * fThreadNumberRatio));

	PTSArena *pArenaToAcquire = NULL;

	//Recycle
	{
		PTSArena *pArenaQueueIterator = m_ArenaQueueTop;
		while (pArenaQueueIterator)
		{
			if (pArenaQueueIterator->m_Capacity == Capacity) //实际中Capacity相等的可能性较大
			{
				if (::PTSAtomic_CompareAndSet(&pArenaQueueIterator->m_Size, 0U, 1U) == 0U)
				{
					pArenaQueueIterator->m_ArenaSlotMemory[0].m_HasBeenAcquired = 1U; //MasterThread的ArenaSlotIndex默认为0
					pArenaToAcquire = pArenaQueueIterator;
					break;
				}
			}
		}
	}

	//New
	if (pArenaToAcquire == NULL)
	{
		PTSArena *newArenaPtr = new(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSArena), alignof(PTSArena)))PTSArena(Capacity); //MasterThread的ArenaSlotIndex默认为0
		assert(newArenaPtr != NULL);

		//
		++newArenaPtr->m_Size;
		newArenaPtr->m_ArenaSlotMemory[0].m_HasBeenAcquired = 1U; //MasterThread的ArenaSlotIndex默认为0

		PTSArena *oldArenaPtr;

		//只会Push 不会Push 不存在ABA //当Size为0时 空闲 可回收
		do {
			oldArenaPtr = m_ArenaQueueTop;
			newArenaPtr->m_Market_Next = oldArenaPtr;
		} while (::PTSAtomic_CompareAndSet(reinterpret_cast<uintptr_t *>(&m_ArenaQueueTop), reinterpret_cast<uintptr_t>(oldArenaPtr), reinterpret_cast<uintptr_t>(newArenaPtr)) != reinterpret_cast<uintptr_t>(oldArenaPtr));

		pArenaToAcquire = newArenaPtr;
	}

	return pArenaToAcquire;
}

//-----------------------------------------------------------------------------------------------------------
inline PTSArena::PTSArena(uint32_t Capacity) :
	m_Market_Next(NULL),
	m_Size(0U),
	m_ArenaSlotMemory(new(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSArenaSlot)*Capacity, alignof(PTSArenaSlot)))PTSArenaSlot[Capacity]),
	m_Capacity(Capacity)
{
	
}

inline PTSArenaSlot *PTSArena::ArenaSlot(uint32_t Index)
{
	return m_ArenaSlotMemory + Index;
}

inline uint32_t PTSArena::Size_Acquire()
{
	return ::PTSAtomic_Get(&m_Size);
}

//------------------------------------------------------------------------------------------------------------
inline PTSArenaSlot::PTSArenaSlot() :m_HeadAndTail{ 0U,0U }, m_HasBeenAcquired(0U), m_TaskDequeMemoryS{ NULL,NULL,NULL,NULL }, m_Capacity(0U)
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
	if ((HeadAndTailOld.m_OneWord.m_Tail - HeadAndTailOld.m_OneWord.m_Head) + 1 >= m_Capacity)
	{
		//64(1) + 64(1) + 128(2)
		//0       1       2 3   
		switch (m_Capacity)
		{
		case 0U:
		{
			::PTSAtomic_Set(reinterpret_cast<uintptr_t *>(&m_TaskDequeMemoryS[0]), reinterpret_cast<uintptr_t>(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSTaskPrefixImpl *) * 64U, s_CacheLine_Size)));
			::PTSAtomic_Set(&m_Capacity, 64U);
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
			::PTSAtomic_Set(&m_Capacity, 128U);
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
			::PTSAtomic_Set(&m_Capacity, 256U);
		}
		break;
		default:
			assert(0);
		}
	}

	//__TBB_ASSERT(t->state() == task::allocated, "attempt to spawn task that is not in 'allocated' state");
	//t->prefix().state = task::ready;

	//Capacity只可能被Owner线程写入
	uint32_t Index = HeadAndTailOld.m_OneWord.m_Tail % m_Capacity;
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

			uint32_t Index = HeadAndTailNew.m_OneWord.m_Tail % m_Capacity;
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

			uint32_t Index = HeadAndTailOld.m_OneWord.m_Head % ::PTSAtomic_Get(&m_Capacity);
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

static inline PTSTaskPrefixImpl * PTS_Internal_Task_Prefix(void *pTask)
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
	assert(m_Parent->m_State != Freed); //parent already freed

	m_State = Allocated;
	m_Parent = static_cast<PTSTaskPrefixImpl *>(pParent);
}

void PTSTaskPrefixImpl::RefCount_Set(uint32_t RefCount)
{
	assert((m_State&Debug_RefCount_InUse) == 0); //应当在SpawnTask之前SetRefCount
	m_RefCount = RefCount;
}

void PTSTaskPrefixImpl::OverrideExecute(void *(*pFn_Execute)(void *pTaskThisVoid))
{
	m_pFn_Execute = pFn_Execute;
}

inline PTSTaskPrefixImpl *PTSTaskPrefixImpl::Execute()
{
	assert(m_pFn_Execute != NULL);
	void *pTask = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(this) + s_TaskPrefix_Reservation_Size);
	void *pTaskByPass = m_pFn_Execute(pTask);
	return (pTaskByPass != NULL) ? (::PTS_Internal_Task_Prefix(pTaskByPass)) : NULL;
}



//------------------------------------------------------------------------------------------------------------
inline PTSTaskSchedulerImpl::PTSTaskSchedulerImpl(PTSArena *pArena, uint32_t ArenaSlot_Index) :m_pArena(pArena), m_ArenaSlot_Index(ArenaSlot_Index)
{

}

void PTSTaskSchedulerImpl::Worker_Wake()
{

}

void PTSTaskSchedulerImpl::Worker_Sleep()
{

}

void *PTSTaskSchedulerImpl::Task_Allocate(size_t SizeTask, size_t AlignmentTask)
{	
	IPTSTaskPrefix *pTaskPrefix;
	void *pTask;

	assert(::PTS_Size_IsPowerOfTwo(static_cast<uint32_t>(AlignmentTask)));
	assert(::PTS_Size_IsAligned(s_TaskPrefix_Alignment, AlignmentTask));


	pTaskPrefix = ::new(::PTSMemoryAllocator_Alloc_Aligned(s_TaskPrefix_Reservation_Size + static_cast<uint32_t>(SizeTask), s_TaskPrefix_Alignment))PTSTaskPrefixImpl{};
	assert(pTaskPrefix != NULL);

	pTask = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(pTaskPrefix) + s_TaskPrefix_Reservation_Size);
	
	
	assert(::PTS_Size_IsAligned(reinterpret_cast<uintptr_t>(pTaskPrefix), s_TaskPrefix_Alignment));
	assert(::PTS_Size_IsAligned(reinterpret_cast<uintptr_t>(pTask), AlignmentTask));
	
	return pTask;
}

void PTSTaskSchedulerImpl::Task_Spawn(void *pTask)
{
	PTS_Internal_Spawn(m_pArena, m_ArenaSlot_Index, ::PTS_Internal_Task_Prefix(pTask));
}


void PTSTaskSchedulerImpl::Task_Spawn_Root_And_Wait(void *pTaskRoot)
{
	assert(pTaskRoot != NULL);

	uint32_t HasBeenFinished = 0U;

	class PTSTaskWait
	{
		uint32_t *m_pHasBeenFinished;
	public:
		inline PTSTaskWait(uint32_t *pHasBeenFinished) :m_pHasBeenFinished(pHasBeenFinished)
		{

		}

		static void * Execute(void *pTaskThisVoid)
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

	assert(pTaskRootPrefix->m_pFn_Execute != NULL);
	::PTS_Internal_Master_Main(m_pArena, m_ArenaSlot_Index, pTaskRootPrefix, &HasBeenFinished);
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
static inline void PTS_Internal_Spawn(PTSArena *pArena, uint32_t ArenaSlot_Index, PTSTaskPrefixImpl *pTaskToSpawnPrefix)
{
	assert(pTaskToSpawnPrefix->m_State == PTSTaskPrefixImpl::Allocated); //attempt to spawn task that is not in 'allocated' state
	pTaskToSpawnPrefix->m_State = PTSTaskPrefixImpl::Ready;

	//Debug
	pTaskToSpawnPrefix->m_State |= PTSTaskPrefixImpl::Debug_RefCount_InUse;

	PTSArenaSlot *pArenaSlot = pArena->ArenaSlot(ArenaSlot_Index);
	pArenaSlot->TaskDeque_Push(pTaskToSpawnPrefix);
}

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

static inline void PTS_Internal_Master_Main(PTSArena *pArena, uint32_t ArenaSlot_Index, PTSTaskPrefixImpl *pTaskRootPrefix, uint32_t *pHasBeenFinished)
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
				assert(pTaskExecuting->m_State == PTSTaskPrefixImpl::Allocated);
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
								pTaskByPass = pTaskParent;
							}
							else
							{
								PTS_Internal_Spawn(pArena, ArenaSlot_Index, pTaskParent);
							}
						}
					}
				}
				break;
				case PTSTaskPrefixImpl::Allocated://Recycle As Child Or Continuation
				{
					//Not Free
				}
				break;
				default:
					assert(0); //illegal state 
				}

				pTaskExecuting = pTaskByPass;
			}

			//Local Task Deque
			pTaskExecuting = pArena->ArenaSlot(ArenaSlot_Index)->TaskDeque_Pop_Private();
		}

		//Victim Task Deque
		uint32_t CountSlot = pArena->Size_Acquire(); //Acquire
		int32_t CountFailure = 0;
		int32_t CountYield = 0;

		while (!::PTSAtomic_Get(pHasBeenFinished)) //Stop Only When HasBeenFinished
		{
			uint32_t IndexVictim = RandomTemp.Generate() % CountSlot;
			if (IndexVictim != ArenaSlot_Index)
			{
				PTSArenaSlot *pVictim = pArena->ArenaSlot(IndexVictim);
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
						if (i != ArenaSlot_Index)
						{
							PTSArenaSlot *pVictim = pArena->ArenaSlot(i);
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
						if (i != ArenaSlot_Index)
						{
							PTSArenaSlot *pVictim = pArena->ArenaSlot(i);
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

					CountSlot = pArena->Size_Acquire(); //Acquire
				}
			}

			++CountFailure;
		}
	}

}

//------------------------------------------------------------------------------------------------------------
static PTS_RML_Server s_RML_Server_Singleton;

static PTSMarket *s_Market_Singleton_Pointer = NULL;

static PTSTSD_KEY s_TaskScheduler_Index;

static int32_t s_TaskScheduler_Initialize_RefCount = 0;
PTBOOL PTCALL PTSTaskScheduler_Initialize(uint32_t ThreadNumber)
{
	if (::PTSAtomic_GetAndAdd(&s_TaskScheduler_Initialize_RefCount, 1) == 0)
	{
		if (ThreadNumber == 0U)
		{
			//默认值
			ThreadNumber = ::PTS_Info_HardwareThreadNumber() - 1U;
		}
		s_Market_Singleton_Pointer = new(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSMarket), alignof(PTSMarket)))PTSMarket(ThreadNumber);
		assert(s_Market_Singleton_Pointer != NULL);

		PTBOOL tbResult;
		tbResult = ::PTSTSD_Create(&s_TaskScheduler_Index, NULL);
		assert(tbResult != PTFALSE);
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

	PTSTaskSchedulerImpl *pTaskScheduler = static_cast<PTSTaskSchedulerImpl *>(::PTSTSD_GetValue(s_TaskScheduler_Index));

	if (pTaskScheduler == NULL)
	{
		PTSArena *pArena = s_Market_Singleton_Pointer->Arena_Acquire(fThreadNumberRatio);
		assert(pArena != NULL);

		pTaskScheduler = new(::PTSMemoryAllocator_Alloc_Aligned(sizeof(PTSTaskSchedulerImpl), alignof(PTSTaskSchedulerImpl)))PTSTaskSchedulerImpl(pArena, 0U); //MasterThread的ArenaSlotIndex默认为0
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

	PTSTaskSchedulerImpl *pTaskScheduler = static_cast<PTSTaskSchedulerImpl *>(::PTSTSD_GetValue(s_TaskScheduler_Index));
	assert(pTaskScheduler != NULL);

	return pTaskScheduler;
}

IPTSTaskPrefix * PTCALL PTSTaskScheduler_Task_Prefix(void *pTask)
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

#if 0

extern "C" PTSYSTEMAPI IPTSTaskScheduler * PTCALL PTSTaskScheduler_Construct(PTSTaskSchedulerStorage *pStorage)
{
	//Construct vptr
	return ::new(pStorage)PTSTaskSchedulerImpl();
}

inline PTSTaskSchedulerImpl::~PTSTaskSchedulerImpl()
{

}

void PTSTaskSchedulerImpl::Destruct()
{
	this->~PTSTaskSchedulerImpl();
}

static char _PadTest[32];

void PTSTaskSchedulerImpl::Task_Allocate( //IPTTask *pParent, //SetParent 
	size_t ui_StorageCustom_Size,
	size_t ui_StorageCustom_Align,
	void *pVoid_StorageCustom_ConstructorArgument,
	void(*pFn_StorageCustom_Constructor)(
		void *pVoid_StorageCustom,
		void *pVoid_StorageCustom_ConstructorArgument
		),
	void(*pFn_Execute)(
		void *pVoid_StorageCustom,
		IPTSTask *pThis,
		IPTSTask **const ppNextToExecute //Scheduler Bypass
		)
)
{
	/* PTSTaskImpl *pTaskImpl */ void *pVoid_StorageCustom = /* sizeof(PTSTaskImpl) + */ _PadTest; //Memory(比如Heap) Alloc
	pFn_StorageCustom_Constructor(_PadTest, pVoid_StorageCustom_ConstructorArgument); //Constructor

	//Execute
	//In the Scheduler
	IPTSTask *pNextToExecute = NULL; //Default NULL
	pFn_Execute(pVoid_StorageCustom, /* *pTaskImpl */ NULL, &pNextToExecute);
}


void PTSTaskSchedulerImpl::SpawnRootAndWait(IPTSTask *pFirst, IPTSTask *pNext)
{

}

#endif
