/*
 * Copyright (C) YuqiaoZhang
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

#ifndef _MCRT_ATOMIC_H_
#define _MCRT_ATOMIC_H_ 1

#include <stddef.h>
#include <stdint.h>
#include "pt_common.h"
#include "pt_mcrt_common.h"

inline int32_t mcrt_atomic_cas_i32(int32_t volatile *dest, int32_t exch, int32_t comp);
inline int64_t mcrt_atomic_cas_i64(int64_t volatile *dest, int64_t exch, int64_t comp);
inline void *mcrt_atomic_cas_ptr(void *volatile *dest, void *exch, void *comp);
inline int32_t mcrt_atomic_xchg_i32(int32_t volatile *dest, int32_t exch);
inline int64_t mcrt_atomic_xchg_i64(int64_t volatile *dest, int64_t exch);
inline void *mcrt_atomic_xchg_ptr(void *volatile *dest, void *exch);
inline int32_t mcrt_atomic_add_i32(int32_t volatile *dest, int32_t add);
inline int64_t mcrt_atomic_add_i64(int64_t volatile *dest, int64_t add);
inline int32_t mcrt_atomic_inc_i32(int32_t volatile *dest);
inline int64_t mcrt_atomic_inc_i64(int64_t volatile *dest);
inline int32_t mcrt_atomic_dec_i32(int32_t volatile *dest);
inline int64_t mcrt_atomic_dec_i64(int64_t volatile *dest);
inline int32_t mcrt_atomic_fetch_add_i32(int32_t volatile *dest, int32_t add);
inline int64_t mcrt_atomic_fetch_add_i64(int64_t volatile *dest, int64_t add);
template <typename T>
inline T mcrt_atomic_load(T volatile *src);
template <typename T>
inline void mcrt_atomic_store(T volatile *dst, T val);

#if defined PT_GCC
#include "pt_mcrt_atomic_gcc.inl"
#elif defined PT_MSVC
#include "pt_mcrt_atomic_msvc.inl"
#else
#error Unknown Compiler
#endif

#include "pt_mcrt_atomic.inl"

#endif