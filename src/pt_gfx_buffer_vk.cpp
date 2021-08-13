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

bool gfx_buffer_vk::read_vertex_input_stream(class gfx_connection_common *gfx_connection, gfx_buffer_ref buffer, int64_t input_stream_offset, int64_t input_stream_length, char const *initial_filename, gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename), intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence), void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream))
{
    return this->read_input_stream_internal(gfx_connection, buffer, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, input_stream_offset, input_stream_length, initial_filename, input_stream_init_callback, input_stream_read_callback, input_stream_seek_callback, input_stream_destroy_callback);
}

bool gfx_buffer_vk::read_index_input_stream(class gfx_connection_common *gfx_connection, gfx_buffer_ref buffer, int64_t input_stream_offset, int64_t input_stream_length, char const *initial_filename, gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename), intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence), void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream))
{
    return this->read_input_stream_internal(gfx_connection, buffer, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, input_stream_offset, input_stream_length, initial_filename, input_stream_init_callback, input_stream_read_callback, input_stream_seek_callback, input_stream_destroy_callback);
}

bool gfx_buffer_vk::read_input_stream_internal(class gfx_connection_common *gfx_connection_base, gfx_buffer_ref buffer, VkBufferUsageFlags buffer_usage, int64_t input_stream_offset, int64_t input_stream_length, char const *initial_filename, gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename), intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence), void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream))
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
            buffer_create_info.size = input_stream_length;
            buffer_create_info.usage = buffer_usage;
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
            task_data->m_input_stream_offset = input_stream_offset;
            task_data->m_input_stream_length = input_stream_length;
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
        this->streaming_destroy_callback(gfx_connection_base);
    }

    return true;
}

mcrt_task_ref gfx_buffer_vk::read_input_stream_task_execute(mcrt_task_ref self)
{
    uint32_t streaming_throttling_index;
    bool recycle;
    // The "read_input_stream_task_execute_internal" makes sure that the "mcrt_task_decrement_ref_count" is called after all works are done
    mcrt_task_ref task_bypass = read_input_stream_task_execute_internal(&streaming_throttling_index, &recycle, self);

    if (!recycle)
    {
        static_assert(sizeof(read_input_stream_task_data) <= sizeof(mcrt_task_user_data_t), "");
        read_input_stream_task_data *task_data = reinterpret_cast<read_input_stream_task_data *>(mcrt_task_get_user_data(self));

        PT_MAYBE_UNUSED streaming_status_t streaming_status = mcrt_atomic_load(&task_data->m_gfx_buffer->m_streaming_status);
        PT_MAYBE_UNUSED bool streaming_error = mcrt_atomic_load(&task_data->m_gfx_buffer->m_streaming_error);
        assert(streaming_error || STREAMING_STATUS_STAGE_THIRD == streaming_status);
        assert(!streaming_error || STREAMING_STATUS_STAGE_SECOND == streaming_status);

        // There is no destructor of the POD "mcrt_task_user_data_t"
        (&task_data->m_initial_filename)->~mcrt_string();
    }
    else
    {
        static_assert(sizeof(read_input_stream_task_data) <= sizeof(mcrt_task_user_data_t), "");
        read_input_stream_task_data *task_data = reinterpret_cast<read_input_stream_task_data *>(mcrt_task_get_user_data(self));

        // tally_completion_of_predecessor manually
        mcrt_task_decrement_ref_count(task_data->m_gfx_connection->streaming_task_root(streaming_throttling_index));
    }

    return task_bypass;
}

