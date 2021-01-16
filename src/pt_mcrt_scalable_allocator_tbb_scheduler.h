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

#ifndef _PT_MCRT_TASK_TBB_TBB_SCHEDULER_H_
#define _PT_MCRT_TASK_TBB_TBB_SCHEDULER_H_ 1

#include "pt_mcrt_scalable_allocator_tbb_scheduler_common.h"

namespace mcrt
{
    namespace internal
    {
        //! Work stealing task scheduler.
        /** None of the fields here are ever read or written by threads other than
        the thread that creates the instance.

        Class generic_scheduler is an abstract base class that contains most of the scheduler,
        except for tweaks specific to processors and tools (e.g. VTune(TM) Performance Tools).
        The derived template class custom_scheduler<SchedulerTraits> fills in the tweaks. */
        class generic_scheduler
        {
            // almost every class in TBB uses generic_scheduler
        public:
            //! If sizeof(task) is <=quick_task_size, it is handled on a free list instead of malloc'd.
            static const size_t quick_task_size = 256 - task_prefix_reservation_size;
        };
    } // namespace internal
} // namespace mcrt
#endif