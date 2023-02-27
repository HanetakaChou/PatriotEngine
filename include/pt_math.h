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

#ifndef _PT_MATH_H_
#define _PT_MATH_H_ 1

#include "pt_math_common.h"
#include <DirectXMath.h>

using pt_math_vec2 = DirectX::XMFLOAT2;

using pt_math_vec3 = DirectX::XMFLOAT3;
using pt_math_vec4 = DirectX::XMFLOAT4;
using pt_math_alignas16_vec3 = DirectX::XMFLOAT3A;
using pt_math_alignas16_vec4 = DirectX::XMFLOAT4A;

using pt_math_mat4x4 = DirectX::XMFLOAT4X4;
using pt_math_alignas16_mat4x4 = DirectX::XMFLOAT4X4A;

using pt_math_simd_vec = DirectX::XMVECTOR;
using pt_math_simd_mat = DirectX::XMMATRIX;

// [Reversed-Z](https://developer.nvidia.com/content/depth-precision-visualized)
#define PT_MATH_Z_FARTHEST 0.0F
#define PT_MATH_Z_NEARER_VK VK_COMPARE_OP_GREATER

inline pt_math_simd_vec PT_VECTORCALL pt_math_load_vec2(pt_math_vec2 const* source)
{
    return DirectX::XMLoadFloat2(source);
}

inline pt_math_simd_vec PT_VECTORCALL pt_math_load_vec3(pt_math_vec3 const* source)
{
	return DirectX::XMLoadFloat3(source);
}

inline pt_math_simd_vec PT_VECTORCALL pt_math_load_alignas16_vec3(pt_math_alignas16_vec3 const* source)
{
    return DirectX::XMLoadFloat3A(source);
}

inline pt_math_simd_vec PT_VECTORCALL pt_math_load_alignas16_vec4(pt_math_alignas16_vec4 const* source)
{
    return DirectX::XMLoadFloat4A(source);
}

inline void PT_VECTORCALL pt_math_store_vec3(pt_math_vec3* destination, pt_math_simd_vec v)
{
    return DirectX::XMStoreFloat3(destination, v);
}

inline void PT_VECTORCALL pt_math_store_vec4(pt_math_vec4* destination, pt_math_simd_vec v)
{
    return DirectX::XMStoreFloat4(destination, v);
}

inline void PT_VECTORCALL pt_math_store_alignas16_vec3(pt_math_alignas16_vec3* destination, pt_math_simd_vec v)
{
    return DirectX::XMStoreFloat3A(destination, v);
}

inline void PT_VECTORCALL pt_math_store_alignas16_vec4(pt_math_alignas16_vec4 *destination, pt_math_simd_vec v)
{
    return DirectX::XMStoreFloat4A(destination, v);
}

inline pt_math_simd_mat PT_VECTORCALL pt_math_load_alignas16_mat4x4(pt_math_alignas16_mat4x4 const* source)
{
    return DirectX::XMLoadFloat4x4A(source);
}

inline void PT_VECTORCALL pt_math_store_alignas16_mat4x4(pt_math_alignas16_mat4x4 *destination, pt_math_simd_mat m)
{
	return DirectX::XMStoreFloat4x4A(destination, m);
}

inline pt_math_simd_vec PT_VECTORCALL pt_math_vec_zero()
{
    return DirectX::XMVectorZero();
}

inline float PT_VECTORCALL pt_math_vec_get_x(pt_math_simd_vec v)
{
    return DirectX::XMVectorGetX(v);
}

inline float PT_VECTORCALL pt_math_vec_get_y(pt_math_simd_vec v)
{
    return DirectX::XMVectorGetY(v);
}

inline float PT_VECTORCALL pt_math_vec_get_z(pt_math_simd_vec v)
{
    return DirectX::XMVectorGetZ(v);
}

inline float PT_VECTORCALL pt_math_vec_get_w(pt_math_simd_vec v)
{
    return DirectX::XMVectorGetW(v);
}

