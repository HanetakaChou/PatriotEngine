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

    virtual void streaming_destroy_callback(class gfx_connection_base *gfx_connection) = 0;

private:
    virtual bool streaming_done_callback(class gfx_connection_base *gfx_connection) = 0;

protected:
    inline gfx_streaming_object_base() : m_streaming_status(STREAMING_STATUS_STAGE_FIRST), m_streaming_error(false), m_streaming_cancel(false)
    {
        mcrt_spin_init(&this->m_spinlock_streaming_done);
    }

    void streaming_destroy_request(bool *streaming_done);

public:
    void streaming_done_execute(class gfx_connection_base *gfx_connection);

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
};

#endif