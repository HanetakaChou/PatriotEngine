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

#ifndef _PT_GFX_TEXTURE_COMMON_H_
#define _PT_GFX_TEXTURE_COMMON_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_gfx_connection.h>
#include "pt_gfx_streaming_object_base.h"

class gfx_texture_base : public gfx_streaming_object_base, public gfx_frame_object_base
{
    uint32_t m_ref_count;
    virtual bool texture_streaming_stage_first_pre_populate_task_data_callback(class gfx_connection_base *gfx_connection, struct common_header_t const *neutral_header) = 0;
    virtual bool texture_streaming_stage_second_pre_calculate_total_size_callback(struct common_header_t const *neutral_header, void **memcpy_dest, void **cmdcopy_dest) = 0;
    virtual size_t texture_streaming_stage_second_calculate_total_size_callback(class gfx_connection_base *gfx_connection, struct common_header_t const *neutral_header, void *memcpy_dest, void *cmdcopy_dest, uint64_t base_offset) = 0;
    virtual void texture_streaming_stage_second_post_calculate_total_size_fail_callback(void *memcpy_dest, void *cmdcopy_dest) = 0;
    virtual bool texture_streaming_stage_second_post_calculate_total_size_success_callback(class gfx_connection_base *gfx_connection, uint32_t streaming_throttling_index, struct common_header_t const *neutral_header, size_t const *neutral_data_offset, void *memcpy_dest, void *cmdcopy_dest, gfx_input_stream_ref gfx_input_stream, intptr_t(PT_PTR *gfx_input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *gfx_input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence)) = 0;
    static mcrt_task_ref texture_streaming_stage_second_task_execute(mcrt_task_ref self);

protected:
    inline gfx_texture_base() : gfx_streaming_object_base(), gfx_frame_object_base(), m_ref_count(1U) {}

public:
    void destroy(class gfx_connection_base *gfx_connection);

    void addref();

    void release(class gfx_connection_base *gfx_connection);

    bool read_input_stream(
        class gfx_connection_base *gfx_connection,
        char const *initial_filename,
        gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename),
        intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count),
        int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence),
        void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream));
};

#endif
