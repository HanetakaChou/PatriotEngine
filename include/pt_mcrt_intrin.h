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

#ifndef _PT_MCRT_INTRIN_H_
#define _PT_MCRT_INTRIN_H_ 1

#include <stddef.h>
#include <stdint.h>
#include "pt_common.h"
#include "pt_mcrt_common.h"

inline void mcrt_intrin_cpuidex(uint32_t cpuInfo[4], uint32_t function_id, uint32_t subfunction_id);

template <typename T>
inline T mcrt_intrin_round_down(T value, T alignment);

template <typename T>
inline T mcrt_intrin_round_up(T value, T alignment);

#if defined(PT_X64) || defined(PT_X86)
#include <emmintrin.h>
typedef __m128i mcrt_uuid;
#elif defined(PT_ARM64) || defined(PT_ARM)
#include <arm_neon.h>
typedef uint32x4_t mcrt_uuid;
#else
#error Unknown Architecture
#endif

#ifdef __cplusplus
extern "C"
{
#endif
    PT_ATTR_MCRT mcrt_uuid PT_CALL mcrt_uuid_load(uint8_t bytes[16]);
    PT_ATTR_MCRT bool PT_CALL mcrt_uuid_equal(mcrt_uuid a, mcrt_uuid b);
#ifdef __cplusplus
}
#endif

#if defined PT_GCC
#include "pt_mcrt_intrin_gcc.inl"
#elif defined PT_MSVC
#include "pt_mcrt_intrin_msvc.inl"
#else
#error Unknown Compiler
#endif

#include "pt_mcrt_intrin.inl"

#endif