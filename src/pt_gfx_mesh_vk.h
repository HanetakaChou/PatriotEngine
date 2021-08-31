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

#ifndef _PT_GFX_MESH_VK_H_
#define _PT_GFX_MESH_VK_H_ 1

#include <stddef.h>
#include <stdint.h>
#include "pt_gfx_mesh_base.h"
#include "pt_gfx_connection_vk.h"
#include <vulkan/vulkan.h>

class gfx_mesh_vk final : public gfx_mesh_base
{
    // mali IDVS
    // seperate position and varying
public:
    VkBuffer m_vertex_position_buffer;

private:
    uint64_t m_vertex_position_gfx_malloc_offset;
    uint64_t m_vertex_position_gfx_malloc_size;
    void *m_vertex_position_gfx_malloc_page_handle;
    VkDeviceMemory m_vertex_position_gfx_malloc_device_memory;

public:
    VkBuffer m_vertex_varying_buffer;

private:
    uint64_t m_vertex_varying_gfx_malloc_offset;
    uint64_t m_vertex_varying_gfx_malloc_size;
    void *m_vertex_varying_gfx_malloc_page_handle;
    VkDeviceMemory m_vertex_varying_gfx_malloc_device_memory;

public:
    VkBuffer m_index_buffer;
    VkIndexType m_indexType;
private:
    uint64_t m_index_gfx_malloc_offset;
    uint64_t m_index_gfx_malloc_size;
    void *m_index_gfx_malloc_page_handle;
    VkDeviceMemory m_index_gfx_malloc_device_memory;

    bool mesh_streaming_stage_first_pre_populate_task_data_callback(class gfx_connection_base *gfx_connection, struct gfx_mesh_neutral_header_t *const neutral_header) override;
    size_t mesh_streaming_stage_second_calculate_total_size_callback(class gfx_connection_base *gfx_connection, struct gfx_mesh_neutral_header_t const *neutral_header, struct gfx_mesh_neutral_memcpy_dest_t *memcpy_dest, uint64_t base_offset) override;
    bool mesh_streaming_stage_second_post_calculate_total_size_callback(class gfx_connection_base *gfx_connection, uint32_t streaming_throttling_index, struct gfx_mesh_neutral_header_t const *neutral_header, struct gfx_mesh_neutral_memcpy_dest_t *memcpy_dest, pt_gfx_input_stream_ref gfx_input_stream, intptr_t(PT_PTR *gfx_input_stream_read_callback)(pt_gfx_input_stream_ref, void *, size_t), int64_t(PT_PTR *gfx_input_stream_seek_callback)(pt_gfx_input_stream_ref, int64_t, int)) override;

    void streaming_destroy_callback(class gfx_connection_base *gfx_connection) override;
    bool streaming_done_callback(class gfx_connection_base *gfx_connection) override;
    void frame_destroy_callback(class gfx_connection_base *gfx_connection) override;
    inline void unified_destory(class gfx_connection_vk *gfx_connection);

public:
    inline gfx_mesh_vk() : gfx_mesh_base(),
                           m_vertex_position_buffer(VK_NULL_HANDLE), m_vertex_position_gfx_malloc_offset(uint64_t(-1)), m_vertex_position_gfx_malloc_size(uint64_t(-1)), m_vertex_position_gfx_malloc_page_handle(NULL), m_vertex_position_gfx_malloc_device_memory(VK_NULL_HANDLE),
                           m_vertex_varying_buffer(VK_NULL_HANDLE), m_vertex_varying_gfx_malloc_offset(uint64_t(-1)), m_vertex_varying_gfx_malloc_size(uint64_t(-1)), m_vertex_varying_gfx_malloc_page_handle(NULL), m_vertex_varying_gfx_malloc_device_memory(VK_NULL_HANDLE),
                           m_index_buffer(VK_NULL_HANDLE), m_index_gfx_malloc_offset(uint64_t(-1)), m_index_gfx_malloc_size(uint64_t(-1)), m_index_gfx_malloc_page_handle(NULL), m_index_gfx_malloc_device_memory(VK_NULL_HANDLE)
    {
    }
};

#endif