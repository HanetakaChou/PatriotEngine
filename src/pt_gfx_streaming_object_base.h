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

#ifndef _PT_GFX_STREAMING_OBJECT_BASE_H_
#define _PT_GFX_STREAMING_OBJECT_BASE_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_mcrt_task.h>
#include <pt_mcrt_spinlock.h>
#include "pt_gfx_connection_base.h"

class gfx_streaming_object_base
{
    // CryEngine
    // m_eStreamingStatus
    enum streaming_status_t
    {
        STREAMING_STATUS_STAGE_FIRST,
        STREAMING_STATUS_STAGE_SECOND,
        STREAMING_STATUS_STAGE_THIRD,
        STREAMING_STATUS_DONE,
    };
    streaming_status_t m_streaming_status;
    bool m_streaming_error;
    bool m_streaming_cancel;

protected:
    struct streaming_stage_second_thread_stack_data_user_defined_t
    {
        uint8_t m_user_defined_data[128];
    };

    struct streaming_stage_second_task_data_user_defined_t
    {
        uint8_t m_user_defined_data[48];
    };

private:
    virtual bool streaming_stage_first_pre_populate_task_data_callback(class gfx_connection_base *gfx_connection, gfx_input_stream_ref input_stream, intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref, void *, size_t), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref, int64_t, int), void *streaming_stage_first_thread_stack_data_user_defined) = 0;

    virtual void streaming_stage_first_populate_task_data_callback(void *streaming_stage_first_thread_stack_data_user_defined, struct streaming_stage_second_task_data_user_defined_t *task_data_user_defined) = 0;

    virtual bool streaming_stage_second_pre_calculate_total_size_callback(struct streaming_stage_second_thread_stack_data_user_defined_t *thread_stack_data_user_defined, gfx_input_stream_ref input_stream, intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref, void *, size_t), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref, int64_t, int), struct streaming_stage_second_task_data_user_defined_t *task_data_user_defined) = 0;

    virtual size_t streaming_stage_second_calculate_total_size_callback(uint64_t base_offset, struct streaming_stage_second_thread_stack_data_user_defined_t *thread_stack_data_user_defined, class gfx_connection_base *gfx_connection, struct streaming_stage_second_task_data_user_defined_t *task_data_user_defined) = 0;

    virtual bool streaming_stage_second_post_calculate_total_size_callback(bool staging_buffer_allocate_success, uint32_t streaming_throttling_index, struct streaming_stage_second_thread_stack_data_user_defined_t *thread_stack_data_user_defined, class gfx_connection_base *gfx_connection, gfx_input_stream_ref input_stream, intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref, void *, size_t), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref, int64_t, int), struct streaming_stage_second_task_data_user_defined_t *task_data_user_defined) = 0;

    struct streaming_stage_second_task_data_base_t
    {
        class gfx_streaming_object_base *m_streaming_object;
        class gfx_connection_base *m_gfx_connection;
        gfx_input_stream_ref m_input_stream;
        intptr_t(PT_PTR *m_input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count);
        int64_t(PT_PTR *m_input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence);
        void(PT_PTR *m_input_stream_destroy_callback)(gfx_input_stream_ref input_stream);
        struct streaming_stage_second_task_data_user_defined_t m_task_data_user_defined;
    };
    static_assert(sizeof(struct streaming_stage_second_task_data_base_t) <= sizeof(mcrt_task_user_data_t), "");

    static mcrt_task_ref streaming_stage_second_task_execute(mcrt_task_ref self);

    static inline mcrt_task_ref streaming_stage_second_task_execute_internal(uint32_t *output_streaming_throttling_index, bool *output_recycle, mcrt_task_ref self);

    mcrt_spinlock_t m_spinlock_streaming_done;

    inline void streaming_done_lock()
    {
        mcrt_spin_lock(&this->m_spinlock_streaming_done);
    }

    inline void streaming_done_unlock()
    {
        mcrt_spin_unlock(&this->m_spinlock_streaming_done);
    }

    virtual void streaming_destroy_callback(class gfx_connection_base *gfx_connection) = 0;

protected:
    inline gfx_streaming_object_base() : m_streaming_status(STREAMING_STATUS_STAGE_FIRST), m_streaming_error(false), m_streaming_cancel(false)
    {
        mcrt_spin_init(&this->m_spinlock_streaming_done);
    }

    bool streaming_stage_first_execute(class gfx_connection_base *gfx_connection, char const *initial_filename, gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename), intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence), void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream), void *streaming_stage_first_thread_stack_data_user_defined);

    void streaming_destroy_request(bool *streaming_done);

public:
    void streaming_done_execute(class gfx_connection_base *gfx_connection);

    inline bool is_streaming_done() { return (STREAMING_STATUS_DONE == mcrt_atomic_load(&this->m_streaming_status)); }
};

#endif