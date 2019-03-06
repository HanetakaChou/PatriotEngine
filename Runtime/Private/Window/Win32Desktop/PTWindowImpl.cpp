#include "PTWindowImpl.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ShellAPI.h>
#include <stdio.h>

static unsigned __stdcall PTInvokeMain(void *pVoid);

#include "PTResource.h"

//注:PTLauncher提供了IPTWWindow的示例实现，但这并不是必须的
//PatriotEngine鼓励第三方用户自定义IPTWWindow的实现来开发编辑器或其它上层应用

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR lpCmdLine, int)
{
	ATOM hAtom;
	WNDCLASSEXW wc;
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = 0U;
	wc.lpfnWndProc = [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT
	{
		switch (uMsg)
		{
		case WM_CREATE:
		{

#ifdef NDEBUG
			HHOOK hHook = ::SetWindowsHookExW(WH_KEYBOARD_LL, [](int nCode, ::WPARAM wParam, ::LPARAM lParam)->LRESULT {
				if (nCode >= 0)
				{
					::KBDLLHOOKSTRUCT *pKeyboardLowLevel = reinterpret_cast<::KBDLLHOOKSTRUCT *>(lParam);
					switch (pKeyboardLowLevel->vkCode)
					{
					case VK_F4:
					{
						if (
							!(pKeyboardLowLevel->flags & LLKHF_EXTENDED)//Not Extended Key
							&& (pKeyboardLowLevel->flags & LLKHF_ALTDOWN)//ALT Key Pressed
							&& !(pKeyboardLowLevel->flags & LLKHF_UP)//Being Pressed
							)
						{
							//if(Hook Alt+F4
							return 1;
						}
					}
					break;
					case VK_TAB:
					{
						if (
							!(pKeyboardLowLevel->flags & LLKHF_EXTENDED)//Not Extended Key
							&& (pKeyboardLowLevel->flags & LLKHF_ALTDOWN)//ALT Key Pressed
							&& !(pKeyboardLowLevel->flags & LLKHF_UP)//Being Pressed
							)
						{
							//if(Hook Alt+Tab
							return 1;
						}
					}
					break;
					case VK_LWIN:
					{
						if (
							(pKeyboardLowLevel->flags & LLKHF_EXTENDED)//Extended Key
							&& !(pKeyboardLowLevel->flags & LLKHF_ALTDOWN)//ALT Key Not Pressed
							&& !(pKeyboardLowLevel->flags & LLKHF_UP)//Being Pressed
							)
						{
							//if(Hook Left Win
							return 1;
						}
					}
					break;
					}
				}
				return ::CallNextHookEx(NULL, nCode, wParam, lParam);
		}, NULL, 0U);
			assert(hHook != NULL);
#endif 
			PTWWindowImpl *pSingleton = static_cast<PTWWindowImpl *>(reinterpret_cast<CREATESTRUCT *>(lParam)->lpCreateParams);
			assert(pSingleton != NULL);

			//WindowLongPtr-Get/Set
			::SetWindowLongPtrW(hWnd, 0U, reinterpret_cast<LONG_PTR>(pSingleton));

			//PTEventOutputType_WindowCreated
			{
				void (PTPTR * pHere_EventOutputCallback)(void *pUserData, void *pEventData);

				//SpinLock
				while ((pHere_EventOutputCallback = reinterpret_cast<void (PTPTR *)(void *pUserData, void *pEventData)>(
					::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(&pSingleton->m_pEventOutputCallback))
					)) == NULL)
				{
					::PTS_Yield();
				}

				void *pHere_EventOutputCallback_UserData = reinterpret_cast<void *>(
					::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(&pSingleton->m_pEventOutputCallback_UserData))
					);

				IPTWWindow::EventOutput_WindowCreated EventData;
				EventData.m_Type = IPTWWindow::EventOutput::Type_WindowCreated;
				EventData.m_hDisplay = reinterpret_cast<HINSTANCE>(::GetClassLongPtrW(hWnd, GCLP_HMODULE));
				EventData.m_hWindow = hWnd;

				pHere_EventOutputCallback(pHere_EventOutputCallback_UserData, &EventData);
			}

			return 0;
		}
		case WM_DESTROY:
		{
			::PostQuitMessage(0U);
			return 0;
		}
#if 0
		case WM_SETFOCUS:
		{
			BOOL bRet;
			HMONITOR hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
			MONITORINFOEXW MonitorInfo;
			MonitorInfo.cbSize = sizeof(MONITORINFOEXW);
			bRet = ::GetMonitorInfoW(hMonitor, &MonitorInfo);
			assert(bRet != 0);
			bRet = ::SetWindowPos(
				hWnd, 
				NULL, 
				MonitorInfo.rcWork.left, MonitorInfo.rcWork.top, 
				MonitorInfo.rcWork.right - MonitorInfo.rcWork.left, MonitorInfo.rcWork.bottom - MonitorInfo.rcWork.top, 
				SWP_NOZORDER
			);
			assert(bRet != 0);
			return 0;
		}
#endif
		case WM_SIZE:
		{
			PTWWindowImpl *pSingleton = reinterpret_cast<PTWWindowImpl *>(::GetWindowLongPtrW(hWnd, 0U));
			assert(pSingleton != NULL);

			//PTEventOutputType_WindowResized
			{
				void (PTPTR * pHere_EventOutputCallback)(void *pUserData, void *pEventData);

				//SpinLock
				while ((pHere_EventOutputCallback = reinterpret_cast<void (PTPTR *)(void *pUserData, void *pEventData)>(
					::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(&pSingleton->m_pEventOutputCallback))
					)) == NULL)
				{
					::PTS_Yield();
				}

				void *pHere_EventOutputCallback_UserData = reinterpret_cast<void *>(
					::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(&pSingleton->m_pEventOutputCallback_UserData))
					);

				IPTWWindow::EventOutput_WindowResized EventData;
				EventData.m_Type = IPTWWindow::EventOutput::Type_WindowResized;
				EventData.m_hDisplay = reinterpret_cast<HINSTANCE>(::GetClassLongPtrW(hWnd, GCLP_HMODULE));
				EventData.m_hWindow = hWnd;
				EventData.m_Width = static_cast<uint32_t>(LOWORD(lParam));
				EventData.m_Height = static_cast<uint32_t>(HIWORD(lParam));

				pHere_EventOutputCallback(pHere_EventOutputCallback_UserData, &EventData);
			}
			
			return 0;
		}
		default:
		{
			return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
		}
		}
	};
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(void *);
	wc.hInstance = hInstance;
	wc.hIcon = ::LoadIconW(hInstance, MAKEINTRESOURCE(IDI_PTICON));
	wc.hCursor = ::LoadCursorW(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	WCHAR szClassName[33];
	::swprintf_s(szClassName, 33, L"PTWindowClass:0X%p", &wc);
	wc.lpszClassName = szClassName;
	wc.hIconSm = wc.hIcon;
	hAtom = ::RegisterClassExW(&wc);
	assert(hAtom != 0);

	HWND hDesktop = ::GetDesktopWindow();
	HMONITOR hMonitor = ::MonitorFromWindow(hDesktop, MONITOR_DEFAULTTONEAREST);
	MONITORINFOEXW MonitorInfo;
	MonitorInfo.cbSize = sizeof(MONITORINFOEXW);
	BOOL bRet = ::GetMonitorInfoW(hMonitor, &MonitorInfo);
	assert(bRet != 0);

	PTWWindowImpl l_WindowImpl_Singleton;
	l_WindowImpl_Singleton.m_pCmdLine_Cache = lpCmdLine;

	PTSThread hThreadInvoke;
	PTBOOL tbResult = ::PTSThread_Create(&PTInvokeMain, static_cast<PTWWindowImpl *>(&l_WindowImpl_Singleton), &hThreadInvoke);
	assert(tbResult != PTFALSE);

	HWND hWnd = ::CreateWindowExW(
		WS_EX_APPWINDOW,
		MAKEINTATOM(hAtom),
		L"PatriotEngine",
		WS_OVERLAPPED | WS_CAPTION | WS_SIZEBOX | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
		MonitorInfo.rcWork.left, MonitorInfo.rcWork.top,
		MonitorInfo.rcWork.right - MonitorInfo.rcWork.left, MonitorInfo.rcWork.bottom - MonitorInfo.rcWork.top,
		hDesktop,
		NULL,
		hInstance,
		static_cast<PTWWindowImpl *>(&l_WindowImpl_Singleton)
	);
	//HWND hWnd = ::CreateWindowExW(WS_EX_APPWINDOW, MAKEINTATOM(atom), L"PTWindow", WS_POPUP | WS_VISIBLE, MonitorInfo.rcWork.left, MonitorInfo.rcWork.top, MonitorInfo.rcWork.right, MonitorInfo.rcWork.bottom, hDesktop, NULL, hInstance, NULL);	
	assert(hWnd != NULL);

	l_WindowImpl_Singleton.m_hWindow_Cache = hWnd;

	MSG msg;
	BOOL wbResult;
	while ((wbResult = ::GetMessageW(&msg, NULL, 0, 0)) != 0)
	{
		assert(wbResult != -1);

		::TranslateMessage(&msg);
		::DispatchMessageW(&msg);
	}
	assert(msg.message == WM_QUIT);

	//确保栈中的内存 PTInvokeParam ParamInvoke 在PTInvokeMain的整个生命期内是有效的
	tbResult = ::PTSThread_Join(&hThreadInvoke);
	assert(tbResult != PTFALSE);

	return static_cast<int>(msg.wParam);
}

