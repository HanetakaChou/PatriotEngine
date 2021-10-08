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

#if defined(PT_X64) || defined(PT_X86)

static pt_math_simd_vec(PT_VECTORPTR *pfn_math_load_vec3)(pt_math_vec3 const *source) = NULL;
static pt_math_simd_vec(PT_VECTORPTR *pfn_math_load_alignas16_vec3)(pt_math_alignas16_vec3 const *source) = NULL;
static pt_math_simd_vec(PT_VECTORPTR *pfn_math_load_vec4)(pt_math_vec4 const *source) = NULL;
static pt_math_simd_vec(PT_VECTORPTR *pfn_math_load_alignas16_vec4)(pt_math_alignas16_vec4 const *source) = NULL;
static pt_math_simd_vec(PT_VECTORPTR *pfn_math_load_alignas16_ivec3)(pt_math_alignas16_ivec3 const *source) = NULL;
static pt_math_simd_vec(PT_VECTORPTR *pfn_math_vec_zero)() = NULL;
static pt_math_simd_vec(PT_VECTORPTR *pfn_math_vec_replicate)(float value) = NULL;
static pt_math_simd_vec(PT_VECTORPTR *pfn_math_vec3_transform)(pt_math_simd_vec v, pt_math_simd_mat m) = NULL;
static pt_math_simd_vec(PT_VECTORPTR *pfn_math_vec3_transform_coord)(pt_math_simd_vec v, pt_math_simd_mat m) = NULL;
static pt_math_simd_vec(PT_VECTORPTR *pfn_math_vec3_normalize)(pt_math_simd_vec v) = NULL;
static pt_math_simd_vec(PT_VECTORPTR *pfn_math_vec3_dot)(pt_math_simd_vec v1, pt_math_simd_vec v2) = NULL;
static pt_math_simd_vec(PT_VECTORPTR *pfn_math_vec4_dot)(pt_math_simd_vec v1, pt_math_simd_vec v2) = NULL;
static pt_math_simd_vec(PT_VECTORPTR *pfn_math_vec_add)(pt_math_simd_vec v1, pt_math_simd_vec v2) = NULL;
static pt_math_simd_vec(PT_VECTORPTR *pfn_math_vec_subtract)(pt_math_simd_vec v1, pt_math_simd_vec v2) = NULL;
static pt_math_simd_vec(PT_VECTORPTR *pfn_math_vec_multiply)(pt_math_simd_vec v1, pt_math_simd_vec v2) = NULL;
static pt_math_simd_vec(PT_VECTORPTR *pfn_math_vec_divide)(pt_math_simd_vec v1, pt_math_simd_vec v2) = NULL;
static float(PT_VECTORPTR *pfn_math_vec_get_x)(pt_math_simd_vec v) = NULL;
static float(PT_VECTORCALL *pfn_math_vec_get_y)(pt_math_simd_vec v) = NULL;
static float(PT_VECTORCALL *pfn_math_vec_get_z)(pt_math_simd_vec v) = NULL;
static float(PT_VECTORPTR *pfn_math_vec_get_w)(pt_math_simd_vec v) = NULL;
static pt_math_simd_vec(PT_VECTORPTR *pfn_math_vec_min)(pt_math_simd_vec v1, pt_math_simd_vec v2) = NULL;
static pt_math_simd_vec(PT_VECTORPTR *pfn_math_vec_abs)(pt_math_simd_vec v) = NULL;
static pt_math_simd_vec(PT_VECTORPTR *pfn_math_vec_splat_w)(pt_math_simd_vec v) = NULL;
static pt_math_simd_vec(PT_VECTORPTR *pfn_math_vec_convert_int_to_float)(pt_math_simd_vec vint, uint32_t div_exponent) = NULL;
static pt_math_simd_vec(PT_VECTORPTR *pfn_math_vec_convert_float_to_int)(pt_math_simd_vec vfloat, uint32_t div_exponent) = NULL;
static void(PT_VECTORPTR *pfn_math_store_vec3)(pt_math_vec3 *destination, pt_math_simd_vec v) = NULL;
static void(PT_VECTORPTR *pfn_math_store_alignas16_vec3)(pt_math_alignas16_vec3 *destination, pt_math_simd_vec v) = NULL;
static void(PT_VECTORPTR *pfn_math_store_vec4)(pt_math_vec4 *destination, pt_math_simd_vec v) = NULL;
static void(PT_VECTORPTR *pfn_math_store_alignas16_vec4)(pt_math_alignas16_vec4 *destination, pt_math_simd_vec v) = NULL;
static void(PT_VECTORPTR *pfn_math_store_alignas16_ivec3)(pt_math_alignas16_ivec3 *destination, pt_math_simd_vec v) = NULL;
static pt_math_simd_mat(PT_VECTORPTR *pfn_math_load_alignas16_mat4x4)(pt_math_alignas16_mat4x4 const *source) = NULL;
static pt_math_simd_mat(PT_VECTORPTR *pfn_math_load_mat4x4)(pt_math_mat4x4 const *source) = NULL;
static pt_math_simd_mat(PT_VECTORPTR *pfn_math_mat_identity)(void) = NULL;
static pt_math_simd_mat(PT_VECTORPTR *pfn_math_mat_multiply)(pt_math_simd_mat m1, pt_math_simd_mat m2) = NULL;
static pt_math_simd_mat(PT_VECTORPTR *pfn_math_mat_rotation_y)(float angle) = NULL;
static pt_math_simd_mat(PT_VECTORPTR *pfn_math_mat_look_to_rh)(pt_math_simd_vec eye_position, pt_math_simd_vec eye_direction, pt_math_simd_vec up_direction) = NULL;
static pt_math_simd_mat(PT_VECTORPTR *pfn_math_mat_transpose)(pt_math_simd_mat m) = NULL;
static pt_math_simd_mat(PT_VECTORPTR *pfn_math_mat_inverse)(pt_math_simd_vec *determinant, pt_math_simd_mat m) = NULL;
static void(PT_VECTORPTR *pfn_math_store_alignas16_mat4x4)(pt_math_alignas16_mat4x4 *destination, pt_math_simd_mat m) = NULL;
static void(PT_VECTORPTR *pfn_math_store_mat4x4)(pt_math_mat4x4 *destination, pt_math_simd_mat m) = NULL;
static pt_math_bounding_sphere(PT_VECTORPTR *pfn_math_bounding_sphere_create)(pt_math_vec3 const *center, float radius) = NULL;
static bool(PT_VECTORPTR *pfn_math_bounding_sphere_intersect_ray)(pt_math_bounding_sphere const *bounding_sphere, pt_math_simd_vec origin, pt_math_simd_vec direction, float *dist) = NULL;
static pt_math_bounding_box(PT_VECTORPTR *pfn_math_bounding_box_create_from_sphere)(pt_math_bounding_sphere const *bounding_sphere) = NULL;
static pt_math_bounding_box(PT_VECTORPTR *pfn_math_bounding_box_create_merged)(pt_math_bounding_box const *b1, pt_math_bounding_box const *b2);
static pt_math_bounding_box(PT_VECTORPTR *pfn_math_bounding_box_create_from_points)(size_t count, pt_math_vec3 const *points, size_t stride);

