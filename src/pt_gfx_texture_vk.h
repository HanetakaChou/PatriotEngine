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

    uint64_t m_gfx_malloc_offset;
    uint64_t m_gfx_malloc_size;
    void *m_gfx_malloc_page_handle;
    VkDeviceMemory m_gfx_malloc_device_memory;

    VkImageView m_image_view;

    bool texture_streaming_stage_first_pre_populate_task_data_callback(class gfx_connection_base *gfx_connection, struct gfx_texture_neutral_header_t const *neutral_header) override;
    bool texture_streaming_stage_second_pre_calculate_total_size_callback(struct gfx_texture_neutral_header_t const *neutral_header, void **memcpy_dest, void **cmdcopy_dest) override;
    size_t texture_streaming_stage_second_calculate_total_size_callback(class gfx_connection_base *gfx_connection, struct gfx_texture_neutral_header_t const *neutral_header, void *memcpy_dest, void *cmdcopy_dest, uint64_t base_offset) override;
    void texture_streaming_stage_second_post_calculate_total_size_fail_callback(void *memcpy_dest, void *cmdcopy_dest) override;
    bool texture_streaming_stage_second_post_calculate_total_size_success_callback(class gfx_connection_base *gfx_connection, uint32_t streaming_throttling_index, struct gfx_texture_neutral_header_t const *neutral_header, size_t const *neutral_data_offset, void *memcpy_dest, void *cmdcopy_dest, gfx_input_stream_ref gfx_input_stream, intptr_t(PT_PTR *gfx_input_stream_read_callback)(gfx_input_stream_ref, void *, size_t), int64_t(PT_PTR *gfx_input_stream_seek_callback)(gfx_input_stream_ref, int64_t, int)) override;

    void streaming_destroy_callback(class gfx_connection_base *gfx_connection) override;
    bool streaming_done_callback(class gfx_connection_base *gfx_connection) override;
    void frame_destroy_callback(class gfx_connection_base *gfx_connection) override;
    inline void unified_destory(class gfx_connection_vk *gfx_connection);

private:
   
public:
    inline gfx_texture_vk() : gfx_texture_base(),
                              m_image(VK_NULL_HANDLE),
                              m_gfx_malloc_offset(uint64_t(-1)), m_gfx_malloc_size(uint64_t(-1)), m_gfx_malloc_page_handle(NULL), m_gfx_malloc_device_memory(VK_NULL_HANDLE),
                              m_image_view(VK_NULL_HANDLE)
    {
    }

    inline VkImageView get_image_view() const
    {
        assert((!this->is_streaming_error()) && this->is_streaming_done());
        return this->m_image_view;
    }
};

#endif