inline mcrt_task_ref gfx_buffer_vk::read_input_stream_task_execute_internal(uint32_t *output_streaming_throttling_index, bool *output_recycle, mcrt_task_ref self)
{
    static_assert(sizeof(read_input_stream_task_data) <= sizeof(mcrt_task_user_data_t), "");
    read_input_stream_task_data *task_data = reinterpret_cast<read_input_stream_task_data *>(mcrt_task_get_user_data(self));

    // different master task doesn't share the task_arena
    // we need to share the same the task arena to make sure the "tbb::this_task_arena::current_thread_id" unique
    assert(NULL != mcrt_task_arena_internal_arena(task_data->m_gfx_connection->task_arena()));
    assert(mcrt_task_arena_internal_arena(task_data->m_gfx_connection->task_arena()) == mcrt_this_task_arena_internal_arena());

    PT_MAYBE_UNUSED streaming_status_t streaming_status = mcrt_atomic_load(&task_data->m_gfx_buffer->m_streaming_status);
    PT_MAYBE_UNUSED bool streaming_error = mcrt_atomic_load(&task_data->m_gfx_buffer->m_streaming_error);
    bool streaming_cancel = mcrt_atomic_load(&task_data->m_gfx_buffer->m_streaming_cancel);
    assert(STREAMING_STATUS_STAGE_SECOND == streaming_status);
    assert(!streaming_error);

    // race condition
    // task: load streaming_throttling_index
    // reduce_streaming_task: store ++streaming_throttling_index
    // reduce_streaming_task: mcrt_task_wait_for_all // return immediately
    // task: mcrt_task_increment_ref_count
    task_data->m_gfx_connection->streaming_throttling_index_lock();
    uint32_t streaming_throttling_index = task_data->m_gfx_connection->streaming_throttling_index();
    mcrt_task_increment_ref_count(task_data->m_gfx_connection->streaming_task_root(streaming_throttling_index));
    task_data->m_gfx_connection->streaming_throttling_index_unlock();

    {
#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
        class internal_streaming_task_debug_executing_guard
        {
            uint32_t m_streaming_throttling_index;
            class gfx_connection_vk *m_gfx_connection;

        public:
            inline internal_streaming_task_debug_executing_guard(uint32_t streaming_throttling_index, class gfx_connection_vk *gfx_connection)
                : m_streaming_throttling_index(streaming_throttling_index), m_gfx_connection(gfx_connection)
            {
                m_gfx_connection->streaming_task_debug_executing_begin(m_streaming_throttling_index);
            }
            inline ~internal_streaming_task_debug_executing_guard()
            {
                m_gfx_connection->streaming_task_debug_executing_end(m_streaming_throttling_index);
            }
        } instance_internal_streaming_task_debug_executing_guard(streaming_throttling_index, task_data->m_gfx_connection);
#endif

        mcrt_task_set_parent(self, task_data->m_gfx_connection->streaming_task_root(streaming_throttling_index));

        if (!streaming_cancel)
        {
            gfx_input_stream_ref input_stream;
            {
                class gfx_input_stream_guard
                {
                    gfx_input_stream_ref &m_input_stream;
                    void(PT_PTR *m_input_stream_destroy_callback)(gfx_input_stream_ref input_stream);

                public:
                    inline gfx_input_stream_guard(
                        gfx_input_stream_ref &input_stream,
                        char const *initial_filename,
                        gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename),
                        void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream))
                        : m_input_stream(input_stream),
                          m_input_stream_destroy_callback(input_stream_destroy_callback)
                    {
                        m_input_stream = input_stream_init_callback(initial_filename);
                    }
                    inline ~gfx_input_stream_guard()
                    {
                        m_input_stream_destroy_callback(m_input_stream);
                    }
                } input_stream_guard(input_stream, task_data->m_initial_filename.c_str(), task_data->m_input_stream_init_callback, task_data->m_input_stream_destroy_callback);

                int64_t res_seek =  task_data->m_input_stream_seek_callback(input_stream, task_data->m_input_stream_offset, PT_GFX_INPUT_STREAM_SEEK_SET);
                if (res_seek == -1 || res_seek != task_data->m_input_stream_offset)
                {
                    mcrt_atomic_store(&task_data->m_gfx_buffer->m_streaming_error, true);
                    (*output_streaming_throttling_index) = streaming_throttling_index;
                    (*output_recycle) = false;
                    return NULL;
                }

                // base_offset
                uint64_t base_offset = uint64_t(-1);
                {
                    uint64_t transfer_src_buffer_begin = task_data->m_gfx_connection->transfer_src_buffer_begin(streaming_throttling_index);
                    uint64_t transfer_src_buffer_end = uint64_t(-1);
                    uint64_t transfer_src_buffer_size = task_data->m_gfx_connection->transfer_src_buffer_size(streaming_throttling_index);

                    // allocate memory
                    do
                    {
                        base_offset = mcrt_atomic_load(task_data->m_gfx_connection->transfer_src_buffer_end(streaming_throttling_index));

                        size_t total_size = task_data->m_input_stream_length;

                        transfer_src_buffer_end = (base_offset + total_size);
                        //assert((transfer_src_buffer_end - transfer_src_buffer_begin) <= transfer_src_buffer_size);

                        // respawn if memory not enough
                        if ((transfer_src_buffer_end - transfer_src_buffer_begin) > transfer_src_buffer_size)
                        {
                            // recycle to prevent free_task
                            mcrt_task_recycle_as_child_of(self, task_data->m_gfx_connection->streaming_task_respawn_root());

                            task_data->m_gfx_connection->streaming_task_respawn_list_push(streaming_throttling_index, self);

                            // recycle needs manually tally_completion_of_predecessor
                            (*output_streaming_throttling_index) = streaming_throttling_index;
                            (*output_recycle) = true;
                            return NULL;
                        }

                    } while (base_offset != mcrt_atomic_cas_u64(task_data->m_gfx_connection->transfer_src_buffer_end(streaming_throttling_index), transfer_src_buffer_end, base_offset));
                }

                // load
                ptrdiff_t res_read = task_data->m_input_stream_read_callback(input_stream, reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(task_data->m_gfx_connection->transfer_src_buffer_pointer()) + static_cast<uintptr_t>(base_offset)), task_data->m_input_stream_length);
                if (res_read == -1 || res_read != task_data->m_input_stream_length)
                {
                    mcrt_atomic_store(&task_data->m_gfx_buffer->m_streaming_error, true);
                    (*output_streaming_throttling_index) = streaming_throttling_index;
                    (*output_recycle) = false;
                    return NULL;
                }

                // cmd copy
                {
                    uint32_t streaming_thread_index = mcrt_this_task_arena_current_thread_index();
                    VkBuffer transfer_src_buffer = task_data->m_gfx_connection->transfer_src_buffer();
                    VkBufferCopy regions[1];
                    regions[0].srcOffset = base_offset;
                    regions[0].dstOffset = 0U;
                    regions[0].size = task_data->m_input_stream_length;
                    task_data->m_gfx_connection->copy_buffer(streaming_throttling_index, streaming_thread_index, transfer_src_buffer, task_data->m_gfx_buffer->m_buffer, 1U, regions);
                }

                mcrt_atomic_store(&task_data->m_gfx_buffer->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
            }
        }
        else
        {
            // The slob allocator is serial which is harmful to the parallel performance
            // leave the "steaming_cancel" to the third stage
            // tracker by "slob_lock_busy_count"
#if 0
            task_data->m_gfx_buffer->streaming_destroy_callback();
#else
            // pass to the third stage
            {
                task_data->m_gfx_connection->streaming_object_list_push(streaming_throttling_index, task_data->m_gfx_buffer);
            }

            mcrt_atomic_store(&task_data->m_gfx_buffer->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
#endif
        }
    }

    (*output_streaming_throttling_index) = streaming_throttling_index;
    (*output_recycle) = false;
    return NULL;
}


