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

#include <intrin.h>

inline int mcrt_intrin_popcount(uint32_t value)
{
    return __popcnt(value);
}
inline int mcrt_intrin_popcount(uint64_t value)
{
    return __popcnt64(value);
}

#if defined(PT_X64) || defined(PT_X86)
inline void mcrt_intrin_cpuidex(uint32_t cpuInfo[4], uint32_t function_id, uint32_t subfunction_id)
{
    return __cpuidex(cpuinfo, function_id, subfunction_id);
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