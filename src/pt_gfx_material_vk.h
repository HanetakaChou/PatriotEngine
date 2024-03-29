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

#ifndef _PT_GFX_MATERIAL_VK_H_
#define _PT_GFX_MATERIAL_VK_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_mcrt_task.h>
#include "pt_gfx_material_base.h"
#include "pt_gfx_connection_vk.h"
#include "imaging/vk/vk_texture.h"
#include "pt_gfx_frame_object_base.h"
#include <vulkan/vulkan.h>

class gfx_material_vk final : public gfx_material_base
{
    VkDescriptorSet m_desciptor_set;

    bool streaming_done_callback(class gfx_connection_base *gfx_connection) override;

    void pre_streaming_done_destroy_callback(class gfx_connection_base *gfx_connection) override;

    void post_stream_done_destroy_callback(class gfx_connection_base *gfx_connection) override;

    inline void unified_destory_execute(class gfx_connection_vk *gfx_connection);

public:
    inline gfx_material_vk() : gfx_material_base(),
                               m_desciptor_set(VK_NULL_HANDLE)
    {
    }

    inline VkDescriptorSet get_descriptor_set() const
    {
        assert((!this->is_streaming_error()) && this->is_streaming_done());
        return this->m_desciptor_set;
    }
};

#endif