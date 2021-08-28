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

#ifndef _PT_GFX_CONNECTION_UTILS_H_
#define _PT_GFX_CONNECTION_UTILS_H_ 1

#include <pt_common.h>
#include <pt_gfx_connection.h>

#if defined(PT_POSIX)
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

inline bool gfx_texture_read_file(gfx_connection_ref gfx_connection, gfx_texture_ref texture, char const *initial_filename)
{
    return gfx_texture_read_input_stream(
        gfx_connection,
        texture,
        initial_filename,
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

inline bool PT_CALL gfx_mesh_read_file(gfx_connection_ref gfx_connection, gfx_mesh_ref mesh, uint32_t mesh_index, uint32_t material_index, char const *initial_filename)
{
    return gfx_mesh_read_input_stream(
        gfx_connection,
        mesh,
        mesh_index,
        material_index,
        initial_filename,
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

#elif defined(PT_WIN32)
#include <winsdkver.h>
#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>

inline bool gfx_texture_read_file(gfx_connection_ref gfx_connection, gfx_texture_ref texture, char const *initial_filename)
{
    return texture->read_input_stream(
        initial_filename,
        [](char const *initial_filename) -> gfx_input_stream_ref { /*ToDo: convert utf8 to utf16 */ HANDLE hFile = CreateFileA(initial_filename, FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); return reinterpret_cast<gfx_input_stream_ref>(static_cast<void*>(hFile)); },
        [](gfx_input_stream_ref gfx_input_stream, void *buf, size_t count) -> intptr_t
        {
            DWORD _numberOfBytesRead;
            BOOL _res = ReadFile(static_cast<HANDLE>(reinterpret_cast<void *>(gfx_input_stream)), buf, count, &_numberOfBytesRead, NULL);
            return ((_res != FALSE) ? _numberOfBytesRead : -1);
        },
        [](gfx_input_stream_ref gfx_input_stream, int64_t offset, int whence) -> int64_t
        {
            off64_t _res = SetFilePointerEx(static_cast<int>(reinterpret_cast<intptr_t>(gfx_input_stream)), offset, whence);
            return _res;
        },
        [](gfx_input_stream_ref gfx_input_stream) -> void
        {
            CloseHandle(static_cast<HANDLE>(reinterpret_cast<void *>(gfx_input_stream)));
        });
}

static_assert(FILE_BEGIN == PT_GFX_INPUT_STREAM_SEEK_SET, "");
static_assert(FILE_CURRENT == PT_GFX_INPUT_STREAM_SEEK_CUR, "");
static_assert(FILE_END == PT_GFX_INPUT_STREAM_SEEK_END, "");

#else
#error Unknown Platform
#endif

#endif
