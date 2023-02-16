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

#ifndef _IMAGING_TEXTURE_H_
#define _IMAGING_TEXTURE_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_gfx_connection.h>
#include "streaming_object.h"
#include "texture_asset_load.h"

class gfx_texture_base : public gfx_streaming_object_base, public gfx_frame_object_base
{
    uint32_t m_ref_count;

protected:
    struct gfx_texture_neutral_header_t m_texture_asset_header;
    size_t m_texture_asset_data_offset;

    inline gfx_texture_base() : gfx_streaming_object_base(), gfx_frame_object_base(), m_ref_count(1U) {}

public:
    void destroy(class gfx_connection_base *gfx_connection);

    void addref();

    void release(class gfx_connection_base *gfx_connection);
};

#endif
