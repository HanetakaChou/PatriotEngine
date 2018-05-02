//Robert D. Blumofe, Charles E. Leiserson. "Scheduling Multithreaded Computations by Work Stealing". Journal of the ACM 1999. //线程的概念在当时尚未普及 论文中的"Thread"在当今即指"Task" 论文中的"Processor"在当今即指"Thread"
//Jeffrey Richter. ".NET Column: The CLR's Thread Pool". MSDN Magazine June 2003.(https://msdn.microsoft.com/en-us/magazine/msdn-magazine-issues.aspx)
//Alexey Kukanov, Michael J.Voss. "The Foundations for Scalable Multi-core Software in Intel Threading Building Blocks". Intel Technology Journal, Volume 11, Issue 4 2007.

//Andrey Marochko. "Exception Handling and Cancellation in TBB - Part II – Basic use cases". Intel Developer Zone 2008. (https://software.intel.com/en-us/blogs/2008/05/29/exception-handling-and-cancellation-in-tbb-part-ii-basic-use-cases)
//Andrey Marochko. "Exception Handling and Cancellation in TBB - Part III – Use cases with nested parallelism". Intel Developer Zone 2008. (https://software.intel.com/en-us/blogs/2008/06/04/exception-handling-and-cancellation-in-tbb-part-iii-use-cases-with-nested-parallelism)
//Andrey Marochko. "Exception Handling and Cancellation in TBB - Part IV – Using context objects". Intel Developer Zone 2008. (https://software.intel.com/en-us/blogs/2008/06/11/exception-handling-and-cancellation-in-tbb-part-iv-using-context-objects)
//Andrey Marochko. "Exception handling in TBB 2.2 - Getting ready for C++0x". Intel Developer Zone 2009.（https://software.intel.com/en-us/blogs/2009/08/18/exception-handling-in-tbb-22-getting-ready-for-c0x）

//Andrey Marochko. "TBB 3.0 task scheduler improves composability of TBB based solutions. Part 1". Intel Developer Zone 2010. (https://software.intel.com/en-us/blogs/2010/05/13/tbb-30-task-scheduler-improves-composability-of-tbb-based-solutions-part-1)
//Andrey Marochko. "TBB 3.0 task scheduler improves composability of TBB based solutions. Part 2". Intel Developer Zone 2010. (https://software.intel.com/en-us/blogs/2010/05/21/tbb-30-task-scheduler-improves-composability-of-tbb-based-solutions-part-2)
//Andrey Marochko. "TBB scheduler clandestine evolution". Intel Developer Zone 2010. (https://software.intel.com/en-us/blogs/2010/12/09/tbb-scheduler-clandestine-evolution)
//Andrey Marochko. "TBB 3.0, high end many - cores, and Windows processor groups". Intel Developer Zone 2010. (https://software.intel.com/en-us/blogs/2010/12/09/tbb-scheduler-clandestine-evolution)
//Andrey Marochko. "TBB 3.0 and processor affinity". Intel Developer Zone 2010. (https://software.intel.com/en-us/blogs/2010/12/28/tbb-30-and-processor-affinity)
//Andrey Marochko. "Task and task group priorities in TBB". Intel Developer Zone 2011. (https://software.intel.com/en-us/blogs/2011/04/01/task-and-task-group-priorities-in-tbb)

//Andrey Marochko. "TBB initialization, termination, and resource management details, juicy and gory." Intel Developer Zone 2011. (https://software.intel.com/en-us/blogs/2011/04/09/tbb-initialization-termination-and-resource-management-details-juicy-and-gory)


#include "../../Public/System/PTSTaskScheduler.h"
#include <stddef.h>
#include <stdlib.h>

#if defined(PTARM) || defined(PTARM64) || defined(PTX86) || defined(PTX64)
static uint32_t const s_CacheLine_Size = 64U;
#else
#error 未知的架构
#endif

class PTSArena;

//Market（市场）
//单例
//跟踪进程中所有的Arena
//Arena与MasterThread维持一一对应关系

//RML（Resource Management Layer）
//The Thread Pool For Worker Thread
//不断Attach到Market中的Arena执行其中的Task

//在PatriotTBB中 
//Market和RML合并为同一个类PTSMarket
class PTSMarket
{
	//Capacity: 64
	PTSArena **m_ArenaPointerArrayMemory; 
	
