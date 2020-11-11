#ifndef PT_SYSTEM_TASKSCHEDULER_H
#define PT_SYSTEM_TASKSCHEDULER_H

#include <stddef.h>
#include <stdint.h>
#include "PTSThread.h"
#include <assert.h>

//COM Aggregation
//https://docs.microsoft.com/en-us/windows/win32/com/aggregation

//Task_Inner provides two implementations of the "Dispose" method: delegating and non-delegating.
//The delegating implementation delegates the "Dispose" method to Task_Outer.
//The non-delegating implementation is the "normal" implementation which can be used to destory Task_Inner.

//The non-delegating implementation can only be obtained when Task_Inner is created.
//Task_Outer is the only holder of the non-delegating implementation and is responsible for releasing Task_Inner.

struct PT_McRT_ITask; //IPT_McRT_Task_Outer
struct PT_McRT_ITask_Inner;

struct PT_McRT_ITask
{
	//virtual void Dispose() = 0; //只可能在TaskScheduler内部用到
	//virtual PT_McRT_ITask_Inner *QueryInterface() = 0; //只可能在TaskScheduler内部用到
	static inline PT_McRT_ITask *Allocate_Root(void *pUserData, PT_McRT_ITask_Inner *(*pFn_CreateTaskInnerInstance)(void *pUserData, PT_McRT_ITask *pTaskOuter));
	virtual PT_McRT_ITask *Allocate_Child(void *pUserData, PT_McRT_ITask_Inner *(*pFn_CreateTaskInnerInstance)(void *pUserData, PT_McRT_ITask *pTaskOuter)) = 0;
	virtual PT_McRT_ITask *Allocate_Continuation(void *pUserData, PT_McRT_ITask_Inner *(*pFn_CreateTaskInnerInstance)(void *pUserData, PT_McRT_ITask *pTaskOuter)) = 0;

	virtual void Recycle_As_Child_Of(PT_McRT_ITask *pParent) = 0;
	virtual void Recycle_As_Safe_Continuation() = 0;

	//应当在SpawnTask之前SetRefCount
	virtual void Set_Ref_Count(uint32_t RefCount) = 0; //Predecessor Count
};

struct PT_McRT_ITask_Inner
{
	virtual void Dispose() = 0;
	//virtual PT_McRT_ITask *QueryInterface() = 0; //只可能在Execute的实现中被用到，直接通过m_pTaskOuter访问

	virtual PT_McRT_ITask *Execute() = 0;
};

extern "C" PTMCRTAPI PT_McRT_ITask *PTCALL PT_McRT_ITask_Allocate_Root(void *pUserData, PT_McRT_ITask_Inner *(*pFn_CreateTaskInnerInstance)(void *pUserData, PT_McRT_ITask *pTaskOuter));

inline PT_McRT_ITask *PT_McRT_ITask::Allocate_Root(void *pUserData, PT_McRT_ITask_Inner *(*pFn_CreateTaskInnerInstance)(void *pUserData, PT_McRT_ITask *pTaskOuter))
{
	return PT_McRT_ITask_Allocate_Root(pUserData, pFn_CreateTaskInnerInstance);
}

struct IPTSTaskScheduler;

struct IPTSTaskScheduler
{
	//模拟GPGPU中的语义
	//与TSD（ThreadSpecificData）的语义略有不同
	//并发执行的Thread对应的ID一定不同
	//但是ID可以被复用，同一ID并不一定始终被同一Thread使用
	//目前的实现中，MasterThread的ID一定为0，但是并不鼓励应用程序依赖于此特定的实现细节
	virtual uint32_t Warp_Size() = 0;
	virtual uint32_t Warp_ThreadID() = 0;

	//TBB不允许应用程序显式控制并行
	virtual void Worker_Wake() = 0;
	virtual void Worker_Sleep() = 0;

	virtual void Task_Spawn(PT_McRT_ITask *pTask) = 0;
	virtual void Task_ExecuteAndWait(PT_McRT_ITask *pTask, void *pVoidForPredicate, bool (*pFnPredicate)(void *)) = 0;

#if 0
	inline void Task_WaitRoot(IPTSTask *pTaskRoot)
	{
		assert(pTaskRoot != NULL);

		uint32_t HasBeenFinished = 0U;

		//Task Wait
		{
			class PTSTaskWait :public IPTSTask
			{
				uint32_t *m_pHasBeenFinished;
			public:
				inline PTSTaskWait(uint32_t *pHasBeenFinished) :m_pHasBeenFinished(pHasBeenFinished)
				{

				}

				IPTSTask * Execute() override
				{
					::PTSAtomic_Set(m_pHasBeenFinished, 1U);
					return NULL;
				}
			};

			IPTSTask *pTaskWait = NULL;

			pTaskWait = new(IPTSTask::Allocate_Root(pTaskWait, this))PTSTaskWait(&HasBeenFinished);

			//assert(pTaskRootPrefix->m_Parent == NULL); //Root
			pTaskRoot->ParentSet(pTaskWait);
			pTaskWait->RefCount_Set(1U);
		}

		this->Task_ExecuteAndWait(pTaskRoot, &HasBeenFinished, [](void *pVoidForPredicate)->bool {return ::PTSAtomic_Get(static_cast<uint32_t *>(pVoidForPredicate)); });
	}
#endif
};

