#include "TextureLoader_DDS.h"

#include <assert.h>

static inline enum VkImageType _GetVulkanType(uint32_t neutraltype);

static inline enum VkFormat _GetVulkanFormat(uint32_t neutralformat);

struct TextureLoader_SpecificHeader TextureLoader_ToSpecificHeader(struct TextureLoader_NeutralHeader const *neutral_texture_header)
{
    struct TextureLoader_SpecificHeader specific_texture_header;

    specific_texture_header.isCubeCompatible = neutral_texture_header->isCubeMap;
    specific_texture_header.imageType = _GetVulkanType(neutral_texture_header->type);
    specific_texture_header.format = _GetVulkanFormat(neutral_texture_header->format);
    specific_texture_header.extent.width = neutral_texture_header->width;
    specific_texture_header.extent.height = neutral_texture_header->height;
    specific_texture_header.extent.depth = neutral_texture_header->depth;
    specific_texture_header.mipLevels = neutral_texture_header->mipLevels;
    specific_texture_header.arrayLayers = neutral_texture_header->arrayLayers;

    return specific_texture_header;
}

static inline uint32_t _GetFormatAspectCount(VkFormat vkformat);

static inline uint32_t _GetFormatAspectMask(VkFormat vkformat, uint32_t aspect);

static inline bool _IsFormatCompressed(VkFormat vkformat);

static inline uint32_t _ComputeCompressedFormatImageSize(VkFormat vkformat, uint32_t width, uint32_t height, uint32_t depth);

static inline uint32_t _GetCompressedFormatBlockWidth(VkFormat vkformat);

static inline uint32_t _GetCompressedFormatBlockHeight(VkFormat vkformat);

template <typename T, typename T2>
static inline T roundUp(const T value, const T2 alignment)
{
    auto temp = value + alignment - static_cast<T>(1);
    return temp - temp % alignment;
}

//optimalBufferCopyOffsetAlignment
//optimalBufferCopyRowPitchAlignment

