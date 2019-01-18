#ifndef PT_SYSTEM_MEMORY_H
#define PT_SYSTEM_MEMORY_H

#include "../PTCommon.h"
#include "PTSCommon.h"

extern "C" PTSYSTEMAPI void PTCALL PTS_MemoryZero(void *pBuf, size_t Count);

extern "C" PTSYSTEMAPI void PTCALL PTS_MemoryCopy(void *pOutBuf, void const *pInBuf, size_t Count);

#endif
