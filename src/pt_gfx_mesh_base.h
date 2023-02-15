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

#ifndef _PT_GFX_MESH_BASE_H_
#define _PT_GFX_MESH_BASE_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_gfx_connection.h>
#include <pt_mcrt_scalable_allocator.h>
#include <pt_mcrt_vector.h>
#include "pt_gfx_streaming_object_base.h"
#include "pt_gfx_frame_object_base.h"
#include "pt_gfx_mesh_base_load.h"

class gfx_mesh_base : public gfx_streaming_object_base, public gfx_frame_object_base
{
    uint32_t m_ref_count;

protected:
    struct pt_gfx_mesh_neutral_header_t m_mesh_asset_header;
    mcrt_vector<struct pt_gfx_mesh_neutral_primitive_header_t> m_mesh_asset_primitive_headers;

    inline gfx_mesh_base() : gfx_streaming_object_base(), gfx_frame_object_base(), m_ref_count(1U) {}

public:
    bool read_input_stream(class gfx_connection_base *gfx_connection, char const *file_name, struct pt_gfx_input_stream_callbacks_t const *gfx_input_stream_callbacks);

    void destroy(class gfx_connection_base *gfx_connection);

    void addref();

    void release(class gfx_connection_base *gfx_connection);
};

#endif