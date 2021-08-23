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

#ifndef _PT_GFX_MATERIAL_BASE_H_
#define _PT_GFX_MATERIAL_BASE_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_gfx_connection.h>
#include "pt_gfx_texture_base.h"
#include "pt_gfx_streaming_object_base.h"

class gfx_material_base : public gfx_streaming_object_base, public gfx_frame_object_base
{
    uint32_t m_ref_count;

protected:
    uint32_t m_texture_count;
    class gfx_texture_base *m_gfx_textures[GFX_MATERIAL_MAX_TEXTURE_COUNT];

private:
    struct material_streaming_stage_second_task_data_t
    {
        class gfx_material_base *m_gfx_streaming_object;
        class gfx_connection_base *m_gfx_connection;
    };
    static_assert(sizeof(struct material_streaming_stage_second_task_data_t) <= sizeof(struct mcrt_task_user_data_t), "");

    static mcrt_task_ref material_streaming_stage_second_task_execute(mcrt_task_ref self);

protected:
    inline gfx_material_base() : gfx_streaming_object_base(), gfx_frame_object_base(), m_ref_count(1U), m_texture_count(-1)
    {
        for (uint32_t texture_index = 0U; texture_index < GFX_MATERIAL_MAX_TEXTURE_COUNT; ++texture_index)
        {
            this->m_gfx_textures[texture_index] = NULL;
        }
    }

public:
    bool init_with_texture(class gfx_connection_base *gfx_connection, uint32_t material_model, uint32_t texture_count, class gfx_texture_base **gfx_textures);

    void destroy(class gfx_connection_base *gfx_connection);

    void addref();

    void release(class gfx_connection_base *gfx_connection);
};

#endif