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

#include "material_asset_load_pmx.h"
#include <assert.h>

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

static bool inline internal_load_pmx_header_from_input_stream(
    struct material_asset_pmx_header *out_mateiral_asset_pmx_header,
    pt_input_stream_ref input_stream, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback);

static bool inline internal_load_pmx_header_from_input_stream(
    struct material_asset_pmx_header *out_mateiral_asset_pmx_header,
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

    assert(2.0F == header_version || 2.1F == header_version);

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

    if (is_text_encoding_utf8)
    {
        // model name local
        {
            int32_t text_byte_count_utf8;
            {
                intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count_utf8, sizeof(int32_t));
                if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count_utf8)
                {
                    return false;
                }
            }
            if (0 != text_byte_count_utf8)
            {
                if (-1 == input_stream_seek_callback(input_stream, text_byte_count_utf8, PT_INPUT_STREAM_SEEK_CUR))
                {
                    return false;
                }
            }
        }

        // model name universal
        {
            int32_t text_byte_count_utf8;
            {
                intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count_utf8, sizeof(int32_t));
                if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count_utf8)
                {
                    return false;
                }
            }
            if (0 != text_byte_count_utf8)
            {
                if (-1 == input_stream_seek_callback(input_stream, text_byte_count_utf8, PT_INPUT_STREAM_SEEK_CUR))
                {
                    return false;
                }
            }
        }

        // comments local
        {
            int32_t text_byte_count_utf8;
            {
                intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count_utf8, sizeof(int32_t));
                if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count_utf8)
                {
                    return false;
                }
            }
            if (0 != text_byte_count_utf8)
            {
                if (-1 == input_stream_seek_callback(input_stream, text_byte_count_utf8, PT_INPUT_STREAM_SEEK_CUR))
                {
                    return false;
                }
            }
        }

        // comments universal
        {
            int32_t text_byte_count_utf8;
            {
                intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count_utf8, sizeof(int32_t));
                if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count_utf8)
                {
                    return false;
                }
            }
            if (0 != text_byte_count_utf8)
            {
                if (-1 == input_stream_seek_callback(input_stream, text_byte_count_utf8, PT_INPUT_STREAM_SEEK_CUR))
                {
                    return false;
                }
            }
        }
    }
    else
    {
        // model name local
        {
            int32_t text_byte_count_utf16;
            {
                intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count_utf16, sizeof(int32_t));
                if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count_utf16)
                {
                    return false;
                }
            }

            if (0 != text_byte_count_utf16)
            {
                if (0 != (text_byte_count_utf16 % sizeof(uint16_t)))
                {
                    return false;
                }

                if (-1 == input_stream_seek_callback(input_stream, text_byte_count_utf16, PT_INPUT_STREAM_SEEK_CUR))
                {
                    return false;
                }
            }
        }

        // model name universal
        {
            int32_t text_byte_count_utf16;
            {
                intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count_utf16, sizeof(int32_t));
                if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count_utf16)
                {
                    return false;
                }
            }

            if (0 != text_byte_count_utf16)
            {
                if (0 != (text_byte_count_utf16 % sizeof(uint16_t)))
                {
                    return false;
                }

                if (-1 == input_stream_seek_callback(input_stream, text_byte_count_utf16, PT_INPUT_STREAM_SEEK_CUR))
                {
                    return false;
                }
            }
        }

        // comments local
        {
            int32_t text_byte_count_utf16;
            {
                intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count_utf16, sizeof(int32_t));
                if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count_utf16)
                {
                    return false;
                }
            }

            if (0 != text_byte_count_utf16)
            {
                if (0 != (text_byte_count_utf16 % sizeof(uint16_t)))
                {
                    return false;
                }

                if (-1 == input_stream_seek_callback(input_stream, text_byte_count_utf16, PT_INPUT_STREAM_SEEK_CUR))
                {
                    return false;
                }
            }
        }

        // comments universal
        {
            int32_t text_byte_count_utf16;
            {
                intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count_utf16, sizeof(int32_t));
                if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count_utf16)
                {
                    return false;
                }
            }

            if (0 != text_byte_count_utf16)
            {
                if (0 != (text_byte_count_utf16 % sizeof(uint16_t)))
                {
                    return false;
                }

                if (-1 == input_stream_seek_callback(input_stream, text_byte_count_utf16, PT_INPUT_STREAM_SEEK_CUR))
                {
                    return false;
                }
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
        if (is_text_encoding_utf8)
        {
            int32_t text_byte_count;
            intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count, sizeof(int32_t));
            if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count)
            {
                return false;
            }

            if (0 != text_byte_count)
            {
                if (-1 == input_stream_seek_callback(input_stream, text_byte_count, PT_INPUT_STREAM_SEEK_CUR))
                {
                    return false;
                }
            }
        }
        else
        {
            int32_t text_byte_count;
            intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count, sizeof(int32_t));
            if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count || (0 != (text_byte_count % sizeof(uint16_t))))
            {
                return false;
            }

            if (0 != text_byte_count)
            {
                if (-1 == input_stream_seek_callback(input_stream, text_byte_count, PT_INPUT_STREAM_SEEK_CUR))
                {
                    return false;
                }
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

    out_internal_header->is_text_encoding_utf8 = is_text_encoding_utf8;
    out_internal_header->additional_vec4_count = additional_vec4_count;
    out_internal_header->bone_index_size = bone_index_size;
    out_internal_header->vertex_index_size = vertex_index_size;
    out_internal_header->texture_index_size = texture_index_size;
    out_internal_header->vertex_count = vertex_count;
    out_internal_header->surface_count = surface_count;
    out_internal_header->texture_count = texture_count;
    out_internal_header->material_count = material_count;
    out_internal_header->vertex_section_offset = vertex_section_offset;
    out_internal_header->surface_section_offset = surface_section_offset;
    out_internal_header->texture_section_offset = texture_section_offset;
    out_internal_header->material_section_offset = material_section_offset;

    return true;
}

