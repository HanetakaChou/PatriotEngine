#if defined(PTPOSIXLINUXGLIBC)||defined(PTPOSIXLINUXBIONIC)
#include <unistd.h>
static inline uint32_t PTS_Info_HardwareThreadNumber()
{
	cpu_set_t AffinityMask;
	int rtval = ::sched_getaffinity(::getpid(), sizeof(cpu_set_t), &AffinityMask);
	assert(rtval == 0);
	int Count = CPU_COUNT(&AffinityMask);
	return static_cast<uint32_t>(Count);
}
#elif defined (PTPOSIXMACH)
#include <mach/mach.h>
#else
#error 未知的平台
#endif

static inline uint32_t PTS_Size_BitScanReverse(uint32_t Value)
{
	int Index = 31 - ::__builtin_clz(Value);
	return static_cast<uint32_t>(Index);
}

static inline uint32_t PTS_Size_BitPopCount(uint32_t Value)
{
	int Index = ::__builtin_popcount(Value);
	return static_cast<uint32_t>(Index);
}

//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
//#include "../../../Public/System/Posix/PTSLocateSourceCode.inl"