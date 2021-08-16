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
#include "pt_math_directx_math.h"

#if defined(PT_X64) || defined(PT_X86)
static bool math_support_avx2 = false;
static bool math_support_avx = false;
#elif defined(PT_ARM64) || defined(PT_ARM)
#else
#error Unknown Architecture
#endif

PT_ATTR_MATH math_simd_vec PT_VECTORCALL math_load_vec3(math_vec3 *source)
{
#if defined(PT_X64) || defined(PT_X86)
    if (math_support_avx2)
    {
        return directx_math_x86_avx2_load_vec3(source);
    }
    else if (math_support_avx)
    {
        return directx_math_x86_avx_load_vec3(source);
    }
    else
    {
        return directx_math_x86_sse2_load_vec3(source);
    }
#elif defined(PT_ARM64) || defined(PT_ARM)
        return directx_math_arm_neon_load_vec3(source);
#else
#error Unknown Architecture
#endif
}

PT_ATTR_MATH math_simd_vec PT_VECTORCALL math_load_vec4(math_vec4 *source)
{
#if defined(PT_X64) || defined(PT_X86)
    if (math_support_avx2)
    {
        return directx_math_x86_avx2_load_vec4(source);
    }
    else if (math_support_avx)
    {
        return directx_math_x86_avx_load_vec4(source);
    }
    else
    {
        return directx_math_x86_sse2_load_vec4(source);
    }
#elif defined(PT_ARM64) || defined(PT_ARM)
        return directx_math_arm_neon_load_vec4(source);
#else
#error Unknown Architecture
#endif
}

PT_ATTR_MATH void PT_VECTORCALL math_store_vec3(math_vec3 *destination, math_simd_vec v)
{
#if defined(PT_X64) || defined(PT_X86)
    if (math_support_avx2)
    {
        return directx_math_x86_avx2_store_vec3(destination, v);
    }
    else if (math_support_avx)
    {
        return directx_math_x86_avx_store_vec3(destination, v);
    }
    else
    {
        return directx_math_x86_sse2_store_vec3(destination, v);
    }
#else
    return directx_math_arm_neon_store_vec3(destination, v);
#error Unknown Architecture
#endif
}

PT_ATTR_MATH void PT_VECTORCALL math_store_vec4(math_vec4 *destination, math_simd_vec v)
{
#if defined(PT_X64) || defined(PT_X86)
    if (math_support_avx2)
    {
        return directx_math_x86_avx2_store_vec4(destination, v);
    }
    else if (math_support_avx)
    {
        return directx_math_x86_avx_store_vec4(destination, v);
    }
    else
    {
        return directx_math_x86_sse2_store_vec4(destination, v);
    }
#else
    return directx_math_arm_neon_store_vec4(destination, v);
#error Unknown Architecture
#endif
}

PT_ATTR_MATH math_simd_mat PT_VECTORCALL math_load_alignas16_mat4x4(math_alignas16_mat4x4 *source)
{
#if defined(PT_X64) || defined(PT_X86)
    if (math_support_avx2)
    {
        return directx_math_x86_avx2_load_alignas16_mat4x4(source);
    }
    else if (math_support_avx)
    {
        return directx_math_x86_avx_load_alignas16_mat4x4(source);
    }
    else
    {
        return directx_math_x86_sse2_load_alignas16_mat4x4(source);
    }
#elif defined(PT_ARM64) || defined(PT_ARM)
        return directx_math_arm_neon_load_alignas16_mat4x4(source);
#else
#error Unknown Architecture
#endif
}

PT_ATTR_MATH math_simd_mat PT_VECTORCALL math_load_mat4x4(math_mat4x4 *source)
{
#if defined(PT_X64) || defined(PT_X86)
    if (math_support_avx2)
    {
        return directx_math_x86_avx2_load_mat4x4(source);
    }
    else if (math_support_avx)
    {
        return directx_math_x86_avx_load_mat4x4(source);
    }
    else
    {
        return directx_math_x86_sse2_load_mat4x4(source);
    }
#elif defined(PT_ARM64) || defined(PT_ARM)
        return directx_math_arm_neon_load_mat4x4(source);
#else
#error Unknown Architecture
#endif
}

PT_ATTR_MATH math_simd_mat PT_VECTORCALL math_mat_identity()
{
    #if defined(PT_X64) || defined(PT_X86)
    if (math_support_avx2)
    {
        return directx_math_x86_avx2_mat_identity();
    }
    else if (math_support_avx)
    {
        return directx_math_x86_avx_mat_identity();
    }
    else
    {
        return directx_math_x86_sse2_mat_identity();
    }
#elif defined(PT_ARM64) || defined(PT_ARM)
        return directx_math_arm_neon_mat_identity();
#else
#error Unknown Architecture
#endif
}

PT_ATTR_MATH math_simd_mat PT_VECTORCALL math_mat_perspective_fov_rh(float fov_angle_y, float aspect_ratio, float near_z, float far_z)
{
#if defined(PT_X64) || defined(PT_X86)
    if (math_support_avx2)
    {
        return directx_math_x86_avx2_mat_perspective_fov_rh(fov_angle_y, aspect_ratio, near_z, far_z);
    }
    else if (math_support_avx)
    {
        return directx_math_x86_avx_mat_perspective_fov_rh(fov_angle_y, aspect_ratio, near_z, far_z);
    }
    else
    {
        return directx_math_x86_sse2_mat_perspective_fov_rh(fov_angle_y, aspect_ratio, near_z, far_z);
    }
#elif defined(PT_ARM64) || defined(PT_ARM)
        return directx_math_arm_neon_mat_perspective_fov_rh(fov_angle_y, aspect_ratio, near_z, far_z);
#else
#error Unknown Architecture
#endif
}

PT_ATTR_MATH void PT_VECTORCALL math_store_alignas16_mat4x4(math_alignas16_mat4x4 *destination, math_simd_mat m)
{
#if defined(PT_X64) || defined(PT_X86)
    if (math_support_avx2)
    {
        return directx_math_x86_avx2_store_alignas16_mat4x4(destination, m);
    }
    else if (math_support_avx)
    {
        return directx_math_x86_avx_store_alignas16_mat4x4(destination, m);
    }
    else
    {
        return directx_math_x86_sse2_store_alignas16_mat4x4(destination, m);
    }
#else
    return directx_math_arm_neon_store_alignas16_mat4x4(destination, m);
#error Unknown Architecture
#endif
}

PT_ATTR_MATH void PT_VECTORCALL math_store_mat4x4(math_mat4x4 *destination, math_simd_mat m)
{
    #if defined(PT_X64) || defined(PT_X86)
    if (math_support_avx2)
    {
        return directx_math_x86_avx2_store_mat4x4(destination, m);
    }
    else if (math_support_avx)
    {
        return directx_math_x86_avx_store_mat4x4(destination, m);
    }
    else
    {
        return directx_math_x86_sse2_store_mat4x4(destination, m);
    }
#else
    return directx_math_arm_neon_store_mat4x4(destination, m);
#error Unknown Architecture
#endif
}

#if defined(PT_X64) || defined(PT_X86)
#include <pt_mcrt_intrin.h>
struct math_verify_cpu_support
{
    inline math_verify_cpu_support()
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
        math_support_avx2 = ((f_7_EBX_ & (1U << 5U)) != 0);
        math_support_avx = ((f_1_ECX_ & (1U << 28U)) != 0);
    }
};
static struct math_verify_cpu_support instance_math_verify_cpu_support;
#elif defined(PT_ARM64) || defined(PT_ARM)
#else
#error Unknown Architecture
#endif