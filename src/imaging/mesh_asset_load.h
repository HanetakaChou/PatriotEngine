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

#ifndef _IMAGING_MESH_ASSET_LOAD_H_
#define _IMAGING_MESH_ASSET_LOAD_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_io.h>

// TODO: vec2 normal and tangent
// From industrial view, the Z value can be calculated by Z = (1 - X2 - Y2)^(1/2) since the vector is normalized.
// From academic view, the sphere surface is 2-dimensional.

//"QTangent" [Spherical Skinning with Dual-Quaternions and QTangents]

// In Vulkan
// the whole pipeline will be affected if the "VertexInputState" is changed and thus we keep the "vertex format" fixed
// while the "index format" can be changed on-the-fly by "vkCmdBindIndexBuffer"

struct mesh_asset_header_t
{
    bool is_skined;
    uint32_t primitive_count; // material count
};

struct mesh_primitive_asset_header_t
{
    uint32_t vertex_count;
    bool is_index_type_uint16;
    uint32_t index_count;
};

struct pt_gfx_mesh_neutral_primitive_memcpy_dest_t
{
    size_t position_staging_offset;
    size_t position_staging_size;
    size_t varying_staging_offset;
    size_t varying_staging_size;
    size_t index_staging_offset;
    size_t index_staging_size;
};

extern bool mesh_load_header_from_input_stream(
    struct mesh_asset_header_t *out_mesh_asset_header,
    pt_input_stream_ref input_stream, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback);

extern bool mesh_load_primitive_headers_from_input_stream(
    struct mesh_asset_header_t const *mesh_asset_header_for_validate, struct mesh_primitive_asset_header_t *out_mesh_primitive_asset_header,
    pt_input_stream_ref input_stream, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback);

extern bool mesh_load_primitive_data_from_input_stream(
    struct mesh_asset_header_t const *mesh_asset_header_for_validate,
    struct mesh_primitive_asset_header_t const *mesh_primitive_asset_header_for_validate,
    void *staging_pointer,
    struct pt_gfx_mesh_neutral_primitive_memcpy_dest_t const *memcpy_dests,
    pt_input_stream_ref input_stream, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback);

#endif