size_t TextureLoader_GetCopyableFootprints(struct TextureLoader_SpecificHeader const *vk_texture_header,
                                           VkDeviceSize optimalBufferCopyOffsetAlignment, VkDeviceSize optimalBufferCopyRowPitchAlignment,
                                           size_t NumSubresources, struct TextureLoader_MemcpyDest *pDest, VkBufferImageCopy *pRegions)
{
    // Context::texSubImage2D libANGLE/Context.cpp
    // Texture::setSubImage libANGLE/Texture.cpp
    // TextureVk::setSubImage libANGLE/renderer/vulkan/TextureVk.cpp
    // TextureVk::setSubImageImpl libANGLE/renderer/vulkan/TextureVk.cpp
    // ImageHelper::stageSubresourceUpdate libANGLE/renderer/vulkan/vk_helpers.cpp
    // ImageHelper::CalculateBufferInfo libANGLE/renderer/vulkan/vk_helpers.cpp
    // ImageHelper::stageSubresourceUpdateImpl libANGLE/renderer/vulkan/vk_helpers.cpp

    uint32_t aspectCount = _GetFormatAspectCount(vk_texture_header->format);

    size_t stagingOffset = 0;
    size_t TotalBytes = 0;
    size_t DstSubresource = 0;

    for (uint32_t aspect = 0; aspect < aspectCount; ++aspect)
    {
        for (uint32_t mipLevel = 0; mipLevel < vk_texture_header->mipLevels; ++mipLevel)
        {
            size_t w = vk_texture_header->extent.width;
            size_t h = vk_texture_header->extent.height;
            size_t d = vk_texture_header->extent.depth;
            for (uint32_t arrayLayer = 0; arrayLayer < vk_texture_header->arrayLayers; ++arrayLayer)
            {
                size_t outputRowPitch;
                size_t outputRowSize;
                size_t outputNumRows;
                size_t outputSlicePitch;
                size_t outputNumSlices;

                uint32_t bufferRowLength;
                uint32_t bufferImageHeight;

                size_t allocationSize;

                if (_IsFormatCompressed(vk_texture_header->format))
                {
                    outputRowSize = _ComputeCompressedFormatImageSize(vk_texture_header->format, w, 1, 1);
                    size_t outputSliceSize = _ComputeCompressedFormatImageSize(vk_texture_header->format, w, h, 1);

                    outputNumRows = outputSliceSize / outputRowSize;
                    outputNumSlices = _ComputeCompressedFormatImageSize(vk_texture_header->format, w, h, d) / outputSliceSize;

                    outputRowPitch = roundUp(outputRowSize, optimalBufferCopyRowPitchAlignment);
                    outputSlicePitch = outputRowPitch * outputNumRows;

                    bufferRowLength = roundUp(vk_texture_header->extent.width, _GetCompressedFormatBlockWidth(vk_texture_header->format)); //support optimalBufferCopyRowPitchAlignment?
                    bufferImageHeight = roundUp(vk_texture_header->extent.height, _GetCompressedFormatBlockHeight(vk_texture_header->format));

                    allocationSize = roundUp(outputSlicePitch * outputNumSlices, optimalBufferCopyOffsetAlignment);
                }
                else
                {
                }

                assert(DstSubresource < NumSubresources);

                pDest[DstSubresource].stagingOffset = stagingOffset;
                pDest[DstSubresource].outputRowPitch = outputRowPitch;
                pDest[DstSubresource].outputRowSize = outputRowSize;
                pDest[DstSubresource].outputNumRows = outputNumRows;
                pDest[DstSubresource].outputSlicePitch = outputSlicePitch;
                pDest[DstSubresource].outputNumSlices = outputNumSlices;

                pRegions[DstSubresource].bufferOffset = stagingOffset;
                pRegions[DstSubresource].bufferRowLength = bufferRowLength;
                pRegions[DstSubresource].bufferImageHeight = bufferImageHeight;
                //GetFormatAspectFlags
                pRegions[DstSubresource].imageSubresource.aspectMask = _GetFormatAspectMask(vk_texture_header->format, aspect);
                pRegions[DstSubresource].imageSubresource.mipLevel = mipLevel;
                pRegions[DstSubresource].imageSubresource.baseArrayLayer = arrayLayer;
                pRegions[DstSubresource].imageSubresource.layerCount = 1;
                pRegions[DstSubresource].imageOffset.x = 0;
                pRegions[DstSubresource].imageOffset.y = 0;
                pRegions[DstSubresource].imageOffset.z = 0;
                pRegions[DstSubresource].imageExtent.width = w;
                pRegions[DstSubresource].imageExtent.height = h;
                pRegions[DstSubresource].imageExtent.depth = d;

                stagingOffset += allocationSize;
                TotalBytes += allocationSize;
                ++DstSubresource;

                w = w >> 1;
                h = h >> 1;
                d = d >> 1;
                if (w == 0)
                {
                    w = 1;
                }
                if (h == 0)
                {
                    h = 1;
                }
                if (d == 0)
                {
                    d = 1;
                }
            }
        }
    }

    return TotalBytes;
}

//--------------------------------------------------------------------------------------
static enum VkImageType gNeutralToVulkanTypeMap[] = {
    VK_IMAGE_TYPE_1D,
    VK_IMAGE_TYPE_2D,
    VK_IMAGE_TYPE_3D};
static_assert(TEXTURE_LOADER_TYPE_RANGE_SIZE == (sizeof(gNeutralToVulkanTypeMap) / sizeof(gNeutralToVulkanTypeMap[0])), "gNeutralToVulkanTypeMap may not match!");

static inline enum VkImageType _GetVulkanType(uint32_t neutraltype)
{
    assert(neutraltype < (sizeof(gNeutralToVulkanTypeMap) / sizeof(gNeutralToVulkanTypeMap[0])));
    return gNeutralToVulkanTypeMap[neutraltype];
}

//--------------------------------------------------------------------------------------
static enum VkFormat gNeutralToVulkanFormatMap[] = {
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
static_assert(TEXTURE_LOADER_FORMAT_RANGE_SIZE == (sizeof(gNeutralToVulkanFormatMap) / sizeof(gNeutralToVulkanFormatMap[0])), "gNeutralToVulkanFormatMap may not match!");

static inline enum VkFormat _GetVulkanFormat(uint32_t neutralformat)
{
    assert(neutralformat < (sizeof(gNeutralToVulkanFormatMap) / sizeof(gNeutralToVulkanFormatMap[0])));
    return gNeutralToVulkanFormatMap[neutralformat];
}

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

struct _FormatInfo
{
    uint32_t aspectCount;
    uint32_t aspectMasks[2];

