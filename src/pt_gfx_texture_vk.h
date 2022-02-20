//
// Copyright (C) YuqiaoZhang(HanetakaYuminaga)
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

#ifndef _PT_GFX_TEXTURE_VK_H_
#define _PT_GFX_TEXTURE_VK_H_ 1

#include <stddef.h>
#include <stdint.h>
#include "pt_gfx_texture_base.h"
#include "pt_gfx_connection_vk.h"
#include "pt_gfx_streaming_object_base.h"
#include "pt_gfx_frame_object_base.h"
#include <vulkan/vulkan.h>

class gfx_texture_vk final : public gfx_texture_base
{
    VkImage m_image;
    struct gfx_malloc_allocation_vk m_image_allocation;
    VkImageView m_image_view;

    bool texture_streaming_stage_first_pre_populate_task_data_callback(class gfx_connection_base* gfx_connection, struct gfx_texture_neutral_header_t const* neutral_header) override;
    void texture_streaming_stage_second_pre_calculate_total_size_callback(struct gfx_texture_neutral_header_t const* neutral_header, struct gfx_texture_backend_header_t* out_backend_header, uint32_t* out_subresource_num) override;
    size_t texture_streaming_stage_second_calculate_total_size_callback(class gfx_connection_base* gfx_connection, struct gfx_texture_backend_header_t const* backend_header, uint32_t subresource_num, struct gfx_texture_neutral_memcpy_dest_t* memcpy_dest, uint64_t base_offset) override;
    void* texture_streaming_stage_second_post_calculate_total_size_get_staging_buffer_pointer_callback(class gfx_connection_base* gfx_connection) override;
    uint32_t(*texture_streaming_stage_second_post_calculate_total_size_get_calculate_subresource_index_callback())(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) override;
    void texture_streaming_stage_second_post_calculate_total_size_callback(class gfx_connection_base* gfx_connection, uint32_t streaming_throttling_index, struct gfx_texture_backend_header_t const* backend_header, uint32_t subresource_num, struct gfx_texture_neutral_memcpy_dest_t* memcpy_dest) override;

    void pre_streaming_done_destroy_callback(class gfx_connection_base* gfx_connection) override;
    bool streaming_done_callback(class gfx_connection_base* gfx_connection) override;
    void post_stream_done_destroy_callback(class gfx_connection_base* gfx_connection) override;
    inline void unified_destory(class gfx_connection_vk* gfx_connection);

public:
    inline gfx_texture_vk() : gfx_texture_base(), m_image(VK_NULL_HANDLE), m_image_view(VK_NULL_HANDLE)
    {
    }

    inline VkImageView get_image_view() const
    {
        assert((!this->is_streaming_error()) && this->is_streaming_done());
        return this->m_image_view;
    }
};

#endif
