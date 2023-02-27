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

#include "mesh_asset_load_pmx.h"
#include <pt_mcrt_vector.h>
#include <pt_mcrt_set.h>
#include <pt_mcrt_map.h>
#include <pt_mcrt_memcpy.h>
#include <pt_math_pack.h>
#include "mesh_tangent.h"
#include "mesh_vertex.sli"
#include <assert.h>

struct mesh_asset_pmx_header_t
{
    bool is_text_encoding_utf8;
    uint32_t additional_vec4_count;
    uint32_t bone_index_size;
    uint32_t vertex_index_size;
    uint32_t texture_index_size;
    uint32_t vertex_count;
    uint32_t surface_count;
    uint32_t texture_count;
    uint32_t material_count;
    int64_t vertex_section_offset;
    int64_t surface_section_offset;
    int64_t texture_section_offset;
    int64_t material_section_offset;
};

static bool inline internal_load_pmx_header_from_input_stream(
    struct mesh_asset_pmx_header_t *out_mesh_asset_pmx_header,
    pt_input_stream_ref input_stream, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback);

extern bool mesh_load_pmx_header_from_input_stream(
    struct mesh_asset_header_t *out_mesh_asset_header,
    pt_input_stream_ref input_stream, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback)
{
    struct mesh_asset_pmx_header_t mesh_asset_pmx_header;
    if (!internal_load_pmx_header_from_input_stream(&mesh_asset_pmx_header, input_stream, input_stream_read_callback, input_stream_seek_callback))
    {
        return false;
    }

    out_mesh_asset_header->is_skined = true;
    out_mesh_asset_header->primitive_count = mesh_asset_pmx_header.material_count;
    return true;
}

