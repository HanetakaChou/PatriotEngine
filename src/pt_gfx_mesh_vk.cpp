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

#include <stddef.h>
#include <stdint.h>
#include <pt_mcrt_intrin.h>
#include <pt_mcrt_malloc.h>
#include <pt_mcrt_log.h>
#include "pt_gfx_mesh_vk.h"
#include "pt_gfx_mesh_base_gltf_parse.h"
#include "pt_gfx_mesh_base_load.h"
#include <assert.h>

bool gfx_mesh_vk::mesh_streaming_stage_first_pre_populate_task_data_callback(class gfx_connection_base *gfx_connection_base, struct gfx_mesh_neutral_header_t *const neutral_header)
{
    class gfx_connection_vk *gfx_connection = static_cast<class gfx_connection_vk *>(gfx_connection_base);
    uint32_t vertex_position_length = (sizeof(float) * 3U) * neutral_header->vertex_count;
    uint32_t vertex_varying_length = (sizeof(float) * 2U) * neutral_header->vertex_count;

    uint32_t index_size;
    if (PT_GFX_MESH_NEUTRAL_INDEX_TYPE_UINT16 == neutral_header->index_type)
    {
        index_size = 2U;
        this->m_indexType = VK_INDEX_TYPE_UINT16;
    }
    else if (PT_GFX_MESH_NEUTRAL_INDEX_TYPE_UINT32 == neutral_header->index_type)
    {
        index_size = 4U;
        this->m_indexType = VK_INDEX_TYPE_UINT32;
    }
    else
    {
        assert(0);
        index_size = -1;
    }
    uint32_t index_length = index_size * neutral_header->index_count;

    // Vertex Position
    {
        assert(VK_NULL_HANDLE == this->m_vertex_position_buffer);
        {
            VkBufferCreateInfo buffer_create_info;
            buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_create_info.pNext = NULL;
            buffer_create_info.flags = 0U;
            buffer_create_info.size = vertex_position_length;
            buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            buffer_create_info.queueFamilyIndexCount = 0U;
            buffer_create_info.pQueueFamilyIndices = NULL;

            PT_MAYBE_UNUSED VkResult res = gfx_connection->create_buffer(&buffer_create_info, &this->m_vertex_position_buffer);
            assert(VK_SUCCESS == res);
        }

        assert(VK_NULL_HANDLE == this->m_vertex_position_gfx_malloc_device_memory);
        {
            VkMemoryRequirements memory_requirements;
            gfx_connection->get_buffer_memory_requirements(this->m_vertex_position_buffer, &memory_requirements);

            // vkAllocateMemory
            // https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/vkspec.html#fundamentals-threadingbehavior

            this->m_vertex_position_gfx_malloc_device_memory = gfx_connection->transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_alloc(&memory_requirements, &this->m_vertex_position_gfx_malloc_page_handle, &this->m_vertex_position_gfx_malloc_offset, &this->m_vertex_position_gfx_malloc_size);
            if (PT_UNLIKELY(VK_NULL_HANDLE == this->m_vertex_position_gfx_malloc_device_memory))
            {
                gfx_connection->destroy_buffer(this->m_vertex_position_buffer);
                this->m_vertex_position_buffer = VK_NULL_HANDLE;
                return false;
            }
        }
        assert(VK_NULL_HANDLE != this->m_vertex_position_gfx_malloc_device_memory);

        {
            PT_MAYBE_UNUSED VkResult res_bind_buffer_memory = gfx_connection->bind_buffer_memory(this->m_vertex_position_buffer, this->m_vertex_position_gfx_malloc_device_memory, this->m_vertex_position_gfx_malloc_offset);
            assert(VK_SUCCESS == res_bind_buffer_memory);
        }
    }

    // Vertex Varying
    {
        assert(VK_NULL_HANDLE == this->m_vertex_varying_buffer);
        {
            VkBufferCreateInfo buffer_create_info;
            buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_create_info.pNext = NULL;
            buffer_create_info.flags = 0U;
            buffer_create_info.size = vertex_varying_length;
            buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            buffer_create_info.queueFamilyIndexCount = 0U;
            buffer_create_info.pQueueFamilyIndices = NULL;

            PT_MAYBE_UNUSED VkResult res = gfx_connection->create_buffer(&buffer_create_info, &this->m_vertex_varying_buffer);
            assert(VK_SUCCESS == res);
        }

        assert(VK_NULL_HANDLE == this->m_vertex_varying_gfx_malloc_device_memory);
        {
            VkMemoryRequirements memory_requirements;
            gfx_connection->get_buffer_memory_requirements(this->m_vertex_varying_buffer, &memory_requirements);

            // vkAllocateMemory
            // https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/vkspec.html#fundamentals-threadingbehavior

            this->m_vertex_varying_gfx_malloc_device_memory = gfx_connection->transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_alloc(&memory_requirements, &this->m_vertex_varying_gfx_malloc_page_handle, &this->m_vertex_varying_gfx_malloc_offset, &this->m_vertex_varying_gfx_malloc_size);
            if (PT_UNLIKELY(VK_NULL_HANDLE == this->m_vertex_varying_gfx_malloc_device_memory))
            {
                gfx_connection->destroy_buffer(this->m_vertex_varying_buffer);
                this->m_vertex_varying_buffer = VK_NULL_HANDLE;
                return false;
            }
        }
        assert(VK_NULL_HANDLE != this->m_vertex_varying_gfx_malloc_device_memory);

        {
            PT_MAYBE_UNUSED VkResult res_bind_buffer_memory = gfx_connection->bind_buffer_memory(this->m_vertex_varying_buffer, this->m_vertex_varying_gfx_malloc_device_memory, this->m_vertex_varying_gfx_malloc_offset);
            assert(VK_SUCCESS == res_bind_buffer_memory);
        }
    }

    // Index
    {
        assert(VK_NULL_HANDLE == this->m_index_buffer);
        {
            VkBufferCreateInfo buffer_create_info;
            buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_create_info.pNext = NULL;
            buffer_create_info.flags = 0U;
            buffer_create_info.size = index_length;
            buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            buffer_create_info.queueFamilyIndexCount = 0U;
            buffer_create_info.pQueueFamilyIndices = NULL;

            PT_MAYBE_UNUSED VkResult res = gfx_connection->create_buffer(&buffer_create_info, &this->m_index_buffer);
            assert(VK_SUCCESS == res);
        }

        assert(VK_NULL_HANDLE == this->m_index_gfx_malloc_device_memory);
        {
            VkMemoryRequirements memory_requirements;
            gfx_connection->get_buffer_memory_requirements(this->m_index_buffer, &memory_requirements);

            // vkAllocateMemory
            // https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/vkspec.html#fundamentals-threadingbehavior

            this->m_index_gfx_malloc_device_memory = gfx_connection->transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_alloc(&memory_requirements, &this->m_index_gfx_malloc_page_handle, &this->m_index_gfx_malloc_offset, &this->m_index_gfx_malloc_size);
            if (PT_UNLIKELY(VK_NULL_HANDLE == this->m_index_gfx_malloc_device_memory))
            {
                gfx_connection->destroy_buffer(this->m_index_buffer);
                this->m_index_buffer = VK_NULL_HANDLE;
                return false;
            }
        }
        assert(VK_NULL_HANDLE != this->m_index_gfx_malloc_device_memory);

        {
            PT_MAYBE_UNUSED VkResult res_bind_buffer_memory = gfx_connection->bind_buffer_memory(this->m_index_buffer, this->m_index_gfx_malloc_device_memory, this->m_index_gfx_malloc_offset);
            assert(VK_SUCCESS == res_bind_buffer_memory);
        }
    }

    return true;
}

