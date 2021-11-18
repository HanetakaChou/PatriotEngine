#include "PTTopLevelForm.h"

#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

void PTSceneEditor::PTTopLevelForm::InitializeRuntime(System::IntPtr hWnd)
{
	{
		WCHAR CommandLine[] = { L"PTLauncher -PTEditor -PTWindow=0XFFFFFFFF" };
#ifdef _WIN64
		::_i64tow_s(static_cast<intptr_t>(hWnd), CommandLine + 33, 9, 16);
#else
		::_itow_s(static_cast<intptr_t>(hWnd), CommandLine + 33, 9, 16);
#endif

		//ʹ���׽��ִ���Application.dll �ٵ���IPTWindow::Parent_Set

		STARTUPINFOW StartupInfo;
		StartupInfo.cb = sizeof(STARTUPINFOW);
		StartupInfo.lpReserved = NULL;
		StartupInfo.lpDesktop = NULL;
		StartupInfo.lpTitle = NULL;
		StartupInfo.dwFlags = 0U;
		StartupInfo.cbReserved2 = 0U;
		StartupInfo.lpReserved2 = NULL;
		PROCESS_INFORMATION ProcessInformation;
		BOOL wbResult = ::CreateProcessW(NULL, CommandLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_SUSPENDED, NULL, NULL, &StartupInfo, &ProcessInformation);
		assert(wbResult != FALSE);
		//�������
		//������ResumeThread֮ǰ���ӵ��´����Ľ���
		::ResumeThread(ProcessInformation.hThread);
		::CloseHandle(ProcessInformation.hThread);
		::CloseHandle(ProcessInformation.hProcess);
	}
}