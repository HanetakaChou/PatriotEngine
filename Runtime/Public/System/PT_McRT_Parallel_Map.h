#ifndef PT_MCRT_PARALLEL_MAP_H
#define PT_MCRT_PARALLEL_MAP_H 1

#include "PTSTaskScheduler.h"
#include "PTSMemoryAllocator.h"

namespace __PT_McRT_Internal_Parallel_Map
{
	class PT_McRT_Task_Map_Continuation :public PT_McRT_ITask_Inner
	{
		PT_McRT_ITask *m_pTaskOuter;

	public:		
		static PT_McRT_ITask_Inner *CreateInstance(void *, PT_McRT_ITask *pTaskOuter)
		{
			return new (PT_McRT_Aligned_Malloc(sizeof(PT_McRT_Task_Map_Continuation), alignof(PT_McRT_Task_Map_Continuation))) PT_McRT_Task_Map_Continuation(pTaskOuter);
		}

	private:
		PT_McRT_ITask *Execute() override
		{
			//Conquer
			return NULL;
		}

		void Dispose() override
		{
			this->~PT_McRT_Task_Map_Continuation();
			PT_McRT_Aligned_Free(this);
		}

		inline PT_McRT_Task_Map_Continuation(PT_McRT_ITask *pTaskOuter) :m_pTaskOuter(pTaskOuter)
		{

		}

		inline ~PT_McRT_Task_Map_Continuation()
		{

		}
	};

	template<typename PTSTYPE_Lambda_Serial_Map>
	class PT_McRT_Task_Map :public PT_McRT_ITask_Inner
	{
		PT_McRT_ITask *m_pTaskOuter;
		PTSTYPE_Lambda_Serial_Map const &m_rSerialMap;
		uint32_t const m_Threshold;
		uint32_t m_Begin;
		uint32_t m_End;

	public:
		struct VA_List
		{
			PTSTYPE_Lambda_Serial_Map const &m_rSerialMap;
			uint32_t const m_Threshold;
			uint32_t m_Begin;
			uint32_t m_End;
		};

		static PT_McRT_ITask_Inner *CreateInstance(void *pUserData, PT_McRT_ITask *pTaskOuter)
		{
			return new (PT_McRT_Aligned_Malloc(sizeof(PT_McRT_Task_Map<PTSTYPE_Lambda_Serial_Map>), alignof(PT_McRT_Task_Map<PTSTYPE_Lambda_Serial_Map>))) PT_McRT_Task_Map<PTSTYPE_Lambda_Serial_Map>(pTaskOuter, static_cast<VA_List*>(pUserData));
		}

	private:
		PT_McRT_ITask *Execute() override
		{
			IPTSTaskScheduler *pTaskScheduler = ::PTSTaskScheduler_ForThread(); //Master And Worker Divergence

			assert(this->m_Threshold > 0);
			if ((this->m_End - this->m_Begin) > this->m_Threshold) //Recursive
			{
				//Divide
				uint32_t Middle = this->m_Begin + (this->m_End - this->m_Begin) / 2U;

				//Continuation Passing Style
				PT_McRT_ITask *pTaskContinuation = m_pTaskOuter->Allocate_Continuation(NULL, PT_McRT_Task_Map_Continuation::CreateInstance);

				VA_List a = { this->m_rSerialMap, this->m_Threshold, Middle, this->m_End };
				PT_McRT_ITask *pTaskChildRight = pTaskContinuation->Allocate_Child(&a, CreateInstance);

				//Recycle
				this->m_pTaskOuter->Recycle_AsChildOf(pTaskContinuation);
				this->m_End = Middle;

				pTaskContinuation->Set_Ref_Count(2);

				pTaskScheduler->Task_Spawn(pTaskChildRight);

				//Scheduler ByPass
				return m_pTaskOuter;
			}
			else //Base Case
			{
				m_rSerialMap(this->m_Begin, this->m_End);
				return NULL;
			}
		}

		void Dispose() override
		{
			this->~PT_McRT_Task_Map<PTSTYPE_Lambda_Serial_Map>();
			PT_McRT_Aligned_Free(this);
		}

		inline PT_McRT_Task_Map(
			PT_McRT_ITask *pTaskOuter,
			VA_List *pA
		)
			:
			m_pTaskOuter(pTaskOuter),
			m_rSerialMap(pA->m_rSerialMap),
			m_Threshold(pA->m_Threshold),
			m_Begin(pA->m_Begin),
			m_End(pA->m_End)
		{

		}

		inline ~PT_McRT_Task_Map()
		{

		}
	};


	class PT_McRT_TaskWait :public PT_McRT_ITask_Inner
	{
		uint32_t *const m_pHasBeenFinished;

	public:
		struct VA_List
		{
			uint32_t *const m_pHasBeenFinished;
		};

		static PT_McRT_ITask_Inner *CreateInstance(void *pUserData, PT_McRT_ITask *pTaskOuter)
		{
			return new (PT_McRT_Aligned_Malloc(sizeof(PT_McRT_TaskWait), alignof(PT_McRT_TaskWait))) PT_McRT_TaskWait(pTaskOuter, static_cast<VA_List*>(pUserData));
		}

	private:
		PT_McRT_ITask *Execute() override
		{
			::PTSAtomic_Set(m_pHasBeenFinished, 1U);
			return NULL;
		}

		void Dispose() override
		{
			this->~PT_McRT_TaskWait();
			PT_McRT_Aligned_Free(this);
		}

		inline PT_McRT_TaskWait(
			PT_McRT_ITask *pTaskOuter,
			VA_List *pA
		)
			:
			m_pHasBeenFinished(pA->m_pHasBeenFinished)
		{

		}

		inline ~PT_McRT_TaskWait()
		{

		}
	};
}

template<typename PTSTYPE_Lambda_Serial_Map>
inline void PT_McRT_Parallel_Map(
	uint32_t Threshold,
	uint32_t Begin,
	uint32_t End,
	PTSTYPE_Lambda_Serial_Map const &rSerialMap
)
{
	IPTSTaskScheduler *pTaskScheduler = ::PTSTaskScheduler_ForThread();
	
	uint32_t HasBeenFinished = 0U;
	
	PT_McRT_ITask *pTaskRoot;
	{
		__PT_McRT_Internal_Parallel_Map::PT_McRT_TaskWait::VA_List _w_a = { &HasBeenFinished };
		PT_McRT_ITask *pTaskWait = PT_McRT_ITask::Allocate_Root(&_w_a, __PT_McRT_Internal_Parallel_Map::PT_McRT_TaskWait::CreateInstance);

		typename __PT_McRT_Internal_Parallel_Map::PT_McRT_Task_Map<PTSTYPE_Lambda_Serial_Map>::VA_List _m_a = { rSerialMap, Threshold, Begin, End };
		pTaskRoot = pTaskWait->Allocate_Child(&_m_a, __PT_McRT_Internal_Parallel_Map::PT_McRT_Task_Map<PTSTYPE_Lambda_Serial_Map>::CreateInstance);
		pTaskWait->Set_Ref_Count(1U);
	}
	
	pTaskScheduler->Task_ExecuteAndWait(pTaskRoot, &HasBeenFinished, [](void *pVoidForPredicate)->bool {return ::PTSAtomic_Get(static_cast<uint32_t *>(pVoidForPredicate)); });
}

#endif