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

#include "mesh_asset_load.h"
#include "mesh_asset_load_pmx.h"

extern bool mesh_load_header_from_input_stream(
    struct mesh_asset_header_t *out_mesh_asset_header,
    pt_input_stream_ref input_stream, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback)
{
    if (-1 == input_stream_seek_callback(input_stream, 0, PT_INPUT_STREAM_SEEK_SET))
    {
        return false;
    }

    uint32_t fourcc_signature;
    {
        intptr_t res_read = input_stream_read_callback(input_stream, &fourcc_signature, sizeof(uint32_t));
        if (res_read == -1 || sizeof(uint32_t) != static_cast<size_t>(res_read))
        {
            return false;
        }
    }

    switch (fourcc_signature)
    {
    case PMX_HEADER_SIGNATURE:
        return mesh_load_pmx_header_from_input_stream(
            out_mesh_asset_header,
            input_stream, input_stream_read_callback, input_stream_seek_callback);
    default:
        return false;
    }
}

extern bool mesh_load_primitive_headers_from_input_stream(
    struct mesh_asset_header_t const *mesh_asset_header_for_validate, struct mesh_primitive_asset_header_t *out_mesh_primitive_asset_header,
    pt_input_stream_ref input_stream, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback)
{
    if (-1 == input_stream_seek_callback(input_stream, 0, PT_INPUT_STREAM_SEEK_SET))
    {
        return false;
    }

    uint32_t fourcc_signature;
    {
        intptr_t res_read = input_stream_read_callback(input_stream, &fourcc_signature, sizeof(uint32_t));
        if (res_read == -1 || sizeof(uint32_t) != static_cast<size_t>(res_read))
        {
            return false;
        }
    }

    switch (fourcc_signature)
    {
    case PMX_HEADER_SIGNATURE:
        return mesh_load_pmx_primitive_headers_from_input_stream(
            mesh_asset_header_for_validate,
            out_mesh_primitive_asset_header,
            input_stream, input_stream_read_callback, input_stream_seek_callback);
    default:
        return false;
    }
}

extern bool mesh_load_primitive_data_from_input_stream(
    struct mesh_asset_header_t const *mesh_asset_header_for_validate,
    struct mesh_primitive_asset_header_t const *mesh_primitive_asset_header_for_validate,
    void *staging_pointer,
    struct pt_gfx_mesh_neutral_primitive_memcpy_dest_t const *memcpy_dests,
    pt_input_stream_ref input_stream, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback)
{
    if (-1 == input_stream_seek_callback(input_stream, 0, PT_INPUT_STREAM_SEEK_SET))
    {
        return false;
    }

    uint32_t fourcc_signature;
    {
        intptr_t res_read = input_stream_read_callback(input_stream, &fourcc_signature, sizeof(uint32_t));
        if (res_read == -1 || sizeof(uint32_t) != static_cast<size_t>(res_read))
        {
            return false;
        }
    }

    switch (fourcc_signature)
    {
    case PMX_HEADER_SIGNATURE:
        return mesh_load_pmx_primitive_data_from_input_stream(
            mesh_asset_header_for_validate,
            mesh_primitive_asset_header_for_validate,
            staging_pointer,
            memcpy_dests,
            input_stream, input_stream_read_callback, input_stream_seek_callback,
            NULL, NULL);
    default:
        return false;
    }
}
