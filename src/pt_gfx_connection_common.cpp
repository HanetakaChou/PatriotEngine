/*
 * Copyright (C) YuqiaoZhang
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
#include "pt_gfx_connection_d3d12.h"
#include "pt_gfx_connection_mtl.h"
#include "pt_gfx_connection_vk.h"

class gfx_connection_common *gfx_connection_common::init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual)
{
    class gfx_connection_common *gfx_connection;

    gfx_connection = gfx_connection_d3d12::init(wsi_connection, wsi_visual);
    if (NULL != gfx_connection)
    {
        return gfx_connection;
    }

    gfx_connection = gfx_connection_mtl::init(wsi_connection, wsi_visual);
    if (NULL != gfx_connection)
    {
        return gfx_connection;
    }

    gfx_connection = gfx_connection_vk::init(wsi_connection, wsi_visual);
    if (NULL != gfx_connection)
    {
        return gfx_connection;
    }

    return NULL;
}