    bool isBlock;
    union {
        struct
        {
            uint32_t compressedBlockWidth;
            uint32_t compressedBlockHeight;
            uint32_t compressedBlockDepth;
            uint32_t compressedBlockSize;
        } compressed;
    };
};

static struct _FormatInfo const gVulkanFormatInfoTable[] = {
    {0, {}, false}, //VK_FORMAT_UNDEFINED
    /* VK_FORMAT_R4G4_UNORM_PACK8 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 1},
    /* VK_FORMAT_R4G4B4A4_UNORM_PACK16 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 2},
    /* VK_FORMAT_B4G4R4A4_UNORM_PACK16 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 3},
    /* VK_FORMAT_R5G6B5_UNORM_PACK16 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 4},
    /* VK_FORMAT_B5G6R5_UNORM_PACK16 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 5},
    /* VK_FORMAT_R5G5B5A1_UNORM_PACK16 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 6},
    /* VK_FORMAT_B5G5R5A1_UNORM_PACK16 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 7},
    /* VK_FORMAT_A1R5G5B5_UNORM_PACK16 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 8},
    /* VK_FORMAT_R8_UNORM */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 9},
    /* VK_FORMAT_R8_SNORM */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 10},
    /* VK_FORMAT_R8_USCALED */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 11},
    /* VK_FORMAT_R8_SSCALED */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 12},
    /* VK_FORMAT_R8_UINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 13},
    /* VK_FORMAT_R8_SINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 14},
    /* VK_FORMAT_R8_SRGB */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 15},
    /* VK_FORMAT_R8G8_UNORM */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 16},
    /* VK_FORMAT_R8G8_SNORM */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 17},
    /* VK_FORMAT_R8G8_USCALED */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 18},
    /* VK_FORMAT_R8G8_SSCALED */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 19},
    /* VK_FORMAT_R8G8_UINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 20},
    /* VK_FORMAT_R8G8_SINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 21},
    /* VK_FORMAT_R8G8_SRGB */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 22},
    /* VK_FORMAT_R8G8B8_UNORM */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 23},
    /* VK_FORMAT_R8G8B8_SNORM */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 24},
    /* VK_FORMAT_R8G8B8_USCALED */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 25},
    /* VK_FORMAT_R8G8B8_SSCALED */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 26},
    /* VK_FORMAT_R8G8B8_UINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 27},
    /* VK_FORMAT_R8G8B8_SINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 28},
    /* VK_FORMAT_R8G8B8_SRGB */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 29},
    /* VK_FORMAT_B8G8R8_UNORM */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 30},
    /* VK_FORMAT_B8G8R8_SNORM */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 31},
    /* VK_FORMAT_B8G8R8_USCALED */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 32},
    /* VK_FORMAT_B8G8R8_SSCALED */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 33},
    /* VK_FORMAT_B8G8R8_UINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 34},
    /* VK_FORMAT_B8G8R8_SINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 35},
    /* VK_FORMAT_B8G8R8_SRGB */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 36},
    /* VK_FORMAT_R8G8B8A8_UNORM */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 37},
    /* VK_FORMAT_R8G8B8A8_SNORM */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 38},
    /* VK_FORMAT_R8G8B8A8_USCALED */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 39},
    /* VK_FORMAT_R8G8B8A8_SSCALED */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 40},
    /* VK_FORMAT_R8G8B8A8_UINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 41},
    /* VK_FORMAT_R8G8B8A8_SINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 42},
    /* VK_FORMAT_R8G8B8A8_SRGB */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 43},
    /* VK_FORMAT_B8G8R8A8_UNORM */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 44},
    /* VK_FORMAT_B8G8R8A8_SNORM */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 45},
    /* VK_FORMAT_B8G8R8A8_USCALED */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 46},
    /* VK_FORMAT_B8G8R8A8_SSCALED */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 47},
    /* VK_FORMAT_B8G8R8A8_UINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 48},
    /* VK_FORMAT_B8G8R8A8_SINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 49},
    /* VK_FORMAT_B8G8R8A8_SRGB */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 50},
    /* VK_FORMAT_A8B8G8R8_UNORM_PACK32 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 51},
    /* VK_FORMAT_A8B8G8R8_SNORM_PACK32 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 52},
    /* VK_FORMAT_A8B8G8R8_USCALED_PACK32 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 53},
    /* VK_FORMAT_A8B8G8R8_SSCALED_PACK32 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 54},
    /* VK_FORMAT_A8B8G8R8_UINT_PACK32 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 55},
    /* VK_FORMAT_A8B8G8R8_SINT_PACK32 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 56},
    /* VK_FORMAT_A8B8G8R8_SRGB_PACK32 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 57},
    /* VK_FORMAT_A2R10G10B10_UNORM_PACK32 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 58},
    /* VK_FORMAT_A2R10G10B10_SNORM_PACK32 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 59},
    /* VK_FORMAT_A2R10G10B10_USCALED_PACK32 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 60},
    /* VK_FORMAT_A2R10G10B10_SSCALED_PACK32 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 61},
    /* VK_FORMAT_A2R10G10B10_UINT_PACK32 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 62},
    /* VK_FORMAT_A2R10G10B10_SINT_PACK32 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 63},
    /* VK_FORMAT_A2B10G10R10_UNORM_PACK32 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 64},
    /* VK_FORMAT_A2B10G10R10_SNORM_PACK32 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 65},
    /* VK_FORMAT_A2B10G10R10_USCALED_PACK32 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 66},
    /* VK_FORMAT_A2B10G10R10_SSCALED_PACK32 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 67},
    /* VK_FORMAT_A2B10G10R10_UINT_PACK32 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 68},
    /* VK_FORMAT_A2B10G10R10_SINT_PACK32 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 69},
    /* VK_FORMAT_R16_UNORM */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 70},
    /* VK_FORMAT_R16_SNORM */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 71},
    /* VK_FORMAT_R16_USCALED */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 72},
    /* VK_FORMAT_R16_SSCALED */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 73},
    /* VK_FORMAT_R16_UINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 74},
    /* VK_FORMAT_R16_SINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 75},
    /* VK_FORMAT_R16_SFLOAT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 76},
    /* VK_FORMAT_R16G16_UNORM */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 77},
    /* VK_FORMAT_R16G16_SNORM */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 78},
    /* VK_FORMAT_R16G16_USCALED */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 79},
    /* VK_FORMAT_R16G16_SSCALED */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 80},
    /* VK_FORMAT_R16G16_UINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 81},
    /* VK_FORMAT_R16G16_SINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 82},
    /* VK_FORMAT_R16G16_SFLOAT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 83},
    /* VK_FORMAT_R16G16B16_UNORM */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 84},
    /* VK_FORMAT_R16G16B16_SNORM */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 85},
    /* VK_FORMAT_R16G16B16_USCALED */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 86},
    /* VK_FORMAT_R16G16B16_SSCALED */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 87},
    /* VK_FORMAT_R16G16B16_UINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 88},
    /* VK_FORMAT_R16G16B16_SINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 89},
    /* VK_FORMAT_R16G16B16_SFLOAT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 90},
    /* VK_FORMAT_R16G16B16A16_UNORM */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 91},
    /* VK_FORMAT_R16G16B16A16_SNORM */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 92},
    /* VK_FORMAT_R16G16B16A16_USCALED */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 93},
    /* VK_FORMAT_R16G16B16A16_SSCALED */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 94},
    /* VK_FORMAT_R16G16B16A16_UINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 95},
    /* VK_FORMAT_R16G16B16A16_SINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 96},
    /* VK_FORMAT_R16G16B16A16_SFLOAT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 97},
    /* VK_FORMAT_R32_UINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 98},
    /* VK_FORMAT_R32_SINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 99},
    /* VK_FORMAT_R32_SFLOAT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 100},
    /* VK_FORMAT_R32G32_UINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 101},
    /* VK_FORMAT_R32G32_SINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 102},
    /* VK_FORMAT_R32G32_SFLOAT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 103},
    /* VK_FORMAT_R32G32B32_UINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 104},
    /* VK_FORMAT_R32G32B32_SINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 105},
    /* VK_FORMAT_R32G32B32_SFLOAT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 106},
    /* VK_FORMAT_R32G32B32A32_UINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 107},
    /* VK_FORMAT_R32G32B32A32_SINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 108},
    /* VK_FORMAT_R32G32B32A32_SFLOAT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 109},
    /* VK_FORMAT_R64_UINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 110},
    /* VK_FORMAT_R64_SINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 111},
    /* VK_FORMAT_R64_SFLOAT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 112},
    /* VK_FORMAT_R64G64_UINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 113},
    /* VK_FORMAT_R64G64_SINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 114},
    /* VK_FORMAT_R64G64_SFLOAT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 115},
    /* VK_FORMAT_R64G64B64_UINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 116},
    /* VK_FORMAT_R64G64B64_SINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 117},
    /* VK_FORMAT_R64G64B64_SFLOAT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 118},
    /* VK_FORMAT_R64G64B64A64_UINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 119},
    /* VK_FORMAT_R64G64B64A64_SINT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 120},
    /* VK_FORMAT_R64G64B64A64_SFLOAT */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 121},
    /* VK_FORMAT_B10G11R11_UFLOAT_PACK32 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 122},
    /* VK_FORMAT_E5B9G9R9_UFLOAT_PACK32 */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 123},
    {1, {VK_IMAGE_ASPECT_DEPTH_BIT}, false},                              //VK_FORMAT_D16_UNORM */
    {1, {VK_IMAGE_ASPECT_DEPTH_BIT}, false},                              //VK_FORMAT_X8_D24_UNORM_PACK32 */
    {1, {VK_IMAGE_ASPECT_DEPTH_BIT}, false},                              //VK_FORMAT_D32_SFLOAT
    {1, {VK_IMAGE_ASPECT_STENCIL_BIT}, false},                            //VK_FORMAT_S8_UINT
    {2, {VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_ASPECT_STENCIL_BIT}, false}, //VK_FORMAT_D16_UNORM_S8_UINT
    {2, {VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_ASPECT_STENCIL_BIT}, false}, //VK_FORMAT_D24_UNORM_S8_UINT
    {2, {VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_ASPECT_STENCIL_BIT}, false}, //VK_FORMAT_D32_SFLOAT_S8_UINT
    /* VK_FORMAT_BC1_RGB_UNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 131},
    /* VK_FORMAT_BC1_RGB_SRGB_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 132},
    /* VK_FORMAT_BC1_RGBA_UNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 133},
    /* VK_FORMAT_BC1_RGBA_SRGB_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 134},
    /* VK_FORMAT_BC2_UNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 135},
    /* VK_FORMAT_BC2_SRGB_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 136},
    /* VK_FORMAT_BC3_UNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 137},
    /* VK_FORMAT_BC3_SRGB_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 138},
    /* VK_FORMAT_BC4_UNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 139},
    /* VK_FORMAT_BC4_SNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 140},
    /* VK_FORMAT_BC5_UNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 141},
    /* VK_FORMAT_BC5_SNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 142},
    /* VK_FORMAT_BC6H_UFLOAT_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 143},
    /* VK_FORMAT_BC6H_SFLOAT_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 144},
    {1, {VK_IMAGE_ASPECT_COLOR_BIT}, true, .compressed = {4, 4, 1, 128}}, //VK_FORMAT_BC7_UNORM_BLOCK
    {1, {VK_IMAGE_ASPECT_COLOR_BIT}, true, .compressed = {4, 4, 1, 128}}, //VK_FORMAT_BC7_SRGB_BLOCK
    /* VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 147},
    /* VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 148},
    /* VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 149},
    /* VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 150},
    /* VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 151},
    /* VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 152},
    /* VK_FORMAT_EAC_R11_UNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 153},
    /* VK_FORMAT_EAC_R11_SNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 154},
    /* VK_FORMAT_EAC_R11G11_UNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 155},
    /* VK_FORMAT_EAC_R11G11_SNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 156},
    /* VK_FORMAT_ASTC_4x4_UNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 157},
    /* VK_FORMAT_ASTC_4x4_SRGB_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 158},
    /* VK_FORMAT_ASTC_5x4_UNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 159},
    /* VK_FORMAT_ASTC_5x4_SRGB_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 160},
    /* VK_FORMAT_ASTC_5x5_UNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 161},
    /* VK_FORMAT_ASTC_5x5_SRGB_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 162},
    /* VK_FORMAT_ASTC_6x5_UNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 163},
    /* VK_FORMAT_ASTC_6x5_SRGB_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 164},
    /* VK_FORMAT_ASTC_6x6_UNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 165},
    /* VK_FORMAT_ASTC_6x6_SRGB_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 166},
    /* VK_FORMAT_ASTC_8x5_UNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 167},
    /* VK_FORMAT_ASTC_8x5_SRGB_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 168},
    /* VK_FORMAT_ASTC_8x6_UNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 169},
    /* VK_FORMAT_ASTC_8x6_SRGB_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 170},
    /* VK_FORMAT_ASTC_8x8_UNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 171},
    /* VK_FORMAT_ASTC_8x8_SRGB_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 172},
    /* VK_FORMAT_ASTC_10x5_UNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 173},
    /* VK_FORMAT_ASTC_10x5_SRGB_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 174},
    /* VK_FORMAT_ASTC_10x6_UNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 175},
    /* VK_FORMAT_ASTC_10x6_SRGB_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 176},
    /* VK_FORMAT_ASTC_10x8_UNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 177},
    /* VK_FORMAT_ASTC_10x8_SRGB_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 178},
    /* VK_FORMAT_ASTC_10x10_UNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 179},
    /* VK_FORMAT_ASTC_10x10_SRGB_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 180},
    /* VK_FORMAT_ASTC_12x10_UNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 181},
    /* VK_FORMAT_ASTC_12x10_SRGB_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 182},
    /* VK_FORMAT_ASTC_12x12_UNORM_BLOCK */ {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false, 183},
    {1, {VK_IMAGE_ASPECT_COLOR_BIT}, false} //VK_FORMAT_ASTC_12x12_SRGB_BLOCK
};
static_assert(VK_FORMAT_RANGE_SIZE == (sizeof(gVulkanFormatInfoTable) / sizeof(gVulkanFormatInfoTable[0])), "gVulkanFormatInfoTable may not match!");

