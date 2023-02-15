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

#include <stddef.h>
#include <stdint.h>
#include <pt_mcrt_intrin.h>
#include <pt_mcrt_malloc.h>
#include <pt_mcrt_log.h>
#include "pt_gfx_mesh_vk.h"
#include "pt_gfx_mesh_base_load.h"
#include <assert.h>

bool gfx_mesh_vk::load_header_callback(
    pt_gfx_input_stream_ref input_stream,
    pt_gfx_input_stream_read_callback input_stream_read_callback,
    pt_gfx_input_stream_seek_callback input_stream_seek_callback,
    class gfx_connection_base* connection,
    size_t* out_memcpy_dests_size,
    size_t* out_memcpy_dests_align)
{
    struct pt_gfx_input_stream_callbacks_t const input_stream_callbacks =
    {
        NULL,
        NULL,
        input_stream_read_callback,
        input_stream_seek_callback,
        NULL
    };

    // 1. load asset header
    if (!mesh_load_header_from_input_stream(&this->m_mesh_asset_header, input_stream, &input_stream_callbacks))
    {
        return false;
    }


    this->m_mesh_asset_primitive_headers.resize(this->m_mesh_asset_header.primitive_count);
    if (!mesh_load_primitive_headers_from_input_stream(&this->m_mesh_asset_header, &this->m_mesh_asset_primitive_headers[0], input_stream, &input_stream_callbacks))
    {
        return false;
    }

    assert(NULL != out_memcpy_dests_size);
    assert(NULL != out_memcpy_dests_align);
    {
        (*out_memcpy_dests_size) = sizeof(struct pt_gfx_mesh_neutral_primitive_memcpy_dest_t) * this->m_mesh_asset_header.primitive_count;
        (*out_memcpy_dests_align) = alignof(struct pt_gfx_mesh_neutral_primitive_memcpy_dest_t);
    }

    // 1-2. allocate device resources
    class gfx_connection_vk* vk_connection = static_cast<class gfx_connection_vk*>(connection);

    m_primitives.resize(this->m_mesh_asset_header.primitive_count);

    for (uint32_t primitive_index = 0U; primitive_index < this->m_mesh_asset_header.primitive_count; ++primitive_index)
    {
        // vertex position buffer
        {
            assert(VK_NULL_HANDLE == this->m_primitives[primitive_index].m_vertex_position_buffer);
            {
                VkBufferCreateInfo buffer_create_info;
                buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                buffer_create_info.pNext = NULL;
                buffer_create_info.flags = 0U;
                buffer_create_info.size = sizeof(pt_gfx_mesh_neutral_vertex_position) * this->m_mesh_asset_primitive_headers[primitive_index].vertex_count;
                buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
                buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                buffer_create_info.queueFamilyIndexCount = 0U;
                buffer_create_info.pQueueFamilyIndices = NULL;

                // TODO: multi-threading
                // vkAllocateMemory
                // https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/vkspec.html#fundamentals-threadingbehavior

                bool res_asset_vertex_buffer_alloc = vk_connection->asset_vertex_buffer_alloc(&buffer_create_info, &this->m_primitives[primitive_index].m_vertex_position_buffer, &this->m_primitives[primitive_index].m_vertex_position_allocation);
                if (PT_UNLIKELY(!res_asset_vertex_buffer_alloc))
                {
                    this->m_primitives[primitive_index].m_vertex_position_buffer = VK_NULL_HANDLE;
                    return false;
                }
            }
        }

        // vertex varying buffer
        {
            assert(VK_NULL_HANDLE == this->m_primitives[primitive_index].m_vertex_varying_buffer);

            VkBufferCreateInfo buffer_create_info;
            buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_create_info.pNext = NULL;
            buffer_create_info.flags = 0U;
            buffer_create_info.size = sizeof(pt_gfx_mesh_neutral_vertex_varying) * this->m_mesh_asset_primitive_headers[primitive_index].vertex_count;
            buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            buffer_create_info.queueFamilyIndexCount = 0U;
            buffer_create_info.pQueueFamilyIndices = NULL;

            // TODO: multi-threading
            // vkAllocateMemory
            // https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/vkspec.html#fundamentals-threadingbehavior

            bool res_asset_vertex_buffer_alloc = vk_connection->asset_vertex_buffer_alloc(&buffer_create_info, &this->m_primitives[primitive_index].m_vertex_varying_buffer, &this->m_primitives[primitive_index].m_vertex_varying_allocation);
            if (PT_UNLIKELY(!res_asset_vertex_buffer_alloc))
            {
                this->m_primitives[primitive_index].m_vertex_varying_buffer = VK_NULL_HANDLE;
                return false;
            }
        }

        // index buffer
        {
            assert(VK_NULL_HANDLE == this->m_primitives[primitive_index].m_index_buffer);

            VkBufferCreateInfo buffer_create_info;
            buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_create_info.pNext = NULL;
            buffer_create_info.flags = 0U;
            buffer_create_info.size = (this->m_mesh_asset_primitive_headers[primitive_index].is_index_type_uint16 ? sizeof(uint16_t) : sizeof(uint32_t)) * this->m_mesh_asset_primitive_headers[primitive_index].index_count;
            buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            buffer_create_info.queueFamilyIndexCount = 0U;
            buffer_create_info.pQueueFamilyIndices = NULL;

            // TODO: multi-threading
            // vkAllocateMemory
            // https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/vkspec.html#fundamentals-threadingbehavior

            bool res_asset_index_buffer_alloc = vk_connection->asset_index_buffer_alloc(&buffer_create_info, &this->m_primitives[primitive_index].m_index_buffer, &this->m_primitives[primitive_index].m_index_allocation);
            if (PT_UNLIKELY(!res_asset_index_buffer_alloc))
            {
                this->m_primitives[primitive_index].m_index_buffer = VK_NULL_HANDLE;
                return false;
            }
        }
    }

    return true;
}

