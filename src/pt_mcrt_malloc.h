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