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
            protected:
                static int __TBB_EXPORTED_FUNC internal_current_slot();
                static int __TBB_EXPORTED_FUNC internal_max_concurrency(const task_arena *);

            public:
                //! Typedef for number of threads that is automatic.
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
            friend int tbb::this_task_arena::max_concurrency();

        public:
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

} // namespace tbb

#include "internal/_warning_suppress_disable_notice.h"
#undef __TBB_task_arena_H_include_area

#endif /* __TBB_task_arena_H */
