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

#ifndef _PT_MATH_DIRECTX_MATH_H_
#define _PT_MATH_DIRECTX_MATH_H_ 1

pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_load_vec3(pt_math_vec3 const *source);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_load_vec3(pt_math_vec3 const *source);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_load_vec3(pt_math_vec3 const *source);

pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_load_alignas16_vec3(pt_math_alignas16_vec3 const *source);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_load_alignas16_vec3(pt_math_alignas16_vec3 const *source);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_load_alignas16_vec3(pt_math_alignas16_vec3 const *source);

pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_load_vec4(pt_math_vec4 const *source);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_load_vec4(pt_math_vec4 const *source);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_load_vec4(pt_math_vec4 const *source);

pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_load_alignas16_vec4(pt_math_alignas16_vec4 const *source);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_load_alignas16_vec4(pt_math_alignas16_vec4 const *source);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_load_alignas16_vec4(pt_math_alignas16_vec4 const *source);

pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_load_alignas16_ivec3(pt_math_alignas16_ivec3 const* source);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_load_alignas16_ivec3(pt_math_alignas16_ivec3 const* source);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_load_alignas16_ivec3(pt_math_alignas16_ivec3 const* source);

pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec_zero();
pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec_zero();
pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_zero();

pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec_replicate(float value);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec_replicate(float value);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_replicate(float value);

pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec3_transform(pt_math_simd_vec v, pt_math_simd_mat m);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec3_transform(pt_math_simd_vec v, pt_math_simd_mat m);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec3_transform(pt_math_simd_vec v, pt_math_simd_mat m);

pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec3_transform_coord(pt_math_simd_vec v, pt_math_simd_mat m);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec3_transform_coord(pt_math_simd_vec v, pt_math_simd_mat m);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec3_transform_coord(pt_math_simd_vec v, pt_math_simd_mat m);

pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec3_normalize(pt_math_simd_vec v);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec3_normalize(pt_math_simd_vec v);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec3_normalize(pt_math_simd_vec v);

pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec3_dot(pt_math_simd_vec v1, pt_math_simd_vec v2);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec3_dot(pt_math_simd_vec v1, pt_math_simd_vec v2);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec3_dot(pt_math_simd_vec v1, pt_math_simd_vec v2);

pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec4_dot(pt_math_simd_vec v1, pt_math_simd_vec v2);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec4_dot(pt_math_simd_vec v1, pt_math_simd_vec v2);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec4_dot(pt_math_simd_vec v1, pt_math_simd_vec v2);

float PT_VECTORCALL directx_math_x86_avx2_vec_get_x(pt_math_simd_vec v);
float PT_VECTORCALL directx_math_x86_avx_vec_get_x(pt_math_simd_vec v);
float PT_VECTORCALL directx_math_x86_sse2_vec_get_x(pt_math_simd_vec v);

float PT_VECTORCALL directx_math_x86_avx2_vec_get_y(pt_math_simd_vec v);
float PT_VECTORCALL directx_math_x86_avx_vec_get_y(pt_math_simd_vec v);
float PT_VECTORCALL directx_math_x86_sse2_vec_get_y(pt_math_simd_vec v);

float PT_VECTORCALL directx_math_x86_avx2_vec_get_z(pt_math_simd_vec v);
float PT_VECTORCALL directx_math_x86_avx_vec_get_z(pt_math_simd_vec v);
float PT_VECTORCALL directx_math_x86_sse2_vec_get_z(pt_math_simd_vec v);

float PT_VECTORCALL directx_math_x86_avx2_vec_get_w(pt_math_simd_vec v);
float PT_VECTORCALL directx_math_x86_avx_vec_get_w(pt_math_simd_vec v);
float PT_VECTORCALL directx_math_x86_sse2_vec_get_w(pt_math_simd_vec v);

pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec_abs(pt_math_simd_vec v);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec_abs(pt_math_simd_vec v);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_abs(pt_math_simd_vec v);

pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec_splat_w(pt_math_simd_vec v);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec_splat_w(pt_math_simd_vec v);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_splat_w(pt_math_simd_vec v);

pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec_convert_int_to_float(pt_math_simd_vec vint, uint32_t div_exponent);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec_convert_int_to_float(pt_math_simd_vec vint, uint32_t div_exponent);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_convert_int_to_float(pt_math_simd_vec vint, uint32_t div_exponent);

pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec_convert_float_to_int(pt_math_simd_vec vfloat, uint32_t div_exponent);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec_convert_float_to_int(pt_math_simd_vec vfloat, uint32_t div_exponent);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_convert_float_to_int(pt_math_simd_vec vfloat, uint32_t div_exponent);

