#ifndef PT_SYSTEM_TASKSCHEDULER_H
#define PT_SYSTEM_TASKSCHEDULER_H

#include <stddef.h>
#include <assert.h>

struct IPTSTaskScheduler;

struct IPTSTask;

struct IPTSTaskPrefix
{
	virtual IPTSTaskPrefix *Parent() = 0;
	virtual void ParentSet(IPTSTaskPrefix *pParent) = 0;

	virtual void Recycle_AsChildOf(IPTSTaskPrefix *pParent) = 0;
	//virtual void Recycle_AsContinuation() = 0;

	//应当在SpawnTask之前SetRefCount
	virtual void RefCount_Set(uint32_t RefCount) = 0;
};

//利用编译时强类型，减少错误发生
struct IPTSTask
{
	virtual IPTSTask *Execute() = 0;

	//Helper Function
private:
	IPTSTaskPrefix *m_pPrefix;
public:
	inline IPTSTask();

	inline void ParentSet(IPTSTask *pParent);

	inline void Recycle_AsChildOf(IPTSTask *pParent);

	inline void RefCount_Set(uint32_t RefCount);

	template<typename TaskImpl>
	static inline void * Allocate_Root(TaskImpl *pTaskNull, IPTSTaskScheduler *pTaskScheduler = NULL);

	template<typename TaskImpl>
	inline void * Allocate_Child(TaskImpl *pTaskNull, IPTSTaskScheduler *pTaskScheduler = NULL);

	template<typename TaskImpl>
	inline void * Allocate_Continuation(TaskImpl *pTaskNull, IPTSTaskScheduler *pTaskScheduler = NULL);
};

struct IPTSTaskScheduler
{
	virtual IPTSTask *Task_Allocate(size_t Size, size_t Alignment) = 0;

	virtual void Task_Spawn(IPTSTask *pTask) = 0;
	virtual void Task_WaitRoot(IPTSTask *pTask) = 0;

	virtual void Worker_Wake() = 0;
	virtual void Worker_Sleep() = 0;
};

extern "C" PTSYSTEMAPI PTBOOL PTCALL PTSTaskScheduler_Initialize(uint32_t ThreadNumber = 0U);
extern "C" PTSYSTEMAPI PTBOOL PTCALL PTSTaskScheduler_Initialize_ForThread(float fThreadNumberRatio = 1.0f);
extern "C" PTSYSTEMAPI IPTSTaskScheduler * PTCALL PTSTaskScheduler_ForThread();
extern "C" PTSYSTEMAPI IPTSTaskPrefix * PTCALL PTSTaskScheduler_Task_Prefix(IPTSTask *pTask);

//Helper Function

inline IPTSTask::IPTSTask()
{
	assert(m_pPrefix != NULL); //You Should Use IPTSTask::Allocate_Root/Child/Continuation Instead Of IPTSTaskScheduler::Task_Allocate
}

inline void IPTSTask::ParentSet(IPTSTask *pParent)
{
	m_pPrefix->ParentSet(pParent->m_pPrefix);
}

inline void IPTSTask::Recycle_AsChildOf(IPTSTask *pParent)
{
	m_pPrefix->Recycle_AsChildOf(pParent->m_pPrefix);
}

inline void IPTSTask::RefCount_Set(uint32_t RefCount)
{
	m_pPrefix->RefCount_Set(RefCount);
}

template<typename TaskImpl>
inline void * IPTSTask::Allocate_Root(TaskImpl *pTaskNull, IPTSTaskScheduler *pTaskScheduler)
{
	assert(pTaskNull == NULL);

	if (pTaskScheduler == NULL)
	{
		pTaskScheduler = ::PTSTaskScheduler_ForThread();
	}

	IPTSTask *pTaskNew = pTaskScheduler->Task_Allocate(sizeof(TaskImpl), alignof(TaskImpl));
	pTaskNew->m_pPrefix = ::PTSTaskScheduler_Task_Prefix(pTaskNew);
	return pTaskNew;
}

template<typename TaskImpl>
inline void * IPTSTask::Allocate_Child(TaskImpl *pTaskNull, IPTSTaskScheduler *pTaskScheduler)
{
	assert(pTaskNull == NULL);

	if (pTaskScheduler == NULL)
	{
		pTaskScheduler = ::PTSTaskScheduler_ForThread();
	}

	IPTSTask *pTaskNew = pTaskScheduler->Task_Allocate(sizeof(TaskImpl), alignof(TaskImpl));
	pTaskNew->m_pPrefix = ::PTSTaskScheduler_Task_Prefix(pTaskNew);
	pTaskNew->m_pPrefix->ParentSet(m_pPrefix);

	//++RefCount

	return pTaskNew;
}

template<typename TaskImpl>
inline void * IPTSTask::Allocate_Continuation(TaskImpl *pTaskNull, IPTSTaskScheduler *pTaskScheduler)
{
	assert(pTaskNull == NULL);

	if (pTaskScheduler == NULL)
	{
		pTaskScheduler = ::PTSTaskScheduler_ForThread();
	}

	IPTSTaskPrefix *pTaskParentPrefix = m_pPrefix->Parent();
	m_pPrefix->ParentSet(NULL);

	IPTSTask *pTaskNew = pTaskScheduler->Task_Allocate(sizeof(TaskImpl), alignof(TaskImpl));
	pTaskNew->m_pPrefix = ::PTSTaskScheduler_Task_Prefix(pTaskNew);
	pTaskNew->m_pPrefix->ParentSet(pTaskParentPrefix);

	return pTaskNew;
}

//RECURSIVE IMPLEMENTATION OF MAP
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

				//Scheduler ByPass
				this->Recycle_AsChildOf(pTaskContinuation);
				this->m_End = Middle;

				pTaskContinuation->RefCount_Set(2);

				pTaskScheduler->Task_Spawn(pTaskChildRight);

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

				//Scheduler ByPass
				this->Recycle_AsChildOf(pTaskContinuation);
				this->m_End = Middle;
				this->m_pResultRoot = &pTaskContinuation->m_ResultChildLeft;

				pTaskContinuation->RefCount_Set(2);

				pTaskScheduler->Task_Spawn(pTaskChildRight);

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