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

#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include "pt_gfx_mesh_base_load.h"
#include "pt_gfx_mesh_base_load_pmx.h"

enum
{
    FACADE_PMX_HEADER_SIGNATURE = 0X20584d50, //'P''M''X'' '
};

extern bool mesh_load_header_from_input_stream(struct pt_gfx_mesh_neutral_header_t *out_neutral_header, pt_gfx_input_stream_ref gfx_input_stream, struct pt_gfx_input_stream_callbacks_t const *gfx_input_stream_callbacks)
{
    if (-1 == gfx_input_stream_callbacks->pfn_seek(gfx_input_stream, 0, PT_GFX_INPUT_STREAM_SEEK_SET))
    {
        return false;
    }

    uint32_t fourcc_signature;
    {
        intptr_t res_read = gfx_input_stream_callbacks->pfn_read(gfx_input_stream, &fourcc_signature, sizeof(uint32_t));
        if (res_read == -1 || sizeof(uint32_t) != static_cast<size_t>(res_read))
        {
            return false;
        }
    }

    switch (fourcc_signature)
    {
    case FACADE_PMX_HEADER_SIGNATURE:
        return mesh_load_pmx_header_from_input_stream(out_neutral_header, gfx_input_stream, gfx_input_stream_callbacks);
    default:
        return false;
    }
}

extern bool mesh_load_primitive_headers_from_input_stream(struct pt_gfx_mesh_neutral_header_t const *neutral_header_for_validate, struct pt_gfx_mesh_neutral_primitive_header_t *out_neutral_primitive_headers, pt_gfx_input_stream_ref gfx_input_stream, struct pt_gfx_input_stream_callbacks_t const *gfx_input_stream_callbacks)
{
    if (-1 == gfx_input_stream_callbacks->pfn_seek(gfx_input_stream, 0, PT_GFX_INPUT_STREAM_SEEK_SET))
    {
        return false;
    }

    uint32_t fourcc_signature;
    {
        intptr_t res_read = gfx_input_stream_callbacks->pfn_read(gfx_input_stream, &fourcc_signature, sizeof(uint32_t));
        if (res_read == -1 || sizeof(uint32_t) != static_cast<size_t>(res_read))
        {
            return false;
        }
    }

    switch (fourcc_signature)
    {
    case FACADE_PMX_HEADER_SIGNATURE:
        return mesh_load_pmx_primitive_headers_from_input_stream(neutral_header_for_validate, out_neutral_primitive_headers, gfx_input_stream, gfx_input_stream_callbacks);
    default:
        return false;
    }
}

extern bool mesh_load_primitive_data_from_input_stream(struct pt_gfx_mesh_neutral_header_t const *neutral_header_for_validate, struct pt_gfx_mesh_neutral_primitive_header_t const *neutral_primitive_headers_for_validate, void *staging_pointer, struct pt_gfx_mesh_neutral_primitive_memcpy_dest_t const *memcpy_dests, pt_gfx_input_stream_ref gfx_input_stream, struct pt_gfx_input_stream_callbacks_t const *gfx_input_stream_callbacks)
{
    if (-1 == gfx_input_stream_callbacks->pfn_seek(gfx_input_stream, 0, PT_GFX_INPUT_STREAM_SEEK_SET))
    {
        return false;
    }

    uint32_t fourcc_signature;
    {
        intptr_t res_read = gfx_input_stream_callbacks->pfn_read(gfx_input_stream, &fourcc_signature, sizeof(uint32_t));
        if (res_read == -1 || sizeof(uint32_t) != static_cast<size_t>(res_read))
        {
            return false;
        }
    }

    switch (fourcc_signature)
    {
    case FACADE_PMX_HEADER_SIGNATURE:
        return mesh_load_pmx_primitive_data_from_input_stream(neutral_header_for_validate, neutral_primitive_headers_for_validate, staging_pointer, memcpy_dests, gfx_input_stream, gfx_input_stream_callbacks);
    default:
        return false;
    }
}
