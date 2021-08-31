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

pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_load_vec3(pt_math_vec3 const *source);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_load_vec3(pt_math_vec3 const *source);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_load_vec3(pt_math_vec3 const *source);
pt_math_simd_vec PT_VECTORCALL directx_math_arm_neon_load_vec3(pt_math_vec3 const *source);

pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_load_vec4(pt_math_vec4 const *source);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_load_vec4(pt_math_vec4 const *source);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_load_vec4(pt_math_vec4 const *source);
pt_math_simd_vec PT_VECTORCALL directx_math_arm_neon_load_vec4(pt_math_vec4 const *source);

void PT_VECTORCALL directx_math_x86_avx2_store_vec3(pt_math_vec3 *source, pt_math_simd_vec v);
void PT_VECTORCALL directx_math_x86_avx_store_vec3(pt_math_vec3 *source, pt_math_simd_vec v);
void PT_VECTORCALL directx_math_x86_sse2_store_vec3(pt_math_vec3 *source, pt_math_simd_vec v);
void PT_VECTORCALL directx_math_arm_neon_store_vec3(pt_math_vec3 *source, pt_math_simd_vec v);

void PT_VECTORCALL directx_math_x86_avx2_store_vec4(pt_math_vec4 *source, pt_math_simd_vec v);
void PT_VECTORCALL directx_math_x86_avx_store_vec4(pt_math_vec4 *source, pt_math_simd_vec v);
void PT_VECTORCALL directx_math_x86_sse2_store_vec4(pt_math_vec4 *source, pt_math_simd_vec v);
void PT_VECTORCALL directx_math_arm_neon_store_vec4(pt_math_vec4 *source, pt_math_simd_vec v);

pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx2_load_alignas16_mat4x4(pt_math_alignas16_mat4x4 const *source);
pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx_load_alignas16_mat4x4(pt_math_alignas16_mat4x4 const *source);
pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_load_alignas16_mat4x4(pt_math_alignas16_mat4x4 const *source);
pt_math_simd_mat PT_VECTORCALL directx_math_arm_neon_load_alignas16_mat4x4(pt_math_alignas16_mat4x4 const *source);

pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx2_load_mat4x4(pt_math_mat4x4 const *source);
pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx_load_mat4x4(pt_math_mat4x4 const *source);
pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_load_mat4x4(pt_math_mat4x4 const *source);
pt_math_simd_mat PT_VECTORCALL directx_math_arm_neon_load_mat4x4(pt_math_mat4x4 const *source);

pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx2_mat_identity();
pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx_mat_identity();
pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_identity();
pt_math_simd_mat PT_VECTORCALL directx_math_arm_neon_mat_identity();

pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx2_mat_multiply(pt_math_simd_mat m1, pt_math_simd_mat m2);
pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx_mat_multiply(pt_math_simd_mat m1, pt_math_simd_mat m2);
pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_multiply(pt_math_simd_mat m1, pt_math_simd_mat m2);
pt_math_simd_mat PT_VECTORCALL directx_math_arm_neon_mat_multiply(pt_math_simd_mat m1, pt_math_simd_mat m2);

pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx2_mat_look_to_rh(pt_math_simd_vec eye_position, pt_math_simd_vec eye_direction, pt_math_simd_vec up_direction);
pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx_mat_look_to_rh(pt_math_simd_vec eye_position, pt_math_simd_vec eye_direction, pt_math_simd_vec up_direction);
pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_look_to_rh(pt_math_simd_vec eye_position, pt_math_simd_vec eye_direction, pt_math_simd_vec up_direction);
pt_math_simd_mat PT_VECTORCALL directx_math_arm_neon_mat_look_to_rh(pt_math_simd_vec eye_position, pt_math_simd_vec eye_direction, pt_math_simd_vec up_direction);

pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx2_mat_perspective_fov_rh(float fov_angle_y, float aspect_ratio, float near_z, float far_z);
pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx_mat_perspective_fov_rh(float fov_angle_y, float aspect_ratio, float near_z, float far_z);
pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_perspective_fov_rh(float fov_angle_y, float aspect_ratio, float near_z, float far_z);
pt_math_simd_mat PT_VECTORCALL directx_math_arm_neon_mat_perspective_fov_rh(float fov_angle_y, float aspect_ratio, float near_z, float far_z);

void PT_VECTORCALL directx_math_x86_avx2_store_alignas16_mat4x4(pt_math_alignas16_mat4x4 *destination, pt_math_simd_mat m);
void PT_VECTORCALL directx_math_x86_avx_store_alignas16_mat4x4(pt_math_alignas16_mat4x4 *destination, pt_math_simd_mat m);
void PT_VECTORCALL directx_math_x86_sse2_store_alignas16_mat4x4(pt_math_alignas16_mat4x4 *destination, pt_math_simd_mat m);
void PT_VECTORCALL directx_math_arm_neon_store_alignas16_mat4x4(pt_math_alignas16_mat4x4 *destination, pt_math_simd_mat m);

void PT_VECTORCALL directx_math_x86_avx2_store_mat4x4(pt_math_mat4x4 *destination, pt_math_simd_mat m);
void PT_VECTORCALL directx_math_x86_avx_store_mat4x4(pt_math_mat4x4 *destination, pt_math_simd_mat m);
void PT_VECTORCALL directx_math_x86_sse2_store_mat4x4(pt_math_mat4x4 *destination, pt_math_simd_mat m);
void PT_VECTORCALL directx_math_arm_neon_store_mat4x4(pt_math_mat4x4 *destination, pt_math_simd_mat m);

#endif