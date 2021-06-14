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
#include "pt_gfx_connection_mtl.h"

inline gfx_connection_mtl::gfx_connection_mtl()
{
}

inline gfx_connection_mtl::~gfx_connection_mtl()
{
}

bool gfx_connection_mtl::init(wsi_window_ref wsi_window)
{
    return m_api_mtl.init(wsi_window);
}

void gfx_connection_mtl::destroy()
{
}

class gfx_texture_common *gfx_connection_mtl::create_texture()
{
    //gfx_texture_vk *texture = new (mcrt_aligned_malloc(sizeof(gfx_texture_vk), alignof(gfx_texture_vk))) gfx_texture_vk(this);
    //return texture;
    return NULL;
}

void gfx_connection_mtl::wsi_on_resized(wsi_window_ref wsi_window, float width, float height)
{
}

void gfx_connection_mtl::wsi_on_redraw_needed_acquire(wsi_window_ref wsi_window, float width, float height)
{
}

void gfx_connection_mtl::wsi_on_redraw_needed_release()
{
}

class gfx_connection_mtl *gfx_connection_mtl_init(wsi_window_ref wsi_window)
{
    //class gfx_connection_mtl *connection = new (mcrt_aligned_malloc(sizeof(gfx_connection_mtl), alignof(gfx_connection_mtl))) gfx_connection_mtl();
    //if (connection->init(wsi_window))
    //{
    //    return connection;
    //}
    //else
    //{
    //    connection->destroy();
    //    return NULL;
    //}
    return NULL;
}
