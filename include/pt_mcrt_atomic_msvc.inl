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

#if defined(_PT_MCRT_ATOMIC_H_) && !defined(_PT_MCRT_ATOMIC_MSVC_INL_)
#define _PT_MCRT_ATOMIC_MSVC_INL_ 1

#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN 1
#define NOGDICAPMASKS 1
#define NOVIRTUALKEYCODES 1
#define NOWINMESSAGES 1
#define NOWINSTYLES 1
#define NOSYSMETRICS 1
#define NOMENUS 1
#define NOICONS 1
#define NOKEYSTATES 1
#define NOSYSCOMMANDS 1
#define NORASTEROPS 1
#define NOSHOWWINDOW 1
#define NOATOM 1
#define NOCLIPBOARD 1
#define NOCOLOR 1
#define NOCTLMGR 1
#define NODRAWTEXT 1
#define NOGDI 1
#define NOKERNEL 1
#define NOUSER 1
#define NONLS 1
#define NOMB 1
#define NOMEMMGR 1
#define NOMETAFILE 1
#define NOMINMAX 1
#define NOMSG 1
#define NOOPENFILE 1
#define NOSCROLL 1
#define NOSERVICE 1
#define NOSOUND 1
#define NOTEXTMETRIC 1
#define NOWH 1
#define NOWINOFFSETS 1
#define NOCOMM 1
#define NOKANJI 1
#define NOHELP 1
#define NOPROFILER 1
#define NODEFERWINDOWPOS 1
#define NOMCX 1
#include <Windows.h>

#include <intrin.h>

inline int32_t mcrt_atomic_cas_i32(int32_t volatile *dest, int32_t exch, int32_t comp)
{
    return InterlockedCompareExchange(reinterpret_cast<LONG volatile *>(dest), static_cast<LONG>(exch), static_cast<LONG>(comp));
}

inline uint32_t mcrt_atomic_cas_u32(uint32_t volatile *dest, uint32_t exch, uint32_t comp)
{
    return InterlockedCompareExchange(reinterpret_cast<LONG volatile *>(dest), static_cast<LONG>(exch), static_cast<LONG>(comp));
}

inline int64_t mcrt_atomic_cas_i64(int64_t volatile *dest, int64_t exch, int64_t comp)
{
    return InterlockedCompareExchange64(reinterpret_cast<LONGLONG volatile *>(dest), static_cast<LONGLONG>(exch), static_cast<LONGLONG>(comp));
}

inline uint64_t mcrt_atomic_cas_u64(uint64_t volatile *dest, uint64_t exch, uint64_t comp)
{
    return InterlockedCompareExchange64(reinterpret_cast<LONGLONG volatile *>(dest), static_cast<LONGLONG>(exch), static_cast<LONGLONG>(comp));
}

template <typename T>
inline T* mcrt_atomic_cas_ptr(T* volatile* dest, T* exch, T* comp)
{
    return static_cast<T*>(_InterlockedCompareExchangePointer(reinterpret_cast<void* volatile*>(dest), static_cast<void*>(exch), static_cast<void*>(comp)));
}

inline int32_t mcrt_atomic_xchg_i32(int32_t volatile *dest, int32_t exch)
{
    return InterlockedExchange(reinterpret_cast<LONG volatile *>(dest), static_cast<LONG>(exch));
}

inline uint32_t mcrt_atomic_xchg_u32(uint32_t volatile *dest, uint32_t exch)
{
    return InterlockedExchange(reinterpret_cast<LONG volatile *>(dest), static_cast<LONG>(exch));
}

inline int64_t mcrt_atomic_xchg_i64(int64_t volatile *dest, int64_t exch)
{
    return InterlockedExchange64(reinterpret_cast<LONGLONG volatile *>(dest), static_cast<LONGLONG>(exch));
}

template <typename T>
inline T *mcrt_atomic_xchg_ptr(T *volatile *dest, T *exch)
{
    return static_cast<T *>(_InterlockedExchangePointer(reinterpret_cast<void * volatile *>(dest), static_cast<void *>(exch)));
}

inline int32_t mcrt_atomic_add_i32(int32_t volatile *dest, int32_t add)
{
    return InterlockedAdd(reinterpret_cast<LONG volatile *>(dest), static_cast<LONGLONG>(add));
}

inline int64_t mcrt_atomic_add_i64(int64_t volatile *dest, int64_t add)
{
    return InterlockedAdd64(reinterpret_cast<LONGLONG volatile *>(dest), static_cast<LONGLONG>(add));
}

inline int32_t mcrt_atomic_inc_i32(int32_t volatile *dest)
{
    return InterlockedIncrement(reinterpret_cast<LONG volatile *>(dest));
}

inline uint32_t mcrt_atomic_inc_u32(uint32_t volatile *dest)
{
    return InterlockedIncrement(reinterpret_cast<LONG volatile *>(dest));
}

inline int64_t mcrt_atomic_inc_i64(int64_t volatile *dest)
{
    return InterlockedIncrement64(reinterpret_cast<LONGLONG volatile *>(dest));
}

inline int32_t mcrt_atomic_dec_i32(int32_t volatile *dest)
{
    return InterlockedDecrement(reinterpret_cast<LONG volatile *>(dest));
}

inline uint32_t mcrt_atomic_dec_u32(uint32_t volatile *dest)
{
    return InterlockedDecrement(reinterpret_cast<LONG volatile *>(dest));
}

inline int64_t mcrt_atomic_dec_i64(int64_t volatile *dest)
{
    return InterlockedDecrement64(reinterpret_cast<LONGLONG volatile *>(dest));
}

inline int32_t mcrt_atomic_fetch_add_i32(int32_t volatile *dest, int32_t add)
{
    return InterlockedExchangeAdd(reinterpret_cast<LONG volatile *>(dest), static_cast<LONGLONG>(add));
}

inline int64_t mcrt_atomic_fetch_add_i64(int64_t volatile *dest, int64_t add)
{
    return InterlockedExchangeAdd64(reinterpret_cast<LONGLONG volatile *>(dest), static_cast<LONGLONG>(add));
}

inline void mcrt_compiler_read_barrier()
{
	_ReadBarrier();
}

inline void mcrt_compiler_write_barrier()
{
    _WriteBarrier();
}

inline void mcrt_compiler_read_write_barrier()
{
    _ReadWriteBarrier();
}

#if defined(PT_ARM) || defined(PT_ARM64)
inline void mcrt_hardware_read_barrier()
{
	__dmb(_ARM_BARRIER_SY);
}
inline void mcrt_hardware_write_barrier()
{
	__dmb(_ARM_BARRIER_ST);
}
inline void mcrt_hardware_read_write_barrier()
{
	__dmb(_ARM_BARRIER_SY);
}
#elif defined(PT_X86) || defined(PT_X64)
inline void mcrt_hardware_read_barrier()
{
	_mm_mfence();
}
inline void mcrt_hardware_write_barrier()
{
	_mm_mfence();
}
inline void mcrt_hardware_read_write_barrier()
{
	_mm_mfence();
}
#else
#error Unknown Architecture
#endif

#else
#error "Never use <pt_mcrt_atomic_msvc.inl> directly; include <pt_mcrt_atomic.h> instead."
#endif