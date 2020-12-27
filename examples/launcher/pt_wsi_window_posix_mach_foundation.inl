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

#ifndef _PT_WSI_WINDOW_POSIX_MACH_FOUNDATION_INL_
#define _PT_WSI_WINDOW_POSIX_MACH_FOUNDATION_INL_ 1

#ifndef _PT_WSI_WINDOW_POSIX_MACH_FOUNDATION_H_
#error 1
#endif

#ifndef _PT_WSI_WINDOW_POSIX_MACH_OBJC_INL_
#error 1
#endif

static inline Class_NSThreadDetachTarget Class_NSThreadDetachTarget_Wrap(Class class_ns_thread_detach_target)
{
    return reinterpret_cast<Class_NSThreadDetachTarget>(class_ns_thread_detach_target);
}

static inline Class_NSObject Class_NSThreadDetachTarget_To_Class_NSObject(Class_NSThreadDetachTarget class_ns_thread_detach_target)
{
    return reinterpret_cast<Class_NSObject>(class_ns_thread_detach_target);
}

static inline NSThreadDetachTarget NSThreadDetachTarget_Wrap(struct objc_object *ns_thread_detach_target)
{
    return reinterpret_cast<NSThreadDetachTarget>(ns_thread_detach_target);
}

static inline struct objc_object *NSThreadDetachTarget_Unwrap(NSThreadDetachTarget ns_thread_detach_target)
{
    return reinterpret_cast<struct objc_object *>(ns_thread_detach_target);
}

static inline NSObject NSThreadDetachTarget_To_NSObject(NSThreadDetachTarget ns_thread_detach_target)
{
    return reinterpret_cast<NSObject>(ns_thread_detach_target);
}

static inline NSThreadDetachTarget NSObject_To_NSThreadDetachTarget(NSObject ns_thread_detach_target)
{
    return reinterpret_cast<NSThreadDetachTarget>(ns_thread_detach_target);
}

static inline Class_NSApplicationDelegate Class_NSApplicationDelegate_Wrap(Class class_ns_application_delegate)
{
    return reinterpret_cast<Class_NSApplicationDelegate>(class_ns_application_delegate);
}

static inline Class Class_NSThreadDetachTarget_Unwrap(Class_NSApplicationDelegate class_ns_application_delegate)
{
    return reinterpret_cast<Class>(class_ns_application_delegate);
}

#else
#error 1
#endif