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

#include <pt_apple_sdk_posix_mach_objc.h>
#include <pt_apple_sdk_posix_mach_foundation.h>

#include <TargetConditionals.h>
#if TARGET_OS_IOS
#include <objc/message.h>
#include <objc/runtime.h>
#elif TARGET_OS_OSX
#include <objc/objc-runtime.h>
#else
#error Unknown Target
#endif

#include <assert.h>

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

PT_ATTR_APPLE_SDK Class_NSThreadDetachTarget NSThreadDetachTarget_allocateClass(char const *class_name, char const *selector_name, void (*_I_NSThreadDetachSelector_)(NSThreadDetachTarget, NSThreadDetachSelector_, void *argument))
{
    Class class_ns_thread_detach_target = objc_allocateClassPair(
        objc_getClass("NSObject"),
        class_name,
        0);
    assert(class_ns_thread_detach_target != NULL);

    BOOL res = class_addMethod(
        class_ns_thread_detach_target,
        sel_registerName(selector_name),
        reinterpret_cast<IMP>(_I_NSThreadDetachSelector_),
        "v@:@");
    assert(res != NO);

    return Class_NSThreadDetachTarget_Wrap(class_ns_thread_detach_target);
}

PT_ATTR_APPLE_SDK NSThreadDetachTarget NSThreadDetachTarget_alloc(Class_NSThreadDetachTarget class_ns_thread_detach_target)
{
    return NSObject_To_NSThreadDetachTarget(NSObject_alloc(Class_NSThreadDetachTarget_To_Class_NSObject(class_ns_thread_detach_target)));
}

PT_ATTR_APPLE_SDK NSThreadDetachTarget NSThreadDetachTarget_init(NSThreadDetachTarget ns_thread_detach_target)
{
    return NSObject_To_NSThreadDetachTarget(NSObject_init(NSThreadDetachTarget_To_NSObject(ns_thread_detach_target)));
}

PT_ATTR_APPLE_SDK void NSThreadDetachTarget_release(NSThreadDetachTarget ns_thread_detach_target)
{
    return NSObject_release(NSThreadDetachTarget_To_NSObject(ns_thread_detach_target));
}

PT_ATTR_APPLE_SDK void NSThread_detachNewThreadSelector(char const *selector_name, NSThreadDetachTarget target, void *argument)
{
    return reinterpret_cast<void (*)(Class, struct objc_selector *, struct objc_selector *, struct objc_object *, void * /* struct objc_object * */)>(objc_msgSend)(
        objc_getClass("NSThread"),
        sel_registerName("detachNewThreadSelector:toTarget:withObject:"),
        sel_registerName(selector_name),
        NSThreadDetachTarget_Unwrap(target),
        argument);
}

PT_ATTR_APPLE_SDK bool NSThread_isMultiThreaded()
{
    BOOL is_multi_threaded = reinterpret_cast<BOOL (*)(Class, struct objc_selector *)>(objc_msgSend)(
        objc_getClass("NSThread"),
        sel_registerName("isMultiThreaded"));
    return (is_multi_threaded != NO) ? true : false;
}