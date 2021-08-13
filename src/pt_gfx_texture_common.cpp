/*
 * Copyright (C) YuqiaoZhang(HanetakaYuminaga)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stddef.h>
#include "pt_gfx_texture_common.h"

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

bool gfx_texture_common::load_header_from_input_stream(
    struct common_header_t *common_header, size_t *common_data_offset,
    gfx_input_stream_ref input_stream, intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence))
{
    if (input_stream_seek_callback(input_stream, 0, PT_GFX_INPUT_STREAM_SEEK_SET) == -1)
    {
        return false;
    }

    uint32_t dwMagicNumber;
    {
        ptrdiff_t BytesRead = input_stream_read_callback(input_stream, &dwMagicNumber, sizeof(uint32_t));
        if (BytesRead == -1 || static_cast<size_t>(BytesRead) < sizeof(uint32_t))
        {
            return false;
        }
    }

    if (input_stream_seek_callback(input_stream, 0, PT_GFX_INPUT_STREAM_SEEK_SET) == -1)
    {
        return false;
    }

    switch (dwMagicNumber)
    {
    case DDS_MAGIC:
        return load_dds_header_from_input_stream(common_header, common_data_offset, input_stream, input_stream_read_callback, input_stream_seek_callback);
    case PVR_HEADER_V3:
    case PVR_HEADER_V2:
    case PVR_HEADER_V1:
        return load_pvr_header_from_input_stream(common_header, common_data_offset, input_stream, input_stream_read_callback, input_stream_seek_callback);
    default:
        return false;
    }
}

bool gfx_texture_common::load_data_from_input_stream(
    struct common_header_t const *common_header_for_validate, size_t const *common_data_offset_for_validate,
    void *_staging_pointer, size_t num_subresources, struct load_memcpy_dest_t const *memcpy_dest,
    uint32_t (*calc_subresource_callback)(uint32_t mipLevel, uint32_t arrayLayer, uint32_t aspectIndex, uint32_t mipLevels, uint32_t arrayLayers),
    gfx_input_stream_ref input_stream, intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence))
{
    uint8_t *staging_pointer = static_cast<uint8_t *>(_staging_pointer);

    if (input_stream_seek_callback(input_stream, 0, PT_GFX_INPUT_STREAM_SEEK_SET) == -1)
    {
        return false;
    }

    uint32_t dwMagicNumber;
    {
        ptrdiff_t BytesRead = input_stream_read_callback(input_stream, &dwMagicNumber, sizeof(uint32_t));
        if (BytesRead == -1 || static_cast<size_t>(BytesRead) < sizeof(uint32_t))
        {
            return false;
        }
    }

    switch (dwMagicNumber)
    {
    case DDS_MAGIC:
        return load_dds_data_from_input_stream(common_header_for_validate, common_data_offset_for_validate, staging_pointer, num_subresources, memcpy_dest, calc_subresource_callback, input_stream, input_stream_read_callback, input_stream_seek_callback);
    case PVR_HEADER_V3:
    case PVR_HEADER_V2:
    case PVR_HEADER_V1:
        return load_pvr_data_from_input_stream(common_header_for_validate, common_data_offset_for_validate, staging_pointer, num_subresources, memcpy_dest, calc_subresource_callback, input_stream, input_stream_read_callback, input_stream_seek_callback);
    default:
        return false;
    }
}

//--- export ---

inline gfx_connection_ref wrap(class gfx_connection_common *gfx_connection) { return reinterpret_cast<gfx_connection_ref>(gfx_connection); }
inline class gfx_connection_common *unwrap(gfx_connection_ref gfx_connection) { return reinterpret_cast<class gfx_connection_common *>(gfx_connection); }

inline gfx_texture_ref wrap(class gfx_texture_common *texture) { return reinterpret_cast<gfx_texture_ref>(texture); }
inline class gfx_texture_common *unwrap(gfx_texture_ref texture) { return reinterpret_cast<class gfx_texture_common *>(texture); }

PT_ATTR_GFX bool PT_CALL gfx_texture_read_input_stream(
    gfx_connection_ref gfx_connection,
    gfx_texture_ref texture,
    char const *initial_filename,
    gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename),
    intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count),
    int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence),
    void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream))
{
    return unwrap(texture)->read_input_stream(unwrap(gfx_connection), initial_filename, input_stream_init_callback, input_stream_read_callback, input_stream_seek_callback, input_stream_destroy_callback);
}

PT_ATTR_GFX void PT_CALL gfx_texture_destroy(gfx_connection_ref gfx_connection, gfx_texture_ref texture)
{
    return unwrap(texture)->destroy(unwrap(gfx_connection));
}
