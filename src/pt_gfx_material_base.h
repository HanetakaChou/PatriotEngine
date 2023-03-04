//
// Copyright (C) YuqiaoZhang(HanetakaChou)
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#ifndef _PT_GFX_MATERIAL_BASE_H_
#define _PT_GFX_MATERIAL_BASE_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_gfx_connection.h>
#include "imaging/texture.h"
#include "imaging/streaming_object.h"
#include <assert.h>

class gfx_material_base : public gfx_streaming_object_base, private gfx_frame_object_base
{
    uint32_t m_ref_count;
    uint32_t m_texture_count;
    class gfx_texture_base *m_gfx_textures[GFX_MATERIAL_MAX_TEXTURE_COUNT];

    static mcrt_task_ref material_streaming_stage_second_task_execute(mcrt_task_ref self);

protected:
    inline uint32_t get_texture_count() const { return this->m_texture_count; }
    inline class gfx_texture_base const *const *get_textures() const { return &this->m_gfx_textures[0]; }

    void unified_destory_execute(class gfx_connection_base *gfx_connection);

    bool load_header_callback(
        pt_input_stream_ref input_stream,
        pt_input_stream_read_callback input_stream_read_callback,
        pt_input_stream_seek_callback input_stream_seek_callback,
        class gfx_connection_base* connection,
        size_t* out_memcpy_dests_size,
        size_t* out_memcpy_dests_align) final 
    { 
        return false;
    }

    // load - second stage
    size_t calculate_staging_buffer_total_size_callback(
        size_t base_offset,
        class gfx_connection_base* connection,
        void* out_memcpy_dests) final 
    {
        return -1;
    }

    bool load_data_callback(
        pt_input_stream_ref input_stream,
        pt_input_stream_read_callback input_stream_read_callback,
        pt_input_stream_seek_callback input_stream_seek_callback,
        class gfx_connection_base* connection,
        void const* memcpy_dests,
        uint32_t streaming_throttling_index) final
    {
        return false;
    }

    inline gfx_material_base() : gfx_streaming_object_base(), gfx_frame_object_base(), m_ref_count(1U), m_texture_count(-1)
    {
        for (uint32_t texture_index = 0U; texture_index < GFX_MATERIAL_MAX_TEXTURE_COUNT; ++texture_index)
        {
            this->m_gfx_textures[texture_index] = NULL;
        }
    }

    inline ~gfx_material_base()
    {
        for (uint32_t texture_index = 0U; texture_index < GFX_MATERIAL_MAX_TEXTURE_COUNT; ++texture_index)
        {
            assert(NULL == this->m_gfx_textures[texture_index]);
        }
    }

public:
    bool init_with_texture(class gfx_connection_base *gfx_connection, uint32_t material_model, uint32_t texture_count, class gfx_texture_base **gfx_textures);

    void destroy(class gfx_connection_base *gfx_connection);

    void addref();

    void release(class gfx_connection_base *gfx_connection);
};

#endif