extern pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_perspective_fov_rh(float fov_angle_y, float aspect_ratio, float near_z, float far_z);

PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_load_vec3(pt_math_vec3 const *source)
{
    return pfn_math_load_vec3(source);
}

PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_load_alignas16_vec3(pt_math_alignas16_vec3 const *source)
{
    return pfn_math_load_alignas16_vec3(source);
}

PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_load_vec4(pt_math_vec4 const *source)
{
    return pfn_math_load_vec4(source);
}

PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_load_alignas16_vec4(pt_math_alignas16_vec4 const *source)
{
    return pfn_math_load_alignas16_vec4(source);
}

PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_load_alignas16_ivec3(pt_math_alignas16_ivec3 const *source)
{
    return pfn_math_load_alignas16_ivec3(source);
}

PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec_zero()
{
    return pfn_math_vec_zero();
}

PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec_replicate(float value)
{
    return pfn_math_vec_replicate(value);
}

PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec3_transform(pt_math_simd_vec v, pt_math_simd_mat m)
{
    return pfn_math_vec3_transform(v, m);
}

PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec3_transform_coord(pt_math_simd_vec v, pt_math_simd_mat m)
{
    return pfn_math_vec3_transform_coord(v, m);
}

PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec3_normalize(pt_math_simd_vec v)
{
    return pfn_math_vec3_normalize(v);
}

PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec3_dot(pt_math_simd_vec v1, pt_math_simd_vec v2)
{
    return pfn_math_vec3_dot(v1, v2);
}

PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec4_dot(pt_math_simd_vec v1, pt_math_simd_vec v2)
{
    return pfn_math_vec4_dot(v1, v2);
}

PT_ATTR_MATH float PT_VECTORCALL pt_math_vec_get_x(pt_math_simd_vec v)
{
    return pfn_math_vec_get_x(v);
}

PT_ATTR_MATH float PT_VECTORCALL pt_math_vec_get_y(pt_math_simd_vec v)
{
    return pfn_math_vec_get_y(v);
}
PT_ATTR_MATH float PT_VECTORCALL pt_math_vec_get_z(pt_math_simd_vec v)
{
    return pfn_math_vec_get_z(v);
}

PT_ATTR_MATH float PT_VECTORCALL pt_math_vec_get_w(pt_math_simd_vec v)
{
    return pfn_math_vec_get_w(v);
}

PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec_abs(pt_math_simd_vec v)
{
    return pfn_math_vec_abs(v);
}

PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec_splat_w(pt_math_simd_vec v)
{
    return pfn_math_vec_splat_w(v);
}

PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec_convert_int_to_float(pt_math_simd_vec vint, uint32_t div_exponent)
{
    return pfn_math_vec_convert_int_to_float(vint, div_exponent);
}

PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec_convert_float_to_int(pt_math_simd_vec vfloat, uint32_t div_exponent)
{
    return pfn_math_vec_convert_float_to_int(vfloat, div_exponent);
}

PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec_min(pt_math_simd_vec v1, pt_math_simd_vec v2)
{
    return pfn_math_vec_min(v1, v2);
}

PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec_add(pt_math_simd_vec v1, pt_math_simd_vec v2)
{
    return pfn_math_vec_add(v1, v2);
}

PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec_subtract(pt_math_simd_vec v1, pt_math_simd_vec v2)
{
    return pfn_math_vec_subtract(v1, v2);
}

PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec_multiply(pt_math_simd_vec v1, pt_math_simd_vec v2)
{
    return pfn_math_vec_multiply(v1, v2);
}

PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec_divide(pt_math_simd_vec v1, pt_math_simd_vec v2)
{
    return pfn_math_vec_divide(v1, v2);
}

PT_ATTR_MATH void PT_VECTORCALL pt_math_store_vec3(pt_math_vec3 *destination, pt_math_simd_vec v)
{
    return pfn_math_store_vec3(destination, v);
}

PT_ATTR_MATH void PT_VECTORCALL pt_math_store_alignas16_vec3(pt_math_alignas16_vec3 *destination, pt_math_simd_vec v)
{
    return pfn_math_store_alignas16_vec3(destination, v);
}

PT_ATTR_MATH void PT_VECTORCALL pt_math_store_vec4(pt_math_vec4 *destination, pt_math_simd_vec v)
{
    return pfn_math_store_vec4(destination, v);
}

PT_ATTR_MATH void PT_VECTORCALL pt_math_store_alignas16_vec4(pt_math_alignas16_vec4 *destination, pt_math_simd_vec v)
{
    return pfn_math_store_alignas16_vec4(destination, v);
}

PT_ATTR_MATH void PT_VECTORCALL pt_math_store_alignas16_ivec3(pt_math_alignas16_ivec3 *destination, pt_math_simd_vec v)
{
    return pfn_math_store_alignas16_ivec3(destination, v);
}

PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_load_alignas16_mat4x4(pt_math_alignas16_mat4x4 const *source)
{
    return pfn_math_load_alignas16_mat4x4(source);
}

PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_load_mat4x4(pt_math_mat4x4 const *source)
{
    return pfn_math_load_mat4x4(source);
}

PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_mat_identity(void)
{
    return pfn_math_mat_identity();
}

PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_mat_multiply(pt_math_simd_mat m1, pt_math_simd_mat m2)
{
    return pfn_math_mat_multiply(m1, m2);
}

PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_mat_rotation_y(float angle)
{
    return pfn_math_mat_rotation_y(angle);
}

PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_mat_look_to_rh(pt_math_simd_vec eye_position, pt_math_simd_vec eye_direction, pt_math_simd_vec up_direction)
{
    return pfn_math_mat_look_to_rh(eye_position, eye_direction, up_direction);
}

PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_mat_perspective_fov_rh(float fov_angle_y, float aspect_ratio, float near_z, float far_z)
{
    return directx_math_x86_sse2_mat_perspective_fov_rh(fov_angle_y, aspect_ratio, near_z, far_z);
}

PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_mat_transpose(pt_math_simd_mat m)
{
    return pfn_math_mat_transpose(m);
}

PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_mat_inverse(pt_math_simd_vec *determinant, pt_math_simd_mat m)
{
    return pfn_math_mat_inverse(determinant, m);
}

PT_ATTR_MATH void PT_VECTORCALL pt_math_store_alignas16_mat4x4(pt_math_alignas16_mat4x4 *destination, pt_math_simd_mat m)
{
    return pfn_math_store_alignas16_mat4x4(destination, m);
}

PT_ATTR_MATH void PT_VECTORCALL pt_math_store_mat4x4(pt_math_mat4x4 *destination, pt_math_simd_mat m)
{
    return pfn_math_store_mat4x4(destination, m);
}

PT_ATTR_MATH pt_math_bounding_sphere PT_VECTORCALL pt_math_bounding_sphere_create(pt_math_vec3 const *center, float radius)
{
    return pfn_math_bounding_sphere_create(center, radius);
}

PT_ATTR_MATH bool PT_VECTORCALL pt_math_bounding_sphere_intersect_ray(pt_math_bounding_sphere const *bounding_sphere, pt_math_simd_vec origin, pt_math_simd_vec direction, float *dist)
{
    return pfn_math_bounding_sphere_intersect_ray(bounding_sphere, origin, direction, dist);
}

PT_ATTR_MATH pt_math_bounding_box PT_VECTORCALL pt_math_bounding_box_create_from_sphere(pt_math_bounding_sphere const *bounding_sphere)
{
    return pfn_math_bounding_box_create_from_sphere(bounding_sphere);
}

PT_ATTR_MATH pt_math_bounding_box PT_VECTORCALL pt_math_bounding_box_create_merged(pt_math_bounding_box const *b1, pt_math_bounding_box const *b2)
{
    return pfn_math_bounding_box_create_merged(b1, b2);
}

PT_ATTR_MATH pt_math_bounding_box PT_VECTORCALL pt_math_bounding_box_create_from_points(size_t count, pt_math_vec3 const *points, size_t stride)
{
    return pfn_math_bounding_box_create_from_points(count, points, stride);
}

#include <pt_mcrt_intrin.h>

extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_load_vec3(pt_math_vec3 const *source);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_load_vec3(pt_math_vec3 const *source);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_load_vec3(pt_math_vec3 const *source);

extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_load_alignas16_vec3(pt_math_alignas16_vec3 const *source);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_load_alignas16_vec3(pt_math_alignas16_vec3 const *source);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_load_alignas16_vec3(pt_math_alignas16_vec3 const *source);

extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_load_vec4(pt_math_vec4 const *source);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_load_vec4(pt_math_vec4 const *source);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_load_vec4(pt_math_vec4 const *source);

extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_load_alignas16_vec4(pt_math_alignas16_vec4 const *source);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_load_alignas16_vec4(pt_math_alignas16_vec4 const *source);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_load_alignas16_vec4(pt_math_alignas16_vec4 const *source);

extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_load_alignas16_ivec3(pt_math_alignas16_ivec3 const *source);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_load_alignas16_ivec3(pt_math_alignas16_ivec3 const *source);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_load_alignas16_ivec3(pt_math_alignas16_ivec3 const *source);

extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec_zero();
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec_zero();
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_zero();

extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec_replicate(float value);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec_replicate(float value);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_replicate(float value);

extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec3_transform(pt_math_simd_vec v, pt_math_simd_mat m);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec3_transform(pt_math_simd_vec v, pt_math_simd_mat m);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec3_transform(pt_math_simd_vec v, pt_math_simd_mat m);

extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec3_transform_coord(pt_math_simd_vec v, pt_math_simd_mat m);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec3_transform_coord(pt_math_simd_vec v, pt_math_simd_mat m);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec3_transform_coord(pt_math_simd_vec v, pt_math_simd_mat m);

extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec3_normalize(pt_math_simd_vec v);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec3_normalize(pt_math_simd_vec v);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec3_normalize(pt_math_simd_vec v);

extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec3_dot(pt_math_simd_vec v1, pt_math_simd_vec v2);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec3_dot(pt_math_simd_vec v1, pt_math_simd_vec v2);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec3_dot(pt_math_simd_vec v1, pt_math_simd_vec v2);

extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec4_dot(pt_math_simd_vec v1, pt_math_simd_vec v2);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec4_dot(pt_math_simd_vec v1, pt_math_simd_vec v2);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec4_dot(pt_math_simd_vec v1, pt_math_simd_vec v2);

