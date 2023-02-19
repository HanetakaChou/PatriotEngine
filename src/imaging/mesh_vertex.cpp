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

#include "mesh_vertex.h"
#include <cfloat>
#include <cmath>
#include <algorithm>
#include <pt_mcrt_vector.h>
#include <assert.h>

extern int8_t mesh_vertex_float_to_8_snorm(float unpacked_input)
{
    // UE: [FPackedNormal](https://github.com/EpicGames/UnrealEngine/blob/4.27/Engine/Source/Runtime/RenderCore/Public/PackedNormal.h#L98)

    assert((-1.01F < unpacked_input) && (unpacked_input < 1.01F));

    // D3DX_DXGIFormatConvert.inl
    // D3DX_FLOAT4_to_R8G8B8A8_SNORM
    float saturate_signed_float = std::min(std::max(unpacked_input, -1.0F), 1.0F);
    float float_to_int = saturate_signed_float * static_cast<float>(INT8_MAX) + ((saturate_signed_float >= 0.0F) ? 0.5F : -0.5F);
    float truncate_float = (float_to_int >= 0.0F) ? std::floor(float_to_int) : std::ceil(float_to_int);
    int8_t packed_output = static_cast<int8_t>(truncate_float);
    return packed_output;
}

extern uint16_t mesh_vertex_float_to_16_unorm(float unpacked_input)
{
    assert((-0.01F < unpacked_input) && (unpacked_input < 1.01F));

    // D3DX_DXGIFormatConvert.inl
    // D3DX_FLOAT4_to_R8G8B8A8_UNORM
    float saturate_float = std::min(std::max(unpacked_input, 0.0F), 1.0F);
    float float_to_uint = saturate_float * static_cast<float>(UINT16_MAX) + 0.5F;
    float truncate_float = std::floor(float_to_uint);
    uint16_t packed_output = static_cast<uint16_t>(truncate_float);
    return packed_output;
}

