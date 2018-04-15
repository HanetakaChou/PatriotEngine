#include <sys/mman.h>
#include <dirent.h>

static uint32_t const s_Page_Size = 1024U * 4U;

static inline void *PTS_MemoryMap_Alloc(uint32_t Size)
{
	//在Linux中仅对齐到页大小（1024*4），并不保证对齐到块大小（1024*4*4）
	//先用mmap分配（SizeAligned/PageSize+3）个页，再用munmap释放多余的3个页（前0后3/前1后2/前3后0）

	assert(s_Page_Size == (1U << 12U));
	assert(s_Block_Size == (s_Page_Size * 4U));

	uint32_t PageNumber = ((Size - 1U) >> 12U) + 1U;
	uintptr_t AddressWholeBegin = reinterpret_cast<uintptr_t>(::mmap(NULL, s_Page_Size*(PageNumber + 3U), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0U));
	assert(::PTS_Size_IsAligned(AddressWholeBegin, s_Page_Size));

	uintptr_t PageWholeBegin = AddressWholeBegin >> 12U;
	uintptr_t PageNeededBegin = ::PTS_Size_AlignUpFrom(PageWholeBegin, static_cast<uintptr_t>(4U));
	uint32_t PageToFreeBeginNumber = static_cast<uint32_t>(PageNeededBegin - PageWholeBegin);

	int iResult;
	if (PageToFreeBeginNumber != 0U)
	{
		iResult = ::munmap(reinterpret_cast<void*>(AddressWholeBegin), s_Page_Size*PageToFreeBeginNumber);
		assert(iResult == 0);
	}
	if (PageToFreeBeginNumber != 3U)
	{
		iResult = ::munmap(reinterpret_cast<void*>(s_Page_Size*(PageNeededBegin + PageNumber)), s_Page_Size*(3U - PageToFreeBeginNumber));
		assert(iResult == 0);
	}
	return reinterpret_cast<void*>(s_Page_Size*PageNeededBegin);
}

static inline void PTS_MemoryMap_Free(void *pVoid)
{
	//在Linux中
	//在x86架构下，用户空间只有2GB（0X7FFFFFFF 8个字符）或3GB（0X‭BFFFFFFF‬ 8个字符）
	//在x86_64架构下，用户空间只有128TB（0X7FFFFFFFFFFF 12个字符）
	char Str_AddressStart[12];
	char *pStr_AddressStart;
	int Str_AddressStart_Length;
	{
		char const *pStr_Number = "0123456789abcdef";
		int index = 12;
		uintptr_t AddressStart = reinterpret_cast<uintptr_t>(pVoid);
		while (AddressStart != 0U)
		{
			--index;
			assert(index >= 0U);
			Str_AddressStart[index] = pStr_Number[(AddressStart & 0XF)];
			AddressStart >>= 4U;
		}
		pStr_AddressStart = Str_AddressStart + index;
		Str_AddressStart_Length = 12 - index;
	}

	
	//mmap MAP_SHARED|MAP_ANONYMOUS
	//在proc/self/map_files 链接到 /dev/zero
	//不同的mmap之间 即使连续照样不会合并
	dirent DirEnt_mmap;
	DIR *pDir_mmap = ::opendir("/proc/self/map_files");
	assert(pDir_mmap != NULL);
	dirent *pDirEnt_mmap;
	bool bIsFound = false;
	while ((::readdir_r(pDir_mmap, &DirEnt_mmap, &pDirEnt_mmap) == 0) && (pDirEnt_mmap != NULL))
	{
		bool bIsEqual = true;
		for (int i = 0; i < Str_AddressStart_Length; ++i)
		{
			if (pDirEnt_mmap->d_name[i] == '\0' || pDirEnt_mmap->d_name[i] != pStr_AddressStart[i])
			{
				bIsEqual = false;
				break;
			}
		}

		if (bIsEqual)
		{
			bIsFound = true;
			break;
		}
	}
	assert(bIsFound);

	int iResult = ::closedir(pDir_mmap);
	assert(iResult == 0);

	uintptr_t AddressEnd = 0U;
	assert(pDirEnt_mmap->d_name[Str_AddressStart_Length] == '-');
	char *pStr_AddressEnd = pDirEnt_mmap->d_name + (Str_AddressStart_Length + 1);
	while ((*pStr_AddressEnd) != '\0')
	{
		AddressEnd <<= 4U;

		uintptr_t ValueToAdd;
		switch (*pStr_AddressEnd)
		{
		case '0':
			ValueToAdd = 0U;
			break;
		case '1':
			ValueToAdd = 1U;
			break;
		case '2':
			ValueToAdd = 2U;
			break;
		case '3':
			ValueToAdd = 3U;
			break;
		case '4':
			ValueToAdd = 4U;
			break;
		case '5':
			ValueToAdd = 5U;
			break;
		case '6':
			ValueToAdd = 6U;
			break;
		case '7':
			ValueToAdd = 7U;
			break;
		case '8':
			ValueToAdd = 8U;
			break;
		case '9':
			ValueToAdd = 9U;
			break;
		case 'a':
			ValueToAdd = 10U;
			break;
		case 'b':
			ValueToAdd = 11U;
			break;
		case 'c':
			ValueToAdd = 12U;
			break;
		case 'd':
			ValueToAdd = 13U;
			break;
		case 'e':
			ValueToAdd = 14U;
			break;
		case 'f':
			ValueToAdd = 15U;
			break;
		default:
			assert(0);
		}
		AddressEnd += ValueToAdd;

		++pStr_AddressEnd;
	}

	iResult = ::munmap(pVoid, AddressEnd - reinterpret_cast<uintptr_t>(pVoid));
	assert(iResult == 0);
}

static inline uint32_t PTS_Size_BitScanReverse(uint32_t Value)
{
	return 31 - ::__builtin_clz(233);
}