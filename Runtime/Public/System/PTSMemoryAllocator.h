#ifndef PT_SYSTEM_MEMORYALLOCATOR_H
#define PT_SYSTEM_MEMORYALLOCATOR_H

#include "../PTCommon.h"
#include "PTSCommon.h"

extern "C" PTSYSTEMAPI PTBOOL PTCALL PTSMemoryAllocator_Initialize();

extern "C" PTSYSTEMAPI void * PTCALL PTSMemoryAllocator_Alloc(uint32_t Size);

extern "C" PTSYSTEMAPI void PTCALL PTSMemoryAllocator_Free(void *pVoid);

extern "C" PTSYSTEMAPI void * PTCALL PTSMemoryAllocator_Realloc(void *pVoid, uint32_t Size);

extern "C" PTSYSTEMAPI void * PTCALL PTSMemoryAllocator_Alloc_Aligned(uint32_t Size, uint32_t Alignment);

extern "C" PTSYSTEMAPI void PTCALL PTSMemoryAllocator_Free_Aligned(void *pVoid);

#endif