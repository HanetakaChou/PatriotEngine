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

#ifndef _PT_GFX_BUFFER_BASE_H_
#define _PT_GFX_BUFFER_BASE_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_gfx_connection.h>
#include "pt_gfx_streaming_object.h"

class gfx_buffer_base : public gfx_streaming_object
{

protected:
    inline gfx_buffer_base() : gfx_streaming_object() {}

public:
    virtual bool read_vertex_input_stream(class gfx_connection_base *, gfx_buffer_ref buffer, int64_t input_stream_offset, int64_t input_stream_length, char const *initial_filename, gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename), intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence), void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream)) = 0;
    
    virtual bool read_index_input_stream(class gfx_connection_base *, gfx_buffer_ref buffer, int64_t input_stream_offset, int64_t input_stream_length, char const *initial_filename, gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename), intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence), void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream)) = 0;

    virtual void destroy(class gfx_connection_base *gfx_connection) = 0;
};

#endif