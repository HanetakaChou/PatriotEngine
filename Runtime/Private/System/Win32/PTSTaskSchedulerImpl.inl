#include <intrin.h>

#if defined(PTARM)
static inline unsigned int __popcnt(unsigned int value)
{
	assert(value == 0U);
	return 0U;
}
#endif

static inline uint32_t PTS_Info_HardwareThreadNumber()
{
	DWORD_PTR ProcessAffinityMask;
	DWORD_PTR SystemAffinityMask;
	BOOL wbResult = ::GetProcessAffinityMask(::GetCurrentProcess(), &ProcessAffinityMask, &SystemAffinityMask);
	assert(wbResult != FALSE);
	assert((ProcessAffinityMask&SystemAffinityMask) == ProcessAffinityMask);
	uint32_t ProcessProcessorCount;
#if defined(PTARM)|| defined(PTX86)
	static_assert(sizeof(DWORD_PTR) == 4U, "");
	ProcessProcessorCount = ::__popcnt(ProcessAffinityMask);
#elif defined(PTX64)
	static_assert(sizeof(DWORD_PTR) == 8U, "");
	ProcessProcessorCount = static_cast<uint32_t>(::__popcnt64(ProcessAffinityMask));
#else
#error δ֪�ļܹ�
#endif
	return ProcessProcessorCount;
}

static inline uint32_t PTS_Size_BitScanReverse(uint32_t Value)
{
	DWORD Index;
	BOOL wbResult = ::_BitScanReverse(&Index, Value);
	assert(wbResult != FALSE);
	return Index;
}

static inline uint32_t PTS_Size_BitPopCount(uint32_t Value)
{
	return ::__popcnt(Value);
}