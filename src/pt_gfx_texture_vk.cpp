/*
 * Copyright (C) YuqiaoZhang
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
#include <assert.h>
#include <pt_mcrt_malloc.h>
#include "pt_gfx_texture_vk.h"
#include <vulkan/vulkan.h>
#include <new>

void gfx_texture_vk::destroy()
{
    this->~gfx_texture_vk();
    mcrt_free(this);
}

bool gfx_texture_vk::read_input_stream(
    char const *initial_filename,
    gfx_input_stream(PT_PTR *input_stream_init_callback)(char const *initial_filename),
    intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream input_stream, void *buf, size_t count),
    int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream input_stream, int64_t offset, int whence),
    void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream input_stream))
{
    gfx_input_stream input_stream;
    {
        class gfx_input_stream_guard
        {
            gfx_input_stream &m_input_stream;
            void(PT_PTR *m_input_stream_destroy_callback)(gfx_input_stream input_stream);

        public:
            inline gfx_input_stream_guard(
                gfx_input_stream &input_stream,
                char const *initial_filename,
                gfx_input_stream(PT_PTR *input_stream_init_callback)(char const *initial_filename),
                void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream input_stream))
                : m_input_stream(input_stream),
                  m_input_stream_destroy_callback(input_stream_destroy_callback)
            {
                m_input_stream = input_stream_init_callback(initial_filename);
            }
            inline ~gfx_input_stream_guard()
            {
                m_input_stream_destroy_callback(m_input_stream);
            }
        } input_stream_guard(input_stream, initial_filename, input_stream_init_callback, input_stream_destroy_callback);

        struct common_header_t common_header;
        size_t common_data_offset;
        bool res = load_header_from_input_stream(&common_header, &common_data_offset, input_stream, input_stream_read_callback, input_stream_seek_callback);
        if (!res)
        {
            return false;
        }

        specific_header_vk_t specific_header_vk = common_to_specific_header_translate(&common_header);

        //multi-thread
        struct VkFormatProperties physical_device_format_properties;
        m_gfx_connection->get_physical_device_format_properties(specific_header_vk.format, &physical_device_format_properties);
        if (0 == (physical_device_format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT))
        {
            return false;
        }

        int huhu = 0;
    }

    return true;
}

inline struct gfx_texture_vk::specific_header_vk_t gfx_texture_vk::common_to_specific_header_translate(struct common_header_t const *common_header)
{
    struct specific_header_vk_t specific_header_vk;

    specific_header_vk.isCubeCompatible = common_header->isCubeMap;
    specific_header_vk.imageType = common_to_vulkan_type_translate(common_header->type);
    specific_header_vk.format = common_to_vulkan_format_translate(common_header->format);
    specific_header_vk.extent.width = common_header->width;
    specific_header_vk.extent.height = common_header->height;
    specific_header_vk.extent.depth = common_header->depth;
    specific_header_vk.mipLevels = common_header->mipLevels;
    specific_header_vk.arrayLayers = common_header->arrayLayers;

    return specific_header_vk;
}

inline enum VkImageType gfx_texture_vk::common_to_vulkan_type_translate(uint32_t common_type)
{
    static enum VkImageType const common_to_vulkan_type_map[] = {
        VK_IMAGE_TYPE_1D,
        VK_IMAGE_TYPE_2D,
        VK_IMAGE_TYPE_3D};
    static_assert(COMMON_TYPE_RANGE_SIZE_PROTECTED == (sizeof(common_to_vulkan_type_map) / sizeof(common_to_vulkan_type_map[0])), "COMMON_TYPE_RANGE_SIZE_PROTECTED == (sizeof(common_to_vulkan_type_map) / sizeof(common_to_vulkan_type_map[0]))");

    assert(common_type < (sizeof(common_to_vulkan_type_map) / sizeof(common_to_vulkan_type_map[0])));
    return common_to_vulkan_type_map[common_type];
}

inline enum VkFormat gfx_texture_vk::common_to_vulkan_format_translate(uint32_t common_format)
{
    static enum VkFormat const common_to_vulkan_format_map[] = {
        VK_FORMAT_R4G4_UNORM_PACK8,
        VK_FORMAT_R4G4B4A4_UNORM_PACK16,
        VK_FORMAT_B4G4R4A4_UNORM_PACK16,
        VK_FORMAT_R5G6B5_UNORM_PACK16,
        VK_FORMAT_B5G6R5_UNORM_PACK16,
        VK_FORMAT_R5G5B5A1_UNORM_PACK16,
        VK_FORMAT_B5G5R5A1_UNORM_PACK16,
        VK_FORMAT_A1R5G5B5_UNORM_PACK16,
        VK_FORMAT_R8_UNORM,
        VK_FORMAT_R8_SNORM,
        VK_FORMAT_R8_USCALED,
        VK_FORMAT_R8_SSCALED,
        VK_FORMAT_R8_UINT,
        VK_FORMAT_R8_SINT,
        VK_FORMAT_R8_SRGB,
        VK_FORMAT_R8G8_UNORM,
        VK_FORMAT_R8G8_SNORM,
        VK_FORMAT_R8G8_USCALED,
        VK_FORMAT_R8G8_SSCALED,
        VK_FORMAT_R8G8_UINT,
        VK_FORMAT_R8G8_SINT,
        VK_FORMAT_R8G8_SRGB,
        VK_FORMAT_R8G8B8_UNORM,
        VK_FORMAT_R8G8B8_SNORM,
        VK_FORMAT_R8G8B8_USCALED,
        VK_FORMAT_R8G8B8_SSCALED,
        VK_FORMAT_R8G8B8_UINT,
        VK_FORMAT_R8G8B8_SINT,
        VK_FORMAT_R8G8B8_SRGB,
        VK_FORMAT_B8G8R8_UNORM,
        VK_FORMAT_B8G8R8_SNORM,
        VK_FORMAT_B8G8R8_USCALED,
        VK_FORMAT_B8G8R8_SSCALED,
        VK_FORMAT_B8G8R8_UINT,
        VK_FORMAT_B8G8R8_SINT,
        VK_FORMAT_B8G8R8_SRGB,
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_FORMAT_R8G8B8A8_SNORM,
        VK_FORMAT_R8G8B8A8_USCALED,
        VK_FORMAT_R8G8B8A8_SSCALED,
        VK_FORMAT_R8G8B8A8_UINT,
        VK_FORMAT_R8G8B8A8_SINT,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_FORMAT_B8G8R8A8_UNORM,
        VK_FORMAT_B8G8R8A8_SNORM,
        VK_FORMAT_B8G8R8A8_USCALED,
        VK_FORMAT_B8G8R8A8_SSCALED,
        VK_FORMAT_B8G8R8A8_UINT,
        VK_FORMAT_B8G8R8A8_SINT,
        VK_FORMAT_B8G8R8A8_SRGB,
        VK_FORMAT_A8B8G8R8_UNORM_PACK32,
        VK_FORMAT_A8B8G8R8_SNORM_PACK32,
        VK_FORMAT_A8B8G8R8_USCALED_PACK32,
        VK_FORMAT_A8B8G8R8_SSCALED_PACK32,
        VK_FORMAT_A8B8G8R8_UINT_PACK32,
        VK_FORMAT_A8B8G8R8_SINT_PACK32,
        VK_FORMAT_A8B8G8R8_SRGB_PACK32,
        VK_FORMAT_A2R10G10B10_UNORM_PACK32,
        VK_FORMAT_A2R10G10B10_SNORM_PACK32,
        VK_FORMAT_A2R10G10B10_USCALED_PACK32,
        VK_FORMAT_A2R10G10B10_SSCALED_PACK32,
        VK_FORMAT_A2R10G10B10_UINT_PACK32,
        VK_FORMAT_A2R10G10B10_SINT_PACK32,
        VK_FORMAT_A2B10G10R10_UNORM_PACK32,
        VK_FORMAT_A2B10G10R10_SNORM_PACK32,
        VK_FORMAT_A2B10G10R10_USCALED_PACK32,
        VK_FORMAT_A2B10G10R10_SSCALED_PACK32,
        VK_FORMAT_A2B10G10R10_UINT_PACK32,
        VK_FORMAT_A2B10G10R10_SINT_PACK32,
        VK_FORMAT_R16_UNORM,
        VK_FORMAT_R16_SNORM,
        VK_FORMAT_R16_USCALED,
        VK_FORMAT_R16_SSCALED,
        VK_FORMAT_R16_UINT,
        VK_FORMAT_R16_SINT,
        VK_FORMAT_R16_SFLOAT,
        VK_FORMAT_R16G16_UNORM,
        VK_FORMAT_R16G16_SNORM,
        VK_FORMAT_R16G16_USCALED,
        VK_FORMAT_R16G16_SSCALED,
        VK_FORMAT_R16G16_UINT,
        VK_FORMAT_R16G16_SINT,
        VK_FORMAT_R16G16_SFLOAT,
        VK_FORMAT_R16G16B16_UNORM,
        VK_FORMAT_R16G16B16_SNORM,
        VK_FORMAT_R16G16B16_USCALED,
        VK_FORMAT_R16G16B16_SSCALED,
        VK_FORMAT_R16G16B16_UINT,
        VK_FORMAT_R16G16B16_SINT,
        VK_FORMAT_R16G16B16_SFLOAT,
        VK_FORMAT_R16G16B16A16_UNORM,
        VK_FORMAT_R16G16B16A16_SNORM,
        VK_FORMAT_R16G16B16A16_USCALED,
        VK_FORMAT_R16G16B16A16_SSCALED,
        VK_FORMAT_R16G16B16A16_UINT,
        VK_FORMAT_R16G16B16A16_SINT,
        VK_FORMAT_R16G16B16A16_SFLOAT,
        VK_FORMAT_R32_UINT,
        VK_FORMAT_R32_SINT,
        VK_FORMAT_R32_SFLOAT,
        VK_FORMAT_R32G32_UINT,
        VK_FORMAT_R32G32_SINT,
        VK_FORMAT_R32G32_SFLOAT,
        VK_FORMAT_R32G32B32_UINT,
        VK_FORMAT_R32G32B32_SINT,
        VK_FORMAT_R32G32B32_SFLOAT,
        VK_FORMAT_R32G32B32A32_UINT,
        VK_FORMAT_R32G32B32A32_SINT,
        VK_FORMAT_R32G32B32A32_SFLOAT,
        VK_FORMAT_R64_UINT,
        VK_FORMAT_R64_SINT,
        VK_FORMAT_R64_SFLOAT,
        VK_FORMAT_R64G64_UINT,
        VK_FORMAT_R64G64_SINT,
        VK_FORMAT_R64G64_SFLOAT,
        VK_FORMAT_R64G64B64_UINT,
        VK_FORMAT_R64G64B64_SINT,
        VK_FORMAT_R64G64B64_SFLOAT,
        VK_FORMAT_R64G64B64A64_UINT,
        VK_FORMAT_R64G64B64A64_SINT,
        VK_FORMAT_R64G64B64A64_SFLOAT,
        VK_FORMAT_B10G11R11_UFLOAT_PACK32,
        VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,
        VK_FORMAT_D16_UNORM,
        VK_FORMAT_X8_D24_UNORM_PACK32,
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_BC1_RGB_UNORM_BLOCK,
        VK_FORMAT_BC1_RGB_SRGB_BLOCK,
        VK_FORMAT_BC1_RGBA_UNORM_BLOCK,
        VK_FORMAT_BC1_RGBA_SRGB_BLOCK,
        VK_FORMAT_BC2_UNORM_BLOCK,
        VK_FORMAT_BC2_SRGB_BLOCK,
        VK_FORMAT_BC3_UNORM_BLOCK,
        VK_FORMAT_BC3_SRGB_BLOCK,
        VK_FORMAT_BC4_UNORM_BLOCK,
        VK_FORMAT_BC4_SNORM_BLOCK,
        VK_FORMAT_BC5_UNORM_BLOCK,
        VK_FORMAT_BC5_SNORM_BLOCK,
        VK_FORMAT_BC6H_UFLOAT_BLOCK,
        VK_FORMAT_BC6H_SFLOAT_BLOCK,
        VK_FORMAT_BC7_UNORM_BLOCK,
        VK_FORMAT_BC7_SRGB_BLOCK,
        VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK,
        VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK,
        VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK,
        VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK,
        VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK,
        VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK,
        VK_FORMAT_EAC_R11_UNORM_BLOCK,
        VK_FORMAT_EAC_R11_SNORM_BLOCK,
        VK_FORMAT_EAC_R11G11_UNORM_BLOCK,
        VK_FORMAT_EAC_R11G11_SNORM_BLOCK,
        VK_FORMAT_ASTC_4x4_UNORM_BLOCK,
        VK_FORMAT_ASTC_4x4_SRGB_BLOCK,
        VK_FORMAT_ASTC_5x4_UNORM_BLOCK,
        VK_FORMAT_ASTC_5x4_SRGB_BLOCK,
        VK_FORMAT_ASTC_5x5_UNORM_BLOCK,
        VK_FORMAT_ASTC_5x5_SRGB_BLOCK,
        VK_FORMAT_ASTC_6x5_UNORM_BLOCK,
        VK_FORMAT_ASTC_6x5_SRGB_BLOCK,
        VK_FORMAT_ASTC_6x6_UNORM_BLOCK,
        VK_FORMAT_ASTC_6x6_SRGB_BLOCK,
        VK_FORMAT_ASTC_8x5_UNORM_BLOCK,
        VK_FORMAT_ASTC_8x5_SRGB_BLOCK,
        VK_FORMAT_ASTC_8x6_UNORM_BLOCK,
        VK_FORMAT_ASTC_8x6_SRGB_BLOCK,
        VK_FORMAT_ASTC_8x8_UNORM_BLOCK,
        VK_FORMAT_ASTC_8x8_SRGB_BLOCK,
        VK_FORMAT_ASTC_10x5_UNORM_BLOCK,
        VK_FORMAT_ASTC_10x5_SRGB_BLOCK,
        VK_FORMAT_ASTC_10x6_UNORM_BLOCK,
        VK_FORMAT_ASTC_10x6_SRGB_BLOCK,
        VK_FORMAT_ASTC_10x8_UNORM_BLOCK,
        VK_FORMAT_ASTC_10x8_SRGB_BLOCK,
        VK_FORMAT_ASTC_10x10_UNORM_BLOCK,
        VK_FORMAT_ASTC_10x10_SRGB_BLOCK,
        VK_FORMAT_ASTC_12x10_UNORM_BLOCK,
        VK_FORMAT_ASTC_12x10_SRGB_BLOCK,
        VK_FORMAT_ASTC_12x12_UNORM_BLOCK,
        VK_FORMAT_ASTC_12x12_SRGB_BLOCK};
    static_assert(COMMON_FORMAT_RANGE_SIZE_PROTECTED == (sizeof(common_to_vulkan_format_map) / sizeof(common_to_vulkan_format_map[0])), "COMMON_FORMAT_RANGE_SIZE_PROTECTED == (sizeof(common_to_vulkan_format_map) / sizeof(common_to_vulkan_format_map[0]))");

    assert(common_format < (sizeof(common_to_vulkan_format_map) / sizeof(common_to_vulkan_format_map[0])));
    return common_to_vulkan_format_map[common_format];
}

uint32_t gfx_texture_vk::get_format_aspect_count(VkFormat vk_format)
{
    //--------------------------------------------------------------------------------------

    // gFormatInfoTable libANGLE/renderer/Format_table_autogen.cpp
    // { FormatID::R8G8B8A8_UNORM, GL_RGBA8, GL_RGBA8, GenerateMip<R8G8B8A8>, NoCopyFunctions, ReadColor<R8G8B8A8, GLfloat>, WriteColor<R8G8B8A8, GLfloat>, GL_UNSIGNED_NORMALIZED, 8, 8, 8, 8, 0, 0, 0, 4, 0 */ {false */ {false */ {false, gl::VertexAttribType::UnsignedByte },

    // BuildInternalFormatInfoMap libANGLE/formatutils.cpp
    // From ES 3.0.1 spec, table 3.12
    //                    | Internal format     |sized| R | G | B | A |S | Format         | Type                             | Component type        | SRGB | Texture supported                                | Filterable     | Texture attachment                               | Renderbuffer                                   | Blend
    // AddRGBAFormat(&map, GL_RGBA8,             true,  8,  8,  8,  8, 0, GL_RGBA,         GL_UNSIGNED_BYTE,                  GL_UNSIGNED_NORMALIZED */ {false, RequireESOrExt<3, 0, &Extensions::textureStorage>, AlwaysSupported, RequireESOrExt<3, 0, &Extensions::textureStorage>, RequireESOrExt<3, 0, &Extensions::rgb8rgba8OES>, RequireESOrExt<3, 0, &Extensions::rgb8rgba8OES>);
    // From EXT_texture_compression_bptc
    //                          | Internal format                         | W | H |D | BS |CC| SRGB | Texture supported                              | Filterable     | Texture attachment | Renderbuffer  | Blend
    // AddCompressedFormat(&map, GL_COMPRESSED_RGBA_BPTC_UNORM_EXT,         4,  4, 1, 128, 4 */ {false, RequireExt<&Extensions::textureCompressionBPTC>, AlwaysSupported, NeverSupported,      NeverSupported, NeverSupported);

    // GetLoadFunctionsLoadFunctionsMap libANGLE/renderer/load_functions_table_autogen.cpp
    // LoadToNative
    // LoadCompressedToNative

    struct vulkan_format_info_t
    {
        uint32_t _union_tag;

        union
        {
            struct
            {
                uint32_t pixelBytes;
            } rgba;
            struct
            {
                uint32_t depthBytes;
                uint32_t stencilBytes;
            } depthstencil;
            struct
            {
                uint32_t compressedBlockWidth;
                uint32_t compressedBlockHeight;
                uint32_t compressedBlockDepth;
                uint32_t compressedBlockSizeInBytes;
            } compressed;
        };
    };

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc99-designator"
    static struct vulkan_format_info_t const vulkan_format_info_table[] = {
        {0},                                       //VK_FORMAT_UNDEFINED
        {1, .rgba = {1}},                          //VK_FORMAT_R4G4_UNORM_PACK8
        {1, .rgba = {2}},                          //VK_FORMAT_R4G4B4A4_UNORM_PACK16
        {1, .rgba = {2}},                          //VK_FORMAT_B4G4R4A4_UNORM_PACK16
        {1, .rgba = {2}},                          //VK_FORMAT_R5G6B5_UNORM_PACK16
        {1, .rgba = {2}},                          //VK_FORMAT_B5G6R5_UNORM_PACK16
        {1, .rgba = {2}},                          //VK_FORMAT_R5G5B5A1_UNORM_PACK16
        {1, .rgba = {2}},                          //VK_FORMAT_B5G5R5A1_UNORM_PACK16
        {1, .rgba = {2}},                          //VK_FORMAT_A1R5G5B5_UNORM_PACK16
        {1, .rgba = {1}},                          //VK_FORMAT_R8_UNORM
        {1, .rgba = {1}},                          //VK_FORMAT_R8_SNORM
        {1, .rgba = {1}},                          //VK_FORMAT_R8_USCALED
        {1, .rgba = {1}},                          //VK_FORMAT_R8_SSCALED
        {1, .rgba = {1}},                          //VK_FORMAT_R8_UINT
        {1, .rgba = {1}},                          //VK_FORMAT_R8_SINT
        {1, .rgba = {1}},                          //VK_FORMAT_R8_SRGB
        {1, .rgba = {2}},                          //VK_FORMAT_R8G8_UNORM
        {1, .rgba = {2}},                          //VK_FORMAT_R8G8_SNORM
        {1, .rgba = {2}},                          //VK_FORMAT_R8G8_USCALED
        {1, .rgba = {2}},                          //VK_FORMAT_R8G8_SSCALED
        {1, .rgba = {2}},                          //VK_FORMAT_R8G8_UINT
        {1, .rgba = {2}},                          //VK_FORMAT_R8G8_SINT
        {1, .rgba = {2}},                          //VK_FORMAT_R8G8_SRGB
        {1, .rgba = {3}},                          //VK_FORMAT_R8G8B8_UNORM
        {1, .rgba = {3}},                          //VK_FORMAT_R8G8B8_SNORM
        {1, .rgba = {3}},                          //VK_FORMAT_R8G8B8_USCALED
        {1, .rgba = {3}},                          //VK_FORMAT_R8G8B8_SSCALED
        {1, .rgba = {3}},                          //VK_FORMAT_R8G8B8_UINT
        {1, .rgba = {3}},                          //VK_FORMAT_R8G8B8_SINT
        {1, .rgba = {3}},                          //VK_FORMAT_R8G8B8_SRGB
        {1, .rgba = {3}},                          //VK_FORMAT_B8G8R8_UNORM
        {1, .rgba = {3}},                          //VK_FORMAT_B8G8R8_SNORM
        {1, .rgba = {3}},                          //VK_FORMAT_B8G8R8_USCALED
        {1, .rgba = {3}},                          //VK_FORMAT_B8G8R8_SSCALED
        {1, .rgba = {3}},                          //VK_FORMAT_B8G8R8_UINT
        {1, .rgba = {3}},                          //VK_FORMAT_B8G8R8_SINT
        {1, .rgba = {3}},                          //VK_FORMAT_B8G8R8_SRGB
        {1, .rgba = {4}},                          //VK_FORMAT_R8G8B8A8_UNORM
        {1, .rgba = {4}},                          //VK_FORMAT_R8G8B8A8_SNORM
        {1, .rgba = {4}},                          //VK_FORMAT_R8G8B8A8_USCALED
        {1, .rgba = {4}},                          //VK_FORMAT_R8G8B8A8_SSCALED
        {1, .rgba = {4}},                          //VK_FORMAT_R8G8B8A8_UINT
        {1, .rgba = {4}},                          //VK_FORMAT_R8G8B8A8_SINT
        {1, .rgba = {4}},                          //VK_FORMAT_R8G8B8A8_SRGB
        {1, .rgba = {4}},                          //VK_FORMAT_B8G8R8A8_UNORM
        {1, .rgba = {4}},                          //VK_FORMAT_B8G8R8A8_SNORM
        {1, .rgba = {4}},                          //VK_FORMAT_B8G8R8A8_USCALED
        {1, .rgba = {4}},                          //VK_FORMAT_B8G8R8A8_SSCALED
        {1, .rgba = {4}},                          //VK_FORMAT_B8G8R8A8_UINT
        {1, .rgba = {4}},                          //VK_FORMAT_B8G8R8A8_SINT
        {1, .rgba = {4}},                          //VK_FORMAT_B8G8R8A8_SRGB
        {1, .rgba = {4}},                          //VK_FORMAT_A8B8G8R8_UNORM_PACK32
        {1, .rgba = {4}},                          //VK_FORMAT_A8B8G8R8_SNORM_PACK32
        {1, .rgba = {4}},                          //VK_FORMAT_A8B8G8R8_USCALED_PACK32
        {1, .rgba = {4}},                          //VK_FORMAT_A8B8G8R8_SSCALED_PACK32
        {1, .rgba = {4}},                          //VK_FORMAT_A8B8G8R8_UINT_PACK32
        {1, .rgba = {4}},                          //VK_FORMAT_A8B8G8R8_SINT_PACK32
        {1, .rgba = {4}},                          //VK_FORMAT_A8B8G8R8_SRGB_PACK32
        {1, .rgba = {4}},                          //VK_FORMAT_A2R10G10B10_UNORM_PACK32
        {1, .rgba = {4}},                          //VK_FORMAT_A2R10G10B10_SNORM_PACK32
        {1, .rgba = {4}},                          //VK_FORMAT_A2R10G10B10_USCALED_PACK32
        {1, .rgba = {4}},                          //VK_FORMAT_A2R10G10B10_SSCALED_PACK32
        {1, .rgba = {4}},                          //VK_FORMAT_A2R10G10B10_UINT_PACK32
        {1, .rgba = {4}},                          //VK_FORMAT_A2R10G10B10_SINT_PACK32
        {1, .rgba = {4}},                          //VK_FORMAT_A2B10G10R10_UNORM_PACK32
        {1, .rgba = {4}},                          //VK_FORMAT_A2B10G10R10_SNORM_PACK32
        {1, .rgba = {4}},                          //VK_FORMAT_A2B10G10R10_USCALED_PACK32
        {1, .rgba = {4}},                          //VK_FORMAT_A2B10G10R10_SSCALED_PACK32
        {1, .rgba = {4}},                          //VK_FORMAT_A2B10G10R10_UINT_PACK32
        {1, .rgba = {4}},                          //VK_FORMAT_A2B10G10R10_SINT_PACK32
        {1, .rgba = {2}},                          //VK_FORMAT_R16_UNORM
        {1, .rgba = {2}},                          //VK_FORMAT_R16_SNORM
        {1, .rgba = {2}},                          //VK_FORMAT_R16_USCALED
        {1, .rgba = {2}},                          //VK_FORMAT_R16_SSCALED
        {1, .rgba = {2}},                          //VK_FORMAT_R16_UINT
        {1, .rgba = {2}},                          //VK_FORMAT_R16_SINT
        {1, .rgba = {2}},                          //VK_FORMAT_R16_SFLOAT
        {1, .rgba = {4}},                          //VK_FORMAT_R16G16_UNORM
        {1, .rgba = {4}},                          //VK_FORMAT_R16G16_SNORM
        {1, .rgba = {4}},                          //VK_FORMAT_R16G16_USCALED
        {1, .rgba = {4}},                          //VK_FORMAT_R16G16_SSCALED
        {1, .rgba = {4}},                          //VK_FORMAT_R16G16_UINT
        {1, .rgba = {4}},                          //VK_FORMAT_R16G16_SINT
        {1, .rgba = {4}},                          //VK_FORMAT_R16G16_SFLOAT
        {1, .rgba = {6}},                          //VK_FORMAT_R16G16B16_UNORM
        {1, .rgba = {6}},                          //VK_FORMAT_R16G16B16_SNORM
        {1, .rgba = {6}},                          //VK_FORMAT_R16G16B16_USCALED
        {1, .rgba = {6}},                          //VK_FORMAT_R16G16B16_SSCALED
        {1, .rgba = {6}},                          //VK_FORMAT_R16G16B16_UINT
        {1, .rgba = {6}},                          //VK_FORMAT_R16G16B16_SINT
        {1, .rgba = {6}},                          //VK_FORMAT_R16G16B16_SFLOAT
        {1, .rgba = {8}},                          //VK_FORMAT_R16G16B16A16_UNORM
        {1, .rgba = {8}},                          //VK_FORMAT_R16G16B16A16_SNORM
        {1, .rgba = {8}},                          //VK_FORMAT_R16G16B16A16_USCALED
        {1, .rgba = {8}},                          //VK_FORMAT_R16G16B16A16_SSCALED
        {1, .rgba = {8}},                          //VK_FORMAT_R16G16B16A16_UINT
        {1, .rgba = {8}},                          //VK_FORMAT_R16G16B16A16_SINT
        {1, .rgba = {8}},                          //VK_FORMAT_R16G16B16A16_SFLOAT
        {1, .rgba = {4}},                          //VK_FORMAT_R32_UINT
        {1, .rgba = {4}},                          //VK_FORMAT_R32_SINT
        {1, .rgba = {4}},                          //VK_FORMAT_R32_SFLOAT
        {1, .rgba = {8}},                          //VK_FORMAT_R32G32_UINT
        {1, .rgba = {8}},                          //VK_FORMAT_R32G32_SINT
        {1, .rgba = {8}},                          //VK_FORMAT_R32G32_SFLOAT
        {1, .rgba = {12}},                         //VK_FORMAT_R32G32B32_UINT
        {1, .rgba = {12}},                         //VK_FORMAT_R32G32B32_SINT
        {1, .rgba = {12}},                         //VK_FORMAT_R32G32B32_SFLOAT
        {1, .rgba = {16}},                         //VK_FORMAT_R32G32B32A32_UINT
        {1, .rgba = {16}},                         //VK_FORMAT_R32G32B32A32_SINT
        {1, .rgba = {16}},                         //VK_FORMAT_R32G32B32A32_SFLOAT
        {1, .rgba = {8}},                          //VK_FORMAT_R64_UINT
        {1, .rgba = {8}},                          //VK_FORMAT_R64_SINT
        {1, .rgba = {8}},                          //VK_FORMAT_R64_SFLOAT
        {1, .rgba = {16}},                         //VK_FORMAT_R64G64_UINT
        {1, .rgba = {16}},                         //VK_FORMAT_R64G64_SINT
        {1, .rgba = {16}},                         //VK_FORMAT_R64G64_SFLOAT
        {1, .rgba = {24}},                         //VK_FORMAT_R64G64B64_UINT
        {1, .rgba = {24}},                         //VK_FORMAT_R64G64B64_SINT
        {1, .rgba = {24}},                         //VK_FORMAT_R64G64B64_SFLOAT
        {1, .rgba = {32}},                         //VK_FORMAT_R64G64B64A64_UINT
        {1, .rgba = {32}},                         //VK_FORMAT_R64G64B64A64_SINT
        {1, .rgba = {32}},                         //VK_FORMAT_R64G64B64A64_SFLOAT
        {1, .rgba = {4}},                          //VK_FORMAT_B10G11R11_UFLOAT_PACK32
        {1, .rgba = {4}},                          //VK_FORMAT_E5B9G9R9_UFLOAT_PACK32
        {2, .depthstencil = {2, 0}},               //VK_FORMAT_D16_UNORM
        {2, .depthstencil = {4, 0}},               //VK_FORMAT_X8_D24_UNORM_PACK32
        {2, .depthstencil = {4, 0}},               //VK_FORMAT_D32_SFLOAT
        {2, .depthstencil = {0, 1}},               //VK_FORMAT_S8_UINT
        {2, .depthstencil = {2, 2}},               //VK_FORMAT_D16_UNORM_S8_UINT         //data copied to or from the depth aspect of a VK_FORMAT_D16_UNORM or VK_FORMAT_D16_UNORM_S8_UINT format is tightly packed with one VK_FORMAT_D16_UNORM value per texel.
        {2, .depthstencil = {4, 4}},               //VK_FORMAT_D24_UNORM_S8_UINT         //data copied to or from the depth aspect of a VK_FORMAT_X8_D24_UNORM_PACK32 or VK_FORMAT_D24_UNORM_S8_UINT format is packed with one 32-bit word per texel with the D24 value in the LSBs of the word, and undefined values in the eight MSBs.
        {2, .depthstencil = {4, 4}},               //VK_FORMAT_D32_SFLOAT_S8_UINT        //data copied to or from the depth aspect of a VK_FORMAT_D32_SFLOAT or VK_FORMAT_D32_SFLOAT_S8_UINT format is tightly packed with one VK_FORMAT_D32_SFLOAT value per texel.
        {3, .compressed = {4, 4, 1, (64 / 8)}},    //VK_FORMAT_BC1_RGB_UNORM_BLOCK       //GL_COMPRESSED_RGB_S3TC_DXT1_EXT           //R5G6B5_UNORM
        {3, .compressed = {4, 4, 1, (64 / 8)}},    //VK_FORMAT_BC1_RGB_SRGB_BLOCK
        {3, .compressed = {4, 4, 1, (64 / 8)}},    //VK_FORMAT_BC1_RGBA_UNORM_BLOCK      //GL_COMPRESSED_RGBA_S3TC_DXT1_EXT          //R5G6B5A1_UNORM
        {3, .compressed = {4, 4, 1, (64 / 8)}},    //VK_FORMAT_BC1_RGBA_SRGB_BLOCK
        {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_BC2_UNORM_BLOCK           //GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE        //R5G6B5A4_UNORM
        {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_BC2_SRGB_BLOCK
        {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_BC3_UNORM_BLOCK           //GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE        //R5G6B5A8_UNORM
        {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_BC3_SRGB_BLOCK
        {3, .compressed = {4, 4, 1, (64 / 8)}},    //VK_FORMAT_BC4_UNORM_BLOCK           //GL_COMPRESSED_RED_RGTC1_EXT               //R8_UNORM
        {3, .compressed = {4, 4, 1, (64 / 8)}},    //VK_FORMAT_BC4_SNORM_BLOCK           //GL_COMPRESSED_SIGNED_RED_RGTC1_EXT        //R8_SNORM
        {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_BC5_UNORM_BLOCK           //GL_COMPRESSED_RED_GREEN_RGTC2_EXT         //R8G8_UNORM
        {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_BC5_SNORM_BLOCK           //GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT  //R8G8_SNORM
        {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_BC6H_UFLOAT_BLOCK         //GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT     //R16G16B16_UFLOAT (HDR)
        {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_BC6H_SFLOAT_BLOCK         //GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT       //R16G16B16_SFLOAT (HDR)
        {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_BC7_UNORM_BLOCK           //GL_COMPRESSED_RGBA_BPTC_UNORM_EXT         //B7G7R7A8_UNORM
        {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_BC7_SRGB_BLOCK            //GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_EXT   //B7G7R7A8_UNORM
        {3, .compressed = {4, 4, 1, (64 / 8)}},    //VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK   //GL_COMPRESSED_RGB8_ETC2
        {3, .compressed = {4, 4, 1, (64 / 8)}},    //VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK    //GL_COMPRESSED_SRGB8_ETC2
        {3, .compressed = {4, 4, 1, (64 / 8)}},    //VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK //GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2
        {3, .compressed = {4, 4, 1, (64 / 8)}},    //VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK  //GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2
        {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK //GL_COMPRESSED_RGBA8_ETC2_EAC
        {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK  //GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC
        {3, .compressed = {4, 4, 1, (64 / 8)}},    //VK_FORMAT_EAC_R11_UNORM_BLOCK       //GL_COMPRESSED_R11_EAC
        {3, .compressed = {4, 4, 1, (64 / 8)}},    //VK_FORMAT_EAC_R11_SNORM_BLOCK       //GL_COMPRESSED_SIGNED_R11_EAC
        {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_EAC_R11G11_UNORM_BLOCK    //GL_COMPRESSED_RG11_EAC
        {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_EAC_R11G11_SNORM_BLOCK    //GL_COMPRESSED_SIGNED_RG11_EAC
        {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_ASTC_4x4_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_4x4_KHR             //VK_EXT_astc_decode_mode
        {3, .compressed = {4, 4, 1, (128 / 8)}},   //VK_FORMAT_ASTC_4x4_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR     //VK_EXT_texture_compression_astc_hdr
        {3, .compressed = {5, 4, 1, (128 / 8)}},   //VK_FORMAT_ASTC_5x4_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_5x4_KHR             //
        {3, .compressed = {5, 4, 1, (128 / 8)}},   //VK_FORMAT_ASTC_5x4_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR     //
        {3, .compressed = {5, 5, 1, (128 / 8)}},   //VK_FORMAT_ASTC_5x5_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_5x5_KHR             //
        {3, .compressed = {5, 5, 1, (128 / 8)}},   //VK_FORMAT_ASTC_5x5_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR     //
        {3, .compressed = {6, 5, 1, (128 / 8)}},   //VK_FORMAT_ASTC_6x5_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_6x5_KHR             //
        {3, .compressed = {6, 5, 1, (128 / 8)}},   //VK_FORMAT_ASTC_6x5_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR     //
        {3, .compressed = {6, 6, 1, (128 / 8)}},   //VK_FORMAT_ASTC_6x6_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_6x6_KHR             //
        {3, .compressed = {6, 6, 1, (128 / 8)}},   //VK_FORMAT_ASTC_6x6_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR     //
        {3, .compressed = {8, 5, 1, (128 / 8)}},   //VK_FORMAT_ASTC_8x5_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_8x5_KHR             //
        {3, .compressed = {8, 5, 1, (128 / 8)}},   //VK_FORMAT_ASTC_8x5_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR     //
        {3, .compressed = {8, 6, 1, (128 / 8)}},   //VK_FORMAT_ASTC_8x6_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_8x6_KHR             //
        {3, .compressed = {8, 6, 1, (128 / 8)}},   //VK_FORMAT_ASTC_8x6_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR     //
        {3, .compressed = {8, 8, 1, (128 / 8)}},   //VK_FORMAT_ASTC_8x8_UNORM_BLOCK      //GL_COMPRESSED_RGBA_ASTC_8x8_KHR             //
        {3, .compressed = {8, 8, 1, (128 / 8)}},   //VK_FORMAT_ASTC_8x8_SRGB_BLOCK       //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR     //
        {3, .compressed = {10, 5, 1, (128 / 8)}},  //VK_FORMAT_ASTC_10x5_UNORM_BLOCK     //GL_COMPRESSED_RGBA_ASTC_10x5_KHR            //
        {3, .compressed = {10, 5, 1, (128 / 8)}},  //VK_FORMAT_ASTC_10x5_SRGB_BLOCK      //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR    //
        {3, .compressed = {10, 6, 1, (128 / 8)}},  //VK_FORMAT_ASTC_10x6_UNORM_BLOCK     //GL_COMPRESSED_RGBA_ASTC_10x6_KHR            //
        {3, .compressed = {10, 6, 1, (128 / 8)}},  //VK_FORMAT_ASTC_10x6_SRGB_BLOCK      //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR    //
        {3, .compressed = {10, 8, 1, (128 / 8)}},  //VK_FORMAT_ASTC_10x8_UNORM_BLOCK     //GL_COMPRESSED_RGBA_ASTC_10x8_KHR            //
        {3, .compressed = {10, 8, 1, (128 / 8)}},  //VK_FORMAT_ASTC_10x8_SRGB_BLOCK      //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR    //
        {3, .compressed = {10, 10, 1, (128 / 8)}}, //VK_FORMAT_ASTC_10x10_UNORM_BLOCK    //GL_COMPRESSED_RGBA_ASTC_10x10_KHR           //
        {3, .compressed = {10, 10, 1, (128 / 8)}}, //VK_FORMAT_ASTC_10x10_SRGB_BLOCK     //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR   //
        {3, .compressed = {12, 10, 1, (128 / 8)}}, //VK_FORMAT_ASTC_12x10_UNORM_BLOCK    //GL_COMPRESSED_RGBA_ASTC_12x10_KHR           //
        {3, .compressed = {12, 10, 1, (128 / 8)}}, //VK_FORMAT_ASTC_12x10_SRGB_BLOCK     //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR   //
        {3, .compressed = {12, 12, 1, (128 / 8)}}, //VK_FORMAT_ASTC_12x12_UNORM_BLOCK    //GL_COMPRESSED_RGBA_ASTC_12x12_KHR           //
        {3, .compressed = {12, 12, 1, (128 / 8)}}  //VK_FORMAT_ASTC_12x12_SRGB_BLOCK     //GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR   //
    };
#pragma GCC diagnostic pop

    static_assert((VK_FORMAT_ASTC_12x12_SRGB_BLOCK - VK_FORMAT_UNDEFINED + 1) == (sizeof(vulkan_format_info_table) / sizeof(vulkan_format_info_table[0])), "(VK_FORMAT_ASTC_12x12_SRGB_BLOCK - VK_FORMAT_UNDEFINED + 1) == (sizeof(vulkan_format_info_table) / sizeof(vulkan_format_info_table[0]))");

    assert(vk_format < (sizeof(vulkan_format_info_table) / sizeof(vulkan_format_info_table[0])));

    struct vulkan_format_info_t vk_format_info = vulkan_format_info_table[vk_format];

    assert(vk_format_info._union_tag == 3 || vk_format_info._union_tag == 1 || vk_format_info._union_tag == 2);
    if (vk_format_info._union_tag == 3 || vk_format_info._union_tag == 1)
    {
        return 1;
    }
    else if (vk_format_info._union_tag == 2)
    {
        if (vk_format_info.depthstencil.depthBytes == 0 || vk_format_info.depthstencil.stencilBytes == 0)
        {
            return 1;
        }
        else
        {
            return 2;
        }
    }
    else
    {
        return 0;
    }
}
