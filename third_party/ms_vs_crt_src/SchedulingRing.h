// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// SchedulingRing.h
//
// Source file containing the SchedulingRing declaration.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     A scheduling node corresponds to a NUMA node or a processor package; containing one or more virtual processor groups.
    /// </summary>
    class SchedulingRing
    {
    public:
        SchedulingRing(SchedulerBase *pScheduler, int id);

        ~SchedulingRing();

        int Id() const 
        { 
            return m_id; 
        }

        // Create a new Schedule Group
        ScheduleGroupBase *AllocateScheduleGroup();

        // Delete a Schedule Group
        void FreeScheduleGroup(ScheduleGroupBase *pGroup);

        // Create a schedule group, add it to the list of groups
        ScheduleGroupBase *CreateScheduleGroup();

        ScheduleGroupBase *GetAnonymousScheduleGroup();

        /// <summary>
        ///     Returns a shared index to pseudo-round robin through schedule groups within the ring.
        /// </summary>
        ScheduleGroupBase *GetPseudoRRScheduleGroup(int *pIdx)
        {
            *pIdx = m_nextGroup;
            return GetNextScheduleGroup(pIdx, -1);
        }

        /// <summary>
        ///     Sets a shared index to pseudo-round robin through schedule groups within the ring.
        /// </summary>
        void SetPseudoRRScheduleGroup(int idx)
        {
            m_nextGroup = idx;
            ASSERT(m_nextGroup >= 0);
        }

        /// <summary>
        ///     Sets a shared index to pseudo-round robin through schedule groups within the ring.  This sets the index
        ///     to the schedule group *AFTER* idx in the iterator position.
        /// </summary>
        void SetPseudoRRScheduleGroupNext(int idx)
        {
            m_nextGroup = (idx + 1) % (m_scheduleGroups.MaxIndex());
            ASSERT(m_nextGroup >= 0);
        }

        /// <summary>
        ///     Returns the first schedule group.
        /// </summary>
        /// <param name="pIdx">
        ///     The iterator position of the returned schedule group will be placed here.  This can only be 
        ///     utilized as the pIdx parameter or the idxStart parameter of a GetNextScheduleGroup.
        /// </param>
        ScheduleGroupBase *GetFirstScheduleGroup(int *pIdx)
        {
            *pIdx = 0;
            return GetNextScheduleGroup(pIdx, -1);
        }

        /// <summary>
        ///     Returns the next schedule group in an iteration.
        /// </summary>
        ScheduleGroupBase *GetNextScheduleGroup(int *pIdx, int idxStart = 0)
        {
            int base = *pIdx + (idxStart == -1 ? 0 : 1);
            int size = m_scheduleGroups.MaxIndex();
            for (int i = 0; i < size; i++)
            {
                int index = (i + base) % size;
                if (index == idxStart)
                    return NULL;

                ScheduleGroupBase *pGroup = m_scheduleGroups[index];
                if (pGroup != NULL)
                {
                    *pIdx = index;
                    return pGroup;
                }
            }

            return NULL;
            
        }

        SchedulingNode *GetOwningNode() const
        {
            return m_pNode;
        }
    
    private:
        friend class SchedulerBase;
        friend class ScheduleGroupBase;
        friend class FairScheduleGroup;
        friend class CacheLocalScheduleGroup;
        friend class SchedulingNode;
        friend class VirtualProcessor;
        friend class InternalContextBase;
        friend class ThreadInternalContext;

        // Owning scheduler
        SchedulerBase *m_pScheduler;

        // Owning Node
        SchedulingNode *m_pNode;

        // The anonymous schedule group - for external contexts and tasks without an explicitly specified schedule group.
        // There is one anonymous sgroup per scheduling node.
        ScheduleGroupBase * m_pAnonymousScheduleGroup;

        // Scheduler groups owned by this ring
        ListArray<ScheduleGroupBase> m_scheduleGroups;

        int m_nextGroup;
        int m_id;

        // Removes the schedule group from m_ScheduleGroups
        void RemoveScheduleGroup(ScheduleGroupBase* pGroup);

        void SetOwningNode(SchedulingNode *pNode)
        {
            m_pNode = pNode;
        }
    };
} // namespace details
} // namespace Concurrency
