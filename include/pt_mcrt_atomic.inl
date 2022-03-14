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

#if defined(_PT_MCRT_ATOMIC_H_) && !defined(_PT_MCRT_ATOMIC_INL_)
#define _PT_MCRT_ATOMIC_INL_ 1

#if defined(PT_ARM) || defined(PT_ARM64)
inline void mcrt_atomic_acquire_barrier()
{
    return mcrt_hardware_read_barrier();
}
inline void mcrt_atomic_release_barrier()
{
    return mcrt_hardware_write_barrier();
}
inline void mcrt_atomic_acquire_release_barrier()
{
    return mcrt_hardware_read_write_barrier();
}
#elif defined(PT_X86) || defined(PT_X64)
inline void mcrt_atomic_acquire_barrier()
{
    return mcrt_compiler_read_barrier();
}
inline void mcrt_atomic_release_barrier()
{
    return mcrt_compiler_write_barrier();
}
inline void mcrt_atomic_acquire_release_barrier()
{
    return mcrt_compiler_read_write_barrier();
}
#else
#error Unknown Architecture
#endif

template <typename T>
inline T mcrt_atomic_load(T volatile *src)
{
    T loaded_value = (*src);
    mcrt_atomic_acquire_barrier();
    return loaded_value;
}

template <typename T>
inline void mcrt_atomic_store(T volatile *dst, T val)
{
    mcrt_atomic_release_barrier();
    (*dst) = val;
}

#else
#error "Never use <pt_mcrt_atomic.inl> directly; include <pt_mcrt_atomic.h> instead."
#endif
