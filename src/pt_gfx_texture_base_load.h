//
// Copyright (C) YuqiaoZhang(HanetakaYuminaga)
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

#ifndef _PT_GFX_TEXTURE_BASE_LOAD_H_
#define _PT_GFX_TEXTURE_BASE_LOAD_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_gfx_connection.h>

enum gfx_texture_neutral_type_t
{
    PT_GFX_TEXTURE_NEUTRAL_TYPE_1D,
    PT_GFX_TEXTURE_NEUTRAL_TYPE_2D,
    PT_GFX_TEXTURE_NEUTRAL_TYPE_3D,
    PT_GFX_TEXTURE_NEUTRAL_TYPE_RANGE_SIZE,
    PT_GFX_TEXTURE_NEUTRAL_TYPE_UNDEFINED
};

enum gfx_texture_neutral_format_t
{
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R4G4_UNORM_PACK8,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R4G4B4A4_UNORM_PACK16,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_B4G4R4A4_UNORM_PACK16,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R5G6B5_UNORM_PACK16,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_B5G6R5_UNORM_PACK16,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R5G5B5A1_UNORM_PACK16,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_B5G5R5A1_UNORM_PACK16,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_A1R5G5B5_UNORM_PACK16,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8_UNORM,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8_SNORM,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8_USCALED,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8_SSCALED,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8_UINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8_SINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8_SRGB,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8G8_UNORM,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8G8_SNORM,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8G8_USCALED,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8G8_SSCALED,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8G8_UINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8G8_SINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8G8_SRGB,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8G8B8_UNORM,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8G8B8_SNORM,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8G8B8_USCALED,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8G8B8_SSCALED,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8G8B8_UINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8G8B8_SINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8G8B8_SRGB,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_B8G8R8_UNORM,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_B8G8R8_SNORM,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_B8G8R8_USCALED,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_B8G8R8_SSCALED,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_B8G8R8_UINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_B8G8R8_SINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_B8G8R8_SRGB,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8G8B8A8_UNORM,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8G8B8A8_SNORM,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8G8B8A8_USCALED,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8G8B8A8_SSCALED,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8G8B8A8_UINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8G8B8A8_SINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R8G8B8A8_SRGB,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_B8G8R8A8_UNORM,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_B8G8R8A8_SNORM,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_B8G8R8A8_USCALED,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_B8G8R8A8_SSCALED,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_B8G8R8A8_UINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_B8G8R8A8_SINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_B8G8R8A8_SRGB,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_A8B8G8R8_UNORM_PACK32,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_A8B8G8R8_SNORM_PACK32,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_A8B8G8R8_USCALED_PACK32,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_A8B8G8R8_SSCALED_PACK32,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_A8B8G8R8_UINT_PACK32,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_A8B8G8R8_SINT_PACK32,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_A8B8G8R8_SRGB_PACK32,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_A2R10G10B10_UNORM_PACK32,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_A2R10G10B10_SNORM_PACK32,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_A2R10G10B10_USCALED_PACK32,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_A2R10G10B10_SSCALED_PACK32,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_A2R10G10B10_UINT_PACK32,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_A2R10G10B10_SINT_PACK32,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_A2B10G10R10_UNORM_PACK32,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_A2B10G10R10_SNORM_PACK32,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_A2B10G10R10_USCALED_PACK32,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_A2B10G10R10_SSCALED_PACK32,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_A2B10G10R10_UINT_PACK32,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_A2B10G10R10_SINT_PACK32,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16_UNORM,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16_SNORM,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16_USCALED,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16_SSCALED,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16_UINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16_SINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16_SFLOAT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16G16_UNORM,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16G16_SNORM,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16G16_USCALED,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16G16_SSCALED,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16G16_UINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16G16_SINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16G16_SFLOAT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16G16B16_UNORM,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16G16B16_SNORM,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16G16B16_USCALED,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16G16B16_SSCALED,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16G16B16_UINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16G16B16_SINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16G16B16_SFLOAT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16G16B16A16_UNORM,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16G16B16A16_SNORM,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16G16B16A16_USCALED,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16G16B16A16_SSCALED,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16G16B16A16_UINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16G16B16A16_SINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R16G16B16A16_SFLOAT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R32_UINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R32_SINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R32_SFLOAT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R32G32_UINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R32G32_SINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R32G32_SFLOAT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R32G32B32_UINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R32G32B32_SINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R32G32B32_SFLOAT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R32G32B32A32_UINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R32G32B32A32_SINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R32G32B32A32_SFLOAT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R64_UINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R64_SINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R64_SFLOAT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R64G64_UINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R64G64_SINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R64G64_SFLOAT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R64G64B64_UINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R64G64B64_SINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R64G64B64_SFLOAT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R64G64B64A64_UINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R64G64B64A64_SINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_R64G64B64A64_SFLOAT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_B10G11R11_UFLOAT_PACK32,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_E5B9G9R9_UFLOAT_PACK32,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_D16_UNORM,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_X8_D24_UNORM_PACK32,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_D32_SFLOAT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_S8_UINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_D16_UNORM_S8_UINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_D24_UNORM_S8_UINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_D32_SFLOAT_S8_UINT,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_BC1_RGB_UNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_BC1_RGB_SRGB_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_BC1_RGBA_UNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_BC1_RGBA_SRGB_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_BC2_UNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_BC2_SRGB_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_BC3_UNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_BC3_SRGB_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_BC4_UNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_BC4_SNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_BC5_UNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_BC5_SNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_BC6H_UFLOAT_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_BC6H_SFLOAT_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_BC7_UNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_BC7_SRGB_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ETC2_R8G8B8_UNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ETC2_R8G8B8_SRGB_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_EAC_R11_UNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_EAC_R11_SNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_EAC_R11G11_UNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_EAC_R11G11_SNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_4x4_UNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_4x4_SRGB_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_5x4_UNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_5x4_SRGB_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_5x5_UNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_5x5_SRGB_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_6x5_UNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_6x5_SRGB_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_6x6_UNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_6x6_SRGB_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_8x5_UNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_8x5_SRGB_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_8x6_UNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_8x6_SRGB_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_8x8_UNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_8x8_SRGB_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_10x5_UNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_10x5_SRGB_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_10x6_UNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_10x6_SRGB_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_10x8_UNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_10x8_SRGB_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_10x10_UNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_10x10_SRGB_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_12x10_UNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_12x10_SRGB_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_12x12_UNORM_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_ASTC_12x12_SRGB_BLOCK,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_RANGE_SIZE,
    PT_GFX_TEXTURE_NEUTRAL_FORMAT_UNDEFINED
};

