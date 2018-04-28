#include <unistd.h>

static inline uint32_t PTS_Info_HardwareThreadNumber()
{
	cpu_set_t AffinityMask;
	int rtval = ::sched_getaffinity(::getpid(), sizeof(cpu_set_t), &AffinityMask);
	assert(rtval == 0);
	return CPU_COUNT(&AffinityMask);
}