extern float PT_VECTORCALL directx_math_x86_avx2_vec_get_x(pt_math_simd_vec v);
extern float PT_VECTORCALL directx_math_x86_avx_vec_get_x(pt_math_simd_vec v);
extern float PT_VECTORCALL directx_math_x86_sse2_vec_get_x(pt_math_simd_vec v);

extern float PT_VECTORCALL directx_math_x86_avx2_vec_get_y(pt_math_simd_vec v);
extern float PT_VECTORCALL directx_math_x86_avx_vec_get_y(pt_math_simd_vec v);
extern float PT_VECTORCALL directx_math_x86_sse2_vec_get_y(pt_math_simd_vec v);

extern float PT_VECTORCALL directx_math_x86_avx2_vec_get_z(pt_math_simd_vec v);
extern float PT_VECTORCALL directx_math_x86_avx_vec_get_z(pt_math_simd_vec v);
extern float PT_VECTORCALL directx_math_x86_sse2_vec_get_z(pt_math_simd_vec v);

extern float PT_VECTORCALL directx_math_x86_avx2_vec_get_w(pt_math_simd_vec v);
extern float PT_VECTORCALL directx_math_x86_avx_vec_get_w(pt_math_simd_vec v);
extern float PT_VECTORCALL directx_math_x86_sse2_vec_get_w(pt_math_simd_vec v);

extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec_abs(pt_math_simd_vec v);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec_abs(pt_math_simd_vec v);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_abs(pt_math_simd_vec v);

extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec_splat_w(pt_math_simd_vec v);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec_splat_w(pt_math_simd_vec v);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_splat_w(pt_math_simd_vec v);

extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec_convert_int_to_float(pt_math_simd_vec vint, uint32_t div_exponent);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec_convert_int_to_float(pt_math_simd_vec vint, uint32_t div_exponent);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_convert_int_to_float(pt_math_simd_vec vint, uint32_t div_exponent);

extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec_convert_float_to_int(pt_math_simd_vec vfloat, uint32_t div_exponent);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec_convert_float_to_int(pt_math_simd_vec vfloat, uint32_t div_exponent);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_convert_float_to_int(pt_math_simd_vec vfloat, uint32_t div_exponent);

extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec_min(pt_math_simd_vec v1, pt_math_simd_vec v2);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec_min(pt_math_simd_vec v1, pt_math_simd_vec v2);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_min(pt_math_simd_vec v1, pt_math_simd_vec v2);

extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec_add(pt_math_simd_vec v1, pt_math_simd_vec v2);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec_add(pt_math_simd_vec v1, pt_math_simd_vec v2);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_add(pt_math_simd_vec v1, pt_math_simd_vec v2);

extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec_subtract(pt_math_simd_vec v1, pt_math_simd_vec v2);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec_subtract(pt_math_simd_vec v1, pt_math_simd_vec v2);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_subtract(pt_math_simd_vec v1, pt_math_simd_vec v2);

extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec_multiply(pt_math_simd_vec v1, pt_math_simd_vec v2);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec_multiply(pt_math_simd_vec v1, pt_math_simd_vec v2);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_multiply(pt_math_simd_vec v1, pt_math_simd_vec v2);

extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx2_vec_divide(pt_math_simd_vec v1, pt_math_simd_vec v2);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_avx_vec_divide(pt_math_simd_vec v1, pt_math_simd_vec v2);
extern pt_math_simd_vec PT_VECTORCALL directx_math_x86_sse2_vec_divide(pt_math_simd_vec v1, pt_math_simd_vec v2);

extern void PT_VECTORCALL directx_math_x86_avx2_store_vec3(pt_math_vec3 *source, pt_math_simd_vec v);
extern void PT_VECTORCALL directx_math_x86_avx_store_vec3(pt_math_vec3 *source, pt_math_simd_vec v);
extern void PT_VECTORCALL directx_math_x86_sse2_store_vec3(pt_math_vec3 *source, pt_math_simd_vec v);

extern void PT_VECTORCALL directx_math_x86_avx2_store_alignas16_vec3(pt_math_alignas16_vec3 *source, pt_math_simd_vec v);
extern void PT_VECTORCALL directx_math_x86_avx_store_alignas16_vec3(pt_math_alignas16_vec3 *source, pt_math_simd_vec v);
extern void PT_VECTORCALL directx_math_x86_sse2_store_alignas16_vec3(pt_math_alignas16_vec3 *source, pt_math_simd_vec v);

extern void PT_VECTORCALL directx_math_x86_avx2_store_vec4(pt_math_vec4 *source, pt_math_simd_vec v);
extern void PT_VECTORCALL directx_math_x86_avx_store_vec4(pt_math_vec4 *source, pt_math_simd_vec v);
extern void PT_VECTORCALL directx_math_x86_sse2_store_vec4(pt_math_vec4 *source, pt_math_simd_vec v);

extern void PT_VECTORCALL directx_math_x86_avx2_store_alignas16_vec4(pt_math_alignas16_vec4 *source, pt_math_simd_vec v);
extern void PT_VECTORCALL directx_math_x86_avx_store_alignas16_vec4(pt_math_alignas16_vec4 *source, pt_math_simd_vec v);
extern void PT_VECTORCALL directx_math_x86_sse2_store_alignas16_vec4(pt_math_alignas16_vec4 *source, pt_math_simd_vec v);

extern void PT_VECTORCALL directx_math_x86_avx2_store_alignas16_ivec3(pt_math_alignas16_ivec3 *destination, pt_math_simd_vec v);
extern void PT_VECTORCALL directx_math_x86_avx_store_alignas16_ivec3(pt_math_alignas16_ivec3 *destination, pt_math_simd_vec v);
extern void PT_VECTORCALL directx_math_x86_sse2_store_alignas16_ivec3(pt_math_alignas16_ivec3 *destination, pt_math_simd_vec v);

extern pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx2_load_alignas16_mat4x4(pt_math_alignas16_mat4x4 const *source);
extern pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx_load_alignas16_mat4x4(pt_math_alignas16_mat4x4 const *source);
extern pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_load_alignas16_mat4x4(pt_math_alignas16_mat4x4 const *source);

