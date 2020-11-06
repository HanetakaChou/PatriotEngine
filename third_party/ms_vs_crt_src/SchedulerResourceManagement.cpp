// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// SchedulerResourceManagement.cpp
//
// Implementation of IScheduler.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Constructs an instance of the class that handles communication with the resource manager on behalf of a scheduler.
    /// </summary>
    SchedulerResourceManagement::SchedulerResourceManagement(SchedulerBase *pScheduler) 
        : m_pScheduler(pScheduler)
    {
    }

    /// <summary>
    ///     Returns a scheduler unique identifier for the context.
    /// </summary>
    /// <returns>
    ///     The Id for the IScheduler.
    /// </returns>
    unsigned int SchedulerResourceManagement::GetId() const
    {
        return m_pScheduler->m_id;
    }

    /// <summary>
    ///     Called by the resource manager in order to gather statistics for a given scheduler.  The statistics gathered here
    ///     will be used to drive dynamic feedback with the scheduler to determine when it is appropriate to assign more resources
    ///     or take resources away.  Note that these counts can be optimistic and do not necessarily have to reflect the current
    ///     count with 100% synchronized accuracy.
    /// </summary>
    /// <param name="pTaskCompletionRate">
    ///     The number of tasks which have been completed by the scheduler since the last call to the Statistics method.
    /// </param>
    /// <param name="pTaskArrivalRate">
    ///     The number of tasks that have arrived in the scheduler since the last call to the Statistics method.
    /// </param>
    /// <param name="pNumberOfTasksEnqueued">
    ///     The total number of tasks in all scheduler queues.
    /// </param>
    void SchedulerResourceManagement::Statistics(unsigned int *pTaskCompletionRate, unsigned int *pTaskArrivalRate, unsigned int *pNumberOfTasksEnqueued)
    {
        m_pScheduler->Statistics(pTaskCompletionRate, pTaskArrivalRate, pNumberOfTasksEnqueued);
    }

    /// <summary>
    ///     Get the scheduler policy.
    /// </summary>
    /// <returns>
    ///     The policy of the scheduler.
    /// </returns>
    SchedulerPolicy SchedulerResourceManagement::GetPolicy() const
    {
        return m_pScheduler->GetPolicy();
    }

    /// <summary>
    ///     Called when the resource manager is giving virtual processors to a particular scheduler.  The virtual processors are
    ///     identified by an array of IVirtualProcessorRoot interfaces. This call is made to grant virtual processor roots
    ///     at initial allocation during the course of ISchedulerProxy::RequestInitialVirtualProcessors, and during dynamic
    ///     core migration.
    /// </summary>
    /// <param name="pVirtualProcessorRoots">
    ///     An array of IVirtualProcessorRoot interfaces representing the virtual processors being added to the scheduler.
    /// </param>
    /// <param name="count">
    ///     Number of IVirtualProcessorRoot interfaces in the array.
    /// </param>
    void SchedulerResourceManagement::AddVirtualProcessors(IVirtualProcessorRoot **ppVirtualProcessorRoots, unsigned int count)
    {
        if (ppVirtualProcessorRoots == NULL)
            throw std::invalid_argument("ppVirtualProcessorRoots");

        if (count < 1)
            throw std::invalid_argument("count");
            
        for (unsigned int idx = 0; idx < count; ++idx)
            m_pScheduler->AddVirtualProcessor(ppVirtualProcessorRoots[idx]);
    }

    /// <summary>
    ///     Called when the resource manager is taking away virtual processors from a particular scheduler.  The scheduler should
    ///     mark the supplied virtual processors such that they are removed asynchronously and return immediately.  Note that
    ///     the scheduler should make every attempt to remove the virtual processors as quickly as possible as the resource manager
    ///     will reaffinitize threads executing upon them to other resources.  Delaying stopping the virtual processors may result
    ///     in unintentional oversubscription within the scheduler.
    /// </summary>
    /// <param name="pVirtualProcessorRoots">
    ///     An array of IVirtualProcessorRoot interfaces representing the virtual processors which are to be removed.
    /// </param>
    /// <param name="count">
    ///     Number of IVirtualProcessorRoot interfaces in the array.
    /// </param>
    void SchedulerResourceManagement::RemoveVirtualProcessors(IVirtualProcessorRoot **ppVirtualProcessorRoots, unsigned int count)
    {
        if (ppVirtualProcessorRoots == NULL)
            throw std::invalid_argument("ppVirtualProcessorRoots");

        if (count < 1)
            throw std::invalid_argument("count");

        for (unsigned int idx = 0; idx < count; ++idx)
            m_pScheduler->RemoveVirtualProcessor(ppVirtualProcessorRoots[idx]);
    }

    /// <summary>
    ///     Called when the resource manager is made aware that the hardware threads underneath the virtual processors assigned to
    ///     this particular scheduler are 'externally idle' once again i.e. any other schedulers that may have been using them have
    ///     stopped using them. This API is called only when a scheduler proxy was created with MinConcurrency = MaxConcurrency.
    /// </summary>
    /// <param name="pVirtualProcessorRoots">
    ///     An array of IVirtualProcessorRoot interfaces representing the virtual processors on which other schedulers have become idle.
    /// </param>
    /// <param name="count">
    ///     Number of IVirtualProcessorRoot interfaces in the array.
    /// </param>
    void SchedulerResourceManagement::NotifyResourcesExternallyIdle(IVirtualProcessorRoot **ppVirtualProcessorRoots, unsigned int count)
    {
    }

    /// <summary>
    ///     Called when the resource manager is made aware that the execution resources underneath the virtual processors assigned to
    ///     this particular scheduler are busy (active) on other schedulers. The reason these execution resources were lent to
    ///     other schedulers is usually a lack of activation on the part of this scheduler, or a system-wide oversubscription.
    ///     This API is called only when a scheduler proxy was created with MinConcurrency = MaxConcurrency.
    /// </summary>
    /// <param name="pVirtualProcessorRoots">
    ///     An array of IVirtualProcessorRoot interfaces representing the virtual processors on which other schedulers have become busy.
    /// </param>
    /// <param name="count">
    ///     Number of IVirtualProcessorRoot interfaces in the array.
    /// </param>
    void SchedulerResourceManagement::NotifyResourcesExternallyBusy(IVirtualProcessorRoot **ppVirtualProcessorRoots, unsigned int count)
    {
    }

    /// <summary>
    ///     After requesting initial threads from the resource manager, a scheduler which specifies it wishes UMS threads will be called from the
    ///     resource manager with a completion list interface which allows the scheduler to determine when UMS threads have unblocked.  Note that
    ///     this interface is only allowed to be touched from a thread running on a virtual processor root.
    /// </summary>
    /// <param name="pCompletionList">
    ///     The completion list interface for the scheduler.  There is a single list per scheduler.
    /// </param>
    void SchedulerResourceManagement::SetCompletionList(IUMSCompletionList *pCompletionList)
    {
        static_cast<UMSThreadScheduler *>(m_pScheduler)->SetCompletionList(pCompletionList);
    }
} // namespace details
} // namespace Concurrency