size_t gfx_mesh_vk::mesh_streaming_stage_second_calculate_total_size_callback(class gfx_connection_base *gfx_connection, struct gfx_mesh_neutral_header_t const *neutral_header, struct gfx_mesh_neutral_memcpy_dest_t *memcpy_dest_void, uint64_t base_offset)
{
    struct gfx_mesh_neutral_memcpy_dest_t *memcpy_dest = static_cast<struct gfx_mesh_neutral_memcpy_dest_t *>(memcpy_dest_void);
    //VkBufferCopy *cmdcopy_dest = static_cast<VkBufferCopy *>(cmdcopy_dest_void);

    // calculate length
    uint32_t vertex_position_length = (sizeof(float) * 3U) * neutral_header->vertex_count;
    uint32_t vertex_varying_length = (sizeof(float) * 2U) * neutral_header->vertex_count;
    uint32_t index_size;
    if (PT_GFX_MESH_NEUTRAL_INDEX_TYPE_UINT16 == neutral_header->index_type)
    {
        index_size = 2U;
    }
    else if (PT_GFX_MESH_NEUTRAL_INDEX_TYPE_UINT32 == neutral_header->index_type)
    {
        index_size = 4U;
    }
    else
    {
        assert(0);
        index_size = -1;
    }
    uint32_t index_length = index_size * neutral_header->index_count;

    // vertex position
    memcpy_dest[0].staging_offset = base_offset;
    memcpy_dest[0].output_size = vertex_position_length;
    //cmdcopy_dest[0].srcOffset = base_offset;
    //cmdcopy_dest[0].dstOffset = 0U;
    //cmdcopy_dest[0].size = vertex_position_length;

    // vertex varying
    memcpy_dest[1].staging_offset = base_offset + vertex_position_length;
    memcpy_dest[1].output_size = vertex_varying_length;
    //cmdcopy_dest[1].srcOffset = base_offset + vertex_position_length;
    //cmdcopy_dest[1].dstOffset = 0U;
    //cmdcopy_dest[1].size = vertex_varying_length;

    // index
    memcpy_dest[2].staging_offset = base_offset + vertex_position_length + vertex_varying_length;
    memcpy_dest[2].output_size = index_length;
    //cmdcopy_dest[2].srcOffset = base_offset + vertex_position_length + vertex_varying_length;
    //cmdcopy_dest[2].dstOffset = 0U;
    //cmdcopy_dest[2].size = index_length;

    return (vertex_position_length + vertex_varying_length + index_length);
}

