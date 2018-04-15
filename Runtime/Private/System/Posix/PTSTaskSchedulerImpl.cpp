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

void PTSTaskSchedulerImpl::SpawnRootAndWait(PTSTask *pFirst, PTTask *pNext)
{

}


