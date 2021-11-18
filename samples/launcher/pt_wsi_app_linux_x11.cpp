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

#include <string>
#include <pt_wsi_main.h>
#include <pt_mcrt_malloc.h>
#include <pt_mcrt_scalable_allocator.h>
#include "pt_wsi_app_base.h"

using mcrt_string = std::basic_string<char, std::char_traits<char>, mcrt::scalable_allocator<char>>;

class wsi_app_linux_11 : public wsi_app_base
{
public:
    void init(pt_gfx_connection_ref gfx_connection);
};

void wsi_app_linux_11::init(pt_gfx_connection_ref gfx_connection)
{
    this->wsi_app_base::init(gfx_connection);
}

inline pt_wsi_app_ref wrap(class wsi_app_linux_11 *wsi_app) { return reinterpret_cast<pt_wsi_app_ref>(wsi_app); }
inline class wsi_app_linux_11 *unwrap(pt_wsi_app_ref wsi_app) { return reinterpret_cast<class wsi_app_linux_11 *>(wsi_app); }

static pt_wsi_app_ref PT_PTR wsi_app_init(pt_gfx_connection_ref gfx_connection)
{
    class wsi_app_linux_11 *wsi_app = new (mcrt_aligned_malloc(sizeof(class wsi_app_linux_11), alignof(class wsi_app_linux_11))) wsi_app_linux_11();
    wsi_app->init(gfx_connection);
    return wrap(wsi_app);
}

static int PT_PTR wsi_app_main(pt_wsi_app_ref wsi_app)
{
    return unwrap(wsi_app)->main();
}

pt_gfx_input_stream_ref PT_CALL cache_input_stream_init_callback(char const *initial_filename);
int PT_CALL cache_input_stream_stat_size_callback(pt_gfx_input_stream_ref cache_input_stream, int64_t *size);
intptr_t PT_PTR cache_input_stream_read_callback(pt_gfx_input_stream_ref cache_input_stream, void *data, size_t size);
void PT_PTR cache_input_stream_destroy_callback(pt_gfx_input_stream_ref cache_input_stream);
pt_gfx_output_stream_ref PT_PTR cache_output_stream_init_callback(char const *initial_filename);
intptr_t PT_PTR cache_output_stream_write_callback(pt_gfx_output_stream_ref cache_output_stream, void *data, size_t size);
void PT_PTR cache_output_stream_destroy_callback(pt_gfx_output_stream_ref cache_output_stream);

int main(int argc, char *argv[])
{
    return pt_wsi_main(argc, argv, wsi_app_init, wsi_app_main, cache_input_stream_init_callback, cache_input_stream_stat_size_callback, cache_input_stream_read_callback, cache_input_stream_destroy_callback, cache_output_stream_init_callback, cache_output_stream_write_callback, cache_output_stream_destroy_callback);
}

#include <pt_mcrt_thread.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

pt_gfx_input_stream_ref PT_CALL cache_input_stream_init_callback(char const *initial_filename)
{
    mcrt_string path = "./bin/";
    path += initial_filename;

    int fd = openat(AT_FDCWD, path.c_str(), O_RDONLY);

    return reinterpret_cast<pt_gfx_input_stream_ref>(static_cast<intptr_t>(fd));
}

int PT_CALL cache_input_stream_stat_size_callback(pt_gfx_input_stream_ref cache_input_stream, int64_t *size)
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

intptr_t PT_PTR cache_input_stream_read_callback(pt_gfx_input_stream_ref cache_input_stream, void *data, size_t size)
{
    int fd = static_cast<int>(reinterpret_cast<intptr_t>(cache_input_stream));

    ssize_t res_read;
    while ((-1 == (res_read = read(fd, data, size))) && (EINTR == errno))
    {
        mcrt_os_yield();
    }
    return res_read;
}

void PT_PTR cache_input_stream_destroy_callback(pt_gfx_input_stream_ref cache_input_stream)
{
    int fd = static_cast<int>(reinterpret_cast<intptr_t>(cache_input_stream));

    close(fd);
}

