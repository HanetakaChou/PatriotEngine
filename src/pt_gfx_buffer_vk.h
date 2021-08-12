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

#ifndef _PT_GFX_BUFFER_VK_H_
#define _PT_GFX_BUFFER_VK_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_mcrt_task.h>
#include <pt_mcrt_scalable_allocator.h>
#include "pt_gfx_buffer_base.h"
#include "pt_gfx_connection_vk.h"
#include <vulkan/vulkan.h>
#include <string>

class gfx_buffer_vk final : public gfx_buffer_base
{
    VkBuffer m_buffer;

    uint64_t m_gfx_malloc_offset;
    uint64_t m_gfx_malloc_size;
    void *m_gfx_malloc_page_handle;
    VkDeviceMemory m_gfx_malloc_device_memory;

    using mcrt_string = std::basic_string<char, std::char_traits<char>, mcrt::scalable_allocator<char>>;

    bool read_vertex_input_stream(class gfx_connection_common *gfx_connection, gfx_buffer_ref buffer, int64_t input_stream_offset, int64_t input_stream_length, char const *initial_filename, gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename), intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence), void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream)) override;

    bool read_index_input_stream(class gfx_connection_common *gfx_connection, gfx_buffer_ref buffer, int64_t input_stream_offset, int64_t input_stream_length, char const *initial_filename, gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename), intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence), void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream)) override;

    bool read_input_stream_internal(class gfx_connection_common *gfx_connection, gfx_buffer_ref buffer, VkBufferUsageFlags buffer_usage, int64_t input_stream_offset, int64_t input_stream_length, char const *initial_filename, gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename), intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence), void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream));

    struct read_input_stream_task_data
    {
        mcrt_string m_initial_filename;
        gfx_input_stream_ref(PT_PTR *m_input_stream_init_callback)(char const *initial_filename);
        intptr_t(PT_PTR *m_input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count);
        int64_t(PT_PTR *m_input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence);
        void(PT_PTR *m_input_stream_destroy_callback)(gfx_input_stream_ref input_stream);
        int64_t m_input_stream_offset;
        int64_t m_input_stream_length;
        class gfx_connection_vk *m_gfx_connection;
        class gfx_buffer_vk *m_gfx_buffer;
    };

    static mcrt_task_ref read_input_stream_task_execute(mcrt_task_ref self);

    static inline mcrt_task_ref read_input_stream_task_execute_internal(uint32_t *output_streaming_throttling_index, bool *output_recycle, mcrt_task_ref self);

    void destroy(class gfx_connection_common *gfx_connection) override;

    void streaming_cancel(class gfx_connection_common *gfx_connection) override;

public:
    inline gfx_buffer_vk() : gfx_buffer_base(), m_buffer(VK_NULL_HANDLE), m_gfx_malloc_offset(uint64_t(-1)), m_gfx_malloc_size(uint64_t(-1)), m_gfx_malloc_page_handle(NULL), m_gfx_malloc_device_memory(VK_NULL_HANDLE) {}
};

#endif