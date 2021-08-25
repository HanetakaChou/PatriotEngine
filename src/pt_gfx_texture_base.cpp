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
#include <pt_mcrt_atomic.h>
#include <pt_mcrt_task.h>
#include "pt_gfx_texture_base.h"
#include "pt_gfx_texture_base_load.h"
#include <assert.h>

struct texture_streaming_stage_second_task_data_t
{
    class gfx_texture_base *m_gfx_streaming_object;
    class gfx_connection_base *m_gfx_connection;
    gfx_input_stream_ref m_gfx_input_stream;
    intptr_t(PT_PTR *m_gfx_input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count);
    int64_t(PT_PTR *m_gfx_input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence);
    void(PT_PTR *m_gfx_input_stream_destroy_callback)(gfx_input_stream_ref input_stream);
};
static_assert(sizeof(struct texture_streaming_stage_second_task_data_t) <= sizeof(mcrt_task_user_data_t), "");

bool gfx_texture_base::read_input_stream(
    class gfx_connection_base *gfx_connection,
    char const *initial_filename,
    gfx_input_stream_ref(PT_PTR *gfx_input_stream_init_callback)(char const *initial_filename),
    intptr_t(PT_PTR *gfx_input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count),
    int64_t(PT_PTR *gfx_input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence),
    void(PT_PTR *gfx_input_stream_destroy_callback)(gfx_input_stream_ref input_stream))
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
        gfx_input_stream_ref gfx_input_stream = gfx_input_stream_init_callback(initial_filename);
        if (gfx_input_stream_ref(-1) == gfx_input_stream)
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
        struct common_header_t common_header;
        size_t common_data_offset;
        if (!load_header_from_input_stream(&common_header, &common_data_offset, gfx_input_stream, gfx_input_stream_read_callback, gfx_input_stream_seek_callback) || !this->texture_streaming_stage_first_pre_populate_task_data_callback(gfx_connection, &common_header))
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
            mcrt_task_ref task = mcrt_task_allocate_root(texture_streaming_stage_second_task_execute);
            static_assert(sizeof(struct texture_streaming_stage_second_task_data_t) <= sizeof(mcrt_task_user_data_t), "");
            struct texture_streaming_stage_second_task_data_t *task_data = reinterpret_cast<struct texture_streaming_stage_second_task_data_t *>(mcrt_task_get_user_data(task));
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