extern "C" PTMCRTAPI bool PTCALL PTSTaskScheduler_Initialize(uint32_t ThreadNumber = 0U);
extern "C" PTMCRTAPI bool PTCALL PTSTaskScheduler_Initialize_ForThread(float fThreadNumberRatio = 1.0f);
extern "C" PTMCRTAPI IPTSTaskScheduler *PTCALL PTSTaskScheduler_ForThread();

//Parallel Programming Pattern

#include <new>

#if 0
//James Reinders,Arch Robison,Michael McCool. "Recursive Implementation Of Map". Structured Parallel Programming: Patterns for Efficient Computation, Chapter 8.3, 2012.
namespace __PTSInternal_Parallel_Map
{
	class PTSTask_Map_Continuation :public IPTSTask
	{
	public:
		inline PTSTask_Map_Continuation()
		{

		}

		IPTSTask *Execute() override
		{
			//Conquer
			return NULL;
		}
	};

	template<typename PTSTYPE_Lambda_Serial_Map>
	class PTSTask_Map :public IPTSTask
	{
		uint32_t const m_Threshold;
		uint32_t m_Begin;
		uint32_t m_End;
		PTSTYPE_Lambda_Serial_Map const &m_rSerialMap;

	public:
		inline PTSTask_Map(
			uint32_t Threshold,
			uint32_t Begin,
			uint32_t End,
			PTSTYPE_Lambda_Serial_Map const &rSerialMap
		)
			:
			m_Threshold(Threshold),
			m_Begin(Begin),
			m_End(End),
			m_rSerialMap(rSerialMap)
		{

		}

		IPTSTask *Execute() override
		{
			IPTSTaskScheduler *pTaskScheduler = ::PTSTaskScheduler_ForThread();//Master And Worker Divergence

			assert(this->m_Threshold > 0);
			if ((this->m_End - this->m_Begin) > this->m_Threshold) //Recursive
			{
				//Divide
				uint32_t Middle = this->m_Begin + (this->m_End - this->m_Begin) / 2U;

				//Continuation Passing Style
				PTSTask_Map_Continuation *pTaskContinuation = NULL;
				pTaskContinuation = new(
					this->Allocate_Continuation(pTaskContinuation, pTaskScheduler)
					) PTSTask_Map_Continuation{};

				PTSTask_Map<PTSTYPE_Lambda_Serial_Map> *pTaskChildRight = NULL;
				pTaskChildRight = new(
					pTaskContinuation->Allocate_Child(pTaskChildRight, pTaskScheduler)
					)PTSTask_Map<PTSTYPE_Lambda_Serial_Map>(
						this->m_Threshold, 
						Middle,
						this->m_End, 
						this->m_rSerialMap);

				//Recycle
				this->Recycle_AsChildOf(pTaskContinuation);
				this->m_End = Middle;

				pTaskContinuation->RefCount_Set(2);

				pTaskScheduler->Task_Spawn(pTaskChildRight);

				//Scheduler ByPass
				return this;
			}
			else //Base Case
			{
				m_rSerialMap(this->m_Begin, this->m_End);
				return NULL;
			}
		}
	};
}

template<typename PTSTYPE_Lambda_Serial_Map>
inline void PTSParallel_Map(
	uint32_t Threshold,
	uint32_t Begin,
	uint32_t End,
	PTSTYPE_Lambda_Serial_Map const &rSerialMap
)
{
	IPTSTaskScheduler *pTaskScheduler = ::PTSTaskScheduler_ForThread();

	__PTSInternal_Parallel_Map::PTSTask_Map<PTSTYPE_Lambda_Serial_Map> *pTaskMap = NULL;
	pTaskMap = new(
		IPTSTask::Allocate_Root(pTaskMap, pTaskScheduler)
		)__PTSInternal_Parallel_Map::PTSTask_Map<PTSTYPE_Lambda_Serial_Map>(Threshold, Begin, End, rSerialMap);

	pTaskScheduler->Task_WaitRoot(pTaskMap);
}
#endif

#if 0
//James Reinders,Arch Robison,Michael McCool. "Reductions And HypeObjects". Structured Parallel Programming: Patterns for Efficient Computation, Chapter 8.10, 2012.
namespace __PTSInternal_Parallel_Reduce
{
	template<typename PTSTYPE_Value, typename PTSTYPE_Lambda_Function_Combine>
	class PTSTask_Reduce_Continuation :public IPTSTask
	{
		PTSTYPE_Value *const m_pResultRoot;
		PTSTYPE_Lambda_Function_Combine const &m_rFunctionCombine;
	public:
		PTSTYPE_Value m_ResultChildLeft;
		PTSTYPE_Value m_ResultChildRight;

