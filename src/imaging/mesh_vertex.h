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

#ifndef _IMAGING_MESH_VERTEX_H_
#define _IMAGING_MESH_VERTEX_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_math.h>

// vertex bindings/attributes
// location = 0 binding = 0 position VK_FORMAT_R32G32B32_SFLOAT
struct mesh_vertex_position
{
    float position[3];
};

// vertex bindings/attributes
// location = 1 binding = 1 normal VK_FORMAT_R8G8B8A8_SNORM
// location = 2 binding = 1 tangent VK_FORMAT_R8G8B8A8_SNORM
// location = 3 binding = 1 uv VK_FORMAT_R16G16_UNORM
struct mesh_vertex_varying
{

    uint8_t normal[4];
    uint8_t tangent[4];
    uint16_t uv[2];
};

extern int8_t mesh_vertex_float_to_8_snorm(float unpacked_input);

extern uint16_t mesh_vertex_float_to_16_unorm(float unpacked_input);

extern void mesh_vertex_compute_tangent_frame(
    size_t face_count,
    uint32_t const* indices,
    size_t vertex_count,
    pt_math_vec3 const positions[3],
    pt_math_vec3 const normals[3],
    pt_math_vec2 const uvs[3]);

#endif
