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

#include <stddef.h>
#include <stdint.h>
#include <pt_math_common.h>

typedef struct pt_math_vec2
{
    union
    {
        struct
        {
            float x;
            float y;
        };
        float v[2];
    };
} pt_math_vec2;


typedef struct pt_math_vec3
{
    union
    {
        struct
        {
            float x;
            float y;
            float z;
        };
        float v[3];
    };
} pt_math_vec3;

typedef struct PT_ALIGN_AS(16) pt_math_alignas16_vec3
{
    float x;
    float y;
    float z;
} pt_math_alignas16_vec3;

typedef struct pt_math_vec4
{
    float x;
    float y;
    float z;
    float w;
} pt_math_vec4;

typedef struct PT_ALIGN_AS(16) pt_math_alignas16_vec4
{
    float x;
    float y;
    float z;
    float w;
} pt_math_alignas16_vec4;

typedef struct PT_ALIGN_AS(16) pt_math_alignas16_ivec3
{
    union
    {
        struct
        {
            int x;
            int y;
            int z;
        };
        int v[3];
    };
} pt_math_alignas16_ivec3;


typedef struct pt_math_mat4x4
{
    union
    {
        struct
        {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };
        float m[4][4];
    };
} pt_math_mat4x4;

typedef struct PT_ALIGN_AS(16) pt_math_alignas16_mat4x4
{
    union
    {
        struct
        {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };
        float m[4][4];
    };
} pt_math_alignas16_mat4x4;

#if defined(PT_X64) || defined(PT_X86)
#include <xmmintrin.h>
typedef __m128 pt_math_simd_vec;
#elif defined(PT_ARM64) || defined(PT_ARM)
#include <arm_neon.h>
typedef float32x4_t pt_math_simd_vec;
#else
#error Unknown Architecture
#endif

typedef struct pt_math_simd_mat
{
    pt_math_simd_vec r[4];
} pt_math_simd_mat;

typedef struct pt_math_bounding_sphere
{
    pt_math_vec3 m_center;
    float m_radius;
} pt_math_bounding_sphere;

typedef struct pt_math_bounding_box
{
    pt_math_vec3 m_center;
    pt_math_vec3 m_extents;
} pt_math_bounding_box;

// [Reversed-Z](https://developer.nvidia.com/content/depth-precision-visualized)
#define PT_MATH_Z_FARTHEST 0.0f
#define PT_MATH_Z_NEARER_VK VK_COMPARE_OP_GREATER

