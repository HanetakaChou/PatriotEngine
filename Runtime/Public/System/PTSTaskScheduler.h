#ifndef PT_SYSTEM_TASKSCHEDULER_H
#define PT_SYSTEM_TASKSCHEDULER_H

#include"PTSMemoryAllocator.h"

struct IPTSTask
{
	virtual void Ref_Count_Set(int count) = 0;

	virtual void Recycle_As_Child_Of(IPTSTask *pParent) = 0;
	virtual void Recycle_As_Continuation() = 0;

	//virtual void ParentSet(IPTSTask *pParent) = 0; //For Internal Use

	//virtual IPTTask *Execute() = 0; // IPTSTaskScheduler::TaskAllocate
};

#include <new>

struct IPTSTaskScheduler
{
	virtual void Destruct() = 0;

#if 0
	inline void Destory(IPTSHeap *pHeap)
	{
		this->Destruct();
		pHeap->AlignedFree(this);
	}
#endif

	//void Lambda(IPTSTask *pThis, IPTSTask **const ppNextToExecute)
	template<typename Lambda>
	inline void Task_Allocate(Lambda &rLambda)
	{
		this->Task_Allocate(
			sizeof(Lambda),
			alignof(Lambda),
			static_cast<void *>(&rLambda),
			[](void *pVoid_StorageCustom, void *pVoid_StorageCustom_ConstructorArgument)->void 
		{
			Lambda *pLambda_StorageCustom_ConstructorArgument = static_cast<Lambda *>(pVoid_StorageCustom_ConstructorArgument);
			new(pVoid_StorageCustom)Lambda(*pLambda_StorageCustom_ConstructorArgument);
		},
			[](void *pVoid_StorageCustom, IPTSTask *pThis, IPTSTask **const ppNextToExecute)->void
		{
			Lambda *pLambda = static_cast<Lambda *>(pVoid_StorageCustom);
			(*pLambda)(pThis, ppNextToExecute);
		}
		);
	}

	//PTSTaskStorage
	//--------
	//PTSTaskImpl //IPTSTask *pThis
	//--------
	//Pad //ui_StorageCustom_Align
	//--------
	//StorageCustom //void *pVoid_StorageCustom
	//->ui_StorageCustom_Size
	//--------

	virtual void Task_Allocate( //IPTTask *pParent, //SetParent 
		size_t ui_StorageCustom_Size,
		size_t ui_StorageCustom_Align,
		void *pVoid_StorageCustom_ConstructorArgument,
		void (*pFn_StorageCustom_Constructor)(
			void *pVoid_StorageCustom,
			void *pVoid_StorageCustom_ConstructorArgument
			),
		void(*pFn_Execute)(
			void *pVoid_StorageCustom,
			IPTSTask *pThis, 
			IPTSTask **const ppNextToExecute //Scheduler Bypass
			)
	) = 0;

	virtual void SpawnRootAndWait(IPTSTask *pFirst, IPTSTask *pNext) = 0;
};

struct alignas(alignof(uintptr_t)) PTSTaskSchedulerStorage
{
	char _Pad[sizeof(uintptr_t)];
};

extern "C" PTSYSTEMAPI IPTSTaskScheduler * PTCALL PTSTaskScheduler_Construct(PTSTaskSchedulerStorage *pStorage);

#if 0
inline IPTSTaskScheduler * PTCALL PTSTaskScheduler_Create(IPTSHeap *pHeap)
{
	PTSTaskSchedulerStorage *pStorage = static_cast<PTSTaskSchedulerStorage *>(pHeap->AlignedAlloc(sizeof(PTSTaskSchedulerStorage), alignof(PTSTaskSchedulerStorage)));
	return (pStorage != NULL) ? ::PTSTaskScheduler_Construct(pStorage) : NULL;
};
#endif

#endif