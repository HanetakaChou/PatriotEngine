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
                        mcrt_atomic_store(&this->m_streaming_error, true);
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
                        mcrt_atomic_store(&this->m_streaming_error, true);
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
                        mcrt_atomic_store(&this->m_streaming_error, true);
                        return false;
                    }
                }
                assert(VK_NULL_HANDLE != this->m_index_gfx_malloc_device_memory);

                {
                    PT_MAYBE_UNUSED VkResult res_bind_buffer_memory = gfx_connection->bind_buffer_memory(this->m_index_buffer, this->m_index_gfx_malloc_device_memory, this->m_index_gfx_malloc_offset);
                    assert(VK_SUCCESS == res_bind_buffer_memory);
                }
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
    uint32_t streaming_throttling_index;
    bool recycle;
    // The "read_input_stream_task_execute_internal" makes sure that the "mcrt_task_decrement_ref_count" is called after all works are done
    mcrt_task_ref task_bypass = read_input_stream_task_execute_internal(&streaming_throttling_index, &recycle, self);

    if (!recycle)
    {
        static_assert(sizeof(read_input_stream_task_data) <= sizeof(mcrt_task_user_data_t), "");
        read_input_stream_task_data *task_data = reinterpret_cast<read_input_stream_task_data *>(mcrt_task_get_user_data(self));

        PT_MAYBE_UNUSED streaming_status_t streaming_status = mcrt_atomic_load(&task_data->m_gfx_mesh->m_streaming_status);
        PT_MAYBE_UNUSED bool streaming_error = mcrt_atomic_load(&task_data->m_gfx_mesh->m_streaming_error);
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

inline mcrt_task_ref gfx_mesh_vk::read_input_stream_task_execute_internal(uint32_t *output_streaming_throttling_index, bool *output_recycle, mcrt_task_ref self)
{
    static_assert(sizeof(read_input_stream_task_data) <= sizeof(mcrt_task_user_data_t), "");
    read_input_stream_task_data *task_data = reinterpret_cast<read_input_stream_task_data *>(mcrt_task_get_user_data(self));

    // different master task doesn't share the task_arena
    // we need to share the same the task arena to make sure the "tbb::this_task_arena::current_thread_id" unique
    assert(NULL != mcrt_task_arena_internal_arena(task_data->m_gfx_connection->task_arena()));
    assert(mcrt_task_arena_internal_arena(task_data->m_gfx_connection->task_arena()) == mcrt_this_task_arena_internal_arena());

    PT_MAYBE_UNUSED streaming_status_t streaming_status = mcrt_atomic_load(&task_data->m_gfx_mesh->m_streaming_status);
    PT_MAYBE_UNUSED bool streaming_error = mcrt_atomic_load(&task_data->m_gfx_mesh->m_streaming_error);
    bool streaming_cancel = mcrt_atomic_load(&task_data->m_gfx_mesh->m_streaming_cancel);
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

                        size_t total_size = (input_vetex_position_length + input_vetex_texture_0_length + input_index_length);

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

                // pass to the third stage
                {
                    task_data->m_gfx_connection->streaming_object_list_push(streaming_throttling_index, task_data->m_gfx_mesh);
                }

                // load
                int vetex_position_offset = 0;
                int vetex_texture_0_offset = input_vetex_position_length;
                int index_offset = (input_vetex_position_length + input_vetex_texture_0_length);

                // vertex position 
                {
                    int64_t res_seek = task_data->m_input_stream_seek_callback(input_stream, input_vetex_position_offset, PT_GFX_INPUT_STREAM_SEEK_SET);
                    if (res_seek == -1 || res_seek != input_vetex_position_offset)
                    {
                        mcrt_atomic_store(&task_data->m_gfx_mesh->m_streaming_error, true);
                        (*output_streaming_throttling_index) = streaming_throttling_index;
                        (*output_recycle) = false;
                        return NULL;
                    }
                    ptrdiff_t res_read = task_data->m_input_stream_read_callback(input_stream, reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(task_data->m_gfx_connection->transfer_src_buffer_pointer()) + static_cast<uintptr_t>(base_offset) + vetex_position_offset), input_vetex_position_length);
                    if (res_read == -1 || res_read != input_vetex_position_length)
                    {
                        mcrt_atomic_store(&task_data->m_gfx_mesh->m_streaming_error, true);
                        (*output_streaming_throttling_index) = streaming_throttling_index;
                        (*output_recycle) = false;
                        return NULL;
                    }
                }

                // vertex texture_0
                {
                    int64_t res_seek = task_data->m_input_stream_seek_callback(input_stream, input_vetex_texture_0_offset, PT_GFX_INPUT_STREAM_SEEK_SET);
                    if (res_seek == -1 || res_seek != input_vetex_texture_0_offset)
                    {
                        mcrt_atomic_store(&task_data->m_gfx_mesh->m_streaming_error, true);
                        (*output_streaming_throttling_index) = streaming_throttling_index;
                        (*output_recycle) = false;
                        return NULL;
                    }
                    ptrdiff_t res_read = task_data->m_input_stream_read_callback(input_stream, reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(task_data->m_gfx_connection->transfer_src_buffer_pointer()) + static_cast<uintptr_t>(base_offset) + vetex_texture_0_offset), input_vetex_texture_0_length);
                    if (res_read == -1 || res_read != input_vetex_texture_0_length)
                    {
                        mcrt_atomic_store(&task_data->m_gfx_mesh->m_streaming_error, true);
                        (*output_streaming_throttling_index) = streaming_throttling_index;
                        (*output_recycle) = false;
                        return NULL;
                    }
                }

                // index
                {
                    int64_t res_seek = task_data->m_input_stream_seek_callback(input_stream, input_index_offset, PT_GFX_INPUT_STREAM_SEEK_SET);
                    if (res_seek == -1 || res_seek != input_index_offset)
                    {
                        mcrt_atomic_store(&task_data->m_gfx_mesh->m_streaming_error, true);
                        (*output_streaming_throttling_index) = streaming_throttling_index;
                        (*output_recycle) = false;
                        return NULL;
                    }
                    ptrdiff_t res_read = task_data->m_input_stream_read_callback(input_stream, reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(task_data->m_gfx_connection->transfer_src_buffer_pointer()) + static_cast<uintptr_t>(base_offset) + index_offset), input_index_length);
                    if (res_read == -1 || res_read != input_index_length)
                    {
                        mcrt_atomic_store(&task_data->m_gfx_mesh->m_streaming_error, true);
                        (*output_streaming_throttling_index) = streaming_throttling_index;
                        (*output_recycle) = false;
                        return NULL;
                    }
                }

                // cmd copy
                {
                    uint32_t streaming_thread_index = mcrt_this_task_arena_current_thread_index();
                    VkBuffer transfer_src_buffer = task_data->m_gfx_connection->transfer_src_buffer();
                    {
                        VkBufferCopy regions[1];
                        regions[0].srcOffset = base_offset + vetex_position_offset;
                        regions[0].dstOffset = 0U;
                        regions[0].size = input_vetex_position_length;
                        task_data->m_gfx_connection->copy_vertex_buffer(streaming_throttling_index, streaming_thread_index, transfer_src_buffer, task_data->m_gfx_mesh->m_vertex_position_buffer, 1U, regions);
                    }
                    {
                        VkBufferCopy regions[1];
                        regions[0].srcOffset = base_offset + vetex_texture_0_offset;
                        regions[0].dstOffset = 0U;
                        regions[0].size = input_vetex_texture_0_length;
                        task_data->m_gfx_connection->copy_vertex_buffer(streaming_throttling_index, streaming_thread_index, transfer_src_buffer, task_data->m_gfx_mesh->m_vertex_varying_buffer, 1U, regions);
                    }
                    {
                        VkBufferCopy regions[1];
                        regions[0].srcOffset = base_offset + index_offset;
                        regions[0].dstOffset = 0U;
                        regions[0].size = input_index_length;
                        task_data->m_gfx_connection->copy_vertex_buffer(streaming_throttling_index, streaming_thread_index, transfer_src_buffer, task_data->m_gfx_mesh->m_index_buffer, 1U, regions);
                    }
                }

                mcrt_atomic_store(&task_data->m_gfx_mesh->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
            }
        }
        else
        {
            // The slob allocator is serial which is harmful to the parallel performance
            // leave the "steaming_cancel" to the third stage
            // tracker by "slob_lock_busy_count"
#if 0
            task_data->m_gfx_mesh->streaming_destroy_callback();
#else
            // pass to the third stage
            {
                task_data->m_gfx_connection->streaming_object_list_push(streaming_throttling_index, task_data->m_gfx_mesh);
            }

            mcrt_atomic_store(&task_data->m_gfx_mesh->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
#endif
        }
    }

    (*output_streaming_throttling_index) = streaming_throttling_index;
    (*output_recycle) = false;
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