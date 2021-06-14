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

#ifndef _PT_APPLE_SDK_POSIX_MACH_METAL_H_
#define _PT_APPLE_SDK_POSIX_MACH_METAL_H_ 1

#include <stddef.h>
#include <stdint.h>
#include "pt_apple_sdk_common.h"
#include "pt_apple_sdk_posix_mach_objc.h"

typedef struct _MTLDevice_T_ *MTLDevice;

#ifdef __cplusplus
extern "C"
{
#endif
    PT_ATTR_APPLE_SDK MTLDevice MTLDevice_CreateSystemDefaultDevice(void);
    PT_ATTR_APPLE_SDK NSArray MTLDevice_CopyAllDevices(void);
    PT_ATTR_APPLE_SDK MTLDevice NSObject_To_MTLDevice(NSObject mtl_device);
    PT_ATTR_APPLE_SDK bool MTLDevice_hasUnifiedMemory(MTLDevice mtl_device);
#ifdef __cplusplus
}
#endif

#endif