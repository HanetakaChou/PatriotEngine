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
#include <string>
#include <pt_mcrt_task.h>
#include <pt_mcrt_spinlock.h>
#include <pt_mcrt_scalable_allocator.h>
#include "pt_gfx_connection_base.h"

class gfx_streaming_object_base
{
    virtual bool streaming_stage_first_parse_header_callback(
        gfx_input_stream_ref input_stream,
        intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count),
        int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence),
        char const *initial_filename,
        void *user_defined) { return false; };

    struct streaming_stage_second_task_data_specific_t
    {
        uint8_t m_user_defined_data[64];
    };

    virtual void streaming_stage_first_populate_task_data_callback(
        struct streaming_stage_second_task_data_specific_t *user_defined_data_specific,
        void *user_defined) {};

    virtual void streaming_destroy_callback(class gfx_connection_base *gfx_connection) = 0;

    using mcrt_string = std::basic_string<char, std::char_traits<char>, mcrt::scalable_allocator<char>>;

    struct streaming_stage_second_task_data_base_t
    {
        mcrt_string m_initial_filename;
        gfx_input_stream_ref(PT_PTR *m_input_stream_init_callback)(char const *initial_filename);
        intptr_t(PT_PTR *m_input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count);
        int64_t(PT_PTR *m_input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence);
        void(PT_PTR *m_input_stream_destroy_callback)(gfx_input_stream_ref input_stream);
        class gfx_connection_base *m_gfx_connection;
        struct streaming_stage_second_task_data_specific_t m_user_defined_data_specific;
    };
    static_assert(sizeof(struct streaming_stage_second_task_data_base_t) <= sizeof(mcrt_task_user_data_t), "");

    static mcrt_task_ref streaming_stage_second_task_execute(mcrt_task_ref self) { return NULL; };

    static inline mcrt_task_ref streaming_stage_second_task_execute_internal(uint32_t *output_streaming_throttling_index, bool *output_recycle, mcrt_task_ref self);

protected:
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

    mcrt_spinlock_t m_spinlock_streaming_done;

    inline gfx_streaming_object_base() : m_streaming_status(STREAMING_STATUS_STAGE_FIRST), m_streaming_error(false), m_streaming_cancel(false)
    {
        mcrt_spin_init(&this->m_spinlock_streaming_done);
    }

    inline void streaming_done_lock()
    {
        mcrt_spin_lock(&this->m_spinlock_streaming_done);
    }

    inline void streaming_done_unlock()
    {
        mcrt_spin_unlock(&this->m_spinlock_streaming_done);
    }

public:
    bool streaming_stage_first_execute(
        class gfx_connection_base *gfx_connection,
        char const *initial_filename,
        gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename),
        intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count),
        int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence),
        void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream),
        void *user_defined);

    void set_streaming_done(class gfx_connection_base *gfx_connection);

    inline bool is_streaming_done() { return STREAMING_STATUS_DONE == this->m_streaming_status; }
};

#endif