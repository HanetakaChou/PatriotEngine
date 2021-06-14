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

#include "pt_gfx_api_mtl.h"
#include <pt_apple_sdk_posix_mach_metal.h>

bool gfx_api_mtl::init(wsi_window_ref wsi_window)
{
    m_device = NULL;
    {
        NSArray devices = MTLDevice_CopyAllDevices();
        NSUInteger count = NSArray_count(devices);
        for (NSUInteger idx = 0U; idx < count; ++idx)
        {
            MTLDevice device = NSObject_To_MTLDevice(NSArray_objectAtIndexedSubscript(devices, idx));
            bool has_unified_memory = MTLDevice_hasUnifiedMemory(device);
            if (!has_unified_memory)
            {
                m_device = device;
                break;
            }
        }
        NSArray_release(devices);
    }

    return true;
}