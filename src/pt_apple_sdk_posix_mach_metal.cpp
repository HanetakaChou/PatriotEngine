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
#include <pt_apple_sdk_posix_mach_metal.h>

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

static inline MTLDevice MTLDevice_Wrap(struct objc_object *mtl_device)
{
    return reinterpret_cast<MTLDevice>(mtl_device);
}

static inline struct objc_object *MTLDevice_Unwrap(MTLDevice mtl_device)
{
    return reinterpret_cast<struct objc_object *>(mtl_device);
}

static inline NSArray NSArray_Wrap(struct objc_object *ns_array)
{
    return reinterpret_cast<NSArray>(ns_array);
}

extern "C" struct objc_object *MTLCreateSystemDefaultDevice(void);

PT_ATTR_APPLE_SDK MTLDevice MTLDevice_CreateSystemDefaultDevice(void)
{
    return MTLDevice_Wrap(MTLCreateSystemDefaultDevice());
}

extern "C" struct objc_object *MTLCopyAllDevices(void);

PT_ATTR_APPLE_SDK NSArray MTLDevice_CopyAllDevices(void)
{
    return NSArray_Wrap(MTLCopyAllDevices());
}

PT_ATTR_APPLE_SDK MTLDevice NSObject_To_MTLDevice(NSObject mtl_device)
{
    return reinterpret_cast<MTLDevice>(mtl_device);
}

PT_ATTR_APPLE_SDK bool MTLDevice_hasUnifiedMemory(MTLDevice mtl_device)
{
    assert(sizeof(BOOL) == 1 || sizeof(BOOL) == 2 || sizeof(BOOL) == 4 || sizeof(BOOL) == 8);
    BOOL ret_has_unified_memory = reinterpret_cast<BOOL (*)(struct objc_object *, struct objc_selector *)>(objc_msgSend)(
        MTLDevice_Unwrap(mtl_device),
        sel_registerName("hasUnifiedMemory"));
    return (ret_has_unified_memory != NO) ? true : false;
}