static inline uint32_t _GetFormatAspectCount(VkFormat vkformat)
{
    assert(vkformat < (sizeof(gVulkanFormatInfoTable) / sizeof(gVulkanFormatInfoTable[0])));

    _FormatInfo vk_format_info = gVulkanFormatInfoTable[vkformat];

    return vk_format_info.aspectCount;
}

static inline uint32_t _GetFormatAspectMask(VkFormat vkformat, uint32_t aspect)
{
    assert(vkformat < (sizeof(gVulkanFormatInfoTable) / sizeof(gVulkanFormatInfoTable[0])));

    _FormatInfo vk_format_info = gVulkanFormatInfoTable[vkformat];

    assert(assert < vk_format_info.aspectCount);

    return vk_format_info.aspectMasks[aspect];
}

static inline bool _IsFormatCompressed(VkFormat vkformat)
{
    assert(vkformat < (sizeof(gVulkanFormatInfoTable) / sizeof(gVulkanFormatInfoTable[0])));

    _FormatInfo vk_format_info = gVulkanFormatInfoTable[vkformat];

    return vk_format_info.isBlock;
}

static inline uint32_t _ComputeCompressedFormatImageSize(VkFormat vkformat, uint32_t width, uint32_t height, uint32_t depth)
{
    assert(vkformat < (sizeof(gVulkanFormatInfoTable) / sizeof(gVulkanFormatInfoTable[0])));

    _FormatInfo vk_format_info = gVulkanFormatInfoTable[vkformat];

    assert(vk_format_info.isBlock);
    uint32_t pixelBytes = vk_format_info.compressed.compressedBlockSize / 8;
    uint32_t numBlocksWide = (width + vk_format_info.compressed.compressedBlockWidth - 1) / vk_format_info.compressed.compressedBlockWidth;
    uint32_t numBlocksHigh = (height + vk_format_info.compressed.compressedBlockHeight - 1) / vk_format_info.compressed.compressedBlockHeight;
    uint32_t bytes = numBlocksWide * numBlocksHigh * pixelBytes * depth;

    return bytes;
}

