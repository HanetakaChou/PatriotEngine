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
#include <vector>
// https://docs.microsoft.com/en-us/windows/win32/winprog/using-the-windows-headers#faster-builds-with-smaller-header-files
#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN 1
#define NOGDICAPMASKS 1
#define NOVIRTUALKEYCODES 1
// #define NOWINMESSAGES 1
// #define NOWINSTYLES 1
#define NOSYSMETRICS 1
#define NOMENUS 1
// #define NOICONS 1
#define NOKEYSTATES 1
#define NOSYSCOMMANDS 1
#define NORASTEROPS 1
#define NOSHOWWINDOW 1
#define NOATOM 1
#define NOCLIPBOARD 1
#define NOCOLOR 1
#define NOCTLMGR 1
#define NODRAWTEXT 1
#define NOGDI 1
#define NOKERNEL 1
// #define NOUSER 1
#define NONLS 1
#define NOMB 1
#define NOMEMMGR 1
#define NOMETAFILE 1
#define NOMINMAX 1
// #define NOMSG 1
#define NOOPENFILE 1
#define NOSCROLL 1
#define NOSERVICE 1
#define NOSOUND 1
#define NOTEXTMETRIC 1
#define NOWH 1
// #define NOWINOFFSETS 1
#define NOCOMM 1
#define NOKANJI 1
#define NOHELP 1
#define NOPROFILER 1
#define NODEFERWINDOWPOS 1
#define NOMCX 1
#include <Windows.h>
#include <pt_mcrt_memcpy.h>
#include <pt_mcrt_atomic.h>
#include <pt_mcrt_thread.h>
#include <pt_wsi_main.h>

class wsi_win32_desktop
{
    ATOM m_atom;
    HWND m_window;
    float m_window_width;
    float m_window_height;
    mcrt_native_thread_id m_draw_main_thread_id;
    mcrt_native_thread_id m_app_main_thread_id;
    bool m_win32_desktop_main_running;

    // TODO
    // padding for cache line

    pt_gfx_connection_ref m_gfx_connection;
    bool m_draw_main_running;
    static unsigned int __stdcall draw_main(void *);

    struct app_main_argument_t
    {
        class wsi_win32_desktop *m_instance;
        pt_wsi_app_ref(PT_PTR *m_wsi_app_init_callback)(pt_gfx_connection_ref);
        int(PT_PTR *m_wsi_app_main_callback)(pt_wsi_app_ref);
    };
    pt_wsi_app_ref m_wsi_app;
    bool m_app_main_running;
    static unsigned int __stdcall app_main(void *);

    static inline pt_gfx_wsi_window_ref wrap_wsi_window(HWND wsi_window);

public:
    inline void init(int cmd_show, pt_wsi_app_ref(PT_PTR *wsi_app_init_callback)(pt_gfx_connection_ref), int(PT_PTR *wsi_app_main_callback)(pt_wsi_app_ref));
    inline int main();
    inline LRESULT CALLBACK wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

PT_ATTR_WSI int PT_CALL pt_wsi_main(wchar_t *cmd_line, int cmd_show, pt_wsi_app_ref(PT_PTR *wsi_app_init_callback)(pt_gfx_connection_ref), int(PT_PTR *wsi_app_main_callback)(pt_wsi_app_ref))
{
    wsi_win32_desktop instance;
    instance.init(cmd_show, wsi_app_init_callback, wsi_app_main_callback);
    return instance.main();
}

extern "C" IMAGE_DOS_HEADER __ImageBase;

static LRESULT CALLBACK __internal_wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

inline void wsi_win32_desktop::init(int cmd_show, pt_wsi_app_ref(PT_PTR *wsi_app_init_callback)(pt_gfx_connection_ref), int(PT_PTR *wsi_app_main_callback)(pt_wsi_app_ref))
{
    HINSTANCE instance_this_component = reinterpret_cast<HINSTANCE>(&__ImageBase);

    //ATOM
    {
        WNDCLASSEXW wc;
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = 0U;
        wc.lpfnWndProc = __internal_wnd_proc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = sizeof(class wsi_win32_desktop *);
        wc.hInstance = instance_this_component;
        wc.hIcon = LoadIconW(instance_this_component, MAKEINTRESOURCEW(IDI_APPLICATION));
        wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
        wc.hbrBackground = NULL;
        wc.lpszMenuName = NULL;
        wc.lpszClassName = L"111";
        wc.hIconSm = wc.hIcon;
        this->m_atom = RegisterClassExW(&wc);
        assert(0 != this->m_atom);
    }

    this->m_window_width = 1280;
    this->m_window_height = 720;
    {
        this->m_window = CreateWindowExW(WS_EX_APPWINDOW,
                                         MAKEINTATOM(this->m_atom),
                                         L"PatriotEngine",
                                         WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_SIZEBOX,
                                         CW_USEDEFAULT, CW_USEDEFAULT, this->m_window_width, this->m_window_height,
                                         NULL,
                                         NULL,
                                         instance_this_component,
                                         this);
        assert(NULL != this->m_window);
        ShowWindow(this->m_window, cmd_show);
        UpdateWindow(this->m_window);
    }

    // draw_main
    {
        this->m_gfx_connection = NULL;
        mcrt_atomic_store(&this->m_draw_main_running, false);

        PT_MAYBE_UNUSED bool res_native_thread_create = mcrt_native_thread_create(&m_draw_main_thread_id, draw_main, this);
        assert(res_native_thread_create);

        while (!mcrt_atomic_load(&this->m_draw_main_running))
        {
            mcrt_os_yield();
        }

        assert(this->m_gfx_connection != NULL);
    }

    // app_main
    {
        struct app_main_argument_t app_main_argument;
        app_main_argument.m_instance = this;
        app_main_argument.m_wsi_app_init_callback = wsi_app_init_callback;
        app_main_argument.m_wsi_app_main_callback = wsi_app_main_callback;
        mcrt_atomic_store(&this->m_app_main_running, false);

        PT_MAYBE_UNUSED bool res_native_thread_create = mcrt_native_thread_create(&m_app_main_thread_id, app_main, &app_main_argument);
        assert(res_native_thread_create);

        while (!mcrt_atomic_load(&this->m_app_main_running))
        {
            mcrt_os_yield();
        }

        assert(NULL != this->m_wsi_app);
    }

    // win32_desktop_main
    mcrt_atomic_store(&this->m_win32_desktop_main_running, true);
}

inline int wsi_win32_desktop::main()
{
    MSG msg;
    while (mcrt_atomic_load(&this->m_win32_desktop_main_running) && (FALSE != GetMessageW(&msg, NULL, 0, 0)))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    mcrt_native_thread_join(this->m_app_main_thread_id);

    mcrt_native_thread_join(this->m_draw_main_thread_id);

    DestroyWindow(this->m_window);

    return 0;
}

static LRESULT CALLBACK __internal_wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        assert(WM_CREATE == uMsg);
        class wsi_win32_desktop *instance = static_cast<class wsi_win32_desktop *>(reinterpret_cast<CREATESTRUCT *>(lParam)->lpCreateParams);
        assert(NULL != instance);
        SetWindowLongPtrW(hWnd, 0U, reinterpret_cast<LONG_PTR>(instance));
        return 0;
    }
    default:
    {
        class wsi_win32_desktop *instance = reinterpret_cast<class wsi_win32_desktop *>(GetWindowLongPtrW(hWnd, 0U));
        return instance->wnd_proc(hWnd, uMsg, wParam, lParam);
    }
    }
}

