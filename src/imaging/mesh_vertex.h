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
// location = 1 binding = 1 qtangentxyz_xyz_reflection_w VK_FORMAT_A2B10G10R10_UNORM_PACK32
// location = 2 binding = 1 qtangentw_x_uv_yz VK_FORMAT_A2B10G10R10_UNORM_PACK32
struct mesh_vertex_varying
{
    uint32_t qtangentxyz_xyz_reflection_w;
    uint32_t qtangentw_x_uv_yz;
};

extern uint32_t mesh_vertex_float4_to_r8g8b8a8_snorm(float unpacked_input[4]);

extern uint32_t mesh_vertex_float4_to_r10g10b10a2_unorm(float unpacked_input[4]);

extern uint32_t mesh_vertex_float4_to_r16g16_unorm(float unpacked_input[2]);

extern void mesh_vertex_compute_tangent_frame(
    size_t face_count,
    uint32_t const* indices,
    size_t vertex_count,
    pt_math_vec3 const *positions,
    pt_math_vec3 const *normals,
    pt_math_vec2 const *uvs,
    pt_math_vec4 *out_qtangents,
    float* out_reflections);

#endif
