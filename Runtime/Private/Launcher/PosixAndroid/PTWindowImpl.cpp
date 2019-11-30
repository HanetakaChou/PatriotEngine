#include <unistd.h>
#include <assert.h>
#include <cstdlib>
#include <android/native_activity.h>
#include "../../../Public/System/PTSThread.h"

static void ANativeActivity_onDestroy(ANativeActivity *);
static void ANativeActivity_onInputQueueCreated(ANativeActivity *, AInputQueue *);
static void ANativeActivity_onInputQueueDestroyed(ANativeActivity *, AInputQueue *);
static void ANativeActivity_onNativeWindowCreated(ANativeActivity *, ANativeWindow *);
static void ANativeActivity_onNativeWindowResized(ANativeActivity *, ANativeWindow *);
static void ANativeActivity_onNativeWindowRedrawNeeded(ANativeActivity *, ANativeWindow *);
static void ANativeActivity_onNativeWindowDestroyed(ANativeActivity *, ANativeWindow *);
static void *PTInvokeMain(void *pVoid);

//PTSystem
#include <fcntl.h>
extern "C" PTSYSTEMAPI int PTS_iFDDir_DataExternal; 
extern "C" PTSYSTEMAPI char PTS_StrPath_DataExternal[0X10000];

#include "PTWindowImpl.h"
static PTWWindowImpl *s_pWindowImpl_Singleton = NULL; //PTInvokeMain线程写入 UI线程读取

#include <sys/wait.h>

extern "C" JNIEXPORT void ANativeActivity_onCreate(ANativeActivity *pActivity, void *, size_t)
{
	static bool bIsProcessCreate = true;
	if (bIsProcessCreate)
	{
		//PTSystem
		PTS_iFDDir_DataExternal = ::open64(pActivity->externalDataPath, O_RDONLY);
		assert(PTS_iFDDir_DataExternal != -1);
		size_t sResult = ::strlcpy(PTS_StrPath_DataExternal, pActivity->externalDataPath, 0X10000U);
		assert(sResult != 0U);

		int iResult = ::chdir(pActivity->internalDataPath);
		assert(iResult == 0);

		PTSThread hThreadInvoke;
		PTBOOL tbResult = ::PTSThread_Create(&PTInvokeMain, &s_pWindowImpl_Singleton, &hThreadInvoke);
		assert(tbResult != false);

		//SpinLock
		while (::PTSAtomic_Get(reinterpret_cast<uintptr_t *>(&s_pWindowImpl_Singleton)) == static_cast<uintptr_t>(NULL))
		{
			::PTS_Yield();
		}

		bIsProcessCreate = false;
	}

	::PTSAtomic_Set(reinterpret_cast<uintptr_t * volatile>(&s_pWindowImpl_Singleton->m_pActivity_Cache), reinterpret_cast<uintptr_t>(pActivity));
	
	//只关心进程的生命期而不关心ANativeActivity的生命期
	pActivity->callbacks->onStart = NULL;
	pActivity->callbacks->onResume = NULL;
	pActivity->callbacks->onSaveInstanceState = NULL;
	pActivity->callbacks->onPause = NULL;
	pActivity->callbacks->onStop = NULL;
	pActivity->callbacks->onDestroy = ANativeActivity_onDestroy;

	//callbacks属于ANativeActivity对象的一部分，因此每次回调都需要重新写入，无论是否是进程创建
	pActivity->callbacks->onWindowFocusChanged = NULL;
	pActivity->callbacks->onNativeWindowCreated = ANativeActivity_onNativeWindowCreated;
	pActivity->callbacks->onNativeWindowResized = ANativeActivity_onNativeWindowResized;
	pActivity->callbacks->onNativeWindowRedrawNeeded = ANativeActivity_onNativeWindowRedrawNeeded;
	pActivity->callbacks->onNativeWindowDestroyed = ANativeActivity_onNativeWindowDestroyed;
	pActivity->callbacks->onInputQueueCreated = ANativeActivity_onInputQueueCreated;
	pActivity->callbacks->onInputQueueDestroyed = ANativeActivity_onInputQueueDestroyed;
	pActivity->callbacks->onContentRectChanged = NULL;
	pActivity->callbacks->onConfigurationChanged = NULL;
	pActivity->callbacks->onLowMemory = NULL;
}