inline PTWWindowImpl::PTWWindowImpl()
{
	//EventOutputCallback
	m_pEventOutputCallback = NULL;
	m_pEventOutputCallback_UserData = NULL;
	//EventInputCallback
	m_pEventInputCallback = NULL;
	m_pEventInputCallback_UserData = NULL;
	//Parent_Set
	m_hWindow_Cache = NULL;
	//TermminateMessagePump
	//m_ThreadID_Cache = -1;
	//PTInvokeMain
	m_pCmdLine_Cache = NULL;
}

inline PTWWindowImpl::~PTWWindowImpl()
{

}

void PTWWindowImpl::EventOutputCallback_Hook(void *pUserData, void(PTPTR *pEventOutputCallback)(void *pUserData, void *pOutputData))
{
	::PTSAtomic_Set(reinterpret_cast<uintptr_t volatile *>(&m_pEventOutputCallback_UserData), reinterpret_cast<uintptr_t>(pUserData));
	::PTSAtomic_Set(reinterpret_cast<uintptr_t volatile *>(&m_pEventOutputCallback), reinterpret_cast<uintptr_t>(pEventOutputCallback));
}

void PTWWindowImpl::EventInputCallback_Hook(void *pUserData, void(PTPTR *pEventInputCallback)(void *pUserData, void *pInputData))
{
	::PTSAtomic_Set(reinterpret_cast<uintptr_t volatile *>(&m_pEventInputCallback_UserData), reinterpret_cast<uintptr_t>(pUserData));
	::PTSAtomic_Set(reinterpret_cast<uintptr_t volatile *>(&m_pEventInputCallback), reinterpret_cast<uintptr_t>(pEventInputCallback));
}

