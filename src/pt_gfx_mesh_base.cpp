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

struct mesh_streaming_stage_second_task_data_t
{
    class gfx_mesh_base *m_gfx_streaming_object;
    class gfx_connection_base *m_gfx_connection;
    pt_gfx_input_stream_ref m_gfx_input_stream;
    intptr_t(PT_PTR *m_gfx_input_stream_read_callback)(pt_gfx_input_stream_ref, void *, size_t);
    int64_t(PT_PTR *m_gfx_input_stream_seek_callback)(pt_gfx_input_stream_ref, int64_t, int);
    void(PT_PTR *m_gfx_input_stream_destroy_callback)(pt_gfx_input_stream_ref);
};
static_assert(sizeof(struct mesh_streaming_stage_second_task_data_t) <= sizeof(mcrt_task_user_data_t), "");
inline struct mesh_streaming_stage_second_task_data_t *unwrap(mcrt_task_user_data_t *task_data) { return reinterpret_cast<struct mesh_streaming_stage_second_task_data_t *>(task_data); }

bool gfx_mesh_base::read_input_stream(class gfx_connection_base *gfx_connection, uint32_t mesh_index, uint32_t material_index, char const *initial_filename, pt_gfx_input_stream_ref(PT_PTR *gfx_input_stream_init_callback)(char const *), intptr_t(PT_PTR *gfx_input_stream_read_callback)(pt_gfx_input_stream_ref, void *, size_t), int64_t(PT_PTR *gfx_input_stream_seek_callback)(pt_gfx_input_stream_ref, int64_t, int), void(PT_PTR *gfx_input_stream_destroy_callback)(pt_gfx_input_stream_ref))
{
    // How to implement pipeline "serial - parallel - serial"
    // follow [McCool 2012] "Structured Parallel Programming: Patterns for Efficient Computation." / 9.4.2 Pipeline in Cilk Plus
    // the second stage is trivially implemented by the task spawned by the first stage
    // the third stage can be implemented as "reduce"

    // We can consider the "reduce_streaming_task" as the third stage

    // the first stage

    PT_MAYBE_UNUSED streaming_status_t streaming_status = mcrt_atomic_load(&this->m_streaming_status);
    PT_MAYBE_UNUSED bool streaming_error = mcrt_atomic_load(&this->m_streaming_error);
    bool streaming_cancel = mcrt_atomic_load(&this->m_streaming_cancel);
    assert(STREAMING_STATUS_STAGE_FIRST == streaming_status);
    assert(!streaming_error);

    if (!streaming_cancel)
    {
        pt_gfx_input_stream_ref gfx_input_stream = gfx_input_stream_init_callback(initial_filename);
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

        //pre populate task data
        struct gfx_mesh_neutral_header_t neutral_header;
        neutral_header.vertex_count = 36U;
        neutral_header.index_type = PT_GFX_MESH_NEUTRAL_INDEX_TYPE_UINT16;
        neutral_header.index_count = 36U;
        if (!this->mesh_streaming_stage_first_pre_populate_task_data_callback(gfx_connection, &neutral_header))
        {
            mcrt_atomic_store(&this->m_streaming_error, true);
            gfx_input_stream_destroy_callback(gfx_input_stream);
            mcrt_atomic_store(&this->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
            gfx_connection->streaming_throttling_index_lock();
            uint32_t streaming_throttling_index = gfx_connection->streaming_throttling_index();
            gfx_connection->streaming_object_list_push(streaming_throttling_index, this);
            gfx_connection->streaming_throttling_index_unlock();
            return false;
        }

        // pass to the second stage
        {
            mcrt_task_ref task = mcrt_task_allocate_root(mesh_streaming_stage_second_task_execute);
            struct mesh_streaming_stage_second_task_data_t *task_data = unwrap(mcrt_task_get_user_data(task));
            task_data->m_gfx_streaming_object = this;
            task_data->m_gfx_connection = gfx_connection;
            task_data->m_gfx_input_stream = gfx_input_stream;
            task_data->m_gfx_input_stream_read_callback = gfx_input_stream_read_callback;
            task_data->m_gfx_input_stream_seek_callback = gfx_input_stream_seek_callback;
            task_data->m_gfx_input_stream_destroy_callback = gfx_input_stream_destroy_callback;

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
        this->streaming_destroy_callback(gfx_connection);
    }

    return true;
}

mcrt_task_ref gfx_mesh_base::mesh_streaming_stage_second_task_execute(mcrt_task_ref self)
{
    struct mesh_streaming_stage_second_task_data_t *task_data = unwrap(mcrt_task_get_user_data(self));

    // different master task doesn't share the task_arena
    // we need to share the same the task arena to make sure the "tbb::this_task_arena::current_thread_id" unique
    // assert(NULL != mcrt_task_arena_internal_arena(task_data->m_gfx_connection->task_arena()));
    // assert(mcrt_task_arena_internal_arena(task_data->m_gfx_connection->task_arena()) == mcrt_this_task_arena_internal_arena());

    PT_MAYBE_UNUSED streaming_status_t streaming_status = mcrt_atomic_load(&task_data->m_gfx_streaming_object->m_streaming_status);
    PT_MAYBE_UNUSED bool streaming_error = mcrt_atomic_load(&task_data->m_gfx_streaming_object->m_streaming_error);
    bool streaming_cancel = mcrt_atomic_load(&task_data->m_gfx_streaming_object->m_streaming_cancel);
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

#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
    task_data->m_gfx_connection->streaming_task_mark_execute_begin(streaming_throttling_index);
#endif

    mcrt_task_set_parent(self, task_data->m_gfx_connection->streaming_task_root(streaming_throttling_index));

    if (!streaming_cancel)
    {
        // pre calculate total size
        struct gfx_mesh_neutral_header_t neutral_header;
        neutral_header.vertex_count = 36U;
        neutral_header.index_type = PT_GFX_MESH_NEUTRAL_INDEX_TYPE_UINT16;
        neutral_header.index_count = 36U;
        struct gfx_mesh_neutral_memcpy_dest_t memcpy_dest[3];
#if 0        
        void *cmdcopy_dest;
        if (!task_data->m_gfx_streaming_object->mesh_streaming_stage_second_pre_calculate_total_size_callback(&neutral_header, &memcpy_dest, &cmdcopy_dest))
        {
            mcrt_atomic_store(&task_data->m_gfx_streaming_object->m_streaming_error, true);
            task_data->m_gfx_input_stream_destroy_callback(task_data->m_gfx_input_stream);
            mcrt_atomic_store(&task_data->m_gfx_streaming_object->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
            task_data->m_gfx_connection->streaming_object_list_push(streaming_throttling_index, task_data->m_gfx_streaming_object);
            task_data->m_gfx_connection->streaming_task_mark_executie_end(streaming_throttling_index);
            return NULL;
        }
        assert(NULL != memcpy_dest);
        assert(NULL != cmdcopy_dest);
#endif

        // try to allocate memory from staging buffer
        {
            uint64_t transfer_src_buffer_begin = task_data->m_gfx_connection->transfer_src_buffer_begin(streaming_throttling_index);
            uint64_t transfer_src_buffer_end = uint64_t(-1);
            uint64_t transfer_src_buffer_size = task_data->m_gfx_connection->transfer_src_buffer_size(streaming_throttling_index);
            uint64_t base_offset = uint64_t(-1);

            do
            {
                base_offset = mcrt_atomic_load(task_data->m_gfx_connection->transfer_src_buffer_end(streaming_throttling_index));

                size_t total_size = task_data->m_gfx_streaming_object->mesh_streaming_stage_second_calculate_total_size_callback(task_data->m_gfx_connection, &neutral_header, memcpy_dest, base_offset);

                transfer_src_buffer_end = (base_offset + total_size);
                //assert((transfer_src_buffer_end - transfer_src_buffer_begin) <= transfer_src_buffer_size);

                // respawn if memory not enough
                if ((transfer_src_buffer_end - transfer_src_buffer_begin) > transfer_src_buffer_size)
                {
                    // recycle to prevent free_task
                    mcrt_task_recycle_as_child_of(self, task_data->m_gfx_connection->streaming_task_respawn_root());

                    task_data->m_gfx_connection->streaming_task_respawn_list_push(streaming_throttling_index, self);

#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
                    task_data->m_gfx_connection->streaming_task_mark_executie_end(streaming_throttling_index);
#endif
                    // recycle needs manually tally_completion_of_predecessor
                    // the "mcrt_task_decrement_ref_count" must be called after all works(include the C++ destructors) are done
                    mcrt_task_decrement_ref_count(task_data->m_gfx_connection->streaming_task_root(streaming_throttling_index));
                    return NULL;
                }

            } while (base_offset != mcrt_atomic_cas_u64(task_data->m_gfx_connection->transfer_src_buffer_end(streaming_throttling_index), transfer_src_buffer_end, base_offset));
        }

        // post calculate total size
        if (!task_data->m_gfx_streaming_object->mesh_streaming_stage_second_post_calculate_total_size_callback(task_data->m_gfx_connection, streaming_throttling_index, &neutral_header, memcpy_dest, task_data->m_gfx_input_stream, task_data->m_gfx_input_stream_read_callback, task_data->m_gfx_input_stream_seek_callback))
        {
            mcrt_atomic_store(&task_data->m_gfx_streaming_object->m_streaming_error, true);
            task_data->m_gfx_input_stream_destroy_callback(task_data->m_gfx_input_stream);
            mcrt_atomic_store(&task_data->m_gfx_streaming_object->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
            task_data->m_gfx_connection->streaming_object_list_push(streaming_throttling_index, task_data->m_gfx_streaming_object);
#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
            task_data->m_gfx_connection->streaming_task_mark_executie_end(streaming_throttling_index);
#endif
            return NULL;
        }

        // pass to the third stage
        mcrt_atomic_store(&task_data->m_gfx_streaming_object->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
        task_data->m_gfx_connection->streaming_object_list_push(streaming_throttling_index, task_data->m_gfx_streaming_object);
    }
    else
    {
        // The slob allocator is serial which is harmful to the parallel performance
        // leave the "steaming_cancel" to the third stage
        // tracker by "slob_lock_busy_count"
#if 0
            task_data->m_streaming_object->streaming_destroy_callback();
#else
        // pass to the third stage
        mcrt_atomic_store(&task_data->m_gfx_streaming_object->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
        task_data->m_gfx_connection->streaming_object_list_push(streaming_throttling_index, task_data->m_gfx_streaming_object);
#endif
    }

    task_data->m_gfx_input_stream_destroy_callback(task_data->m_gfx_input_stream);
#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
    task_data->m_gfx_connection->streaming_task_mark_executie_end(streaming_throttling_index);
#endif
    return NULL;
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
        bool streaming_done;
        this->streaming_destroy_request(&streaming_done);

        if (streaming_done)
        {
            // the object is used by the rendering system
            this->frame_destroy_request(gfx_connection);
        }
    }
}

// API
inline pt_gfx_connection_ref wrap(class gfx_connection_base *gfx_connection) { return reinterpret_cast<pt_gfx_connection_ref>(gfx_connection); }
inline class gfx_connection_base *unwrap(pt_gfx_connection_ref gfx_connection) { return reinterpret_cast<class gfx_connection_base *>(gfx_connection); }

inline pt_gfx_mesh_ref wrap(class gfx_mesh_base *mesh) { return reinterpret_cast<pt_gfx_mesh_ref>(mesh); }
inline class gfx_mesh_base *unwrap(pt_gfx_mesh_ref mesh) { return reinterpret_cast<class gfx_mesh_base *>(mesh); }

PT_ATTR_GFX bool PT_CALL pt_gfx_mesh_read_input_stream(pt_gfx_connection_ref gfx_connection, pt_gfx_mesh_ref mesh, uint32_t mesh_index, uint32_t material_index, char const *initial_filename, pt_gfx_input_stream_ref(PT_PTR *gfx_input_stream_init_callback)(char const *), intptr_t(PT_PTR *gfx_input_stream_read_callback)(pt_gfx_input_stream_ref, void *, size_t), int64_t(PT_PTR *gfx_input_stream_seek_callback)(pt_gfx_input_stream_ref, int64_t, int), void(PT_PTR *gfx_input_stream_destroy_callback)(pt_gfx_input_stream_ref))
{
    return unwrap(mesh)->read_input_stream(unwrap(gfx_connection), mesh_index, material_index, initial_filename, gfx_input_stream_init_callback, gfx_input_stream_read_callback, gfx_input_stream_seek_callback, gfx_input_stream_destroy_callback);
}

PT_ATTR_GFX void PT_CALL pt_gfx_mesh_destroy(pt_gfx_connection_ref gfx_connection, pt_gfx_mesh_ref mesh)
{
    return unwrap(mesh)->destroy(unwrap(gfx_connection));
}