void gfx_buffer_vk::destroy(class gfx_connection_common *gfx_connection_base)
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

        if (VK_NULL_HANDLE != this->m_gfx_malloc_device_memory)
        {
            gfx_connection->transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_free(this->m_gfx_malloc_page_handle, this->m_gfx_malloc_offset, this->m_gfx_malloc_size, this->m_gfx_malloc_device_memory);
        }

        if (VK_NULL_HANDLE != this->m_buffer)
        {
            gfx_connection->destroy_buffer(this->m_buffer);
        }

        this->~gfx_buffer_vk();
        mcrt_aligned_free(this);
    }
}

void gfx_buffer_vk::streaming_destroy_callback(class gfx_connection_common *gfx_connection_base)
{
    class gfx_connection_vk *gfx_connection = static_cast<class gfx_connection_vk *>(gfx_connection_base);

    PT_MAYBE_UNUSED bool streaming_cancel = mcrt_atomic_load(&this->m_streaming_cancel);
    assert(streaming_cancel);

    if (VK_NULL_HANDLE != this->m_gfx_malloc_device_memory)
    {
        gfx_connection->transfer_dst_and_vertex_buffer_or_transfer_dst_and_index_buffer_free(this->m_gfx_malloc_page_handle, this->m_gfx_malloc_offset, this->m_gfx_malloc_size, this->m_gfx_malloc_device_memory);
    }

    if (VK_NULL_HANDLE != this->m_buffer)
    {
        gfx_connection->destroy_buffer(this->m_buffer);
    }

    this->~gfx_buffer_vk();
    mcrt_aligned_free(this);
}