size_t gfx_mesh_vk::calculate_staging_buffer_total_size_callback(
    size_t base_offset,
    class gfx_connection_base* connection,
    void* out_void_memcpy_dests)
{
    struct pt_gfx_mesh_neutral_primitive_memcpy_dest_t* out_memcpy_dests = static_cast<struct pt_gfx_mesh_neutral_primitive_memcpy_dest_t*>(out_void_memcpy_dests);

    size_t staging_offset = base_offset;
    size_t total_bytes = 0U;

    for (uint32_t primitive_index = 0U; primitive_index < this->m_mesh_asset_header.primitive_count; ++primitive_index)
    {
        out_memcpy_dests[primitive_index].position_staging_offset = staging_offset;
        out_memcpy_dests[primitive_index].position_staging_size = sizeof(pt_gfx_mesh_neutral_vertex_position) * this->m_mesh_asset_primitive_headers[primitive_index].vertex_count;
        // no alignment required
        size_t new_staging_offset = staging_offset + out_memcpy_dests[primitive_index].position_staging_size;
        total_bytes += (new_staging_offset - staging_offset);
        staging_offset = new_staging_offset;

        out_memcpy_dests[primitive_index].varying_staging_offset = staging_offset;
        out_memcpy_dests[primitive_index].varying_staging_size = sizeof(pt_gfx_mesh_neutral_vertex_varying) * this->m_mesh_asset_primitive_headers[primitive_index].vertex_count;
        new_staging_offset = staging_offset + out_memcpy_dests[primitive_index].varying_staging_size;
        total_bytes += (new_staging_offset - staging_offset);
        staging_offset = new_staging_offset;

        out_memcpy_dests[primitive_index].index_staging_offset = staging_offset;
        out_memcpy_dests[primitive_index].index_staging_size = (this->m_mesh_asset_primitive_headers[primitive_index].is_index_type_uint16 ? sizeof(uint16_t) : sizeof(uint32_t)) * this->m_mesh_asset_primitive_headers[primitive_index].index_count;
        new_staging_offset = staging_offset + out_memcpy_dests[primitive_index].index_staging_size;
        total_bytes += (new_staging_offset - staging_offset);
        staging_offset = new_staging_offset;
    }

    return total_bytes;
}

