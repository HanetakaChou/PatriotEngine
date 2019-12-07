#ifndef PT_SYSTEM_MEMORY_H
#define PT_SYSTEM_MEMORY_H

#include "../PTCommon.h"
#include "PTSCommon.h"

#include <stddef.h>
#include <stdint.h>

extern "C" PTMCRTAPI void PTCALL PTS_MemoryZero(void *pBuf, size_t Count);

extern "C" PTMCRTAPI void PTCALL PTS_MemoryCopy(void *pOutBuf, void const *pInBuf, size_t Count);

#endif