static void ANativeActivity_onDestroy(ANativeActivity *)
{
	::PTSAtomic_Set(reinterpret_cast<uintptr_t * volatile>(&s_pWindowImpl_Singleton->m_pActivity_Cache), static_cast<uintptr_t>(NULL));
}

static void ANativeActivity_onInputQueueCreated(ANativeActivity *, AInputQueue *__pInputQueue)
{
	ALooper* pLooper = ::ALooper_forThread();
	assert(pLooper != NULL);

	::AInputQueue_attachLooper(
		__pInputQueue,
		pLooper,
		0,//在CallBack非空时,Identifier被忽略;在UI线程调用ALooper_pollOnce时，始终返回ALOOPER_POLL_CALLBACK   
		[](int fd, int events, void *pVoid)->int {

		AInputQueue *pInputQueue = static_cast<AInputQueue *>(pVoid);

		AInputEvent *pInputEvent;
		while (::AInputQueue_getEvent(pInputQueue, &pInputEvent) >= 0)
		{
			//如果没有对所有非预处理的事件调用AInputQueue_finishEvent并且在handled中传入非0值，那么系统会认为应用程序“未响应”！！！
			if (::AInputQueue_preDispatchEvent(pInputQueue, pInputEvent) == 0)
			{
				if (s_pWindowImpl_Singleton->m_pEventInputCallback != NULL)
				{
					AInputEvent_getType(pInputEvent);

					//s_pWindowImpl_Singleton->m_pEventInputCallback
				}

				::AInputQueue_finishEvent(pInputQueue, pInputEvent, 1);
			}
		}

		return 1;
	},
		__pInputQueue
		);
}

static void ANativeActivity_onInputQueueDestroyed(ANativeActivity *, AInputQueue *__pInputQueue)
{
	ALooper* pLooper = ::ALooper_forThread();
	assert(pLooper != NULL);

	::AInputQueue_detachLooper(__pInputQueue);
}

static void ANativeActivity_onNativeWindowCreated(ANativeActivity *pActivity, ANativeWindow *pWindow)
{
	void (PTPTR * pHere_EventOutputCallback)(void *pUserData, void *pEventData);

	//SpinLock
	while ((pHere_EventOutputCallback = reinterpret_cast<void (PTPTR *)(void *pUserData, void *pEventData)>(
		::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(&s_pWindowImpl_Singleton->m_pEventOutputCallback))
		)) == NULL)
	{
		::PTS_Yield();
	}

	void *pHere_EventOutputCallback_UserData = reinterpret_cast<void *>(
		::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(&s_pWindowImpl_Singleton->m_pEventOutputCallback_UserData))
		);

	IPTWWindow::EventOutput_WindowCreated EventData;
	EventData.m_Type = IPTWWindow::EventOutput::Type_WindowCreated;
	EventData.m_hDisplay = NULL;
	EventData.m_hWindow = pWindow;

	pHere_EventOutputCallback(pHere_EventOutputCallback_UserData, &EventData);
}

static void ANativeActivity_onNativeWindowResized(ANativeActivity *, ANativeWindow *pWindow)
{
	void (PTPTR * pHere_EventOutputCallback)(void *pUserData, void *pEventData);

	//SpinLock
	while ((pHere_EventOutputCallback = reinterpret_cast<void (PTPTR *)(void *pUserData, void *pEventData)>(
			::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(&s_pWindowImpl_Singleton->m_pEventOutputCallback))
			)) == NULL)
	{
		::PTS_Yield();
	}

	void *pHere_EventOutputCallback_UserData = reinterpret_cast<void *>(
		::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(&s_pWindowImpl_Singleton->m_pEventOutputCallback_UserData))
		);

	IPTWWindow::EventOutput_WindowResized EventData;
	EventData.m_Type = IPTWWindow::EventOutput::Type_WindowResized;
	EventData.m_hDisplay = NULL;
	EventData.m_hWindow = pWindow;
	EventData.m_Width = static_cast<uint32_t>(::ANativeWindow_getWidth(pWindow));
	EventData.m_Height = static_cast<uint32_t>(::ANativeWindow_getHeight(pWindow));

	pHere_EventOutputCallback(pHere_EventOutputCallback_UserData, &EventData);
}

