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
        STREAMING_STATUS_NOT_LOAD,
        STREAMING_STATUS_IN_PROCESS,
        STREAMING_STATUS_READY,
        STREAMING_STATUS_ERROR
    };
    streaming_status_t m_streaming_status;

    inline gfx_streaming_object(streaming_status_t streaming_status) : m_streaming_status(streaming_status) {}

public:
    void mark_ready();
};

#endif