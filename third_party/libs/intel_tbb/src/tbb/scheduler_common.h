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

#ifndef _TBB_scheduler_common_H
#define _TBB_scheduler_common_H

namespace tbb
{
    namespace internal
    {
        //! Alignment for a task object
        const size_t task_alignment = 32;

        // sizeof(internal::task_prefix) == 64

        //! Number of bytes reserved for a task prefix
        /** If not exactly sizeof(task_prefix), the extra bytes *precede* the task_prefix. */
        const size_t task_prefix_reservation_size = ((64 - 1) / task_alignment + 1) * task_alignment;
    } // namespace internal
} // namespace mcrt
#endif
