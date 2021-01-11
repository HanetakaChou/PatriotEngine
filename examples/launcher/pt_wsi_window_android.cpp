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

#include <stddef.h>
#include <stdint.h>
#include <android/native_activity.h>
#include <assert.h>

static void ANativeActivity_onInputQueueCreated(ANativeActivity *, AInputQueue *input_queue);
static void ANativeActivity_onInputQueueDestroyed(ANativeActivity *, AInputQueue *input_queue);

extern "C" JNIEXPORT void ANativeActivity_onCreate(ANativeActivity *pActivity, void *, size_t)
{
	static bool bIsProcessCreate = true;
	if (bIsProcessCreate)
	{
		// 
		bIsProcessCreate = false;
	}

	pActivity->callbacks->onStart = NULL;
	pActivity->callbacks->onResume = NULL;
	pActivity->callbacks->onSaveInstanceState = NULL;
	pActivity->callbacks->onPause = NULL;
	pActivity->callbacks->onStop = NULL;
	pActivity->callbacks->onDestroy = NULL;

	pActivity->callbacks->onWindowFocusChanged = NULL;
	pActivity->callbacks->onNativeWindowCreated = NULL;
	pActivity->callbacks->onNativeWindowResized = NULL;
	pActivity->callbacks->onNativeWindowRedrawNeeded = NULL;
	pActivity->callbacks->onNativeWindowDestroyed = NULL;
	pActivity->callbacks->onInputQueueCreated = ANativeActivity_onInputQueueCreated;
	pActivity->callbacks->onInputQueueDestroyed = ANativeActivity_onInputQueueDestroyed;
	pActivity->callbacks->onContentRectChanged = NULL;
	pActivity->callbacks->onConfigurationChanged = NULL;
	pActivity->callbacks->onLowMemory = NULL;
}

static void ANativeActivity_onInputQueueCreated(ANativeActivity *, AInputQueue *input_queue)
{
	ALooper *looper = ALooper_forThread();
	assert(looper != NULL);

	AInputQueue_attachLooper(
		input_queue,
		looper,
		0, //Identifier is ignored when callback is not NULL. ALooper_pollOnce always returns the ALOOPER_POLL_CALLBACK when invoked by the UI thread.
		[](int fd, int events, void *__input_queue) -> int {
			AInputQueue *input_queue = static_cast<AInputQueue *>(__input_queue);

			AInputEvent *input_event;
			while (AInputQueue_getEvent(input_queue, &input_event) >= 0)
			{
				//The app will be "No response" if we don't call AInputQueue_finishEvent and pass the non-zero value for all events which is not pre-dispatched.
				if (0==AInputQueue_preDispatchEvent(input_queue, input_event))
				{
					AInputEvent_getType(input_event);

					//EventInputCallback

					AInputQueue_finishEvent(input_queue, input_event, 1);
				}
			}

			return 1;
		},
		input_queue);
}

static void ANativeActivity_onInputQueueDestroyed(ANativeActivity *, AInputQueue *input_queue)
{
	ALooper *looper = ::ALooper_forThread();
	assert(looper != NULL);

	AInputQueue_detachLooper(input_queue);
}