		inline PTSTask_Reduce_Continuation(
			PTSTYPE_Value *pResultRoot,
			PTSTYPE_Lambda_Function_Combine const &rFunctionCombine)
			:
			m_pResultRoot(pResultRoot),
			m_rFunctionCombine(rFunctionCombine)
		{

		}

		IPTSTask *Execute() override
		{
			//Conquer
			(*this->m_pResultRoot) = m_rFunctionCombine(m_ResultChildLeft, m_ResultChildRight);
			return NULL;
		}
	};

	template<typename PTSTYPE_Value, typename PTSTYPE_Lambda_Serial_Reduce, typename PTSTYPE_Lambda_Function_Combine>
	class PTSTask_Reduce :public IPTSTask
	{
		uint32_t const m_Threshold;
		uint32_t m_Begin;
		uint32_t m_End;
		PTSTYPE_Value *m_pResultRoot;
		PTSTYPE_Lambda_Serial_Reduce const &m_rSerialReduce;
		PTSTYPE_Lambda_Function_Combine  const &m_rFunctionCombine;

	public:
		inline PTSTask_Reduce(
			uint32_t Threshold,
			uint32_t Begin,
			uint32_t End,
			PTSTYPE_Value *pResultRoot,
			PTSTYPE_Lambda_Serial_Reduce const &rSerialReduce,
			PTSTYPE_Lambda_Function_Combine const &rFunctionCombine
		)
			:
			m_Threshold(Threshold),
			m_Begin(Begin),
			m_End(End),
			m_pResultRoot(pResultRoot),
			m_rSerialReduce(rSerialReduce),
			m_rFunctionCombine(rFunctionCombine)
		{

		}

		IPTSTask *Execute() override
		{
			IPTSTaskScheduler *pTaskScheduler = ::PTSTaskScheduler_ForThread();

			assert(this->m_Threshold > 0);
			if ((this->m_End - this->m_Begin) > this->m_Threshold) //Recursive
			{
				//Divide
				uint32_t Middle = this->m_Begin + (this->m_End - this->m_Begin) / 2U;

				//Continuation Passing Style
				PTSTask_Reduce_Continuation<PTSTYPE_Value, PTSTYPE_Lambda_Function_Combine> *pTaskContinuation = NULL;
				pTaskContinuation = new(
					this->Allocate_Continuation(pTaskContinuation, pTaskScheduler)
					) PTSTask_Reduce_Continuation<PTSTYPE_Value, PTSTYPE_Lambda_Function_Combine>(
						this->m_pResultRoot,
						this->m_rFunctionCombine
						);

				PTSTask_Reduce<PTSTYPE_Value, PTSTYPE_Lambda_Serial_Reduce, PTSTYPE_Lambda_Function_Combine> *pTaskChildRight = NULL;
				pTaskChildRight = new(
					pTaskContinuation->Allocate_Child(pTaskChildRight, pTaskScheduler)
					)PTSTask_Reduce<PTSTYPE_Value, PTSTYPE_Lambda_Serial_Reduce, PTSTYPE_Lambda_Function_Combine>(
						this->m_Threshold,
						Middle, 
						this->m_End,
						&pTaskContinuation->m_ResultChildRight,
						this->m_rSerialReduce,
						this->m_rFunctionCombine
					);

				//Recycle
				this->Recycle_AsChildOf(pTaskContinuation);
				this->m_End = Middle;
				this->m_pResultRoot = &pTaskContinuation->m_ResultChildLeft;

				pTaskContinuation->RefCount_Set(2);

				pTaskScheduler->Task_Spawn(pTaskChildRight);

				//Scheduler ByPass
				return this;
			}
			else //Base Case
			{
				(*this->m_pResultRoot) = m_rSerialReduce(this->m_Begin, this->m_End);
				return NULL;
			}
		}
	};
}

template<typename PTSTYPE_Value, typename PTSTYPE_Lambda_Serial_Reduce, typename PTSTYPE_Lambda_Function_Combine>
inline void PTSParallel_Reduce(
	uint32_t Threshold,
	uint32_t Begin,
	uint32_t End,
	PTSTYPE_Value &rResult, //Initialize -> Identity
	PTSTYPE_Lambda_Serial_Reduce const &rSerialReduce,
	PTSTYPE_Lambda_Function_Combine const &rFunctionCombine
)
{
	IPTSTaskScheduler *pTaskScheduler = ::PTSTaskScheduler_ForThread();

	__PTSInternal_Parallel_Reduce::PTSTask_Reduce<PTSTYPE_Value, PTSTYPE_Lambda_Serial_Reduce, PTSTYPE_Lambda_Function_Combine> *pTaskReduce = NULL;
	pTaskReduce = new(
		IPTSTask::Allocate_Root(pTaskReduce, pTaskScheduler)
		)__PTSInternal_Parallel_Reduce::PTSTask_Reduce<PTSTYPE_Value, PTSTYPE_Lambda_Serial_Reduce, PTSTYPE_Lambda_Function_Combine>(
			Threshold, 
			Begin, 
			End, 
			&rResult,
			rSerialReduce,
			rFunctionCombine);

	pTaskScheduler->Task_WaitRoot(pTaskReduce);
}
#endif

#endif