	uint32_t m_ArenaPointerArraySize; //只会Push 不会Push 不存在ABA //当Size为0时 空闲 可回收

	//减轻伪共享
	uint8_t __PaddingForPublicFields[s_CacheLine_Size - sizeof(uint32_t) - sizeof(void *)];

	PTSThread *m_ThreadArrayMemory;

	PTSSemaphore m_Semaphore;

	uint32_t const m_ThreadNumber;

	//64
	uint32_t const m_ArenaPointerArrayCapacity; 

	//对齐到CacheLine
	uint8_t __PaddingForPrivateFields[s_CacheLine_Size - sizeof(uint32_t) * 2U - sizeof(PTSSemaphore) - sizeof(void *)];
public:
	inline PTSMarket(uint32_t ThreadNumber);
	
	inline PTSArena *Arena_Allocate_Master(float fThreadNumberRatio);

	inline PTSArena *Arena_Attach_Worker(uint32_t *pSlot_Index);

	inline void Worker_Wake(uint32_t ThreadNumber);

	inline void Worker_Sleep(uint32_t ThreadNumber);

#ifdef PTWIN32
	static inline unsigned __stdcall Worker_Thread_Main(void *pMarketVoid);
#elif defined(PTPOSIX)
	static inline void * Worker_Thread_Main(void *pMarketVoid);
#else
#error 未知的平台
#endif

	static inline bool constexpr StaticAssert()
	{
		return (offsetof(PTSMarket, m_ThreadArrayMemory) == s_CacheLine_Size) && (sizeof(PTSMarket) == s_CacheLine_Size * 2U);
	}
};
static_assert(PTSMarket::StaticAssert(), "PTSMarket: Padding Not Correct");

class PTSArenaSlot;

//Arena（舞台）
//Arena与MasterThread维持一一对应关系
class PTSArena
{
	uint32_t m_SlotIndexAffinityMask;

	//减轻伪共享
	uint8_t __PaddingForPublicFields[s_CacheLine_Size - sizeof(uint32_t)];

	PTSArenaSlot * const m_SlotArrayMemory;
	
	uint32_t const m_SlotArrayCapacity; 

	//对齐到CacheLine
	uint8_t __PaddingForPrivateFields[s_CacheLine_Size - sizeof(uint32_t) - sizeof(void*)];

public:
	//由于SlotIndexMask的约束，SlotArrayCapacity不得大于32
	inline PTSArena(uint32_t Capacity);

	//MasterThread的SlotIndex一定为0
	inline bool Slot_Acquire_Master();

	inline bool Slot_Acquire_Worker(uint32_t *pSlot_Index);

	inline void Slot_Release(uint32_t Slot_Index);

	inline uint32_t SlotIndexMaximum_Load_Acquire();

	inline uint32_t Size_Load_Acquire();

	inline PTSArenaSlot *Slot(uint32_t Index);

	inline uint32_t Capacity();

	static inline bool constexpr StaticAssert()
	{
		return (offsetof(PTSArena, m_SlotArrayMemory) == s_CacheLine_Size) && (sizeof(PTSArena) == s_CacheLine_Size * 2U);
	}
};
static_assert(PTSArena::StaticAssert(), "PTSArena: Padding Not Correct");

class PTSTaskPrefixImpl;

//Arena Slot（舞台槽）
//The Local Task Deque
//MasterThread在Arena对应的Slot的Index一定为0
//WorkerThread在Attach到Arena后会得到自己的Slot，WorkerThread在执行Task时，Spawn产生的Task会被添加到WorkerThread自己的Slot中
class PTSArenaSlot
{
	union
	{
		struct
		{
			uint32_t m_Head;
			uint32_t m_Tail;
		}m_OneWord;
		uint64_t m_TwoWord;
	}m_HeadAndTail;

	uint32_t m_HasBeenAcquired;

	//减轻伪共享
	uint8_t __PaddingForPublicFields[s_CacheLine_Size - sizeof(uint32_t) - sizeof(int64_t)];

	PTSTaskPrefixImpl **m_TaskDequeMemoryS[16]; //64(1)+64(1)+128(2)+256(4)+512(8)

	uint32_t m_TaskDequeCapacity;

