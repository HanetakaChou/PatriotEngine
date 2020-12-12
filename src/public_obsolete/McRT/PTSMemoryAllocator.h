#ifndef PT_SYSTEM_MEMORYALLOCATOR_H
#define PT_SYSTEM_MEMORYALLOCATOR_H

#include "../PTCommon.h"
#include "PTSCommon.h"

#include <stddef.h>
#include <stdint.h>

extern "C" PTMCRTAPI bool PTCALL PTSMemoryAllocator_Initialize();

extern "C" PTMCRTAPI void * PTCALL PTSMemoryAllocator_Alloc(uint32_t Size);

extern "C" PTMCRTAPI void PTCALL PTSMemoryAllocator_Free(void *pVoid);

extern "C" PTMCRTAPI void * PTCALL PTSMemoryAllocator_Alloc_Aligned(uint32_t Size, uint32_t Alignment);

extern "C" PTMCRTAPI void PTCALL PTSMemoryAllocator_Free_Aligned(void *pVoid);

extern "C" PTMCRTAPI uint32_t PTCALL PTSMemoryAllocator_Size(void *pVoid);

extern "C" PTMCRTAPI void * PTCALL PTSMemoryAllocator_Realloc(void *pVoid, uint32_t Size);

extern "C" PTMCRTAPI void * PTCALL PTSMemoryAllocator_Realloc_Aligned(void *pVoid, uint32_t Size, uint32_t Alignment);

//McRT(multi-core runtime)

inline void *PT_McRT_Malloc(uint32_t Size)
{
	return PTSMemoryAllocator_Alloc(Size);
}

inline void *PT_McRT_Realloc(void *pVoid, uint32_t Size)
{
	return PTSMemoryAllocator_Realloc(pVoid, Size);
}

inline void PT_McRT_Free(void *pVoid)
{
	return PTSMemoryAllocator_Free(pVoid);
}

inline void *PT_McRT_Aligned_Malloc(uint32_t Size, uint32_t Alignment)
{
	return PTSMemoryAllocator_Alloc_Aligned(Size, Alignment);
}

inline void PT_McRT_Aligned_Free(void *pVoid)
{
	return PTSMemoryAllocator_Free_Aligned(pVoid);
}

#endif