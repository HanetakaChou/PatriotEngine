//
// Copyright (C) YuqiaoZhang(HanetakaChou)
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

#include <string>
#include <pt_wsi_main.h>
#include <pt_mcrt_malloc.h>
#include <pt_mcrt_scalable_allocator.h>
#include "pt_wsi_app_base.h"

extern "C" void get_main_bundle_resource_path(char *, size_t *);
extern "C" void get_library_directory(char *path, size_t *length);

using mcrt_string = std::basic_string<char, std::char_traits<char>, mcrt::scalable_allocator<char> >;
static mcrt_string g_wsi_app_mach_osx_library_path;

static pt_wsi_app_ref PT_PTR launcher_app_init(int argc, char const *argv[], pt_gfx_connection_ref gfx_connection);
static int PT_PTR launcher_app_main(pt_wsi_app_ref wsi_app);

static pt_gfx_input_stream_ref PT_CALL cache_input_stream_init_callback(char const *initial_filename);
static int PT_CALL cache_input_stream_stat_size_callback(pt_gfx_input_stream_ref cache_input_stream, int64_t *size);
static intptr_t PT_PTR cache_input_stream_read_callback(pt_gfx_input_stream_ref cache_input_stream, void *data, size_t size);
static void PT_PTR cache_input_stream_destroy_callback(pt_gfx_input_stream_ref cache_input_stream);
static pt_gfx_output_stream_ref PT_PTR cache_output_stream_init_callback(char const *initial_filename);
static intptr_t PT_PTR cache_output_stream_write_callback(pt_gfx_output_stream_ref cache_output_stream, void *data, size_t size);
static void PT_PTR cache_output_stream_destroy_callback(pt_gfx_output_stream_ref cache_output_stream);

extern pt_gfx_input_stream_ref PT_PTR asset_input_stream_init_callback(char const *);
extern intptr_t PT_PTR asset_input_stream_read_callback(pt_gfx_input_stream_ref, void *, size_t);
extern int64_t PT_PTR asset_input_stream_seek_callback(pt_gfx_input_stream_ref, int64_t, int);
extern void PT_PTR asset_input_stream_destroy_callback(pt_gfx_input_stream_ref);

int main(int argc, char const *argv[])
{
    // Lunarg Vulkan SDK
    {
        size_t mainbundle_resource_path_length_before;
        get_main_bundle_resource_path(NULL, &mainbundle_resource_path_length_before);

        char *mainbundle_resource_path = static_cast<char *>(mcrt_aligned_malloc(sizeof(char) * (mainbundle_resource_path_length_before + 1), alignof(char)));
        size_t mainbundle_resource_path_length;
        get_main_bundle_resource_path(mainbundle_resource_path, &mainbundle_resource_path_length);
        assert(mainbundle_resource_path_length_before == mainbundle_resource_path_length);
        mainbundle_resource_path[mainbundle_resource_path_length] = '\0';

        PT_MAYBE_UNUSED int res_set_env_vk_layer_path = setenv("VK_LAYER_PATH", mainbundle_resource_path, 1);
        assert(0 == res_set_env_vk_layer_path);

        char const *moltenvk_icd_file_name = "MoltenVK_icd.json";

        size_t moltenvk_icd_file_name_length = strlen(moltenvk_icd_file_name);
        char *vk_icd_file_names = static_cast<char *>(mcrt_aligned_malloc(sizeof(char) * (mainbundle_resource_path_length + 1 + moltenvk_icd_file_name_length + 1), alignof(char)));
        assert(NULL != vk_icd_file_names);
        memcpy(vk_icd_file_names, mainbundle_resource_path, sizeof(char) * mainbundle_resource_path_length);
        vk_icd_file_names[mainbundle_resource_path_length] = '/';
        memcpy(vk_icd_file_names + mainbundle_resource_path_length + 1, moltenvk_icd_file_name, moltenvk_icd_file_name_length);
        vk_icd_file_names[mainbundle_resource_path_length + 1 + moltenvk_icd_file_name_length] = '\0';

        PT_MAYBE_UNUSED int res_set_env_vk_icd_file_names = setenv("VK_ICD_FILENAMES", vk_icd_file_names, 1);
        assert(0 == res_set_env_vk_icd_file_names);

        mcrt_aligned_free(mainbundle_resource_path);
        mcrt_aligned_free(vk_icd_file_names);
    }

    // Standard Library Directory
    {
        size_t library_directory_path_length_before;
        get_library_directory(NULL, &library_directory_path_length_before);

        char *library_directory_path = static_cast<char *>(mcrt_aligned_malloc(sizeof(char) * (library_directory_path_length_before + 1), alignof(char)));
        size_t library_directory_path_length;
        get_library_directory(library_directory_path, &library_directory_path_length);
        assert(library_directory_path_length_before == library_directory_path_length);

        g_wsi_app_mach_osx_library_path.assign(library_directory_path, library_directory_path + library_directory_path_length);

        mcrt_aligned_free(library_directory_path);
    }

    return pt_wsi_main(
        argc, argv,
        cache_input_stream_init_callback, cache_input_stream_stat_size_callback, cache_input_stream_read_callback, cache_input_stream_destroy_callback,
        cache_output_stream_init_callback, cache_output_stream_write_callback, cache_output_stream_destroy_callback,
        launcher_app_init, launcher_app_main);
}

