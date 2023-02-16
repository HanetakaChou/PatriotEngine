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

#ifndef _IMAGING_MATERIAL_LOAD_PMX_H_
#define _IMAGING_MATERIAL_LOAD_PMX_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_gfx_connection.h>

struct material_asset_pmx_header
{
    bool is_text_encoding_utf8;
    uint32_t texture_index_size;
    uint32_t texture_count;
    uint32_t material_count;
    int64_t texture_section_offset;
    int64_t material_section_offset;
};

extern bool material_asset_load_pmx_header_from_input_stream(
    struct material_asset_pmx_header *out_mateiral_asset_pmx_header, 
    pt_input_stream_ref input_stream, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback);


extern bool mesh_load_pmx_primitive_data_from_input_stream(
    struct material_asset_pmx_header const *mateiral_asset_pmx_header_for_validate, 
    char **out_material_texture_path, size_t * out_material_texture_size);

#endif