#ifndef _MCRT_ATOMIC_H_
#define _MCRT_ATOMIC_H_ 1

#include "pt_common.h"
#include "pt_mcrt_common.h"

#include <stddef.h>
#include <stdint.h>

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

#if defined PT_POSIX
#include "pt_mcrt_atomic_posix.inl"
#elif defined PT_WIN32
#include "pt_mcrt_atomic_win32.inl"
#else
#error Unknown Platform
#endif

#endif