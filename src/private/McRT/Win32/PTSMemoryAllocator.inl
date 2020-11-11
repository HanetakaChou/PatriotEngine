static inline void *PTS_MemoryMap_Alloc(uint32_t size)
{
	HANDLE hSection = ::CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0U, size, NULL);
	assert(hSection != NULL);

	void *pVoid = ::MapViewOfFile(hSection, FILE_MAP_READ | FILE_MAP_WRITE, 0U, 0U, size);
	assert(::PTS_Size_IsAligned(reinterpret_cast<uintptr_t>(pVoid), s_Block_Size));

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

static inline uint32_t PTS_MemoryMap_Size(void *pVoid)
{
	//在Windows中
	//即使不同Map连续
	//VirtualQuery的结果不会将不同的Map合并
	//可以正确运行

	MEMORY_BASIC_INFORMATION Buffer;
	SIZE_T wsResult = ::VirtualQuery(pVoid, &Buffer, sizeof(Buffer));
	assert(wsResult == sizeof(Buffer));
	assert(wsResult != 0U);
	assert(Buffer.Protect == PAGE_READWRITE);
	assert(Buffer.Type == MEM_MAPPED);
	return static_cast<uint32_t>(Buffer.RegionSize);
}

#include <intrin.h>

static inline uint32_t PTS_Size_BitScanReverse(uint32_t Value)
{
	DWORD Index;
	BOOL wbResult = ::_BitScanReverse(&Index, Value);
	assert(wbResult != FALSE);
	return Index;
}