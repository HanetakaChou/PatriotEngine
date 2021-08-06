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
#include "pt_gfx_streaming_object.h"
#include <pt_mcrt_atomic.h>
#include <pt_mcrt_thread.h>

void gfx_streaming_object::streaming_done()
{
    while (0U != mcrt_atomic_xchg_u32(&this->m_spin_lock_streaming_done, 1U))
    {
        mcrt_os_yield();
    }

    streaming_status_t streaming_status = this->m_streaming_status;
    bool streaming_error = this->m_streaming_error;
    bool streaming_cancel = this->m_streaming_cancel;
    assert(STREAMING_STATUS_STAGE_THIRD == streaming_status);

    if (!streaming_cancel)
    {
        this->m_streaming_status = STREAMING_STATUS_DONE;
    }
    else
    {
        this->streaming_cancel();
    }

    mcrt_atomic_store(&this->m_spin_lock_streaming_done, 0U);
}
