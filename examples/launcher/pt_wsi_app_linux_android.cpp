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

#include <android/native_activity.h>
#include <pt_wsi_main.h>

static pt_wsi_app_ref PT_PTR wsi_app_init(pt_gfx_connection_ref gfx_connection, char const *internal_data_path);
static int PT_PTR wsi_app_main(pt_wsi_app_ref wsi_app);

extern "C" JNIEXPORT void ANativeActivity_onCreate(ANativeActivity *native_activity, void *savedState, size_t savedStateSize)
{
	return pt_wsi_main(native_activity, savedState, savedStateSize, wsi_app_init, wsi_app_main);
}

#include <pt_mcrt_malloc.h>
#include "pt_wsi_app_base.h"

class wsi_app_linux_android : public wsi_app_base
{
public:
	void init(pt_gfx_connection_ref gfx_connection, char const *internal_data_path);
};

inline pt_wsi_app_ref wrap(class wsi_app_linux_android *wsi_app) { return reinterpret_cast<pt_wsi_app_ref>(wsi_app); }
inline class wsi_app_linux_android *unwrap(pt_wsi_app_ref wsi_app) { return reinterpret_cast<class wsi_app_linux_android *>(wsi_app); }

static pt_wsi_app_ref PT_PTR wsi_app_init(pt_gfx_connection_ref gfx_connection, char const *internal_data_path)
{
	class wsi_app_linux_android *wsi_app = new(mcrt_aligned_malloc(sizeof(class wsi_app_linux_android), alignof(class wsi_app_linux_android)))wsi_app_linux_android();
	wsi_app->init(gfx_connection, internal_data_path);
	return wrap(wsi_app);
}

static int PT_PTR wsi_app_main(pt_wsi_app_ref wsi_app)
{
	return unwrap(wsi_app)->main();
}

#include <string>
#include <pt_mcrt_scalable_allocator.h>

using mcrt_string = std::basic_string<char, std::char_traits<char>, mcrt::scalable_allocator<char>>;
mcrt_string g_wsi_app_linux_android_internal_data_path;

void wsi_app_linux_android::init(pt_gfx_connection_ref gfx_connection, char const *internal_data_path)
{
	this->wsi_app_base::init(gfx_connection);
	g_wsi_app_linux_android_internal_data_path = internal_data_path;
}

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

bool gfx_texture_read_file(pt_gfx_connection_ref gfx_connection, pt_gfx_texture_ref texture, char const *initial_filename)
{
	mcrt_string path = g_wsi_app_linux_android_internal_data_path;
	path += '/';
	path += initial_filename;

	return pt_gfx_texture_read_input_stream(
		gfx_connection,
		texture,
		path.c_str(),
		[](char const *initial_filename) -> pt_gfx_input_stream_ref
		{
			int fd = openat(AT_FDCWD, initial_filename, O_RDONLY);
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
			close(static_cast<int>(reinterpret_cast<intptr_t>(gfx_input_stream)));
		});
}

bool gfx_mesh_read_file(pt_gfx_connection_ref gfx_connection, pt_gfx_mesh_ref mesh, uint32_t mesh_index, uint32_t material_index, char const *initial_filename)
{
	mcrt_string path = g_wsi_app_linux_android_internal_data_path;
	path += '/';
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
			close(static_cast<int>(reinterpret_cast<intptr_t>(gfx_input_stream)));
		});
}

static_assert(SEEK_SET == PT_GFX_INPUT_STREAM_SEEK_SET, "");
static_assert(SEEK_CUR == PT_GFX_INPUT_STREAM_SEEK_CUR, "");
static_assert(SEEK_END == PT_GFX_INPUT_STREAM_SEEK_END, "");