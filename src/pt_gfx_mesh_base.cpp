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

#include <stddef.h>
#include <stdint.h>
#include <pt_mcrt_atomic.h>
#include "pt_gfx_mesh_base.h"
#include "pt_gfx_mesh_base_load.h"
#include <assert.h>

struct read_input_stream_task_data_t
{
    class gfx_mesh_base *m_gfx_streaming_object;
    class gfx_connection_base *m_gfx_connection;
    pt_gfx_input_stream_ref m_gfx_input_stream;
    struct pt_gfx_input_stream_callbacks_t m_gfx_input_stream_callbacks;
};
static_assert(sizeof(struct read_input_stream_task_data_t) <= sizeof(mcrt_task_user_data_t), "");
inline struct read_input_stream_task_data_t *unwrap(mcrt_task_user_data_t *task_data) { return reinterpret_cast<struct read_input_stream_task_data_t *>(task_data); }

// first stage
// 1-1. mesh load header - base
// 1-2. alloc backend - callback
// 1-3. populate task data - base
bool gfx_mesh_base::read_input_stream_task_spawn(class gfx_connection_base *gfx_connection, char const *file_name, struct pt_gfx_input_stream_callbacks_t const *gfx_input_stream_callbacks)
{
    // How to implement pipeline "serial - parallel - serial"
    // follow [McCool 2012] "Structured Parallel Programming: Patterns for Efficient Computation." / 9.4.2 Pipeline in Cilk Plus
    // the second stage is trivially implemented by the task spawned by the first stage
    // the third stage can be implemented as "reduce"

    // We can consider the "reduce_streaming_task" as the third stage

    // 1. first stage

    PT_MAYBE_UNUSED streaming_status_t streaming_status = mcrt_atomic_load(&this->m_streaming_status);
    PT_MAYBE_UNUSED bool streaming_error = mcrt_atomic_load(&this->m_streaming_error);
    bool streaming_cancel = mcrt_atomic_load(&this->m_streaming_cancel);
    assert(STREAMING_STATUS_STAGE_FIRST == streaming_status);
    assert(!streaming_error);

    if (!streaming_cancel)
    {
        pt_gfx_input_stream_ref gfx_input_stream = gfx_input_stream_callbacks->pfn_create(file_name);
        if (pt_gfx_input_stream_ref(-1) == gfx_input_stream)
        {
            mcrt_atomic_store(&this->m_streaming_error, true);
            mcrt_atomic_store(&this->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
            gfx_connection->streaming_throttling_index_lock();
            uint32_t streaming_throttling_index = gfx_connection->streaming_throttling_index();
            gfx_connection->streaming_object_list_push(streaming_throttling_index, this);
            gfx_connection->streaming_throttling_index_unlock();
            return false;
        }

        // 1-1. load header
        struct pt_gfx_mesh_neutral_header_t neutral_header;
        if (!mesh_load_header_from_input_stream(&neutral_header, gfx_input_stream, gfx_input_stream_callbacks))
        {
            mcrt_atomic_store(&this->m_streaming_error, true);
            mcrt_atomic_store(&this->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
            gfx_connection->streaming_throttling_index_lock();
            uint32_t streaming_throttling_index = gfx_connection->streaming_throttling_index();
            gfx_connection->streaming_object_list_push(streaming_throttling_index, this);
            gfx_connection->streaming_throttling_index_unlock();
            return false;
        }

        struct pt_gfx_mesh_neutral_primitive_header_t *neutral_primitive_headers = static_cast<struct pt_gfx_mesh_neutral_primitive_header_t *>(mcrt_aligned_malloc(sizeof(struct pt_gfx_mesh_neutral_primitive_header_t) * neutral_header.primitive_count, alignof(struct pt_gfx_mesh_neutral_primitive_header_t)));
        if (!mesh_load_primitive_headers_from_input_stream(&neutral_header, neutral_primitive_headers, gfx_input_stream, gfx_input_stream_callbacks))
        {
            mcrt_aligned_free(neutral_primitive_headers);

            mcrt_atomic_store(&this->m_streaming_error, true);
            mcrt_atomic_store(&this->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
            gfx_connection->streaming_throttling_index_lock();
            uint32_t streaming_throttling_index = gfx_connection->streaming_throttling_index();
            gfx_connection->streaming_object_list_push(streaming_throttling_index, this);
            gfx_connection->streaming_throttling_index_unlock();
            return false;
        }

        // 1-2. alloc asset vertex/index buffer
        if (!this->read_input_stream_alloc_asset_buffers(gfx_connection, &neutral_header, neutral_primitive_headers))
        {
            mcrt_aligned_free(neutral_primitive_headers);

            mcrt_atomic_store(&this->m_streaming_error, true);
            mcrt_atomic_store(&this->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
            gfx_connection->streaming_throttling_index_lock();
            uint32_t streaming_throttling_index = gfx_connection->streaming_throttling_index();
            gfx_connection->streaming_object_list_push(streaming_throttling_index, this);
            gfx_connection->streaming_throttling_index_unlock();
            return false;
        }

        // 1-3. populate task data
        {
            mcrt_task_ref task = mcrt_task_allocate_root(read_input_stream_task_execute, gfx_connection->task_group_context());
            struct read_input_stream_task_data_t *task_data = unwrap(mcrt_task_get_user_data(task));
            task_data->m_gfx_streaming_object = this;
            task_data->m_gfx_connection = gfx_connection;
            task_data->m_gfx_input_stream = gfx_input_stream;
            task_data->m_gfx_input_stream_callbacks = (*gfx_input_stream_callbacks);

            mcrt_atomic_store(&this->m_streaming_status, STREAMING_STATUS_STAGE_SECOND);
            // different master task doesn't share the task_arena
            // we need to share the same the task arena to make sure the "tbb::this_task_arena::current_thread_id" unique
            // mcrt_task_spawn(task);
            mcrt_task_enqueue(task, gfx_connection->task_arena());
        }
    }
    else
    {
        // race condition with the "streaming_done"
        // inevitable since the "transfer_dst_and_sampled_image_alloc" nervertheless races with the "streaming_done"
        this->pre_streaming_done_destroy_callback(gfx_connection);
    }

    return true;
}

// second stage
// 2-1. mesh load header - base
// 2-2. translate headers - callback
// 2-3. calculate total size - callback
// 2-4. get staging buffer pointer - callback
// 2-5. mesh load data - base
// 2-6. record copy commands - callback
mcrt_task_ref gfx_mesh_base::read_input_stream_task_execute(mcrt_task_ref self)
{
    struct read_input_stream_task_data_t *task_data = unwrap(mcrt_task_get_user_data(self));

    class gfx_mesh_base *const gfx_streaming_object = task_data->m_gfx_streaming_object;
    class gfx_connection_base *const gfx_connection = task_data->m_gfx_connection;
    pt_gfx_input_stream_ref const gfx_input_stream = task_data->m_gfx_input_stream;
    struct pt_gfx_input_stream_callbacks_t const *const gfx_input_stream_callbacks = &task_data->m_gfx_input_stream_callbacks;

    // different master task doesn't share the task_arena
    // we need to share the same the task arena to make sure the "tbb::this_task_arena::current_thread_id" unique
    // assert(NULL != mcrt_task_arena_internal_arena(task_data->m_gfx_connection->task_arena()));
    // assert(mcrt_task_arena_internal_arena(task_data->m_gfx_connection->task_arena()) == mcrt_this_task_arena_internal_arena());

    PT_MAYBE_UNUSED streaming_status_t streaming_status = mcrt_atomic_load(&gfx_streaming_object->m_streaming_status);
    PT_MAYBE_UNUSED bool streaming_error = mcrt_atomic_load(&gfx_streaming_object->m_streaming_error);
    bool streaming_cancel = mcrt_atomic_load(&gfx_streaming_object->m_streaming_cancel);
    assert(STREAMING_STATUS_STAGE_SECOND == streaming_status);
    assert(!streaming_error);

    // race condition
    // task: load streaming_throttling_index
    // reduce_streaming_task: store ++streaming_throttling_index
    // reduce_streaming_task: mcrt_task_wait_for_all // return immediately
    // task: mcrt_task_increment_ref_count
    gfx_connection->streaming_throttling_index_lock();
    uint32_t streaming_throttling_index = gfx_connection->streaming_throttling_index();
    mcrt_task_increment_ref_count(gfx_connection->streaming_task_root(streaming_throttling_index));
    gfx_connection->streaming_throttling_index_unlock();

#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
    gfx_connection->streaming_task_mark_execute_begin(streaming_throttling_index);
#endif

    mcrt_task_set_parent(self, gfx_connection->streaming_task_root(streaming_throttling_index));

    if (!streaming_cancel)
    {
        // 2-1. mesh load header
        struct pt_gfx_mesh_neutral_header_t neutral_header;
        if (!mesh_load_header_from_input_stream(&neutral_header, gfx_input_stream, gfx_input_stream_callbacks))
        {
            mcrt_atomic_store(&gfx_streaming_object->m_streaming_error, true);
            gfx_input_stream_callbacks->pfn_close(gfx_input_stream);
            mcrt_atomic_store(&gfx_streaming_object->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
            gfx_connection->streaming_object_list_push(streaming_throttling_index, gfx_streaming_object);
#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
            gfx_connection->streaming_task_mark_executie_end(streaming_throttling_index);
#endif
            return NULL;
        }

        struct pt_gfx_mesh_neutral_primitive_header_t *neutral_primitive_headers = static_cast<struct pt_gfx_mesh_neutral_primitive_header_t *>(mcrt_aligned_malloc(sizeof(struct pt_gfx_mesh_neutral_primitive_header_t) * neutral_header.primitive_count, alignof(struct pt_gfx_mesh_neutral_primitive_header_t)));
        if (!mesh_load_primitive_headers_from_input_stream(&neutral_header, neutral_primitive_headers, gfx_input_stream, gfx_input_stream_callbacks))
        {
            mcrt_aligned_free(neutral_primitive_headers);

            mcrt_atomic_store(&gfx_streaming_object->m_streaming_error, true);
            gfx_input_stream_callbacks->pfn_close(gfx_input_stream);
            mcrt_atomic_store(&gfx_streaming_object->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
            gfx_connection->streaming_object_list_push(streaming_throttling_index, gfx_streaming_object);
#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
            gfx_connection->streaming_task_mark_executie_end(streaming_throttling_index);
#endif
            return NULL;
        }

        // 2-2. translate headers
        // do nothing

        struct pt_gfx_mesh_neutral_primitive_memcpy_dest_t *memcpy_dests = static_cast<struct pt_gfx_mesh_neutral_primitive_memcpy_dest_t *>(mcrt_aligned_malloc(sizeof(struct pt_gfx_mesh_neutral_primitive_memcpy_dest_t) * neutral_header.primitive_count, alignof(struct pt_gfx_mesh_neutral_primitive_memcpy_dest_t)));

        // 2-3. calculate total size
        {
            uint64_t staging_buffer_begin = gfx_connection->staging_buffer_begin(streaming_throttling_index);
            uint64_t staging_buffer_end = uint64_t(-1);
            uint64_t staging_buffer_size = gfx_connection->staging_buffer_size(streaming_throttling_index);
            uint64_t base_offset = uint64_t(-1);

            // use CAS to allocate memory from staging buffer
            do
            {
                base_offset = mcrt_atomic_load(gfx_connection->staging_buffer_end(streaming_throttling_index));

                size_t total_size = gfx_streaming_object->read_input_stream_calculate_total_size(&neutral_header, neutral_primitive_headers, base_offset, memcpy_dests);

                staging_buffer_end = (base_offset + total_size);
                // assert((staging_buffer_end - staging_buffer_begin) <= staging_buffer_size);

                // respawn if memory not enough
                if ((staging_buffer_end - staging_buffer_begin) > staging_buffer_size)
                {
                    // recycle to prevent free_task
                    mcrt_task_set_parent(self, NULL);
                    mcrt_task_recycle_as_child_of(self, gfx_connection->streaming_task_respawn_root());

                    gfx_connection->streaming_task_respawn_list_push(streaming_throttling_index, self);

                    mcrt_aligned_free(neutral_primitive_headers);
                    mcrt_aligned_free(memcpy_dests);

#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
                    task_data->m_gfx_connection->streaming_task_mark_executie_end(streaming_throttling_index);
#endif
                    // recycle needs manually tally_completion_of_predecessor
                    // the "mcrt_task_decrement_ref_count" must be called after all works(include the C++ destructors) are done
                    mcrt_task_decrement_ref_count(gfx_connection->streaming_task_root(streaming_throttling_index));
                    return NULL;
                }

            } while (base_offset != mcrt_atomic_cas_u64(gfx_connection->staging_buffer_end(streaming_throttling_index), staging_buffer_end, base_offset));
        }

        // 2-4. get staging buffer pointer
        void *staging_pointer = gfx_connection->staging_buffer_pointer();

        // 2-5. mesh load data - base
        if (!mesh_load_primitive_data_from_input_stream(&neutral_header, neutral_primitive_headers, staging_pointer, memcpy_dests, gfx_input_stream, gfx_input_stream_callbacks))
        {
            mcrt_aligned_free(neutral_primitive_headers);
            mcrt_aligned_free(memcpy_dests);

            mcrt_atomic_store(&gfx_streaming_object->m_streaming_error, true);
            gfx_input_stream_callbacks->pfn_close(gfx_input_stream);
            mcrt_atomic_store(&gfx_streaming_object->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
            gfx_connection->streaming_object_list_push(streaming_throttling_index, gfx_streaming_object);
#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
            gfx_connection->streaming_task_mark_executie_end(streaming_throttling_index);
#endif
            return NULL;
        }
        mcrt_aligned_free(neutral_primitive_headers);

        // 2-6. record copy commands - callback
        gfx_streaming_object->read_input_stream_record_copy_commands(gfx_connection, streaming_throttling_index, memcpy_dests);
        mcrt_aligned_free(memcpy_dests);

        // pass to the third stage
        mcrt_atomic_store(&gfx_streaming_object->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
        gfx_connection->streaming_object_list_push(streaming_throttling_index, gfx_streaming_object);
    }
    else
    {
        // The slob allocator is serial which is harmful to the parallel performance
        // leave the "steaming_cancel" to the third stage
        // tracker by "slob_lock_busy_count"
#if 0
            task_data->m_streaming_object->pre_streaming_done_destroy_callback();
#else
        // pass to the third stage
        mcrt_atomic_store(&gfx_streaming_object->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
        gfx_connection->streaming_object_list_push(streaming_throttling_index, gfx_streaming_object);
#endif
    }

    gfx_input_stream_callbacks->pfn_close(gfx_input_stream);
#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
    gfx_connection->streaming_task_mark_executie_end(streaming_throttling_index);
#endif
    return NULL;
}

bool gfx_mesh_base::read_input_stream(class gfx_connection_base *gfx_connection, char const *file_name, struct pt_gfx_input_stream_callbacks_t const *gfx_input_stream_callbacks)
{
    return this->read_input_stream_task_spawn(gfx_connection, file_name, gfx_input_stream_callbacks);
}

void gfx_mesh_base::destroy(class gfx_connection_base *gfx_connection)
{
    this->release(gfx_connection);
}

void gfx_mesh_base::addref()
{
    PT_MAYBE_UNUSED uint32_t ref_count = mcrt_atomic_inc_u32(&this->m_ref_count);
    // can't set_mesh after destory
    assert(1U < ref_count);
}

void gfx_mesh_base::release(class gfx_connection_base *gfx_connection)
{
    if (0U == mcrt_atomic_dec_u32(&this->m_ref_count))
    {
        this->streaming_destroy_request(gfx_connection);
    }
}

// API
static inline pt_gfx_connection_ref wrap(class gfx_connection_base *gfx_connection) { return reinterpret_cast<pt_gfx_connection_ref>(gfx_connection); }
static inline class gfx_connection_base *unwrap(pt_gfx_connection_ref gfx_connection) { return reinterpret_cast<class gfx_connection_base *>(gfx_connection); }

static inline pt_gfx_mesh_ref wrap(class gfx_mesh_base *mesh) { return reinterpret_cast<pt_gfx_mesh_ref>(mesh); }
static inline class gfx_mesh_base *unwrap(pt_gfx_mesh_ref mesh) { return reinterpret_cast<class gfx_mesh_base *>(mesh); }

PT_ATTR_GFX bool PT_CALL pt_gfx_mesh_read_input_stream(pt_gfx_connection_ref gfx_connection, pt_gfx_mesh_ref mesh, char const *initial_filename, pt_gfx_input_stream_init_callback gfx_input_stream_init_callback, pt_gfx_input_stream_read_callback gfx_input_stream_read_callback, pt_gfx_input_stream_seek_callback gfx_input_stream_seek_callback, pt_gfx_input_stream_destroy_callback gfx_input_stream_destroy_callback)
{
    pt_gfx_input_stream_callbacks_t gfx_input_stream_callbacks = {
        gfx_input_stream_init_callback,
        NULL,
        gfx_input_stream_read_callback,
        gfx_input_stream_seek_callback,
        gfx_input_stream_destroy_callback};

    return unwrap(mesh)->read_input_stream(unwrap(gfx_connection), initial_filename, &gfx_input_stream_callbacks);
}

PT_ATTR_GFX void PT_CALL pt_gfx_mesh_destroy(pt_gfx_connection_ref gfx_connection, pt_gfx_mesh_ref mesh)
{
    return unwrap(mesh)->destroy(unwrap(gfx_connection));
}
