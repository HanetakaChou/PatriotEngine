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
//Mach IPC Interface
//http://web.mit.edu/darwin/src/modules/xnu/osfmk/man/
static inline uint32_t PTS_Info_HardwareThreadNumber()
{
	processor_set_name_t assigned_set_name;
	kern_return_t kResult = ::thread_get_assignment(::mach_thread_self(), &assigned_set_name);
	assert(kResult == KERN_SUCCESS);

	host_t host = ::mach_host_self();
	processor_set_basic_info_data_t info;
	mach_msg_type_number_t infoCnt = PROCESSOR_SET_BASIC_INFO_COUNT;
	kResult = ::processor_set_info(assigned_set_name, PROCESSOR_SET_BASIC_INFO, &host, reinterpret_cast<processor_set_info_t>(&info), &infoCnt);
	assert(kResult == KERN_SUCCESS);
	return static_cast<uint32_t>(info.processor_count);
}
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
//#include "../../../Public/McRT/Posix/PTSLocateSourceCode.inl"