pt_gfx_output_stream_ref PT_PTR cache_output_stream_init_callback(char const *initial_filename)
{
    mcrt_string path = "./bin/";
    path += initial_filename;

    int fd = openat(AT_FDCWD, path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    return reinterpret_cast<pt_gfx_output_stream_ref>(static_cast<intptr_t>(fd));
}

intptr_t PT_PTR cache_output_stream_write_callback(pt_gfx_output_stream_ref cache_output_stream, void *data, size_t size)
{
    int fd = static_cast<int>(reinterpret_cast<intptr_t>(cache_output_stream));

    ssize_t res_write;
    while ((-1 == (res_write = write(fd, data, size))) && (EINTR == errno))
    {
        mcrt_os_yield();
    }

    return res_write;
}

void PT_PTR cache_output_stream_destroy_callback(pt_gfx_output_stream_ref cache_output_stream)
{
    int fd = static_cast<int>(reinterpret_cast<intptr_t>(cache_output_stream));

    close(fd);
}

bool gfx_texture_read_file(pt_gfx_connection_ref gfx_connection, pt_gfx_texture_ref texture, char const *initial_filename)
{
    mcrt_string path = "./assets/";
    path += initial_filename;

    return pt_gfx_texture_read_input_stream(
        gfx_connection,
        texture,
        path.c_str(),
        [](char const *initial_filename) -> pt_gfx_input_stream_ref
        {
            int fd = openat(AT_FDCWD, initial_filename, O_RDONLY);
#if 0
            if (-1 != fd)
            {
                instance_gfx_input_stream_verify_support.insert(fd);
            }
#endif
            return reinterpret_cast<pt_gfx_input_stream_ref>(static_cast<intptr_t>(fd));
        },
        [](pt_gfx_input_stream_ref gfx_input_stream, void *buf, size_t count) -> intptr_t
        {
            ssize_t res_read = read(static_cast<int>(reinterpret_cast<intptr_t>(gfx_input_stream)), buf, count);
            return res_read;
        },
        [](pt_gfx_input_stream_ref gfx_input_stream, int64_t offset, int whence) -> int64_t
        {
            off_t res_lseek = lseek(static_cast<int>(reinterpret_cast<intptr_t>(gfx_input_stream)), offset, whence);
            return res_lseek;
        },
        [](pt_gfx_input_stream_ref gfx_input_stream) -> void
        {
            int fd = static_cast<int>(reinterpret_cast<intptr_t>(gfx_input_stream));
#if 0
            instance_gfx_input_stream_verify_support.erase(fd);
#endif
            close(fd);
        });
}

bool gfx_mesh_read_file(pt_gfx_connection_ref gfx_connection, pt_gfx_mesh_ref mesh, uint32_t mesh_index, uint32_t material_index, char const *initial_filename)
{
    mcrt_string path = "./assets/";
    ;
    path += initial_filename;

    return pt_gfx_mesh_read_input_stream(
        gfx_connection,
        mesh,
        mesh_index,
        material_index,
        path.c_str(),
        [](char const *initial_filename) -> pt_gfx_input_stream_ref
        {
            int fd = openat(AT_FDCWD, initial_filename, O_RDONLY);
#if 0
            if (-1 != fd)
            {
                instance_gfx_input_stream_verify_support.insert(fd);
            }
#endif
            return reinterpret_cast<pt_gfx_input_stream_ref>(static_cast<intptr_t>(fd));
        },
        [](pt_gfx_input_stream_ref gfx_input_stream, void *buf, size_t count) -> intptr_t
        {
            ssize_t res_read = read(static_cast<int>(reinterpret_cast<intptr_t>(gfx_input_stream)), buf, count);
            return res_read;
        },
        [](pt_gfx_input_stream_ref gfx_input_stream, int64_t offset, int whence) -> int64_t
        {
            off_t res_lseek = lseek(static_cast<int>(reinterpret_cast<intptr_t>(gfx_input_stream)), offset, whence);
            return res_lseek;
        },
        [](pt_gfx_input_stream_ref gfx_input_stream) -> void
        {
            int fd = static_cast<int>(reinterpret_cast<intptr_t>(gfx_input_stream));
#if 0
            instance_gfx_input_stream_verify_support.erase(fd);
#endif
            close(fd);
        });
}

static_assert(SEEK_SET == PT_GFX_INPUT_STREAM_SEEK_SET, "");
static_assert(SEEK_CUR == PT_GFX_INPUT_STREAM_SEEK_CUR, "");
static_assert(SEEK_END == PT_GFX_INPUT_STREAM_SEEK_END, "");