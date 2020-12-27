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

#ifndef _PT_WSI_WINDOW_POSIX_MACH_FOUNDATION_H_
#define _PT_WSI_WINDOW_POSIX_MACH_FOUNDATION_H_ 1

#include "pt_wsi_window_posix_mach_objc.h"

typedef struct _Class_NSThreadDetachTarget_T_ *Class_NSThreadDetachTarget;
typedef struct _NSThreadDetachTarget_T_ *NSThreadDetachTarget;
typedef struct _NSThreadDetachSelector__T_ *NSThreadDetachSelector_;
static inline Class_NSThreadDetachTarget NSThreadDetachTarget_allocateClass(char const *class_name, char const *selector_name, void (*_I_NSThreadDetachSelector_)(NSThreadDetachTarget, NSThreadDetachSelector_, void *argument));
static inline NSThreadDetachTarget NSThreadDetachTarget_alloc(Class_NSThreadDetachTarget class_ns_thread_detach_target);
static inline NSThreadDetachTarget NSThreadDetachTarget_init(NSThreadDetachTarget ns_thread_detach_target);
static inline void NSThreadDetachTarget_release(NSThreadDetachTarget ns_thread_detach_target);
/** @file /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/Foundation.framework/Headers/NSThread.h */
static inline void NSThread_detachNewThreadSelector(char const *selector_name, NSThreadDetachTarget target, void *argument);
static inline bool NSThread_isMultiThreaded();

#endif