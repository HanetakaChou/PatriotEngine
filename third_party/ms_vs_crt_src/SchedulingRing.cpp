// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// SchedulingRing.cpp
//
// Source file containing the SchedulingRing implementation.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Construct a new scheduling ring.
    /// </summary>
    SchedulingRing::SchedulingRing(SchedulerBase *pScheduler, int id) :
        m_pScheduler(pScheduler), 
        m_pNode(NULL), // Will be set later explicitly by the creating scheduler
        m_pAnonymousScheduleGroup(NULL), 
        m_id(id),
        m_nextGroup(0),
        m_scheduleGroups(pScheduler, 256, 64)
    {
        //
        // Create the anonymous schedule group early.  UMS schedulers need somewhere to begin a search for a given node that is guaranteed
        // to be safe.  This is the only such place.
        //
        // Create schedule group takes a reference to the schedule group. The scheduling
        // node maintains this reference and will release it when it disassociates from the
        // schedule group (either in the destructor, or if the schedule group is moved to
        // a different node due to resource management reclaiming the node).
        //
        m_pAnonymousScheduleGroup = CreateScheduleGroup();
        // An anonymous SG, once created lives for the lifetime of the scheduler.
        m_pAnonymousScheduleGroup->m_kind |= ScheduleGroupBase::AnonymousScheduleGroup;
    }

    SchedulingRing::~SchedulingRing()
    {
        ASSERT(m_pAnonymousScheduleGroup != NULL);
        m_pAnonymousScheduleGroup->m_kind &= ~ScheduleGroupBase::AnonymousScheduleGroup;
        m_pAnonymousScheduleGroup->InternalRelease();
        m_pAnonymousScheduleGroup = NULL;
    }

    ScheduleGroupBase *SchedulingRing::AllocateScheduleGroup()
    {
        if (m_pScheduler->m_schedulingProtocol == ::Concurrency::EnhanceScheduleGroupLocality)
        {
            return new CacheLocalScheduleGroup(this);
        }
        else
        {
            ASSERT(m_pScheduler->m_schedulingProtocol == ::Concurrency::EnhanceForwardProgress);
            return new FairScheduleGroup(this);
        }
    }

#pragma warning(disable:25007)  // method can be static
    void SchedulingRing::FreeScheduleGroup(ScheduleGroupBase *pGroup)
    {
        delete pGroup;
    }
#pragma warning(default:25007)  // method can be static

    ScheduleGroupBase *SchedulingRing::CreateScheduleGroup()
    {
        ScheduleGroupBase* pGroup = m_scheduleGroups.PullFromFreePool();

        if (pGroup == NULL)
        {
            pGroup = AllocateScheduleGroup();
        }
        else
        {
            // reinitialize the reused schedule group to its initial state
            ASSERT(pGroup->m_refCount == 0);
            pGroup->m_refCount = 1;
            pGroup->m_pRing = this;
        }

        m_scheduleGroups.Add(pGroup);
        SetPseudoRRScheduleGroup(m_nextGroup);

        ASSERT(pGroup->m_refCount >= 0);
        return pGroup;
    }

    //
    // Called when a schedule group's ref count is 0. remove this schedule group from the action.
    //
    void SchedulingRing::RemoveScheduleGroup(ScheduleGroupBase *pGroup)
    {
        //
        // Essentially m_refCount is the refcount of how many contexts reference this schedule group.
        // m_refCount is incremented whenever a schedule group is associated with a context, decremented 
        // when a context is switched out to another context.  When workstealing happens the context that 
        // steals from this schedule group increments m_refCount and decrements after it is done with this 
        // schedule group, whether by stealing from another or by switching out and going back on the 
        // freelist.  When m_refCount hits 0, then the schedule group is taken out of m_groups and hence 
        // is not searched for runnables or stolen chores.
        //
        ASSERT(pGroup != NULL && pGroup->m_refCount == 0);

        // It is still in the circular list, but other apis that are traversing the list will move over
        // it, since it will not have any runnable contexts or chores to steal.

        m_scheduleGroups.Remove(pGroup);
    }

    ScheduleGroupBase *SchedulingRing::GetAnonymousScheduleGroup()
    {
        ASSERT(m_pAnonymousScheduleGroup != NULL);
        return m_pAnonymousScheduleGroup;
    }
} // namespace details
} // namespace Concurrency
