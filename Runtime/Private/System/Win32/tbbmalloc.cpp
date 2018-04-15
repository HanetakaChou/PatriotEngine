#include "../../../Public/System/PTSMemoryAllocator.h"

#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <assert.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		::DisableThreadLibraryCalls(hModule);
	}
	break;
	case DLL_PROCESS_DETACH:
	{
		//Do Nothing
	}
	break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	default:
		assert(0);//由于已经Disable，因此不应当再接收到消息
	}
	return TRUE;
}

static struct PTS_INITONCE
{
	inline PTS_INITONCE()
	{
		PTBOOL tbResult = ::PTSMemoryAllocator_Initialize();
		assert(tbResult != PTFALSE);
	}
}s_PTS_InitOnce;

#define __TBB_EXPORTED_FUNC   __cdecl

extern "C" void * __TBB_EXPORTED_FUNC scalable_malloc(size_t size)
{
	return ::PTSMemoryAllocator_Alloc(static_cast<uint32_t>(size));
}

extern "C" void __TBB_EXPORTED_FUNC scalable_free(void* ptr)
{
	return ::PTSMemoryAllocator_Free(ptr);
}

extern "C" void * __TBB_EXPORTED_FUNC scalable_aligned_malloc(size_t size, size_t alignment)
{
	return ::PTSMemoryAllocator_Alloc_Aligned(static_cast<uint32_t>(size), static_cast<uint32_t>(alignment));
}

extern "C" void __TBB_EXPORTED_FUNC scalable_aligned_free(void* ptr)
{
	return ::PTSMemoryAllocator_Free_Aligned(ptr);
}
