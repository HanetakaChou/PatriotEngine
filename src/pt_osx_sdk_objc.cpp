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

#include <pt_os_sdk_objc.h>

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
#include <string.h>

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

static inline struct objc_object *NSArray_Unwrap(NSArray ns_array)
{
    return reinterpret_cast<struct objc_object *>(ns_array);
}

static inline NSObject NSArray_To_NSObject(NSArray ns_array)
{
    return reinterpret_cast<NSObject>(ns_array);
}

static inline struct objc_object *NSString_Unwrap(NSString ns_string)
{
    return reinterpret_cast<struct objc_object *>(ns_string);
}

extern "C" void *objc_autoreleasePoolPush(void);
extern "C" void objc_autoreleasePoolPop(void *);

PT_ATTR_APPLE_SDK void *AutoReleasePool_Push(void)
{
    return objc_autoreleasePoolPush();
}

PT_ATTR_APPLE_SDK void AutoReleasePool_Pop(void *auto_release_pool_object)
{
    return objc_autoreleasePoolPop(auto_release_pool_object);
}

PT_ATTR_APPLE_SDK bool Class_NSObject_addIvarVoidPointer(Class_NSObject class_ns_object, char const *ivarname)
{
    BOOL res = class_addIvar(
        Class_NSObject_Unwrap(class_ns_object),
        ivarname,
        sizeof(void *),
        alignof(void *),
        "^v");
    return (res != NO) ? true : false;
}

PT_ATTR_APPLE_SDK NSObject NSObject_alloc(Class_NSObject class_ns_object)
{
    struct objc_object *ns_object = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        Class_NSObject_Unwrap(class_ns_object),
        sel_registerName("alloc"));
    return NSObject_Wrap(ns_object);
}

PT_ATTR_APPLE_SDK NSObject NSObject_init(NSObject ns_object)
{
    struct objc_object *ret_ns_object = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        NSObject_Unwrap(ns_object),
        sel_registerName("init"));
    return NSObject_Wrap(ret_ns_object);
}

PT_ATTR_APPLE_SDK void NSObject_release(NSObject ns_object)
{
    return reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        NSObject_Unwrap(ns_object),
        sel_registerName("release"));
}

PT_ATTR_APPLE_SDK void NSObject_setIvarVoidPointer(NSObject ns_object, char const *ivarname, void *pVoid)
{
    Ivar ivar_pUserData = class_getInstanceVariable(object_getClass(NSObject_Unwrap(ns_object)), ivarname);
    assert(ivar_pUserData != NULL);
    assert(strcmp(ivar_getTypeEncoding(ivar_pUserData), "^v") == 0);
    (*reinterpret_cast<void **>(reinterpret_cast<uintptr_t>(ns_object) + ivar_getOffset(ivar_pUserData))) = pVoid;
    return;
}

PT_ATTR_APPLE_SDK void *NSObject_getIvarVoidPointer(NSObject ns_object, char const *ivarname)
{
    Ivar ivar_pUserData = class_getInstanceVariable(object_getClass(NSObject_Unwrap(ns_object)), ivarname);
    assert(ivar_pUserData != NULL);
    assert(strcmp(ivar_getTypeEncoding(ivar_pUserData), "^v") == 0);
    return (*reinterpret_cast<void **>(reinterpret_cast<uintptr_t>(ns_object) + ivar_getOffset(ivar_pUserData)));
}

PT_ATTR_APPLE_SDK NSUInteger NSArray_count(NSArray ns_array)
{
    assert(sizeof(NSUInteger) == 1 || sizeof(NSUInteger) == 2 || sizeof(NSUInteger) == 4 || sizeof(NSUInteger) == 8);
    return reinterpret_cast<NSUInteger (*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        NSArray_Unwrap(ns_array),
        sel_registerName("count"));
}

PT_ATTR_APPLE_SDK NSObject NSArray_objectAtIndexedSubscript(NSArray ns_array, NSUInteger idx)
{
    struct objc_object *ret_ns_object = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *, NSUInteger)>(objc_msgSend)(
        NSArray_Unwrap(ns_array),
        sel_registerName("objectAtIndexedSubscript:"),
        idx);
    return NSObject_Wrap(ret_ns_object);
}

PT_ATTR_APPLE_SDK void NSArray_release(NSArray ns_array)
{
    return NSObject_release(NSArray_To_NSObject(ns_array));
}

PT_ATTR_APPLE_SDK char const *NSString_UTF8String(NSString ns_string)
{
    assert(sizeof(char const *) == 1 || sizeof(char const *) == 2 || sizeof(char const *) == 4 || sizeof(char const *) == 8);

    char const *utf8_string = reinterpret_cast<char const *(*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        NSString_Unwrap(ns_string),
        sel_registerName("UTF8String"));
    return utf8_string;
}
