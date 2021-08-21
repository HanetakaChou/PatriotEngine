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

#include <assert.h>
#include "pt_gfx_streaming_object_base.h"

bool gfx_streaming_object_base::streaming_stage_first_execute(
    class gfx_connection_base *gfx_connection,
    char const *initial_filename,
    gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename),
    intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count),
    int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence),
    void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream),
    void *streaming_stage_first_thread_stack_data_user_defined)
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
        gfx_input_stream_ref input_stream = input_stream_init_callback(initial_filename);
        if (gfx_input_stream_ref(-1) == input_stream)
        {
            mcrt_atomic_store(&this->m_streaming_error, true);
            return false;
        }

        if (!this->streaming_stage_first_pre_populate_task_data_callback(gfx_connection, input_stream, input_stream_read_callback, input_stream_seek_callback, streaming_stage_first_thread_stack_data_user_defined))
        {
            mcrt_atomic_store(&this->m_streaming_error, true);
            return false;
        }

        // pass to the second stage
        {
            mcrt_task_ref task = mcrt_task_allocate_root(streaming_stage_second_task_execute);
            static_assert(sizeof(struct streaming_stage_second_task_data_base_t) <= sizeof(mcrt_task_user_data_t), "");
            struct streaming_stage_second_task_data_base_t *task_data = reinterpret_cast<struct streaming_stage_second_task_data_base_t *>(mcrt_task_get_user_data(task));
            task_data->m_streaming_object = this;
            task_data->m_gfx_connection = gfx_connection;
            task_data->m_input_stream = input_stream;
            task_data->m_input_stream_read_callback = input_stream_read_callback;
            task_data->m_input_stream_seek_callback = input_stream_seek_callback;
            task_data->m_input_stream_destroy_callback = input_stream_destroy_callback;
            this->streaming_stage_first_populate_task_data_callback(streaming_stage_first_thread_stack_data_user_defined, &task_data->m_task_data_user_defined);

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

mcrt_task_ref gfx_streaming_object_base::streaming_stage_second_task_execute(mcrt_task_ref self)
{
    uint32_t streaming_throttling_index;
    bool recycle;
    // The "streaming_stage_second_task_execute_internal" makes sure that the "mcrt_task_decrement_ref_count" is called after all works(include the C++ destructors) are done
    mcrt_task_ref task_bypass = streaming_stage_second_task_execute_internal(&streaming_throttling_index, &recycle, self);

    if (!recycle)
    {
        static_assert(sizeof(struct streaming_stage_second_task_data_base_t) <= sizeof(mcrt_task_user_data_t), "");
        struct streaming_stage_second_task_data_base_t *task_data = reinterpret_cast<struct streaming_stage_second_task_data_base_t *>(mcrt_task_get_user_data(self));

        PT_MAYBE_UNUSED streaming_status_t streaming_status = mcrt_atomic_load(&task_data->m_streaming_object->m_streaming_status);
        PT_MAYBE_UNUSED bool streaming_error = mcrt_atomic_load(&task_data->m_streaming_object->m_streaming_error);
        assert(streaming_error || STREAMING_STATUS_STAGE_THIRD == streaming_status);
        assert(!streaming_error || STREAMING_STATUS_STAGE_SECOND == streaming_status);

        task_data->m_input_stream_destroy_callback(task_data->m_input_stream);
    }
    else
    {
        static_assert(sizeof(streaming_stage_second_task_data_base_t) <= sizeof(mcrt_task_user_data_t), "");
        streaming_stage_second_task_data_base_t *task_data = reinterpret_cast<streaming_stage_second_task_data_base_t *>(mcrt_task_get_user_data(self));

        // tally_completion_of_predecessor manually
        mcrt_task_decrement_ref_count(task_data->m_gfx_connection->streaming_task_root(streaming_throttling_index));
    }

    return task_bypass;
}

inline mcrt_task_ref gfx_streaming_object_base::streaming_stage_second_task_execute_internal(uint32_t *output_streaming_throttling_index, bool *output_recycle, mcrt_task_ref self)
{
    static_assert(sizeof(struct streaming_stage_second_task_data_base_t) <= sizeof(mcrt_task_user_data_t), "");
    struct streaming_stage_second_task_data_base_t *task_data = reinterpret_cast<struct streaming_stage_second_task_data_base_t *>(mcrt_task_get_user_data(self));

    // different master task doesn't share the task_arena
    // we need to share the same the task arena to make sure the "tbb::this_task_arena::current_thread_id" unique
    assert(NULL != mcrt_task_arena_internal_arena(task_data->m_gfx_connection->task_arena()));
    assert(mcrt_task_arena_internal_arena(task_data->m_gfx_connection->task_arena()) == mcrt_this_task_arena_internal_arena());

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
    task_data->m_gfx_connection->streaming_throttling_index_lock();
    uint32_t streaming_throttling_index = task_data->m_gfx_connection->streaming_throttling_index();
    mcrt_task_increment_ref_count(task_data->m_gfx_connection->streaming_task_root(streaming_throttling_index));
    task_data->m_gfx_connection->streaming_throttling_index_unlock();

    {
#if defined(PT_GFX_DEBUG_MCRT) && PT_GFX_DEBUG_MCRT
        class internal_streaming_task_debug_executing_guard
        {
            uint32_t m_streaming_throttling_index;
            class gfx_connection_base *m_gfx_connection;

        public:
            inline internal_streaming_task_debug_executing_guard(uint32_t streaming_throttling_index, class gfx_connection_base *gfx_connection)
                : m_streaming_throttling_index(streaming_throttling_index), m_gfx_connection(gfx_connection)
            {
                m_gfx_connection->streaming_task_mark_executing_begin(m_streaming_throttling_index);
            }
            inline ~internal_streaming_task_debug_executing_guard()
            {
                m_gfx_connection->streaming_task_mark_executing_end(m_streaming_throttling_index);
            }
        } instance_internal_streaming_task_debug_executing_guard(streaming_throttling_index, task_data->m_gfx_connection);
#endif

        mcrt_task_set_parent(self, task_data->m_gfx_connection->streaming_task_root(streaming_throttling_index));

        if (!streaming_cancel)
        {
            // thread stack data
            struct streaming_stage_second_thread_stack_data_user_defined_t thread_stack_data_user_defined;

            // load header
            // allocate temp memory for the calculation
            if (!task_data->m_streaming_object->streaming_stage_second_pre_calculate_total_size_callback(&thread_stack_data_user_defined, task_data->m_input_stream, task_data->m_input_stream_read_callback, task_data->m_input_stream_seek_callback, &task_data->m_task_data_user_defined))
            {
                mcrt_atomic_store(&task_data->m_streaming_object->m_streaming_error, true);
                (*output_streaming_throttling_index) = streaming_throttling_index;
                (*output_recycle) = false;
                return NULL;
            }

            // try to allocate memory from staging buffer
            {
                uint64_t transfer_src_buffer_begin = task_data->m_gfx_connection->transfer_src_buffer_begin(streaming_throttling_index);
                uint64_t transfer_src_buffer_end = uint64_t(-1);
                uint64_t transfer_src_buffer_size = task_data->m_gfx_connection->transfer_src_buffer_size(streaming_throttling_index);
                uint64_t base_offset = uint64_t(-1);

                do
                {
                    base_offset = mcrt_atomic_load(task_data->m_gfx_connection->transfer_src_buffer_end(streaming_throttling_index));

                    size_t total_size = task_data->m_streaming_object->streaming_stage_second_calculate_total_size_callback(base_offset, &thread_stack_data_user_defined, task_data->m_gfx_connection, &task_data->m_task_data_user_defined);

                    transfer_src_buffer_end = (base_offset + total_size);
                    //assert((transfer_src_buffer_end - transfer_src_buffer_begin) <= transfer_src_buffer_size);

                    // respawn if memory not enough
                    if ((transfer_src_buffer_end - transfer_src_buffer_begin) > transfer_src_buffer_size)
                    {
                        // recycle to prevent free_task
                        mcrt_task_recycle_as_child_of(self, task_data->m_gfx_connection->streaming_task_respawn_root());

                        task_data->m_gfx_connection->streaming_task_respawn_list_push(streaming_throttling_index, self);

                        // release temp memory for the calculation
                        task_data->m_streaming_object->streaming_stage_second_post_calculate_total_size_callback(false, streaming_throttling_index, &thread_stack_data_user_defined, task_data->m_gfx_connection, task_data->m_input_stream, task_data->m_input_stream_read_callback, task_data->m_input_stream_seek_callback, &task_data->m_task_data_user_defined);

                        // recycle needs manually tally_completion_of_predecessor
                        (*output_streaming_throttling_index) = streaming_throttling_index;
                        (*output_recycle) = true;
                        return NULL;
                    }

                } while (base_offset != mcrt_atomic_cas_u64(task_data->m_gfx_connection->transfer_src_buffer_end(streaming_throttling_index), transfer_src_buffer_end, base_offset));
            }

            // release temp memory for the calculation
            // load data
            // GPU copy cmd
            if (!task_data->m_streaming_object->streaming_stage_second_post_calculate_total_size_callback(true, streaming_throttling_index, &thread_stack_data_user_defined, task_data->m_gfx_connection, task_data->m_input_stream, task_data->m_input_stream_read_callback, task_data->m_input_stream_seek_callback, &task_data->m_task_data_user_defined))
            {
                mcrt_atomic_store(&task_data->m_streaming_object->m_streaming_error, true);
                (*output_streaming_throttling_index) = streaming_throttling_index;
                (*output_recycle) = false;
                return NULL;
            }

            // pass to the third stage
            task_data->m_gfx_connection->streaming_object_list_push(streaming_throttling_index, task_data->m_streaming_object);

            mcrt_atomic_store(&task_data->m_streaming_object->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
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
            task_data->m_gfx_connection->streaming_object_list_push(streaming_throttling_index, task_data->m_streaming_object);

            mcrt_atomic_store(&task_data->m_streaming_object->m_streaming_status, STREAMING_STATUS_STAGE_THIRD);
#endif
        }
    }

    (*output_streaming_throttling_index) = streaming_throttling_index;
    (*output_recycle) = false;
    return NULL;
}

void gfx_streaming_object_base::streaming_destroy_request(bool *streaming_done)
{
    // make sure this function happens before or after the gfx_streaming_object_base::streaming_done
    this->streaming_done_lock();

    streaming_status_t streaming_status = mcrt_atomic_load(&this->m_streaming_status);

    if (STREAMING_STATUS_STAGE_FIRST == streaming_status || STREAMING_STATUS_STAGE_SECOND == streaming_status || STREAMING_STATUS_STAGE_THIRD == streaming_status)
    {
        mcrt_atomic_store(&this->m_streaming_cancel, true);
        (*streaming_done) = false;
    }
    else if (STREAMING_STATUS_DONE == streaming_status)
    {
        (*streaming_done) = true;
    }
    else
    {
        assert(0);
        (*streaming_done) = false;
    }

    this->streaming_done_unlock();
}

void gfx_streaming_object_base::streaming_done_execute(class gfx_connection_base *gfx_connection)
{
    this->streaming_done_lock();

    PT_MAYBE_UNUSED streaming_status_t streaming_status = this->m_streaming_status;
    PT_MAYBE_UNUSED bool streaming_error = this->m_streaming_error;
    bool streaming_cancel = this->m_streaming_cancel;
    assert(streaming_error || STREAMING_STATUS_STAGE_THIRD == streaming_status);
    assert(!streaming_error || STREAMING_STATUS_STAGE_SECOND == streaming_status);

    if (!streaming_cancel)
    {
        mcrt_atomic_store(&this->m_streaming_status, STREAMING_STATUS_DONE);
    }
    else
    {
        this->streaming_destroy_callback(gfx_connection);
    }

    this->streaming_done_unlock();
}
