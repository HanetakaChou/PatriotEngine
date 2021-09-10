//
// Copyright (C) YuqiaoZhang(HanetakaYuminaga)
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

#include <pt_math.h>

#if defined(PT_ARM) || defined(PT_ARM64)

#if !defined(_MSC_VER)
//#include "sal.h"
#define _In_
#define _In_reads_(expression)
#define _In_reads_bytes_(expression)
#define _Out_
#define _Out_opt_
#define _Out_writes_(expression)
#define _Out_writes_bytes_(expression)
#define _Success_(expression)
#define _Use_decl_annotations_
#define _Analysis_assume_(expression)
#endif

//https://github.com/microsoft/DirectXMath/tree/83634c742a85d1027765af53fbe79506fd72e0c3
#include <DirectXMath.h>

#ifndef _XM_ARM_NEON_INTRINSICS_
#error NEON not supported
#endif

static_assert(sizeof(pt_math_vec3) == sizeof(DirectX::XMFLOAT3), "");
static_assert(alignof(pt_math_vec3) == alignof(DirectX::XMFLOAT3), "");
static_assert(sizeof(pt_math_vec4) == sizeof(DirectX::XMFLOAT4), "");
static_assert(alignof(pt_math_vec4) == alignof(DirectX::XMFLOAT4), "");
static_assert(sizeof(pt_math_simd_vec) == sizeof(DirectX::XMVECTOR), "");
static_assert(alignof(pt_math_simd_vec) == alignof(DirectX::XMVECTOR), "");
static_assert(sizeof(pt_math_mat4x4) == sizeof(DirectX::XMFLOAT4X4), "");
static_assert(alignof(pt_math_mat4x4) == alignof(DirectX::XMFLOAT4X4), "");
static_assert(sizeof(pt_math_alignas16_mat4x4) == sizeof(DirectX::XMFLOAT4X4A), "");
static_assert(alignof(pt_math_alignas16_mat4x4) == alignof(DirectX::XMFLOAT4X4A), "");
static_assert(sizeof(pt_math_simd_mat) == sizeof(DirectX::XMMATRIX), "");
static_assert(alignof(pt_math_simd_mat) == alignof(DirectX::XMMATRIX), "");

inline DirectX::XMFLOAT3 *unwrap(pt_math_vec3 *vec3) { return reinterpret_cast<DirectX::XMFLOAT3 *>(vec3); }
inline DirectX::XMFLOAT3 const *unwrap(pt_math_vec3 const *vec3) { return reinterpret_cast<DirectX::XMFLOAT3 const *>(vec3); }
inline DirectX::XMFLOAT4 *unwrap(pt_math_vec4 *vec4) { return reinterpret_cast<DirectX::XMFLOAT4 *>(vec4); }
inline DirectX::XMFLOAT4 const *unwrap(pt_math_vec4 const *vec4) { return reinterpret_cast<DirectX::XMFLOAT4 const *>(vec4); }
inline DirectX::XMVECTOR unwrap(pt_math_simd_vec simd_vec) { return (*reinterpret_cast<DirectX::XMVECTOR *>(&simd_vec)); }
inline pt_math_simd_vec wrap(DirectX::XMVECTOR simd_vec) { return (*reinterpret_cast<pt_math_simd_vec *>(&simd_vec)); }
inline DirectX::XMFLOAT4X4 *unwrap(pt_math_mat4x4 *mat4x4) { return reinterpret_cast<DirectX::XMFLOAT4X4 *>(mat4x4); }
inline DirectX::XMFLOAT4X4 const *unwrap(pt_math_mat4x4 const *mat4x4) { return reinterpret_cast<DirectX::XMFLOAT4X4 const *>(mat4x4); }
inline DirectX::XMFLOAT4X4A *unwrap(pt_math_alignas16_mat4x4 *alignas16_mat4x4) { return reinterpret_cast<DirectX::XMFLOAT4X4A *>(alignas16_mat4x4); }
inline DirectX::XMFLOAT4X4A const *unwrap(pt_math_alignas16_mat4x4 const *alignas16_mat4x4) { return reinterpret_cast<DirectX::XMFLOAT4X4A const *>(alignas16_mat4x4); }
inline DirectX::XMMATRIX unwrap(pt_math_simd_mat simd_mat) { return (*reinterpret_cast<DirectX::XMMATRIX *>(&simd_mat)); }
inline pt_math_simd_mat wrap(DirectX::XMMATRIX simd_mat) { return (*reinterpret_cast<pt_math_simd_mat *>(&simd_mat)); }

pt_math_simd_vec PT_VECTORCALL directx_math_arm_neon_load_vec3(pt_math_vec3 const *source)
{
    return wrap(DirectX::XMLoadFloat3(unwrap(source)));
}

pt_math_simd_vec PT_VECTORCALL directx_math_arm_neon_load_vec4(pt_math_vec4 const *source)
{
    return wrap(DirectX::XMLoadFloat4(unwrap(source)));
}

void PT_VECTORCALL directx_math_arm_neon_store_vec3(pt_math_vec3 *source, pt_math_simd_vec v)
{
    return DirectX::XMStoreFloat3(unwrap(source), unwrap(v));
}

void PT_VECTORCALL directx_math_arm_neon_store_vec4(pt_math_vec4 *source, pt_math_simd_vec v)
{
    return DirectX::XMStoreFloat4(unwrap(source), unwrap(v));
}

pt_math_simd_mat PT_VECTORCALL directx_math_arm_neon_load_alignas16_mat4x4(pt_math_alignas16_mat4x4 const *source)
{
    return wrap(DirectX::XMLoadFloat4x4A(unwrap(source)));
}

pt_math_simd_mat PT_VECTORCALL directx_math_arm_neon_load_mat4x4(pt_math_mat4x4 const *source)
{
    return wrap(DirectX::XMLoadFloat4x4(unwrap(source)));
}

pt_math_simd_mat PT_VECTORCALL directx_math_arm_neon_mat_identity()
{
    return wrap(DirectX::XMMatrixIdentity());
}

pt_math_simd_mat PT_VECTORCALL directx_math_arm_neon_mat_multiply(pt_math_simd_mat m1, pt_math_simd_mat m2)
{
    return wrap(DirectX::XMMatrixMultiply(unwrap(m1), unwrap(m2)));
}

pt_math_simd_mat PT_VECTORCALL directx_math_arm_neon_mat_look_to_rh(pt_math_simd_vec eye_position, pt_math_simd_vec eye_direction, pt_math_simd_vec up_direction)
{
    return wrap(DirectX::XMMatrixLookToRH(unwrap(eye_position), unwrap(eye_direction), unwrap(up_direction)));
}

pt_math_simd_mat PT_VECTORCALL directx_math_arm_neon_mat_perspective_fov_rh(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
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
    return wrap(M);
}

void PT_VECTORCALL directx_math_arm_neon_store_alignas16_mat4x4(pt_math_alignas16_mat4x4 *destination, pt_math_simd_mat m)
{
    return DirectX::XMStoreFloat4x4A(unwrap(destination), unwrap(m));
}

void PT_VECTORCALL directx_math_arm_neon_store_mat4x4(pt_math_mat4x4 *destination, pt_math_simd_mat m)
{
    return DirectX::XMStoreFloat4x4(unwrap(destination), unwrap(m));
}

#endif
