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

#include <pz_asset.h>
#include "../mesh_asset_load.h"
#include "../mesh_asset_load_pmx.h"
#include <pt_mcrt_malloc.h>


PT_ATTR_GFX bool PT_CALL pt_asset_pmx_get_material_count(char const* initial_filename, pt_input_stream_init_callback input_stream_init_callback, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback, pt_input_stream_destroy_callback input_stream_destroy_callback, size_t* out_material_count)
{
    pt_input_stream_ref input_stream = input_stream_init_callback(initial_filename);
    if (pt_input_stream_ref(-1) == input_stream)
    {
        return false;
    }

    struct pt_gfx_input_stream_callbacks_t const input_stream_callbacks =
    {
        NULL,
        NULL,
        input_stream_read_callback,
        input_stream_seek_callback,
        NULL
    };

	struct mesh_asset_header_t mesh_asset_header;
    if (!mesh_load_pmx_header_from_input_stream(&mesh_asset_header, input_stream, &input_stream_callbacks))
    {
        input_stream_destroy_callback(input_stream);
        return false;
    }

    (*out_material_count) = mesh_asset_header.primitive_count;
    return true;
}


PT_ATTR_GFX bool PT_CALL pt_asset_pmx_get_material_texture_paths(char const* initial_filename, pt_input_stream_init_callback input_stream_init_callback, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback, pt_input_stream_destroy_callback input_stream_destroy_callback, char** out_material_texture_path, size_t* out_material_texture_size)
{
    pt_input_stream_ref input_stream = input_stream_init_callback(initial_filename);
    if (pt_input_stream_ref(-1) == input_stream)
    {
        return false;
    }

    struct pt_gfx_input_stream_callbacks_t const input_stream_callbacks =
    {
        NULL,
        NULL,
        input_stream_read_callback,
        input_stream_seek_callback,
        NULL
    };

    struct mesh_asset_header_t mesh_asset_header;
    if (!mesh_load_pmx_header_from_input_stream(&mesh_asset_header, input_stream, &input_stream_callbacks))
    {
        input_stream_destroy_callback(input_stream);
        return false;
    }

    struct pt_gfx_mesh_neutral_primitive_memcpy_dest_t* memcpy_dests = static_cast<struct pt_gfx_mesh_neutral_primitive_memcpy_dest_t*>(mcrt_aligned_malloc(sizeof(struct pt_gfx_mesh_neutral_primitive_memcpy_dest_t) * mesh_asset_header.primitive_count, alignof(struct pt_gfx_mesh_neutral_primitive_memcpy_dest_t)));

    mcrt_aligned_free(memcpy_dests);

    return true;
}
