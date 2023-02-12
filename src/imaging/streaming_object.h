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

#ifndef _IMAGING_STREAMING_OBJECT_H_
#define _IMAGING_STREAMING_OBJECT_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_mcrt_task.h>
#include <pt_mcrt_spinlock.h>
#include "../pt_gfx_connection_base.h"
#include <assert.h>

class gfx_streaming_object_base
{
protected:
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

protected:
    inline void streaming_done_lock()
    {
        mcrt_spin_lock(&this->m_spinlock_streaming_done);
    }

    inline void streaming_done_unlock()
    {
        mcrt_spin_unlock(&this->m_spinlock_streaming_done);
    }

    // destory before "streaming_done"
    virtual void pre_streaming_done_destroy_callback(class gfx_connection_base *gfx_connection) = 0;

private:
    // "streaming_done"
    virtual bool streaming_done_callback(class gfx_connection_base *gfx_connection) = 0;
    // destory after "streaming_done"
    virtual void post_stream_done_destroy_callback(class gfx_connection_base *gfx_connection) = 0;

    // load - first stage
    virtual bool load_header_callback(
        pt_gfx_input_stream_ref input_stream,
        pt_gfx_input_stream_read_callback input_stream_read_callback,
        pt_gfx_input_stream_seek_callback input_stream_seek_callback,
        class gfx_connection_base *gfx_connection,
        size_t *out_memcpy_dests_size,
        size_t *out_memcpy_dests_align)
    {
        return false;
    };

    // load - second stage
    static mcrt_task_ref load_task_execute(mcrt_task_ref self);
    virtual size_t calculate_staging_buffer_total_size_callback(
        size_t base_offset,
        class gfx_connection_base *gfx_connection,
        void *out_memcpy_dests)
    {
        return -1;
    };
    virtual bool load_data_callback(
        pt_gfx_input_stream_ref input_stream,
        pt_gfx_input_stream_read_callback input_stream_read_callback,
        pt_gfx_input_stream_seek_callback input_stream_seek_callback,
        class gfx_connection_base *gfx_connection,
        void const *memcpy_dests,
        uint32_t streaming_throttling_index)
    {
        return false;
    };

protected:
    inline gfx_streaming_object_base() : m_streaming_status(STREAMING_STATUS_STAGE_FIRST), m_streaming_error(false), m_streaming_cancel(false)
    {
        mcrt_spin_init(&this->m_spinlock_streaming_done);
    }

    void streaming_destroy_request(class gfx_connection_base *gfx_connection);

public:
    void streaming_done_execute(class gfx_connection_base *gfx_connection);

    void post_stream_done_destroy_execute(class gfx_connection_base *gfx_connection);

    inline bool is_streaming_error() const
    {
        return (mcrt_atomic_load(&this->m_streaming_error));
    }

    inline bool is_streaming_done() const
    {
        // we need to check "is_streaming_error" first
        assert((STREAMING_STATUS_DONE != mcrt_atomic_load(&this->m_streaming_status)) || (!mcrt_atomic_load(&this->m_streaming_error)));

        return (STREAMING_STATUS_DONE == mcrt_atomic_load(&this->m_streaming_status));
    }

    bool load(
        class gfx_connection_base *gfx_connection,
        char const *initial_filename,
        pt_gfx_input_stream_init_callback gfx_input_stream_init_callback,
        pt_gfx_input_stream_read_callback gfx_input_stream_read_callback,
        pt_gfx_input_stream_seek_callback gfx_input_stream_seek_callback,
        pt_gfx_input_stream_destroy_callback gfx_input_stream_destroy_callback);
};

#endif