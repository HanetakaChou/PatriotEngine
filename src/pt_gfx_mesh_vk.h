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
#include <pt_mcrt_scalable_allocator.h>
#include <pt_mcrt_vector.h>
#include "pt_gfx_mesh_base.h"
#include "pt_gfx_connection_vk.h"
#include <vulkan/vulkan.h>

struct gfx_mesh_primitive_vk
{
    // mali IDVS
    // seperate position and varying
    VkBuffer m_vertex_position_buffer;
    struct gfx_malloc_allocation_vk m_vertex_position_allocation;
    VkBuffer m_vertex_varying_buffer;
    struct gfx_malloc_allocation_vk m_vertex_varying_allocation;
    VkIndexType m_index_type;
    uint32_t m_index_count;
    VkBuffer m_index_buffer;
    struct gfx_malloc_allocation_vk m_index_allocation;
};

class gfx_mesh_vk final : public gfx_mesh_base
{
    mcrt_vector<gfx_mesh_primitive_vk> m_primitives;

    // create
    bool read_input_stream_alloc_asset_buffers(class gfx_connection_base *gfx_connection, struct pt_gfx_mesh_neutral_header_t *const neutral_header, struct pt_gfx_mesh_neutral_primitive_header_t const *neutral_primitive_headers);
    size_t read_input_stream_calculate_total_size(struct pt_gfx_mesh_neutral_header_t const *neutral_header, struct pt_gfx_mesh_neutral_primitive_header_t const *neutral_primitive_headers, uint64_t base_offset, struct pt_gfx_mesh_neutral_primitive_memcpy_dest_t *out_memcpy_dests);
    void read_input_stream_record_copy_commands(class gfx_connection_base *gfx_connection, uint32_t streaming_throttling_index, struct pt_gfx_mesh_neutral_primitive_memcpy_dest_t const *memcpy_dests);

public:
    // in use
    mcrt_vector<gfx_mesh_primitive_vk> const &primitives_get() const;

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
    inline ~gfx_mesh_vk() {}
};

#endif