extern pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx2_load_mat4x4(pt_math_mat4x4 const *source);
extern pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx_load_mat4x4(pt_math_mat4x4 const *source);
extern pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_load_mat4x4(pt_math_mat4x4 const *source);

extern pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx2_mat_identity();
extern pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx_mat_identity();
extern pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_identity();

extern pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx2_mat_multiply(pt_math_simd_mat m1, pt_math_simd_mat m2);
extern pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx_mat_multiply(pt_math_simd_mat m1, pt_math_simd_mat m2);
extern pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_multiply(pt_math_simd_mat m1, pt_math_simd_mat m2);

extern pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx2_mat_rotation_y(float angle);
extern pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx_mat_rotation_y(float angle);
extern pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_rotation_y(float angle);

extern pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx2_mat_look_to_rh(pt_math_simd_vec eye_position, pt_math_simd_vec eye_direction, pt_math_simd_vec up_direction);
extern pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx_mat_look_to_rh(pt_math_simd_vec eye_position, pt_math_simd_vec eye_direction, pt_math_simd_vec up_direction);
extern pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_look_to_rh(pt_math_simd_vec eye_position, pt_math_simd_vec eye_direction, pt_math_simd_vec up_direction);

extern pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx2_mat_transpose(pt_math_simd_mat m);
extern pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx_mat_transpose(pt_math_simd_mat m);
extern pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_transpose(pt_math_simd_mat m);

extern pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx2_mat_inverse(pt_math_simd_vec *determinant, pt_math_simd_mat m);
extern pt_math_simd_mat PT_VECTORCALL directx_math_x86_avx_mat_inverse(pt_math_simd_vec *determinant, pt_math_simd_mat m);
extern pt_math_simd_mat PT_VECTORCALL directx_math_x86_sse2_mat_inverse(pt_math_simd_vec *determinant, pt_math_simd_mat m);

extern void PT_VECTORCALL directx_math_x86_avx2_store_alignas16_mat4x4(pt_math_alignas16_mat4x4 *destination, pt_math_simd_mat m);
extern void PT_VECTORCALL directx_math_x86_avx_store_alignas16_mat4x4(pt_math_alignas16_mat4x4 *destination, pt_math_simd_mat m);
extern void PT_VECTORCALL directx_math_x86_sse2_store_alignas16_mat4x4(pt_math_alignas16_mat4x4 *destination, pt_math_simd_mat m);

extern void PT_VECTORCALL directx_math_x86_avx2_store_mat4x4(pt_math_mat4x4 *destination, pt_math_simd_mat m);
extern void PT_VECTORCALL directx_math_x86_avx_store_mat4x4(pt_math_mat4x4 *destination, pt_math_simd_mat m);
extern void PT_VECTORCALL directx_math_x86_sse2_store_mat4x4(pt_math_mat4x4 *destination, pt_math_simd_mat m);

extern pt_math_bounding_sphere PT_VECTORCALL directx_math_x86_avx2_bounding_sphere_create(pt_math_vec3 const *center, float radius);
extern pt_math_bounding_sphere PT_VECTORCALL directx_math_x86_avx_bounding_sphere_create(pt_math_vec3 const *center, float radius);
extern pt_math_bounding_sphere PT_VECTORCALL directx_math_x86_sse2_bounding_sphere_create(pt_math_vec3 const *center, float radius);

extern bool PT_VECTORCALL directx_math_x86_avx2_bounding_sphere_intersect_ray(pt_math_bounding_sphere const *bounding_sphere, pt_math_simd_vec origin, pt_math_simd_vec direction, float *dist);
extern bool PT_VECTORCALL directx_math_x86_avx_bounding_sphere_intersect_ray(pt_math_bounding_sphere const *bounding_sphere, pt_math_simd_vec origin, pt_math_simd_vec direction, float *dist);
extern bool PT_VECTORCALL directx_math_x86_sse2_bounding_sphere_intersect_ray(pt_math_bounding_sphere const *bounding_sphere, pt_math_simd_vec origin, pt_math_simd_vec direction, float *dist);

extern pt_math_bounding_box PT_VECTORCALL directx_math_x86_avx2_bounding_box_create_from_sphere(pt_math_bounding_sphere const *bounding_sphere);
extern pt_math_bounding_box PT_VECTORCALL directx_math_x86_avx_bounding_box_create_from_sphere(pt_math_bounding_sphere const *bounding_sphere);
extern pt_math_bounding_box PT_VECTORCALL directx_math_x86_sse2_bounding_box_create_from_sphere(pt_math_bounding_sphere const *bounding_sphere);

extern pt_math_bounding_box PT_VECTORCALL directx_math_x86_avx2_bounding_box_create_merged(pt_math_bounding_box const *b1, pt_math_bounding_box const *b2);
extern pt_math_bounding_box PT_VECTORCALL directx_math_x86_avx_bounding_box_create_merged(pt_math_bounding_box const *b1, pt_math_bounding_box const *b2);
extern pt_math_bounding_box PT_VECTORCALL directx_math_x86_sse2_bounding_box_create_merged(pt_math_bounding_box const *b1, pt_math_bounding_box const *b2);

extern pt_math_bounding_box PT_VECTORCALL directx_math_x86_avx2_bounding_box_create_from_points(size_t count, pt_math_vec3 const *points, size_t stride);
extern pt_math_bounding_box PT_VECTORCALL directx_math_x86_avx_bounding_box_create_from_points(size_t count, pt_math_vec3 const *points, size_t stride);
extern pt_math_bounding_box PT_VECTORCALL directx_math_x86_sse2_bounding_box_create_from_points(size_t count, pt_math_vec3 const *points, size_t stride);

