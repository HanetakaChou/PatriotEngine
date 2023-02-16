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
#include <pt_mcrt_vector.h>
#include <pt_mcrt_set.h>
#include <pt_mcrt_map.h>
#include <pt_mcrt_memcpy.h>
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

struct material_asset_pmx_header_t
{
    bool is_text_encoding_utf8;
    uint32_t texture_index_size;
    uint32_t texture_count;
    uint32_t material_count;
    int64_t texture_section_offset;
    int64_t material_section_offset;
};

static bool inline internal_load_pmx_header_from_input_stream(
    pt_input_stream_ref input_stream, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback,
    struct material_asset_pmx_header_t *out_mateiral_asset_pmx_header);

extern bool material_asset_load_pmx_header_from_input_stream(
    pt_input_stream_ref input_stream, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback,
    size_t *out_material_count)
{
    struct material_asset_pmx_header_t material_asset_pmx_header;
    if (!internal_load_pmx_header_from_input_stream(input_stream, input_stream_read_callback, input_stream_seek_callback, &material_asset_pmx_header))
    {
        return false;
    }

    (*out_material_count) = material_asset_pmx_header.material_count;
    return true;
}

static bool inline internal_load_pmx_header_from_input_stream(
    pt_input_stream_ref input_stream, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback,
    struct material_asset_pmx_header_t *out_mateiral_asset_pmx_header)
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

    out_mateiral_asset_pmx_header->is_text_encoding_utf8 = is_text_encoding_utf8;
    out_mateiral_asset_pmx_header->texture_index_size = texture_index_size;
    out_mateiral_asset_pmx_header->texture_count = texture_count;
    out_mateiral_asset_pmx_header->material_count = material_count;
    out_mateiral_asset_pmx_header->texture_section_offset = texture_section_offset;
    out_mateiral_asset_pmx_header->material_section_offset = material_section_offset;

    return true;
}

static inline bool internal_utf16_to_utf8(uint16_t const *pInBuf, uint32_t *pInCharsLeft, uint8_t *pOutBuf, uint32_t *pOutCharsLeft);

