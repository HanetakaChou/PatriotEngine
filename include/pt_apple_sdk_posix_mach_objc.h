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

#ifndef _PT_APPLE_SDK_POSIX_MACH_OBJC_H_
#define _PT_APPLE_SDK_POSIX_MACH_OBJC_H_ 1

#include "pt_apple_sdk_common.h"

/** @file objc/obc-internal.h */
PT_ATTR_APPLE_SDK void *AutoReleasePool_Push(void);
PT_ATTR_APPLE_SDK void AutoReleasePool_Pop(void *);

/** @file objc/NSObjCRuntime.h */
#if __LP64__ || 0 || NS_BUILD_32_LIKE_64
typedef long NSInteger;
typedef unsigned long NSUInteger;
#else
typedef int NSInteger;
typedef unsigned int NSUInteger;
#endif
typedef struct _Class_NSObject_T_ *Class_NSObject;
typedef struct _NSObject_T_ *NSObject;
PT_ATTR_APPLE_SDK bool Class_NSObject_addIvarVoidPointer(Class_NSObject class_ns_object, char const *ivarname);
PT_ATTR_APPLE_SDK NSObject NSObject_alloc(Class_NSObject);
PT_ATTR_APPLE_SDK NSObject NSObject_init(NSObject);
PT_ATTR_APPLE_SDK void NSObject_release(NSObject);

#endif