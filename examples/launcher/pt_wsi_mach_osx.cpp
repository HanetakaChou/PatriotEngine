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
#include <string>

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

#include <pt_gfx_connection.h>

extern "C" void *gfx_connection_init_callback(void *layer, float width, float height)
{
    gfx_connection_ref gfx_connection = gfx_connection_init(NULL, NULL);
    bool res_on_wsi_window_created = gfx_connection_on_wsi_window_created(gfx_connection, NULL, reinterpret_cast<wsi_window_ref>(layer), width, height);
    assert(res_on_wsi_window_created);
    return gfx_connection;
}

extern "C" void gfx_connection_redraw_callback(void *gfx_connection_void)
{
    gfx_connection_ref gfx_connection = static_cast<gfx_connection_ref>(gfx_connection_void);
    gfx_connection_on_wsi_redraw_needed_acquire(gfx_connection);
    gfx_connection_on_wsi_redraw_needed_release(gfx_connection);
}
