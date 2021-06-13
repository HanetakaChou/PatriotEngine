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

#ifndef _PT_APPLE_SDK_POSIX_MACH_FOUNDATION_H_
#define _PT_APPLE_SDK_POSIX_MACH_FOUNDATION_H_ 1

#include "pt_apple_sdk_common.h"
#include "pt_apple_sdk_posix_mach_objc.h"

typedef struct _Class_NSThreadDetachTarget_T_ *Class_NSThreadDetachTarget;
typedef struct _NSThreadDetachTarget_T_ *NSThreadDetachTarget;
typedef struct _NSThreadDetachSelector__T_ *NSThreadDetachSelector_;
PT_ATTR_APPLE_SDK Class_NSThreadDetachTarget NSThreadDetachTarget_allocateClass(char const *class_name, char const *selector_name, void (*_I_NSThreadDetachSelector_)(NSThreadDetachTarget, NSThreadDetachSelector_, void *argument));
PT_ATTR_APPLE_SDK NSThreadDetachTarget NSThreadDetachTarget_alloc(Class_NSThreadDetachTarget class_ns_thread_detach_target);
PT_ATTR_APPLE_SDK NSThreadDetachTarget NSThreadDetachTarget_init(NSThreadDetachTarget ns_thread_detach_target);
PT_ATTR_APPLE_SDK void NSThreadDetachTarget_release(NSThreadDetachTarget ns_thread_detach_target);
/** @file /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/Foundation.framework/Headers/NSThread.h */
PT_ATTR_APPLE_SDK void NSThread_detachNewThreadSelector(char const *selector_name, NSThreadDetachTarget target, void *argument);
PT_ATTR_APPLE_SDK bool NSThread_isMultiThreaded();

#endif