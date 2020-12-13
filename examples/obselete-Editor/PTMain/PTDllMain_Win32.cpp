#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
		::DisableThreadLibraryCalls(hModule);
	return TRUE;
}