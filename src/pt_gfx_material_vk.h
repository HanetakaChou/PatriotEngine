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

#ifndef _PT_GFX_MATERIAL_VK_H_
#define _PT_GFX_MATERIAL_VK_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_mcrt_task.h>
#include "pt_gfx_material_base.h"
#include "pt_gfx_connection_vk.h"
#include "pt_gfx_streaming_object_base.h"
#include "pt_gfx_frame_object_base.h"
#include <vulkan/vulkan.h>

class gfx_material_vk final : public gfx_material_base, public gfx_streaming_object_base, public gfx_frame_object_base
{
    uint32_t m_ref_count;

    VkDescriptorSet m_desciptor_set;

    class gfx_texture_vk *m_gfx_textures[GFX_MATERIAL_MAX_TEXTURE_COUNT];

public:
    void addref();

    void release(class gfx_connection_vk *gfx_connection);

    inline gfx_material_vk() : gfx_material_base(),
                               gfx_streaming_object_base(),
                               m_ref_count(1U),
                               m_desciptor_set(VK_NULL_HANDLE)
    {
        for (uint32_t texture_index = 0U; texture_index < GFX_MATERIAL_MAX_TEXTURE_COUNT; ++texture_index)
        {
            this->m_gfx_textures[texture_index] = NULL;
        }
    }
};

#endif