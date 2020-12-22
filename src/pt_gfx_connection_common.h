/*
 * Copyright (C) YuqiaoZhang張羽喬(HanetakaYuminaga弓長羽高)
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
#include "pt_gfx_texture_common.h"

class gfx_connection_proxy
{
    //channel buffer
};

class gfx_connection_common
{

    // proxy -> stub
    // https://docs.microsoft.com/en-us/windows/win32/com/proxy
    // https://docs.microsoft.com/en-us/windows/win32/com/stub

    // https://en.wikipedia.org/wiki/Distributed_object_communication
    // proxy/stub -> skeleton

    //Maybe we can do the cpu side work in the calling thread?
    struct texture_read_request
    {
        class gfx_texture_common *m_texture;
        char const *m_initial_filename;
        void *(PT_PTR *m_input_stream_init_callback)(char const *initial_filename);
        intptr_t(PT_PTR *m_input_stream_read_callback)(void *input_stream, void *buf, size_t count);
        void(PT_PTR *m_input_stream_destroy_callback)(void *input_stream);
    };

    //\[Hudson 2006\] Richard L. Hudson, Bratin Saha, Ali-Reza Adl-Tabatabai, Benjamin C. Hertzberg. "McRT-Malloc: a scalable transactional memory allocator". ISMM 2006.
    //freeListPush
    //repatriatePublicFreeList

    texture_read_request *m_public_texture_read_request;

public:
    static class gfx_connection_common *init(wsi_connection_ref wsi_connection, wsi_visual_ref wsi_visual);

    virtual void destroy() = 0;

    virtual class gfx_texture_common *create_texture() = 0;

    virtual void wsi_on_resized(wsi_window_ref wsi_window, float width, float height) = 0;

    virtual void wsi_on_redraw_needed_acquire(wsi_window_ref wsi_window, float width, float height) = 0; //frame throttling

    virtual void wsi_on_redraw_needed_release() = 0;
};

#endif
