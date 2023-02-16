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

#include "streaming_object.h"

struct streaming_object_load_task_data_t
{
    class gfx_streaming_object_base *m_streaming_object;
    class gfx_connection_base *m_connection;
    pt_input_stream_ref m_input_stream;
    intptr_t(PT_PTR *m_input_stream_read_callback)(pt_input_stream_ref, void *, size_t);
    int64_t(PT_PTR *m_input_stream_seek_callback)(pt_input_stream_ref, int64_t, int);
    void(PT_PTR *m_input_stream_destroy_callback)(pt_input_stream_ref);
    size_t m_memcpy_dests_size;
    size_t m_memcpy_dests_align;
};
static_assert(sizeof(struct streaming_object_load_task_data_t) <= sizeof(mcrt_task_user_data_t), "");
inline struct streaming_object_load_task_data_t *unwrap(mcrt_task_user_data_t *task_data) { return reinterpret_cast<struct streaming_object_load_task_data_t *>(task_data); }

bool gfx_streaming_object_base::load(
    class gfx_connection_base *gfx_connection,
    char const *initial_filename,
    pt_input_stream_init_callback input_stream_init_callback,
    pt_input_stream_read_callback input_stream_read_callback,
    pt_input_stream_seek_callback input_stream_seek_callback,
    pt_input_stream_destroy_callback input_stream_destroy_callback)
{
    // How to implement pipeline "serial - parallel - serial"
    // follow [McCool 2012] "Structured Parallel Programming: Patterns for Efficient Computation." / 9.4.2 Pipeline in Cilk Plus
    // the second stage is trivially implemented by the task spawned by the first stage
    // the third stage can be implemented as "reduce"   // this means that the "reduce_streaming_task" can be considered as the third stage

    // the first stage
    PT_MAYBE_UNUSED streaming_status_t streaming_status = mcrt_atomic_load(&this->m_streaming_status);
    PT_MAYBE_UNUSED bool streaming_error = mcrt_atomic_load(&this->m_streaming_error);
    bool streaming_cancel = mcrt_atomic_load(&this->m_streaming_cancel);
    assert(STREAMING_STATUS_STAGE_FIRST == streaming_status);
    assert(!streaming_error);

    if (!streaming_cancel)
    {
        pt_input_stream_ref gfx_input_stream = input_stream_init_callback(initial_filename);
        if (pt_input_stream_ref(-1) == gfx_input_stream)
        {
            mcrt_atomic_store(&this->m_streaming_error, true);
            mcrt_atomic_store(&this->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
            // TODO: return 'streaming_throttling_index' from 'streaming_throttling_index_lock'
            gfx_connection->streaming_throttling_index_lock();
            uint32_t streaming_throttling_index = gfx_connection->streaming_throttling_index();
            gfx_connection->streaming_object_list_push(streaming_throttling_index, this);
            gfx_connection->streaming_throttling_index_unlock();
            return false;
        }

        // pre task spawn callback
        // usually load the asset header and allocate device resources
        size_t memcpy_dests_size = -1;
        size_t memcpy_dests_align = -1;
        if (!this->load_header_callback(gfx_input_stream, input_stream_read_callback, input_stream_seek_callback, gfx_connection, &memcpy_dests_size, &memcpy_dests_align))
        {
            input_stream_destroy_callback(gfx_input_stream);

            mcrt_atomic_store(&this->m_streaming_error, true);
            mcrt_atomic_store(&this->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
            gfx_connection->streaming_throttling_index_lock();
            uint32_t streaming_throttling_index = gfx_connection->streaming_throttling_index();
            gfx_connection->streaming_object_list_push(streaming_throttling_index, this);
            gfx_connection->streaming_throttling_index_unlock();
            return false;
        }

        // pass to the second stage
        {
            mcrt_task_ref task = mcrt_task_allocate_root(load_task_execute, gfx_connection->task_group_context());
            struct streaming_object_load_task_data_t *task_data = unwrap(mcrt_task_get_user_data(task));
            task_data->m_streaming_object = this;
            task_data->m_connection = gfx_connection;
            task_data->m_input_stream = gfx_input_stream;
            task_data->m_input_stream_read_callback = input_stream_read_callback;
            task_data->m_input_stream_seek_callback = input_stream_seek_callback;
            task_data->m_input_stream_destroy_callback = input_stream_destroy_callback;
            task_data->m_memcpy_dests_size = memcpy_dests_size;
            task_data->m_memcpy_dests_align = memcpy_dests_align;

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

mcrt_task_ref gfx_streaming_object_base::load_task_execute(mcrt_task_ref self)
{
    struct streaming_object_load_task_data_t *task_data = unwrap(mcrt_task_get_user_data(self));

    // different master task doesn't share the task_arena
    // we need to share the same the task arena to make sure the "tbb::this_task_arena::current_thread_id" unique
    // assert(NULL != mcrt_task_arena_internal_arena(task_data->m_gfx_connection->task_arena()));
    // assert(mcrt_task_arena_internal_arena(task_data->m_gfx_connection->task_arena()) == mcrt_this_task_arena_internal_arena());

    PT_MAYBE_UNUSED streaming_status_t streaming_status = mcrt_atomic_load(&task_data->m_streaming_object->m_streaming_status);
    PT_MAYBE_UNUSED bool streaming_error = mcrt_atomic_load(&task_data->m_streaming_object->m_streaming_error);
    bool streaming_cancel = mcrt_atomic_load(&task_data->m_streaming_object->m_streaming_cancel);
    assert(STREAMING_STATUS_STAGE_SECOND == streaming_status);
    assert(!streaming_error);

    // race condition
    // task: load streaming_throttling_index
    // reduce_streaming_task: store ++streaming_throttling_index
    // reduce_streaming_task: mcrt_task_wait_for_all // return immediately
    // task: mcrt_task_increment_ref_count
    task_data->m_connection->streaming_throttling_index_lock();
    uint32_t streaming_throttling_index = task_data->m_connection->streaming_throttling_index();
    mcrt_task_increment_ref_count(task_data->m_connection->streaming_task_root(streaming_throttling_index));
    task_data->m_connection->streaming_throttling_index_unlock();

#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
    task_data->m_connection->streaming_task_mark_execute_begin(streaming_throttling_index);
#endif

    mcrt_task_set_parent(self, task_data->m_connection->streaming_task_root(streaming_throttling_index));

    if (!streaming_cancel)
    {
        // allocate temp 'memcpy_dests' memory
        void *memcpy_dests = mcrt_aligned_malloc(task_data->m_memcpy_dests_size, task_data->m_memcpy_dests_align);
#if 0
        if (PT_UNLIKELY(NULL == memcpy_dests))
        {
            task_data->m_input_stream_destroy_callback(task_data->m_input_stream);

            mcrt_atomic_store(&task_data->m_streaming_object->m_streaming_error, true);
            mcrt_atomic_store(&task_data->m_streaming_object->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
            task_data->m_connection->streaming_object_list_push(streaming_throttling_index, task_data->m_streaming_object);
#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
            task_data->m_connection->streaming_task_mark_executie_end(streaming_throttling_index);
#endif
            return NULL;
        }
#else
        assert(NULL != memcpy_dests);
#endif

        // try to allocate memory from staging buffer
        {
            uint64_t staging_buffer_begin = task_data->m_connection->staging_buffer_begin(streaming_throttling_index);
            uint64_t staging_buffer_end = uint64_t(-1);
            uint64_t staging_buffer_size = task_data->m_connection->staging_buffer_size(streaming_throttling_index);
            uint64_t base_offset = uint64_t(-1);

            do
            {
                base_offset = mcrt_atomic_load(task_data->m_connection->staging_buffer_end(streaming_throttling_index));

                size_t total_size = task_data->m_streaming_object->calculate_staging_buffer_total_size_callback(base_offset, task_data->m_connection, memcpy_dests);

                staging_buffer_end = (base_offset + total_size);
                // assert((staging_buffer_end - staging_buffer_begin) <= staging_buffer_size);

                // respawn if memory not enough
                if ((staging_buffer_end - staging_buffer_begin) > staging_buffer_size)
                {
                    // reuse the input stream next time // do not need to destory
                    // task_data->m_input_stream_destroy_callback(task_data->m_gfx_input_stream);

                    // release temp memory for the calculation
                    mcrt_aligned_free(memcpy_dests);

                    // recycle to prevent free_task
                    mcrt_task_set_parent(self, NULL);
                    mcrt_task_recycle_as_child_of(self, task_data->m_connection->streaming_task_respawn_root());

                    task_data->m_connection->streaming_task_respawn_list_push(streaming_throttling_index, self);

#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
                    task_data->m_connection->streaming_task_mark_executie_end(streaming_throttling_index);
#endif
                    // recycle needs manually tally_completion_of_predecessor
                    // the "mcrt_task_decrement_ref_count" must be called after all works(include the C++ destructors) are done
                    mcrt_task_decrement_ref_count(task_data->m_connection->streaming_task_root(streaming_throttling_index));
                    return NULL;
                }

            } while (base_offset != mcrt_atomic_cas_u64(task_data->m_connection->staging_buffer_end(streaming_throttling_index), staging_buffer_end, base_offset));
        }

        // post calculate staging buffer total size callback
        // usually load the asset data into the staging buffer and copy the data into the image from the staging buffer
        if (!task_data->m_streaming_object->load_data_callback(task_data->m_input_stream, task_data->m_input_stream_read_callback, task_data->m_input_stream_seek_callback, task_data->m_connection, memcpy_dests, streaming_throttling_index))
        {
            mcrt_aligned_free(memcpy_dests);

            mcrt_atomic_store(&task_data->m_streaming_object->m_streaming_error, true);
            task_data->m_input_stream_destroy_callback(task_data->m_input_stream);
            mcrt_atomic_store(&task_data->m_streaming_object->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
            task_data->m_connection->streaming_object_list_push(streaming_throttling_index, task_data->m_streaming_object);
#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
            task_data->m_connection->streaming_task_mark_executie_end(streaming_throttling_index);
#endif
            return NULL;
        }

        mcrt_aligned_free(memcpy_dests);

        // pass to the third stage
        mcrt_atomic_store(&task_data->m_streaming_object->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
        task_data->m_connection->streaming_object_list_push(streaming_throttling_index, task_data->m_streaming_object);
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
        mcrt_atomic_store(&task_data->m_streaming_object->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
        task_data->m_connection->streaming_object_list_push(streaming_throttling_index, task_data->m_streaming_object);
#endif
    }

    task_data->m_input_stream_destroy_callback(task_data->m_input_stream);
#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
    task_data->m_connection->streaming_task_mark_executie_end(streaming_throttling_index);
#endif
    return NULL;
}

void gfx_streaming_object_base::streaming_destroy_request(class gfx_connection_base *gfx_connection)
{
    bool streaming_done;
    {
        // make sure this function happens before or after the gfx_streaming_object_base::streaming_done_execute
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
        // the object is used by the rendering system
        gfx_connection->streaming_done_object_destroy_list_push(this);
    }
}

void gfx_streaming_object_base::streaming_done_execute(class gfx_connection_base *gfx_connection)
{
    this->streaming_done_lock();

    PT_MAYBE_UNUSED streaming_status_t streaming_status = this->m_streaming_status;
    bool streaming_error = this->m_streaming_error;
    bool streaming_cancel = this->m_streaming_cancel;
    assert(STREAMING_STATUS_STAGE_THIRD == streaming_status);

    if (!streaming_cancel)
    {
        if (!streaming_error)
        {
            if (!this->streaming_done_callback(gfx_connection))
            {
                mcrt_atomic_store(&this->m_streaming_error, true);
            }
        }

        mcrt_atomic_store(&this->m_streaming_status, STREAMING_STATUS_DONE);
    }
    else
    {
        this->pre_streaming_done_destroy_callback(gfx_connection);
    }

    this->streaming_done_unlock();
}

void gfx_streaming_object_base::post_stream_done_destroy_execute(class gfx_connection_base *gfx_connection)
{
    return this->post_stream_done_destroy_callback(gfx_connection);
}