inline pt_math_simd_vec PT_VECTORCALL pt_math_vec3_length(pt_math_simd_vec v)
{
    return DirectX::XMVector3Length(v);
}

inline pt_math_simd_vec PT_VECTORCALL pt_math_vec3_normalize(pt_math_simd_vec v)
{
    return DirectX::XMVector3Normalize(v);
}

inline pt_math_simd_vec PT_VECTORCALL pt_math_vec4_normalize(pt_math_simd_vec v)
{
    return DirectX::XMVector4Normalize(v);
}

inline pt_math_simd_vec PT_VECTORCALL pt_math_vec_scale(pt_math_simd_vec v, float s)
{
    return DirectX::XMVectorScale(v, s);
}

inline pt_math_simd_vec PT_VECTORCALL pt_math_vec_add(pt_math_simd_vec v1, pt_math_simd_vec v2)
{
    return DirectX::XMVectorAdd(v1, v2);
}

inline pt_math_simd_vec PT_VECTORCALL pt_math_vec_subtract(pt_math_simd_vec v1, pt_math_simd_vec v2)
{
    return DirectX::XMVectorSubtract(v1, v2);
}

inline pt_math_simd_vec PT_VECTORCALL pt_math_vec3_dot(pt_math_simd_vec v1, pt_math_simd_vec v2)
{
    return DirectX::XMVector3Dot(v1, v2);
}

inline pt_math_simd_vec PT_VECTORCALL pt_math_vec3_cross(pt_math_simd_vec v1, pt_math_simd_vec v2)
{
    return DirectX::XMVector3Cross(v1, v2);
}

inline pt_math_simd_vec PT_VECTORCALL pt_math_vec_merge_xy(pt_math_simd_vec v1, pt_math_simd_vec v2)
{
    return DirectX::XMVectorMergeXY(v1, v2);
}

inline pt_math_simd_vec PT_VECTORCALL pt_math_quat_rotation_mat(pt_math_simd_mat m)
{
    return DirectX::XMQuaternionRotationMatrix(m);
}

inline bool PT_VECTORCALL pt_math_mat_decompose(pt_math_simd_vec *out_scale, pt_math_simd_vec *out_rot_quat, pt_math_simd_vec *out_trans, pt_math_simd_mat m)
{
    return DirectX::XMMatrixDecompose(out_scale, out_rot_quat, out_trans, m);
}

inline pt_math_simd_mat PT_VECTORCALL pt_math_mat_identity()
{
    return DirectX::XMMatrixIdentity();
}

inline pt_math_simd_mat PT_VECTORCALL pt_math_mat_look_to_rh(pt_math_simd_vec eye_position, pt_math_simd_vec eye_direction, pt_math_simd_vec up_direction)
{
    return DirectX::XMMatrixLookToRH(eye_position, eye_direction, up_direction);
}

