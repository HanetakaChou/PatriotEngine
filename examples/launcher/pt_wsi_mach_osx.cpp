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
#include <stdlib.h>
#include <assert.h>

extern "C" void lunarg_vulkan_sdk_setenv(void);
extern "C" void cocoa_set_multithreaded(void);
extern "C" bool cocoa_is_multithreaded(void);
extern "C" void application_set_delegate(void);
extern "C" int application_main(int argc, char const *argv[]);

int main(int argc, char const *argv[])
{
    // Lunarg Vulkan SDK
    {
        lunarg_vulkan_sdk_setenv();
    }

    // Enable MultiThreaded
    // https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/MemoryMgmt/Articles/mmAutoreleasePools.html
    {
        cocoa_set_multithreaded();
        assert(cocoa_is_multithreaded());
    }

    //Register NSApplicationDelegate
    {
        application_set_delegate();
    }

    return application_main(argc, argv);
}

#include <pt_mcrt_thread.h>
#include <pt_gfx_connection.h>
#include "pt_wsi_neutral_app.h"

struct wsi_mach_osx_app_main_argument_t
{
    gfx_connection_ref m_gfx_connection;
    void **m_void_instance;
} wsi_mach_osx_app_main_argument;
static void *wsi_mach_osx_app_main(void *argument);
mcrt_native_thread_id wsi_mach_osx_app_main_thread_id;

extern "C" void *gfx_connection_init_callback(void *layer, float width, float height, void **void_instance)
{
    gfx_connection_ref gfx_connection = gfx_connection_init(NULL, NULL);
    PT_MAYBE_UNUSED bool res_on_wsi_window_created = gfx_connection_on_wsi_window_created(gfx_connection, NULL, reinterpret_cast<wsi_window_ref>(layer), width, height);
    assert(res_on_wsi_window_created);

    wsi_mach_osx_app_main_argument.m_gfx_connection = gfx_connection;
    wsi_mach_osx_app_main_argument.m_void_instance = void_instance;
    PT_MAYBE_UNUSED bool res_native_thread_create = mcrt_native_thread_create(&wsi_mach_osx_app_main_thread_id, wsi_mach_osx_app_main, &wsi_mach_osx_app_main_argument);
    assert(res_native_thread_create);

    return gfx_connection;
}

extern "C" void gfx_connection_redraw_callback(void *gfx_connection_void)
{
    gfx_connection_ref gfx_connection = static_cast<gfx_connection_ref>(gfx_connection_void);
    gfx_connection_on_wsi_redraw_needed_acquire(gfx_connection);
    gfx_connection_on_wsi_redraw_needed_release(gfx_connection);
}

static void *wsi_mach_osx_app_main(void *argument_void)
{
    struct wsi_mach_osx_app_main_argument_t *argument = static_cast<struct wsi_mach_osx_app_main_argument_t *>(argument_void);
    PT_MAYBE_UNUSED bool res_neutral_app_init =  wsi_neutral_app_init(argument->m_gfx_connection, argument->m_void_instance);
    assert(res_neutral_app_init);

    int res_neutral_app_main = wsi_neutral_app_main(argument->m_void_instance);

    return reinterpret_cast<void *>(static_cast<intptr_t>(res_neutral_app_main));
}
