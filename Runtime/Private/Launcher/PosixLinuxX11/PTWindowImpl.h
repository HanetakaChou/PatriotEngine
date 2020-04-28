#include "../../../Public/WSI/PT_WSI_Window.h"
#include "../../../Public/McRT/PTSThread.h"

class PTWWindowImpl : public PT_WSI_IWindow
{
	//IPTWindow
	void EventOutputCallback_Hook(void *pUserData, void(PTPTR *pEventOutputCallback)(void *pUserData, void *pOutputData)) override;
	void EventInputCallback_Hook(void *pUserData, void(PTPTR *pEventInputCallback)(void *pUserData, void *pInputData)) override;
	void Parent_Set(struct PT_WSI_Window_T *hWindowParent) override;
	void Position_Set(uint32_t TopLeftX, uint32_t TopLeftY) override;
	void Size_Set(uint32_t Width, uint32_t Height) override;

	//PTWWindowImpl
	void *volatile m_pEventOutputCallback_UserData;
	void(PTPTR *volatile m_pEventOutputCallback)(void *pUserData, void *pEventData);
	void *volatile m_pEventInputCallback_UserData;
	void(PTPTR *volatile m_pEventInputCallback)(void *pUserData, void *pEventData);

	//
	bool m_bMessagePump;
	int m_Argc_Cache;
	char **m_Argv_Cache;

	friend int main(int argc, char *argv[]);
	friend void *PTInvokeMain(void *pVoid);

public:
	//main线程调用
	inline PTWWindowImpl(int argc, char *argv[]);
	inline ~PTWWindowImpl();

	//PTInvokeMain线程调用
	inline void TermminateMessagePump();
};
