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

#if defined(_PT_MCRT_ATOMIC_H_) && !defined(_PT_MCRT_ATOMIC_GCC_INL_)
#define _PT_MCRT_ATOMIC_GCC_INL_ 1

inline int32_t mcrt_atomic_cas_i32(int32_t volatile *dest, int32_t exch, int32_t comp)
{
    return __sync_val_compare_and_swap(dest, comp, exch);
}

inline uint32_t mcrt_atomic_cas_u32(uint32_t volatile *dest, uint32_t exch, uint32_t comp)
{
    return __sync_val_compare_and_swap(dest, comp, exch);
}

inline int64_t mcrt_atomic_cas_i64(int64_t volatile *dest, int64_t exch, int64_t comp)
{
    return __sync_val_compare_and_swap(dest, comp, exch);
}

inline uint64_t mcrt_atomic_cas_u64(uint64_t volatile *dest, uint64_t exch, uint64_t comp)
{
    return __sync_val_compare_and_swap(dest, comp, exch);
}

template <typename T>
inline T *mcrt_atomic_cas_ptr(T *volatile *dest, T *exch, T *comp)
{
    uintptr_t dest_old = __sync_val_compare_and_swap(reinterpret_cast<uintptr_t volatile *>(dest), reinterpret_cast<uintptr_t>(comp), reinterpret_cast<uintptr_t>(exch));
    return reinterpret_cast<T *>(dest_old);
}

inline int32_t mcrt_atomic_xchg_i32(int32_t volatile *dest, int32_t exch)
{
    // https://gcc.gnu.org/onlinedocs/gcc-4.6.4/gcc/Atomic-Builtins.html#Atomic-Builtins
    // Many targets have only minimal support for such locks, and do not support a full exchange operation.
    // In this case, a target may support reduced functionality here by which the only valid value to store is the immediate constant 1.
    // The exact value actually stored in *ptr is implementation defined.

    // return __sync_lock_test_and_set(dest, exch);
    // __sync_lock_release

    int32_t dest_old;
    do
    {
        dest_old = (*dest);
    } while (__sync_val_compare_and_swap(dest, dest_old, exch) != dest_old);
    return dest_old;
}

inline uint32_t mcrt_atomic_xchg_u32(uint32_t volatile *dest, uint32_t exch)
{
    uint32_t dest_old;
    do
    {
        dest_old = (*dest);
    } while (__sync_val_compare_and_swap(dest, dest_old, exch) != dest_old);
    return dest_old;
}

inline int64_t mcrt_atomic_xchg_i64(int64_t volatile *dest, int64_t exch)
{
    int64_t dest_old;
    do
    {
        dest_old = (*dest);
    } while (__sync_val_compare_and_swap(dest, dest_old, exch) != dest_old);
    return dest_old;
}

template <typename T>
inline T *mcrt_atomic_xchg_ptr(T *volatile *dest, T *exch)
{
    uintptr_t dest_old;
    do
    {
        dest_old = (*reinterpret_cast<uintptr_t volatile *>(dest));
    } while (__sync_val_compare_and_swap(reinterpret_cast<uintptr_t volatile *>(dest), dest_old, reinterpret_cast<uintptr_t>(exch)) != dest_old);
    return reinterpret_cast<T *>(dest_old);
}

inline int32_t mcrt_atomic_add_i32(int32_t volatile *dest, int32_t add)
{
    return __sync_add_and_fetch(dest, add);
}

inline int64_t mcrt_atomic_add_i64(int64_t volatile *dest, int64_t add)
{
    return __sync_add_and_fetch(dest, add);
}

inline int32_t mcrt_atomic_inc_i32(int32_t volatile *dest)
{
    return __sync_add_and_fetch(dest, static_cast<int32_t>(1));
}

inline uint32_t mcrt_atomic_inc_u32(uint32_t volatile *dest)
{
    return __sync_add_and_fetch(dest, static_cast<uint32_t>(1));
}

inline int64_t mcrt_atomic_inc_i64(int64_t volatile *dest)
{
    return __sync_add_and_fetch(dest, static_cast<int64_t>(1));
}

inline int32_t mcrt_atomic_dec_i32(int32_t volatile *dest)
{
    return __sync_sub_and_fetch(dest, static_cast<int32_t>(1));
}

inline uint32_t mcrt_atomic_dec_u32(uint32_t volatile *dest)
{
    return __sync_sub_and_fetch(dest, static_cast<uint32_t>(1));
}

inline int64_t mcrt_atomic_dec_i64(int64_t volatile *dest)
{
    return __sync_sub_and_fetch(dest, static_cast<int64_t>(1));
}

inline int32_t mcrt_atomic_fetch_add_i32(int32_t volatile *dest, int32_t add)
{
    return __sync_fetch_and_add(dest, add);
}

inline int64_t mcrt_atomic_fetch_add_i64(int64_t volatile *dest, int64_t add)
{
    return __sync_fetch_and_add(dest, add);
}

inline void mcrt_compiler_read_barrier()
{
    __asm__ __volatile__(""
                         :
                         :
                         : "memory");
}

inline void mcrt_compiler_write_barrier()
{
    __asm__ __volatile__(""
                         :
                         :
                         : "memory");
}

inline void mcrt_compiler_read_write_barrier()
{
    __asm__ __volatile__(""
                         :
                         :
                         : "memory");
}

#if defined(PT_ARM)
inline void mcrt_hardware_read_barrier()
{
    __asm__ __volatile__("dmb sy"
                         :
                         :
                         : "memory");
}
inline void mcrt_hardware_write_barrier()
{
    __asm__ __volatile__("dmb st"
                         :
                         :
                         : "memory");
}
inline void mcrt_hardware_read_write_barrier()
{
    __asm__ __volatile__("dmb sy"
                         :
                         :
                         : "memory");
}
#elif defined(PT_ARM64)
inline void mcrt_hardware_read_barrier()
{
    __asm__ __volatile__("dmb ld"
                         :
                         :
                         : "memory");
}
inline void mcrt_hardware_write_barrier()
{
    __asm__ __volatile__("dmb st"
                         :
                         :
                         : "memory");
}
inline void mcrt_hardware_read_write_barrier()
{
    __asm__ __volatile__("dmb sy"
                         :
                         :
                         : "memory");
}
#elif defined(PT_X86)
inline void mcrt_hardware_read_barrier()
{
    __asm__ __volatile__("mfence"
                         :
                         :
                         : "memory");
}
inline void mcrt_hardware_write_barrier()
{
    __asm__ __volatile__("mfence"
                         :
                         :
                         : "memory");
}
inline void mcrt_hardware_read_write_barrier()
{
    __asm__ __volatile__("mfence"
                         :
                         :
                         : "memory");
}
#elif defined(PT_X64)
inline void mcrt_hardware_read_barrier()
{
    __asm__ __volatile__("mf"
                         :
                         :
                         : "memory");
}
inline void mcrt_hardware_write_barrier()
{
    __asm__ __volatile__("mf"
                         :
                         :
                         : "memory");
}
inline void mcrt_hardware_read_write_barrier()
{
    __asm__ __volatile__("mf"
                         :
                         :
                         : "memory");
}
#else
#error Unknown Architecture
#endif

#else
#error "Never use <pt_mcrt_atomic_gcc.inl> directly; include <pt_mcrt_atomic.h> instead."
#endif