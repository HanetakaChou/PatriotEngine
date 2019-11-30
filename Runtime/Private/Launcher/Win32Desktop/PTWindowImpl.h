#include "../../../Public/Launcher/PTWWindow.h"
#include "../../../Public/System/PTSThread.h"

class PTWWindowImpl :public IPTWWindow
{
	//IPTWindow
	void EventOutputCallback_Hook(void *pUserData, void(PTPTR *pOutputEventCallback)(void *pUserData, void *pEventData)) override;
	void EventInputCallback_Hook(void *pUserData, void(PTPTR *pInputCallback)(void *pUserData, void *pEventData)) override;
	void Parent_Set(PTWHWindow hWindowParent) override;
	void Position_Set(uint32_t TopLeftX, uint32_t TopLeftY) override;
	void Size_Set(uint32_t Width, uint32_t Height) override;

	//PTWWindowImpl
	void * volatile m_pEventOutputCallback_UserData;
	void (PTPTR * volatile m_pEventOutputCallback)(void *pUserData, void *pEventData);
	void * volatile m_pEventInputCallback_UserData;
	void (PTPTR * volatile m_pEventInputCallback)(void *pUserData, void *pEventData);

	HWND volatile m_hWindow_Cache;

	friend int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR lpCmdLine, int);

public:
	//wWinMain线程调用
	//inline void Handle_Cache(HWND hWindow);
	//inline void Size_Cache(uint32_t width, uint32_t Height);
	inline PTWWindowImpl();
	inline ~PTWWindowImpl();

	//Cache CmdLine
	wchar_t *m_pCmdLine_Cache;

	//PTInvokeMain线程调用
	void TermminateMessagePump();
};