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

#if defined(_PT_MCRT_INTRIN_H_) && !defined(_PT_MCRT_INTRIN_GCC_INL_)
#define _PT_MCRT_INTRIN_GCC_INL_ 1

#include <intrin.h>

#if defined(PT_X64) || defined(PT_X86)
inline void mcrt_intrin_cpuidex(uint32_t cpu_info[4], uint32_t function_id, uint32_t subfunction_id)
{
    return __cpuidex(reinterpret_cast<int *>(cpu_info), function_id, subfunction_id);
}
#elif defined(PT_ARM64) || defined(PT_ARM)
inline void mcrt_intrin_cpuidex(uint32_t cpu_info[4], uint32_t function_id, uint32_t subfunction_id)
{
    assert(false);
    return;
}
#else
#error Unknown Architecture
#endif

#else
#error "Never use <pt_mcrt_intrin_msvc.inl> directly; include <pt_mcrt_intrin.h> instead."
#endif