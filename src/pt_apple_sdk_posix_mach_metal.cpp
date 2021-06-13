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

static inline MTLDevice MTLDevice_Wrap(struct objc_object *mtl_device)
{
    return reinterpret_cast<MTLDevice>(mtl_device);
}

static inline struct objc_object *MTLDevice_Unwrap(MTLDevice mtl_device)
{
    return reinterpret_cast<struct objc_object *>(mtl_device);
}

extern "C" struct objc_object *MTLCreateSystemDefaultDevice(void);

PT_ATTR_APPLE_SDK MTLDevice MTLDevice_CreateSystemDefault(void)
{
    return MTLDevice_Wrap(MTLCreateSystemDefaultDevice());
}