extern bool mesh_load_pmx_primitive_data_from_input_stream(struct mesh_asset_header_t const *mesh_asset_header_for_validate, struct mesh_primitive_asset_header_t const *mesh_primitive_asset_header_for_validate, void *staging_pointer, struct pt_gfx_mesh_neutral_primitive_memcpy_dest_t const *memcpy_dests, pt_input_stream_ref input_stream, struct pt_gfx_input_stream_callbacks_t const *gfx_input_stream_callbacks, char **out_material_texture_path, size_t *out_material_texture_size)
{
    struct mesh_internal_pmx_header_t internal_header;
    if (!internal_load_pmx_header_from_input_stream(&internal_header, input_stream, gfx_input_stream_callbacks))
    {
        return false;
    }

    // validate neutral header
    assert(mesh_asset_header_for_validate->is_skined == true);
    assert(mesh_asset_header_for_validate->primitive_count == internal_header.material_count);

    // vertex section
    if (-1 == input_stream_seek_callback(input_stream, internal_header.vertex_section_offset, PT_INPUT_STREAM_SEEK_SET))
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
    assert(internal_header.vertex_count = vertex_count);

    mcrt_vector<pt_math_vec3> vertices_positions(static_cast<size_t>(vertex_count));
    mcrt_vector<pt_math_vec3> vertices_normals(static_cast<size_t>(vertex_count));
    mcrt_vector<pt_math_vec2> vertices_uvs(static_cast<size_t>(vertex_count));

    for (uint32_t vertex_index = 0U; vertex_index < vertex_count; ++vertex_index)
    {
        // position
        pt_math_vec3 vertex_position;
        intptr_t res_read_position = input_stream_read_callback(input_stream, &vertex_position, (sizeof(float) * 3U));
        if (res_read_position == -1 || (sizeof(float) * 3U) != static_cast<size_t>(res_read_position))
        {
            return false;
        }
        vertices_positions[vertex_index] = vertex_position;

        // normal
        pt_math_vec3 vertex_normal;
        intptr_t res_read_normal = input_stream_read_callback(input_stream, &vertex_normal, (sizeof(float) * 3U));
        if (res_read_normal == -1 || (sizeof(float) * 3U) != static_cast<size_t>(res_read_normal))
        {
            return false;
        }
        vertices_normals[vertex_index] = vertex_normal;

        // uv
        pt_math_vec2 vertex_uv;
        intptr_t res_read_uv = input_stream_read_callback(input_stream, &vertex_uv, (sizeof(float) * 2U));
        if (res_read_uv == -1 || (sizeof(float) * 2U) != static_cast<size_t>(res_read_uv))
        {
            return false;
        }
        vertices_uvs[vertex_index] = vertex_uv;

        // additional vec4
        if (-1 == input_stream_seek_callback(input_stream, sizeof(float) * 4U * internal_header.additional_vec4_count, PT_INPUT_STREAM_SEEK_CUR))
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
            weight_deform_size = internal_header.bone_index_size;
        }
        break;
        case 1:
        {
            // BDEF 2
            weight_deform_size = (internal_header.bone_index_size * 2U + sizeof(float));
        }
        break;
        case 2:
        {
            // BDEF4
            weight_deform_size = (internal_header.bone_index_size * 4U + sizeof(float) * 4U);
        }
        break;
        case 3:
        {
            // SDEF
            weight_deform_size = (internal_header.bone_index_size * 2U + sizeof(float) + sizeof(float) * 3U + sizeof(float) * 3U + sizeof(float) * 3U);
        }
        break;
        case 4:
        {
            // QDEF
            weight_deform_size = (internal_header.bone_index_size * 4U + sizeof(float) * 4U);
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
    assert(internal_header.surface_count == surface_count);

    mcrt_vector<uint32_t> mesh_vertex_indices(static_cast<size_t>(surface_count));
    {
        switch (internal_header.vertex_index_size)
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
    if (-1 == input_stream_seek_callback(input_stream, internal_header.texture_section_offset, PT_INPUT_STREAM_SEEK_SET))
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
    assert(internal_header.texture_count == texture_count);

    mcrt_vector<mcrt_vector<uint8_t>> mesh_texture_paths(static_cast<size_t>(texture_count));
    {
        for (int32_t texture_index = 0; texture_index < texture_count; ++texture_index)
        {
            // path
            if (internal_header.is_text_encoding_utf8)
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
                    mesh_texture_paths[texture_index].resize(text_byte_count);

                    intptr_t res_read = input_stream_read_callback(input_stream, &mesh_texture_paths[texture_index][0], text_byte_count);
                    if (res_read == -1 || text_byte_count != static_cast<size_t>(res_read))
                    {
                        return false;
                    }
                }
            }
            else
            {
                int32_t text_byte_count;
                {
                    intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count, sizeof(int32_t));
                    if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count || (0 != (text_byte_count % sizeof(uint16_t))))
                    {
                        return false;
                    }
                }

                if (0 != text_byte_count)
                {
                    uint16_t *text_utf16 = static_cast<uint16_t *>(mcrt_aligned_malloc(text_byte_count, alignof(uint16_t)));
                    if (-1 == input_stream_read_callback(input_stream, &text_utf16[0], text_byte_count))
                    {
                        mcrt_aligned_free(text_utf16);
                        return false;
                    }

                    uint8_t *text_utf8 = static_cast<uint8_t *>(mcrt_aligned_malloc(4U * (text_byte_count / sizeof(uint16_t)), alignof(uint8_t)));

                    uint32_t text_utf16_char_count_left = text_byte_count / sizeof(uint16_t);
                    uint32_t text_utf8_char_count_left = 4U * (text_byte_count / sizeof(uint16_t));
                    bool success_utf16_to_utf8 = internal_utf16_to_utf8(&text_utf16[0], &text_utf16_char_count_left, &text_utf8[0], &text_utf8_char_count_left);
                    assert(success_utf16_to_utf8);
                    mcrt_aligned_free(text_utf16);

                    mesh_texture_paths[texture_index].resize((4U * (text_byte_count / sizeof(uint16_t)) - text_utf8_char_count_left) + 1U);
                    mcrt_memcpy(&mesh_texture_paths[texture_index][0], &text_utf8[0], 4U * (text_byte_count / sizeof(uint16_t)) - text_utf8_char_count_left);
                    mcrt_aligned_free(text_utf8);
                    mesh_texture_paths[texture_index][4U * (text_byte_count / sizeof(uint16_t)) - text_utf8_char_count_left] = '\0';
                }
            }
        }
    }

    // material section
    if (-1 == input_stream_seek_callback(input_stream, internal_header.material_section_offset, PT_INPUT_STREAM_SEEK_SET))
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
    assert(internal_header.material_count = material_count);

    // It is based on the offset of the previous material through to the size of the current material.
    // If you add up all the surface counts for all materials you should end up with the total number of surfaces.
    uint32_t material_total_surface_count = 0U;

    for (uint32_t material_index = 0U; material_index < material_count; ++material_index)
    {
        // material name local
        if (internal_header.is_text_encoding_utf8)
        {
            int32_t text_byte_count;
            intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count, sizeof(int32_t));
            if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count)
            {
                return false;
            }

            if (0 != text_byte_count)
            {
                if (-1 == input_stream_seek_callback(input_stream, text_byte_count, PT_INPUT_STREAM_SEEK_CUR))
                {
                    return false;
                }
            }
        }
        else
        {
            int32_t text_byte_count;
            intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count, sizeof(int32_t));
            if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count || (0 != (text_byte_count % sizeof(uint16_t))))
            {
                return false;
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
        if (internal_header.is_text_encoding_utf8)
        {
            int32_t text_byte_count;
            intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count, sizeof(int32_t));
            if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count)
            {
                return false;
            }

            if (0 != text_byte_count)
            {
                if (-1 == input_stream_seek_callback(input_stream, text_byte_count, PT_INPUT_STREAM_SEEK_CUR))
                {
                    return false;
                }
            }
        }
        else
        {
            int32_t text_byte_count;
            intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count, sizeof(int32_t));
            if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count || (0 != (text_byte_count % sizeof(uint16_t))))
            {
                return false;
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
                          + sizeof(float),
                      PT_INPUT_STREAM_SEEK_CUR))
        {
            return false;
        }

        // texture index
        uint32_t texture_index;
        {
            if (1U == internal_header.texture_index_size)
            {
                int8_t texture_index_int8;
                {
                    intptr_t res_read = input_stream_read_callback(input_stream, &texture_index_int8, sizeof(int8_t));
                    if (res_read == -1 || sizeof(int8_t) != static_cast<size_t>(res_read) || 0 > texture_index_int8)
                    {
                        return false;
                    }
                }

                texture_index = texture_index_int8;
            }
            else if (2U == internal_header.texture_index_size)
            {
                int16_t texture_index_int16;
                {
                    intptr_t res_read = input_stream_read_callback(input_stream, &texture_index_int16, sizeof(int16_t));
                    if (res_read == -1 || sizeof(int16_t) != static_cast<size_t>(res_read) || 0 > texture_index_int16)
                    {
                        return false;
                    }
                }

                texture_index = texture_index_int16;
            }
            else if (4U == internal_header.texture_index_size)
            {
                intptr_t res_read = input_stream_read_callback(input_stream, &texture_index, sizeof(int32_t));
                if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > texture_index)
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }

        mcrt_vector<uint8_t> const &material_texture_path = mesh_texture_paths[texture_index];
        if (out_material_texture_size)
        {
            out_material_texture_size[material_index] = material_texture_path.size();
        }

        if (out_material_texture_path)
        {
            mcrt_memcpy(out_material_texture_path[material_index], &material_texture_path[0], material_texture_path.size());
        }

        if (-1 == input_stream_seek_callback(
                      input_stream,
                      // environment index
                      internal_header.texture_index_size
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
            if (-1 == input_stream_seek_callback(input_stream, internal_header.texture_index_size, PT_INPUT_STREAM_SEEK_CUR))
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
        if (internal_header.is_text_encoding_utf8)
        {
            int32_t text_byte_count;
            intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count, sizeof(int32_t));
            if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count)
            {
                return false;
            }

            if (0 != text_byte_count)
            {
                if (-1 == input_stream_seek_callback(input_stream, text_byte_count, PT_INPUT_STREAM_SEEK_CUR))
                {
                    return false;
                }
            }
        }
        else
        {
            int32_t text_byte_count;
            intptr_t res_read = input_stream_read_callback(input_stream, &text_byte_count, sizeof(int32_t));
            if (res_read == -1 || sizeof(int32_t) != static_cast<size_t>(res_read) || 0 > text_byte_count || (0 != (text_byte_count % sizeof(uint16_t))))
            {
                return false;
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

        mcrt_map<uint32_t, uint32_t> mesh_vertex_indices_to_material_vertex_indices;
        mcrt_vector<uint32_t> material_indices(static_cast<size_t>(material_surface_count));
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

        material_total_surface_count += material_surface_count;

        // validate primitive headers
        assert(static_cast<uint32_t>(mesh_vertex_indices_to_material_vertex_indices.size()) == mesh_primitive_asset_header_for_validate[material_index].vertex_count);
        assert(static_cast<uint32_t>(material_indices.size()) == mesh_primitive_asset_header_for_validate[material_index].index_count);

        // write to staging buffer
        for (std::pair<int32_t, uint32_t> const &pair : mesh_vertex_indices_to_material_vertex_indices)
        {
            uint32_t const mesh_vertex_index = pair.first;
            uint32_t const material_vertex_index = pair.second;

            pt_math_vec3 *out_position = reinterpret_cast<pt_math_vec3 *>(reinterpret_cast<uintptr_t>(staging_pointer) + memcpy_dests[material_index].position_staging_offset + sizeof(pt_gfx_mesh_neutral_vertex_position) * material_vertex_index + offsetof(pt_gfx_mesh_neutral_vertex_position, position));
            (*out_position) = vertices_positions[mesh_vertex_index];

            pt_math_vec3 *out_normal = reinterpret_cast<pt_math_vec3 *>(reinterpret_cast<uintptr_t>(staging_pointer) + memcpy_dests[material_index].varying_staging_offset + sizeof(pt_gfx_mesh_neutral_vertex_varying) * material_vertex_index + offsetof(pt_gfx_mesh_neutral_vertex_varying, normal));
            (*out_normal) = vertices_normals[mesh_vertex_index];

            // pt_math_vec3 *out_tangent = reinterpret_cast<pt_math_vec3 *>(reinterpret_cast<uintptr_t>(staging_pointer) + memcpy_dests[material_index].varying_staging_offset + sizeof(pt_gfx_mesh_neutral_vertex_varying) * material_vertex_index + offsetof(pt_gfx_mesh_neutral_vertex_varying, tangent));
            //(*out_tangent) =

            pt_math_vec2 *out_uv = reinterpret_cast<pt_math_vec2 *>(reinterpret_cast<uintptr_t>(staging_pointer) + memcpy_dests[material_index].varying_staging_offset + sizeof(pt_gfx_mesh_neutral_vertex_varying) * material_vertex_index + offsetof(pt_gfx_mesh_neutral_vertex_varying, uv));
            (*out_uv) = vertices_uvs[mesh_vertex_index];
        }

        if (0XFFFFFFFFU > mesh_vertex_indices_to_material_vertex_indices.size())
        {
            // validate primitive headers
            assert(mesh_primitive_asset_header_for_validate[material_index].is_index_type_uint16);

            assert(material_indices.size() == material_surface_count);
            for (uint32_t material_surface_index = 0; material_surface_index < material_surface_count; ++material_surface_index)
            {
                uint32_t material_vertex_index = material_indices[material_surface_index];

                uint16_t *out_index = reinterpret_cast<uint16_t *>(reinterpret_cast<uintptr_t>(staging_pointer) + memcpy_dests[material_index].index_staging_offset + sizeof(uint16_t) * material_surface_index);
                (*out_index) = material_vertex_index;
            }
        }
        else
        {
            // validate internal header
            assert(internal_header.vertex_index_size != 1 && internal_header.vertex_index_size != 2);

            // validate primitive headers
            assert(!mesh_primitive_asset_header_for_validate[material_index].is_index_type_uint16);

            assert(material_indices.size() == material_surface_count);
            for (uint32_t material_surface_index = 0; material_surface_index < material_surface_count; ++material_surface_index)
            {
                uint32_t material_vertex_index = material_indices[material_surface_index];

                uint32_t *out_index = reinterpret_cast<uint32_t *>(reinterpret_cast<uintptr_t>(staging_pointer) + memcpy_dests[material_index].index_staging_offset + sizeof(uint32_t) * material_surface_index);
                (*out_index) = material_vertex_index;
            }
        }
    }

    assert(surface_count == material_total_surface_count);

    return true;
}
