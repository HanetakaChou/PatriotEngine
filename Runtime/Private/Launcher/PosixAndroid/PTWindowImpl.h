#include "../../../Public/Window/PTWWindow.h"
#include <android/native_activity.h>

class PTWWindowImpl :public IPTWWindow
{
	//IPTWindow
	void EventOutputCallback_Hook(void *pUserData, void(PTPTR *pEventOutputCallback)(void *pUserData, void *pOutputData))override;
	void EventInputCallback_Hook(void *pUserData, void(PTPTR *pEventInputCallback)(void *pUserData, void *pInputData)) override;

	void Parent_Set(PTWHWindow hWindowParent) override;
	void Position_Set(uint32_t TopLeftX, uint32_t TopLeftY) override;
	void Size_Set(uint32_t Width, uint32_t Height) override;

	//PTWWindowImpl

	void * volatile m_pEventOutputCallback_UserData;
	void (PTPTR * volatile m_pEventOutputCallback)(void *pUserData, void *pEventData);

	void * volatile m_pEventInputCallback_UserData;
	void (PTPTR * volatile m_pEventInputCallback)(void *pUserData, void *pEventData);

	friend void ANativeActivity_onInputQueueCreated(ANativeActivity *, AInputQueue *);
	friend void ANativeActivity_onInputQueueDestroyed(ANativeActivity *, AInputQueue *);
	friend void ANativeActivity_onNativeWindowCreated(ANativeActivity *, ANativeWindow *);
	friend void ANativeActivity_onNativeWindowResized(ANativeActivity *, ANativeWindow *);
	friend void ANativeActivity_onNativeWindowDestroyed(ANativeActivity *, ANativeWindow *);

	ANativeActivity volatile *m_pActivity_Cache;//UI线程写入 PTInvokeMain线程读取

	friend void ANativeActivity_onCreate(ANativeActivity *pActivity, void *, size_t);
	friend void ANativeActivity_onDestroy(ANativeActivity *);
	
	friend  void *PTInvokeMain(void *pVoid);

public:
	//
	

	//PTInvokeMain线程调用
	inline PTWWindowImpl();
	inline ~PTWWindowImpl();
};