void PTWWindowImpl::Parent_Set(PTWHWindow hWindowParent)
{
	if (hWindowParent == NULL)
	{
		LONG_PTR lResult = ::SetWindowLongPtrW(m_hWindow_Cache, GWL_STYLE, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE);
		assert(lResult != 0);

		HWND hDesktop = ::GetDesktopWindow();
		assert(hDesktop != NULL);

		HWND hParentPrevious = ::SetParent(m_hWindow_Cache, hDesktop);
		assert(hParentPrevious != NULL);
	}
	else
	{
		LONG_PTR lResult = ::SetWindowLongPtrW(m_hWindow_Cache, GWL_STYLE, WS_CHILD | WS_VISIBLE);
		assert(lResult != 0);

		HWND hParentPrevious = ::SetParent(m_hWindow_Cache, hWindowParent);
		assert(hParentPrevious != NULL);
	}
}

void PTWWindowImpl::Position_Set(uint32_t TopLeftX, uint32_t TopLeftY)
{
	BOOL wbResult = ::SetWindowPos(m_hWindow_Cache, NULL, TopLeftX, TopLeftY, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	assert(wbResult != FALSE);
}

void PTWWindowImpl::Size_Set(uint32_t Width, uint32_t Height)
{
	BOOL wbResult = ::SetWindowPos(m_hWindow_Cache, NULL, 0, 0, Width, Height, SWP_NOZORDER | SWP_NOMOVE);
	assert(wbResult != FALSE);
}

void PTWWindowImpl::TermminateMessagePump()
{
	HWND hWnd;

	//SpinLock
	while ((hWnd = reinterpret_cast<HWND>(::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(&m_hWindow_Cache)))) == NULL)
	{
		::PTS_Yield();
	}

	DWORD ThreadId_wWinMain = ::GetWindowThreadProcessId(hWnd, NULL);
	BOOL wbResult = ::PostThreadMessageW(ThreadId_wWinMain, WM_QUIT, 0U, 0U);
	assert(wbResult != FALSE);
}