	//对齐到CacheLine
	uint8_t __PaddingForPrivateFields[s_CacheLine_Size*3U - sizeof(uint32_t) - sizeof(void*) * 16U];

	friend PTSArena::PTSArena(uint32_t Capacity);
	friend bool PTSArena::Slot_Acquire_Master();
	friend bool PTSArena::Slot_Acquire_Worker(uint32_t *pSlot_Index);
	friend void PTSArena::Slot_Release(uint32_t Slot_Index);
public:
	inline PTSArenaSlot();

	inline void TaskDeque_Push(PTSTaskPrefixImpl *pTaskToPush);

	inline PTSTaskPrefixImpl * TaskDeque_Pop_Private();

	inline PTSTaskPrefixImpl * TaskDeque_Pop_Public();

	static inline bool constexpr StaticAssert()
	{
		return (offsetof(PTSArenaSlot, m_TaskDequeMemoryS[0]) == s_CacheLine_Size) && (sizeof(PTSArenaSlot) == s_CacheLine_Size * 4U);
	}
};
static_assert(PTSArenaSlot::StaticAssert(), "PTSArenaSlot: Padding Not Correct");


//Task
//即"Task"-Based Work-Stealing Scheduler中的"Task"
class PTSTaskPrefixImpl :public IPTSTaskPrefix
{
	IPTSTaskPrefix * Parent() override;
	void ParentSet(IPTSTaskPrefix *pParent) override;

	void Recycle_AsChildOf(IPTSTaskPrefix *pParent) override;

	void RefCount_Set(uint32_t RefCount) override;

public:
	PTSTaskPrefixImpl * m_Parent;

	uint32_t m_RefCount; //Count Of Child

	enum :uint8_t
	{
		Allocated = 0U,
		Ready = 1U,
		Executing = 2U,
		Freed = 4U,
		Debug_RefCount_InUse = 8U
	};

	uint8_t m_State;

	inline PTSTaskPrefixImpl();

	inline PTSTaskPrefixImpl *Execute();
};
static uint32_t const s_TaskPrefix_Alignment = 32U;
static inline PTSTaskPrefixImpl * PTS_Internal_Task_Prefix(IPTSTask *pTask);


//TaskScheduler
//外观（Facade）模式
class PTSTaskSchedulerMasterImpl : public IPTSTaskScheduler
{
	PTSArena *m_pArena;
	//MasterThread的SlotIndex一定为0

	uint32_t m_HasWaked;

	IPTSTask *Task_Allocate(size_t Size, size_t Alignment) override;

	void Task_Spawn(IPTSTask *pTask) override;
	void Task_Spawn_Root_And_Wait(IPTSTask *pTask) override;

	//TBB不允许应用程序显示控制并行
	//Worker_Wake: arena::advertise_new_work->market::adjust_demand->private_server/rml_server::adjust_job_count_estimate
	//Worker_Sleep: arena::is_out_of_work->market::adjust_demand->private_server/rml_server::adjust_job_count_estimate
	void Worker_Wake() override;
	void Worker_Sleep() override;

public:
	inline PTSTaskSchedulerMasterImpl(PTSArena *pArena);
	inline ~PTSTaskSchedulerMasterImpl();
};

//TaskScheduler
//WorkerThread在执行Task时，会通过TLS访问TaskScheduler
//正常情况下，WorkerThread只可能访问TaskScheduler的Task_Allocate和Task_Spawn方法
class PTSTaskSchedulerWorkerImpl : public IPTSTaskScheduler
{
	PTSArena *m_pArena;
	uint32_t m_Slot_Index;

	IPTSTask *Task_Allocate(size_t Size, size_t Alignment) override;

	void Task_Spawn(IPTSTask *pTask) override;
	void Task_Spawn_Root_And_Wait(IPTSTask *pTask) override;

	void Worker_Wake() override;
	void Worker_Sleep() override;

#ifdef PTWIN32
	friend unsigned __stdcall PTSMarket::Worker_Thread_Main(void *pMarketVoid);
#elif defined(PTPOSIX)
	friend void * PTSMarket::Worker_Thread_Main(void *pMarketVoid);
#else
#error 未知的平台
#endif

public:
	inline PTSTaskSchedulerWorkerImpl();
};