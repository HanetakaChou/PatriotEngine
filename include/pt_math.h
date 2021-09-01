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

#ifndef _PT_MATH_H_
#define _PT_MATH_H_ 1

#include <pt_math_common.h>

typedef struct pt_math_vec3
{
    float x;
    float y;
    float z;
} pt_math_vec3;

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
}pt_math_alignas16_mat4x4;

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
}pt_math_simd_mat;

#ifdef __cplusplus
extern "C"
{
#endif
    PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_load_vec3(pt_math_vec3 const *source);
    PT_ATTR_MATH pt_math_simd_vec PT_VECTORCALL pt_math_load_vec4(pt_math_vec4 const *source);
    PT_ATTR_MATH void PT_VECTORCALL pt_math_store_vec3(pt_math_vec3 *source, pt_math_simd_vec v);
    PT_ATTR_MATH void PT_VECTORCALL pt_math_store_vec4(pt_math_vec4 *source, pt_math_simd_vec v);
    PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_load_alignas16_mat4x4(pt_math_alignas16_mat4x4 const *source);
    PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_load_mat4x4(pt_math_mat4x4 const *source);
    PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_mat_identity(void);
    PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_mat_multiply(pt_math_simd_mat m1, pt_math_simd_mat m2);
    PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_mat_look_to_rh(pt_math_simd_vec eye_position, pt_math_simd_vec eye_direction, pt_math_simd_vec up_direction);
    PT_ATTR_MATH pt_math_simd_mat PT_VECTORCALL pt_math_mat_perspective_fov_rh(float fov_angle_y, float aspect_ratio, float near_z, float far_z);
    PT_ATTR_MATH void PT_VECTORCALL pt_math_store_alignas16_mat4x4(pt_math_alignas16_mat4x4 *destination, pt_math_simd_mat m);
    PT_ATTR_MATH void PT_VECTORCALL pt_math_store_mat4x4(pt_math_mat4x4 *destination, pt_math_simd_mat m);
#ifdef __cplusplus
}
#endif

#endif
