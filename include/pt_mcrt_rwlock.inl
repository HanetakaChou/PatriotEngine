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

#if defined(_PT_MCRT_RWLOCK_H_) && !defined(_PT_MCRT_RWLOCK_INL_)
#define _PT_MCRT_RWLOCK_INL_ 1

#include "pt_mcrt_atomic.h"
#include "pt_mcrt_thread.h"

struct mcrt_rwlock_t
{
    uint32_t m_lock;
    uint32_t m_reader_count;
};

inline void mcrt_rwlock_init(struct mcrt_rwlock_t *rwlock)
{
    rwlock->m_lock = 0U;
    rwlock->m_reader_count = 0U;
}

inline void mcrt_rwlock_rdlock(struct mcrt_rwlock_t *rwlock)
{
    while (1)
    {
        mcrt_atomic_inc_u32(&rwlock->m_reader_count);

        if (0U == mcrt_atomic_load(&rwlock->m_lock))
        {
            return;
        }

        mcrt_atomic_dec_u32(&rwlock->m_reader_count);

        while (0U != mcrt_atomic_load(&rwlock->m_lock))
        {
            mcrt_os_yield();
        }
    }
}

inline void mcrt_rwlock_wrlock(struct mcrt_rwlock_t *rwlock)
{
    while (0U != mcrt_atomic_xchg_u32(&rwlock->m_lock, 1U))
    {
        mcrt_os_yield();
    }

    while (0U != mcrt_atomic_load(&rwlock->m_reader_count))
    {
        mcrt_os_yield();
    }
}

inline void mcrt_rwlock_rdunlock(struct mcrt_rwlock_t *rwlock)
{
    mcrt_atomic_dec_u32(&rwlock->m_reader_count);
}

inline void mcrt_rwlock_wrunlock(struct mcrt_rwlock_t *rwlock)
{
    mcrt_atomic_store(&rwlock->m_lock, 0U);
}

#else
#error "Never use <pt_mcrt_rwlock.inl> directly; include <pt_mcrt_rwlock.h> instead."
#endif