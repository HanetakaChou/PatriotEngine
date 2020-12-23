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

#if defined(PT_X64) || defined(PT_X86)
#include <cpuid.h>
#elif defined(PT_ARM64) || defined(PT_ARM)

#else
#error Unknown Architecture
#endif

inline int mcrt_intrin_popcount(uint32_t value)
{
    return __builtin_popcount(value);
}

inline int __mcrt_internal_intrin_popcount64(unsigned long value)
{
    return __builtin_popcountl(value);
}

inline int __mcrt_internal_intrin_popcount64(unsigned long long value)
{
    return __builtin_popcountll(value);
}

inline int mcrt_intrin_popcount(uint64_t value)
{
    return __mcrt_internal_intrin_popcount64(value);
}

#if defined(PT_X64) || defined(PT_X86)
inline void mcrt_intrin_cpuidex(uint32_t cpuInfo[4], uint32_t function_id, uint32_t subfunction_id)
{
    __cpuid_count(function_id, subfunction_id, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    return;
}
#elif defined(PT_ARM64) || defined(PT_ARM)
inline void mcrt_intrin_cpuidex(uint32_t cpuInfo[4], uint32_t function_id, uint32_t subfunction_id)
{
    assert(false);
    return;
}
#else
#error Unknown Architecture
#endif