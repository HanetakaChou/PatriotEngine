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

#include "pt_mcrt_thread.h"
#include "pt_mcrt_atomic.h"
#include "pt_mcrt_malloc.h"

#include <tbb/scalable_allocator.h>

PT_MCRT_ATTR void *PT_CALL mcrt_malloc(size_t size)
{
    return scalable_malloc(size);
}

PT_MCRT_ATTR void *PT_CALL mcrt_calloc(size_t nobj, size_t size);

PT_MCRT_ATTR void PT_CALL mcrt_free(void *ptr)
{
    return scalable_free(ptr);
}

PT_MCRT_ATTR void *PT_CALL mcrt_realloc(void *ptr, size_t size);

PT_MCRT_ATTR void *PT_CALL mcrt_aligned_malloc(size_t size, size_t alignment)
{
    return scalable_aligned_malloc(size, alignment);
}

PT_MCRT_ATTR void *PT_CALL mcrt_aligned_realloc(void *ptr, size_t size, size_t alignment)
{
    return scalable_aligned_realloc(ptr, size, alignment);
}

PT_MCRT_ATTR size_t PT_CALL mcrt_msize(void *ptr);