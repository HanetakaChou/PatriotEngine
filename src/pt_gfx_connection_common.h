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

#ifndef _PT_GFX_CONNECTION_COMMON_H_
#define _PT_GFX_CONNECTION_COMMON_H_ 1

#include <stddef.h>
#include <stdint.h>

class gfx_connection_common : public gfx_iconnection
{
    typedef struct _input_stream_t_ *input_stream;

    //Maybe we can do the cpu side work in the calling thread?
    struct texture_read_request
    {
        gfx_itexture *m_texture;
        char const *m_initial_filename;
        input_stream(PT_PTR *m_input_stream_init_callback)(char const *initial_filename);
        intptr_t(PT_PTR *m_input_stream_read_callback)(input_stream input_stream, void *buf, size_t count);
        void(PT_PTR *m_input_stream_destroy_callback)(input_stream input_stream);
    };

    //\[Hudson 2006\] Richard L. Hudson, Bratin Saha, Ali-Reza Adl-Tabatabai, Benjamin C. Hertzberg. "McRT-Malloc: a scalable transactional memory allocator". ISMM 2006.
    //freeListPush
    //repatriatePublicFreeList

    texture_read_request *m_public_texture_read_request;
};

#endif
