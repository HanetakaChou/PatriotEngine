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

#ifndef _PT_GFX_STREAMING_OBJECT_H_
#define _PT_GFX_STREAMING_OBJECT_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_mcrt_scalable_allocator.h>
#include <string>

class gfx_streaming_object
{

protected:
    //using vector = std::vector<T, mcrt::scalable_allocator<T>>;
    using mcrt_string = std::basic_string<char, std::char_traits<char>, mcrt::scalable_allocator<char>>;
    mcrt_string m_asset_filename;

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

    uint32_t m_spin_lock_streaming_done;

    inline gfx_streaming_object(streaming_status_t streaming_status) : m_streaming_status(streaming_status), m_streaming_error(false), m_streaming_cancel(false), m_spin_lock_streaming_done(0U) {}
public:
    void streaming_done();
    virtual void streaming_cancel() = 0;
};

#endif