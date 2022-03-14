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

#include <pt_wsi_main.h>
#include <pt_mcrt_malloc.h>
#include "pt_wsi_app_base.h"

static pt_wsi_app_ref PT_PTR launcher_app_init(int argc, char *argv[], pt_gfx_connection_ref gfx_connection);
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

int main(int argc, char *argv[])
{
    return pt_wsi_main(
        argc, argv,
        cache_input_stream_init_callback, cache_input_stream_stat_size_callback, cache_input_stream_read_callback, cache_input_stream_destroy_callback,
        cache_output_stream_init_callback, cache_output_stream_write_callback, cache_output_stream_destroy_callback,
        launcher_app_init, launcher_app_main);
}

inline pt_wsi_app_ref wrap(class launcher_app *wsi_app) { return reinterpret_cast<pt_wsi_app_ref>(wsi_app); }
inline class launcher_app *unwrap(pt_wsi_app_ref wsi_app) { return reinterpret_cast<class launcher_app *>(wsi_app); }

static pt_wsi_app_ref PT_PTR launcher_app_init(int argc, char *argv[], pt_gfx_connection_ref gfx_connection)
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
#include <assert.h>
#include <string>
#include <pt_mcrt_scalable_allocator.h>
#include <pt_mcrt_thread.h>

using mcrt_string = std::basic_string<char, std::char_traits<char>, mcrt::scalable_allocator<char>>;

static pt_gfx_input_stream_ref PT_CALL cache_input_stream_init_callback(char const *initial_filename)
{
    mcrt_string path = "./bin/";
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
    mcrt_string path = "./bin/";
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
    mcrt_string path = "./assets/";
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