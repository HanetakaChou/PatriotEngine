/*
 * Copyright (C) YuqiaoZhang(HanetakaYuminaga)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <pt_math.h>
#include "pt_math_base.h"

#include <DirectXMath.h>

#ifdef _XM_AVX2_INTRINSICS_
#error AVX2 supported
#endif

#ifdef _XM_AVX_INTRINSICS_
#error AVX supported
#endif

#ifndef _XM_SSE_INTRINSICS_
#error SSE not supported
#endif

static_assert(sizeof(math_alignas16_mat4x4) == sizeof(DirectX::XMFLOAT4X4A), "");
static_assert(alignof(math_alignas16_mat4x4) == alignof(DirectX::XMFLOAT4X4A), "");
static_assert(sizeof(math_simd_mat) == sizeof(DirectX::XMMATRIX), "");
static_assert(alignof(math_simd_mat) == alignof(DirectX::XMMATRIX), "");

inline DirectX::XMFLOAT4X4A *unwrap(math_alignas16_mat4x4 *alignas16_mat4x4) { return reinterpret_cast<DirectX::XMFLOAT4X4A *>(alignas16_mat4x4); }
inline DirectX::XMMATRIX unwrap(math_simd_mat simd_mat) { return (*reinterpret_cast<DirectX::XMMATRIX *>(&simd_mat)); }
inline math_simd_mat wrap(DirectX::XMMATRIX simd_mat) { return (*reinterpret_cast<math_simd_mat *>(&simd_mat)); }

void PT_VECTORCALL math_sse2_store_alignas16_mat4x4(math_alignas16_mat4x4 *destination, math_simd_mat m)
{
    return DirectX::XMStoreFloat4x4A(unwrap(destination), unwrap(m));
}

math_simd_mat PT_VECTORCALL math_sse2_mat_perspective_fov_rh(float fov_angle_y, float aspect_ratio, float near_z, float far_z)
{
    return wrap(DirectX::XMMatrixPerspectiveFovRH(fov_angle_y, aspect_ratio, near_z, far_z));
}