inline pt_wsi_app_ref wrap(class launcher_app *wsi_app) { return reinterpret_cast<pt_wsi_app_ref>(wsi_app); }
inline class launcher_app *unwrap(pt_wsi_app_ref wsi_app) { return reinterpret_cast<class launcher_app *>(wsi_app); }

static pt_wsi_app_ref PT_PTR launcher_app_init(int argc, char const *argv[], pt_gfx_connection_ref gfx_connection)
{
    class launcher_app *wsi_app = new (mcrt_aligned_malloc(sizeof(class launcher_app), alignof(class launcher_app))) launcher_app();
    wsi_app->init(gfx_connection);
    return wrap(wsi_app);
}

static int PT_PTR launcher_app_main(pt_wsi_app_ref wsi_app)
{
    return unwrap(wsi_app)->main();
}

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pt_mcrt_thread.h>

static pt_gfx_input_stream_ref PT_CALL cache_input_stream_init_callback(char const *initial_filename)
{
    mcrt_string path = g_wsi_app_mach_osx_library_path;
    path += '/';
    path += initial_filename;

    int fd = openat(AT_FDCWD, path.c_str(), O_RDONLY);

    return reinterpret_cast<pt_gfx_input_stream_ref>(static_cast<intptr_t>(fd));
}

static int PT_CALL cache_input_stream_stat_size_callback(pt_gfx_input_stream_ref cache_input_stream, int64_t *size)
{
    int fd = static_cast<int>(reinterpret_cast<intptr_t>(cache_input_stream));

    struct stat buf;
    int res_fstat = fstat(fd, &buf);
    if (0 == res_fstat)
    {
        (*size) = buf.st_size;
        return 0;
    }
    else
    {
        (*size) = -1;
        return -1;
    }
}

static intptr_t PT_PTR cache_input_stream_read_callback(pt_gfx_input_stream_ref cache_input_stream, void *data, size_t size)
{
    int fd = static_cast<int>(reinterpret_cast<intptr_t>(cache_input_stream));

    ssize_t res_read;
    while ((-1 == (res_read = read(fd, data, size))) && (EINTR == errno))
    {
        mcrt_os_yield();
    }
    return res_read;
}

static void PT_PTR cache_input_stream_destroy_callback(pt_gfx_input_stream_ref cache_input_stream)
{
    int fd = static_cast<int>(reinterpret_cast<intptr_t>(cache_input_stream));

    int res_close = close(fd);
    assert(0 == res_close);
}

static pt_gfx_output_stream_ref PT_PTR cache_output_stream_init_callback(char const *initial_filename)
{
    mcrt_string path = g_wsi_app_mach_osx_library_path;
    path += '/';
    path += initial_filename;

    int fd = openat(AT_FDCWD, path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    return reinterpret_cast<pt_gfx_output_stream_ref>(static_cast<intptr_t>(fd));
}

static intptr_t PT_PTR cache_output_stream_write_callback(pt_gfx_output_stream_ref cache_output_stream, void *data, size_t size)
{
    int fd = static_cast<int>(reinterpret_cast<intptr_t>(cache_output_stream));

    ssize_t res_write;
    while ((-1 == (res_write = write(fd, data, size))) && (EINTR == errno))
    {
        mcrt_os_yield();
    }

    return res_write;
}

static void PT_PTR cache_output_stream_destroy_callback(pt_gfx_output_stream_ref cache_output_stream)
{
    int fd = static_cast<int>(reinterpret_cast<intptr_t>(cache_output_stream));

    int res_close = close(fd);
    assert(0 == res_close);
}

pt_gfx_input_stream_ref PT_PTR asset_input_stream_init_callback(char const *initial_filename)
{
    mcrt_string path = g_wsi_app_mach_osx_library_path;
    path += '/';
    path += initial_filename;

    int fd = openat(AT_FDCWD, path.c_str(), O_RDONLY);

    return reinterpret_cast<pt_gfx_input_stream_ref>(static_cast<intptr_t>(fd));
}

intptr_t PT_PTR asset_input_stream_read_callback(pt_gfx_input_stream_ref asset_input_stream, void *data, size_t size)
{
    int fd = static_cast<int>(reinterpret_cast<intptr_t>(asset_input_stream));

    ssize_t res_read;
    while ((-1 == (res_read = read(fd, data, size))) && (EINTR == errno))
    {
        mcrt_os_yield();
    }
    return res_read;
}

int64_t PT_PTR asset_input_stream_seek_callback(pt_gfx_input_stream_ref asset_input_stream, int64_t offset, int whence)
{
    int fd = static_cast<int>(reinterpret_cast<intptr_t>(asset_input_stream));

    static_assert(SEEK_SET == PT_GFX_INPUT_STREAM_SEEK_SET, "");
    static_assert(SEEK_CUR == PT_GFX_INPUT_STREAM_SEEK_CUR, "");
    static_assert(SEEK_END == PT_GFX_INPUT_STREAM_SEEK_END, "");
    off_t res_lseek = lseek(fd, offset, whence);
    return res_lseek;
}

void PT_PTR asset_input_stream_destroy_callback(pt_gfx_input_stream_ref asset_input_stream)
{
    int fd = static_cast<int>(reinterpret_cast<intptr_t>(asset_input_stream));

    int res_close = close(fd);
    assert(0 == res_close);
}