static void ANativeActivity_onNativeWindowRedrawNeeded(ANativeActivity *, ANativeWindow *)
{
	//Frame Throttling
	//To Minimize The Latency

	//[Harris 2014] Peter Harris "The Mali GPU: An Abstract Machine, Part 1 - Frame Pipelining." ARM Graphics and Gaming blog 2014.
	//https://community.arm.com/developer/tools-software/graphics/b/blog/posts/the-mali-gpu-an-abstract-machine-part-1---frame-pipelining

	//Structure Pallerl //Pipeline Pattern //Map Pattern

}

static void ANativeActivity_onNativeWindowDestroyed(ANativeActivity *, ANativeWindow *)
{
	void (PTPTR * pHere_EventOutputCallback)(void *pUserData, void *pEventData);

	//SpinLock
	while ((pHere_EventOutputCallback = reinterpret_cast<void (PTPTR *)(void *pUserData, void *pEventData)>(
		::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(&s_pWindowImpl_Singleton->m_pEventOutputCallback))
		)) == NULL)
	{
		::PTS_Yield();
	}

	void *pHere_EventOutputCallback_UserData = reinterpret_cast<void *>(
		::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(&s_pWindowImpl_Singleton->m_pEventOutputCallback_UserData))
		);

	struct
	{
		uint32_t m_Type;
		uint32_t m_Width;
		uint32_t m_Height;
	}EventOutput_WindowDestroyed;
	EventOutput_WindowDestroyed.m_Type = IPTWWindow::EventOutput::Type_WindowDestroyed;

	pHere_EventOutputCallback(pHere_EventOutputCallback_UserData, &EventOutput_WindowDestroyed);
}

#include "../../../Public/App/PTAExport.h"
static void *PTInvokeMain(void *pVoid)
{
	PTWWindowImpl l_WindowImpl_Singleton;
	
	(*static_cast<PTWWindowImpl **>(pVoid)) = &l_WindowImpl_Singleton;

	char CmdLine[] = { "PTLauncher" };
	char *argv[1] = { CmdLine };

	int iResult = ::PTAMain(static_cast<IPTWWindow *>(&l_WindowImpl_Singleton), 1, argv);

	//PTApp中设置的CallBack可能会引起访问违规
	l_WindowImpl_Singleton.EventOutputCallback_Hook(NULL, [](void *pUserData, void *pInputData)->void {});
	l_WindowImpl_Singleton.EventInputCallback_Hook(NULL, [](void *pUserData, void *pInputData)->void {});

	//Finish Acitivity
	ANativeActivity *pActivity = reinterpret_cast<ANativeActivity *>(::PTSAtomic_Get(reinterpret_cast<uintptr_t * volatile>(&s_pWindowImpl_Singleton->m_pActivity_Cache)));
	if (pActivity != NULL)
	{
		::ANativeActivity_finish(pActivity);
	}

	//Wait For The UI Thread
	for (uint32_t i = 0U; i < 0X10000U; ++i)
	{
		pActivity = reinterpret_cast<ANativeActivity *>(::PTSAtomic_Get(reinterpret_cast<uintptr_t * volatile>(&s_pWindowImpl_Singleton->m_pActivity_Cache)));
		if (pActivity == NULL)
		{
			break;
		}
		::PTS_Yield();
	}

	//Terminate Process
	std::exit(iResult);

	//Never Goes Here
	return NULL;
}

inline PTWWindowImpl::PTWWindowImpl()
{
	m_pEventOutputCallback_UserData = NULL;
	m_pEventOutputCallback = NULL;

	m_pEventInputCallback_UserData = NULL;
	m_pEventInputCallback = NULL;
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
	assert(0);
}

void PTWWindowImpl::Position_Set(uint32_t, uint32_t)
{
	assert(0);
}

void PTWWindowImpl::Size_Set(uint32_t, uint32_t)
{
	assert(0);
}