#if defined(PT_X64) || defined(PT_X86)
inline pt_math_simd_mat PT_VECTORCALL pt_math_mat_perspective_fov_rh(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
{
    // [Reversed-Z](https://developer.nvidia.com/content/depth-precision-visualized)
    //
    // _  0  0  0
    // 0  _  0  0
    // 0  0  b -1
    // 0  0  a  0
    //
    // _  0  0  0
    // 0  _  0  0
    // 0  0 zb  -z
    // 0  0  a
    //
    // z' = -b - a/z
    //
    // Standard
    // 0 = -b + a/nearz // z=-nearz
    // 1 = -b + a/farz  // z=-farz
    // a = farz*nearz/(nearz - farz)
    // b = farz/(nearz - farz)
    //
    // Reversed-Z
    // 1 = -b + a/nearz // z=-nearz
    // 0 = -b + a/farz  // z=-farz
    // a = farz*nearz/(farz - nearz)
    // b = nearz/(farz - nearz)

    // __m128 _mm_shuffle_ps(__m128 lo,__m128 hi, _MM_SHUFFLE(hi3,hi2,lo1,lo0))
    // Interleave inputs into low 2 floats and high 2 floats of output. Basically
    // out[0]=lo[lo0];
    // out[1]=lo[lo1];
    // out[2]=hi[hi2];
    // out[3]=hi[hi3];

    // DirectX::XMMatrixPerspectiveFovRH
    float SinFov;
    float CosFov;
    DirectX::XMScalarSinCos(&SinFov, &CosFov, 0.5f * FovAngleY);
    // Note: This is recorded on the stack
    float Height = CosFov / SinFov;
    DirectX::XMVECTOR rMem = {
        Height / AspectRatio,
        Height,
        NearZ / (FarZ - NearZ),
        (FarZ / (FarZ - NearZ)) * NearZ };
    // Copy from memory to SSE register
    DirectX::XMVECTOR vValues = rMem;
    DirectX::XMVECTOR vTemp = _mm_setzero_ps();
    // Copy x only
    vTemp = _mm_move_ss(vTemp, vValues);
    // CosFov / SinFov,0,0,0
    DirectX::XMMATRIX M;
    M.r[0] = vTemp;
    // 0,Height / AspectRatio,0,0
    vTemp = vValues;
    vTemp = _mm_and_ps(vTemp, DirectX::g_XMMaskY);
    M.r[1] = vTemp;
    // x=b,y=a,0,-1.0f
    vTemp = _mm_setzero_ps();
    vValues = _mm_shuffle_ps(vValues, DirectX::g_XMNegIdentityR3, _MM_SHUFFLE(3, 2, 3, 2));
    // 0,0,b,-1.0f
    vTemp = _mm_shuffle_ps(vTemp, vValues, _MM_SHUFFLE(3, 0, 0, 0));
    M.r[2] = vTemp;
    // 0,0,a,0.0f
    vTemp = _mm_shuffle_ps(vTemp, vValues, _MM_SHUFFLE(2, 1, 0, 0));
    M.r[3] = vTemp;
    return M;
}
#elif defined(PT_ARM64) || defined(PT_ARM)
inline pt_math_simd_mat PT_VECTORCALL pt_math_mat_perspective_fov_rh(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
{
    // [Reversed-Z](https://developer.nvidia.com/content/depth-precision-visualized)
    //
    // _  0  0  0
    // 0  _  0  0
    // 0  0  b -1
    // 0  0  a  0
    //
    // _  0  0  0
    // 0  _  0  0
    // 0  0 zb  -z
    // 0  0  a
    //
    // z' = -b - a/z
    //
    // Standard
    // 0 = -b + a/nearz // z=-nearz
    // 1 = -b + a/farz  // z=-farz
    // a = farz*nearz/(nearz - farz)
    // b = farz/(nearz - farz)
    //
    // Reversed-Z
    // 1 = -b + a/nearz // z=-nearz
    // 0 = -b + a/farz  // z=-farz
    // a = farz*nearz/(farz - nearz)
    // b = nearz/(farz - nearz)

    // DirectX::XMMatrixPerspectiveFovRH
    float SinFov;
    float CosFov;
    DirectX::XMScalarSinCos(&SinFov, &CosFov, 0.5f * FovAngleY);
    float Height = CosFov / SinFov;
    float Width = Height / AspectRatio;
    const DirectX::XMVECTOR Zero = vdupq_n_f32(0);

    DirectX::XMMATRIX M;
    M.r[0] = vsetq_lane_f32(Width, Zero, 0);
    M.r[1] = vsetq_lane_f32(Height, Zero, 1);
    M.r[2] = vsetq_lane_f32(NearZ / (FarZ - NearZ), DirectX::g_XMNegIdentityR3.v, 2);
    M.r[3] = vsetq_lane_f32((FarZ / (FarZ - NearZ)) * NearZ, Zero, 2);
    return M;
}
#else
#error Unknown Architecture
#endif

inline pt_math_simd_mat PT_VECTORCALL pt_math_mat_multiply(pt_math_simd_mat m1, pt_math_simd_mat m2)
{
    return DirectX::XMMatrixMultiply(m1, m2);
}

#endif
