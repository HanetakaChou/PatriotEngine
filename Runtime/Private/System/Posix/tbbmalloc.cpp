#include "../../../Public/System/PTSMemoryAllocator.h"

#include <stddef.h>
#include <assert.h>

static struct PTS_INITONCE
{
	inline PTS_INITONCE()
	{
		PTBOOL tbResult = ::PTSMemoryAllocator_Initialize();
		assert(tbResult != PTFALSE);
	}
}s_PTS_InitOnce;

#define TBBAPI __attribute__((visibility("default")))
#define TBBCALL

extern "C" TBBAPI void * TBBCALL scalable_malloc(size_t size)
{
	return ::PTSMemoryAllocator_Alloc(static_cast<uint32_t>(size));
}

extern "C" TBBAPI void TBBCALL scalable_free(void* ptr)
{
	return ::PTSMemoryAllocator_Free(ptr);
}

extern "C" TBBAPI void * TBBCALL scalable_aligned_malloc(size_t size, size_t alignment)
{
	return ::PTSMemoryAllocator_Alloc_Aligned(static_cast<uint32_t>(size), static_cast<uint32_t>(alignment));
}

extern "C" TBBAPI void TBBCALL scalable_aligned_free(void* ptr)
{
	return ::PTSMemoryAllocator_Free_Aligned(ptr);
}
