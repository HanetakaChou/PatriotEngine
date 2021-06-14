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

#include <assert.h>

static inline MTKView MTKView_Wrap(struct objc_object *mtk_view)
{
    return reinterpret_cast<MTKView>(mtk_view);
}

static inline struct objc_object *MTKView_Unwrap(MTKView mtk_view)
{
    return reinterpret_cast<struct objc_object *>(mtk_view);
}

static inline Class_MTKViewDelegate Class_MTKViewDelegate_Wrap(Class class_mtk_view_delegate)
{
    return reinterpret_cast<Class_MTKViewDelegate>(class_mtk_view_delegate);
}

static inline Class Class_MTKViewDelegate_Unwrap(Class_MTKViewDelegate class_mtk_view_delegate)
{
    return reinterpret_cast<Class>(class_mtk_view_delegate);
}

static inline Class_NSObject Class_MTKViewDelegate_To_Class_NSObject(Class_MTKViewDelegate class_mtk_view_delegate)
{
    return reinterpret_cast<Class_NSObject>(class_mtk_view_delegate);
}

static inline NSObject MTKViewDelegate_To_NSObject(MTKViewDelegate mtk_view_delegate)
{
    return reinterpret_cast<NSObject>(mtk_view_delegate);
}

static inline MTKViewDelegate NSObject_To_MTKViewDelegate(NSObject mtk_view_delegate)
{
    return reinterpret_cast<MTKViewDelegate>(mtk_view_delegate);
}

static inline MTKViewDelegate MTKViewDelegate_Wrap(struct objc_object *mtk_view_delegate)
{
    return reinterpret_cast<MTKViewDelegate>(mtk_view_delegate);
}

static inline struct objc_object *MTKViewDelegate_Unwrap(MTKViewDelegate mtk_view_delegate)
{
    return reinterpret_cast<struct objc_object *>(mtk_view_delegate);
}

// ---
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

PT_ATTR_APPLE_SDK void MTKView_setDelegate(MTKView mtk_view, MTKViewDelegate delegate)
{
    return reinterpret_cast<void (*)(struct objc_object *, struct objc_selector *, struct objc_object *)>(objc_msgSend)(
        MTKView_Unwrap(mtk_view),
        sel_registerName("setDelegate:"),
        MTKViewDelegate_Unwrap(delegate));
}

PT_ATTR_APPLE_SDK Class_MTKViewDelegate MTKViewDelegate_allocClass(
    char const *class_name,
    void (*_I_MTKViewDelegate_drawableSizeWillChange_)(MTKViewDelegate mtk_view_delegate, MTKViewDelegate_drawSizeWillChange_, MTKView mtk_view, CGSize size),
    void (*_I_MTKViewDelegate_drawInMTKView_)(MTKViewDelegate mtk_view_delegate, MTKViewDelegate_drawInMTKView_, MTKView mtk_view))
{
    Class class_mtk_view_delegate = objc_allocateClassPair(
        objc_getClass("NSObject"),
        class_name,
        0);
    assert(class_mtk_view_delegate != NULL);

    BOOL result_drawable_size_will_change = class_addMethod(
        class_mtk_view_delegate,
        sel_registerName("mtkView:drawableSizeWillChange:"),
        reinterpret_cast<IMP>(_I_MTKViewDelegate_drawableSizeWillChange_),
        "v@:@{CGSize=dd}");
    assert(result_drawable_size_will_change != NO);

    BOOL result_draw_in_mtk_view = class_addMethod(
        class_mtk_view_delegate,
        sel_registerName("drawInMTKView:"),
        reinterpret_cast<IMP>(_I_MTKViewDelegate_drawInMTKView_),
        "v@:@");
    assert(result_draw_in_mtk_view != NO);

    return Class_MTKViewDelegate_Wrap(class_mtk_view_delegate);
}

PT_ATTR_APPLE_SDK bool Class_MTKViewDelegate_addIvarVoidPointer(Class_MTKViewDelegate class_mtk_view_delegate, char const *ivarname)
{
    return Class_NSObject_addIvarVoidPointer(Class_MTKViewDelegate_To_Class_NSObject(class_mtk_view_delegate), ivarname);
}

PT_ATTR_APPLE_SDK MTKViewDelegate MTKViewDelegate_alloc(Class_MTKViewDelegate class_mtk_view_delegate)
{
    struct objc_object *mtk_view_delegate = reinterpret_cast<struct objc_object *(*)(Class, struct objc_selector *)>(objc_msgSend)(
        Class_MTKViewDelegate_Unwrap(class_mtk_view_delegate),
        sel_registerName("alloc"));
    return MTKViewDelegate_Wrap(mtk_view_delegate);
}

PT_ATTR_APPLE_SDK MTKViewDelegate MTKViewDelegate_init(MTKViewDelegate mtk_view_delegate)
{
    return NSObject_To_MTKViewDelegate(NSObject_init(MTKViewDelegate_To_NSObject(mtk_view_delegate)));
}
