// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// CurrentScheduler.cpp
//
// Implementation of static scheduler APIs for CurrentScheduler::
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
/// <returns>
///     Returns a unique identifier for the current scheduler.  Returns -1 if no current scheduler exists.
/// </returns>
unsigned int CurrentScheduler::Id()
{
    const SchedulerBase *pScheduler = SchedulerBase::SafeFastCurrentScheduler();
    return pScheduler != NULL ? pScheduler->Id() : UINT_MAX;
}

/// <returns>
///     Returns a current number of virtual processors for the current scheduler. Returns -1 if no current scheduler exists.
///     No error state.
/// </returns> 
unsigned int CurrentScheduler::GetNumberOfVirtualProcessors()
{
    const SchedulerBase *pScheduler = SchedulerBase::SafeFastCurrentScheduler();
    return pScheduler != NULL ? pScheduler->GetNumberOfVirtualProcessors() : UINT_MAX;
}

/// <returns>
///     Returns a copy of the policy the current scheduler is using.  Returns NULL if no current
///     scheduler exists.
/// </returns>
SchedulerPolicy CurrentScheduler::GetPolicy()
{
    const SchedulerBase *pScheduler = SchedulerBase::CurrentScheduler();
    return pScheduler->GetPolicy();
}

/// <summary>
///     Returns a reference to the scheduler instance in TLS storage (viz., the current scheduler).
///     If one does not exist, the default scheduler for the process is attached to the calling thread and returned -- 
///     if the default scheduler does not exist it is created
/// </summary>  
/// <returns>
///     The TLS storage for the current scheduler is returned.
/// </returns>
Scheduler* CurrentScheduler::Get()
{
    return SchedulerBase::CurrentScheduler();
}

/// <summary>
///     The normal scheduler factory. (Implicitly calls Scheduler::Attach on the internally represented scheduler instance.)
///     The created scheduler will become the current scheduler for the current context (if it is an OS context it will be
///     inducted to a ConcRT context).  To shutdown such a scheduler, Detach needs to be called. Any extra Reference calls
///     must be matched with Release for shutdown to commence. 
/// </summary>
/// <param name="pPolicy">
///     [in] A const pointer to the scheduler policy (See Scheduler Policy API)
/// </param>
void CurrentScheduler::Create(const SchedulerPolicy& policy)
{
    SchedulerBase *pScheduler = SchedulerBase::Create(policy);
    pScheduler->Attach();
}

/// <summary>
///     Detaches the current scheduler from the calling thread and restores the previously attached scheduler as the current
///     scheduler. Implicitly calls Release. After this function is called, the calling thread is then managed by the scheduler
///     that was previously activated via Create() or Attach(). 
/// </summary>
void CurrentScheduler::Detach()
{
    SchedulerBase* pScheduler = SchedulerBase::SafeFastCurrentScheduler();

    if (pScheduler != NULL)
    {
        return pScheduler->Detach();
    }
    else 
    {
        throw scheduler_not_attached();
    }
}

/// <summary>
///     Causes the OS event object ‘shutdownEvent’ to be set when the scheduler shuts down and destroys itself.
/// </summary>
/// <param name="shutdownEvent">
///     [in] A valid event object
/// </param>
void CurrentScheduler::RegisterShutdownEvent(HANDLE shutdownEvent)
{
    SchedulerBase* pScheduler = SchedulerBase::SafeFastCurrentScheduler();

    if (pScheduler != NULL)
    {
        return pScheduler->RegisterShutdownEvent(shutdownEvent);
    }
    else
    {
        throw scheduler_not_attached();
    }
}

/// <summary>
///     Create a schedule group within the current scheduler.
/// </summary>
ScheduleGroup* CurrentScheduler::CreateScheduleGroup()
{
    return SchedulerBase::CurrentScheduler()->CreateScheduleGroup();
}

/// <summary>
///     Create a light-weight schedule within the current scheduler in an implementation dependent schedule group.
/// </summary>
/// <param name="proc">
///     [in] A pointer to the main function of a task.
/// </param>
/// <param name="data">
///     [in] A void pointer to the data that will be passed in to the task.
/// <param>
void CurrentScheduler::ScheduleTask(TaskProc proc, void *data)
{
    SchedulerBase::CurrentScheduler()->ScheduleTask(proc, data);
}

} // namespace Concurrency
