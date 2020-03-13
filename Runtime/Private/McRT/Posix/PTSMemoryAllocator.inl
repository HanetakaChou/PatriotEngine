#if defined(PTPOSIXLINUX) 
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#if 0
//proc/self/map_files
//kernel 3.3
#include <dirent.h>
#endif

#elif defined(PTPOSIXMACH)
#include <stdint.h>
#include <assert.h>
#include <mach/mach.h>
#else
#error 未知的平台
#endif

static inline uint32_t PTS_Size_BitScanReverse(uint32_t Value)
{
	int Index = 31 - ::__builtin_clz(Value);
	return static_cast<uint32_t>(Index);
}

#if defined(PTPOSIXLINUXGLIBC) || defined(PTPOSIXLINUXBIONIC)

static inline uint64_t PTS_Size_AlignUpFrom(uint64_t Value, uint64_t Alignment)
{
	return ((Value - 1U) | (Alignment - 1U)) + 1U;
}

static constexpr uint32_t const s_Page_Size = 1024U * 4U;
static constexpr uint32_t const s_Page_Size_Order = 12U;
static constexpr uint32_t const s_Page_Per_Block = 4U;

static inline void *PTS_MemoryMap_Alloc(uint32_t Size)
{
	//在Linux中仅对齐到页大小（1024*4），并不保证对齐到块大小（1024*4*4）
	//先用mmap分配（SizeAligned/PageSize+3）个页，再用munmap释放多余的3个页（前0后3/前1后2/前3后0）

	void *pMemoryAllocated;

	assert(s_Block_Size == (s_Page_Size * s_Page_Per_Block));
	assert(s_Page_Size == (1U << s_Page_Size_Order));

	uint32_t PageNumber = ((Size - 1U) >> s_Page_Size_Order) + 1U;

	void *pMMapVoid = ::mmap(NULL, s_Page_Size * (PageNumber + 3U), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0U);

	if (pMMapVoid != MAP_FAILED)
	{
		assert(pMMapVoid != NULL);

		uintptr_t AddressWholeBegin = reinterpret_cast<uintptr_t>(pMMapVoid);
		assert(::PTS_Size_IsAligned(AddressWholeBegin, s_Page_Size));

		uintptr_t PageWholeBegin = AddressWholeBegin >> s_Page_Size_Order;
		uintptr_t PageNeededBegin = ::PTS_Size_AlignUpFrom(PageWholeBegin, static_cast<uintptr_t>(s_Page_Per_Block));
		uint32_t PageToFreeBeginNumber = static_cast<uint32_t>(PageNeededBegin - PageWholeBegin);

		int iResult;
		if (PageToFreeBeginNumber != 0U)
		{
			iResult = ::munmap(reinterpret_cast<void *>(AddressWholeBegin), s_Page_Size * PageToFreeBeginNumber);
			assert(iResult == 0);
		}
		if (PageToFreeBeginNumber != 3U)
		{
			iResult = ::munmap(reinterpret_cast<void *>(s_Page_Size * (PageNeededBegin + PageNumber)), s_Page_Size * (3U - PageToFreeBeginNumber));
			assert(iResult == 0);
		}

		pMemoryAllocated = reinterpret_cast<void *>(s_Page_Size * PageNeededBegin);
	}
	else
	{
		assert(errno == ENOMEM);
		pMemoryAllocated = NULL;
	}

	return pMemoryAllocated;
}

static inline bool PTS_Number_CharToInt_HEX(char C, uint32_t *pI);

