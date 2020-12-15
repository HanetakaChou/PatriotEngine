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

PT_GFX_ATTR gfx_iconnection *PT_CALL gfx_connection_init(struct wsi_iwindow *window)
{
    gfx_iconnection *connection;

    connection = gfx_connection_d3d12_init(window);
    if (NULL != connection)
    {
        return connection;
    }

    connection = gfx_connection_mtl_init(window);
    if (NULL != connection)
    {
        return connection;
    }

    connection = gfx_connection_vk_init(window);
    if (NULL != connection)
    {
        return connection;
    }

    return NULL;
}
