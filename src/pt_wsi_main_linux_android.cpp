//
// Copyright (C) YuqiaoZhang(HanetakaYuminaga)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <new>
#include <sys/socket.h>
#include <android/native_activity.h>
#include <pt_mcrt_thread.h>
#include <pt_mcrt_atomic.h>
#include <pt_mcrt_malloc.h>
#include <pt_gfx_connection.h>
#include <pt_wsi_main.h>

static void ANativeActivity_onDestroy(ANativeActivity *native_activity);
static void ANativeActivity_onWindowFocusChanged(ANativeActivity *native_activity, int hasFocus);
static void ANativeActivity_onNativeWindowCreated(ANativeActivity *native_activity, ANativeWindow *native_window);
static void ANativeActivity_onNativeWindowResized(ANativeActivity *native_activity, ANativeWindow *native_window);
static void ANativeActivity_onNativeWindowRedrawNeeded(ANativeActivity *native_activity, ANativeWindow *native_window);
static void ANativeActivity_onNativeWindowDestroyed(ANativeActivity *native_activity, ANativeWindow *native_window);
static void ANativeActivity_onInputQueueCreated(ANativeActivity *native_activity, AInputQueue *input_queue);
static void ANativeActivity_onInputQueueDestroyed(ANativeActivity *native_activity, AInputQueue *input_queue);

static ANativeActivity *g_wsi_linux_android_native_activity = NULL;
static ANativeWindow *g_wsi_linux_android_native_window = NULL;

class wsi_linux_android
{
	mcrt_native_thread_id m_periodic_timer_main_thread_id;
	mcrt_native_thread_id m_app_main_thread_id;

	pt_gfx_connection_ref m_gfx_connection;
	int m_main_thread_alooper_fd_read;
	static int main_thread_alooper_callback(int fd, int events, void *data);
	void draw_on_main_thread();

	int m_main_thread_alooper_fd_write;
	bool m_has_focus;
	bool m_periodic_timer_main_running;
	static void *periodic_timer_main(void *argument);
	void request_draw_on_main_thread();
	// TODO
	// padding for cache line
	pt_wsi_app_ref m_wsi_app;

	struct app_main_argument_t
	{
		class wsi_linux_android *m_instance;
		char const *m_internal_data_path;
		pt_wsi_app_ref(PT_PTR *m_wsi_app_init_callback)(pt_gfx_connection_ref, char const *);
		int(PT_PTR *m_wsi_app_main_callback)(pt_wsi_app_ref);
		bool m_has_inited;
	};
	static void *app_main(void *argument_void);

public:
	inline wsi_linux_android();
	bool init(char const *internal_data_path, pt_wsi_app_ref(PT_PTR *wsi_app_init_callback)(pt_gfx_connection_ref, char const *), int(PT_PTR *wsi_app_main_callback)(pt_wsi_app_ref));
	void on_window_focus_changed(bool has_focus);
	bool on_window_created(ANativeWindow *native_window);
	void on_window_resized(ANativeWindow *native_window);
	void on_window_redraw_needed();
	void on_window_destroyed();
};

static class wsi_linux_android *g_wsi_linux_android_instance = NULL;

PT_ATTR_WSI void PT_CALL pt_wsi_main(ANativeActivity *native_activity, void *, size_t, pt_wsi_app_ref(PT_PTR *wsi_app_init_callback)(pt_gfx_connection_ref, char const *), int(PT_PTR *wsi_app_main_callback)(pt_wsi_app_ref))
{
	assert(NULL == g_wsi_linux_android_native_activity);
	g_wsi_linux_android_native_activity = native_activity;

	native_activity->callbacks->onStart = NULL;
	native_activity->callbacks->onResume = NULL;
	native_activity->callbacks->onSaveInstanceState = NULL;
	native_activity->callbacks->onPause = NULL;
	native_activity->callbacks->onStop = NULL;
	native_activity->callbacks->onDestroy = ANativeActivity_onDestroy;
	native_activity->callbacks->onWindowFocusChanged = ANativeActivity_onWindowFocusChanged;
	native_activity->callbacks->onNativeWindowCreated = ANativeActivity_onNativeWindowCreated;
	native_activity->callbacks->onNativeWindowResized = ANativeActivity_onNativeWindowResized;
	native_activity->callbacks->onNativeWindowRedrawNeeded = ANativeActivity_onNativeWindowRedrawNeeded;
	native_activity->callbacks->onNativeWindowDestroyed = ANativeActivity_onNativeWindowDestroyed;
	native_activity->callbacks->onInputQueueCreated = ANativeActivity_onInputQueueCreated;
	native_activity->callbacks->onInputQueueDestroyed = ANativeActivity_onInputQueueDestroyed;
	native_activity->callbacks->onContentRectChanged = NULL;
	native_activity->callbacks->onConfigurationChanged = NULL;
	native_activity->callbacks->onLowMemory = NULL;

	if (PT_UNLIKELY(NULL == g_wsi_linux_android_instance))
	{
		g_wsi_linux_android_instance = new (mcrt_aligned_malloc(sizeof(class wsi_linux_android), alignof(wsi_linux_android))) wsi_linux_android();
		assert(NULL != g_wsi_linux_android_instance);

		PT_MAYBE_UNUSED bool res_linux_android_instance_init = g_wsi_linux_android_instance->init(native_activity->internalDataPath, wsi_app_init_callback, wsi_app_main_callback);
		assert(res_linux_android_instance_init);
	}
}