mcrt_task_ref gfx_texture_base::texture_streaming_stage_second_task_execute(mcrt_task_ref self)
{
    static_assert(sizeof(struct texture_streaming_stage_second_task_data_t) <= sizeof(mcrt_task_user_data_t), "");
    struct texture_streaming_stage_second_task_data_t *task_data = reinterpret_cast<struct texture_streaming_stage_second_task_data_t *>(mcrt_task_get_user_data(self));

    // different master task doesn't share the task_arena
    // we need to share the same the task arena to make sure the "tbb::this_task_arena::current_thread_id" unique
    assert(NULL != mcrt_task_arena_internal_arena(task_data->m_gfx_connection->task_arena()));
    assert(mcrt_task_arena_internal_arena(task_data->m_gfx_connection->task_arena()) == mcrt_this_task_arena_internal_arena());

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

    task_data->m_gfx_connection->streaming_task_mark_execute_begin(streaming_throttling_index);

    mcrt_task_set_parent(self, task_data->m_gfx_connection->streaming_task_root(streaming_throttling_index));

    if (!streaming_cancel)
    {
        // pre calculate total size
        struct common_header_t common_header;
        size_t common_data_offset;
        void *memcpy_dest;
        void *cmdcopy_dest;
        if (PT_UNLIKELY(!load_header_from_input_stream(&common_header, &common_data_offset, task_data->m_gfx_input_stream, task_data->m_gfx_input_stream_read_callback, task_data->m_gfx_input_stream_seek_callback)) || !task_data->m_gfx_streaming_object->texture_streaming_stage_second_pre_calculate_total_size_callback(&common_header, &memcpy_dest, &cmdcopy_dest))
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

        // try to allocate memory from staging buffer
        {
            uint64_t transfer_src_buffer_begin = task_data->m_gfx_connection->transfer_src_buffer_begin(streaming_throttling_index);
            uint64_t transfer_src_buffer_end = uint64_t(-1);
            uint64_t transfer_src_buffer_size = task_data->m_gfx_connection->transfer_src_buffer_size(streaming_throttling_index);
            uint64_t base_offset = uint64_t(-1);

            do
            {
                base_offset = mcrt_atomic_load(task_data->m_gfx_connection->transfer_src_buffer_end(streaming_throttling_index));

                size_t total_size = task_data->m_gfx_streaming_object->texture_streaming_stage_second_calculate_total_size_callback(task_data->m_gfx_connection, &common_header, memcpy_dest, cmdcopy_dest, base_offset);

                transfer_src_buffer_end = (base_offset + total_size);
                //assert((transfer_src_buffer_end - transfer_src_buffer_begin) <= transfer_src_buffer_size);

                // respawn if memory not enough
                if ((transfer_src_buffer_end - transfer_src_buffer_begin) > transfer_src_buffer_size)
                {
                    // recycle to prevent free_task
                    mcrt_task_recycle_as_child_of(self, task_data->m_gfx_connection->streaming_task_respawn_root());

                    task_data->m_gfx_connection->streaming_task_respawn_list_push(streaming_throttling_index, self);

                    // release temp memory for the calculation
                    task_data->m_gfx_streaming_object->texture_streaming_stage_second_post_calculate_total_size_fail_callback(memcpy_dest, cmdcopy_dest);

                    // recycle needs manually tally_completion_of_predecessor
                    task_data->m_gfx_connection->streaming_task_mark_executie_end(streaming_throttling_index);
                    // the "mcrt_task_decrement_ref_count" must be called after all works(include the C++ destructors) are done
                    mcrt_task_decrement_ref_count(task_data->m_gfx_connection->streaming_task_root(streaming_throttling_index));
                    return NULL;
                }

            } while (base_offset != mcrt_atomic_cas_u64(task_data->m_gfx_connection->transfer_src_buffer_end(streaming_throttling_index), transfer_src_buffer_end, base_offset));
        }

        // post calculate total size
        if (!task_data->m_gfx_streaming_object->texture_streaming_stage_second_post_calculate_total_size_success_callback(task_data->m_gfx_connection, streaming_throttling_index, &common_header, &common_data_offset, memcpy_dest, cmdcopy_dest, task_data->m_gfx_input_stream, task_data->m_gfx_input_stream_read_callback, task_data->m_gfx_input_stream_seek_callback))
        {
            mcrt_atomic_store(&task_data->m_gfx_streaming_object->m_streaming_error, true);
            task_data->m_gfx_input_stream_destroy_callback(task_data->m_gfx_input_stream);
            mcrt_atomic_store(&task_data->m_gfx_streaming_object->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
            task_data->m_gfx_connection->streaming_object_list_push(streaming_throttling_index, task_data->m_gfx_streaming_object);
            task_data->m_gfx_connection->streaming_task_mark_executie_end(streaming_throttling_index);
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
    task_data->m_gfx_connection->streaming_task_mark_executie_end(streaming_throttling_index);
    return NULL;
}

void gfx_texture_base::destroy(class gfx_connection_base *gfx_connection)
{
    this->release(gfx_connection);
}

void gfx_texture_base::addref()
{
    PT_MAYBE_UNUSED uint32_t ref_count = mcrt_atomic_inc_u32(&this->m_ref_count);
    // can't set_mesh after destory
    assert(1U < ref_count);
}

void gfx_texture_base::release(class gfx_connection_base *gfx_connection)
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
inline gfx_connection_ref wrap(class gfx_connection_base *gfx_connection) { return reinterpret_cast<gfx_connection_ref>(gfx_connection); }
inline class gfx_connection_base *unwrap(gfx_connection_ref gfx_connection) { return reinterpret_cast<class gfx_connection_base *>(gfx_connection); }

inline gfx_texture_ref wrap(class gfx_texture_base *texture) { return reinterpret_cast<gfx_texture_ref>(texture); }
inline class gfx_texture_base *unwrap(gfx_texture_ref texture) { return reinterpret_cast<class gfx_texture_base *>(texture); }

PT_ATTR_GFX bool PT_CALL gfx_texture_read_input_stream(
    gfx_connection_ref gfx_connection,
    gfx_texture_ref texture,
    char const *initial_filename,
    gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename),
    intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count),
    int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence),
    void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream))
{
    return unwrap(texture)->read_input_stream(unwrap(gfx_connection), initial_filename, input_stream_init_callback, input_stream_read_callback, input_stream_seek_callback, input_stream_destroy_callback);
}

PT_ATTR_GFX void PT_CALL gfx_texture_destroy(gfx_connection_ref gfx_connection, gfx_texture_ref texture)
{
    return unwrap(texture)->destroy(unwrap(gfx_connection));
}