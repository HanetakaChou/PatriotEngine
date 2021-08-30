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
#include <string.h>
#include <assert.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pt_mcrt_thread.h>
#include <pt_mcrt_atomic.h>
#include <pt_mcrt_malloc.h>
#include <pt_mcrt_scalable_allocator.h>
#include <pt_gfx_connection.h>

using mcrt_string = std::basic_string<char, std::char_traits<char>, mcrt::scalable_allocator<char> >;
static mcrt_string wsi_mach_ios_library_path;

extern "C" void get_library_directory(char *, size_t *);
extern "C" void cocoa_set_multithreaded(void);
extern "C" bool cocoa_is_multithreaded(void);
extern "C" int application_main(int, char *[]);

int main(int argc, char *argv[])
{
    // Standard Library Directory
    {
        size_t library_directory_path_length_before;
        get_library_directory(NULL, &library_directory_path_length_before);

        char *library_directory_path = static_cast<char *>(mcrt_aligned_malloc(sizeof(char) * (library_directory_path_length_before + 1), alignof(char)));
        size_t library_directory_path_length;
        get_library_directory(library_directory_path, &library_directory_path_length);
        assert(library_directory_path_length_before == library_directory_path_length);

        wsi_mach_ios_library_path.assign(library_directory_path, library_directory_path + library_directory_path_length);

        mcrt_aligned_free(library_directory_path);
    }

    // Enable MultiThreaded
    // https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/MemoryMgmt/Articles/mmAutoreleasePools.html
    {
        cocoa_set_multithreaded();
        assert(cocoa_is_multithreaded());
    }

    return application_main(argc, argv);
}

// neutral app
#include "pt_wsi_neutral_app.h"

struct wsi_mach_ios_app_main_argument_t
{
    pt_gfx_connection_ref m_gfx_connection;
    void **m_void_instance;
    bool m_has_inited;
};
static void *wsi_mach_ios_app_main(void *argument);
mcrt_native_thread_id wsi_mach_ios_app_main_thread_id;

extern "C" void *gfx_connection_init_callback(void *layer, float width, float height, void **void_instance)
{
    pt_gfx_connection_ref gfx_connection = gfx_connection_init(NULL, NULL, wsi_mach_ios_library_path.c_str());
    PT_MAYBE_UNUSED bool res_on_wsi_window_created = gfx_connection_on_wsi_window_created(gfx_connection, NULL, reinterpret_cast<wsi_window_ref>(layer), width, height);
    assert(res_on_wsi_window_created);

    struct wsi_mach_ios_app_main_argument_t wsi_mach_ios_app_main_argument;
    wsi_mach_ios_app_main_argument.m_gfx_connection = gfx_connection;
    wsi_mach_ios_app_main_argument.m_void_instance = void_instance;
    wsi_mach_ios_app_main_argument.m_has_inited = false;
    PT_MAYBE_UNUSED bool res_native_thread_create = mcrt_native_thread_create(&wsi_mach_ios_app_main_thread_id, wsi_mach_ios_app_main, &wsi_mach_ios_app_main_argument);
    assert(res_native_thread_create);
    while (!mcrt_atomic_load(&wsi_mach_ios_app_main_argument.m_has_inited))
    {
        mcrt_os_yield();
    }

    return gfx_connection;
}

extern "C" void gfx_connection_redraw_callback(void *gfx_connection_void)
{
    pt_gfx_connection_ref gfx_connection = static_cast<pt_gfx_connection_ref>(gfx_connection_void);
    gfx_connection_draw_acquire(gfx_connection);
    gfx_connection_draw_release(gfx_connection);
}

static void *wsi_mach_ios_app_main(void *argument_void)
{
    struct wsi_mach_ios_app_main_argument_t *argument = static_cast<struct wsi_mach_ios_app_main_argument_t *>(argument_void);

    //Init
    {
        PT_MAYBE_UNUSED bool res_neutral_app_init = wsi_neutral_app_init(argument->m_gfx_connection, argument->m_void_instance);
        assert(res_neutral_app_init);
        mcrt_atomic_store(&argument->m_has_inited, true);
    }

    int res_neutral_app_main = wsi_neutral_app_main((*argument->m_void_instance));

    return reinterpret_cast<void *>(static_cast<intptr_t>(res_neutral_app_main));
}

bool gfx_texture_read_file(pt_gfx_connection_ref gfx_connection, gfx_texture_ref texture, char const *initial_filename)
{
    mcrt_string path = wsi_mach_ios_library_path.c_str();
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

bool gfx_mesh_read_file(pt_gfx_connection_ref gfx_connection, gfx_mesh_ref mesh, uint32_t mesh_index, uint32_t material_index, char const *initial_filename)
{
    mcrt_string path = wsi_mach_ios_library_path.c_str();
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

static_assert(SEEK_SET == PT_GFX_INPUT_STREAM_SEEK_SET, "");
static_assert(SEEK_CUR == PT_GFX_INPUT_STREAM_SEEK_CUR, "");
static_assert(SEEK_END == PT_GFX_INPUT_STREAM_SEEK_END, "");
