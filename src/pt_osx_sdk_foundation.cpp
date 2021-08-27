//
// Copyright (C) YuqiaoZhang(HanetakaYuminaga)
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#include <pt_osx_sdk_objc.h>
#include <pt_osx_sdk_foundation.h>

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

static inline NSFileManager NSFileManager_Wrap(struct objc_object *ns_file_manager)
{
    return reinterpret_cast<NSFileManager>(ns_file_manager);
}

static inline struct objc_object *NSFileManager_Unwrap(NSFileManager ns_file_manager)
{
    return reinterpret_cast<struct objc_object *>(ns_file_manager);
}

static inline NSArray_NSURL NSArray_NSURL_Wrap(struct objc_object *ns_array_ns_url)
{
    return reinterpret_cast<NSArray_NSURL>(ns_array_ns_url);
}

static inline NSArray NSArray_NSURL_To_NSArray(NSArray_NSURL ns_array_ns_url)
{
    return reinterpret_cast<NSArray>(ns_array_ns_url);
}

static inline struct objc_object *NSURL_Unwrap(NSURL ns_url)
{
    return reinterpret_cast<struct objc_object *>(ns_url);
}

static inline NSURL NSObject_To_NSURL(NSObject ns_url)
{
    return reinterpret_cast<NSURL>(ns_url);
}

static inline NSBundle NSBundle_Wrap(struct objc_object *ns_bundle)
{
    return reinterpret_cast<NSBundle>(ns_bundle);
}

static inline struct objc_object *NSBundle_Unwrap(NSBundle ns_bundle)
{
    return reinterpret_cast<struct objc_object *>(ns_bundle);
}

static inline NSString NSString_Wrap(struct objc_object *ns_string)
{
    return reinterpret_cast<NSString>(ns_string);
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

PT_ATTR_APPLE_SDK NSFileManager NSFileManager_defaultManager()
{
    struct objc_object *ret_ns_file_Manager = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        objc_getClass("NSFileManager"),
        sel_registerName("defaultManager"));
    return NSFileManager_Wrap(ret_ns_file_Manager);
}

PT_ATTR_APPLE_SDK NSArray_NSURL NSFileManager_URLsForDirectory(NSFileManager ns_file_manager, NSSearchPathDirectory in_domains, NSSearchPathDomainMask domain_mask)
{
    struct objc_object *ns_array_ns_url = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *, NSSearchPathDirectory, NSSearchPathDomainMask)>(objc_msgSend)(
        NSFileManager_Unwrap(ns_file_manager),
        sel_registerName("URLsForDirectory:inDomains:"),
        in_domains,
        domain_mask);

    return NSArray_NSURL_Wrap(ns_array_ns_url);
}

PT_ATTR_APPLE_SDK NSURL NSArray_NSURL_objectAtIndexedSubscript(NSArray_NSURL ns_array_ns_url, NSUInteger idx)
{
    return NSObject_To_NSURL(NSArray_objectAtIndexedSubscript(NSArray_NSURL_To_NSArray(ns_array_ns_url), idx));
}

PT_ATTR_APPLE_SDK char const *NSURL_fileSystemRepresentation(NSURL ns_url)
{
    assert(sizeof(char const *) == 1 || sizeof(char const *) == 2 || sizeof(char const *) == 4 || sizeof(char const *) == 8);

    char const *file_system_representation = reinterpret_cast<char const *(*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        NSURL_Unwrap(ns_url),
        sel_registerName("fileSystemRepresentation"));
    return file_system_representation;
}

PT_ATTR_APPLE_SDK NSBundle NSBundle_mainBundle()
{
    struct objc_object *ret_ns_bundle = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        objc_getClass("NSBundle"),
        sel_registerName("mainBundle"));
    return NSBundle_Wrap(ret_ns_bundle);
}

PT_ATTR_APPLE_SDK NSString NSBundle_resourcePath(NSBundle ns_bundle)
{
    struct objc_object *ns_resource_path = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        NSBundle_Unwrap(ns_bundle),
        sel_registerName("resourcePath"));
    return NSString_Wrap(ns_resource_path);
}