extern bool mmaterial_asset_load_pmx_primitive_data_from_input_stream(
    pt_input_stream_ref input_stream, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback,
    char **out_material_texture_paths, size_t *out_material_texture_path_size)
{
    struct material_asset_pmx_header_t mateiral_asset_pmx_header;
    if (!internal_load_pmx_header_from_input_stream(input_stream, input_stream_read_callback, input_stream_seek_callback, &mateiral_asset_pmx_header))
    {
        return false;
    }

    // texture section
    if (-1 == input_stream_seek_callback(input_stream, mateiral_asset_pmx_header.texture_section_offset, PT_INPUT_STREAM_SEEK_SET))
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
    assert(mateiral_asset_pmx_header.texture_count == texture_count);

    mcrt_vector<mcrt_vector<uint8_t>> mesh_texture_paths(static_cast<size_t>(texture_count));
    {
        for (int32_t texture_index = 0; texture_index < texture_count; ++texture_index)
        {
            // path
            if (mateiral_asset_pmx_header.is_text_encoding_utf8)
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
    assert(mateiral_asset_pmx_header.material_count = material_count);

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
                          + sizeof(float),
                      PT_INPUT_STREAM_SEEK_CUR))
        {
            return false;
        }

        // texture index
        uint32_t texture_index;
        {
            if (1U == mateiral_asset_pmx_header.texture_index_size)
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
            else if (2U == mateiral_asset_pmx_header.texture_index_size)
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
            else if (4U == mateiral_asset_pmx_header.texture_index_size)
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
        if (out_material_texture_path_size)
        {
            out_material_texture_path_size[material_index] = material_texture_path.size();
        }

        if (out_material_texture_paths)
        {
            mcrt_memcpy(out_material_texture_paths[material_index], &material_texture_path[0], material_texture_path.size());
        }

        if (-1 == input_stream_seek_callback(
                      input_stream,
                      // environment index
                      mateiral_asset_pmx_header.texture_index_size
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
            if (-1 == input_stream_seek_callback(input_stream, mateiral_asset_pmx_header.texture_index_size, PT_INPUT_STREAM_SEEK_CUR))
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
    }

    return true;
}

static inline bool internal_utf16_to_utf8(uint16_t const *pInBuf, uint32_t *pInCharsLeft, uint8_t *pOutBuf, uint32_t *pOutCharsLeft)
{
    while ((*pInCharsLeft) >= 1)
    {
        uint32_t ucs4code = 0; // Accumulator

        // UTF-16 To UCS-4
        if ((*pInBuf) >= 0XD800U && (*pInBuf) <= 0XDBFFU) // 110110xxxxxxxxxx
        {
            if ((*pInCharsLeft) >= 2)
            {
                ucs4code += (((*pInBuf) - 0XD800U) << 10U); // Accumulate

                ++pInBuf;
                --(*pInCharsLeft);

                if ((*pInBuf) >= 0XDC00U && (*pInBuf) <= 0XDFFF) // 110111xxxxxxxxxx
                {
                    ucs4code += ((*pInBuf) - 0XDC00U); // Accumulate

                    ++pInBuf;
                    --(*pInCharsLeft);
                }
                else
                {
                    return false;
                }

                ucs4code += 0X10000U;
            }
            else
            {
                return false;
            }
        }
        else
        {
            ucs4code += (*pInBuf); // Accumulate

            ++pInBuf;
            --(*pInCharsLeft);
        }

        // UCS-4 To UTF-16
        if (ucs4code < 128U) // 0XXX XXXX
        {
            if ((*pOutCharsLeft) >= 1)
            {
                (*pOutBuf) = static_cast<uint8_t>(ucs4code);

                ++pOutBuf;
                --(*pOutCharsLeft);
            }
            else
            {
                return false;
            }
        }
        else if (ucs4code < 2048U) // 110X XXXX 10XX XXXX
        {
            if ((*pOutCharsLeft) >= 2)
            {
                (*pOutBuf) = static_cast<uint8_t>(((ucs4code & 0X7C0U) >> 6U) + 192U);

                ++pOutBuf;
                --(*pOutCharsLeft);

                (*pOutBuf) = static_cast<uint8_t>((ucs4code & 0X3FU) + 128U);

                ++pOutBuf;
                --(*pOutCharsLeft);
            }
            else
            {
                return false;
            }
        }
        else if (ucs4code < 0X10000U) // 1110 XXXX 10XX XXXX 10XX XXXX
        {
            if ((*pOutCharsLeft) >= 3)
            {
                (*pOutBuf) = static_cast<uint8_t>(((ucs4code & 0XF000U) >> 12U) + 224U);

                ++pOutBuf;
                --(*pOutCharsLeft);

                (*pOutBuf) = static_cast<uint8_t>(((ucs4code & 0XFC0U) >> 6U) + 128U);

                ++pOutBuf;
                --(*pOutCharsLeft);

                (*pOutBuf) = static_cast<uint8_t>((ucs4code & 0X3FU) + 128U);

                ++pOutBuf;
                --(*pOutCharsLeft);
            }
            else
            {
                return false;
            }
        }
        else if (ucs4code < 0X200000U) // 1111 0XXX 10XX XXXX 10XX XXXX 10XX XXXX
        {
            if ((*pOutCharsLeft) >= 4)
            {
                (*pOutBuf) = static_cast<uint8_t>(((ucs4code & 0X1C0000U) >> 18U) + 240U);

                ++pOutBuf;
                --(*pOutCharsLeft);

                (*pOutBuf) = static_cast<uint8_t>(((ucs4code & 0X3F000U) >> 12U) + 128U);

                ++pOutBuf;
                --(*pOutCharsLeft);

                (*pOutBuf) = static_cast<uint8_t>(((ucs4code & 0XFC0U) >> 6U) + 128U);

                ++pOutBuf;
                --(*pOutCharsLeft);

                (*pOutBuf) = static_cast<uint8_t>((ucs4code & 0X3FU) + 128U);

                ++pOutBuf;
                --(*pOutCharsLeft);
            }
            else // ucs4code >= 0X200000U
            {
                return false;
            }
        }
    }

    return true;
}
