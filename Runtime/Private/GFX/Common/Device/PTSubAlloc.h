#ifndef PTSUBALLOC_H
#define PTSUBALLOC_H

#include "../../../../Public/System/PTMemorySystem.h"
#include <stdint.h>

#define PTSUBALLOC_INVIALD_OFFSET_VALUE (~0U)

struct IPTSubAlloc
{
	virtual void PTCALL Destroy(IPTHeap *pHeap) = 0;

	virtual uint64_t PTCALL AlignedAlloc(IPTHeap *pHeap, uint64_t Size, uint64_t Alignment) = 0;
	virtual uint64_t PTCALL AlignedRealloc(IPTHeap *pHeap, uint64_t Offset, uint64_t Size, uint64_t Alignment) = 0;
	virtual void PTCALL AlignedFree(IPTHeap *pHeap, uint64_t Offset) = 0;
};

IPTSubAlloc * PTCALL PTSubAlloc_Create(IPTHeap *pHeap, uint64_t BlockWholeSize, uint64_t BlockMinimumAlignment);

#endif

