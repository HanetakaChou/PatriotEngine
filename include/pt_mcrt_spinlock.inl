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

#if defined(_PT_MCRT_SPINLOCK_H_) && !defined(_PT_MCRT_SPINLOCK_INL_)
#define _PT_MCRT_SPINLOCK_INL_ 1

#include "pt_mcrt_atomic.h"
#include "pt_mcrt_thread.h"

struct mcrt_spinlock_t
{
    uint32_t m_lock;
};

inline void mcrt_spin_init(struct mcrt_spinlock_t *lock)
{
    lock->m_lock = 0U;
}

inline void mcrt_spin_lock(struct mcrt_spinlock_t *lock)
{
    while (0U != mcrt_atomic_xchg_u32(&lock->m_lock, 1U))
    {
        mcrt_os_yield();
    }
}

inline void mcrt_spin_unlock(struct mcrt_spinlock_t *lock)
{
    mcrt_atomic_store(&lock->m_lock, 0U);
}

#else
#error "Never use <pt_mcrt_spinlock.inl> directly; include <pt_mcrt_spinlock.h> instead."
#endif