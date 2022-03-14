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

#include <assert.h>
#include "pt_gfx_streaming_object_base.h"

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