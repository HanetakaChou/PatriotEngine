/*
 * Copyright (C) YuqiaoZhang(HanetakaYuminaga)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <android/native_activity.h>

extern "C" JNIEXPORT void ANativeActivity_onCreate(ANativeActivity * pActivity, void*, size_t)
{
	static bool bIsProcessCreate = true;
	if (bIsProcessCreate)
	{
		//PTMcRT
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
		while (::PTSAtomic_Get(reinterpret_cast<uintptr_t*>(&s_pWindowImpl_Singleton)) == static_cast<uintptr_t>(NULL))
		{
			::PTS_Yield();
		}

		bIsProcessCreate = false;
	}

	::PTSAtomic_Set(reinterpret_cast<uintptr_t* volatile>(&s_pWindowImpl_Singleton->m_pActivity_Cache), reinterpret_cast<uintptr_t>(pActivity));

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

static void ANativeActivity_onInputQueueCreated(ANativeActivity*, AInputQueue* __pInputQueue)
{
	ALooper* pLooper = ::ALooper_forThread();
	assert(pLooper != NULL);

	::AInputQueue_attachLooper(
		__pInputQueue,
		pLooper,
		0,//在CallBack非空时,Identifier被忽略;在UI线程调用ALooper_pollOnce时，始终返回ALOOPER_POLL_CALLBACK   
		[](int fd, int events, void* pVoid)->int {

			AInputQueue* pInputQueue = static_cast<AInputQueue*>(pVoid);

			AInputEvent* pInputEvent;
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

static void ANativeActivity_onInputQueueDestroyed(ANativeActivity*, AInputQueue* __pInputQueue)
{
	ALooper* pLooper = ::ALooper_forThread();
	assert(pLooper != NULL);

	::AInputQueue_detachLooper(__pInputQueue);
}