bool gfx_mesh_vk::load_data_callback(
    pt_gfx_input_stream_ref input_stream,
    pt_gfx_input_stream_read_callback input_stream_read_callback,
    pt_gfx_input_stream_seek_callback input_stream_seek_callback,
    class gfx_connection_base* connection,
    void const* void_memcpy_dests,
    uint32_t streaming_throttling_index)
{
    struct pt_gfx_mesh_neutral_primitive_memcpy_dest_t const* memcpy_dests = static_cast<struct pt_gfx_mesh_neutral_primitive_memcpy_dest_t const*>(void_memcpy_dests);

    // 1. load the asset data into staging buffer according to the 'memcpy_dests'
    struct pt_gfx_input_stream_callbacks_t const input_stream_callbacks =
    {
        NULL,
        NULL,
        input_stream_read_callback,
        input_stream_seek_callback,
        NULL
    };

    if (!mesh_load_primitive_data_from_input_stream(&this->m_mesh_asset_header, &this->m_mesh_asset_primitive_headers[0], connection->staging_buffer_pointer(), memcpy_dests, input_stream, &input_stream_callbacks))
    {
        return false;
    }

    // 2. copy the data into image from staging buffer
    class gfx_connection_vk *vk_connection = static_cast<class gfx_connection_vk*>(connection);
    
    uint32_t streaming_thread_index = mcrt_this_task_arena_current_thread_index();
    
    VkBuffer staging_buffer = vk_connection->staging_buffer();

    uint32_t primitive_count = static_cast<uint32_t>(this->m_primitives.size());
    for (uint32_t primitive_index = 0U; primitive_index < primitive_count; ++primitive_index)
    {
        VkBufferCopy region_position[1];
        region_position[0].srcOffset = memcpy_dests[primitive_index].position_staging_offset;
        region_position[0].dstOffset = 0U;
        region_position[0].size = memcpy_dests[primitive_index].position_staging_size;
        vk_connection->copy_vertex_buffer(streaming_throttling_index, streaming_thread_index, staging_buffer, this->m_primitives[primitive_index].m_vertex_position_buffer, 1U, region_position);

        VkBufferCopy region_varying[1];
        region_varying[0].srcOffset = memcpy_dests[primitive_index].varying_staging_offset;
        region_varying[0].dstOffset = 0U;
        region_varying[0].size = memcpy_dests[primitive_index].varying_staging_size;
        vk_connection->copy_vertex_buffer(streaming_throttling_index, streaming_thread_index, staging_buffer, this->m_primitives[primitive_index].m_vertex_varying_buffer, 1U, region_varying);

        VkBufferCopy region_index[1];
        region_index[0].srcOffset = memcpy_dests[primitive_index].index_staging_offset;
        region_index[0].dstOffset = 0U;
        region_index[0].size = memcpy_dests[primitive_index].index_staging_size;
        vk_connection->copy_vertex_buffer(streaming_throttling_index, streaming_thread_index, staging_buffer, this->m_primitives[primitive_index].m_index_buffer, 1U, region_index);
    }

    return true;
}

void gfx_mesh_vk::record_draw_command_buffer(class gfx_device_vk* vk_device, VkCommandBuffer draw_command_buffer) const
{
    for (uint32_t primitive_index = 0U; primitive_index < this->m_mesh_asset_header.primitive_count; ++primitive_index)
    {
        VkBuffer buffers[2] = { this->m_primitives[primitive_index].m_vertex_position_buffer, this->m_primitives[primitive_index].m_vertex_varying_buffer };
        VkDeviceSize offsets[2] = { 0U, 0U };

        vk_device->cmd_bind_vertex_buffers(draw_command_buffer, 0, 2, buffers, offsets);
        vk_device->cmd_bind_index_buffer(draw_command_buffer, this->m_primitives[primitive_index].m_index_buffer, 0U, this->m_mesh_asset_primitive_headers[primitive_index].is_index_type_uint16 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);

        vk_device->cmd_draw_indexed(draw_command_buffer, this->m_mesh_asset_primitive_headers[primitive_index].index_count, 1U, 0U, 0U, 0U);
    }
}

void gfx_mesh_vk::pre_streaming_done_destroy_callback(class gfx_connection_base *gfx_connection_base)
{
    class gfx_connection_vk *gfx_connection = static_cast<class gfx_connection_vk *>(gfx_connection_base);
    this->unified_destory(gfx_connection);
}

bool gfx_mesh_vk::streaming_done_callback(class gfx_connection_base *gfx_connection_base)
{
    return true;
}

void gfx_mesh_vk::post_stream_done_destroy_callback(class gfx_connection_base *gfx_connection_base)
{
    class gfx_connection_vk *gfx_connection = static_cast<class gfx_connection_vk *>(gfx_connection_base);
    this->unified_destory(gfx_connection);
}

inline void gfx_mesh_vk::unified_destory(class gfx_connection_vk *gfx_connection)
{
    // assert(0U == mcrt_atomic_load(&this->m_ref_count));

    uint32_t primitive_count = static_cast<uint32_t>(this->m_primitives.size());
    for (uint32_t primitive_index = 0U; primitive_index < primitive_count; ++primitive_index)
    {
        if (VK_NULL_HANDLE != this->m_primitives[primitive_index].m_vertex_position_buffer)
        {
            gfx_connection->asset_vertex_buffer_free(this->m_primitives[primitive_index].m_vertex_position_buffer, &this->m_primitives[primitive_index].m_vertex_position_allocation);
        }

        if (VK_NULL_HANDLE != this->m_primitives[primitive_index].m_vertex_varying_buffer)
        {
            gfx_connection->asset_vertex_buffer_free(this->m_primitives[primitive_index].m_vertex_varying_buffer, &this->m_primitives[primitive_index].m_vertex_varying_allocation);
        }

        if (VK_NULL_HANDLE != this->m_primitives[primitive_index].m_index_buffer)
        {
            gfx_connection->asset_index_buffer_free(this->m_primitives[primitive_index].m_index_buffer, &this->m_primitives[primitive_index].m_index_allocation);
        }
    }

    this->~gfx_mesh_vk();

    mcrt_aligned_free(this);
}