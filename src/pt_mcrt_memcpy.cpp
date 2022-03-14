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

#include <stddef.h>
#include <stdint.h>
#include <pt_common.h>
#include <pt_mcrt_common.h>
#include <pt_mcrt_memcpy.h>
#include <pt_mcrt_intrin.h>

static inline void *rte_memcpy(void *__restrict dest, void const *__restrict src, size_t count);

PT_ATTR_MCRT void *PT_CALL mcrt_memcpy(void *__restrict dest, void const *__restrict src, size_t count)
{
    return rte_memcpy(dest, src, count);
}

#if defined(PT_X64) || defined(PT_X86)
#include <string.h>
extern void *rte_memcpy_x86_avx512f(void *__restrict dest, void const *__restrict src, size_t count);
extern void *rte_memcpy_x86_avx(void *__restrict dest, void const *__restrict src, size_t count);
extern void *rte_memcpy_x86_ssse3(void *__restrict dest, void const *__restrict src, size_t count);

static void *(*pfn_rte_memcpy)(void *dst, void const *src, size_t n) = NULL;

static inline void *rte_memcpy(void *__restrict dest, void const *__restrict src, size_t count)
{
    return pfn_rte_memcpy(dest, src, count);
}

struct rte_memcpy_verify_x86_cpu_support
{
    inline rte_memcpy_verify_x86_cpu_support()
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

        bool rte_memcpy_support_x86_avx512f = (((f_7_EBX_ & (1U << 16U)) != 0) ? true : false);
        bool rte_memcpy_support_x86_avx = (((f_1_ECX_ & (1U << 28U)) != 0) ? true : false);
        bool rte_memcpy_support_x86_ssse3 = (((f_1_ECX_ & (1U << 9U)) != 0) ? true : false);

        if (rte_memcpy_support_x86_avx512f)
        {
            pfn_rte_memcpy = rte_memcpy_x86_avx512f;
        }
        else if (rte_memcpy_support_x86_avx)
        {
            pfn_rte_memcpy = rte_memcpy_x86_avx;
        }
        else if (rte_memcpy_support_x86_ssse3)
        {
            pfn_rte_memcpy = rte_memcpy_x86_ssse3;
        }
        else
        {
            pfn_rte_memcpy = memcpy;
        }
    }
};
static struct rte_memcpy_verify_x86_cpu_support instance_rte_memcpy_verify_x86_cpu_support;

#elif defined(PT_ARM)

#if defined(PT_POSIX)

#if defined(PT_POSIX_LINUX)
#include <asm/hwcap.h>
#include <sys/auxv.h>
#include <string.h>
extern void *rte_memcpy_arm32_neon(void *__restrict dest, void const *__restrict src, size_t count);

static void *(*pfn_rte_memcpy)(void *dst, void const *src, size_t n) = NULL;

static inline void *rte_memcpy(void *__restrict dest, void const *__restrict src, size_t count)
{
    return pfn_rte_memcpy(dest, src, count);
}

struct rte_memcpy_verify_arm_cpu_support
{
    inline rte_memcpy_verify_arm_cpu_support()
    {
        // https://android.googlesource.com/platform/ndk/+/master/sources/android/cpufeatures/cpu-features.c
        // ANDROID_CPU_ARM_FEATURE_NEON
        // https://android.googlesource.com/platform/bionic/+/master/libc/bionic/getauxval.cpp
        // __bionic_getauxval
        
        bool rte_memcpy_support_arm32_neon = ((getauxval(AT_HWCAP) & HWCAP_NEON) != 0) ? true : false;

        if (rte_memcpy_support_arm32_neon)
        {
            pfn_rte_memcpy = rte_memcpy_arm32_neon;
        }
        else
        {
            pfn_rte_memcpy = memcpy;
        }
    }
};
static struct rte_memcpy_verify_arm_cpu_support instance_rte_memcpy_verify_arm32_cpu_support;
#elif defined(PT_POSIX_MACH)
static inline void *rte_memcpy(void *__restrict dest, void const *__restrict src, size_t count)
{
    return rte_memcpy_arm32_neon(dest, src, count);
}
#else
#error Unknown Platform
#endif

#elif defined(PT_WIN32)
#error To Be Done
#else
#error Unknown Platform
#endif

#elif defined(PT_ARM64)
extern void *rte_memcpy_arm64_neon(void *__restrict dest, void const *__restrict src, size_t count);
static inline void *rte_memcpy(void *__restrict dest, void const *__restrict src, size_t count)
{
    return rte_memcpy_arm64_neon(dest, src, count);
}
#else
#error Unknown Architecture
#endif