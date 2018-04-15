#include "PTSTaskSchedulerImpl.h"
#include <new>

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
	/* PTSTaskImpl *pTaskImpl */ void *pVoid_StorageCustom = /* sizeof(PTSTaskImpl) + */ _PadTest; //Memory(±»»ÁHeap) Alloc
	pFn_StorageCustom_Constructor(_PadTest, pVoid_StorageCustom_ConstructorArgument); //Constructor

	//Execute
	//In the Scheduler
	IPTSTask *pNextToExecute = NULL; //Default NULL
	pFn_Execute(pVoid_StorageCustom, /* *pTaskImpl */ NULL, &pNextToExecute);
}


void PTSTaskSchedulerImpl::SpawnRootAndWait(IPTSTask *pFirst, IPTSTask *pNext)
{

}


