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
#include <pt_apple_sdk_posix_mach_metalkit.h>

#include <TargetConditionals.h>
#if TARGET_OS_IOS
#include <objc/message.h>
#include <objc/runtime.h>
#elif TARGET_OS_OSX
#include <objc/objc-runtime.h>
#else
#error Unknown Target
#endif

static inline MTKView MTKView_Wrap(struct objc_object *mtk_view)
{
    return reinterpret_cast<MTKView>(mtk_view);
}

static inline struct objc_object *MTKView_Unwrap(MTKView mtk_view)
{
    return reinterpret_cast<struct objc_object *>(mtk_view);
}

PT_ATTR_APPLE_SDK MTKView MTKView_alloc()
{
    struct objc_object *mtk_view = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        objc_getClass("MTKView"),
        sel_registerName("alloc"));
    return MTKView_Wrap(mtk_view);
}

PT_ATTR_APPLE_SDK MTKView MTKView_initWithFrame(MTKView mtk_view, CGRect frameRect, MTLDevice device)
{
    struct objc_object *ret_mtk_view = reinterpret_cast<struct objc_object *(*)(struct objc_object *, struct objc_selector *, CGRect, struct objc_object *)>(objc_msgSend)(
        MTKView_Unwrap(mtk_view),
        sel_registerName("initWithFrame:device:"),
        frameRect,
        reinterpret_cast<struct objc_object *>(device));
    return MTKView_Wrap(ret_mtk_view);
}