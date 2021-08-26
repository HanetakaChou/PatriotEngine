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

#ifndef _PT_MATH_BASE_H_
#define _PT_MATH_BASE_H_ 1

math_simd_vec PT_VECTORCALL directx_math_x86_avx2_load_vec3(math_vec3 *source);
math_simd_vec PT_VECTORCALL directx_math_x86_avx_load_vec3(math_vec3 *source);
math_simd_vec PT_VECTORCALL directx_math_x86_sse2_load_vec3(math_vec3 *source);
extern "C" math_simd_vec PT_VECTORCALL directx_math_arm_neon_load_vec3(math_vec3 *source);

math_simd_vec PT_VECTORCALL directx_math_x86_avx2_load_vec4(math_vec4 *source);
math_simd_vec PT_VECTORCALL directx_math_x86_avx_load_vec4(math_vec4 *source);
math_simd_vec PT_VECTORCALL directx_math_x86_sse2_load_vec4(math_vec4 *source);
extern "C" math_simd_vec PT_VECTORCALL directx_math_arm_neon_load_vec4(math_vec4 *source);

void PT_VECTORCALL directx_math_x86_avx2_store_vec3(math_vec3 *source, math_simd_vec v);
void PT_VECTORCALL directx_math_x86_avx_store_vec3(math_vec3 *source, math_simd_vec v);
void PT_VECTORCALL directx_math_x86_sse2_store_vec3(math_vec3 *source, math_simd_vec v);
extern "C" void PT_VECTORCALL directx_math_arm_neon_store_vec3(math_vec3 *source, math_simd_vec v);

void PT_VECTORCALL directx_math_x86_avx2_store_vec4(math_vec4 *source, math_simd_vec v);
void PT_VECTORCALL directx_math_x86_avx_store_vec4(math_vec4 *source, math_simd_vec v);
void PT_VECTORCALL directx_math_x86_sse2_store_vec4(math_vec4 *source, math_simd_vec v);
extern "C" void PT_VECTORCALL directx_math_arm_neon_store_vec4(math_vec4 *source, math_simd_vec v);

math_simd_mat PT_VECTORCALL directx_math_x86_avx2_load_alignas16_mat4x4(math_alignas16_mat4x4 *source);
math_simd_mat PT_VECTORCALL directx_math_x86_avx_load_alignas16_mat4x4(math_alignas16_mat4x4 *source);
math_simd_mat PT_VECTORCALL directx_math_x86_sse2_load_alignas16_mat4x4(math_alignas16_mat4x4 *source);
extern "C" math_simd_mat PT_VECTORCALL directx_math_arm_neon_load_alignas16_mat4x4(math_alignas16_mat4x4 *source);

math_simd_mat PT_VECTORCALL directx_math_x86_avx2_load_mat4x4(math_mat4x4 *source);
math_simd_mat PT_VECTORCALL directx_math_x86_avx_load_mat4x4(math_mat4x4 *source);
math_simd_mat PT_VECTORCALL directx_math_x86_sse2_load_mat4x4(math_mat4x4 *source);
extern "C" math_simd_mat PT_VECTORCALL directx_math_arm_neon_load_mat4x4(math_mat4x4 *source);

math_simd_mat PT_VECTORCALL directx_math_x86_avx2_mat_identity();
math_simd_mat PT_VECTORCALL directx_math_x86_avx_mat_identity();
math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_identity();
extern "C" math_simd_mat PT_VECTORCALL directx_math_arm_neon_mat_identity();

math_simd_mat PT_VECTORCALL directx_math_x86_avx2_mat_multiply(math_simd_mat m1, math_simd_mat m2);
math_simd_mat PT_VECTORCALL directx_math_x86_avx_mat_multiply(math_simd_mat m1, math_simd_mat m2);
math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_multiply(math_simd_mat m1, math_simd_mat m2);
extern "C" math_simd_mat PT_VECTORCALL directx_math_arm_neon_mat_multiply(math_simd_mat m1, math_simd_mat m2);

math_simd_mat PT_VECTORCALL directx_math_x86_avx2_mat_look_to_rh(math_simd_vec eye_position, math_simd_vec eye_direction, math_simd_vec up_direction);
math_simd_mat PT_VECTORCALL directx_math_x86_avx_mat_look_to_rh(math_simd_vec eye_position, math_simd_vec eye_direction, math_simd_vec up_direction);
math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_look_to_rh(math_simd_vec eye_position, math_simd_vec eye_direction, math_simd_vec up_direction);
extern "C" math_simd_mat PT_VECTORCALL directx_math_arm_neon_mat_look_to_rh(math_simd_vec eye_position, math_simd_vec eye_direction, math_simd_vec up_direction);

math_simd_mat PT_VECTORCALL directx_math_x86_avx2_mat_perspective_fov_rh(float fov_angle_y, float aspect_ratio, float near_z, float far_z);
math_simd_mat PT_VECTORCALL directx_math_x86_avx_mat_perspective_fov_rh(float fov_angle_y, float aspect_ratio, float near_z, float far_z);
math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_perspective_fov_rh(float fov_angle_y, float aspect_ratio, float near_z, float far_z);
extern "C" math_simd_mat PT_VECTORCALL directx_math_arm_neon_mat_perspective_fov_rh(float fov_angle_y, float aspect_ratio, float near_z, float far_z);

void PT_VECTORCALL directx_math_x86_avx2_store_alignas16_mat4x4(math_alignas16_mat4x4 *destination, math_simd_mat m);
void PT_VECTORCALL directx_math_x86_avx_store_alignas16_mat4x4(math_alignas16_mat4x4 *destination, math_simd_mat m);
void PT_VECTORCALL directx_math_x86_sse2_store_alignas16_mat4x4(math_alignas16_mat4x4 *destination, math_simd_mat m);
extern "C" void PT_VECTORCALL directx_math_arm_neon_store_alignas16_mat4x4(math_alignas16_mat4x4 *destination, math_simd_mat m);

void PT_VECTORCALL directx_math_x86_avx2_store_mat4x4(math_mat4x4 *destination, math_simd_mat m);
void PT_VECTORCALL directx_math_x86_avx_store_mat4x4(math_mat4x4 *destination, math_simd_mat m);
void PT_VECTORCALL directx_math_x86_sse2_store_mat4x4(math_mat4x4 *destination, math_simd_mat m);
extern "C" void PT_VECTORCALL directx_math_arm_neon_store_mat4x4(math_mat4x4 *destination, math_simd_mat m);

#endif