static bool inline internal_load_pmx_header_from_input_stream(
    struct mesh_asset_pmx_header_t *out_mesh_asset_pmx_header,
    pt_input_stream_ref input_stream, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback)
{
    if (-1 == input_stream_seek_callback(input_stream, 0, PT_INPUT_STREAM_SEEK_SET))
    {
        return false;
    }

    uint32_t header_signature;
    {
        intptr_t res_read = input_stream_read_callback(input_stream, &header_signature, sizeof(uint32_t));
        if (res_read == -1 || sizeof(uint32_t) != static_cast<size_t>(res_read))
        {
            return false;
        }
    }

    if (PMX_HEADER_SIGNATURE != header_signature)
    {
        return false;
    }

    float header_version;
    {
        intptr_t res_read = input_stream_read_callback(input_stream, &header_version, sizeof(float));
        if (res_read == -1 || sizeof(float) != static_cast<size_t>(res_read))
        {
            return false;
        }
    }

    assert(2.0f == header_version || 2.1f == header_version);

    int8_t header_globals_count;
    {
        intptr_t res_read = input_stream_read_callback(input_stream, &header_globals_count, sizeof(int8_t));
        if (res_read == -1 || sizeof(int8_t) != static_cast<size_t>(res_read))
        {
            return false;
        }
    }
    assert(8 == header_globals_count);

    int8_t header_globals[8];
    {
        intptr_t res_read = input_stream_read_callback(input_stream, header_globals, sizeof(int8_t) * 8U);
        if (res_read == -1 || (sizeof(int8_t) * 8U) != static_cast<size_t>(res_read))
        {
            return false;
        }
    }

    if (!(0 == header_globals[0] || 1 == header_globals[0]))
    {
        return false;
    }

    bool is_text_encoding_utf8 = header_globals[0];

    // model name local
    {
        int32_t text_byte_count;
        {
            intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count, sizeof(int32_t));
            if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count)
            {
                return false;
            }
        }

        if (0 != text_byte_count)
        {
            if (-1 == input_stream_seek_callback(input_stream, text_byte_count, PT_INPUT_STREAM_SEEK_CUR))
            {
                return false;
            }
        }
    }

    // model name universal
    {
        int32_t text_byte_count;
        {
            intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count, sizeof(int32_t));
            if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count)
            {
                return false;
            }
        }

        if (0 != text_byte_count)
        {
            if (-1 == input_stream_seek_callback(input_stream, text_byte_count, PT_INPUT_STREAM_SEEK_CUR))
            {
                return false;
            }
        }
    }

    // comments local
    {
        int32_t text_byte_count;
        {
            intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count, sizeof(int32_t));
            if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count)
            {
                return false;
            }
        }

        if (0 != text_byte_count)
        {
            if (-1 == input_stream_seek_callback(input_stream, text_byte_count, PT_INPUT_STREAM_SEEK_CUR))
            {
                return false;
            }
        }
    }

    // comments universal
    {
        int32_t text_byte_count;
        {
            intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count, sizeof(int32_t));
            if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count)
            {
                return false;
            }
        }

        if (0 != text_byte_count)
        {
            if (-1 == input_stream_seek_callback(input_stream, text_byte_count, PT_INPUT_STREAM_SEEK_CUR))
            {
                return false;
            }
        }
    }
    if (!(0 == header_globals[1] || 1 == header_globals[1] || 2 == header_globals[1] || 3U == header_globals[1] || 4 == header_globals[1]))
    {
        return false;
    }
    uint32_t additional_vec4_count = header_globals[1];

    if (!(1 == header_globals[5] || 2 == header_globals[5] || 4 == header_globals[5]))
    {
        return false;
    }
    static_assert(1 == sizeof(int8_t), "");
    static_assert(2 == sizeof(int16_t), "");
    static_assert(4 == sizeof(int32_t), "");
    uint32_t bone_index_size = header_globals[5];

    if (!(1 == header_globals[2] || 2 == header_globals[2] || 4 == header_globals[2]))
    {
        return false;
    }
    static_assert(1 == sizeof(uint8_t), "");
    static_assert(2 == sizeof(uint16_t), "");
    static_assert(4 == sizeof(int32_t), "");
    uint32_t vertex_index_size = header_globals[2];

    if (!(1 == header_globals[3] || 2 == header_globals[3] || 4 == header_globals[3]))
    {
        return false;
    }
    static_assert(1 == sizeof(int8_t), "");
    static_assert(2 == sizeof(int16_t), "");
    static_assert(4 == sizeof(int32_t), "");
    uint32_t texture_index_size = header_globals[3];

    // vertex section
    int64_t vertex_section_offset = input_stream_seek_callback(input_stream, 0, PT_INPUT_STREAM_SEEK_CUR);
    if (-1 == vertex_section_offset)
    {
        return false;
    }

    // [int] vertex count	| vertices
    uint32_t vertex_count;
    {
        int32_t vertex_count_signed;

        intptr_t res_read = input_stream_read_callback(input_stream, &vertex_count_signed, sizeof(int32_t));
        if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > vertex_count_signed)
        {
            return false;
        }

        vertex_count = vertex_count_signed;
    }

    for (uint32_t vertex_index = 0U; vertex_index < vertex_count; ++vertex_index)
    {
        // position
        // normal
        // uv
        // additional vec4
        if (-1 == input_stream_seek_callback(input_stream, sizeof(float) * 3U + sizeof(float) * 3U + sizeof(float) * 2U + sizeof(float) * 4U * additional_vec4_count, PT_INPUT_STREAM_SEEK_CUR))
        {
            return false;
        }

        int8_t weight_deform_type;
        {
            intptr_t res_read = input_stream_read_callback(input_stream, &weight_deform_type, sizeof(int8_t));
            if (res_read == -1 || sizeof(int8_t) != static_cast<size_t>(res_read))
            {
                return false;
            }
        }

        // weight deform
        size_t weight_deform_size;
        switch (weight_deform_type)
        {
        case 0:
        {
            // BDEF 1
            weight_deform_size = bone_index_size;
        }
        break;
        case 1:
        {
            // BDEF 2
            weight_deform_size = (bone_index_size * 2U + sizeof(float));
        }
        break;
        case 2:
        {
            // BDEF4
            weight_deform_size = (bone_index_size * 4U + sizeof(float) * 4U);
        }
        break;
        case 3:
        {
            // SDEF
            weight_deform_size = (bone_index_size * 2U + sizeof(float) + sizeof(float) * 3U + sizeof(float) * 3U + sizeof(float) * 3U);
        }
        break;
        case 4:
        {
            // QDEF
            weight_deform_size = (bone_index_size * 4U + sizeof(float) * 4U);
        }
        break;
        default:
            return false;
        }

        if (-1 == input_stream_seek_callback(input_stream, weight_deform_size, PT_INPUT_STREAM_SEEK_CUR))
        {
            return false;
        }

        // Edge scale
        if (-1 == input_stream_seek_callback(input_stream, sizeof(float), PT_INPUT_STREAM_SEEK_CUR))
        {
            return false;
        }
    }

    // surface section
    int64_t surface_section_offset = input_stream_seek_callback(input_stream, 0, PT_INPUT_STREAM_SEEK_CUR);
    if (-1 == surface_section_offset)
    {
        return false;
    }

    // [int] surface count | surfaces
    uint32_t surface_count;
    {
        int32_t surface_count_signed;

        intptr_t res_read = input_stream_read_callback(input_stream, &surface_count_signed, sizeof(int32_t));
        if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > surface_count_signed)
        {
            return false;
        }

        surface_count = surface_count_signed;
    }

    if (-1 == input_stream_seek_callback(input_stream, vertex_index_size * surface_count, PT_INPUT_STREAM_SEEK_CUR))
    {
        return false;
    }

    // texture section
    int64_t texture_section_offset = input_stream_seek_callback(input_stream, 0, PT_INPUT_STREAM_SEEK_CUR);
    // [int] texture count | textures
    int32_t texture_count;
    {
        intptr_t res_read = input_stream_read_callback(input_stream, &texture_count, sizeof(int32_t));
        if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > texture_count)
        {
            return false;
        }
    }

    for (int32_t texture_index = 0; texture_index < texture_count; ++texture_index)
    {
        // path
        int32_t text_byte_count;
        {
            intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count, sizeof(int32_t));
            if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count)
            {
                return false;
            }
        }

        if (0 != text_byte_count)
        {
            if (-1 == input_stream_seek_callback(input_stream, text_byte_count, PT_INPUT_STREAM_SEEK_CUR))
            {
                return false;
            }
        }
    }

    // material section
    int64_t material_section_offset = input_stream_seek_callback(input_stream, 0, PT_INPUT_STREAM_SEEK_CUR);
    if (-1 == material_section_offset)
    {
        return false;
    }

    // [int] material count	| materials
    int32_t material_count;
    {
        intptr_t res_read = input_stream_read_callback(input_stream, &material_count, sizeof(int32_t));
        if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > material_count)
        {
            return false;
        }
    }

    out_mesh_asset_pmx_header->is_text_encoding_utf8 = is_text_encoding_utf8;
    out_mesh_asset_pmx_header->additional_vec4_count = additional_vec4_count;
    out_mesh_asset_pmx_header->bone_index_size = bone_index_size;
    out_mesh_asset_pmx_header->vertex_index_size = vertex_index_size;
    out_mesh_asset_pmx_header->texture_index_size = texture_index_size;
    out_mesh_asset_pmx_header->vertex_count = vertex_count;
    out_mesh_asset_pmx_header->surface_count = surface_count;
    out_mesh_asset_pmx_header->texture_count = texture_count;
    out_mesh_asset_pmx_header->material_count = material_count;
    out_mesh_asset_pmx_header->vertex_section_offset = vertex_section_offset;
    out_mesh_asset_pmx_header->surface_section_offset = surface_section_offset;
    out_mesh_asset_pmx_header->texture_section_offset = texture_section_offset;
    out_mesh_asset_pmx_header->material_section_offset = material_section_offset;

    return true;
}

