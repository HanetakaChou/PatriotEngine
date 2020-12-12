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

#ifndef _MCRT_TASK_H_
#define _MCRT_TASK_H_ 1

#ifdef __cplusplus
extern "C"
{
#endif

#include "pt_common.h"
#include "pt_mcrt_common.h"

    // VK_DEFINE_HANDLE
    // VK_DEFINE_NON_DISPATCHABLE_HANDLE
    typedef struct mcrt_task_T *mcrt_task;

    // The tbb task has no destructor! //It's designed to store the lightweight data.

    PT_MCRT_ATTR mcrt_task PT_CALL mcrt_task_allocate_root(mcrt_task (*execute_callback)(char user_data[128]), void (*init_callback)(char user_data[128]));

    // Continuation Passing
    // https://software.intel.com/content/www/us/en/develop/documentation/tbb-documentation/top/intel-threading-building-blocks-developer-guide/the-task-scheduler/useful-task-techniques/continuation-passing.html
    PT_MCRT_ATTR mcrt_task PT_CALL mcrt_task_allocate_continuation(mcrt_task self, mcrt_task (*execute_callback)(char user_data[128]), void (*init_callback)(char user_data[128]));

    // Recycling
    // https://software.intel.com/content/www/us/en/develop/documentation/tbb-documentation/top/intel-threading-building-blocks-developer-guide/the-task-scheduler/useful-task-techniques/recycling.html
    PT_MCRT_ATTR void PT_CALL mcrt_task_recycle_as_child_of(mcrt_task self, mcrt_task successor);

    // Scheduler Bypass
    // https://software.intel.com/content/www/us/en/develop/documentation/tbb-documentation/top/intel-threading-building-blocks-developer-guide/the-task-scheduler/useful-task-techniques/scheduler-bypass.html

#ifdef __cplusplus
}
#endif

#endif