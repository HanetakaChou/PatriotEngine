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

#include <stddef.h>
#include <pt_gfx_connection.h>
#include "pt_gfx_connection_common.h"

#if defined(PT_WIN32)
#include "pt_gfx_connection_d3d12.h"
#endif

#if defined(PT_POSIX_MACH)
#include "pt_gfx_connection_mtl.h"
#endif

#if defined(PT_POSIX_LINUX) || defined(PT_WIN32)
#include "pt_gfx_connection_vk.h"
#endif

class gfx_connection_common *gfx_connection_common_init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual, wsi_window_ref wsi_window)
{
    class gfx_connection_common *gfx_connection = NULL;

#if defined(PT_WIN32)
    if (NULL == gfx_connection)
    {
        gfx_connection = gfx_connection_d3d12_init(wsi_connection, wsi_visual);
    }
#endif

#if defined(PT_POSIX_MACH)
    if (NULL == gfx_connection)
    {
        gfx_connection = gfx_connection_mtl_init(wsi_window);
    }
#endif

#if defined(PT_POSIX_LINUX) || defined(PT_WIN32)
    if (NULL == gfx_connection)
    {
        gfx_connection = gfx_connection_vk_init(wsi_connection, wsi_visual);
    }
#endif

    return gfx_connection;
}
