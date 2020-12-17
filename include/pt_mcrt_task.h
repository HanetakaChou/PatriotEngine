/*
 * Copyright (C) YuqiaoZhang
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _PT_MCRT_TASK_H_
#define _PT_MCRT_TASK_H_ 1

#ifdef __cplusplus
extern "C"
{
#endif

#include "pt_common.h"
#include "pt_mcrt_common.h"
#include <stdint.h>

    // VK_DEFINE_HANDLE
    // VK_DEFINE_NON_DISPATCHABLE_HANDLE
    typedef struct _mcrt_task_t_ *mcrt_task;

    struct mcrt_task_user_data_t
    {
        alignas(void *) uint8_t m_user_data[128];
    };

    // Scheduler Bypass
    // https://software.intel.com/content/www/us/en/develop/documentation/tbb-documentation/top/intel-threading-building-blocks-developer-guide/the-task-scheduler/useful-task-techniques/scheduler-bypass.html

    // The tbb task has no destructor! //It's designed to store the lightweight data.
    PT_MCRT_ATTR mcrt_task PT_CALL mcrt_task_allocate_root(mcrt_task (*execute_callback)(mcrt_task_user_data_t *user_data));

    // Continuation Passing
    // https://software.intel.com/content/www/us/en/develop/documentation/tbb-documentation/top/intel-threading-building-blocks-developer-guide/the-task-scheduler/useful-task-techniques/continuation-passing.html
    PT_MCRT_ATTR mcrt_task PT_CALL mcrt_task_allocate_continuation(mcrt_task self, mcrt_task (*execute_callback)(mcrt_task_user_data_t *user_data));

    PT_MCRT_ATTR mcrt_task_user_data_t *PT_CALL mcrt_task_user_data(mcrt_task t);

    PT_MCRT_ATTR void PT_CALL mcrt_task_destory(mcrt_task victim);

    // Recycling
    // https://software.intel.com/content/www/us/en/develop/documentation/tbb-documentation/top/intel-threading-building-blocks-developer-guide/the-task-scheduler/useful-task-techniques/recycling.html
    PT_MCRT_ATTR void PT_CALL mcrt_task_recycle_as_child_of(mcrt_task self, mcrt_task successor);

    PT_MCRT_ATTR int PT_CALL mcrt_task_ref_count(mcrt_task self);

    PT_MCRT_ATTR void PT_CALL mcrt_task_set_ref_count(mcrt_task self, int count);

    PT_MCRT_ATTR int PT_CALL mcrt_task_decrement_ref_count(mcrt_task self);

    PT_MCRT_ATTR void PT_CALL mcrt_task_set_parent(mcrt_task self, mcrt_task parent);

    PT_MCRT_ATTR void PT_CALL mcrt_task_spawn(mcrt_task t);

    PT_MCRT_ATTR void PT_CALL mcrt_task_spawn_and_wait_for_all(mcrt_task self, mcrt_task child);

#ifdef __cplusplus
}
#endif

#endif