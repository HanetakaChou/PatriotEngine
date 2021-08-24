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
#include <assert.h>
#include "pt_gfx_mesh_base_gltf_parse.h"

bool gfx_mesh_vk::read_input_stream(class gfx_connection_base *gfx_connection, uint32_t mesh_index, uint32_t material_index, char const *initial_filename, gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename), intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence), void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream))
{
    struct mesh_streaming_stage_first_thread_stack_data_t thread_stack_user_defined;
    thread_stack_user_defined.m_mesh_index = mesh_index;
    thread_stack_user_defined.m_material_index = material_index;
    return this->streaming_stage_first_execute(gfx_connection, initial_filename, input_stream_init_callback, input_stream_read_callback, input_stream_seek_callback, input_stream_destroy_callback, &thread_stack_user_defined);
}

bool gfx_mesh_vk::streaming_stage_first_pre_populate_task_data_callback(class gfx_connection_base *gfx_connection_base, gfx_input_stream_ref input_stream, intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref, void *, size_t), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref, int64_t, int), void *thread_stack_user_defined_void)
{
    class gfx_connection_vk *gfx_connection = static_cast<class gfx_connection_vk *>(gfx_connection_base);

    //struct gltf_root my_gltf_root;
    //if (PT_UNLIKELY(!gltf_parse_input_stream(
    //        &my_gltf_root,
    //        input_stream, input_stream_read_callback,
    //        const_cast<void *>(static_cast<void const *>(initial_filename)), [](int line, int column, char const *msg, void *error_callback_data) -> void
    //        { mcrt_log_print("%s:%i:%i: error: %s", static_cast<char const *>(error_callback_data), line, column, msg); })))
    //{
    //    mcrt_atomic_store(&this->m_streaming_error, true);
    //    return false;
    //}

    //int input_vertex_count = 36;
    //int vetex_position_type = 12; //float vec3
    //int input_vetex_position_offset = 132;
    int input_vetex_position_length = 432;
    //int m_vetex_texture_0_type = 8; //float vec2
    //int input_vetex_texture_0_offset = 1572;
    int input_vetex_texture_0_length = 288;

    //int index_type = 2; //uint16
    //int input_index_count = 36;
    //int input_index_offset = 60;
    int input_index_length = 72;

    // Vertex Position
    {
        assert(VK_NULL_HANDLE == this->m_vertex_position_buffer);
        {
            VkBufferCreateInfo buffer_create_info;
            buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_create_info.pNext = NULL;
            buffer_create_info.flags = 0U;
            buffer_create_info.size = input_vetex_position_length;
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
            buffer_create_info.size = input_vetex_texture_0_length;
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
            buffer_create_info.size = input_index_length;
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

void gfx_mesh_vk::streaming_stage_first_populate_task_data_callback(void *thread_stack_user_defined_void, struct streaming_stage_second_task_data_user_defined_t *task_data_user_defined_void)
{
    struct mesh_streaming_stage_first_thread_stack_data_t *thread_stack_user_defined = reinterpret_cast<struct mesh_streaming_stage_first_thread_stack_data_t *>(thread_stack_user_defined_void);
    static_assert(sizeof(struct mesh_streaming_stage_first_thread_stack_data_t) <= sizeof(struct streaming_stage_second_thread_stack_data_user_defined_t), "");
    static_assert(sizeof(struct mesh_streaming_stage_second_task_data_t) <= sizeof(struct streaming_stage_second_task_data_user_defined_t), "");
    struct mesh_streaming_stage_second_task_data_t *task_data_user_defined = reinterpret_cast<struct mesh_streaming_stage_second_task_data_t *>(task_data_user_defined_void);

    task_data_user_defined->m_mesh_index = thread_stack_user_defined->m_mesh_index;
    task_data_user_defined->m_material_index = thread_stack_user_defined->m_material_index;
}

bool gfx_mesh_vk::streaming_stage_second_pre_calculate_total_size_callback(struct streaming_stage_second_thread_stack_data_user_defined_t *thread_stack_data_user_defined_void, gfx_input_stream_ref input_stream, intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref, void *, size_t), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref, int64_t, int), struct streaming_stage_second_task_data_user_defined_t *task_data_user_defined_void)
{
    return true;
}

size_t gfx_mesh_vk::streaming_stage_second_calculate_total_size_callback(uint64_t base_offset, struct streaming_stage_second_thread_stack_data_user_defined_t *thread_stack_data_user_defined_void, class gfx_connection_base *gfx_connection, struct streaming_stage_second_task_data_user_defined_t *task_data_user_defined_void)
{
    static_assert(sizeof(struct mesh_streaming_stage_second_thread_stack_data_t) <= sizeof(struct streaming_stage_second_thread_stack_data_user_defined_t), "");
    struct mesh_streaming_stage_second_thread_stack_data_t *thread_stack_data_user_defined = reinterpret_cast<struct mesh_streaming_stage_second_thread_stack_data_t *>(thread_stack_data_user_defined_void);

    //int input_vertex_count = 36;
    //int m_vetex_position_type = 12; //float vec3
    //int input_vetex_position_offset = 132;
    int input_vetex_position_length = 432;
    //int m_vetex_texture_0_type = 8; //float vec2
    //int input_vetex_texture_0_offset = 1572;
    int input_vetex_texture_0_length = 288;

    //int m_index_type = 2; //uint16
    //int input_index_count = 36;
    //int input_index_offset = 60;
    int input_index_length = 72;

    //mode 4 //triangle list
    //

    size_t total_size = (input_vetex_position_length + input_vetex_texture_0_length + input_index_length);
    thread_stack_data_user_defined->m_base_offset = base_offset;
    return total_size;
}

bool gfx_mesh_vk::streaming_stage_second_post_calculate_total_size_callback(bool staging_buffer_allocate_success, uint32_t streaming_throttling_index, struct streaming_stage_second_thread_stack_data_user_defined_t *thread_stack_data_user_defined_void, class gfx_connection_base *gfx_connection_base, gfx_input_stream_ref input_stream, intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref, void *, size_t), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref, int64_t, int), struct streaming_stage_second_task_data_user_defined_t *task_data_user_defined_void)
{
    class gfx_connection_vk *gfx_connection = static_cast<class gfx_connection_vk *>(gfx_connection_base);
    static_assert(sizeof(struct mesh_streaming_stage_second_thread_stack_data_t) <= sizeof(struct streaming_stage_second_thread_stack_data_user_defined_t), "");
    struct mesh_streaming_stage_second_thread_stack_data_t *thread_stack_data_user_defined = reinterpret_cast<struct mesh_streaming_stage_second_thread_stack_data_t *>(thread_stack_data_user_defined_void);

    //int input_vertex_count = 36;
    //int m_vetex_position_type = 12; //float vec3
    int input_vetex_position_offset = 132;
    int input_vetex_position_length = 432;
    //int m_vetex_texture_0_type = 8; //float vec2
    int input_vetex_texture_0_offset = 1572;
    int input_vetex_texture_0_length = 288;

    //int m_index_type = 2; //uint16
    //int input_index_count = 36;
    int input_index_offset = 60;
    int input_index_length = 72;

    //mode 4 //triangle list
    //

    // load
    int vetex_position_offset = 0;
    int vetex_texture_0_offset = input_vetex_position_length;
    int index_offset = (input_vetex_position_length + input_vetex_texture_0_length);

    // vertex position
    {
        int64_t res_seek = input_stream_seek_callback(input_stream, input_vetex_position_offset, PT_GFX_INPUT_STREAM_SEEK_SET);
        if (res_seek == -1 || res_seek != input_vetex_position_offset)
        {
            return false;
        }
        ptrdiff_t res_read = input_stream_read_callback(input_stream, reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(gfx_connection->transfer_src_buffer_pointer()) + static_cast<uintptr_t>(thread_stack_data_user_defined->m_base_offset) + vetex_position_offset), input_vetex_position_length);
        if (res_read == -1 || res_read != input_vetex_position_length)
        {
            return false;
        }
    }

    // vertex texture_0
    {
        int64_t res_seek = input_stream_seek_callback(input_stream, input_vetex_texture_0_offset, PT_GFX_INPUT_STREAM_SEEK_SET);
        if (res_seek == -1 || res_seek != input_vetex_texture_0_offset)
        {
            return false;
        }
        ptrdiff_t res_read = input_stream_read_callback(input_stream, reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(gfx_connection->transfer_src_buffer_pointer()) + static_cast<uintptr_t>(thread_stack_data_user_defined->m_base_offset) + vetex_texture_0_offset), input_vetex_texture_0_length);

        float *texcoord0 = reinterpret_cast<float(*)>(reinterpret_cast<uintptr_t>(gfx_connection->transfer_src_buffer_pointer()) + static_cast<uintptr_t>(thread_stack_data_user_defined->m_base_offset) + vetex_texture_0_offset);
        for (int i = 0; i < 72; ++i)
        {
            texcoord0[i] = ((texcoord0[i] > 0) ? texcoord0[i] : (-texcoord0[i]));
        }

        if (res_read == -1 || res_read != input_vetex_texture_0_length)
        {
            return false;
        }
    }

    // index
    {
        int64_t res_seek = input_stream_seek_callback(input_stream, input_index_offset, PT_GFX_INPUT_STREAM_SEEK_SET);
        if (res_seek == -1 || res_seek != input_index_offset)
        {
            return false;
        }
        ptrdiff_t res_read = input_stream_read_callback(input_stream, reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(gfx_connection->transfer_src_buffer_pointer()) + static_cast<uintptr_t>(thread_stack_data_user_defined->m_base_offset) + index_offset), input_index_length);
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
            regions[0].srcOffset = thread_stack_data_user_defined->m_base_offset + vetex_position_offset;
            regions[0].dstOffset = 0U;
            regions[0].size = input_vetex_position_length;
            gfx_connection->copy_vertex_buffer(streaming_throttling_index, streaming_thread_index, transfer_src_buffer, this->m_vertex_position_buffer, 1U, regions);
        }
        {
            VkBufferCopy regions[1];
            regions[0].srcOffset = thread_stack_data_user_defined->m_base_offset + vetex_texture_0_offset;
            regions[0].dstOffset = 0U;
            regions[0].size = input_vetex_texture_0_length;
            gfx_connection->copy_vertex_buffer(streaming_throttling_index, streaming_thread_index, transfer_src_buffer, this->m_vertex_varying_buffer, 1U, regions);
        }
        {
            VkBufferCopy regions[1];
            regions[0].srcOffset = thread_stack_data_user_defined->m_base_offset + index_offset;
            regions[0].dstOffset = 0U;
            regions[0].size = input_index_length;
            gfx_connection->copy_vertex_buffer(streaming_throttling_index, streaming_thread_index, transfer_src_buffer, this->m_index_buffer, 1U, regions);
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