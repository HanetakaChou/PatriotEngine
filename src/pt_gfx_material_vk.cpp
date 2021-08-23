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
#include <stdint.h>
#include <pt_mcrt_intrin.h>
#include <pt_mcrt_atomic.h>
#include <pt_mcrt_malloc.h>
#include <pt_mcrt_log.h>
#include "pt_gfx_material_vk.h"
#include <assert.h>

void gfx_material_vk::streaming_destroy_callback(class gfx_connection_base *gfx_connection_base)
{
    class gfx_connection_vk *gfx_connection = static_cast<class gfx_connection_vk *>(gfx_connection_base);
    // first stage parallel // no desciptor_set
    // third stage serial  // no desciptor_set
    this->unified_destory(gfx_connection);
}

bool gfx_material_vk::streaming_done_callback(class gfx_connection_base *gfx_connection_base)
{
    class gfx_connection_vk *gfx_connection = static_cast<class gfx_connection_vk *>(gfx_connection_base);
    // third stage serial
    bool res_allocate_frame_object_descriptor_set = gfx_connection->allocate_descriptor_set(&this->m_desciptor_set);
    if (res_allocate_frame_object_descriptor_set)
    {
        gfx_connection->init_descriptor_set(this->m_desciptor_set, this->m_texture_count, this->m_gfx_textures);
    }

    return res_allocate_frame_object_descriptor_set;
}

void gfx_material_vk::frame_destroy_callback(class gfx_connection_base *gfx_connection_base)
{
    class gfx_connection_vk *gfx_connection = static_cast<class gfx_connection_vk *>(gfx_connection_base);
    // this function is serial
    gfx_connection->free_descriptor_set(this->m_desciptor_set);
    this->m_desciptor_set = VK_NULL_HANDLE;

    this->unified_destory(gfx_connection);
}

inline void gfx_material_vk::unified_destory(class gfx_connection_vk *gfx_connection)
{
    assert(VK_NULL_HANDLE == this->m_desciptor_set);

    for (uint32_t texture_index = 0U; texture_index < this->m_texture_count; ++texture_index)
    {
        this->m_gfx_textures[texture_index]->release(gfx_connection);
    }

    this->~gfx_material_vk();
    mcrt_aligned_free(this);
}