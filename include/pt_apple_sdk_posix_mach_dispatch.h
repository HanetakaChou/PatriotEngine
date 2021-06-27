/*
 * Copyright (C) YuqiaoZhang(HanetakaYuminaga)
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

#ifndef _PT_APPLE_SDK_POSIX_MACH_DISPATCH_H_
#define _PT_APPLE_SDK_POSIX_MACH_DISPATCH_H_ 1

#include <stddef.h>
#include <stdint.h>
#include "pt_apple_sdk_common.h"

typedef struct _DISPATCHQUEUE_T_ *dispatch_queue_t;
typedef struct _DISPATCHSOURCETYPE_T_ *dispatch_source_type_t;
typedef struct _DISPATCHSOURCE_T_ *dispatch_source_t;

#ifdef __cplusplus
extern "C"
{
#endif
    PT_ATTR_APPLE_SDK dispatch_queue_t dispatch_get_main_queue(void);
    PT_ATTR_APPLE_SDK dispatch_source_type_t __attribute__((availability(macos, introduced = 10.6), availability(ios, introduced = 4.0))) dispatch_get_source_type_data_add();
    PT_ATTR_APPLE_SDK dispatch_source_t __attribute__((availability(macos, introduced = 10.6), availability(ios, introduced = 4.0))) dispatch_create_source(dispatch_source_type_t type, uintptr_t handle, uintptr_t mask, dispatch_queue_t queue);
    PT_ATTR_APPLE_SDK void __attribute__((availability(macos, introduced = 10.6), availability(ios, introduced = 4.0))) dispatch_set_source_event_handler(dispatch_source_t source, void (*pfnCallback)(void *pUserData), void *pUserData);
    PT_ATTR_APPLE_SDK void __attribute__((availability(macos, introduced = 10.6), availability(ios, introduced = 4.0))) dispatch_resume_source(dispatch_source_t source);
    PT_ATTR_APPLE_SDK void __attribute__((availability(macos, introduced = 10.6), availability(ios, introduced = 4.0))) dispatch_merge_source_data(dispatch_source_t source, uintptr_t value);
#ifdef __cplusplus
}
#endif

#define DISPATCH_SOURCE_TYPE_DATA_ADD dispatch_get_source_type_data_add()

#endif
