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
#include <assert.h>
#include "texture_asset_load.h"

// facade pattern

extern bool load_dds_header_from_input_stream(
    struct gfx_texture_neutral_header_t *neutral_header, size_t *neutral_data_offset,
    pt_input_stream_ref gfx_input_stream, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback);

extern bool load_dds_data_from_input_stream(
    struct gfx_texture_neutral_header_t const *common_header_for_validate, size_t const *common_data_offset_for_validate,
    uint8_t *staging_pointer, size_t num_subresources, struct gfx_texture_neutral_memcpy_dest_t const *memcpy_dest,
    uint32_t (*calculate_subresource_index_callback)(uint32_t mipLevel, uint32_t arrayLayer, uint32_t aspectIndex, uint32_t mip_levels, uint32_t array_layers),
    pt_input_stream_ref gfx_input_stream, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback);

extern bool load_pvr_header_from_input_stream(
    struct gfx_texture_neutral_header_t *neutral_header, size_t *neutral_data_offset,
    pt_input_stream_ref gfx_input_stream, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback);

extern bool load_pvr_data_from_input_stream(
    struct gfx_texture_neutral_header_t const *common_header_for_validate, size_t const *common_data_offset_for_validate,
    uint8_t *staging_pointer, size_t num_subresources, struct gfx_texture_neutral_memcpy_dest_t const *memcpy_dest,
    uint32_t (*calculate_subresource_index_callback)(uint32_t mipLevel, uint32_t arrayLayer, uint32_t aspectIndex, uint32_t mip_levels, uint32_t array_layers),
    pt_input_stream_ref gfx_input_stream, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback);

static inline constexpr uint32_t Common_MakeFourCC(char ch0, char ch1, char ch2, char ch3)
{
    return static_cast<uint32_t>(static_cast<uint8_t>(ch0)) | (static_cast<uint32_t>(static_cast<uint8_t>(ch1)) << 8) | (static_cast<uint32_t>(static_cast<uint8_t>(ch2)) << 16) | (static_cast<uint32_t>(static_cast<uint8_t>(ch3)) << 24);
}

enum
{
    DDS_MAGIC = Common_MakeFourCC('D', 'D', 'S', ' '),
    PVR_HEADER_V1 = 44,
    PVR_HEADER_V2 = 52,
    PVR_HEADER_V3 = Common_MakeFourCC('P', 'V', 'R', 3)
};

bool texture_load_header_from_input_stream(
    struct gfx_texture_neutral_header_t *neutral_header, size_t *neutral_data_offset,
    pt_input_stream_ref gfx_input_stream, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback)
{
    if (input_stream_seek_callback(gfx_input_stream, 0, PT_INPUT_STREAM_SEEK_SET) == -1)
    {
        return false;
    }

    uint32_t dwMagicNumber;
    {
        ptrdiff_t BytesRead = input_stream_read_callback(gfx_input_stream, &dwMagicNumber, sizeof(uint32_t));
        if (BytesRead == -1 || static_cast<size_t>(BytesRead) < sizeof(uint32_t))
        {
            return false;
        }
    }

    if (input_stream_seek_callback(gfx_input_stream, 0, PT_INPUT_STREAM_SEEK_SET) == -1)
    {
        return false;
    }

    switch (dwMagicNumber)
    {
    case DDS_MAGIC:
        return load_dds_header_from_input_stream(neutral_header, neutral_data_offset, gfx_input_stream, input_stream_read_callback, input_stream_seek_callback);
    case PVR_HEADER_V3:
    case PVR_HEADER_V2:
    case PVR_HEADER_V1:
        return load_pvr_header_from_input_stream(neutral_header, neutral_data_offset, gfx_input_stream, input_stream_read_callback, input_stream_seek_callback);
    default:
        return false;
    }
}

bool texture_load_data_from_input_stream(
    struct gfx_texture_neutral_header_t const *common_header_for_validate, size_t const *common_data_offset_for_validate,
    void *_staging_pointer, size_t num_subresources, struct gfx_texture_neutral_memcpy_dest_t const *memcpy_dest,
    uint32_t (*calculate_subresource_index_callback)(uint32_t mipLevel, uint32_t arrayLayer, uint32_t aspectIndex, uint32_t mip_levels, uint32_t array_layers),
    pt_input_stream_ref gfx_input_stream, pt_input_stream_read_callback input_stream_read_callback, pt_input_stream_seek_callback input_stream_seek_callback)
{
    uint8_t *staging_pointer = static_cast<uint8_t *>(_staging_pointer);

    if (input_stream_seek_callback(gfx_input_stream, 0, PT_INPUT_STREAM_SEEK_SET) == -1)
    {
        return false;
    }

    uint32_t dwMagicNumber;
    {
        ptrdiff_t BytesRead = input_stream_read_callback(gfx_input_stream, &dwMagicNumber, sizeof(uint32_t));
        if (BytesRead == -1 || static_cast<size_t>(BytesRead) < sizeof(uint32_t))
        {
            return false;
        }
    }

    switch (dwMagicNumber)
    {
    case DDS_MAGIC:
        return load_dds_data_from_input_stream(common_header_for_validate, common_data_offset_for_validate, staging_pointer, num_subresources, memcpy_dest, calculate_subresource_index_callback, gfx_input_stream, input_stream_read_callback, input_stream_seek_callback);
    case PVR_HEADER_V3:
    case PVR_HEADER_V2:
    case PVR_HEADER_V1:
        return load_pvr_data_from_input_stream(common_header_for_validate, common_data_offset_for_validate, staging_pointer, num_subresources, memcpy_dest, calculate_subresource_index_callback, gfx_input_stream, input_stream_read_callback, input_stream_seek_callback);
    default:
        return false;
    }
}
