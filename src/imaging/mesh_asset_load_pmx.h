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

#ifndef _IMAGING_MESH_ASSET_LOAD_PMX_H_
#define _IMAGING_MESH_ASSET_LOAD_PMX_H_ 1

#include <stddef.h>
#include <stdint.h>
#include "mesh_asset_load.h"

//--------------------------------------------------------------------------------------
// This is an English description of the .PMX file format used in Miku Miku Dance (MMD)
//
// https://gist.github.com/felixjones/f8a06bd48f9da9a4539f
//--------------------------------------------------------------------------------------

// [Model::load](https://github.com/sugiany/blender_mmd_tools/blob/master/mmd_tools/core/pmx/__init__.py)
// [ReadPMXFile](https://github.com/benikabocha/saba/blob/master/src/Saba/Model/MMD/PMXFile.cpp)

enum
{
    PMX_HEADER_SIGNATURE = 0X20584d50, //'P''M''X'' '
};

extern bool mesh_load_pmx_header_from_input_stream(
    struct mesh_asset_header_t *out_mesh_asset_header,
    pt_input_stream_ref input_stream, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback);

extern bool mesh_load_pmx_primitive_headers_from_input_stream(
    struct mesh_asset_header_t const *mesh_asset_header_for_validate,
    struct mesh_primitive_asset_header_t *out_mesh_primitive_asset_header,
    pt_input_stream_ref input_stream, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback);

extern bool mesh_load_pmx_primitive_data_from_input_stream(
    struct mesh_asset_header_t const *mesh_asset_header_for_validate,
    struct mesh_primitive_asset_header_t const *mesh_primitive_asset_header_for_validate,
    void *staging_pointer,
    struct pt_gfx_mesh_neutral_primitive_memcpy_dest_t const *memcpy_dests,
    pt_input_stream_ref input_stream, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback,
    char **out_material_texture_path, size_t *out_material_texture_size);

#endif