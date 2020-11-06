// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// SchedulerResourceManagement.h
//
// Implementation of IScheduler.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Concurrency
{
namespace details
{
    class SchedulerResourceManagement : public IUMSScheduler
    {
    public:
        /// <summary>
        ///     Constructs an instance of the class that handles communication with the resource manager on behalf of a scheduler.
        /// </summary>
        SchedulerResourceManagement(SchedulerBase *pScheduler);

        /// <summary>
        ///     Returns a scheduler unique identifier for the context.
        /// </summary>
        /// <returns>
        ///     The Id for the IScheduler.
        /// </returns>
        virtual unsigned int GetId() const;

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
        virtual void Statistics(unsigned int *pTaskCompletionRate, unsigned int *pTaskArrivalRate, unsigned int *pNumberOfTasksEnqueued);

        /// <summary>
        ///     Get the scheduler policy.
        /// </summary>
        /// <returns>
        ///     The policy of the scheduler.
        /// </returns>
        virtual SchedulerPolicy GetPolicy() const;

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
        virtual void AddVirtualProcessors(IVirtualProcessorRoot **ppVirtualProcessorRoots, unsigned int count);

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
        virtual void RemoveVirtualProcessors(IVirtualProcessorRoot **ppVirtualProcessorRoots, unsigned int count);

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
        virtual void NotifyResourcesExternallyIdle(IVirtualProcessorRoot ** ppVirtualProcessorRoots, unsigned int count);

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
        virtual void NotifyResourcesExternallyBusy(IVirtualProcessorRoot ** ppVirtualProcessorRoots, unsigned int count);

        /// <summary>
        ///     After requesting initial threads from the resource manager, a scheduler which specifies it wishes UMS threads will be called from the
        ///     resource manager with a completion list interface which allows the scheduler to determine when UMS threads have unblocked.  Note that
        ///     this interface is only allowed to be touched from a thread running on a virtual processor root.
        /// </summary>
        /// <param name="pCompletionList">
        ///     The completion list interface for the scheduler.  There is a single list per scheduler.
        /// </param>
        virtual void SetCompletionList(IUMSCompletionList *pCompletionList);

    private:

        // The scheduler for which communication with the RM is handled.
        SchedulerBase *m_pScheduler;

        /// <summary>
        ///     Destroys an instance of scheduler resource management.
        /// </summary>
        ~SchedulerResourceManagement() {}
    };
} // namespace details
} // namespace Concurrency
