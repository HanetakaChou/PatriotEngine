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

static void ANativeActivity_onDestroy(ANativeActivity *native_activity);
static void ANativeActivity_onInputQueueCreated(ANativeActivity *native_activity, AInputQueue *input_queue);
static void ANativeActivity_onInputQueueDestroyed(ANativeActivity *native_activity, AInputQueue *input_queue);
static ANativeActivity *wsi_window_andoird_native_activity = NULL;

extern "C" JNIEXPORT void ANativeActivity_onCreate(ANativeActivity *native_activity, void *, size_t)
{
	static bool app_process_on_create = true;
	if (app_process_on_create)
	{
		//

		app_process_on_create = false;
	}

	native_activity->callbacks->onStart = NULL;
	native_activity->callbacks->onResume = NULL;
	native_activity->callbacks->onSaveInstanceState = NULL;
	native_activity->callbacks->onPause = NULL;
	native_activity->callbacks->onStop = NULL;
	native_activity->callbacks->onDestroy = ANativeActivity_onDestroy;

	native_activity->callbacks->onWindowFocusChanged = NULL;
	native_activity->callbacks->onNativeWindowCreated = NULL;
	native_activity->callbacks->onNativeWindowResized = NULL;
	native_activity->callbacks->onNativeWindowRedrawNeeded = NULL;
	native_activity->callbacks->onNativeWindowDestroyed = NULL;
	native_activity->callbacks->onInputQueueCreated = ANativeActivity_onInputQueueCreated;
	native_activity->callbacks->onInputQueueDestroyed = ANativeActivity_onInputQueueDestroyed;
	native_activity->callbacks->onContentRectChanged = NULL;
	native_activity->callbacks->onConfigurationChanged = NULL;
	native_activity->callbacks->onLowMemory = NULL;

	wsi_window_andoird_native_activity = native_activity;
}

static void ANativeActivity_onDestroy(ANativeActivity *native_activity)
{
	assert(native_activity == wsi_window_andoird_native_activity);
	wsi_window_andoird_native_activity = NULL;
}

static void ANativeActivity_onInputQueueCreated(ANativeActivity *native_activity, AInputQueue *input_queue)
{
	assert(native_activity == wsi_window_andoird_native_activity);

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
				if (0 == AInputQueue_preDispatchEvent(input_queue, input_event))
				{
					int handled = 0;

					int32_t type = AInputEvent_getType(input_event);
					switch (type)
					{
					case AINPUT_EVENT_TYPE_KEY:
					{
						int32_t key_code = AKeyEvent_getKeyCode(input_event);
						switch (key_code)
						{
						case AKEYCODE_HOME:
						{
							assert(NULL != wsi_window_andoird_native_activity);
							ANativeActivity_finish(wsi_window_andoird_native_activity);
							handled = 1;
						}
						break;
						default:
							break;
						}
					}
					break;
					default:
						break;
					}

					AInputQueue_finishEvent(input_queue, input_event, handled);
				}
			}
			return 1;
		},
		input_queue);
}

static void ANativeActivity_onInputQueueDestroyed(ANativeActivity *native_activity, AInputQueue *input_queue)
{
	assert(native_activity == wsi_window_andoird_native_activity);

	ALooper *looper = ALooper_forThread();
	assert(looper != NULL);

	AInputQueue_detachLooper(input_queue);
}
