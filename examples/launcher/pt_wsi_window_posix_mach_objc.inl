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

#ifndef _PT_WSI_WINDOW_POSIX_MACH_OBJC_INL_
#define _PT_WSI_WINDOW_POSIX_MACH_OBJC_INL_ 1

#ifndef _PT_WSI_WINDOW_POSIX_MACH_OBJC_H_
#error 1
#endif

#include <TargetConditionals.h>
#if TARGET_OS_IOS
#include <objc/message.h>
#include <objc/runtime.h>
#elif TARGET_OS_OSX
#include <objc/objc-runtime.h>
#else
#error Unknown Target
#endif

static inline Class Class_NSObject_Unwrap(Class_NSObject class_ns_object)
{
    return reinterpret_cast<Class>(class_ns_object);
}

static inline NSObject NSObject_Wrap(struct objc_object *ns_object)
{
    return reinterpret_cast<NSObject>(ns_object);
}

static inline struct objc_object *NSObject_Unwrap(NSObject ns_object)
{
    return reinterpret_cast<struct objc_object *>(ns_object);
}

#else
#error 1
#endif