#include "../../../Public/App/PTAExport.h"
#include "../../../Public/System/PTSConvUTF.h"

static unsigned __stdcall PTInvokeMain(void *pVoid)
{
	PTWWindowImpl *pWindow = static_cast<PTWWindowImpl *>(pVoid);

	assert(pWindow->m_pCmdLine_Cache != NULL);

	char CmdLineA[0X10000] = { "PTLauncher " };
	{
		uint32_t InCharsLeft = static_cast<uint32_t>(::wcslen(pWindow->m_pCmdLine_Cache)) + 1U;//包括'\0'
		uint32_t OutCharsLeft = 4096 - 11;
		bool bResult = ::PTSConv_UTF16ToUTF8(reinterpret_cast<char16_t *>(pWindow->m_pCmdLine_Cache), &InCharsLeft, CmdLineA + 11, &OutCharsLeft);
		assert(bResult);
	}

	char *argv[0X10000];
	int argc = 0;

	enum
	{
		WhiteSpace,
		Text
	}StateMachine = WhiteSpace;

	for (size_t i = 0U; CmdLineA[i] != '\0'; ++i)
	{
		switch (StateMachine)
		{
		case WhiteSpace:
		{
			if (!((CmdLineA[i] == ' ' || CmdLineA[i] == '\t' || CmdLineA[i] == '\r' || CmdLineA[i] == '\n')))
			{
				assert(argc < 0X10000);
				argv[argc] = CmdLineA + i;
				++argc;
				StateMachine = Text;
			}
		}
		case Text:
		{
			if ((CmdLineA[i] == ' ' || CmdLineA[i] == '\t' || CmdLineA[i] == '\r' || CmdLineA[i] == '\n'))
			{
				CmdLineA[i] = '\0';
				StateMachine = WhiteSpace;
			}
		}
		}

		assert(i < 0X10000U);
	}

	argv[argc] = NULL;

	int iResult = ::PTAMain(static_cast<IPTWWindow *>(pWindow), argc, argv);
	assert(iResult == 0);

	pWindow->TermminateMessagePump();

	return static_cast<unsigned>(iResult);
}
