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

#ifndef _MCRT_MALLOC_H
#define _MCRT_MALLOC_H 1

#include "pt_common.h"
#include "pt_mcrt_common.h"

PT_MCRT_ATTR void *mcrt_malloc(size_t size);
PT_MCRT_ATTR void *mcrt_calloc(size_t nobj, size_t size);
PT_MCRT_ATTR void mcrt_free(void *ptr);
PT_MCRT_ATTR void *mcrt_realloc(void *ptr, size_t size);
PT_MCRT_ATTR void *mcrt_aligned_malloc(size_t size, size_t alignment);
PT_MCRT_ATTR void *mcrt_aligned_realloc(void *ptr, size_t size, size_t alignment);
PT_MCRT_ATTR size_t mcrt_msize(void *ptr);

#endif