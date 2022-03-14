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

#ifndef _PT_GFX_MESH_BASE_LOAD_H_
#define _PT_GFX_MESH_BASE_LOAD_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_gfx_connection.h>

// TODO: vec2 normal and tangent
// From industrial view, the Z value can be calculated by Z = (1 - X2 - Y2)^(1/2) since the vector is normalized.
// From academic view, the sphere surface is 2-dimensional.

//"QTangent" [Spherical Skinning with Dual-Quaternions and QTangents]

// In Vulkan
// the whole pipeline will be affected if the "VertexInputState" is changed and thus we keep the "vertex format" fixed
// while the "index format" can be changed on-the-fly by "vkCmdBindIndexBuffer"

struct pt_gfx_mesh_neutral_header_t
{
    bool is_skined;
    uint32_t primitive_count; // material count
};

struct pt_gfx_mesh_neutral_primitive_header_t
{
    uint32_t vertex_count;
    bool is_index_type_uint16;
    uint32_t index_count;
};

// vertex bindings/attributes
// location = 0 binding = 0 position VK_FORMAT_R32G32B32_SFLOAT
// location = 1 binding = 1 normal VK_FORMAT_R32G32B32_SFLOAT
// location = 2 binding = 1 tangent VK_FORMAT_R32G32B32_SFLOAT
// location = 3 binding = 1 uv DXGI_FORMAT_R32G32_FLOAT
struct pt_gfx_mesh_neutral_vertex_position
{
    pt_math_vec3 position;
};

struct pt_gfx_mesh_neutral_vertex_varying
{
    pt_math_vec3 normal;
    pt_math_vec3 tangent;
    pt_math_vec2 uv;
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

extern bool mesh_load_header_from_input_stream(struct pt_gfx_mesh_neutral_header_t *out_neutral_header, pt_gfx_input_stream_ref gfx_input_stream, struct pt_gfx_input_stream_callbacks_t const *gfx_input_stream_callbacks);

extern bool mesh_load_primitive_headers_from_input_stream(struct pt_gfx_mesh_neutral_header_t const *neutral_header_for_validate, struct pt_gfx_mesh_neutral_primitive_header_t *out_neutral_primitive_headers, pt_gfx_input_stream_ref gfx_input_stream, struct pt_gfx_input_stream_callbacks_t const *gfx_input_stream_callbacks);

extern bool mesh_load_primitive_data_from_input_stream(struct pt_gfx_mesh_neutral_header_t const *neutral_header_for_validate, struct pt_gfx_mesh_neutral_primitive_header_t const *neutral_primitive_headers_for_validate, void *staging_pointer, struct pt_gfx_mesh_neutral_primitive_memcpy_dest_t const *memcpy_dests, pt_gfx_input_stream_ref gfx_input_stream, struct pt_gfx_input_stream_callbacks_t const *gfx_input_stream_callbacks);

#endif