extern bool mesh_load_pmx_primitive_headers_from_input_stream(
    struct mesh_asset_header_t const *mesh_asset_header_for_validate,
    struct mesh_primitive_asset_header_t *out_mesh_primitive_asset_header,
    pt_input_stream_ref input_stream, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback)
{
    struct mesh_asset_pmx_header_t mesh_asset_pmx_header;
    if (!internal_load_pmx_header_from_input_stream(&mesh_asset_pmx_header, input_stream, input_stream_read_callback, input_stream_seek_callback))
    {
        return false;
    }

    // validate neutral header
    assert(mesh_asset_header_for_validate->is_skined == true);
    assert(mesh_asset_header_for_validate->primitive_count == mesh_asset_pmx_header.material_count);

    // surface section
    if (-1 == input_stream_seek_callback(input_stream, mesh_asset_pmx_header.surface_section_offset, PT_INPUT_STREAM_SEEK_SET))
    {
        return false;
    }

    uint32_t surface_count;
    {
        int32_t surface_count_signed;
        intptr_t res_read = input_stream_read_callback(input_stream, &surface_count_signed, sizeof(int32_t));
        if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > surface_count_signed)
        {
            return false;
        }

        surface_count = surface_count_signed;
    }

    // validate internal header
    assert(mesh_asset_pmx_header.surface_count == surface_count);

    mcrt_vector<uint32_t> mesh_vertex_indices(static_cast<size_t>(surface_count));
    {
        switch (mesh_asset_pmx_header.vertex_index_size)
        {
        case 1:
        {
            uint8_t *surface_data_uint8 = static_cast<uint8_t *>(mcrt_aligned_malloc(sizeof(uint8_t) * static_cast<size_t>(surface_count), alignof(uint8_t)));

            intptr_t res_read = input_stream_read_callback(input_stream, surface_data_uint8, sizeof(uint8_t) * static_cast<size_t>(surface_count));
            if (res_read == -1 || (sizeof(uint8_t) * static_cast<size_t>(surface_count)) != static_cast<size_t>(res_read))
            {
                mcrt_aligned_free(surface_data_uint8);
                return false;
            }

            for (uint32_t surface_index = 0U; surface_index < surface_count; ++surface_index)
            {
                mesh_vertex_indices[surface_index] = surface_data_uint8[surface_index];
            }

            mcrt_aligned_free(surface_data_uint8);
        }
        break;
        case 2:
        {
            uint16_t *surface_data_uint16 = static_cast<uint16_t *>(mcrt_aligned_malloc(sizeof(uint16_t) * static_cast<size_t>(surface_count), alignof(uint16_t)));

            intptr_t res_read = input_stream_read_callback(input_stream, surface_data_uint16, sizeof(uint16_t) * static_cast<size_t>(surface_count));
            if (res_read == -1 || (sizeof(uint16_t) * static_cast<size_t>(surface_count)) != static_cast<size_t>(res_read))
            {
                mcrt_aligned_free(surface_data_uint16);
                return false;
            }

            for (uint32_t surface_index = 0U; surface_index < surface_count; ++surface_index)
            {
                mesh_vertex_indices[surface_index] = surface_data_uint16[surface_index];
            }

            mcrt_aligned_free(surface_data_uint16);
        }
        break;
        case 4:
        {
            int32_t *surface_data_int32 = static_cast<int32_t *>(mcrt_aligned_malloc(sizeof(int32_t) * static_cast<size_t>(surface_count), alignof(int32_t)));

            intptr_t res_read = input_stream_read_callback(input_stream, surface_data_int32, sizeof(int32_t) * static_cast<size_t>(surface_count));
            if (res_read == -1 || (sizeof(int32_t) * static_cast<size_t>(surface_count)) != static_cast<size_t>(res_read))
            {
                mcrt_aligned_free(surface_data_int32);
                return false;
            }

            for (uint32_t surface_index = 0U; surface_index < surface_count; ++surface_index)
            {
                int32_t vertex_index_signed = surface_data_int32[surface_index];
                if (0 > vertex_index_signed)
                {
                    mcrt_aligned_free(surface_data_int32);
                    return false;
                }
                mesh_vertex_indices[surface_index] = vertex_index_signed;
            }

            mcrt_aligned_free(surface_data_int32);
        }
        break;
        default:
        {
            return false;
        }
        }
    }

    // material section
    if (-1 == input_stream_seek_callback(input_stream, mesh_asset_pmx_header.material_section_offset, PT_INPUT_STREAM_SEEK_SET))
    {
        return false;
    }

    uint32_t material_count;
    {
        int32_t material_count_signed;
        intptr_t res_read = input_stream_read_callback(input_stream, &material_count_signed, sizeof(int32_t));
        if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > material_count_signed)
        {
            return false;
        }

        material_count = material_count_signed;
    }

    // validate internal header
    assert(mesh_asset_pmx_header.material_count = material_count);

    // It is based on the offset of the previous material through to the size of the current material.
    // If you add up all the surface counts for all materials you should end up with the total number of surfaces.
    uint32_t material_total_surface_count = 0U;

    for (uint32_t material_index = 0U; material_index < material_count; ++material_index)
    {
        // material name local
        {
            int32_t text_byte_count;
            {
                intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count, sizeof(int32_t));
                if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count)
                {
                    return false;
                }
            }

            if (0 != text_byte_count)
            {
                if (-1 == input_stream_seek_callback(input_stream, text_byte_count, PT_INPUT_STREAM_SEEK_CUR))
                {
                    return false;
                }
            }
        }

        // material name universal
        {
            int32_t text_byte_count;
            {
                intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count, sizeof(int32_t));
                if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count)
                {
                    return false;
                }
            }

            if (0 != text_byte_count)
            {
                if (-1 == input_stream_seek_callback(input_stream, text_byte_count, PT_INPUT_STREAM_SEEK_CUR))
                {
                    return false;
                }
            }
        }

        if (-1 == input_stream_seek_callback(
                      input_stream,
                      // diffuse colour
                      sizeof(float) * 4U
                          // specular colour
                          + sizeof(float) * 3U
                          // specular strength
                          + sizeof(float)
                          // ambient colour
                          + sizeof(float) * 3U
                          // drawing flags
                          + sizeof(int8_t)
                          // edge colour
                          + sizeof(float) * 4U
                          // edge scale
                          + sizeof(float)
                          // texture index
                          + mesh_asset_pmx_header.texture_index_size
                          // environment index
                          + mesh_asset_pmx_header.texture_index_size
                          // environment blend mode
                          + sizeof(int8_t),
                      PT_INPUT_STREAM_SEEK_CUR))
        {
            return false;
        }

        int8_t toon_reference;
        {
            intptr_t res_read = input_stream_read_callback(input_stream, &toon_reference, sizeof(int8_t));
            if (res_read == -1 || sizeof(int8_t) != static_cast<size_t>(res_read))
            {
                return false;
            }
        }

        // toon value
        switch (toon_reference)
        {
        case 0:
        {
            // texture reference
            if (-1 == input_stream_seek_callback(input_stream, mesh_asset_pmx_header.texture_index_size, PT_INPUT_STREAM_SEEK_CUR))
            {
                return false;
            }
        }
        break;
        case 1:
        {
            // internal reference
            if (-1 == input_stream_seek_callback(input_stream, sizeof(int8_t), PT_INPUT_STREAM_SEEK_CUR))
            {
                return false;
            }
        }
        break;
        default:
        {
            return false;
        }
        }

        // meta data
        {
            int32_t text_byte_count;
            {
                intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count, sizeof(int32_t));
                if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count)
                {
                    return false;
                }
            }

            if (0 != text_byte_count)
            {
                if (-1 == input_stream_seek_callback(input_stream, text_byte_count, PT_INPUT_STREAM_SEEK_CUR))
                {
                    return false;
                }
            }
        }

        uint32_t material_surface_count;
        {
            int32_t material_surface_count_signed;
            intptr_t res_read = input_stream_read_callback(input_stream, &material_surface_count_signed, sizeof(int32_t));
            if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > material_surface_count_signed || 0 != (material_surface_count_signed % 3))
            {
                return false;
            }

            material_surface_count = material_surface_count_signed;
        }

        // IDVS
        // according to Mali GPU Best Practices
        // we should split the vertex buffer and create a tightly packed vertex buffer for each primitive to improving cache hitting

        mcrt_set<uint32_t> material_mesh_vertex_indices;
        for (uint32_t material_surface_index = 0; material_surface_index < material_surface_count; ++material_surface_index)
        {
            // https://gcc.gnu.org/onlinedocs/libstdc++/manual/associative.html#containers.associative.insert_hints

            uint32_t mesh_vertex_index = mesh_vertex_indices[material_total_surface_count + material_surface_index];

            material_mesh_vertex_indices.insert(mesh_vertex_index);
        }

        material_total_surface_count += material_surface_count;

        out_mesh_primitive_asset_header[material_index].vertex_count = static_cast<uint32_t>(material_mesh_vertex_indices.size());
        out_mesh_primitive_asset_header[material_index].index_count = material_surface_count;

        if (0XFFFFFFFFU > static_cast<uint32_t>(material_mesh_vertex_indices.size()))
        {
            out_mesh_primitive_asset_header[material_index].is_index_type_uint16 = true;
        }
        else
        {
            // validate internal header
            assert(mesh_asset_pmx_header.vertex_index_size != 1 && mesh_asset_pmx_header.vertex_index_size != 2);

            out_mesh_primitive_asset_header[material_index].is_index_type_uint16 = false;
        }
    }

    assert(surface_count == material_total_surface_count);

    return true;
}

