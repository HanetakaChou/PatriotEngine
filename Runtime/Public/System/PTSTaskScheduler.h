#ifndef PT_SYSTEM_TASKSCHEDULER_H
#define PT_SYSTEM_TASKSCHEDULER_H

struct IPTSTaskScheduler;

struct IPTSTaskPrefix
{
	virtual IPTSTaskPrefix *Parent() = 0;
	virtual void ParentSet(IPTSTaskPrefix *pParent) = 0;

	virtual void Recycle_AsChildOf(IPTSTaskPrefix *pParent) = 0;
	//virtual void Recycle_AsContinuation() = 0;

	//应当在SpawnTask之前SetRefCount
	virtual void RefCount_Set(uint32_t RefCount) = 0;

	virtual void OverrideExecute(void *(*pFn_Execute)(void *pTaskThisVoid)) = 0;

	inline void *Allocate_Child(size_t Size, size_t Alignment, IPTSTaskScheduler *pTaskScheduler = NULL);
	inline void *Allocate_Continuation(size_t Size, size_t Alignment, IPTSTaskScheduler *pTaskScheduler = NULL);
};

struct IPTSTaskScheduler
{
	virtual void Worker_Wake() = 0;
	virtual void Worker_Sleep() = 0;

	virtual void *Task_Allocate(size_t Size, size_t Alignment) = 0;

	virtual void Task_Spawn(void *pTask) = 0;
	virtual void Task_Spawn_Root_And_Wait(void *pTask) = 0;
};

extern "C" PTSYSTEMAPI PTBOOL PTCALL PTSTaskScheduler_Initialize(uint32_t ThreadNumber = 0U);
extern "C" PTSYSTEMAPI PTBOOL PTCALL PTSTaskScheduler_Uninitialize();
extern "C" PTSYSTEMAPI PTBOOL PTCALL PTSTaskScheduler_Local_Initialize(float fThreadNumberRatio = 1.0f);
extern "C" PTSYSTEMAPI PTBOOL PTCALL PTSTaskScheduler_Local_Uninitialize();
extern "C" PTSYSTEMAPI IPTSTaskScheduler * PTCALL PTSTaskScheduler_Local();
extern "C" PTSYSTEMAPI IPTSTaskPrefix * PTCALL PTSTaskScheduler_Task_Prefix(void *pTask);

//Helper Function

inline void *IPTSTaskPrefix::Allocate_Child(size_t Size, size_t Alignment, IPTSTaskScheduler *pTaskScheduler)
{
	if (pTaskScheduler == NULL)
	{
		pTaskScheduler = ::PTSTaskScheduler_Local();
	}

	void *pTaskNew = pTaskScheduler->Task_Allocate(Size, Alignment);
	IPTSTaskPrefix *pTaskNewPrefix = ::PTSTaskScheduler_Task_Prefix(pTaskNew);
	pTaskNewPrefix->ParentSet(this);

	return pTaskNew;
}

inline void *IPTSTaskPrefix::Allocate_Continuation(size_t Size, size_t Alignment, IPTSTaskScheduler *pTaskScheduler)
{
	if (pTaskScheduler == NULL)
	{
		pTaskScheduler = ::PTSTaskScheduler_Local();
	}

	IPTSTaskPrefix *pTaskParentPrefix = this->Parent();
	this->ParentSet(NULL);

	void *pTaskNew = pTaskScheduler->Task_Allocate(Size, Alignment);
	IPTSTaskPrefix *pTaskNewPrefix = ::PTSTaskScheduler_Task_Prefix(pTaskNew);
	pTaskNewPrefix->ParentSet(pTaskParentPrefix);

	return pTaskNew;
}

class IPTSTask
{
	IPTSTaskPrefix * const m_pTaskPrefix;
	void * const m_pTask;
public:
	inline explicit IPTSTask(void *pTask) :m_pTaskPrefix(::PTSTaskScheduler_Task_Prefix(pTask)), m_pTask(pTask)
	{

	}

	IPTSTaskPrefix *operator->() const
	{
		return m_pTaskPrefix;
	}

	operator void *() const
	{
		return m_pTask;
	}
};

#if 0
struct IPTSTaskScheduler
{
	virtual void Destruct() = 0;

	inline void Destory(IPTSHeap *pHeap)
	{
		this->Destruct();
		pHeap->AlignedFree(this);
	}

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


	virtual void *Task_Allocate(
		size_t ui_StorageCustom_Size,
		size_t ui_StorageCustom_Align,
		IPTSTask **pThis
	) = 0;

	virtual void *Task_Construct(
		void *pVoid_StorageCustom_ConstructorArgument,
		void(*pFn_StorageCustom_Constructor)(
			void *pVoid_StorageCustom,
			void *pVoid_StorageCustom_ConstructorArgument
			),
		void(*pFn_Execute)(
			void *pVoid_StorageCustom,
			IPTSTask *pThis,//prefix
			IPTSTask **const ppNextToExecute //Scheduler Bypass
			)
	) = 0;

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
		void(*pFn_StorageCustom_Constructor)(
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

#endif