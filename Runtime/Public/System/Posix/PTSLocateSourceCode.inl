//There Is Some Bugs In The IDE —— "Visual C++ For Linux Development"
//To Force IDE To Locate The Source Code

#if defined(PTPOSIX)
#if defined(PTPOSIXLINUXGLIBC)

#ifndef NDEBUG
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>

static inline bool PTS_Number_CharToInt_DEC(char C, int32_t *pI);
static inline bool PTS_Number_CharToInt_DEC(char C, uint32_t *pI);
static inline bool PTS_Number_CharToInt_DEC(char C, int64_t *pI);
static inline bool PTS_Number_CharToInt_DEC(char C, uint64_t *pI);

static struct PTS_Init_DebugBreak
{
	inline PTS_Init_DebugBreak()
	{
		char Str_status[4096U];
		//assert(s_Page_Size == 4096U);
		{
			int FD_status = ::open("/proc/thread-self/status", O_RDONLY);
			assert(FD_status != -1);
			ssize_t ssResult = ::read(FD_status, Str_status, 4096U);
			assert(ssResult != -1);
			int iResult = ::close(FD_status);
			assert(iResult == 0);
		}

		char *Str_TracerPidEnd = NULL;

		char Str_TracerPidStart[10] = { "TracerPid" };
		int Str_TracerPidStart_Length = 9U;

		char *pStr_RowBegin = Str_status;

		bool bIsFound = false;
		while ((*pStr_RowBegin) != '\0')
		{
			bool bIsEqual = true;
			for (int i = 0; i < Str_TracerPidStart_Length; ++i)
			{
				assert(pStr_RowBegin[i] != '\0');
				if (pStr_RowBegin[i] != Str_TracerPidStart[i])
				{
					bIsEqual = false;
					break;
				}
			}

			if (bIsEqual)
			{
				Str_TracerPidEnd = pStr_RowBegin;
				assert(Str_TracerPidEnd[Str_TracerPidStart_Length] == ':');
				Str_TracerPidEnd += (Str_TracerPidStart_Length + 1);
				assert((*Str_TracerPidEnd) == '\t');
				while ((*Str_TracerPidEnd) == '\t')
				{
					++Str_TracerPidEnd;
				}
				bIsFound = true;
				break;
			}

			while ((*pStr_RowBegin) != '\n')
			{
				++pStr_RowBegin;
			}

			++pStr_RowBegin;
		}

		assert(Str_TracerPidEnd != NULL);

		pid_t TracerPid = 0;
		pid_t ValueToAdd;
		while (::PTS_Number_CharToInt_DEC(*Str_TracerPidEnd, &ValueToAdd))
		{
			TracerPid *= 10U;
			TracerPid += ValueToAdd;
			++Str_TracerPidEnd;
		}

		if (TracerPid != pid_t(0))
		{
			//__debugbreak In MSVC
			__builtin_debugtrap();
		}

	}
}s_Init_DebugBreak;

static inline bool PTS_Number_CharToInt_DEC(char C, int32_t *pI)
{
	switch (C)
	{
	case '0':
		(*pI) = 0;
		return true;
	case '1':
		(*pI) = 1;
		return true;
	case '2':
		(*pI) = 2;
		return true;
	case '3':
		(*pI) = 3;
		return true;
	case '4':
		(*pI) = 4;
		return true;
	case '5':
		(*pI) = 5;
		return true;
	case '6':
		(*pI) = 6;
		return true;
	case '7':
		(*pI) = 7;
		return true;
	case '8':
		(*pI) = 8;
		return true;
	case '9':
		(*pI) = 9;
		return true;
	default:
		return false;
	}
}
static inline bool PTS_Number_CharToInt_DEC(char C, uint32_t *pI)
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
	default:
		return false;
	}
}
static inline bool PTS_Number_CharToInt_DEC(char C, int64_t *pI)
{
	switch (C)
	{
	case '0': (*pI) = 0; return true;
	case '1': (*pI) = 1; return true;
	case '2': (*pI) = 2; return true;
	case '3': (*pI) = 3; return true;
	case '4': (*pI) = 4; return true;
	case '5': (*pI) = 5; return true;
	case '6': (*pI) = 6; return true;
	case '7': (*pI) = 7; return true;
	case '8': (*pI) = 8; return true;
	case '9': (*pI) = 9; return true;
	default: return false;
	}
}
static inline bool PTS_Number_CharToInt_DEC(char C, uint64_t *pI)
{
	switch (C)
	{
	case '0': (*pI) = 0U; return true;
	case '1': (*pI) = 1U; return true;
	case '2': (*pI) = 2U; return true;
	case '3': (*pI) = 3U; return true;
	case '4': (*pI) = 4U; return true;
	case '5': (*pI) = 5U; return true;
	case '6': (*pI) = 6U; return true;
	case '7': (*pI) = 7U; return true;
	case '8': (*pI) = 8U; return true;
	case '9': (*pI) = 9U; return true;
	default: return false;
	}
}
#endif

#elif defined(PTPOSIXLINUXBIONIC)
#else
#error 未知的平台
#endif
#else
#error 未知的平台
#endif