static inline uint32_t _GetCompressedFormatBlockWidth(VkFormat vkformat)
{
    assert(vkformat < (sizeof(gVulkanFormatInfoTable) / sizeof(gVulkanFormatInfoTable[0])));

    _FormatInfo vk_format_info = gVulkanFormatInfoTable[vkformat];

    assert(vk_format_info.isBlock);

    return vk_format_info.compressed.compressedBlockWidth;
}

static inline uint32_t _GetCompressedFormatBlockHeight(VkFormat vkformat)
{
    assert(vkformat < (sizeof(gVulkanFormatInfoTable) / sizeof(gVulkanFormatInfoTable[0])));

    _FormatInfo vk_format_info = gVulkanFormatInfoTable[vkformat];

    assert(vk_format_info.isBlock);

    return vk_format_info.compressed.compressedBlockHeight;
}

#if 0
void TextureLoader_UpdateSubresources(struct TextureLoader_NeutralHeader const *texture_desc)
{
    // Context::texSubImage2D libANGLE/Context.cpp
    // Texture::setSubImage libANGLE/Texture.cpp
    // TextureVk::setSubImage libANGLE/renderer/vulkan/TextureVk.cpp
    // TextureVk::setSubImageImpl libANGLE/renderer/vulkan/TextureVk.cpp
    // ImageHelper::stageSubresourceUpdate libANGLE/renderer/vulkan/vk_helpers.cpp
    // ImageHelper::CalculateBufferInfo libANGLE/renderer/vulkan/vk_helpers.cpp
    // ImageHelper::stageSubresourceUpdateImpl libANGLE/renderer/vulkan/vk_helpers.cpp

    // gFormatInfoTable libANGLE/renderer/Format_table_autogen.cpp
    // { FormatID::R8G8B8A8_UNORM, GL_RGBA8, GL_RGBA8, GenerateMip<R8G8B8A8>, NoCopyFunctions, ReadColor<R8G8B8A8, GLfloat>, WriteColor<R8G8B8A8, GLfloat>, GL_UNSIGNED_NORMALIZED, 8, 8, 8, 8, 0, 0, 0, 4, 0 */ {false */ {false */ {false, gl::VertexAttribType::UnsignedByte },

    // BuildInternalFormatInfoMap libANGLE/formatutils.cpp
    // From ES 3.0.1 spec, table 3.12
    //                    | Internal format     |sized| R | G | B | A |S | Format         | Type                             | Component type        | SRGB | Texture supported                                | Filterable     | Texture attachment                               | Renderbuffer                                   | Blend
    // AddRGBAFormat(&map, GL_RGBA8,             true,  8,  8,  8,  8, 0, GL_RGBA,         GL_UNSIGNED_BYTE,                  GL_UNSIGNED_NORMALIZED */ {false, RequireESOrExt<3, 0, &Extensions::textureStorage>, AlwaysSupported, RequireESOrExt<3, 0, &Extensions::textureStorage>, RequireESOrExt<3, 0, &Extensions::rgb8rgba8OES>, RequireESOrExt<3, 0, &Extensions::rgb8rgba8OES>);
    // From EXT_texture_compression_bptc
    //                          | Internal format                         | W | H |D | BS |CC| SRGB | Texture supported                              | Filterable     | Texture attachment | Renderbuffer  | Blend
    // AddCompressedFormat(&map, GL_COMPRESSED_RGBA_BPTC_UNORM_EXT,         4,  4, 1, 128, 4 */ {false, RequireExt<&Extensions::textureCompressionBPTC>, AlwaysSupported, NeverSupported,      NeverSupported, NeverSupported);

    struct FormatInfo storageFormat = _FormatInfoTable(texture_desc->format);

    size_t outputRowPitch;
    size_t outputDepthPitch;

    uint32_t bufferRowLength;
    uint32_t bufferImageHeight;

    size_t allocationSize;

    if (storageFormat.isBlock)
    {
        uint32_t rowPitch;
        uint32_t depthPitch;
        uint32_t totalSize;

        rowPitch = storageFormat.computeCompressedImageSize(texture_desc->width, 1, 1);
        depthPitch = storageFormat.computeCompressedImageSize(texture_desc->width, texture_desc->height, 1);
        totalSize = storageFormat.computeCompressedImageSize(texture_desc->width, texture_desc->height, texture_desc->depth);

        outputRowPitch = rowPitch;
        outputDepthPitch = depthPitch;

        bufferRowLength = roundUp(texture_desc->width, storageFormat.compressed.compressedBlockWidth);
        bufferImageHeight = roundUp(texture_desc->height, storageFormat.compressed.compressedBlockHeight);
        allocationSize = totalSize;
    }
    else
    {
    }

    VkBuffer bufferHandle = VK_NULL_HANDLE;

    uint8_t *stagingPointer    = nullptr;
    VkDeviceSize stagingOffset = 0;
    ANGLE_TRY(mStagingBuffer.allocate(contextVk, allocationSize, &stagingPointer, &bufferHandle,
                                      &stagingOffset, nullptr));

    const uint8_t *source = pixels + static_cast<ptrdiff_t>(inputSkipBytes);

    loadFunctionInfo.loadFunction(glExtents.width, glExtents.height, glExtents.depth, source,
                                  inputRowPitch, inputDepthPitch, stagingPointer, outputRowPitch,
                                  outputDepthPitch);

    VkBufferImageCopy copy         = {};
    VkImageAspectFlags aspectFlags = GetFormatAspectFlags(vkFormat.actualImageFormat());

    copy.bufferOffset      = stagingOffset;
    copy.bufferRowLength   = bufferRowLength;
    copy.bufferImageHeight = bufferImageHeight;

    copy.imageSubresource.mipLevel   = index.getLevelIndex();
    copy.imageSubresource.layerCount = index.getLayerCount();

    gl_vk::GetOffset(offset, &copy.imageOffset);
    gl_vk::GetExtent(glExtents, &copy.imageExtent);

    if (gl::IsArrayTextureType(index.getType()))
    {
        copy.imageSubresource.baseArrayLayer = offset.z;
        copy.imageOffset.z                   = 0;
        copy.imageExtent.depth               = 1;
    }
    else
    {
        copy.imageSubresource.baseArrayLayer = index.hasLayer() ? index.getLayerIndex() : 0;
    }

    if (stencilAllocationSize > 0)
    {
        // Note: Stencil is always one byte
        ASSERT((aspectFlags & VK_IMAGE_ASPECT_STENCIL_BIT) != 0);

        // Skip over depth data.
        stagingPointer += outputDepthPitch * glExtents.depth;
        stagingOffset += outputDepthPitch * glExtents.depth;

        // recompute pitch for stencil data
        outputRowPitch   = glExtents.width;
        outputDepthPitch = outputRowPitch * glExtents.height;

        ASSERT(stencilLoadFunction != nullptr);
        stencilLoadFunction(glExtents.width, glExtents.height, glExtents.depth, source,
                            inputRowPitch, inputDepthPitch, stagingPointer, outputRowPitch,
                            outputDepthPitch);

        VkBufferImageCopy stencilCopy = {};

        stencilCopy.bufferOffset                    = stagingOffset;
        stencilCopy.bufferRowLength                 = bufferRowLength;
        stencilCopy.bufferImageHeight               = bufferImageHeight;
        stencilCopy.imageSubresource.mipLevel       = copy.imageSubresource.mipLevel;
        stencilCopy.imageSubresource.baseArrayLayer = copy.imageSubresource.baseArrayLayer;
        stencilCopy.imageSubresource.layerCount     = copy.imageSubresource.layerCount;
        stencilCopy.imageOffset                     = copy.imageOffset;
        stencilCopy.imageExtent                     = copy.imageExtent;
        stencilCopy.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_STENCIL_BIT;
        mSubresourceUpdates.emplace_back(mStagingBuffer.getCurrentBuffer(), stencilCopy);

        aspectFlags &= ~VK_IMAGE_ASPECT_STENCIL_BIT;
    }

        if (IsMaskFlagSet(aspectFlags, static_cast<VkImageAspectFlags>(VK_IMAGE_ASPECT_STENCIL_BIT |
                                                                   VK_IMAGE_ASPECT_DEPTH_BIT)))
    {
        // We still have both depth and stencil aspect bits set. That means we have a destination
        // buffer that is packed depth stencil and that the application is only loading one aspect.
        // Figure out which aspect the user is touching and remove the unused aspect bit.
        if (formatInfo.stencilBits > 0)
        {
            aspectFlags &= ~VK_IMAGE_ASPECT_DEPTH_BIT;
        }
        else
        {
            aspectFlags &= ~VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    }

    if (aspectFlags)
    {
        copy.imageSubresource.aspectMask = aspectFlags;
        mSubresourceUpdates.emplace_back(mStagingBuffer.getCurrentBuffer(), copy);
    }
    //return true;
}
#endif