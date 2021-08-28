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
#include <assert.h>
#include <string>
#include <android/native_activity.h>
#include <pt_mcrt_thread.h>
#include <pt_mcrt_atomic.h>
#include <pt_mcrt_scalable_allocator.h>
#include <pt_gfx_connection.h>

static void ANativeActivity_onDestroy(ANativeActivity *native_activity);
static void ANativeActivity_onInputQueueCreated(ANativeActivity *native_activity, AInputQueue *input_queue);
static void ANativeActivity_onInputQueueDestroyed(ANativeActivity *native_activity, AInputQueue *input_queue);
static void ANativeActivity_onNativeWindowCreated(ANativeActivity *native_activity, ANativeWindow *native_window);
static void ANativeActivity_onNativeWindowDestroyed(ANativeActivity *native_activity, ANativeWindow *native_window);
static void ANativeActivity_onNativeWindowResized(ANativeActivity *native_activity, ANativeWindow *native_window);
static void ANativeActivity_onNativeWindowRedrawNeeded(ANativeActivity *native_activity, ANativeWindow *native_window);

using mcrt_string = std::basic_string<char, std::char_traits<char>, mcrt::scalable_allocator<char>>;

static ANativeActivity *wsi_linux_android_native_activity = NULL;
static mcrt_string wsi_linux_android_internal_data_path;
static ANativeWindow *wsi_linux_android_native_window = NULL;
static gfx_connection_ref wsi_linux_android_gfx_connection = NULL;
static void *wsi_linux_android_void_instance = NULL;

struct wsi_linux_android_app_main_argument_t
{
	gfx_connection_ref m_gfx_connection;
	void **m_void_instance;
	bool m_has_inited;
};
static void *wsi_linux_android_app_main(void *argument);
static mcrt_native_thread_id wsi_linux_android_app_main_thread_id;

extern "C" JNIEXPORT void ANativeActivity_onCreate(ANativeActivity *native_activity, void *, size_t)
{
	wsi_linux_android_native_activity = native_activity;
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

	static bool app_process_on_create = true;
	if (PT_UNLIKELY(app_process_on_create))
	{
		wsi_linux_android_internal_data_path = native_activity->internalDataPath;

		wsi_linux_android_gfx_connection = gfx_connection_init(NULL, NULL, native_activity->internalDataPath);

		struct wsi_linux_android_app_main_argument_t linux_android_app_main_argument;
		linux_android_app_main_argument.m_gfx_connection = wsi_linux_android_gfx_connection;
		linux_android_app_main_argument.m_void_instance = &wsi_linux_android_void_instance;
		linux_android_app_main_argument.m_has_inited = false;

		PT_MAYBE_UNUSED bool res_native_thread_create = mcrt_native_thread_create(&wsi_linux_android_app_main_thread_id, wsi_linux_android_app_main, &linux_android_app_main_argument);
		assert(res_native_thread_create);

		while (!mcrt_atomic_load(&linux_android_app_main_argument.m_has_inited))
		{
			mcrt_os_yield();
		}

		app_process_on_create = false;
	}
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
	wsi_linux_android_native_window = native_window;
	bool res_on_wsi_window_created = gfx_connection_on_wsi_window_created(wsi_linux_android_gfx_connection, NULL, reinterpret_cast<wsi_window_ref>(native_window), ANativeWindow_getWidth(native_window), ANativeWindow_getHeight(native_window));
	assert(res_on_wsi_window_created);
}

static void ANativeActivity_onNativeWindowDestroyed(ANativeActivity *native_activity, ANativeWindow *native_window)
{
	assert(native_activity == wsi_linux_android_native_activity);
	assert(native_window == wsi_linux_android_native_window);
	wsi_linux_android_native_window = NULL;
	gfx_connection_on_wsi_window_destroyed(wsi_linux_android_gfx_connection);
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

#include "pt_wsi_neutral_app.h"
static void *wsi_linux_android_app_main(void *argument_void)
{
	struct wsi_linux_android_app_main_argument_t *argument = static_cast<struct wsi_linux_android_app_main_argument_t *>(argument_void);
	bool res_neutral_app_init = wsi_neutral_app_init(argument->m_gfx_connection, argument->m_void_instance);
	assert(res_neutral_app_init);
	mcrt_atomic_store(&argument->m_has_inited, true);

	int res_neutral_app_main = wsi_neutral_app_main((*argument->m_void_instance));

	//mcrt_atomic_store(&self->m_loop, false);

	//wsi_window_app_destroy() //used in run //wsi_window_app_handle_event

	return reinterpret_cast<void *>(static_cast<intptr_t>(res_neutral_app_main));
}

// neutral app file system
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

bool gfx_texture_read_file(gfx_connection_ref gfx_connection, gfx_texture_ref texture, char const *initial_filename)
{
	mcrt_string path = wsi_linux_android_internal_data_path;
	path += '/';
	path += initial_filename;

	return gfx_texture_read_input_stream(
		gfx_connection,
		texture,
		path.c_str(),
		[](char const *initial_filename) -> gfx_input_stream_ref
		{
			int fd = openat(AT_FDCWD, initial_filename, O_RDONLY);
			return reinterpret_cast<gfx_input_stream_ref>(static_cast<intptr_t>(fd));
		},
		[](gfx_input_stream_ref gfx_input_stream, void *buf, size_t count) -> intptr_t
		{
			ssize_t res_read = read(static_cast<int>(reinterpret_cast<intptr_t>(gfx_input_stream)), buf, count);
			return res_read;
		},
		[](gfx_input_stream_ref gfx_input_stream, int64_t offset, int whence) -> int64_t
		{
			off_t res_lseek = lseek(static_cast<int>(reinterpret_cast<intptr_t>(gfx_input_stream)), offset, whence);
			return res_lseek;
		},
		[](gfx_input_stream_ref gfx_input_stream) -> void
		{
			close(static_cast<int>(reinterpret_cast<intptr_t>(gfx_input_stream)));
		});
}

bool gfx_mesh_read_file(gfx_connection_ref gfx_connection, gfx_mesh_ref mesh, uint32_t mesh_index, uint32_t material_index, char const *initial_filename)
{
	mcrt_string path = wsi_linux_android_internal_data_path;
	path += '/';
	path += initial_filename;

	return gfx_mesh_read_input_stream(
		gfx_connection,
		mesh,
		mesh_index,
		material_index,
		path.c_str(),
		[](char const *initial_filename) -> gfx_input_stream_ref
		{
			int fd = openat(AT_FDCWD, initial_filename, O_RDONLY);
			return reinterpret_cast<gfx_input_stream_ref>(static_cast<intptr_t>(fd));
		},
		[](gfx_input_stream_ref gfx_input_stream, void *buf, size_t count) -> intptr_t
		{
			ssize_t _res = read(static_cast<int>(reinterpret_cast<intptr_t>(gfx_input_stream)), buf, count);
			return _res;
		},
		[](gfx_input_stream_ref gfx_input_stream, int64_t offset, int whence) -> int64_t
		{
			off_t _res = lseek(static_cast<int>(reinterpret_cast<intptr_t>(gfx_input_stream)), offset, whence);
			return _res;
		},
		[](gfx_input_stream_ref gfx_input_stream) -> void
		{
			close(static_cast<int>(reinterpret_cast<intptr_t>(gfx_input_stream)));
		});
}

static_assert(SEEK_SET == PT_GFX_INPUT_STREAM_SEEK_SET, "");
static_assert(SEEK_CUR == PT_GFX_INPUT_STREAM_SEEK_CUR, "");
static_assert(SEEK_END == PT_GFX_INPUT_STREAM_SEEK_END, "");