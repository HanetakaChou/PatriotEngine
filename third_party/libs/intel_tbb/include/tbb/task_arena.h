/*
    Copyright (c) 2005-2019 Intel Corporation

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef __TBB_task_arena_H
#define __TBB_task_arena_H

#define __TBB_task_arena_H_include_area
#include "internal/_warning_suppress_enable_notice.h"

#include "task.h"
#if TBB_USE_THREADING_TOOLS
#include "atomic.h" // for as_atomic
#endif

namespace tbb
{

    namespace this_task_arena
    {
        int max_concurrency();
    } // namespace this_task_arena

    //! @cond INTERNAL
    namespace internal
    {
        //! Internal to library. Should not be used by clients.
        /** @ingroup task_scheduling */
        class arena;
        class task_scheduler_observer_v3;
    } // namespace internal
    //! @endcond

    namespace interface7
    {
        class task_arena;

        //! @cond INTERNAL
        namespace internal
        {
            using namespace tbb::internal; //e.g. function_task from task.h

            class task_arena_base
            {
            public:
                //! NULL if not currently initialized.
                internal::arena *my_arena;

            protected:
#if __TBB_TASK_GROUP_CONTEXT
                //! default context of the arena
                task_group_context *my_context;
#endif

                //! Concurrency level for deferred initialization
                int my_max_concurrency;

                //! Reserved master slots
                unsigned my_master_slots;

                //! Special settings
                intptr_t my_version_and_traits;

                enum
                {
                    default_flags = 0
#if __TBB_TASK_GROUP_CONTEXT
                                    | (task_group_context::default_traits & task_group_context::exact_exception) // 0 or 1 << 16
                    ,
                    exact_exception_flag = task_group_context::exact_exception // used to specify flag for context directly
#endif
                };

                task_arena_base(int max_concurrency, unsigned reserved_for_masters)
                    : my_arena(0)
#if __TBB_TASK_GROUP_CONTEXT
                      ,
                      my_context(0)
#endif
                      ,
                      my_max_concurrency(max_concurrency), my_master_slots(reserved_for_masters), my_version_and_traits(default_flags)
                {
                }

                void __TBB_EXPORTED_METHOD internal_initialize();
                void __TBB_EXPORTED_METHOD internal_terminate();

                void __TBB_EXPORTED_METHOD internal_attach();
                void __TBB_EXPORTED_METHOD internal_enqueue(task &, intptr_t) const;
                static int __TBB_EXPORTED_FUNC internal_current_slot();
                static int __TBB_EXPORTED_FUNC internal_max_concurrency(const task_arena *);

            public:
                //! Typedef for number of threads that is automatic.
                static const int automatic = -1;

                static const int not_initialized = -2;
            };

        } // namespace internal
        //! @endcond

        /** 1-to-1 proxy representation class of scheduler's arena
         * Constructors set up settings only, real construction is deferred till the first method invocation
         * Destructor only removes one of the references to the inner arena representation.
         * Final destruction happens when all the references (and the work) are gone.
         */
        class task_arena : public internal::task_arena_base
        {
            friend void task::enqueue(task &, task_arena &
#if __TBB_TASK_PRIORITY
                                      ,
                                      priority_t
#endif
            );
            friend int tbb::this_task_arena::max_concurrency();
            bool my_initialized;
            void mark_initialized()
            {
                __TBB_ASSERT(my_arena, "task_arena initialization is incomplete");
#if __TBB_TASK_GROUP_CONTEXT
                __TBB_ASSERT(my_context, "task_arena initialization is incomplete");
#endif
                my_initialized = true;
            }

        public:
            //! Tag class used to indicate the "attaching" constructor
            struct attach
            {
            };

            //! Creates an instance of task_arena attached to the current arena of the thread
            explicit task_arena(attach)
                : task_arena_base(automatic, 1) // use default settings if attach fails
                  ,
                  my_initialized(false)
            {
                internal_attach();
                if (my_arena)
                    my_initialized = true;
            }

            //! Forces allocation of the resources for the task_arena as specified in constructor arguments
            inline void initialize()
            {
                if (!my_initialized)
                {
                    internal_initialize();
                    mark_initialized();
                }
            }

            //! Removes the reference to the internal arena representation.
            //! Not thread safe wrt concurrent invocations of other methods.
            inline void terminate()
            {
                if (my_initialized)
                {
                    internal_terminate();
                    my_initialized = false;
                }
            }

            //! Removes the reference to the internal arena representation, and destroys the external object.
            //! Not thread safe wrt concurrent invocations of other methods.
            ~task_arena()
            {
                terminate();
            }

            //! Returns true if the arena is active (initialized); false otherwise.
            //! The name was chosen to match a task_scheduler_init method with the same semantics.
            bool is_active() const { return my_initialized; }

            //! Returns the index, aka slot number, of the calling thread in its current arena
            //! This method is deprecated and replaced with this_task_arena::current_thread_index()
            inline static int current_thread_index()
            {
                return internal_current_slot();
            }
        };

    } // namespace interfaceX

    using interface7::task_arena;

    namespace this_task_arena
    {
        //! Returns the index, aka slot number, of the calling thread in its current arena
        inline int current_thread_index()
        {
            int idx = tbb::task_arena::current_thread_index();
            return idx == -1 ? tbb::task_arena::not_initialized : idx;
        }

        //! Returns the maximal number of threads that can work inside the arena
        inline int max_concurrency()
        {
            return tbb::task_arena::internal_max_concurrency(NULL);
        }
    } // namespace this_task_arena

    //! Enqueue task in task_arena
#if __TBB_TASK_PRIORITY
    void task::enqueue(task &t, task_arena &arena, priority_t p)
    {
#else
    void task::enqueue(task &t, task_arena &arena)
    {
        intptr_t p = 0;
#endif
        arena.initialize();
        //! Note: the context of the task may differ from the context instantiated by task_arena
        arena.internal_enqueue(t, p);
    }

} // namespace tbb

#include "internal/_warning_suppress_disable_notice.h"
#undef __TBB_task_arena_H_include_area

#endif /* __TBB_task_arena_H */
