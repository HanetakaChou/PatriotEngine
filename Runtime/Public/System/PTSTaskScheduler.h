#ifndef PT_SYSTEM_TASKSCHEDULER_H
#define PT_SYSTEM_TASKSCHEDULER_H

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

	virtual void OverrideExecute(IPTSTask *(*pFn_Execute)(IPTSTask *pTaskThisVoid)) = 0;

	template<typename TaskImpl>
	static inline void * Allocate_Root(TaskImpl *, size_t Size, size_t Alignment, IPTSTaskScheduler *pTaskScheduler = NULL);

	template<typename TaskImpl>
	inline void * Allocate_Child(TaskImpl *, size_t Size, size_t Alignment, IPTSTaskScheduler *pTaskScheduler = NULL);

	template<typename TaskImpl>
	inline void * Allocate_Continuation(TaskImpl *, size_t Size, size_t Alignment, IPTSTaskScheduler *pTaskScheduler = NULL);
};

struct IPTSTask
{
	//利用编译时强类型，减少错误发生
};

struct IPTSTaskScheduler
{
	virtual void Worker_Wake() = 0;
	virtual void Worker_Sleep() = 0;

	virtual IPTSTask *Task_Allocate(size_t Size, size_t Alignment) = 0;

	virtual void Task_Spawn(IPTSTask *pTask) = 0;
	virtual void Task_Spawn_Root_And_Wait(IPTSTask *pTask) = 0;
};

extern "C" PTSYSTEMAPI PTBOOL PTCALL PTSTaskScheduler_Initialize(uint32_t ThreadNumber = 0U);
extern "C" PTSYSTEMAPI PTBOOL PTCALL PTSTaskScheduler_Uninitialize();
extern "C" PTSYSTEMAPI PTBOOL PTCALL PTSTaskScheduler_Local_Initialize(float fThreadNumberRatio = 1.0f);
extern "C" PTSYSTEMAPI PTBOOL PTCALL PTSTaskScheduler_Local_Uninitialize();
extern "C" PTSYSTEMAPI IPTSTaskScheduler * PTCALL PTSTaskScheduler_Local();
extern "C" PTSYSTEMAPI IPTSTaskPrefix * PTCALL PTSTaskScheduler_Task_Prefix(IPTSTask *pTask);

//Helper Function

template<typename TaskImpl>
inline void * IPTSTaskPrefix::Allocate_Root(TaskImpl *, size_t Size, size_t Alignment, IPTSTaskScheduler *pTaskScheduler)
{
	if (pTaskScheduler == NULL)
	{
		pTaskScheduler = ::PTSTaskScheduler_Local();
	}

	IPTSTask *pTaskNew = pTaskScheduler->Task_Allocate(Size, Alignment);
	IPTSTaskPrefix *pTaskNewPrefix = ::PTSTaskScheduler_Task_Prefix(pTaskNew);
	pTaskNewPrefix->OverrideExecute(TaskImpl::Execute);

	return pTaskNew;
}

template<typename TaskImpl>
inline void * IPTSTaskPrefix::Allocate_Child(TaskImpl *, size_t Size, size_t Alignment, IPTSTaskScheduler *pTaskScheduler)
{
	if (pTaskScheduler == NULL)
	{
		pTaskScheduler = ::PTSTaskScheduler_Local();
	}

	IPTSTask *pTaskNew = pTaskScheduler->Task_Allocate(Size, Alignment);
	IPTSTaskPrefix *pTaskNewPrefix = ::PTSTaskScheduler_Task_Prefix(pTaskNew);
	pTaskNewPrefix->ParentSet(this);
	pTaskNewPrefix->OverrideExecute(TaskImpl::Execute);

	//++RefCount

	return pTaskNew;
}

template<typename TaskImpl>
inline void * IPTSTaskPrefix::Allocate_Continuation(TaskImpl *, size_t Size, size_t Alignment, IPTSTaskScheduler *pTaskScheduler)
{
	if (pTaskScheduler == NULL)
	{
		pTaskScheduler = ::PTSTaskScheduler_Local();
	}

	IPTSTaskPrefix *pTaskParentPrefix = this->Parent();
	this->ParentSet(NULL);

	IPTSTask *pTaskNew = pTaskScheduler->Task_Allocate(Size, Alignment);
	IPTSTaskPrefix *pTaskNewPrefix = ::PTSTaskScheduler_Task_Prefix(pTaskNew);
	pTaskNewPrefix->ParentSet(pTaskParentPrefix);
	pTaskNewPrefix->OverrideExecute(TaskImpl::Execute);

	return pTaskNew;
}

#endif