inline LRESULT CALLBACK wsi_win32_desktop::wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_SIZE:
    {
        assert(WM_SIZE == uMsg);
        this->m_window_width = static_cast<uint32_t>(LOWORD(lParam));
        this->m_window_height = static_cast<uint32_t>(HIWORD(lParam));
        pt_gfx_connection_on_wsi_window_resized(this->m_gfx_connection, this->m_window_width, this->m_window_height);
        return 0;
    }
    default:
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }
}

inline pt_gfx_wsi_window_ref wsi_win32_desktop::wrap_wsi_window(HWND wsi_window)
{
    return reinterpret_cast<pt_gfx_wsi_window_ref>(reinterpret_cast<uintptr_t>(wsi_window));
}

unsigned int wsi_win32_desktop::draw_main(void *argument)
{
    class wsi_win32_desktop *instance = static_cast<class wsi_win32_desktop *>(argument);

    instance->m_gfx_connection = pt_gfx_connection_init(NULL, NULL, ".");
    assert(instance->m_gfx_connection != NULL);
    mcrt_atomic_store(&instance->m_draw_main_running, true);

    pt_gfx_connection_on_wsi_window_created(instance->m_gfx_connection, NULL, wrap_wsi_window(instance->m_window), instance->m_window_height, instance->m_window_width);

    while (mcrt_atomic_load(&instance->m_draw_main_running))
    {
        // usage
        // on_redraw_needed
        // app update scenetree //maybe in other thread //not depend on accurate time
        // gfx acquire //sync and flatten scenetree //then frame throttling
        // app update time-related (animation etc) //frame throttling make the time here less latency //scenetree modify will impact on the next frame
        // gfx release //draw and present //draw //not sync scenetree

        // update scenetree
        pt_gfx_connection_draw_acquire(instance->m_gfx_connection);

        // TODO
        // output visibility set

        // update animation etc
        pt_gfx_connection_draw_release(instance->m_gfx_connection);
    }

    pt_gfx_connection_on_wsi_window_destroyed(instance->m_gfx_connection);

    pt_gfx_connection_destroy(instance->m_gfx_connection);

    return 0U;
}

unsigned int wsi_win32_desktop::app_main(void *argument_void)
{
    pt_wsi_app_ref wsi_app;
    int(PT_PTR * wsi_app_main_callback)(pt_wsi_app_ref);
    // app_init
    {
        struct app_main_argument_t *argument = static_cast<struct app_main_argument_t *>(argument_void);
        wsi_app = argument->m_wsi_app_init_callback(argument->m_instance->m_gfx_connection);
        wsi_app_main_callback = argument->m_wsi_app_main_callback;
        argument->m_instance->m_wsi_app = wsi_app;
        mcrt_atomic_store(&argument->m_instance->m_app_main_running, true);
    }

    // app_main
    int res_app_main_callback = wsi_app_main_callback(wsi_app);

    // mcrt_atomic_store(&self->m_loop, false);

    // wsi_window_app_destroy() //used in run //wsi_window_app_handle_event

    return static_cast<unsigned int>(res_app_main_callback);
}