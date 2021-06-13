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

#ifndef _PT_APPLE_SDK_POSIX_MACH_METALKIT_H_
#define _PT_APPLE_SDK_POSIX_MACH_METALKIT_H_ 1

#include <stddef.h>
#include <stdint.h>
#include "pt_apple_sdk_common.h"
#include "pt_apple_sdk_posix_mach_foundation.h"
#include "pt_apple_sdk_posix_mach_metal.h"

typedef struct _MTKView_T_ *MTKView;

#ifdef __cplusplus
extern "C"
{
#endif
    PT_ATTR_APPLE_SDK MTKView MTKView_alloc();
    PT_ATTR_APPLE_SDK MTKView MTKView_initWithFrame(MTKView mtk_view, CGRect frameRect, MTLDevice device);
#ifdef __cplusplus
}
#endif

typedef struct _Class_MTKViewDelegate_T_ *Class_MTKViewDelegate;
typedef struct _MTKViewDelegate_T_ *MTKViewDelegate;
typedef struct _MTKViewDelegate_drawableSizeWillChange__T_ *MTKViewDelegate_drawSizeWillChange_;
typedef struct _MTKViewDelegate_drawInMTKView__T_ *MTKViewDelegate_drawInMTKView_;

#ifdef __cplusplus
extern "C"
{
#endif
    PT_ATTR_APPLE_SDK Class_MTKViewDelegate *MTKViewDelegate_allocClass(
        char const *class_name,
        void (*_I_MTKViewDelegate_drawableSizeWillChange_)(MTKViewDelegate, MTKViewDelegate_drawSizeWillChange_, MTKView, CGSize size),
        void (*_I_MTKViewDelegate_drawInMTKView_)(MTKViewDelegate, MTKViewDelegate_drawInMTKView_, MTKView));
    PT_ATTR_APPLE_SDK bool Class_MTKViewDelegate_addIvarVoidPointer(Class_MTKViewDelegate class_mtk_view_delegate, char const *ivarname);
    PT_ATTR_APPLE_SDK MTKViewDelegate MTKViewDelegate_alloc(Class_MTKViewDelegate class_mtk_view_delegate);
    PT_ATTR_APPLE_SDK MTKViewDelegate MTKViewDelegate_init(MTKViewDelegate mtk_view_delegate);
#ifdef __cplusplus
}
#endif

#endif
