// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// FairScheduleGroup.h
//
// Header file containing FairScheduleGroup related declarations.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

namespace Concurrency
{
namespace details
{

    class FairScheduleGroup : public ScheduleGroupBase
    {

    public:

        //
        // Public Methods
        //

        /// <summary>
        ///     Constructs a fair schedule group
        /// </summary>
        FairScheduleGroup(SchedulingRing *pRing) 
            : ScheduleGroupBase(pRing)
        {
            m_kind = FairScheduling;
        }

    private:
        friend class SchedulerBase;
        friend class ContextBase;
        friend class ExternalContextBase;
        friend class InternalContextBase;
        friend class ThreadInternalContext;
        friend class SchedulingNode;
        friend class SchedulingRing;
        friend class VirtualProcessor;

        //
        // Private data
        //

        // Each schedule group has three stores of work. It has a collection of runnable contexts,
        // a FIFO queue of realized chores and a list of workqueues that hold unrealized chores.

        // A collection of Runnable contexts.
        SafeSQueue<InternalContextBase, _HyperNonReentrantLock> m_runnableContexts;

        //
        // Private methods
        //

        /// <summary>
        ///     Puts a runnable context into the runnables collection in the schedule group.
        /// </summary>
        void AddToRunnablesCollection(InternalContextBase *pContext);

        InternalContextBase *GetRunnableContext()
        {
            if (m_runnableContexts.Empty())
                return NULL;

            InternalContextBase *pContext = m_runnableContexts.Dequeue();
#if defined(_DEBUG)
            SetContextDebugBits(pContext, CTX_DEBUGBIT_REMOVEDFROMRUNNABLES);
#endif // _DEBUG
            return pContext;
        }

        void * operator new(size_t _Size)
        {
            return ::operator new(_Size);
        }

        void operator delete(void * _Ptr)
        {
            ::operator delete(_Ptr);
        }
    };
} // namespace details
} // namespace Concurrency