pt_math_simd_vec PT_VECTORCALL  directx_math_x86_avx2_vec_min(pt_math_simd_vec v1, pt_math_simd_vec v2);
pt_math_simd_vec PT_VECTORCALL  directx_math_x86_avx_vec_min(pt_math_simd_vec v1, pt_math_simd_vec v2);
pt_math_simd_vec PT_VECTORCALL  directx_math_x86_sse2_vec_min(pt_math_simd_vec v1, pt_math_simd_vec v2);

pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec_add(pt_math_simd_vec v1, pt_math_simd_vec v2);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec_add(pt_math_simd_vec v1, pt_math_simd_vec v2);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_add(pt_math_simd_vec v1, pt_math_simd_vec v2);

pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec_subtract(pt_math_simd_vec v1, pt_math_simd_vec v2);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec_subtract(pt_math_simd_vec v1, pt_math_simd_vec v2);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_subtract(pt_math_simd_vec v1, pt_math_simd_vec v2);

pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec_multiply(pt_math_simd_vec v1, pt_math_simd_vec v2);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec_multiply(pt_math_simd_vec v1, pt_math_simd_vec v2);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_multiply(pt_math_simd_vec v1, pt_math_simd_vec v2);

pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec_divide(pt_math_simd_vec v1, pt_math_simd_vec v2);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec_divide(pt_math_simd_vec v1, pt_math_simd_vec v2);
pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_divide(pt_math_simd_vec v1, pt_math_simd_vec v2);

void PT_VECTORCALL directx_math_x86_avx2_store_vec3(pt_math_vec3 *source, pt_math_simd_vec v);
void PT_VECTORCALL directx_math_x86_avx_store_vec3(pt_math_vec3 *source, pt_math_simd_vec v);
void PT_VECTORCALL directx_math_x86_sse2_store_vec3(pt_math_vec3 *source, pt_math_simd_vec v);

void PT_VECTORCALL directx_math_x86_avx2_store_alignas16_vec3(pt_math_alignas16_vec3 *source, pt_math_simd_vec v);
void PT_VECTORCALL directx_math_x86_avx_store_alignas16_vec3(pt_math_alignas16_vec3 *source, pt_math_simd_vec v);
void PT_VECTORCALL directx_math_x86_sse2_store_alignas16_vec3(pt_math_alignas16_vec3 *source, pt_math_simd_vec v);

void PT_VECTORCALL directx_math_x86_avx2_store_vec4(pt_math_vec4 *source, pt_math_simd_vec v);
void PT_VECTORCALL directx_math_x86_avx_store_vec4(pt_math_vec4 *source, pt_math_simd_vec v);
void PT_VECTORCALL directx_math_x86_sse2_store_vec4(pt_math_vec4 *source, pt_math_simd_vec v);

void PT_VECTORCALL directx_math_x86_avx2_store_alignas16_vec4(pt_math_alignas16_vec4 *source, pt_math_simd_vec v);
void PT_VECTORCALL directx_math_x86_avx_store_alignas16_vec4(pt_math_alignas16_vec4 *source, pt_math_simd_vec v);
void PT_VECTORCALL directx_math_x86_sse2_store_alignas16_vec4(pt_math_alignas16_vec4 *source, pt_math_simd_vec v);

void PT_VECTORCALL directx_math_x86_avx2_store_alignas16_ivec3(pt_math_alignas16_ivec3* destination, pt_math_simd_vec v);
void PT_VECTORCALL directx_math_x86_avx_store_alignas16_ivec3(pt_math_alignas16_ivec3* destination, pt_math_simd_vec v);
void PT_VECTORCALL directx_math_x86_sse2_store_alignas16_ivec3(pt_math_alignas16_ivec3* destination, pt_math_simd_vec v);

pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx2_load_alignas16_mat4x4(pt_math_alignas16_mat4x4 const *source);
pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx_load_alignas16_mat4x4(pt_math_alignas16_mat4x4 const *source);
pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_load_alignas16_mat4x4(pt_math_alignas16_mat4x4 const *source);

pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx2_load_mat4x4(pt_math_mat4x4 const *source);
pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx_load_mat4x4(pt_math_mat4x4 const *source);
pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_load_mat4x4(pt_math_mat4x4 const *source);

pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx2_mat_identity();
pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx_mat_identity();
pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_identity();

pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx2_mat_multiply(pt_math_simd_mat m1, pt_math_simd_mat m2);
pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx_mat_multiply(pt_math_simd_mat m1, pt_math_simd_mat m2);
pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_multiply(pt_math_simd_mat m1, pt_math_simd_mat m2);

pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx2_mat_rotation_y(float angle);
pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx_mat_rotation_y(float angle);
pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_rotation_y(float angle);

pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx2_mat_look_to_rh(pt_math_simd_vec eye_position, pt_math_simd_vec eye_direction, pt_math_simd_vec up_direction);
pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx_mat_look_to_rh(pt_math_simd_vec eye_position, pt_math_simd_vec eye_direction, pt_math_simd_vec up_direction);
pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_look_to_rh(pt_math_simd_vec eye_position, pt_math_simd_vec eye_direction, pt_math_simd_vec up_direction);

pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx2_mat_perspective_fov_rh(float fov_angle_y, float aspect_ratio, float near_z, float far_z);
pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx_mat_perspective_fov_rh(float fov_angle_y, float aspect_ratio, float near_z, float far_z);
pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_perspective_fov_rh(float fov_angle_y, float aspect_ratio, float near_z, float far_z);

pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx2_mat_transpose(pt_math_simd_mat m);
pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx_mat_transpose(pt_math_simd_mat m);
pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_transpose(pt_math_simd_mat m);

pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx2_mat_inverse(pt_math_simd_vec *determinant, pt_math_simd_mat m);
pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx_mat_inverse(pt_math_simd_vec *determinant, pt_math_simd_mat m);
pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_inverse(pt_math_simd_vec *determinant, pt_math_simd_mat m);

void PT_VECTORCALL directx_math_x86_avx2_store_alignas16_mat4x4(pt_math_alignas16_mat4x4 *destination, pt_math_simd_mat m);
void PT_VECTORCALL directx_math_x86_avx_store_alignas16_mat4x4(pt_math_alignas16_mat4x4 *destination, pt_math_simd_mat m);
void PT_VECTORCALL directx_math_x86_sse2_store_alignas16_mat4x4(pt_math_alignas16_mat4x4 *destination, pt_math_simd_mat m);

void PT_VECTORCALL directx_math_x86_avx2_store_mat4x4(pt_math_mat4x4 *destination, pt_math_simd_mat m);
void PT_VECTORCALL directx_math_x86_avx_store_mat4x4(pt_math_mat4x4 *destination, pt_math_simd_mat m);
void PT_VECTORCALL directx_math_x86_sse2_store_mat4x4(pt_math_mat4x4 *destination, pt_math_simd_mat m);

pt_math_bounding_sphere PT_VECTORCALL directx_math_x86_avx2_bounding_sphere_create(pt_math_vec3 const *center, float radius);
pt_math_bounding_sphere PT_VECTORCALL directx_math_x86_avx_bounding_sphere_create(pt_math_vec3 const *center, float radius);
pt_math_bounding_sphere PT_VECTORCALL directx_math_x86_sse2_bounding_sphere_create(pt_math_vec3 const *center, float radius);

bool PT_VECTORCALL directx_math_x86_avx2_bounding_sphere_intersect_ray(pt_math_bounding_sphere const *bounding_sphere, pt_math_simd_vec origin, pt_math_simd_vec direction, float *dist);
bool PT_VECTORCALL directx_math_x86_avx_bounding_sphere_intersect_ray(pt_math_bounding_sphere const *bounding_sphere, pt_math_simd_vec origin, pt_math_simd_vec direction, float *dist);
bool PT_VECTORCALL directx_math_x86_sse2_bounding_sphere_intersect_ray(pt_math_bounding_sphere const *bounding_sphere, pt_math_simd_vec origin, pt_math_simd_vec direction, float *dist);

pt_math_bounding_box PT_VECTORCALL directx_math_x86_avx2_bounding_box_create_from_sphere(pt_math_bounding_sphere const *bounding_sphere);
pt_math_bounding_box PT_VECTORCALL directx_math_x86_avx_bounding_box_create_from_sphere(pt_math_bounding_sphere const *bounding_sphere);
pt_math_bounding_box PT_VECTORCALL directx_math_x86_sse2_bounding_box_create_from_sphere(pt_math_bounding_sphere const *bounding_sphere);

pt_math_bounding_box PT_VECTORCALL directx_math_x86_avx2_bounding_box_create_merged(pt_math_bounding_box const *b1, pt_math_bounding_box const *b2);
pt_math_bounding_box PT_VECTORCALL directx_math_x86_avx_bounding_box_create_merged(pt_math_bounding_box const *b1, pt_math_bounding_box const *b2);
pt_math_bounding_box PT_VECTORCALL directx_math_x86_sse2_bounding_box_create_merged(pt_math_bounding_box const *b1, pt_math_bounding_box const *b2);

pt_math_bounding_box PT_VECTORCALL directx_math_x86_avx2_bounding_box_create_from_points(size_t count, pt_math_vec3 const *points, size_t stride);
pt_math_bounding_box PT_VECTORCALL directx_math_x86_avx_bounding_box_create_from_points(size_t count, pt_math_vec3 const *points, size_t stride);
pt_math_bounding_box PT_VECTORCALL directx_math_x86_sse2_bounding_box_create_from_points(size_t count, pt_math_vec3 const *points, size_t stride);

#endif