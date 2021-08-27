﻿/*
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
#include <pt_mcrt_thread.h>
#include <pt_gfx_connection.h>
#include <assert.h>

static void ANativeActivity_onDestroy(ANativeActivity *native_activity);
static void ANativeActivity_onInputQueueCreated(ANativeActivity *native_activity, AInputQueue *input_queue);
static void ANativeActivity_onInputQueueDestroyed(ANativeActivity *native_activity, AInputQueue *input_queue);
static void ANativeActivity_onNativeWindowCreated(ANativeActivity *native_activity, ANativeWindow *native_window);
static void ANativeActivity_onNativeWindowDestroyed(ANativeActivity *native_activity, ANativeWindow *native_window);
static void ANativeActivity_onNativeWindowResized(ANativeActivity *native_activity, ANativeWindow *native_window);
static void ANativeActivity_onNativeWindowRedrawNeeded(ANativeActivity *native_activity, ANativeWindow *native_window);

static ANativeActivity *wsi_linux_android_native_activity = NULL;
static ANativeWindow *wsi_linux_android_native_window = NULL;
static gfx_connection_ref wsi_linux_android_gfx_connection = NULL;

static void *wsi_linux_android_app_main(void *arg);
mcrt_native_thread_id wsi_linux_android_app_main_thread_id;
//bool m_app_thread_running;

extern "C" JNIEXPORT void ANativeActivity_onCreate(ANativeActivity *native_activity, void *, size_t)
{
	static bool app_process_on_create = true;
	if (app_process_on_create)
	{
		wsi_linux_android_gfx_connection = gfx_connection_init(NULL, NULL);

		PT_MAYBE_UNUSED bool res_native_thread_create = mcrt_native_thread_create(&wsi_linux_android_app_main_thread_id, wsi_linux_android_app_main, NULL);
		assert(res_native_thread_create);

		app_process_on_create = false;
	}

	native_activity->callbacks->onStart = NULL;
	native_activity->callbacks->onResume = NULL;
	native_activity->callbacks->onSaveInstanceState = NULL;
	native_activity->callbacks->onPause = NULL;
	native_activity->callbacks->onStop = NULL;
	native_activity->callbacks->onDestroy = ANativeActivity_onDestroy;

	native_activity->callbacks->onWindowFocusChanged = NULL;
	native_activity->callbacks->onNativeWindowCreated = ANativeActivity_onNativeWindowCreated;
	native_activity->callbacks->onNativeWindowResized = ANativeActivity_onNativeWindowResized;
	native_activity->callbacks->onNativeWindowRedrawNeeded = ANativeActivity_onNativeWindowRedrawNeeded;
	native_activity->callbacks->onNativeWindowDestroyed = ANativeActivity_onNativeWindowDestroyed;
	native_activity->callbacks->onInputQueueCreated = ANativeActivity_onInputQueueCreated;
	native_activity->callbacks->onInputQueueDestroyed = ANativeActivity_onInputQueueDestroyed;
	native_activity->callbacks->onContentRectChanged = NULL;
	native_activity->callbacks->onConfigurationChanged = NULL;
	native_activity->callbacks->onLowMemory = NULL;

	wsi_linux_android_native_activity = native_activity;
}

static void ANativeActivity_onDestroy(ANativeActivity *native_activity)
{
	assert(native_activity == wsi_linux_android_native_activity);
	wsi_linux_android_native_activity = NULL;
}

static void ANativeActivity_onInputQueueCreated(ANativeActivity *native_activity, AInputQueue *input_queue)
{
	assert(native_activity == wsi_linux_android_native_activity);

	ALooper *looper = ALooper_forThread();
	assert(looper != NULL);

	AInputQueue_attachLooper(
		input_queue,
		looper,
		0, //Identifier is ignored when callback is not NULL. ALooper_pollOnce always returns the ALOOPER_POLL_CALLBACK when invoked by the UI thread.
		[](int fd, int events, void *__input_queue) -> int
		{
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
							assert(NULL != wsi_linux_android_native_activity);
							ANativeActivity_finish(wsi_linux_android_native_activity);
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
	assert(native_activity == wsi_linux_android_native_activity);

	ALooper *looper = ALooper_forThread();
	assert(looper != NULL);

	AInputQueue_detachLooper(input_queue);
}

static void ANativeActivity_onNativeWindowCreated(ANativeActivity *native_activity, ANativeWindow *native_window)
{
	assert(native_activity == wsi_linux_android_native_activity);
	bool res_on_wsi_window_created = gfx_connection_on_wsi_window_created(wsi_linux_android_gfx_connection, NULL, reinterpret_cast<wsi_window_ref>(native_window), ANativeWindow_getWidth(native_window), ANativeWindow_getHeight(native_window));
	assert(res_on_wsi_window_created);
	wsi_linux_android_native_window = native_window;
}

static void ANativeActivity_onNativeWindowDestroyed(ANativeActivity *native_activity, ANativeWindow *native_window)
{
	assert(native_activity == wsi_linux_android_native_activity);
	assert(native_window == wsi_linux_android_native_window);
	gfx_connection_on_wsi_window_destroyed(wsi_linux_android_gfx_connection);
	wsi_linux_android_native_window = NULL;
}

static void ANativeActivity_onNativeWindowResized(ANativeActivity *native_activity, ANativeWindow *native_window)
{
	assert(native_activity == wsi_linux_android_native_activity);
	assert(native_window == wsi_linux_android_native_window);
	gfx_connection_on_wsi_resized(wsi_linux_android_gfx_connection, ANativeWindow_getWidth(native_window), ANativeWindow_getHeight(native_window));
}

static void ANativeActivity_onNativeWindowRedrawNeeded(ANativeActivity *native_activity, ANativeWindow *native_window)
{
	assert(native_activity == wsi_linux_android_native_activity);
	assert(native_window == wsi_linux_android_native_window);
	gfx_connection_on_wsi_redraw_needed_acquire(wsi_linux_android_gfx_connection);
	gfx_connection_on_wsi_redraw_needed_release(wsi_linux_android_gfx_connection);
}

#include "pt_wsi_window_app.h"
wsi_window_app_ref wsi_linux_android_app;

static void *wsi_linux_android_app_main(void *arg)
{
	wsi_linux_android_app = wsi_window_app_init(wsi_linux_android_gfx_connection);
	assert(wsi_linux_android_app != NULL);
	//mcrt_atomic_store(&self->m_app_thread_running, true);

	int res = wsi_window_app_main(wsi_linux_android_app);

	//mcrt_atomic_store(&self->m_loop, false);

	//wsi_window_app_destroy() //used in run //wsi_window_app_handle_event

	return reinterpret_cast<void *>(static_cast<intptr_t>(res));
}