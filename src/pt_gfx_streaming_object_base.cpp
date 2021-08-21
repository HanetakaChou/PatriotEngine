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
    void *user_defined)
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

            if (gfx_input_stream_ref(-1) == input_stream)
            {
                mcrt_atomic_store(&this->m_streaming_error, true);
                return false;
            }

            if (this->streaming_stage_first_parse_header_callback(input_stream, input_stream_read_callback, input_stream_seek_callback, initial_filename, user_defined))
            {
                mcrt_atomic_store(&this->m_streaming_error, true);
                return false;
            }
        }

        // pass to the second stage
        {
            mcrt_task_ref task = mcrt_task_allocate_root(streaming_stage_second_task_execute);
            static_assert(sizeof(struct streaming_stage_second_task_data_base_t) <= sizeof(mcrt_task_user_data_t), "");
            struct streaming_stage_second_task_data_base_t *task_data = reinterpret_cast<struct streaming_stage_second_task_data_base_t *>(mcrt_task_get_user_data(task));
            // There is no constructor of the POD "mcrt_task_user_data_t"
            new (&task_data->m_initial_filename) mcrt_string(initial_filename);
            task_data->m_input_stream_init_callback = input_stream_init_callback;
            task_data->m_input_stream_read_callback = input_stream_read_callback;
            task_data->m_input_stream_seek_callback = input_stream_seek_callback;
            task_data->m_input_stream_destroy_callback = input_stream_destroy_callback;
            task_data->m_gfx_connection = gfx_connection;
            this->streaming_stage_first_populate_task_data_callback(&task_data->m_user_defined_data_specific, user_defined);

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

void gfx_streaming_object_base::set_streaming_done(class gfx_connection_base *gfx_connection)
{
    this->streaming_done_lock();

    PT_MAYBE_UNUSED streaming_status_t streaming_status = this->m_streaming_status;
    PT_MAYBE_UNUSED bool streaming_error = this->m_streaming_error;
    bool streaming_cancel = this->m_streaming_cancel;
    assert(streaming_error || STREAMING_STATUS_STAGE_THIRD == streaming_status);
    assert(!streaming_error || STREAMING_STATUS_STAGE_SECOND == streaming_status);

    if (!streaming_cancel)
    {
        this->m_streaming_status = STREAMING_STATUS_DONE;
    }
    else
    {
        this->streaming_destroy_callback(gfx_connection);
    }

    this->streaming_done_unlock();
}
