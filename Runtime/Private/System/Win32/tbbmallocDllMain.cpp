#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <assert.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		::DisableThreadLibraryCalls(hModule);
	}
	break;
	case DLL_PROCESS_DETACH:
	{
		//Do Nothing
	}
	break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	default:
		assert(0);//由于已经Disable，因此不应当再接收到消息
	}
	return TRUE;
}