bool gfx_mesh_vk::mesh_streaming_stage_second_post_calculate_total_size_callback(class gfx_connection_base *gfx_connection_base, uint32_t streaming_throttling_index, struct gfx_mesh_neutral_header_t const *neutral_header, struct gfx_mesh_neutral_memcpy_dest_t *memcpy_dest, gfx_input_stream_ref gfx_input_stream, intptr_t(PT_PTR *gfx_input_stream_read_callback)(gfx_input_stream_ref, void *, size_t), int64_t(PT_PTR *gfx_input_stream_seek_callback)(gfx_input_stream_ref, int64_t, int))
{
    class gfx_connection_vk *gfx_connection = static_cast<class gfx_connection_vk *>(gfx_connection_base);

    int input_vertex_position_offset = 132;
    int input_vertex_position_length = 432;

    int input_vertex_varying_offset = 1572;
    int input_vertex_varying_length = 288;

    int input_index_offset = 60;
    int input_index_length = 72;

    // vertex position
    {
        int64_t res_seek = gfx_input_stream_seek_callback(gfx_input_stream, input_vertex_position_offset, PT_GFX_INPUT_STREAM_SEEK_SET);
        if (res_seek == -1 || res_seek != input_vertex_position_offset)
        {
            return false;
        }
        ptrdiff_t res_read = gfx_input_stream_read_callback(gfx_input_stream, reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(gfx_connection->transfer_src_buffer_pointer()) + memcpy_dest[0].staging_offset), input_vertex_position_length);
        if (res_read == -1 || res_read != input_vertex_position_length)
        {
            return false;
        }
    }

    // vertex varying
    {
        int64_t res_seek = gfx_input_stream_seek_callback(gfx_input_stream, input_vertex_varying_offset, PT_GFX_INPUT_STREAM_SEEK_SET);
        if (res_seek == -1 || res_seek != input_vertex_varying_offset)
        {
            return false;
        }
        ptrdiff_t res_read = gfx_input_stream_read_callback(gfx_input_stream, reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(gfx_connection->transfer_src_buffer_pointer()) + memcpy_dest[1].staging_offset), input_vertex_varying_length);

        float *texcoord0 = reinterpret_cast<float(*)>(reinterpret_cast<uintptr_t>(gfx_connection->transfer_src_buffer_pointer()) + memcpy_dest[1].staging_offset);
        for (int i = 0; i < 72; ++i)
        {
            texcoord0[i] = ((texcoord0[i] > 0) ? texcoord0[i] : (-texcoord0[i]));
        }

        if (res_read == -1 || res_read != input_vertex_varying_length)
        {
            return false;
        }
    }

    // index
    {
        int64_t res_seek = gfx_input_stream_seek_callback(gfx_input_stream, input_index_offset, PT_GFX_INPUT_STREAM_SEEK_SET);
        if (res_seek == -1 || res_seek != input_index_offset)
        {
            return false;
        }
        ptrdiff_t res_read = gfx_input_stream_read_callback(gfx_input_stream, reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(gfx_connection->transfer_src_buffer_pointer()) + memcpy_dest[2].staging_offset), input_index_length);
        if (res_read == -1 || res_read != input_index_length)
        {
            return false;
        }
    }

    // cmd copy
    {
        uint32_t streaming_thread_index = mcrt_this_task_arena_current_thread_index();
        VkBuffer transfer_src_buffer = gfx_connection->transfer_src_buffer();
        {
            VkBufferCopy regions[1];
            regions[0].srcOffset = memcpy_dest[0].staging_offset;
            regions[0].dstOffset = 0U;
            regions[0].size = memcpy_dest[0].output_size;
            gfx_connection->copy_vertex_buffer(streaming_throttling_index, streaming_thread_index, transfer_src_buffer, this->m_vertex_position_buffer, 1U, regions);
        }
        {
            VkBufferCopy regions[1];
            regions[0].srcOffset = memcpy_dest[1].staging_offset;
            regions[0].dstOffset = 0U;
            regions[0].size = memcpy_dest[1].output_size;
            gfx_connection->copy_vertex_buffer(streaming_throttling_index, streaming_thread_index, transfer_src_buffer, this->m_vertex_varying_buffer, 1U, regions);
        }
        {
            VkBufferCopy regions[1];
            regions[0].srcOffset = memcpy_dest[2].staging_offset;
            regions[0].dstOffset = 0U;
            regions[0].size = memcpy_dest[2].output_size;
            gfx_connection->copy_index_buffer(streaming_throttling_index, streaming_thread_index, transfer_src_buffer, this->m_index_buffer, 1U, regions);
        }
    }

    return true;
}

