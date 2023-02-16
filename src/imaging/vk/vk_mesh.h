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

#ifndef _IMAGING_VK_MESH_H_
#define _IMAGING_VK_MESH_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_mcrt_allocator.h>
#include <pt_mcrt_vector.h>
#include "../mesh.h"
#include "../../pt_gfx_connection_vk.h"
#include <vulkan/vulkan.h>

struct gfx_mesh_primitive_vk
{
    // mali IDVS
    // seperate position and varying
    VkBuffer m_vertex_position_buffer;
    struct gfx_malloc_allocation_vk m_vertex_position_allocation;
    VkBuffer m_vertex_varying_buffer;
    struct gfx_malloc_allocation_vk m_vertex_varying_allocation;
    VkBuffer m_index_buffer;
    struct gfx_malloc_allocation_vk m_index_allocation;
};

class gfx_mesh_vk final : public gfx_mesh_base
{
    mcrt_vector<gfx_mesh_primitive_vk> m_primitives;

   bool load_header_callback(
        pt_input_stream_ref input_stream,
        pt_input_stream_read_callback input_stream_read_callback,
        pt_input_stream_seek_callback input_stream_seek_callback,
        class gfx_connection_base* connection,
        size_t* out_memcpy_dests_size,
        size_t* out_memcpy_dests_align) final;

   size_t calculate_staging_buffer_total_size_callback(
       size_t base_offset,
       class gfx_connection_base* connection,
       void* out_memcpy_dests) final;

   bool load_data_callback(
       pt_input_stream_ref input_stream,
       pt_input_stream_read_callback input_stream_read_callback,
       pt_input_stream_seek_callback input_stream_seek_callback,
       class gfx_connection_base* connection,
       void const* memcpy_dests,
       uint32_t streaming_throttling_index) final;

public:
    // in use
    void record_draw_command_buffer(uint32_t primitive_index, class gfx_device_vk* vk_device, VkCommandBuffer draw_command_buffer) const;

private:
    // destroy
    void pre_streaming_done_destroy_callback(class gfx_connection_base *gfx_connection) override;
    bool streaming_done_callback(class gfx_connection_base *gfx_connection) override;
    void post_stream_done_destroy_callback(class gfx_connection_base *gfx_connection) override;
    inline void unified_destory(class gfx_connection_vk *gfx_connection);

public:
    inline gfx_mesh_vk() : gfx_mesh_base()
    {

    }

private:
    inline ~gfx_mesh_vk() 
    {

    }
};

#endif