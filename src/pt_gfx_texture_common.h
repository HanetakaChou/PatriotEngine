/*
 * Copyright (C) YuqiaoZhang張羽喬(HanetakaYuminaga弓長羽高)
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

#ifndef _PT_GFX_TEXTURE_COMMON_H_
#define _PT_GFX_TEXTURE_COMMON_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_gfx_connection.h>
#include <pt_mcrt_scalable_allocator.h>
#include <string>

class gfx_texture_common
{
protected:
    enum gfx_texture_common_type_t
    {
        PT_GFX_TEXTURE_COMMON_TYPE_1D,
        PT_GFX_TEXTURE_COMMON_TYPE_2D,
        PT_GFX_TEXTURE_COMMON_TYPE_3D,
        PT_GFX_TEXTURE_COMMON_TYPE_RANGE_SIZE,
        PT_GFX_TEXTURE_COMMON_TYPE_UNDEFINED
    };

    enum gfx_texture_common_format_t
    {
        PT_GFX_TEXTURE_COMMON_FORMAT_R4G4_UNORM_PACK8,
        PT_GFX_TEXTURE_COMMON_FORMAT_R4G4B4A4_UNORM_PACK16,
        PT_GFX_TEXTURE_COMMON_FORMAT_B4G4R4A4_UNORM_PACK16,
        PT_GFX_TEXTURE_COMMON_FORMAT_R5G6B5_UNORM_PACK16,
        PT_GFX_TEXTURE_COMMON_FORMAT_B5G6R5_UNORM_PACK16,
        PT_GFX_TEXTURE_COMMON_FORMAT_R5G5B5A1_UNORM_PACK16,
        PT_GFX_TEXTURE_COMMON_FORMAT_B5G5R5A1_UNORM_PACK16,
        PT_GFX_TEXTURE_COMMON_FORMAT_A1R5G5B5_UNORM_PACK16,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8_UNORM,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8_SNORM,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8_USCALED,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8_SSCALED,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8_UINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8_SINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8_SRGB,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8G8_UNORM,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8G8_SNORM,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8G8_USCALED,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8G8_SSCALED,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8G8_UINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8G8_SINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8G8_SRGB,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8G8B8_UNORM,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8G8B8_SNORM,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8G8B8_USCALED,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8G8B8_SSCALED,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8G8B8_UINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8G8B8_SINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8G8B8_SRGB,
        PT_GFX_TEXTURE_COMMON_FORMAT_B8G8R8_UNORM,
        PT_GFX_TEXTURE_COMMON_FORMAT_B8G8R8_SNORM,
        PT_GFX_TEXTURE_COMMON_FORMAT_B8G8R8_USCALED,
        PT_GFX_TEXTURE_COMMON_FORMAT_B8G8R8_SSCALED,
        PT_GFX_TEXTURE_COMMON_FORMAT_B8G8R8_UINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_B8G8R8_SINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_B8G8R8_SRGB,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8G8B8A8_UNORM,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8G8B8A8_SNORM,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8G8B8A8_USCALED,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8G8B8A8_SSCALED,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8G8B8A8_UINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8G8B8A8_SINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R8G8B8A8_SRGB,
        PT_GFX_TEXTURE_COMMON_FORMAT_B8G8R8A8_UNORM,
        PT_GFX_TEXTURE_COMMON_FORMAT_B8G8R8A8_SNORM,
        PT_GFX_TEXTURE_COMMON_FORMAT_B8G8R8A8_USCALED,
        PT_GFX_TEXTURE_COMMON_FORMAT_B8G8R8A8_SSCALED,
        PT_GFX_TEXTURE_COMMON_FORMAT_B8G8R8A8_UINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_B8G8R8A8_SINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_B8G8R8A8_SRGB,
        PT_GFX_TEXTURE_COMMON_FORMAT_A8B8G8R8_UNORM_PACK32,
        PT_GFX_TEXTURE_COMMON_FORMAT_A8B8G8R8_SNORM_PACK32,
        PT_GFX_TEXTURE_COMMON_FORMAT_A8B8G8R8_USCALED_PACK32,
        PT_GFX_TEXTURE_COMMON_FORMAT_A8B8G8R8_SSCALED_PACK32,
        PT_GFX_TEXTURE_COMMON_FORMAT_A8B8G8R8_UINT_PACK32,
        PT_GFX_TEXTURE_COMMON_FORMAT_A8B8G8R8_SINT_PACK32,
        PT_GFX_TEXTURE_COMMON_FORMAT_A8B8G8R8_SRGB_PACK32,
        PT_GFX_TEXTURE_COMMON_FORMAT_A2R10G10B10_UNORM_PACK32,
        PT_GFX_TEXTURE_COMMON_FORMAT_A2R10G10B10_SNORM_PACK32,
        PT_GFX_TEXTURE_COMMON_FORMAT_A2R10G10B10_USCALED_PACK32,
        PT_GFX_TEXTURE_COMMON_FORMAT_A2R10G10B10_SSCALED_PACK32,
        PT_GFX_TEXTURE_COMMON_FORMAT_A2R10G10B10_UINT_PACK32,
        PT_GFX_TEXTURE_COMMON_FORMAT_A2R10G10B10_SINT_PACK32,
        PT_GFX_TEXTURE_COMMON_FORMAT_A2B10G10R10_UNORM_PACK32,
        PT_GFX_TEXTURE_COMMON_FORMAT_A2B10G10R10_SNORM_PACK32,
        PT_GFX_TEXTURE_COMMON_FORMAT_A2B10G10R10_USCALED_PACK32,
        PT_GFX_TEXTURE_COMMON_FORMAT_A2B10G10R10_SSCALED_PACK32,
        PT_GFX_TEXTURE_COMMON_FORMAT_A2B10G10R10_UINT_PACK32,
        PT_GFX_TEXTURE_COMMON_FORMAT_A2B10G10R10_SINT_PACK32,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16_UNORM,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16_SNORM,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16_USCALED,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16_SSCALED,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16_UINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16_SINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16_SFLOAT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16G16_UNORM,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16G16_SNORM,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16G16_USCALED,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16G16_SSCALED,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16G16_UINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16G16_SINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16G16_SFLOAT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16G16B16_UNORM,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16G16B16_SNORM,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16G16B16_USCALED,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16G16B16_SSCALED,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16G16B16_UINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16G16B16_SINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16G16B16_SFLOAT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16G16B16A16_UNORM,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16G16B16A16_SNORM,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16G16B16A16_USCALED,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16G16B16A16_SSCALED,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16G16B16A16_UINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16G16B16A16_SINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R16G16B16A16_SFLOAT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R32_UINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R32_SINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R32_SFLOAT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R32G32_UINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R32G32_SINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R32G32_SFLOAT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R32G32B32_UINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R32G32B32_SINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R32G32B32_SFLOAT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R32G32B32A32_UINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R32G32B32A32_SINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R32G32B32A32_SFLOAT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R64_UINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R64_SINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R64_SFLOAT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R64G64_UINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R64G64_SINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R64G64_SFLOAT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R64G64B64_UINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R64G64B64_SINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R64G64B64_SFLOAT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R64G64B64A64_UINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R64G64B64A64_SINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_R64G64B64A64_SFLOAT,
        PT_GFX_TEXTURE_COMMON_FORMAT_B10G11R11_UFLOAT_PACK32,
        PT_GFX_TEXTURE_COMMON_FORMAT_E5B9G9R9_UFLOAT_PACK32,
        PT_GFX_TEXTURE_COMMON_FORMAT_D16_UNORM,
        PT_GFX_TEXTURE_COMMON_FORMAT_X8_D24_UNORM_PACK32,
        PT_GFX_TEXTURE_COMMON_FORMAT_D32_SFLOAT,
        PT_GFX_TEXTURE_COMMON_FORMAT_S8_UINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_D16_UNORM_S8_UINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_D24_UNORM_S8_UINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_D32_SFLOAT_S8_UINT,
        PT_GFX_TEXTURE_COMMON_FORMAT_BC1_RGB_UNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_BC1_RGB_SRGB_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_BC1_RGBA_UNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_BC1_RGBA_SRGB_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_BC2_UNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_BC2_SRGB_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_BC3_UNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_BC3_SRGB_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_BC4_UNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_BC4_SNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_BC5_UNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_BC5_SNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_BC6H_UFLOAT_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_BC6H_SFLOAT_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_BC7_UNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_BC7_SRGB_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ETC2_R8G8B8_UNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ETC2_R8G8B8_SRGB_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_EAC_R11_UNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_EAC_R11_SNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_EAC_R11G11_UNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_EAC_R11G11_SNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_4x4_UNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_4x4_SRGB_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_5x4_UNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_5x4_SRGB_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_5x5_UNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_5x5_SRGB_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_6x5_UNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_6x5_SRGB_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_6x6_UNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_6x6_SRGB_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_8x5_UNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_8x5_SRGB_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_8x6_UNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_8x6_SRGB_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_8x8_UNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_8x8_SRGB_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_10x5_UNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_10x5_SRGB_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_10x6_UNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_10x6_SRGB_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_10x8_UNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_10x8_SRGB_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_10x10_UNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_10x10_SRGB_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_12x10_UNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_12x10_SRGB_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_12x12_UNORM_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_ASTC_12x12_SRGB_BLOCK,
        PT_GFX_TEXTURE_COMMON_FORMAT_RANGE_SIZE,
        PT_GFX_TEXTURE_COMMON_FORMAT_UNDEFINED
    };

    struct common_header_t
    {
        bool isCubeMap;
        enum gfx_texture_common_type_t type;
        enum gfx_texture_common_format_t format;
        uint32_t width;
        uint32_t height;
        uint32_t depth;
        uint32_t mipLevels;
        uint32_t arrayLayers;
    };

    struct load_memcpy_dest_t
    {
        size_t stagingOffset;
        size_t outputRowPitch;
        size_t outputRowSize;
        size_t outputNumRows;
        size_t outputSlicePitch;
        size_t outputNumSlices;
    };

private:
    //using vector = std::vector<T, mcrt::scalable_allocator<T>>;
    using mcrt_string = std::basic_string<char, std::char_traits<char>, mcrt::scalable_allocator<char>>;
    mcrt_string m_asset_filename;

    static inline enum gfx_texture_common_type_t dds_get_common_type(uint32_t dds_type);

    static inline enum gfx_texture_common_format_t dds_get_common_format(uint32_t dds_format);

    static bool load_dds_header_from_input_stream(
        struct common_header_t *common_header, size_t *common_data_offset,
        gfx_input_stream_ref input_stream, intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence));

    static bool load_dds_data_from_input_stream(
        struct common_header_t const *common_header_for_validate, size_t const *common_data_offset_for_validate,
        uint8_t *staging_pointer, size_t num_subresources, struct load_memcpy_dest_t const *memcpy_dest,
        uint32_t (*calc_subresource_callback)(uint32_t mipLevel, uint32_t arrayLayer, uint32_t aspectIndex, uint32_t mipLevels, uint32_t arrayLayers),
        gfx_input_stream_ref input_stream, intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence));

    static inline enum gfx_texture_common_type_t pvr_get_common_type(uint32_t height, uint32_t depth);

    static inline enum gfx_texture_common_format_t pvr_get_common_format(uint64_t pixelFormat, uint32_t colorSpace, uint32_t channelType);

    static bool load_pvr_header_from_input_stream(
        struct common_header_t *common_header, size_t *common_data_offset,
        gfx_input_stream_ref input_stream, intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence));

    static bool load_pvr_data_from_input_stream(
        struct common_header_t const *common_header_for_validate, size_t const *common_data_offset_for_validate,
        uint8_t *staging_pointer, size_t num_subresources, struct load_memcpy_dest_t const *memcpy_dest,
        uint32_t (*calc_subresource_callback)(uint32_t mipLevel, uint32_t arrayLayer, uint32_t aspectIndex, uint32_t mipLevels, uint32_t arrayLayers),
        gfx_input_stream_ref input_stream, intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence));

protected:
    enum
    {
        PT_GFX_TEXTURE_COMMON_TYPE_RANGE_SIZE_PROTECTED = PT_GFX_TEXTURE_COMMON_TYPE_RANGE_SIZE
    };

    enum
    {
        PT_GFX_TEXTURE_COMMON_FORMAT_RANGE_SIZE_PROTECTED = PT_GFX_TEXTURE_COMMON_FORMAT_RANGE_SIZE
    };

    static bool load_header_from_input_stream(
        struct common_header_t *common_header, size_t *common_data_offset,
        gfx_input_stream_ref input_stream, intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence));

    static bool load_data_from_input_stream(
        struct common_header_t const *common_header_for_validate, size_t const *common_data_offset_for_validate,
        uint8_t *staging_pointer, size_t num_subresources, struct load_memcpy_dest_t const *memcpy_dest,
        uint32_t (*calc_subresource_callback)(uint32_t mipLevel, uint32_t arrayLayer, uint32_t aspectIndex, uint32_t mipLevels, uint32_t arrayLayers),
        gfx_input_stream_ref input_stream, intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence));

public:
    virtual void destroy() = 0;

    virtual bool read_input_stream(char const *initial_filename,
                                   gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename),
                                   intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count),
                                   int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence),
                                   void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream)) = 0;
};

#endif
