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
    virtual bool texture_streaming_stage_first_pre_populate_task_data_callback(class gfx_connection_base *gfx_connection, struct gfx_texture_neutral_header_t const *neutral_header) = 0;

    virtual void texture_streaming_stage_second_pre_calculate_total_size_callback(struct gfx_texture_neutral_header_t const *neutral_header, struct gfx_texture_backend_header_t *out_backend_header, uint32_t *out_subresource_num) = 0;
    virtual size_t texture_streaming_stage_second_calculate_total_size_callback(class gfx_connection_base *gfx_connection, struct gfx_texture_backend_header_t const *backend_header, uint32_t subresource_num, struct gfx_texture_neutral_memcpy_dest_t *memcpy_dest, uint64_t base_offset) = 0;
    virtual void *texture_streaming_stage_second_get_staging_buffer_pointer_callback(class gfx_connection_base *gfx_connection) = 0;
    virtual uint32_t (*texture_streaming_stage_second_get_calculate_subresource_index_callback())(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) = 0;
    virtual void texture_streaming_stage_second_post_calculate_total_size_callback(class gfx_connection_base *gfx_connection, uint32_t streaming_throttling_index, struct gfx_texture_backend_header_t const *backend_header, uint32_t subresource_num, struct gfx_texture_neutral_memcpy_dest_t *memcpy_dest) = 0;

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
        gfx_input_stream_ref(PT_PTR *gfx_input_stream_init_callback)(char const *),
        intptr_t(PT_PTR *gfx_input_stream_read_callback)(gfx_input_stream_ref, void *, size_t),
        int64_t(PT_PTR *gfx_input_stream_seek_callback)(gfx_input_stream_ref, int64_t, int),
        void(PT_PTR *gfx_input_stream_destroy_callback)(gfx_input_stream_ref));
};

#endif
