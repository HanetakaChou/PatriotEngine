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
#include "pt_math_directx_math.h"

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

#ifdef _XM_AVX2_INTRINSICS_
#error AVX2 supported
#endif

#ifndef _XM_AVX_INTRINSICS_
#error AVX not supported
#endif

#include <DirectXMath.h>

static_assert(sizeof(math_vec3) == sizeof(DirectX::XMFLOAT3), "");
static_assert(alignof(math_vec3) == alignof(DirectX::XMFLOAT3), "");
static_assert(sizeof(math_vec4) == sizeof(DirectX::XMFLOAT4), "");
static_assert(alignof(math_vec4) == alignof(DirectX::XMFLOAT4), "");
static_assert(sizeof(math_simd_vec) == sizeof(DirectX::XMVECTOR), "");
static_assert(alignof(math_simd_vec) == alignof(DirectX::XMVECTOR), "");
static_assert(sizeof(math_mat4x4) == sizeof(DirectX::XMFLOAT4X4), "");
static_assert(alignof(math_mat4x4) == alignof(DirectX::XMFLOAT4X4), "");
static_assert(sizeof(math_alignas16_mat4x4) == sizeof(DirectX::XMFLOAT4X4A), "");
static_assert(alignof(math_alignas16_mat4x4) == alignof(DirectX::XMFLOAT4X4A), "");
static_assert(sizeof(math_simd_mat) == sizeof(DirectX::XMMATRIX), "");
static_assert(alignof(math_simd_mat) == alignof(DirectX::XMMATRIX), "");

inline DirectX::XMFLOAT3 *unwrap(math_vec3 *vec3) { return reinterpret_cast<DirectX::XMFLOAT3 *>(vec3); }
inline DirectX::XMFLOAT4 *unwrap(math_vec4 *vec4) { return reinterpret_cast<DirectX::XMFLOAT4 *>(vec4); }
inline DirectX::XMVECTOR unwrap(math_simd_vec simd_vec) { return (*reinterpret_cast<DirectX::XMVECTOR *>(&simd_vec)); }
inline math_simd_vec wrap(DirectX::XMVECTOR simd_vec) { return (*reinterpret_cast<math_simd_vec *>(&simd_vec)); }
inline DirectX::XMFLOAT4X4 *unwrap(math_mat4x4 *mat4x4) { return reinterpret_cast<DirectX::XMFLOAT4X4 *>(mat4x4); }
inline DirectX::XMFLOAT4X4A *unwrap(math_alignas16_mat4x4 *alignas16_mat4x4) { return reinterpret_cast<DirectX::XMFLOAT4X4A *>(alignas16_mat4x4); }
inline DirectX::XMMATRIX unwrap(math_simd_mat simd_mat) { return (*reinterpret_cast<DirectX::XMMATRIX *>(&simd_mat)); }
inline math_simd_mat wrap(DirectX::XMMATRIX simd_mat) { return (*reinterpret_cast<math_simd_mat *>(&simd_mat)); }

math_simd_vec PT_VECTORCALL directx_math_x86_avx_load_vec3(math_vec3 *source)
{
    return wrap(DirectX::XMLoadFloat3(unwrap(source)));
}

math_simd_vec PT_VECTORCALL directx_math_x86_avx_load_vec4(math_vec4 *source)
{
    return wrap(DirectX::XMLoadFloat4(unwrap(source)));
}

void PT_VECTORCALL directx_math_x86_avx_store_vec3(math_vec3 *source, math_simd_vec v)
{
    return DirectX::XMStoreFloat3(unwrap(source), unwrap(v));
}

void PT_VECTORCALL directx_math_x86_avx_store_vec4(math_vec4 *source, math_simd_vec v)
{
    return DirectX::XMStoreFloat4(unwrap(source), unwrap(v));
}

math_simd_mat PT_VECTORCALL directx_math_x86_avx_load_alignas16_mat4x4(math_alignas16_mat4x4 *source)
{
    return wrap(DirectX::XMLoadFloat4x4A(unwrap(source)));
}

math_simd_mat PT_VECTORCALL directx_math_x86_avx_load_mat4x4(math_mat4x4 *source)
{
    return wrap(DirectX::XMLoadFloat4x4(unwrap(source)));
}

math_simd_mat PT_VECTORCALL directx_math_x86_avx_mat_identity()
{
    return wrap(DirectX::XMMatrixIdentity());
}

math_simd_mat PT_VECTORCALL directx_math_x86_avx_mat_multiply(math_simd_mat m1, math_simd_mat m2)
{
    return wrap(DirectX::XMMatrixMultiply(unwrap(m1), unwrap(m2)));
}

math_simd_mat PT_VECTORCALL directx_math_x86_avx_mat_look_to_rh(math_simd_vec eye_position, math_simd_vec eye_direction, math_simd_vec up_direction)
{
    return wrap(DirectX::XMMatrixLookToRH(unwrap(eye_position), unwrap(eye_direction), unwrap(up_direction)));
}

math_simd_mat PT_VECTORCALL directx_math_x86_avx_mat_perspective_fov_rh(float fov_angle_y, float aspect_ratio, float near_z, float far_z)
{
    return wrap(DirectX::XMMatrixPerspectiveFovRH(fov_angle_y, aspect_ratio, near_z, far_z));
}

void PT_VECTORCALL directx_math_x86_avx_store_alignas16_mat4x4(math_alignas16_mat4x4 *destination, math_simd_mat m)
{
    return DirectX::XMStoreFloat4x4A(unwrap(destination), unwrap(m));
}

void PT_VECTORCALL directx_math_x86_avx_store_mat4x4(math_mat4x4 *destination, math_simd_mat m)
{
    return DirectX::XMStoreFloat4x4(unwrap(destination), unwrap(m));
}
