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

#ifndef _IMAGING_VK_TEXTURE_H_
#define _IMAGING_VK_TEXTURE_H_ 1

#include <stddef.h>
#include <stdint.h>
#include "../texture.h"
#include "../streaming_object.h"
#include "../../pt_gfx_frame_object_base.h"
#include "../../pt_gfx_connection_vk.h"
#include <vulkan/vulkan.h>

class gfx_texture_vk final : public gfx_texture_base
{
    VkImage m_image;
    struct gfx_malloc_allocation_vk m_image_allocation;
    VkImageView m_image_view;

    void pre_streaming_done_destroy_callback(class gfx_connection_base *gfx_connection) override;
    bool streaming_done_callback(class gfx_connection_base *gfx_connection) override;
    void post_stream_done_destroy_callback(class gfx_connection_base *gfx_connection) override;
    inline void unified_destory(class gfx_connection_vk *gfx_connection);

    bool load_header_callback(
        pt_input_stream_ref input_stream,
        pt_input_stream_read_callback input_stream_read_callback,
        pt_input_stream_seek_callback input_stream_seek_callback,
        class gfx_connection_base *connection,
        size_t *out_memcpy_dests_size,
        size_t *out_memcpy_dests_align) final;

    size_t calculate_staging_buffer_total_size_callback(
        size_t base_offset,
        class gfx_connection_base *connection,
        void *out_memcpy_dests) final;

    bool load_data_callback(
        pt_input_stream_ref input_stream,
        pt_input_stream_read_callback input_stream_read_callback,
        pt_input_stream_seek_callback input_stream_seek_callback,
        class gfx_connection_base *connection,
        void const *memcpy_dests,
        uint32_t streaming_throttling_index) final;

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
