static inline void *PTS_MemoryMap_Alloc(uint32_t size)
{
	HANDLE hSection = ::CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0U, size, NULL);
	assert(hSection != NULL);

	void *pVoid = ::MapViewOfFile(hSection, FILE_MAP_READ | FILE_MAP_WRITE, 0U, 0U, size);

	//https://msdn.microsoft.com/en-us/library/windows/desktop/aa366537(v=vs.85).aspx
	BOOL wbResult = ::CloseHandle(hSection); //被映射的视图会增加引用计数
	assert(wbResult != FALSE);

	return pVoid;
}

static inline void PTS_MemoryMap_Free(void *pVoid)
{
	BOOL wbResult = ::UnmapViewOfFile(pVoid);
	assert(wbResult != FALSE);
}

#include <intrin.h>

static inline uint32_t PTS_Size_BitScanReverse(uint32_t Value)
{
	DWORD Index;
	BOOL wbResult = ::_BitScanReverse(&Index, Value - 1U);
	assert(wbResult != FALSE);
	return Index;
}