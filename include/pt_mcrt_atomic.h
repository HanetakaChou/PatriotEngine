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

#ifndef _PT_MCRT_ATOMIC_H_
#define _PT_MCRT_ATOMIC_H_ 1

#include <stddef.h>
#include <stdint.h>
#include "pt_common.h"
#include "pt_mcrt_common.h"

inline int32_t mcrt_atomic_cas_i32(int32_t volatile *dest, int32_t exch, int32_t comp);

inline uint32_t mcrt_atomic_cas_u32(uint32_t volatile *dest, uint32_t exch, uint32_t comp);

inline int64_t mcrt_atomic_cas_i64(int64_t volatile *dest, int64_t exch, int64_t comp);

inline uint64_t mcrt_atomic_cas_u64(uint64_t volatile *dest, uint64_t exch, uint64_t comp);

template <typename T>
inline T *mcrt_atomic_cas_ptr(T *volatile *dest, T *exch, T *comp);

inline int32_t mcrt_atomic_xchg_i32(int32_t volatile *dest, int32_t exch);

inline uint32_t mcrt_atomic_xchg_u32(uint32_t volatile *dest, uint32_t exch);

inline int64_t mcrt_atomic_xchg_i64(int64_t volatile *dest, int64_t exch);

template <typename T>
inline T *mcrt_atomic_xchg_ptr(T *volatile *dest, T *exch);

inline int32_t mcrt_atomic_add_i32(int32_t volatile *dest, int32_t add);
inline int64_t mcrt_atomic_add_i64(int64_t volatile *dest, int64_t add);
inline int32_t mcrt_atomic_inc_i32(int32_t volatile *dest);
inline uint32_t mcrt_atomic_inc_u32(uint32_t volatile *dest);
inline int64_t mcrt_atomic_inc_i64(int64_t volatile *dest);
inline int32_t mcrt_atomic_dec_i32(int32_t volatile *dest);
inline uint32_t mcrt_atomic_dec_u32(uint32_t volatile *dest);
inline int64_t mcrt_atomic_dec_i64(int64_t volatile *dest);
inline int32_t mcrt_atomic_fetch_add_i32(int32_t volatile *dest, int32_t add);
inline int64_t mcrt_atomic_fetch_add_i64(int64_t volatile *dest, int64_t add);

// A load operation performs the acquire operation on the affected memory location: 
// no reads or writes in the current thread can be reordered before this load. 
// All writes in other threads that release the same atomic variable are visible in the current thread.
template <typename T>
inline T mcrt_atomic_load(T volatile *src);

// A store operation performs the release operation: 
// no reads or writes in the current thread can be reordered after this store. 
// All writes in the current thread are visible in other threads that acquire the same atomic variable.
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