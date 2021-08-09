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

#ifndef _PT_MATH_H_
#define _PT_MATH_H_ 1

#include <pt_math_common.h>

struct math_mat4x4
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
};

struct alignas(16) math_alignas16_mat4x4 : public math_mat4x4
{
};

#if defined(PT_X64) || defined(PT_X86)
#include <xmmintrin.h>
typedef __m128 math_simd_vec;
#elif defined(PT_ARM64) || defined(PT_ARM)
#include <arm_neon.h>
typedef float32x4_t math_simd_vec;
#else
#error Unknown Architecture
#endif

struct math_simd_mat
{
    math_simd_vec r[4];
};

#ifdef __cplusplus
extern "C"
{
#endif
    PT_ATTR_MATH math_simd_mat PT_VECTORCALL math_mat_perspective_fov_rh(float fov_angle_y, float aspect_ratio, float near_z, float far_z);
    PT_ATTR_MATH void PT_VECTORCALL math_store_alignas16_mat4x4(math_alignas16_mat4x4 *destination, math_simd_mat m);
#ifdef __cplusplus
}
#endif

#endif