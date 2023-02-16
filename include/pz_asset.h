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

#ifndef _PZ_ASSET_H_
#define _PZ_ASSET_H_ 1

#include <pt_gfx_connection.h>

// TODO: remove in the future
PT_ATTR_GFX bool PT_CALL pt_asset_pmx_get_material_count(char const* initial_filename, pt_input_stream_init_callback input_stream_init_callback, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback, pt_input_stream_destroy_callback input_stream_destroy_callback, size_t *out_material_count);
PT_ATTR_GFX bool PT_CALL pt_asset_pmx_get_material_texture_paths(char const* initial_filename, pt_input_stream_init_callback input_stream_init_callback, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback, pt_input_stream_destroy_callback input_stream_destroy_callback, char** out_material_texture_path, size_t* out_material_texture_size);

#endif