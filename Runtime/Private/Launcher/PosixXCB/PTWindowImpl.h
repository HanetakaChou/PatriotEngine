#include "../../../Public/Launcher/PTWWindow.h"
#include "../../../Public/System/PTSThread.h"

class PTWWindowImpl :public IPTWWindow
{
	//IPTWindow
	void EventOutputCallback_Hook(void *pUserData, void(PTPTR *pEventOutputCallback)(void *pUserData, void *pOutputData)) override;
	void EventInputCallback_Hook(void *pUserData, void(PTPTR *pEventInputCallback)(void *pUserData, void *pInputData)) override;
	void Parent_Set(PTWHWindow hWindowParent) override;
	void Position_Set(uint32_t TopLeftX, uint32_t TopLeftY) override;
	void Size_Set(uint32_t Width, uint32_t Height) override;

	//PTWWindowImpl
	void * volatile m_pEventOutputCallback_UserData;
	void (PTPTR * volatile m_pEventOutputCallback)(void *pUserData, void *pEventData);
	void * volatile m_pEventInputCallback_UserData;
	void (PTPTR * volatile m_pEventInputCallback)(void *pUserData, void *pEventData);

	xcb_connection_t *m_hDisplay_Cache;
	int m_Argc_Cache;
	char **m_Argv_Cache;

	friend int main(int argc, char *argv[]);
	friend void * PTInvokeMain(void *pVoid);

public:
	//main�̵߳���
	inline PTWWindowImpl();
	inline ~PTWWindowImpl();

	//PTInvokeMain�̵߳���
	inline void TermminateMessagePump();
};
