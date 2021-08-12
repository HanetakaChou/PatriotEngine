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
#include <pt_mcrt_atomic.h>
#include <pt_mcrt_malloc.h>
#include "pt_gfx_buffer_vk.h"
#include <vulkan/vulkan.h>
#include <assert.h>
#include <new>

bool gfx_buffer_vk::read_vertex_input_stream(class gfx_connection_common *gfx_connection_base, gfx_buffer_ref buffer, int64_t byte_offset, int64_t byte_length, char const *initial_filename, gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename), intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence), void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream))
{
    class gfx_connection_vk *gfx_connection = static_cast<class gfx_connection_vk *>(gfx_connection_base);

    // How to implement pipeline "serial - parallel - serial"
    // follow [McCool 2012] "Structured Parallel Programming: Patterns for Efficient Computation." / 9.4.2 Pipeline in Cilk Plus
    // the second stage is trivially implemented by the task spawned by the first stage
    // the third stage can be implemented as "reduce"

    // We don't have the third stage here

    // the first stage

    PT_MAYBE_UNUSED streaming_status_t streaming_status = mcrt_atomic_load(&this->m_streaming_status);
    PT_MAYBE_UNUSED bool streaming_error = mcrt_atomic_load(&this->m_streaming_error);
    bool streaming_cancel = mcrt_atomic_load(&this->m_streaming_cancel);
    assert(STREAMING_STATUS_STAGE_FIRST == streaming_status);
    assert(!streaming_error);

    if (!streaming_cancel)
    {
        assert(VK_NULL_HANDLE == this->m_buffer);
        {
            VkBufferCreateInfo buffer_create_info;
            buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_create_info.pNext = NULL;
            buffer_create_info.flags = 0U;
            buffer_create_info.size = byte_length;
            buffer_create_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            buffer_create_info.queueFamilyIndexCount = 0U;
            buffer_create_info.pQueueFamilyIndices = NULL;

            PT_MAYBE_UNUSED VkResult res = gfx_connection->create_buffer(&buffer_create_info, &this->m_buffer);
            assert(VK_SUCCESS == res);
        }

        assert(VK_NULL_HANDLE == this->m_gfx_malloc_device_memory);
        {
            VkMemoryRequirements memory_requirements;
            gfx_connection->get_buffer_memory_requirements(this->m_buffer, &memory_requirements);

            // vkAllocateMemory
            // https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/vkspec.html#fundamentals-threadingbehavior

            this->m_gfx_malloc_device_memory = gfx_connection->transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_alloc(&memory_requirements, &this->m_gfx_malloc_page_handle, &this->m_gfx_malloc_offset, &this->m_gfx_malloc_size);
            if (PT_UNLIKELY(VK_NULL_HANDLE == this->m_gfx_malloc_device_memory))
            {
                gfx_connection->destroy_buffer(this->m_buffer);
                this->m_buffer = VK_NULL_HANDLE;
                mcrt_atomic_store(&this->m_streaming_error, true);
                return false;
            }
        }
        assert(VK_NULL_HANDLE != this->m_gfx_malloc_device_memory);

        {
            PT_MAYBE_UNUSED VkResult res_bind_buffer_memory = gfx_connection->bind_buffer_memory(this->m_buffer, this->m_gfx_malloc_device_memory, this->m_gfx_malloc_offset);
            assert(VK_SUCCESS == res_bind_buffer_memory);
        }

        // pass to the second stage
        {
            mcrt_task_ref task = mcrt_task_allocate_root(read_input_stream_task_execute);
            static_assert(sizeof(read_input_stream_task_data) <= sizeof(mcrt_task_user_data_t), "");
            read_input_stream_task_data *task_data = reinterpret_cast<read_input_stream_task_data *>(mcrt_task_get_user_data(task));
            // There is no constructor of the POD "mcrt_task_user_data_t"
            new (&task_data->m_initial_filename) mcrt_string(initial_filename);
            task_data->m_input_stream_init_callback = input_stream_init_callback;
            task_data->m_input_stream_read_callback = input_stream_read_callback;
            task_data->m_input_stream_seek_callback = input_stream_seek_callback;
            task_data->m_input_stream_destroy_callback = input_stream_destroy_callback;
            task_data->m_gfx_buffer = this;
            task_data->m_gfx_connection = gfx_connection;

            mcrt_atomic_store(&this->m_streaming_status, STREAMING_STATUS_STAGE_SECOND);

            // different master task doesn't share the task_arena
            // we need to share the same the task arena to make sure the "tbb::this_task_arena::current_thread_id" unique
            //mcrt_task_spawn(task);
            mcrt_task_enqueue(task, gfx_connection->task_arena());
        }
    }
    else
    {
        // race condition with the "streaming_done"
        // inevitable since the "transfer_dst_and_sampled_image_alloc" nervertheless races with the "streaming_done"
        this->streaming_cancel(gfx_connection_base);
    }

    return true;
}
