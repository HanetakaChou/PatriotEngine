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

#ifndef _PT_GFX_CONNECTION_COMMON_H_
#define _PT_GFX_CONNECTION_COMMON_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_gfx_connection.h>

class gfx_connection_proxy
{
    //channel buffer
};

class gfx_connection_base
{

    // proxy -> stub
    // https://docs.microsoft.com/en-us/windows/win32/com/proxy
    // https://docs.microsoft.com/en-us/windows/win32/com/stub

    // https://en.wikipedia.org/wiki/Distributed_object_communication
    // proxy/stub -> skeleton

    //\[Hudson 2006\] Richard L. Hudson, Bratin Saha, Ali-Reza Adl-Tabatabai, Benjamin C. Hertzberg. "McRT-Malloc: a scalable transactional memory allocator". ISMM 2006.
    //freeListPush
    //repatriatePublicFreeList

public:
    virtual void destroy() = 0;

    virtual class gfx_buffer_base *create_buffer() = 0;

    virtual class gfx_node_base *create_node() = 0;

    virtual class gfx_mesh_base *create_mesh() = 0;

    virtual class gfx_texture_base *create_texture() = 0;

    virtual void on_wsi_resized(float width, float height) = 0;

    virtual void on_wsi_redraw_needed_acquire() = 0; //frame throttling

    virtual void on_wsi_redraw_needed_release() = 0;
};

class gfx_connection_base *gfx_connection_common_init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual, wsi_window_ref wsi_window);

#endif