extern bool mesh_load_pmx_primitive_data_from_input_stream(
    struct mesh_asset_header_t const *mesh_asset_header_for_validate,
    struct mesh_primitive_asset_header_t const *mesh_primitive_asset_header_for_validate,
    void *staging_pointer,
    struct pt_gfx_mesh_neutral_primitive_memcpy_dest_t const *memcpy_dests,
    pt_input_stream_ref input_stream, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback,
    char **out_material_texture_path, size_t *out_material_texture_size)
{
    struct mesh_asset_pmx_header_t mesh_asset_pmx_header;
    if (!internal_load_pmx_header_from_input_stream(&mesh_asset_pmx_header, input_stream, input_stream_read_callback, input_stream_seek_callback))
    {
        return false;
    }

    // validate neutral header
    assert(mesh_asset_header_for_validate->is_skined == true);
    assert(mesh_asset_header_for_validate->primitive_count == mesh_asset_pmx_header.material_count);

    // vertex section
    if (-1 == input_stream_seek_callback(input_stream, mesh_asset_pmx_header.vertex_section_offset, PT_INPUT_STREAM_SEEK_SET))
    {
        return false;
    }

    // [int] vertex count	| vertices
    uint32_t vertex_count;
    {
        int32_t vertex_count_signed;

        intptr_t res_read = input_stream_read_callback(input_stream, &vertex_count_signed, sizeof(int32_t));
        if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > vertex_count_signed)
        {
            return false;
        }

        vertex_count = vertex_count_signed;
    }

    // validate internal header
    assert(mesh_asset_pmx_header.vertex_count = vertex_count);

    mcrt_vector<pt_math_vec3> mesh_vertex_positions(static_cast<size_t>(vertex_count));
    mcrt_vector<pt_math_vec3> mesh_vertex_normals(static_cast<size_t>(vertex_count));
    mcrt_vector<pt_math_vec2> mesh_vertex_uvs(static_cast<size_t>(vertex_count));

    for (uint32_t vertex_index = 0U; vertex_index < vertex_count; ++vertex_index)
    {
        // position
        float vertex_position[3];
        intptr_t res_read_position = input_stream_read_callback(input_stream, &vertex_position, (sizeof(float) * 3U));
        if (res_read_position == -1 || (sizeof(float) * 3U) != static_cast<size_t>(res_read_position))
        {
            return false;
        }
        mesh_vertex_positions[vertex_index].x = vertex_position[0];
        mesh_vertex_positions[vertex_index].y = vertex_position[1];
        mesh_vertex_positions[vertex_index].z = vertex_position[2];

        // normal
        float vertex_normal[3];
        intptr_t res_read_normal = input_stream_read_callback(input_stream, &vertex_normal, (sizeof(float) * 3U));
        if (res_read_normal == -1 || (sizeof(float) * 3U) != static_cast<size_t>(res_read_normal))
        {
            return false;
        }
        mesh_vertex_normals[vertex_index].x = vertex_normal[0];
        mesh_vertex_normals[vertex_index].y = vertex_normal[1];
        mesh_vertex_normals[vertex_index].z = vertex_normal[2];

        // uv
        float vertex_uv[2];
        intptr_t res_read_uv = input_stream_read_callback(input_stream, &vertex_uv, (sizeof(float) * 2U));
        if (res_read_uv == -1 || (sizeof(float) * 2U) != static_cast<size_t>(res_read_uv))
        {
            return false;
        }
        mesh_vertex_uvs[vertex_index].x = vertex_uv[0];
        mesh_vertex_uvs[vertex_index].y = vertex_uv[1];

        // additional vec4
        if (-1 == input_stream_seek_callback(input_stream, sizeof(float) * 4U * mesh_asset_pmx_header.additional_vec4_count, PT_INPUT_STREAM_SEEK_CUR))
        {
            return false;
        }

        int8_t weight_deform_type;
        {
            intptr_t res_read = input_stream_read_callback(input_stream, &weight_deform_type, sizeof(int8_t));
            if (res_read == -1 || sizeof(int8_t) != static_cast<size_t>(res_read))
            {
                return false;
            }
        }

        // weight deform
        size_t weight_deform_size;
        switch (weight_deform_type)
        {
        case 0:
        {
            // BDEF 1
            weight_deform_size = mesh_asset_pmx_header.bone_index_size;
        }
        break;
        case 1:
        {
            // BDEF 2
            weight_deform_size = (mesh_asset_pmx_header.bone_index_size * 2U + sizeof(float));
        }
        break;
        case 2:
        {
            // BDEF4
            weight_deform_size = (mesh_asset_pmx_header.bone_index_size * 4U + sizeof(float) * 4U);
        }
        break;
        case 3:
        {
            // SDEF
            weight_deform_size = (mesh_asset_pmx_header.bone_index_size * 2U + sizeof(float) + sizeof(float) * 3U + sizeof(float) * 3U + sizeof(float) * 3U);
        }
        break;
        case 4:
        {
            // QDEF
            weight_deform_size = (mesh_asset_pmx_header.bone_index_size * 4U + sizeof(float) * 4U);
        }
        break;
        default:
            return false;
        }

        if (-1 == input_stream_seek_callback(input_stream, weight_deform_size, PT_INPUT_STREAM_SEEK_CUR))
        {
            return false;
        }

        // Edge scale
        if (-1 == input_stream_seek_callback(input_stream, sizeof(float), PT_INPUT_STREAM_SEEK_CUR))
        {
            return false;
        }
    }

    // surface section
    int64_t surface_section_offset = input_stream_seek_callback(input_stream, 0, PT_INPUT_STREAM_SEEK_CUR);
    if (-1 == surface_section_offset)
    {
        return false;
    }

    // [int] surface count | surfaces
    uint32_t surface_count;
    {
        int32_t surface_count_signed;

        intptr_t res_read = input_stream_read_callback(input_stream, &surface_count_signed, sizeof(int32_t));
        if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > surface_count_signed)
        {
            return false;
        }

        surface_count = surface_count_signed;
    }

    // validate internal header
    assert(mesh_asset_pmx_header.surface_count == surface_count);

    mcrt_vector<uint32_t> mesh_vertex_indices(static_cast<size_t>(surface_count));
    {
        switch (mesh_asset_pmx_header.vertex_index_size)
        {
        case 1:
        {
            uint8_t *surface_data_uint8 = static_cast<uint8_t *>(mcrt_aligned_malloc(sizeof(uint8_t) * static_cast<size_t>(surface_count), alignof(uint8_t)));

            intptr_t res_read = input_stream_read_callback(input_stream, surface_data_uint8, sizeof(uint8_t) * static_cast<size_t>(surface_count));
            if (res_read == -1 || (sizeof(uint8_t) * static_cast<size_t>(surface_count)) != static_cast<size_t>(res_read))
            {
                mcrt_aligned_free(surface_data_uint8);
                return false;
            }

            for (uint32_t surface_index = 0U; surface_index < surface_count; ++surface_index)
            {
                mesh_vertex_indices[surface_index] = surface_data_uint8[surface_index];
            }

            mcrt_aligned_free(surface_data_uint8);
        }
        break;
        case 2:
        {
            uint16_t *surface_data_uint16 = static_cast<uint16_t *>(mcrt_aligned_malloc(sizeof(uint16_t) * static_cast<size_t>(surface_count), alignof(uint16_t)));

            intptr_t res_read = input_stream_read_callback(input_stream, surface_data_uint16, sizeof(uint16_t) * static_cast<size_t>(surface_count));
            if (res_read == -1 || (sizeof(uint16_t) * static_cast<size_t>(surface_count)) != static_cast<size_t>(res_read))
            {
                mcrt_aligned_free(surface_data_uint16);
                return false;
            }

            for (uint32_t surface_index = 0U; surface_index < surface_count; ++surface_index)
            {
                mesh_vertex_indices[surface_index] = surface_data_uint16[surface_index];
            }

            mcrt_aligned_free(surface_data_uint16);
        }
        break;
        case 4:
        {
            int32_t *surface_data_int32 = static_cast<int32_t *>(mcrt_aligned_malloc(sizeof(int32_t) * static_cast<size_t>(surface_count), alignof(int32_t)));

            intptr_t res_read = input_stream_read_callback(input_stream, surface_data_int32, sizeof(int32_t) * static_cast<size_t>(surface_count));
            if (res_read == -1 || (sizeof(int32_t) * static_cast<size_t>(surface_count)) != static_cast<size_t>(res_read))
            {
                mcrt_aligned_free(surface_data_int32);
                return false;
            }

            for (uint32_t surface_index = 0U; surface_index < surface_count; ++surface_index)
            {
                int32_t vertex_index_signed = surface_data_int32[surface_index];
                if (0 > vertex_index_signed)
                {
                    mcrt_aligned_free(surface_data_int32);
                    return false;
                }
                mesh_vertex_indices[surface_index] = vertex_index_signed;
            }

            mcrt_aligned_free(surface_data_int32);
        }
        break;
        default:
        {
            return false;
        }
        }
    }

    // texture section
    if (-1 == input_stream_seek_callback(input_stream, mesh_asset_pmx_header.texture_section_offset, PT_INPUT_STREAM_SEEK_SET))
    {
        return false;
    }

    int32_t texture_count;
    {
        intptr_t res_read = input_stream_read_callback(input_stream, &texture_count, sizeof(int32_t));
        if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > texture_count)
        {
            return false;
        }
    }

    // validate internal header
    assert(mesh_asset_pmx_header.texture_count == texture_count);

    for (int32_t texture_index = 0; texture_index < texture_count; ++texture_index)
    {
        // path
        int32_t text_byte_count;
        {
            intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count, sizeof(int32_t));
            if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count)
            {
                return false;
            }
        }

        if (0 != text_byte_count)
        {
            if (-1 == input_stream_seek_callback(input_stream, text_byte_count, PT_INPUT_STREAM_SEEK_CUR))
            {
                return false;
            }
        }
    }

    // material section
    if (-1 == input_stream_seek_callback(input_stream, mesh_asset_pmx_header.material_section_offset, PT_INPUT_STREAM_SEEK_SET))
    {
        return false;
    }

    uint32_t material_count;
    {
        int32_t material_count_signed;
        intptr_t res_read = input_stream_read_callback(input_stream, &material_count_signed, sizeof(int32_t));
        if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > material_count_signed)
        {
            return false;
        }

        material_count = material_count_signed;
    }

    // validate internal header
    assert(mesh_asset_pmx_header.material_count = material_count);

    // It is based on the offset of the previous material through to the size of the current material.
    // If you add up all the surface counts for all materials you should end up with the total number of surfaces.
    uint32_t material_total_surface_count = 0U;

    for (uint32_t material_index = 0U; material_index < material_count; ++material_index)
    {
        // material name local
        {
            int32_t text_byte_count;
            {
                intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count, sizeof(int32_t));
                if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count)
                {
                    return false;
                }
            }

            if (0 != text_byte_count)
            {
                if (-1 == input_stream_seek_callback(input_stream, text_byte_count, PT_INPUT_STREAM_SEEK_CUR))
                {
                    return false;
                }
            }
        }

        // material name universal
        {
            int32_t text_byte_count;
            {
                intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count, sizeof(int32_t));
                if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count)
                {
                    return false;
                }
            }

            if (0 != text_byte_count)
            {
                if (-1 == input_stream_seek_callback(input_stream, text_byte_count, PT_INPUT_STREAM_SEEK_CUR))
                {
                    return false;
                }
            }
        }

        if (-1 == input_stream_seek_callback(
                      input_stream,
                      // diffuse colour
                      sizeof(float) * 4U
                          // specular colour
                          + sizeof(float) * 3U
                          // specular strength
                          + sizeof(float)
                          // ambient colour
                          + sizeof(float) * 3U
                          // drawing flags
                          + sizeof(int8_t)
                          // edge colour
                          + sizeof(float) * 4U
                          // edge scale
                          + sizeof(float)
                          // texture index
                          + mesh_asset_pmx_header.texture_index_size
                          // environment index
                          + mesh_asset_pmx_header.texture_index_size
                          // environment blend mode
                          + sizeof(int8_t),
                      PT_INPUT_STREAM_SEEK_CUR))
        {
            return false;
        }

        int8_t toon_reference;
        {
            intptr_t res_read = input_stream_read_callback(input_stream, &toon_reference, sizeof(int8_t));
            if (res_read == -1 || sizeof(int8_t) != static_cast<size_t>(res_read))
            {
                return false;
            }
        }

        // toon value
        switch (toon_reference)
        {
        case 0:
        {
            // texture reference
            if (-1 == input_stream_seek_callback(input_stream, mesh_asset_pmx_header.texture_index_size, PT_INPUT_STREAM_SEEK_CUR))
            {
                return false;
            }
        }
        break;
        case 1:
        {
            // internal reference
            if (-1 == input_stream_seek_callback(input_stream, sizeof(int8_t), PT_INPUT_STREAM_SEEK_CUR))
            {
                return false;
            }
        }
        break;
        default:
        {
            return false;
        }
        }

        // meta data
        {
            int32_t text_byte_count;
            {
                intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count, sizeof(int32_t));
                if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count)
                {
                    return false;
                }
            }

            if (0 != text_byte_count)
            {
                if (-1 == input_stream_seek_callback(input_stream, text_byte_count, PT_INPUT_STREAM_SEEK_CUR))
                {
                    return false;
                }
            }
        }

        uint32_t material_surface_count;
        {
            int32_t material_surface_count_signed;
            intptr_t res_read = input_stream_read_callback(input_stream, &material_surface_count_signed, sizeof(int32_t));
            if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > material_surface_count_signed || 0 != (material_surface_count_signed % 3))
            {
                return false;
            }

            material_surface_count = material_surface_count_signed;
        }

        // IDVS
        // according to Mali GPU Best Practices
        // we should split the vertex buffer and create a tightly packed vertex buffer for each primitive to improving cache hitting
        uint32_t material_index_count;
        mcrt_vector<uint32_t> material_indices;
        uint32_t material_vertex_count;
        mcrt_vector<pt_math_vec3> material_vertex_positions;
        mcrt_vector<pt_math_vec3> material_vertex_normals;
        mcrt_vector<pt_math_vec2> material_vertex_uvs;
        {
            material_index_count = material_surface_count;
            material_indices.resize(material_index_count);

            mcrt_map<uint32_t, uint32_t> mesh_vertex_indices_to_material_vertex_indices;
            for (uint32_t material_surface_index = 0; material_surface_index < material_surface_count; ++material_surface_index)
            {
                uint32_t mesh_vertex_index = mesh_vertex_indices[material_total_surface_count + material_surface_index];

                // https://gcc.gnu.org/onlinedocs/libstdc++/manual/associative.html#containers.associative.insert_hints
                // use "lower_bound" rather than "find"
                mcrt_map<uint32_t, uint32_t>::iterator iter_lower_bound = mesh_vertex_indices_to_material_vertex_indices.lower_bound(mesh_vertex_index);
                if (mesh_vertex_indices_to_material_vertex_indices.end() != iter_lower_bound && mesh_vertex_index == iter_lower_bound->first)
                {
                    uint32_t material_vertex_index = iter_lower_bound->second;
                    material_indices[material_surface_index] = material_vertex_index;
                }
                else
                {
                    uint32_t material_vertex_index = static_cast<uint32_t>(mesh_vertex_indices_to_material_vertex_indices.size());

                    std::pair<int32_t, uint32_t> val(mesh_vertex_index, material_vertex_index);
                    mesh_vertex_indices_to_material_vertex_indices.insert(iter_lower_bound, val);

                    material_indices[material_surface_index] = material_vertex_index;
                }
            }

            material_vertex_count = static_cast<uint32_t>(mesh_vertex_indices_to_material_vertex_indices.size());
            material_vertex_positions.resize(material_vertex_count);
            material_vertex_normals.resize(material_vertex_count);
            material_vertex_uvs.resize(material_vertex_count);

            for (std::pair<int32_t, uint32_t> const &pair : mesh_vertex_indices_to_material_vertex_indices)
            {
                uint32_t const mesh_vertex_index = pair.first;
                uint32_t const material_vertex_index = pair.second;

                material_vertex_positions[material_vertex_index].x = mesh_vertex_positions[mesh_vertex_index].x;
                material_vertex_positions[material_vertex_index].y = mesh_vertex_positions[mesh_vertex_index].y;
                material_vertex_positions[material_vertex_index].z = mesh_vertex_positions[mesh_vertex_index].z;

                material_vertex_normals[material_vertex_index].x = mesh_vertex_normals[mesh_vertex_index].x;
                material_vertex_normals[material_vertex_index].y = mesh_vertex_normals[mesh_vertex_index].y;
                material_vertex_normals[material_vertex_index].z = mesh_vertex_normals[mesh_vertex_index].z;

                material_vertex_uvs[material_vertex_index].x = mesh_vertex_uvs[mesh_vertex_index].x;
                material_vertex_uvs[material_vertex_index].y = mesh_vertex_uvs[mesh_vertex_index].y;
            }
        }

        // validate primitive headers
        assert(material_vertex_count == mesh_primitive_asset_header_for_validate[material_index].vertex_count);
        assert(material_index_count == mesh_primitive_asset_header_for_validate[material_index].index_count);

        // calculate tangents
        mcrt_vector<pt_math_vec4> material_vertex_qtangents(material_vertex_count);
        mcrt_vector<float> material_vertex_reflections(material_vertex_count);
        assert(0U == (material_index_count % 3U));
        mesh_calculate_tangents(material_index_count / 3U, &material_indices[0], material_vertex_count, &material_vertex_positions[0], &material_vertex_normals[0], &material_vertex_uvs[0], &material_vertex_qtangents[0], &material_vertex_reflections[0]);

        // write to staging buffer
        for (uint32_t material_vertex_index = 0U; material_vertex_index < material_vertex_count; ++material_vertex_index)
        {
            decltype(mesh_vertex_position::position_x) *out_position_x = reinterpret_cast<decltype(mesh_vertex_position::position_x) *>(reinterpret_cast<uintptr_t>(staging_pointer) + memcpy_dests[material_index].position_staging_offset + sizeof(mesh_vertex_position) * material_vertex_index + offsetof(mesh_vertex_position, position_x));
            (*out_position_x) = pt_math_float_to_r16_float(material_vertex_positions[material_vertex_index].x);

            decltype(mesh_vertex_position::position_y)* out_position_y = reinterpret_cast<decltype(mesh_vertex_position::position_y)*>(reinterpret_cast<uintptr_t>(staging_pointer) + memcpy_dests[material_index].position_staging_offset + sizeof(mesh_vertex_position) * material_vertex_index + offsetof(mesh_vertex_position, position_y));
            (*out_position_y) = pt_math_float_to_r16_float(material_vertex_positions[material_vertex_index].y);

            decltype(mesh_vertex_position::position_z) *out_position_z = reinterpret_cast<decltype(mesh_vertex_position::position_z) *>(reinterpret_cast<uintptr_t>(staging_pointer) + memcpy_dests[material_index].position_staging_offset + sizeof(mesh_vertex_position) * material_vertex_index + offsetof(mesh_vertex_position, position_z));
            (*out_position_z) = pt_math_float_to_r16_float(material_vertex_positions[material_vertex_index].z);

            decltype(mesh_vertex_varying::qtangentxyz_xyz_reflection_w) *out_qtangentxyz_xyz_reflection_w = reinterpret_cast<decltype(mesh_vertex_varying::qtangentxyz_xyz_reflection_w) *>(reinterpret_cast<uintptr_t>(staging_pointer) + memcpy_dests[material_index].varying_staging_offset + sizeof(mesh_vertex_varying) * material_vertex_index + offsetof(mesh_vertex_varying, qtangentxyz_xyz_reflection_w));
            float unpacked_qtangentxyz_xyz_reflection_w[4] = {
                0.5F * (material_vertex_qtangents[material_vertex_index].x + 1.0F),
                0.5F * (material_vertex_qtangents[material_vertex_index].y + 1.0F),
                0.5F * (material_vertex_qtangents[material_vertex_index].z + 1.0F),
                0.5F * (material_vertex_reflections[material_vertex_index] + 1.0F)};
            (*out_qtangentxyz_xyz_reflection_w) = pt_math_float4_to_r10g10b10a2_unorm(unpacked_qtangentxyz_xyz_reflection_w);

            decltype(mesh_vertex_varying::qtangentw_x_uv_yz) *out_qtangentw_x_uv_yz = reinterpret_cast<decltype(mesh_vertex_varying::qtangentw_x_uv_yz) *>(reinterpret_cast<uintptr_t>(staging_pointer) + memcpy_dests[material_index].varying_staging_offset + sizeof(mesh_vertex_varying) * material_vertex_index + offsetof(mesh_vertex_varying, qtangentw_x_uv_yz));
            float unpacked_qtangentw_x_uv_yz[4] = {
                0.5F * (material_vertex_qtangents[material_vertex_index].w + 1.0F),
                material_vertex_uvs[material_vertex_index].x,
                material_vertex_uvs[material_vertex_index].y};
            (*out_qtangentw_x_uv_yz) = pt_math_float4_to_r10g10b10a2_unorm(unpacked_qtangentw_x_uv_yz);
        }

        if (material_vertex_count < 0XFFFFFFFFU)
        {
            // validate primitive headers
            assert(mesh_primitive_asset_header_for_validate[material_index].is_index_type_uint16);

            for (uint32_t material_index_index = 0; material_index_index < material_index_count; ++material_index_index)
            {
                uint32_t material_vertex_index = material_indices[material_index_index];

                uint16_t *out_index = reinterpret_cast<uint16_t *>(reinterpret_cast<uintptr_t>(staging_pointer) + memcpy_dests[material_index].index_staging_offset + sizeof(uint16_t) * material_index_index);
                (*out_index) = material_vertex_index;
            }
        }
        else
        {
            // validate internal header
            assert(mesh_asset_pmx_header.vertex_index_size != 1 && mesh_asset_pmx_header.vertex_index_size != 2);

            // validate primitive headers
            assert(!mesh_primitive_asset_header_for_validate[material_index].is_index_type_uint16);

            for (uint32_t material_index_index = 0; material_index_index < material_index_count; ++material_index_index)
            {
                uint32_t material_vertex_index = material_indices[material_index_index];

                uint32_t *out_index = reinterpret_cast<uint32_t *>(reinterpret_cast<uintptr_t>(staging_pointer) + memcpy_dests[material_index].index_staging_offset + sizeof(uint32_t) * material_index_index);
                (*out_index) = material_vertex_index;
            }
        }

        // It is based on the offset of the previous material through to the size of the current material.
        material_total_surface_count += material_surface_count;
    }

    // If you add up all the surface counts for all materials you should end up with the total number of surfaces.
    assert(surface_count == material_total_surface_count);

    return true;
}