static void ANativeActivity_onDestroy(ANativeActivity *native_activity)
{
	assert(native_activity == g_wsi_linux_android_native_activity);
	g_wsi_linux_android_native_activity = NULL;
}

static void ANativeActivity_onWindowFocusChanged(ANativeActivity *native_activity, int hasFocus)
{
	assert(native_activity == g_wsi_linux_android_native_activity);
	g_wsi_linux_android_instance->on_window_focus_changed(0 != hasFocus);
}

static void ANativeActivity_onNativeWindowCreated(ANativeActivity *native_activity, ANativeWindow *native_window)
{
	assert(native_activity == g_wsi_linux_android_native_activity);
	g_wsi_linux_android_native_window = native_window;

	PT_MAYBE_UNUSED bool res_on_window_created = g_wsi_linux_android_instance->on_window_created(native_window);
	assert(res_on_window_created);
}

static void ANativeActivity_onNativeWindowResized(ANativeActivity *native_activity, ANativeWindow *native_window)
{
	assert(native_activity == g_wsi_linux_android_native_activity);
	assert(native_window == g_wsi_linux_android_native_window);

	g_wsi_linux_android_instance->on_window_resized(native_window);
}

static void ANativeActivity_onNativeWindowRedrawNeeded(ANativeActivity *native_activity, ANativeWindow *native_window)
{
	assert(native_activity == g_wsi_linux_android_native_activity);
	assert(native_window == g_wsi_linux_android_native_window);

	g_wsi_linux_android_instance->on_window_redraw_needed();
}

static void ANativeActivity_onNativeWindowDestroyed(ANativeActivity *native_activity, ANativeWindow *native_window)
{
	assert(native_activity == g_wsi_linux_android_native_activity);
	assert(native_window == g_wsi_linux_android_native_window);
	g_wsi_linux_android_native_window = NULL;

	g_wsi_linux_android_instance->on_window_destroyed();
}

