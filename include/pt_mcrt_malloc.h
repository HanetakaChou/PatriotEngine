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

#ifndef _PT_MCRT_MALLOC_H_
#define _PT_MCRT_MALLOC_H_ 1

#include <stddef.h>
#include "pt_common.h"
#include "pt_mcrt_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

    PT_ATTR_MCRT void *PT_CALL mcrt_malloc(size_t size);
    PT_ATTR_MCRT void *PT_CALL mcrt_calloc(size_t nobj, size_t size);
    PT_ATTR_MCRT void PT_CALL mcrt_free(void *ptr);
    PT_ATTR_MCRT void *PT_CALL mcrt_realloc(void *ptr, size_t size);
    PT_ATTR_MCRT void *PT_CALL mcrt_aligned_malloc(size_t size, size_t alignment);
    PT_ATTR_MCRT void *PT_CALL mcrt_aligned_realloc(void *ptr, size_t size, size_t alignment);
    PT_ATTR_MCRT size_t PT_CALL mcrt_msize(void *ptr);

#ifdef __cplusplus
}
#endif

#endif