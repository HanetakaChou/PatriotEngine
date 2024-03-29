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

#ifndef _PT_MCRT_RWLOCK_H_
#define _PT_MCRT_RWLOCK_H_ 1

#include <stddef.h>
#include <stdint.h>
#include "pt_common.h"
#include "pt_mcrt_common.h"

inline void mcrt_rwlock_init(struct mcrt_rwlock_t *rwlock);
inline void mcrt_rwlock_rdlock(struct mcrt_rwlock_t *rwlock);
inline void mcrt_rwlock_wrlock(struct mcrt_rwlock_t *rwlock);
inline void mcrt_rwlock_rdunlock(struct mcrt_rwlock_t *rwlock);
inline void mcrt_rwlock_wrunlock(struct mcrt_rwlock_t *rwlock);

inline void mcrt_assert_rwlock_init(struct mcrt_assert_rwlock_t *rwlock);
inline void mcrt_assert_rwlock_rdlock(struct mcrt_assert_rwlock_t *rwlock);
inline void mcrt_assert_rwlock_wrlock(struct mcrt_assert_rwlock_t *rwlock);
inline void mcrt_assert_rwlock_rdunlock(struct mcrt_assert_rwlock_t *rwlock);
inline void mcrt_assert_rwlock_wrunlock(struct mcrt_assert_rwlock_t *rwlock);

#include "pt_mcrt_rwlock.inl"

#endif