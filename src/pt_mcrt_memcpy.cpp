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

#include <pt_common.h>
#include <pt_mcrt_common.h>
#include <pt_mcrt_memcpy.h>

#if defined(PT_X64) || defined(PT_X86)
static bool support_avx512f = false;
static bool support_avx = false;
static bool support_ssse3 = false;
extern void *rte_memcpy_avx512f(void *__restrict dest, void const *__restrict src, size_t count);
extern void *rte_memcpy_avx(void *__restrict dest, void const *__restrict src, size_t count);
extern void *rte_memcpy_ssse3(void *__restrict dest, void const *__restrict src, size_t count);
static inline void *rte_memcpy(void *__restrict dest, void const *__restrict src, size_t count);
#elif defined(PT_ARM64)
#include "pt_mcrt_memcpy_dpdk_rte_memcpy_arm32.h"
#elif defined(PT_ARM)
#include "pt_mcrt_memcpy_dpdk_rte_memcpy_arm64.h"
#else
#endif

#include <pt_mcrt_intrin.h>

PT_ATTR_MCRT bool PT_CALL mcrt_memcpy(void *__restrict dest, void const *__restrict src, size_t count)
{
    rte_memcpy(dest, src, count);

    return true;
}

#if defined(PT_X64) || defined(PT_X86)
static inline void *rte_memcpy(void *__restrict dest, void const *__restrict src, size_t count)
{
    if (support_avx512f)
    {
        return rte_memcpy_avx512f(dest, src, count);
    }
    else if (support_avx)
    {
        return rte_memcpy_avx(dest, src, count);
    }
    else if (support_ssse3)
    {
        return rte_memcpy_ssse3(dest, src, count);
    }
    else
    {
        return NULL;
    }
}

static class mcrt_intrin_cpuidex_guard_t
{
public:
    inline mcrt_intrin_cpuidex_guard_t()
    {
        uint32_t cpui[4];
        mcrt_intrin_cpuidex(cpui, 0, 0);
        uint32_t nIds = cpui[0];

        if (nIds >= 1)
        {
            mcrt_intrin_cpuidex(cpui, 1, 0);
            uint32_t f_1_ECX_ = cpui[2];
            support_ssse3 = ((f_1_ECX_ & (1U << 9U)) ? true : false);
            support_avx = ((f_1_ECX_ & (1U << 28U)) ? true : false);
        }

        if (nIds >= 7)
        {
            mcrt_intrin_cpuidex(cpui, 1, 0);
            uint32_t f_7_EBX_ = cpui[1];
            support_avx512f = ((f_7_EBX_ & (1U << 16U)) ? true : false);
        }
    }
    inline ~mcrt_intrin_cpuidex_guard_t() {}
} mcrt_intrin_cpuidex_guard;
#endif