void gfx_mesh_vk::streaming_destroy_callback(class gfx_connection_base *gfx_connection_base)
{
    class gfx_connection_vk *gfx_connection = static_cast<class gfx_connection_vk *>(gfx_connection_base);
    this->unified_destory(gfx_connection);
}

bool gfx_mesh_vk::streaming_done_callback(class gfx_connection_base *gfx_connection_base)
{
    return true;
}

void gfx_mesh_vk::frame_destroy_callback(class gfx_connection_base *gfx_connection_base)
{
    class gfx_connection_vk *gfx_connection = static_cast<class gfx_connection_vk *>(gfx_connection_base);
    this->unified_destory(gfx_connection);
}

inline void gfx_mesh_vk::unified_destory(class gfx_connection_vk *gfx_connection)
{
    //assert(0U == mcrt_atomic_load(&this->m_ref_count));

    if (VK_NULL_HANDLE != this->m_vertex_position_buffer)
    {
        gfx_connection->destroy_buffer(this->m_vertex_position_buffer);
    }

    if (VK_NULL_HANDLE != this->m_vertex_varying_buffer)
    {
        gfx_connection->destroy_buffer(this->m_vertex_varying_buffer);
    }

    if (VK_NULL_HANDLE != this->m_index_buffer)
    {
        gfx_connection->destroy_buffer(this->m_index_buffer);
    }

    if (VK_NULL_HANDLE != this->m_vertex_position_gfx_malloc_device_memory)
    {
        gfx_connection->transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_free(this->m_vertex_position_gfx_malloc_page_handle, this->m_vertex_position_gfx_malloc_offset, this->m_vertex_position_gfx_malloc_size, this->m_vertex_position_gfx_malloc_device_memory);
    }

    if (VK_NULL_HANDLE != this->m_vertex_varying_gfx_malloc_device_memory)
    {
        gfx_connection->transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_free(this->m_vertex_varying_gfx_malloc_page_handle, this->m_vertex_varying_gfx_malloc_offset, this->m_vertex_varying_gfx_malloc_size, this->m_vertex_varying_gfx_malloc_device_memory);
    }

    if (VK_NULL_HANDLE != this->m_index_gfx_malloc_device_memory)
    {
        gfx_connection->transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_free(this->m_index_gfx_malloc_page_handle, this->m_index_gfx_malloc_offset, this->m_index_gfx_malloc_size, this->m_index_gfx_malloc_device_memory);
    }

    this->~gfx_mesh_vk();
    mcrt_aligned_free(this);
}