#ifdef __cplusplus
extern "C"
{
#endif
     // vector3_transform(_coord) - the w component of the result is available
    // vector3_transform - not divide w
    // vector3_transform_coord - divide w
    //
    // vector3_normalize - vec.xyzw /= length(vec.xyz) - the w component will be divided by the length as well
    //

    PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_load_alignas16_vec3(pt_math_alignas16_vec3 const *source);
    PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_load_alignas16_vec4(pt_math_alignas16_vec4 const *source);
    PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_load_alignas16_ivec3(pt_math_alignas16_ivec3 const* source);
    PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec_zero(void);
    PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec_replicate(float value);
    PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec3_transform(pt_math_simd_vec v, pt_math_simd_mat m);
    PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec3_transform_coord(pt_math_simd_vec v, pt_math_simd_mat m);
    PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec3_normalize(pt_math_simd_vec v);
    PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec3_dot(pt_math_simd_vec v1, pt_math_simd_vec v2);
    PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec4_dot(pt_math_simd_vec v1, pt_math_simd_vec v2);
    PT_ATTR_MATH float PT_VECTORCALL pt_math_vec_get_x(pt_math_simd_vec v);
    PT_ATTR_MATH float PT_VECTORCALL pt_math_vec_get_y(pt_math_simd_vec v);
    PT_ATTR_MATH float PT_VECTORCALL pt_math_vec_get_z(pt_math_simd_vec v);
    PT_ATTR_MATH float PT_VECTORCALL pt_math_vec_get_w(pt_math_simd_vec v);
    PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec_abs(pt_math_simd_vec v);
    PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec_splat_w(pt_math_simd_vec v);
    PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec_convert_int_to_float(pt_math_simd_vec vint, uint32_t div_exponent);
    PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec_convert_float_to_int(pt_math_simd_vec vfloat, uint32_t div_exponent);
    PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec_min(pt_math_simd_vec v1, pt_math_simd_vec v2);
    PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec_add(pt_math_simd_vec v1, pt_math_simd_vec v2);
    PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec_subtract(pt_math_simd_vec v1, pt_math_simd_vec v2);
    PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec_multiply(pt_math_simd_vec v1, pt_math_simd_vec v2);
    PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_vec_divide(pt_math_simd_vec v1, pt_math_simd_vec v2);
    PT_ATTR_MATH void PT_VECTORCALL pt_math_store_alignas16_vec3(pt_math_alignas16_vec3 *destination, pt_math_simd_vec v);
    PT_ATTR_MATH void PT_VECTORCALL pt_math_store_alignas16_vec4(pt_math_alignas16_vec4 *destination, pt_math_simd_vec v);
    PT_ATTR_MATH void PT_VECTORCALL pt_math_store_alignas16_ivec3(pt_math_alignas16_ivec3* destination, pt_math_simd_vec v);
    PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_mat_rotation_y(float angle);
    PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_mat_look_to_rh(pt_math_simd_vec eye_position, pt_math_simd_vec eye_direction, pt_math_simd_vec up_direction);
    PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_mat_perspective_fov_rh(float fov_angle_y, float aspect_ratio, float near_z, float far_z);
    PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_mat_multiply(pt_math_simd_mat m1, pt_math_simd_mat m2);
    PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_mat_transpose(pt_math_simd_mat m);
    PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_mat_inverse(pt_math_simd_vec *determinant, pt_math_simd_mat m);

    PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_load_vec3(pt_math_vec3 const *source);
    PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_load_vec4(pt_math_vec4 const *source);
    PT_ATTR_MATH void PT_VECTORCALL pt_math_store_vec3(pt_math_vec3 *destination, pt_math_simd_vec v);
    PT_ATTR_MATH void PT_VECTORCALL pt_math_store_vec4(pt_math_vec4 *destination, pt_math_simd_vec v);
    PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_load_alignas16_mat4x4(pt_math_alignas16_mat4x4 const *source);
    PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_load_mat4x4(pt_math_mat4x4 const *source);
    PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_mat_identity(void);
    PT_ATTR_MATH void PT_VECTORCALL pt_math_store_alignas16_mat4x4(pt_math_alignas16_mat4x4 *destination, pt_math_simd_mat m);
    PT_ATTR_MATH void PT_VECTORCALL pt_math_store_mat4x4(pt_math_mat4x4 *destination, pt_math_simd_mat m);

    PT_ATTR_MATH pt_math_bounding_sphere PT_VECTORCALL pt_math_bounding_sphere_create(pt_math_vec3 const *center, float radius);
    PT_ATTR_MATH bool PT_VECTORCALL pt_math_bounding_sphere_intersect_ray(pt_math_bounding_sphere const *bounding_sphere, pt_math_simd_vec origin, pt_math_simd_vec direction, float *dist);
    PT_ATTR_MATH pt_math_bounding_box PT_VECTORCALL pt_math_bounding_box_create_from_sphere(pt_math_bounding_sphere const *bounding_sphere);
    PT_ATTR_MATH pt_math_bounding_box PT_VECTORCALL pt_math_bounding_box_create_merged(pt_math_bounding_box const *b1, pt_math_bounding_box const *b2);
    PT_ATTR_MATH pt_math_bounding_box PT_VECTORCALL pt_math_bounding_box_create_from_points(size_t count, pt_math_vec3 const *points, size_t stride);
#ifdef __cplusplus
}
#endif

#endif