static void ANativeActivity_onInputQueueCreated(ANativeActivity *native_activity, AInputQueue *input_queue)
{
	assert(native_activity == g_wsi_linux_android_native_activity);

	ALooper *looper = ALooper_forThread();
	assert(looper != NULL);

	AInputQueue_attachLooper(
		input_queue,
		looper,
		0, //Identifier is ignored when callback is not NULL. ALooper_pollOnce always returns the ALOOPER_POLL_CALLBACK when invoked by the UI thread.
		[](int fd, int, void *input_queue_void) -> int
		{
			AInputQueue *input_queue = static_cast<AInputQueue *>(input_queue_void);

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
							assert(NULL != g_wsi_linux_android_native_activity);
							ANativeActivity_finish(g_wsi_linux_android_native_activity);
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
	assert(native_activity == g_wsi_linux_android_native_activity);

	PT_MAYBE_UNUSED ALooper *looper = ALooper_forThread();
	assert(looper != NULL);

	AInputQueue_detachLooper(input_queue);
}

inline wsi_linux_android::wsi_linux_android() : m_gfx_connection(NULL), m_main_thread_alooper_fd_read(-1), m_main_thread_alooper_fd_write(-1), m_has_focus(true), m_wsi_app(NULL)
{
}

bool wsi_linux_android::init(char const *internal_data_path, pt_wsi_app_ref(PT_PTR *wsi_app_init_callback)(pt_gfx_connection_ref, char const *), int(PT_PTR *wsi_app_main_callback)(pt_wsi_app_ref))
{
	//
	this->m_gfx_connection = pt_gfx_connection_init(NULL, NULL, internal_data_path);
	assert(NULL != this->m_gfx_connection);

	// the "display_link_output_main"
	{
		ALooper *looper = ALooper_forThread();
		assert(looper != NULL);

		// we use "SOCK_STREAM" since the "draw_on_main_thread" may be slower than the "request_draw_on_main_thread"
		int sv[2];
		PT_MAYBE_UNUSED int res_socketpair = socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
		assert(0 == res_socketpair);
		this->m_main_thread_alooper_fd_read = sv[0];
		this->m_main_thread_alooper_fd_write = sv[1];

		// the "main_thread_alooper_callback"
		// the identifier is ignored when callback is not NULL
		// and the ALooper_pollOnce always returns the ALOOPER_POLL_CALLBACK when invoked by the main thread
		PT_MAYBE_UNUSED int res_looper_add_fd = ALooper_addFd(looper, this->m_main_thread_alooper_fd_read, -1, ALOOPER_EVENT_INPUT, main_thread_alooper_callback, this);
		assert(1 == res_looper_add_fd);

		// the "periodic_timer_main"
		mcrt_atomic_store(&this->m_periodic_timer_main_running, false);
		PT_MAYBE_UNUSED bool res_native_thread_create = mcrt_native_thread_create(&this->m_periodic_timer_main_thread_id, periodic_timer_main, this);
		assert(res_native_thread_create);

		while (!mcrt_atomic_load(&this->m_periodic_timer_main_running))
		{
			mcrt_os_yield();
		}
	}

	// the "app_main"
	{
		struct app_main_argument_t app_main_argument;
		app_main_argument.m_instance = this;
		app_main_argument.m_internal_data_path = internal_data_path;
		app_main_argument.m_wsi_app_init_callback = wsi_app_init_callback;
		app_main_argument.m_wsi_app_main_callback = wsi_app_main_callback;
		app_main_argument.m_has_inited = false;

		PT_MAYBE_UNUSED bool res_native_thread_create = mcrt_native_thread_create(&this->m_app_main_thread_id, app_main, &app_main_argument);
		assert(res_native_thread_create);

		while (!mcrt_atomic_load(&app_main_argument.m_has_inited))
		{
			mcrt_os_yield();
		}

		assert(NULL != this->m_wsi_app);
	}

	return true;
}

int wsi_linux_android::main_thread_alooper_callback(int fd, int events, void *data)
{
	class wsi_linux_android *instance = static_cast<class wsi_linux_android *>(data);
	assert(fd == instance->m_main_thread_alooper_fd_read);
	assert(ALOOPER_EVENT_INPUT == events);

	// we use "SOCK_STREAM" since the "draw_on_main_thread" may be slower than the "request_draw_on_main_thread"
	// and we read all the pending "requests" once here
	{
		uint8_t buf[4096];
		ssize_t res_recv;
		while ((-1 == (res_recv = recv(fd, buf, 4096U, 0))) && (EINTR == errno))
		{
			mcrt_os_yield();
		}
		assert(-1 != res_recv);
	}

	instance->draw_on_main_thread();

	return 1;
}

void wsi_linux_android::draw_on_main_thread()
{
	pt_gfx_connection_draw_acquire(this->m_gfx_connection);

	pt_gfx_connection_draw_release(this->m_gfx_connection);
}

void *wsi_linux_android::periodic_timer_main(void *argument_void)
{
	class wsi_linux_android *instance;
	// periodic_timer_init
	{

		instance = static_cast<class wsi_linux_android *>(argument_void);
		mcrt_atomic_store(&instance->m_periodic_timer_main_running, true);
	}

	// periodic_timer_main
	while (mcrt_atomic_load(&instance->m_periodic_timer_main_running))
	{
		// 60 FPS
		uint32_t milli_second = 1000U / 60U;
		mcrt_os_sleep(milli_second);

		if (mcrt_atomic_load(&instance->m_has_focus))
		{
			instance->request_draw_on_main_thread();
		}
	}

	return NULL;
}

void wsi_linux_android::request_draw_on_main_thread()
{
	uint8_t buf[1] = {7}; // seven is the luck number
	ssize_t res_send;
	while ((-1 == (res_send = send(this->m_main_thread_alooper_fd_write, buf, 1U, 0))) && (EINTR == errno))
	{
		mcrt_os_yield();
	}
	assert(1 == res_send);
}

void *wsi_linux_android::app_main(void *argument_void)
{
	pt_wsi_app_ref wsi_app;
	int(PT_PTR * wsi_app_main_callback)(pt_wsi_app_ref);
	// app_init
	{
		struct app_main_argument_t *argument = static_cast<struct app_main_argument_t *>(argument_void);
		wsi_app = argument->m_wsi_app_init_callback(argument->m_instance->m_gfx_connection, argument->m_internal_data_path);
		wsi_app_main_callback = argument->m_wsi_app_main_callback;
		argument->m_instance->m_wsi_app = wsi_app;
		mcrt_atomic_store(&argument->m_has_inited, true);
	}

	// app_main
	int res_app_main_callback = wsi_app_main_callback(wsi_app);

	//mcrt_atomic_store(&self->m_loop, false);

	//wsi_window_app_destroy() //used in run //wsi_window_app_handle_event

	return reinterpret_cast<void *>(static_cast<intptr_t>(res_app_main_callback));
}

void wsi_linux_android::on_window_focus_changed(bool has_focus)
{
	mcrt_atomic_store(&this->m_has_focus, has_focus);
}

bool wsi_linux_android::on_window_created(ANativeWindow *native_window)
{
	return pt_gfx_connection_on_wsi_window_created(this->m_gfx_connection, NULL, reinterpret_cast<pt_gfx_wsi_window_ref>(native_window), ANativeWindow_getWidth(native_window), ANativeWindow_getHeight(native_window));
}

void wsi_linux_android::on_window_resized(ANativeWindow *native_window)
{
	return pt_gfx_connection_on_wsi_window_resized(this->m_gfx_connection, ANativeWindow_getWidth(native_window), ANativeWindow_getHeight(native_window));
}

void wsi_linux_android::on_window_redraw_needed()
{
	this->request_draw_on_main_thread();
	return;
}

void wsi_linux_android::on_window_destroyed()
{
	return pt_gfx_connection_on_wsi_window_destroyed(this->m_gfx_connection);
}