extern void mesh_vertex_compute_tangent_frame(
    size_t face_count,
    uint32_t const *indices,
    size_t vertex_count,
    pt_math_vec3 const positions[3],
    pt_math_vec3 const normals[3],
    pt_math_vec2 const uvs[3])
{
    // [DirectX::ComputeTangentFrameImpl](https://github.com/microsoft/DirectXMesh/blob/dec2022/DirectXMesh/DirectXMeshTangentFrame.cpp#L22)

    mcrt_vector<pt_math_alignas16_vec3> tmp_tangents(vertex_count);
    mcrt_vector<pt_math_alignas16_vec3> tmp_bitangents(vertex_count);
    for (size_t vertex_index = 0U; vertex_index < vertex_count; ++vertex_index)
    {
        pt_math_store_vec3(&tmp_tangents[vertex_index], pt_math_vec_zero());
        pt_math_store_vec3(&tmp_bitangents[vertex_index], pt_math_vec_zero());
    }

    for (size_t face_index = 0U; face_index < face_count; ++face_index)
    {
        uint32_t i0 = indices[face_index * 3];
        uint32_t i1 = indices[face_index * 3 + 1];
        uint32_t i2 = indices[face_index * 3 + 2];

        pt_math_simd_vec const p0 = XMLoadFloat3(&positions[i0]);
        pt_math_simd_vec const p1 = XMLoadFloat3(&positions[i1]);
        pt_math_simd_vec const p2 = XMLoadFloat3(&positions[i2]);

        pt_math_simd_vec const uv0 = pt_math_load_vec2(&uvs[i0]);
        pt_math_simd_vec const uv1 = pt_math_load_vec2(&uvs[i1]);
        pt_math_simd_vec const uv2 = pt_math_load_vec2(&uvs[i2]);

        // p1 - p0 = (u1 - u0) * t + (v1 - v0) * b
        // p2 - p0 = (u2 - u0) * t + (v2 - v0) * b
        //
        // [ p1 - p0 ]   | u1 - u0  v1 - v0 |   | t |
        // [ p2 - p0 ] = | u2 - u0  v2 - v0 | * | b |
        //
        // determinant =  (u1 - u0) * (v2 - v0) - (u2 - u0) - (v1 - v0)
        //
        //                               |   v2 - v0    -(v1 - v0) |
        // inverse = (1.0 / determinant) | -(u2 - u0)     u1 - u0  |
        //
        // t = (1.0 / determinant) *   (v2 - v0)  * (p1 - p0) + (1.0 / determinant) * (-(v1 - v0)) * (p2 - p0)
        // b = (1.0 / determinant) * (-(u2 - u0)) * (p1 - p0) + (1.0 / determinant) *   (u1 - u0)  * (p2 - p0)

        // s = [u1 - u0, v1 - v0, u2 - u0, v1 - v0]
        pt_math_alignas16_vec4 s;
        pt_math_store_alignas16_vec4(&s, pt_math_vec_merge_xy(pt_math_vec_subtract(uv1, uv0), pt_math_vec_subtract(uv2, uv0)));

        // determinant =  (u1 - u0) * (v2 - v0) - (u2 - u0) - (v1 - v0)
        float determinant = s.x * s.w - s.z * s.y;

        float reciprocal_determinant = (((determinant < (-FLT_EPSILON)) && (determinant > FLT_EPSILON)) ? (1.0F / determinant) : 1.0F);

        // (1.0 / determinant) * [v2 - v0, -(u2 - u0), 0, 0]
        pt_math_alignas16_vec4 inverse_r0(reciprocal_determinant * s.w, reciprocal_determinant * (-s.z), 0.0F, 0.0F);

        // (1.0 / determinant) * [-(v1 - v0), u1 - u0, 0, 0]
        pt_math_alignas16_vec4 inverse_r1(reciprocal_determinant * (-s.y), reciprocal_determinant * s.x, 0.0F, 0.0F);

        pt_math_simd_mat inverse;
        inverse.r[0] = pt_math_load_alignas16_vec4(&inverse_r0);
        inverse.r[1] = pt_math_load_alignas16_vec4(&inverse_r1);
        inverse.r[2] = pt_math_vec_zero();
        inverse.r[3] = pt_math_vec_zero();

        // (p1 - p0)
        // (p2 - p0)
        pt_math_simd_mat p;
        p.r[0] = DirectX::XMVectorSubtract(p1, p0);
        p.r[1] = DirectX::XMVectorSubtract(p2, p0);
        p.r[2] = pt_math_vec_zero();
        p.r[3] = pt_math_vec_zero();

        // t
        // b
        pt_math_simd_mat uv = pt_math_mat_multiply(inverse, p);

        // blend tangents of the same vertex
        pt_math_store_alignas16_vec3(&tmp_tangents[i0], pt_math_vec_add(pt_math_load_alignas16_vec3(&tmp_tangents[i0]), uv.r[0]));
        pt_math_store_alignas16_vec3(&tmp_tangents[i1], pt_math_vec_add(pt_math_load_alignas16_vec3(&tmp_tangents[i1]), uv.r[0]));
        pt_math_store_alignas16_vec3(&tmp_tangents[i2], pt_math_vec_add(pt_math_load_alignas16_vec3(&tmp_tangents[i2]), uv.r[0]));
        pt_math_store_alignas16_vec3(&tmp_bitangents[i0], pt_math_vec_add(pt_math_load_alignas16_vec3(&tmp_bitangents[i0]), uv.r[1]));
        pt_math_store_alignas16_vec3(&tmp_bitangents[i1], pt_math_vec_add(pt_math_load_alignas16_vec3(&tmp_bitangents[i1]), uv.r[1]));
        pt_math_store_alignas16_vec3(&tmp_bitangents[i2], pt_math_vec_add(pt_math_load_alignas16_vec3(&tmp_bitangents[i2]), uv.r[1]));
    }

    for (size_t vertex_index = 0U; vertex_index < vertex_count; ++vertex_index)
    {
        // float4 tangent:
        // [DirectXMesh Wiki / ComputeTangentFrameImpl](https://github.com/microsoft/DirectXMesh/wiki/ComputeTangentFrame)
        // "7.5 Tangent Space" of [Lengyel 2019] [Eric Lengyel. "Foundations of Game Engine Development : Volume 2 - Rendering." Terathon Software LLC 2019.](http://foundationsofgameenginedev.com/)
        // Since the vectors are orthogonal, it is not necessary to store all three of the vectors t, b, and n for each vertex.
        // Just the normal vector and the tangent vector will always suffice, but we do need one additional bit of information.
        // The tangent frame can form either a right-handed or left-handed coordinate system, and which one is given by the sign of determinant of matrix tangent.
        // Calling the sign of this determinant σ, we can reconstitute the bitangent with the cross product b = σ * cross(n, t), and then only the normal and tangent need to be supplied as vertex attributes.

        // float4 quaternion:
        // [Frey 2011] Ivo Frey. "Spherical Skinning withDual-Quaternions and Qtangents." SIGGRAPH 2011.
        // represent the tangent frame with the unit quaternion
        // sign invariant: since the unit quaternion q and -q represent the same rotation transform, we can compress the unit quaternion to three components and use the sign of the w component to denote the reflection (note the +0 and -0 may NOT be distinguished by GPU)

        // float3 tangent + float3 bitangent:
        // Gram-Schmidt
        // 
    }
}
