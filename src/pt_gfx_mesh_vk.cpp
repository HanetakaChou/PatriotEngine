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
#include "pt_gfx_mesh_vk.h"
#include <vulkan/vulkan.h>
#include <assert.h>
#include <new>
#include "pt_gfx_mesh_base_gltf_parse.h"
#include <pt_mcrt_log.h>

bool gfx_mesh_vk::read_input_stream(class gfx_connection_common *gfx_connection_base, uint32_t mesh_index, uint32_t material_index, char const *initial_filename, gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename), intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence), void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream))
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
        gfx_input_stream_ref input_stream;
        {
            class internal_input_stream_guard
            {
                gfx_input_stream_ref *const m_input_stream;
                void(PT_PTR *m_input_stream_destroy_callback)(gfx_input_stream_ref input_stream);

            public:
                inline internal_input_stream_guard(
                    gfx_input_stream_ref *input_stream,
                    char const *initial_filename,
                    gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename),
                    void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream))
                    : m_input_stream(input_stream),
                      m_input_stream_destroy_callback(input_stream_destroy_callback)
                {
                    (*m_input_stream) = input_stream_init_callback(initial_filename);
                }
                inline ~internal_input_stream_guard()
                {
                    if (gfx_input_stream_ref(-1) != (*m_input_stream))
                    {
                        m_input_stream_destroy_callback((*m_input_stream));
                    }
                }
            } instance_internal_input_stream_guard(&input_stream, initial_filename, input_stream_init_callback, input_stream_destroy_callback);

            if (PT_UNLIKELY(gfx_input_stream_ref(-1) == input_stream))
            {
                mcrt_atomic_store(&this->m_streaming_error, true);
                return false;
            }

            struct gltf_root my_gltf_root;
            if (PT_UNLIKELY(!gltf_parse_input_stream(
                    &my_gltf_root,
                    input_stream, input_stream_read_callback,
                    const_cast<void *>(static_cast<void const *>(initial_filename)), [](int line, int column, char const *msg, void *error_callback_data) -> void
                    { mcrt_log_print("%s:%i:%i: error: %s", static_cast<char const *>(error_callback_data), line, column, msg); })))
            {
                mcrt_atomic_store(&this->m_streaming_error, true);
                return false;
            }
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
            task_data->m_mesh_index = mesh_index;
            task_data->m_material_index = material_index;
            task_data->m_gfx_mesh = this;
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
        this->streaming_destroy_callback(gfx_connection_base);
    }

    return true;
}

mcrt_task_ref gfx_mesh_vk::read_input_stream_task_execute(mcrt_task_ref self)
{
    return NULL;
}

void gfx_mesh_vk::destroy(class gfx_connection_common *gfx_connection_base)
{
    class gfx_connection_vk *gfx_connection = static_cast<class gfx_connection_vk *>(gfx_connection_base);

    bool streaming_done;
    {
        // make sure this function happens before or after the gfx_streaming_object::streaming_done
        this->streaming_done_lock();

        streaming_status_t streaming_status = mcrt_atomic_load(&this->m_streaming_status);

        if (STREAMING_STATUS_STAGE_FIRST == streaming_status || STREAMING_STATUS_STAGE_SECOND == streaming_status || STREAMING_STATUS_STAGE_THIRD == streaming_status)
        {
            mcrt_atomic_store(&this->m_streaming_cancel, true);
            streaming_done = false;
        }
        else if (STREAMING_STATUS_DONE == streaming_status)
        {
            streaming_done = true;
        }
        else
        {
            assert(0);
            streaming_done = false;
        }

        this->streaming_done_unlock();
    }

    if (streaming_done)
    {
        //TODO: the object is used by the rendering system
        //

        //
        // race condition with the "streaming_done"

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
}

void gfx_mesh_vk::streaming_destroy_callback(class gfx_connection_common *gfx_connection_base)
{
    class gfx_connection_vk *gfx_connection = static_cast<class gfx_connection_vk *>(gfx_connection_base);

    PT_MAYBE_UNUSED bool streaming_cancel = mcrt_atomic_load(&this->m_streaming_cancel);
    assert(streaming_cancel);

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