struct gfx_texture_neutral_header_t
{
    bool is_cube_map;
    enum gfx_texture_neutral_type_t type;
    enum gfx_texture_neutral_format_t format;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t mip_levels;
    uint32_t array_layers;
};

struct gfx_texture_neutral_memcpy_dest_t
{
    size_t staging_offset;
    uint32_t output_row_pitch;
    uint32_t output_row_size;
    uint32_t output_num_rows;
    uint32_t output_slice_pitch;
    uint32_t output_num_slices;
};

bool load_header_from_input_stream(
    struct gfx_texture_neutral_header_t *neutral_header, size_t *neutral_data_offset,
    gfx_input_stream_ref gfx_input_stream, intptr_t(PT_PTR *gfx_input_stream_read_callback)(gfx_input_stream_ref, void *, size_t), int64_t(PT_PTR *gfx_input_stream_seek_callback)(gfx_input_stream_ref, int64_t, int));

bool load_data_from_input_stream(
    struct gfx_texture_neutral_header_t const *common_header_for_validate, size_t const *common_data_offset_for_validate,
    void *staging_pointer, size_t num_subresources, struct gfx_texture_neutral_memcpy_dest_t const *memcpy_dest,
    uint32_t (*calculate_subresource_index_callback)(uint32_t mipLevel, uint32_t arrayLayer, uint32_t aspectIndex, uint32_t mip_levels, uint32_t array_layers),
    gfx_input_stream_ref gfx_input_stream, intptr_t(PT_PTR *gfx_input_stream_read_callback)(gfx_input_stream_ref, void *, size_t), int64_t(PT_PTR *gfx_input_stream_seek_callback)(gfx_input_stream_ref, int64_t, int));

struct gfx_texture_backend_header_t
{
    uint8_t m_user_data[128];
};

#endif