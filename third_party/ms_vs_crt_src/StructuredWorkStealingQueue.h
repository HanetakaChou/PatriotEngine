// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// StructuredWorkStealingQueue.h
//
// Header file containing the core implementation of the work stealing data structures and algorithms.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     A StructuredWorkStealingQueue is a wait-free, lock-free structure associated with a single
    ///     thread that can Push and Pop elements. Other threads can do Steal operations
    ///     on the other end of the StructuredWorkStealingQueue with little contention.
    /// </summary>
    template <typename T, typename LOCK=_NonReentrantLock>
    class StructuredWorkStealingQueue
    {
        // A 'StructuredWorkStealingQueue' always runs its code in a single OS thread. We call this the
        // 'bound' thread. Only the code in the Steal operation can be executed by
        // other 'foreign' threads that try to steal work.
        //
        // The queue is implemented as a lock-free dequeue of arrays. The m_head and m_tail index this
        // array. To avoid copying elements, the m_head and m_tail index the array modulo
        // the size of the array. By making this a power of two, we can use a cheap
        // bit-and operation to take the modulus. The "m_mask" is always equal to the
        // size of the task array minus one (where the size is a power of two).
        //
        // The m_head and m_tail are volatile as they can be updated from different OS threads.
        // The "m_head" is only updated by foreign threads as they Steal a task from
        // this queue. By putting a lock in Steal, there is at most one foreign thread
        // changing m_head at a time. The m_tail is only updated by the bound thread.
        //
        // invariants:
        //   tasks.length is a power of 2
        //   m_mask == tasks.length-1
        //   m_head is only written to by foreign threads
        //   m_tail is only written to by the bound thread
        //   At most one foreign thread can do a Steal
        //   All methods except Steal are executed from a single bound thread
        //   m_tail points to the first unused location
        //
        // This work stealing implementation also supports the notion of out-of-order waiting
        // and out-of-order removal from the bound thread given that it is initialized to do so.
        // There is additional cost to performing this.
        //

    public:

        /// <summary>
        ///     Constructs a new work stealing queue
        /// </summary>
        StructuredWorkStealingQueue(LOCK *pLock)
            : m_head(0),
            m_tail(0),
            m_pLock(pLock)
        {      
            ASSERT(s_initialSize > 1);
            m_mask = s_initialSize - 1;
            m_ppTasks = new T*[s_initialSize];
            memset(m_ppTasks, 0, s_initialSize * sizeof(T*));
            ASSERT(m_pLock != NULL);
        }

        /// <summary>
        ///     Reinitializes a workqueue to the state essentially equivalent to just after construction.  
        ///     This is used when recycling a workqueue from its ListArray
        /// </summary>
        /// <param name="allowOutOfOrder">
        ///     Indicates whether or not the work stealing queue will allow out of order waiting on the bound thread.  
        ///     Allowing this has additional cost.
        /// </param>
        /// <param name="initialSize">
        ///     Indicates the initially allocated size for the physical work item storage
        /// </param>
        void Reinitialize()
        {
            m_head = 0;
            m_tail = 0;
        }

        //
        // unlocked count
        //
        int Count() const 
        {
            return (m_tail - m_head);
        }

        //
        // unlocked check
        //
        bool Empty() const 
        {
            return (m_tail <= m_head);
        }

        //
        // Push/Pop and Steal can be executed interleaved. In particular:
        // 1) A steal and pop should be careful when there is just one element
        //    in the queue. This is done by first incrementing the m_head/decrementing the m_tail
        //    and than checking if it interleaved (m_head > m_tail).
        // 2) A push and steal can interleave in the sense that a push can overwrite the
        //    value that is just stolen. To account for this, we check conservatively in
        //    the push to assume that the size is one less than it actually is.

        /// <summary>
        ///     Attempts to steal the oldest element in the queue.  This handles potential interleaving with both
        ///     a Pop and other Steal operations.
        /// </summary>
        T* Steal()
        {
            LOCK::_Scoped_lock lock(*m_pLock);
            return UnlockedSteal();
        }

        /// <summary>
        ///     Must be called under m_pLock->_Acquire/m_pLock->_TryAcquire
        /// </summary>
        T* UnlockedSteal()
        {
            if (m_head < m_tail)
            {
                int h = m_head;
                T *pResult = (T*) _InterlockedExchangePointer((volatile PVOID*) &m_ppTasks[h & m_mask], (PVOID) NULL);
                if (pResult != NULL)
                    m_head = h+1;
                return pResult;
            }

            return NULL;
        }

        /// <summary>
        ///     Attempts to pop the newest element on the work stealing queue.  It may return NULL if there is no such
        ///     item (either unbalanced push/pop, a chore stolen)
        /// </summary>
        T* Pop()
        {
            int t = m_tail - 1;
            m_tail = t;
            T* pResult = (T*) _InterlockedExchangePointer((volatile PVOID*) &m_ppTasks[t & m_mask], (PVOID) NULL);
            if (pResult == NULL)
                m_tail = t + 1;
            return pResult;
        }

        /// <summary>
        ///     Pushes an element onto the work stealing queue.
        /// </summary>
        void Push(T* element)
        {
            int t = m_tail;
            //
            // Careful here since we might interleave with Steal.
            // This is no problem since we just conservatively check if there is
            // enough space left (t < m_head + size). However, Steal might just have
            // incremented m_head and we could potentially overwrite the old m_head
            // entry, so we always leave at least one extra 'buffer' element and
            // check (m_tail < m_head + size - 1). This also plays nicely with our
            // initial m_mask of 0, where size is 2^0 == 1, but the tasks array is
            // still null.
            //
            if (t < m_head + m_mask)  // == t < m_head + size - 1
            {
                m_ppTasks[t & m_mask] = element;
                m_tail = t + 1;       // only increment once we have initialized the task entry.
            }
            else
                GrowAndPush(element);
        }

        /// <summary>
        ///     Destroys a work stealing queue.
        /// </summary>
        ~StructuredWorkStealingQueue()
        {      
            delete [] m_ppTasks;
        }

    private:

        // The m_head and m_tail are volatile as they can be updated from different OS threads.
        // The "m_head" is only updated by foreign threads as they Steal a task from
        // this queue. By putting a lock in Steal, there is at most one foreign thread
        // changing m_head at a time. The m_tail is only updated by the bound thread.
        //
        // invariants:
        //   tasks.length is a power of 2
        //   m_mask == tasks.length-1
        //   m_head is only written to by foreign threads
        //   m_tail is only written to by the bound thread
        //   At most one foreign thread can do a Steal
        //   All methods except Steal are executed from a single bound thread
        //   m_tail points to the first unused location
        //

        static const int s_initialSize = 64;  // must be a power of 2

        volatile int m_head;                  // only updated by Steal 
        volatile int m_tail; // 8/8           // only updated by Push and Pop 
        
        int m_mask;          // 12/16         // the m_mask for taking modulus 

        T**  m_ppTasks;      // 16/24         // the array of tasks 

        LOCK *m_pLock;       // 20/32

        // private helpers

        T* LockedPop(int t)
        {
            LOCK::_Scoped_lock lock(*m_pLock);
            T* pResult = NULL;

            if (m_head <= t)
                pResult = m_ppTasks[t & m_mask];
            else
                m_tail = t + 1;
            if (m_tail <= m_head)
                m_head = m_tail = 0;

            return pResult;
        }

        void GrowAndPush(T* element)
        {
            // We're full; expand the queue by doubling its size.
            int newLength = (m_mask + 1) << 1;
            T** ppNewTasks = new T*[newLength];
            T** ppOldTasks = m_ppTasks;

            {//for lock scope to exclude the delete[]
                LOCK::_Scoped_lock lock(*m_pLock);

                int t = m_tail;
                int h = m_head;
                int count = Count();

                for (int i = 0; i < count; i++)
                    ppNewTasks[i] = m_ppTasks[(i + h) & m_mask];

                memset(ppNewTasks + count, 0, (newLength - count) * sizeof(T*));

                // Reset the field values.
                m_ppTasks = ppNewTasks;
                m_head = 0;
                t = count;
                m_mask = newLength - 1;

                m_ppTasks[t & m_mask] = element;
                m_tail = t + 1;
            }//end: lock scope 

            delete[] ppOldTasks;
        }
    };
} // namespace details
} // namespace Concurrency