struct math_verify_x86_cpu_support
{
    inline math_verify_x86_cpu_support()
    {
        //https://docs.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex
        uint32_t f_1_ECX_ = 0;
        uint32_t f_7_EBX_ = 0;
        {
            uint32_t cpui[4];
            mcrt_intrin_cpuidex(cpui, 0, 0);
            uint32_t nIds = cpui[0];

            uint32_t data_1[4];
            if (nIds >= 1)
            {
                mcrt_intrin_cpuidex(data_1, 1, 0);
                f_1_ECX_ = data_1[2];
            }

            uint32_t data_7[4];
            if (nIds >= 7)
            {
                mcrt_intrin_cpuidex(data_7, 7, 0);
                f_7_EBX_ = data_7[1];
            }
        }
        // math_support_avx512f = ((f_7_EBX_ & (1U << 16U)) != 0);
        bool support_avx2 = ((f_7_EBX_ & (1U << 5U)) != 0);
        bool support_fma = ((f_1_ECX_ & (1U << 12U)) != 0);
        bool support_avx = ((f_1_ECX_ & (1U << 28U)) != 0);

        bool math_support_avx2 = (support_avx2 && support_fma);
        bool math_support_avx = support_avx;

        if (math_support_avx2)
        {
            pfn_math_load_vec3 = directx_math_x86_avx2_load_vec3;
            pfn_math_load_alignas16_vec3 = directx_math_x86_avx2_load_alignas16_vec3;
            pfn_math_load_vec4 = directx_math_x86_avx2_load_vec4;
            pfn_math_load_alignas16_vec4 = directx_math_x86_avx2_load_alignas16_vec4;
            pfn_math_load_alignas16_ivec3 = directx_math_x86_avx2_load_alignas16_ivec3;
            pfn_math_vec_zero = directx_math_x86_avx2_vec_zero;
            pfn_math_vec_replicate = directx_math_x86_avx2_vec_replicate;
            pfn_math_vec3_transform = directx_math_x86_avx2_vec3_transform;
            pfn_math_vec3_transform_coord = directx_math_x86_avx2_vec3_transform_coord;
            pfn_math_vec3_normalize = directx_math_x86_avx2_vec3_normalize;
            pfn_math_vec3_dot = directx_math_x86_avx2_vec3_dot;
            pfn_math_vec4_dot = directx_math_x86_avx2_vec4_dot;
            pfn_math_vec_add = directx_math_x86_avx2_vec_add;
            pfn_math_vec_subtract = directx_math_x86_avx2_vec_subtract;
            pfn_math_vec_multiply = directx_math_x86_avx2_vec_multiply;
            pfn_math_vec_divide = directx_math_x86_avx2_vec_divide;
            pfn_math_vec_get_x = directx_math_x86_avx2_vec_get_x;
            pfn_math_vec_get_y = directx_math_x86_avx2_vec_get_y;
            pfn_math_vec_get_z = directx_math_x86_avx2_vec_get_z;
            pfn_math_vec_get_w = directx_math_x86_avx2_vec_get_w;
            pfn_math_vec_abs = directx_math_x86_avx2_vec_abs;
            pfn_math_vec_splat_w = directx_math_x86_avx2_vec_splat_w;
            pfn_math_vec_convert_int_to_float = directx_math_x86_avx2_vec_convert_int_to_float;
            pfn_math_vec_convert_float_to_int = directx_math_x86_avx2_vec_convert_float_to_int;
            pfn_math_vec_min = directx_math_x86_avx2_vec_min;
            pfn_math_store_vec3 = directx_math_x86_avx2_store_vec3;
            pfn_math_store_alignas16_vec3 = directx_math_x86_avx2_store_alignas16_vec3;
            pfn_math_store_vec4 = directx_math_x86_avx2_store_vec4;
            pfn_math_store_alignas16_vec4 = directx_math_x86_avx2_store_alignas16_vec4;
            pfn_math_store_alignas16_ivec3 = directx_math_x86_avx2_store_alignas16_ivec3;
            pfn_math_load_alignas16_mat4x4 = directx_math_x86_avx2_load_alignas16_mat4x4;
            pfn_math_load_mat4x4 = directx_math_x86_avx2_load_mat4x4;
            pfn_math_mat_identity = directx_math_x86_avx2_mat_identity;
            pfn_math_mat_multiply = directx_math_x86_avx2_mat_multiply;
            pfn_math_mat_rotation_y = directx_math_x86_avx2_mat_rotation_y;
            pfn_math_mat_look_to_rh = directx_math_x86_avx2_mat_look_to_rh;
            pfn_math_mat_transpose = directx_math_x86_avx2_mat_transpose;
            pfn_math_mat_inverse = directx_math_x86_avx2_mat_inverse;
            pfn_math_store_alignas16_mat4x4 = directx_math_x86_avx2_store_alignas16_mat4x4;
            pfn_math_store_mat4x4 = directx_math_x86_avx2_store_mat4x4;
            pfn_math_bounding_sphere_create = directx_math_x86_avx2_bounding_sphere_create;
            pfn_math_bounding_sphere_intersect_ray = directx_math_x86_avx2_bounding_sphere_intersect_ray;
            pfn_math_bounding_box_create_from_sphere = directx_math_x86_avx2_bounding_box_create_from_sphere;
            pfn_math_bounding_box_create_merged = directx_math_x86_avx2_bounding_box_create_merged;
            pfn_math_bounding_box_create_from_points = directx_math_x86_avx2_bounding_box_create_from_points;
        }
        else if (math_support_avx)
        {
            pfn_math_load_vec3 = directx_math_x86_avx_load_vec3;
            pfn_math_load_alignas16_vec3 = directx_math_x86_avx_load_alignas16_vec3;
            pfn_math_load_vec4 = directx_math_x86_avx_load_vec4;
            pfn_math_load_alignas16_vec4 = directx_math_x86_avx_load_alignas16_vec4;
            pfn_math_load_alignas16_ivec3 = directx_math_x86_avx_load_alignas16_ivec3;
            pfn_math_vec_zero = directx_math_x86_avx_vec_zero;
            pfn_math_vec_replicate = directx_math_x86_avx_vec_replicate;
            pfn_math_vec3_transform = directx_math_x86_avx_vec3_transform;
            pfn_math_vec3_transform_coord = directx_math_x86_avx_vec3_transform_coord;
            pfn_math_vec3_normalize = directx_math_x86_avx_vec3_normalize;
            pfn_math_vec3_dot = directx_math_x86_avx_vec3_dot;
            pfn_math_vec4_dot = directx_math_x86_avx_vec4_dot;
            pfn_math_vec_add = directx_math_x86_avx_vec_add;
            pfn_math_vec_subtract = directx_math_x86_avx_vec_subtract;
            pfn_math_vec_multiply = directx_math_x86_avx_vec_multiply;
            pfn_math_vec_divide = directx_math_x86_avx_vec_divide;
            pfn_math_vec_get_x = directx_math_x86_avx_vec_get_x;
            pfn_math_vec_get_y = directx_math_x86_avx_vec_get_y;
            pfn_math_vec_get_z = directx_math_x86_avx_vec_get_z;
            pfn_math_vec_get_w = directx_math_x86_avx_vec_get_w;
            pfn_math_vec_abs = directx_math_x86_avx_vec_abs;
            pfn_math_vec_splat_w = directx_math_x86_avx_vec_splat_w;
            pfn_math_vec_convert_int_to_float = directx_math_x86_avx2_vec_convert_int_to_float;
            pfn_math_vec_convert_float_to_int = directx_math_x86_avx2_vec_convert_float_to_int;
            pfn_math_vec_min = directx_math_x86_avx_vec_min;
            pfn_math_store_vec3 = directx_math_x86_avx_store_vec3;
            pfn_math_store_alignas16_vec3 = directx_math_x86_avx_store_alignas16_vec3;
            pfn_math_store_vec4 = directx_math_x86_avx_store_vec4;
            pfn_math_store_alignas16_vec4 = directx_math_x86_avx_store_alignas16_vec4;
            pfn_math_store_alignas16_ivec3 = directx_math_x86_avx_store_alignas16_ivec3;
            pfn_math_load_alignas16_mat4x4 = directx_math_x86_avx_load_alignas16_mat4x4;
            pfn_math_load_mat4x4 = directx_math_x86_avx_load_mat4x4;
            pfn_math_mat_identity = directx_math_x86_avx_mat_identity;
            pfn_math_mat_multiply = directx_math_x86_avx_mat_multiply;
            pfn_math_mat_rotation_y = directx_math_x86_avx_mat_rotation_y;
            pfn_math_mat_look_to_rh = directx_math_x86_avx_mat_look_to_rh;
            pfn_math_mat_transpose = directx_math_x86_avx_mat_transpose;
            pfn_math_mat_inverse = directx_math_x86_avx_mat_inverse;
            pfn_math_store_alignas16_mat4x4 = directx_math_x86_avx_store_alignas16_mat4x4;
            pfn_math_store_mat4x4 = directx_math_x86_avx_store_mat4x4;
            pfn_math_bounding_sphere_create = directx_math_x86_avx_bounding_sphere_create;
            pfn_math_bounding_sphere_intersect_ray = directx_math_x86_avx_bounding_sphere_intersect_ray;
            pfn_math_bounding_box_create_from_sphere = directx_math_x86_avx_bounding_box_create_from_sphere;
            pfn_math_bounding_box_create_merged = directx_math_x86_avx_bounding_box_create_merged;
            pfn_math_bounding_box_create_from_points = directx_math_x86_avx_bounding_box_create_from_points;
        }
        else
        {
            pfn_math_load_vec3 = directx_math_x86_sse2_load_vec3;
            pfn_math_load_alignas16_vec3 = directx_math_x86_sse2_load_alignas16_vec3;
            pfn_math_load_vec4 = directx_math_x86_sse2_load_vec4;
            pfn_math_load_alignas16_vec4 = directx_math_x86_sse2_load_alignas16_vec4;
            pfn_math_load_alignas16_ivec3 = directx_math_x86_sse2_load_alignas16_ivec3;
            pfn_math_vec_zero = directx_math_x86_sse2_vec_zero;
            pfn_math_vec_replicate = directx_math_x86_sse2_vec_replicate;
            pfn_math_vec3_transform = directx_math_x86_sse2_vec3_transform;
            pfn_math_vec3_transform_coord = directx_math_x86_sse2_vec3_transform_coord;
            pfn_math_vec3_normalize = directx_math_x86_sse2_vec3_normalize;
            pfn_math_vec3_dot = directx_math_x86_sse2_vec3_dot;
            pfn_math_vec4_dot = directx_math_x86_sse2_vec4_dot;
            pfn_math_vec_abs = directx_math_x86_sse2_vec_abs;
            pfn_math_vec_splat_w = directx_math_x86_sse2_vec_splat_w;
            pfn_math_vec_convert_int_to_float = directx_math_x86_avx2_vec_convert_int_to_float;
            pfn_math_vec_convert_float_to_int = directx_math_x86_avx2_vec_convert_float_to_int;
            pfn_math_vec_min = directx_math_x86_sse2_vec_min;
            pfn_math_vec_add = directx_math_x86_sse2_vec_add;
            pfn_math_vec_subtract = directx_math_x86_sse2_vec_subtract;
            pfn_math_vec_multiply = directx_math_x86_sse2_vec_multiply;
            pfn_math_vec_divide = directx_math_x86_sse2_vec_divide;
            pfn_math_vec_get_x = directx_math_x86_sse2_vec_get_x;
            pfn_math_vec_get_y = directx_math_x86_sse2_vec_get_y;
            pfn_math_vec_get_z = directx_math_x86_sse2_vec_get_z;
            pfn_math_vec_get_w = directx_math_x86_sse2_vec_get_w;
            pfn_math_store_vec3 = directx_math_x86_sse2_store_vec3;
            pfn_math_store_alignas16_vec3 = directx_math_x86_sse2_store_alignas16_vec3;
            pfn_math_store_vec4 = directx_math_x86_sse2_store_vec4;
            pfn_math_store_alignas16_vec4 = directx_math_x86_sse2_store_alignas16_vec4;
            pfn_math_store_alignas16_ivec3 = directx_math_x86_sse2_store_alignas16_ivec3;
            pfn_math_load_alignas16_mat4x4 = directx_math_x86_sse2_load_alignas16_mat4x4;
            pfn_math_load_mat4x4 = directx_math_x86_sse2_load_mat4x4;
            pfn_math_mat_identity = directx_math_x86_sse2_mat_identity;
            pfn_math_mat_multiply = directx_math_x86_sse2_mat_multiply;
            pfn_math_mat_rotation_y = directx_math_x86_sse2_mat_rotation_y;
            pfn_math_mat_look_to_rh = directx_math_x86_sse2_mat_look_to_rh;
            pfn_math_mat_transpose = directx_math_x86_sse2_mat_transpose;
            pfn_math_mat_inverse = directx_math_x86_sse2_mat_inverse;
            pfn_math_store_alignas16_mat4x4 = directx_math_x86_sse2_store_alignas16_mat4x4;
            pfn_math_store_mat4x4 = directx_math_x86_sse2_store_mat4x4;
            pfn_math_bounding_sphere_create = directx_math_x86_sse2_bounding_sphere_create;
            pfn_math_bounding_sphere_intersect_ray = directx_math_x86_sse2_bounding_sphere_intersect_ray;
            pfn_math_bounding_box_create_from_sphere = directx_math_x86_sse2_bounding_box_create_from_sphere;
            pfn_math_bounding_box_create_merged = directx_math_x86_sse2_bounding_box_create_merged;
            pfn_math_bounding_box_create_from_points = directx_math_x86_sse2_bounding_box_create_from_points;
        }
    }
};
static struct math_verify_x86_cpu_support instance_math_verify_x86_cpu_support;
#elif defined(PT_ARM64) || defined(PT_ARM)
extern pt_math_simd_vec PT_VECTORCALL directx_math_arm_neon_load_vec3(pt_math_vec3 const *source);
extern pt_math_simd_vec PT_VECTORCALL directx_math_arm_neon_load_vec4(pt_math_vec4 const *source);
extern void PT_VECTORCALL directx_math_arm_neon_store_vec3(pt_math_vec3 *source, pt_math_simd_vec v);
extern void PT_VECTORCALL directx_math_arm_neon_store_vec4(pt_math_vec4 *source, pt_math_simd_vec v);
extern pt_math_simd_mat PT_VECTORCALL directx_math_arm_neon_load_alignas16_mat4x4(pt_math_alignas16_mat4x4 const *source);
extern pt_math_simd_mat PT_VECTORCALL directx_math_arm_neon_load_mat4x4(pt_math_mat4x4 const *source);
extern pt_math_simd_mat PT_VECTORCALL directx_math_arm_neon_mat_identity();
extern pt_math_simd_mat PT_VECTORCALL directx_math_arm_neon_mat_multiply(pt_math_simd_mat m1, pt_math_simd_mat m2);
extern pt_math_simd_mat PT_VECTORCALL directx_math_arm_neon_mat_look_to_rh(pt_math_simd_vec eye_position, pt_math_simd_vec eye_direction, pt_math_simd_vec up_direction);
extern pt_math_simd_mat PT_VECTORCALL directx_math_arm_neon_mat_perspective_fov_rh(float fov_angle_y, float aspect_ratio, float near_z, float far_z);
extern void PT_VECTORCALL directx_math_arm_neon_store_alignas16_mat4x4(pt_math_alignas16_mat4x4 *destination, pt_math_simd_mat m);
extern void PT_VECTORCALL directx_math_arm_neon_store_mat4x4(pt_math_mat4x4 *destination, pt_math_simd_mat m);

PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_load_vec3(pt_math_vec3 const *source)
{
    return directx_math_arm_neon_load_vec3(source);
}

PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_load_vec4(pt_math_vec4 const *source)
{
    return directx_math_arm_neon_load_vec4(source);
}

PT_ATTR_MATH void PT_VECTORCALL pt_math_store_vec3(pt_math_vec3 *destination, pt_math_simd_vec v)
{
    return directx_math_arm_neon_store_vec3(destination, v);
}

PT_ATTR_MATH void PT_VECTORCALL pt_math_store_vec4(pt_math_vec4 *destination, pt_math_simd_vec v)
{
    return directx_math_arm_neon_store_vec4(destination, v);
}

PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_load_alignas16_mat4x4(pt_math_alignas16_mat4x4 const *source)
{
    return directx_math_arm_neon_load_alignas16_mat4x4(source);
}

PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_load_mat4x4(pt_math_mat4x4 const *source)
{
    return directx_math_arm_neon_load_mat4x4(source);
}

PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_mat_identity()
{
    return directx_math_arm_neon_mat_identity();
}

PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_mat_multiply(pt_math_simd_mat m1, pt_math_simd_mat m2)
{
    return directx_math_arm_neon_mat_multiply(m1, m2);
}

PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_mat_look_to_rh(pt_math_simd_vec eye_position, pt_math_simd_vec eye_direction, pt_math_simd_vec up_direction)
{
    return directx_math_arm_neon_mat_look_to_rh(eye_position, eye_direction, up_direction);
}

PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_mat_perspective_fov_rh(float fov_angle_y, float aspect_ratio, float near_z, float far_z)
{
    return directx_math_arm_neon_mat_perspective_fov_rh(fov_angle_y, aspect_ratio, near_z, far_z);
}

PT_ATTR_MATH void PT_VECTORCALL pt_math_store_alignas16_mat4x4(pt_math_alignas16_mat4x4 *destination, pt_math_simd_mat m)
{
    return directx_math_arm_neon_store_alignas16_mat4x4(destination, m);
}

PT_ATTR_MATH void PT_VECTORCALL pt_math_store_mat4x4(pt_math_mat4x4 *destination, pt_math_simd_mat m)
{
    return directx_math_arm_neon_store_mat4x4(destination, m);
}
#else
#error Unknown Architecture
#endif