static inline size_t PTS_MemoryMap_Internal_Size(void *pVoid)
{
	//在Linux中
	//在x86或ARM架构下，用户空间最多不超过4GB（0XFFFFFFFF 8个字符）
	//在x86_64（https://www.kernel.org/doc/Documentation/x86/x86_64/mm.txt）架构下，用户空间只有128TB（0X7FFFFFFFFFFF 12个字符）
	//在ARM64（https://www.kernel.org/doc/Documentation/arm64/memory.txt）架构下，用户空间只有512GB(0X7FFFFFFFFF 10个字符）/256TB(0XFFFFFFFFFFFF 12个字符）/4TB(0X3FFFFFFFFFF 11个字符）

#if defined(PTX86) || defined(PTARM)
	char Str_AddressStart[8 + 1];
	char *pStr_AddressStart;
	int Str_AddressStart_Length;
	{
		Str_AddressStart[8] = '\0';
		char const *pStr_Number = "0123456789abcdef";
		int index = 8;
		uintptr_t AddressStart = reinterpret_cast<uintptr_t>(pVoid);
		while (AddressStart != 0U)
		{
			--index;
			assert(index >= 0U);
			Str_AddressStart[index] = pStr_Number[(AddressStart & 0XF)];
			AddressStart >>= 4U;
		}
		pStr_AddressStart = Str_AddressStart + index;
		Str_AddressStart_Length = 8 - index;
	}
#elif defined(PTX64) || defined(PTARM64)
	char Str_AddressStart[12 + 1];
	char *pStr_AddressStart;
	int Str_AddressStart_Length;
	{
		Str_AddressStart[12] = '\0';
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
#else
#error 未知的架构
#endif

	//proc/self/maps

	int FD_maps = ::open("/proc/self/maps", O_RDONLY);
	assert(FD_maps != -1);
	char Str_maps[4096U];
	assert(s_Page_Size == 4096U);
	ssize_t ssResult;
	char *pStr_AddressEnd = NULL;
	while ((ssResult = ::read(FD_maps, Str_maps, 4096U)) != 0)
	{
		assert(ssResult != -1);
		assert(ssResult < 4096);
		assert(Str_maps[ssResult - 1] == '\n');
		Str_maps[ssResult] = '\0';

		char *pStr_RowBegin = Str_maps;

		bool bIsFound = false;
		while ((*pStr_RowBegin) != '\0')
		{
			bool bIsEqual = true;
			for (int i = 0; i < Str_AddressStart_Length; ++i)
			{
				assert(pStr_RowBegin[i] != '\0');
				if (pStr_RowBegin[i] != pStr_AddressStart[i])
				{
					bIsEqual = false;
					break;
				}
			}

			if (bIsEqual)
			{
				pStr_AddressEnd = pStr_RowBegin;
				assert(pStr_AddressEnd[Str_AddressStart_Length] == '-');
				pStr_AddressEnd += (Str_AddressStart_Length + 1);
				bIsFound = true;
				break;
			}

			while ((*pStr_RowBegin) != '\n')
			{
				assert((*pStr_RowBegin) != '\0');
				++pStr_RowBegin;
			}

			++pStr_RowBegin;
		}

		if (bIsFound)
		{
			break;
		}
	}

	assert(pStr_AddressEnd != NULL);

	int iResult = ::close(FD_maps);
	assert(iResult == 0);

#if 0
	//proc/self/map_files
	//kernel 3.3

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

	assert(pDirEnt_mmap->d_name[Str_AddressStart_Length] == '-');
	char *pStr_AddressEnd = pDirEnt_mmap->d_name + (Str_AddressStart_Length + 1);
#endif

	uintptr_t AddressEnd = 0U;
	uint32_t ValueToAdd;
	while (::PTS_Number_CharToInt_HEX(*pStr_AddressEnd, &ValueToAdd))
	{
		AddressEnd <<= 4U;
		AddressEnd += ValueToAdd;
		++pStr_AddressEnd;
	}

	//assert((*pStr_AddressEnd) == '\0');

	return AddressEnd - reinterpret_cast<uintptr_t>(pVoid);
}

static inline void PTS_MemoryMap_Free(void *pVoid)
{
	size_t Len = ::PTS_MemoryMap_Internal_Size(pVoid);
	int iResult = ::munmap(pVoid, Len);
	assert(iResult == 0);
}

static inline uint32_t PTS_MemoryMap_Size(void *pVoid)
{
	return static_cast<uint32_t>(::PTS_MemoryMap_Internal_Size(pVoid));
}

static inline bool PTS_Number_CharToInt_HEX(char C, uint32_t *pI)
{
	switch (C)
	{
	case '0':
		(*pI) = 0U;
		return true;
	case '1':
		(*pI) = 1U;
		return true;
	case '2':
		(*pI) = 2U;
		return true;
	case '3':
		(*pI) = 3U;
		return true;
	case '4':
		(*pI) = 4U;
		return true;
	case '5':
		(*pI) = 5U;
		return true;
	case '6':
		(*pI) = 6U;
		return true;
	case '7':
		(*pI) = 7U;
		return true;
	case '8':
		(*pI) = 8U;
		return true;
	case '9':
		(*pI) = 9U;
		return true;
	case 'a':
		(*pI) = 10U;
		return true;
	case 'b':
		(*pI) = 11U;
		return true;
	case 'c':
		(*pI) = 12U;
		return true;
	case 'd':
		(*pI) = 13U;
		return true;
	case 'e':
		(*pI) = 14U;
		return true;
	case 'f':
		(*pI) = 15U;
		return true;
	default:
		return false;
	}
}

#elif defined(PTPOSIXMACH)

static inline uintptr_t PTS_Size_AlignUpFrom(uintptr_t Value, uintptr_t Alignment)
{
	return ((Value - 1U) | (Alignment - 1U)) + 1U;
}

//Mach IPC Interface
//http://web.mit.edu/darwin/src/modules/xnu/osfmk/man/

static constexpr uint32_t const s_Page_Size = 1024U * 4U;
static constexpr uint32_t const s_Page_Size_Order = 12U;
static constexpr uint32_t const s_Page_Per_Block = 4U;

static inline void *PTS_MemoryMap_Alloc(uint32_t Size)
{
	//在mach中仅对齐到页大小（1024*4），并不保证对齐到块大小（1024*4*4）
	//先用vm_allocate分配（SizeAligned/PageSize+3）个页，再用vm_deallocate释放多余的3个页（前0后3/前1后2/前3后0）

	assert(s_Block_Size == (s_Page_Size * s_Page_Per_Block));
	assert(s_Page_Size == (1U << s_Page_Size_Order));

#ifndef NDEBUG
	{
		vm_size_t pagesize;
		kern_return_t kResult = ::host_page_size(::mach_host_self(), &pagesize);
		assert((kResult == KERN_SUCCESS) && (pagesize == s_Page_Size));
	}
#endif

	void *pMemoryAllocated;

	uint32_t PageNumber = ((Size - 1U) >> s_Page_Size_Order) + 1U;

	vm_address_t kAddress = NULL;
	kern_return_t kResult = ::vm_allocate(mach_task_self(), &kAddress, s_Page_Size * (PageNumber + 3U), TRUE);
	if (kResult == KERN_SUCCESS)
	{
		uintptr_t AddressWholeBegin = static_cast<uintptr_t>(kAddress);
		assert(::PTS_Size_IsAligned(AddressWholeBegin, s_Page_Size));

		uintptr_t PageWholeBegin = AddressWholeBegin >> s_Page_Size_Order;
		uintptr_t PageNeededBegin = ::PTS_Size_AlignUpFrom(PageWholeBegin, static_cast<uintptr_t>(s_Page_Per_Block));

		uint32_t PageToFreeBeginNumber = static_cast<uint32_t>(PageNeededBegin - PageWholeBegin);
		assert(PageToFreeBeginNumber <= 3U);

		if (PageToFreeBeginNumber != 0U)
		{
			kResult = ::vm_deallocate(::mach_task_self(), static_cast<vm_address_t>(AddressWholeBegin), s_Page_Size * PageToFreeBeginNumber);
			assert(kResult == KERN_SUCCESS);
		}

		if (PageToFreeBeginNumber != 3U)
		{
			kResult = ::vm_deallocate(::mach_task_self(), static_cast<vm_address_t>(s_Page_Size * (PageNeededBegin + PageNumber)), s_Page_Size * (3U - PageToFreeBeginNumber));
			assert(kResult == KERN_SUCCESS);
		}

		//设置继承属性后，即使分配的内存连续，内核也不会合并，vm_region_64得到单次分配的结果
		//ToDO: 设置成VM_INHERIT_SHARE是否更可靠？
		kResult = vm_inherit(::mach_task_self(), static_cast<vm_address_t>(s_Page_Size * PageNeededBegin), s_Page_Size * PageNumber, VM_INHERIT_NONE);
		assert(VM_INHERIT_NONE != VM_INHERIT_DEFAULT);
		assert(kResult == KERN_SUCCESS);

		pMemoryAllocated = reinterpret_cast<void *>(s_Page_Size * PageNeededBegin);
	}
	else
	{
		assert(kResult == KERN_NO_SPACE);
		pMemoryAllocated = NULL;
	}

	return pMemoryAllocated;
}

static inline vm_size_t PTS_MemoryMap_Internal_Size(void *pVoid)
{
	vm_address_t address = reinterpret_cast<vm_address_t>(pVoid);
	vm_size_t size;

	vm_region_basic_info_data_64_t info;
	mach_msg_type_number_t infoCnt = VM_REGION_BASIC_INFO_COUNT_64;

	mach_port_t object_name;

	kern_return_t kResult = ::vm_region_64(::mach_task_self(), &address, &size, VM_REGION_BASIC_INFO, reinterpret_cast<vm_region_info_t>(&info), &infoCnt, &object_name);
	assert(kResult == KERN_SUCCESS);

	return size;
}

static inline void PTS_MemoryMap_Free(void *pVoid)
{
	vm_size_t Len = ::PTS_MemoryMap_Internal_Size(pVoid);
	kern_return_t kResult = ::vm_deallocate(::mach_task_self(), reinterpret_cast<vm_address_t>(pVoid), Len);
	assert(kResult == KERN_SUCCESS);
}

static inline uint32_t PTS_MemoryMap_Size(void *pVoid)
{
	return static_cast<uint32_t>(::PTS_MemoryMap_Internal_Size(pVoid));
}
#else
#error 未知的平台
#endif
