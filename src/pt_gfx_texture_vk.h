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

    bool read_input_stream(
        class gfx_connection_base *gfx_connection,
        char const *initial_filename,
        gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename),
        intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count),
        int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence),
        void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream)) override;

    bool streaming_stage_first_pre_populate_task_data_callback(
        class gfx_connection_base *gfx_connection,
        gfx_input_stream_ref input_stream,
        intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count),
        int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence),
        void *thread_stack_user_defined) override;

    void streaming_stage_first_populate_task_data_callback(
        void *thread_stack_user_defined,
        struct streaming_stage_second_task_data_user_defined_t *task_data_user_defined) override;

    bool streaming_stage_second_pre_calculate_total_size_callback(
        struct streaming_stage_second_thread_stack_data_user_defined_t *thread_stack_data_user_defined,
        gfx_input_stream_ref input_stream,
        intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count),
        int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence),
        struct streaming_stage_second_task_data_user_defined_t *task_data_user_defined) override;

    size_t streaming_stage_second_calculate_total_size_callback(
        uint64_t base_offset,
        struct streaming_stage_second_thread_stack_data_user_defined_t *thread_stack_data_user_defined,
        class gfx_connection_base *gfx_connection,
        struct streaming_stage_second_task_data_user_defined_t *task_data_user_defined) override;

    bool streaming_stage_second_post_calculate_total_size_callback(
        bool staging_buffer_allocate_success,
        uint32_t streaming_throttling_index,
        struct streaming_stage_second_thread_stack_data_user_defined_t *thread_stack_data_user_defined,
        class gfx_connection_base *gfx_connection,
        gfx_input_stream_ref input_stream,
        intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count),
        int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence),
        struct streaming_stage_second_task_data_user_defined_t *task_data_user_defined) override;

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
