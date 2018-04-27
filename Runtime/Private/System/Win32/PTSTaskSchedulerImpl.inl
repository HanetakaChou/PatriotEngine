static inline uint32_t PTS_Info_HardwareThreadNumber()
{
	DWORD_PTR ProcessAffinityMask;
	DWORD_PTR SystemAffinityMask;
	BOOL wbResult = ::GetProcessAffinityMask(::GetCurrentProcess(), &ProcessAffinityMask, &SystemAffinityMask);
	assert(wbResult != FALSE);
	assert((ProcessAffinityMask&SystemAffinityMask) == ProcessAffinityMask);

	uint32_t ProcessProcessorNumber = 0U;
	for (DWORD_PTR i = 0U; i < (DWORD_PTR(CHAR_BIT) * sizeof(DWORD_PTR)); ++i)
	{
		DWORD_PTR ProcessorAffinityMask = (DWORD_PTR(1U) << i);

		if (ProcessAffinityMask & ProcessorAffinityMask)
		{
			++ProcessProcessorNumber;
		}
	}

	return ProcessProcessorNumber;
}