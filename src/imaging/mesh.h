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

#ifndef _IMAGING_MESH_H_
#define _IMAGING_MESH_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_gfx_connection.h>
#include <pt_mcrt_allocator.h>
#include <pt_mcrt_vector.h>
#include <pt_gfx_frame_object_base.h>
#include "streaming_object.h"
#include "mesh_asset_load.h"

class gfx_mesh_base : public gfx_streaming_object_base, public gfx_frame_object_base
{
    uint32_t m_ref_count;

protected:
    struct mesh_asset_header_t m_mesh_asset_header;
    mcrt_vector<struct mesh_primitive_asset_header_t> m_mesh_asset_primitive_headers;

    inline gfx_mesh_base() : gfx_streaming_object_base(), gfx_frame_object_base(), m_ref_count(1U) {}

public:
    inline uint32_t get_primitive_count() { return this->m_mesh_asset_header.primitive_count; }

    void destroy(class gfx_connection_base *gfx_connection);

    void addref();

    void release(class gfx_connection_base *gfx_connection);
};

#endif