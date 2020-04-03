#include "TextureLoader_VK.h"

#include <assert.h>

uint32_t TextureLoader_CalcSubresource(uint32_t mipLevel, uint32_t arrayLayer, uint32_t aspectIndex, uint32_t mipLevels, uint32_t arrayLayers)
{
    return mipLevel + arrayLayer * mipLevels + aspectIndex * mipLevels * arrayLayers;
}

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

static inline uint32_t _GetFormatAspectMask(VkFormat vkformat, uint32_t aspectIndex);

static inline bool _IsFormatCompressed(VkFormat vkformat);

static inline bool _IsFormatRGBA(VkFormat vkformat);

static inline bool _IsFormatDepthStencil(VkFormat vkformat);

static inline uint32_t _GetCompressedFormatBlockWidth(VkFormat vkformat);

static inline uint32_t _GetCompressedFormatBlockHeight(VkFormat vkformat);

static inline uint32_t _GetCompressedFormatBlockDepth(VkFormat vkformat);

static inline uint32_t _GetCompressedFormatBlockSizeInBytes(VkFormat vkformat);

static inline uint32_t _GetRGBAFormatPixelBytes(VkFormat vkformat);

static inline uint32_t _GetDepthStencilFormatPixelBytes(VkFormat vkformat, uint32_t aspectIndex);

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

    uint32_t aspectCount = TextureLoader_GetFormatAspectCount(vk_texture_header->format);

    size_t stagingOffset = 0;
    size_t TotalBytes = 0;

    for (uint32_t aspectIndex = 0; aspectIndex < aspectCount; ++aspectIndex)
    {
        for (uint32_t arrayLayer = 0; arrayLayer < vk_texture_header->arrayLayers; ++arrayLayer)
        {
            size_t w = vk_texture_header->extent.width;
            size_t h = vk_texture_header->extent.height;
            size_t d = vk_texture_header->extent.depth;
            for (uint32_t mipLevel = 0; mipLevel < vk_texture_header->mipLevels; ++mipLevel)
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
                    assert(1 == _GetCompressedFormatBlockDepth(vk_texture_header->format));

                    outputRowSize = ((w + _GetCompressedFormatBlockWidth(vk_texture_header->format) - 1) / _GetCompressedFormatBlockWidth(vk_texture_header->format)) * _GetCompressedFormatBlockSizeInBytes(vk_texture_header->format);
                    outputNumRows = (h + _GetCompressedFormatBlockHeight(vk_texture_header->format) - 1) / _GetCompressedFormatBlockHeight(vk_texture_header->format);
                    outputNumSlices = d;

                    outputRowPitch = roundUp(outputRowSize, optimalBufferCopyRowPitchAlignment);
                    outputSlicePitch = outputRowPitch * outputNumRows;

                    // bufferOffset must be a multiple of 4
                    // bufferRowLength must be 0, or greater than or equal to the width member of imageExtent
                    // bufferImageHeight must be 0, or greater than or equal to the height member of imageExtent
                    // If the calling command’s VkImage parameter is a compressed image, ... , bufferRowLength must be a multiple of the compressed texel block width
                    // If the calling command’s VkImage parameter is a compressed image, ... , bufferImageHeight must be a multiple of the compressed texel block height

                    // 19.4.1. Buffer and Image Addressing
                    // For block-compressed formats, all parameters are still specified in texels rather than compressed texel blocks, but the addressing math operates on whole compressed texel blocks.
                    // Pseudocode for compressed copy addressing is:
                    // ```
                    // rowLength = region->bufferRowLength;
                    // if (rowLength == 0) rowLength = region->imageExtent.width;
                    //
                    // imageHeight = region->bufferImageHeight;
                    // if (imageHeight == 0) imageHeight = region->imageExtent.height;
                    //
                    // compressedTexelBlockSizeInBytes = <compressed texel block size taken from the src/dstImage>;
                    // rowLength /= compressedTexelBlockWidth;
                    // imageHeight /= compressedTexelBlockHeight;
                    //
                    // address of (x,y,z) = region->bufferOffset + (((z * imageHeight) + y) * rowLength + x) * compressedTexelBlockSizeInBytes;
                    // where x,y,z range from (0,0,0) to region->imageExtent.{width/compressedTexelBlockWidth,height/compressedTexelBlockHeight,depth/compressedTexelBlockDepth}.
                    //
                    // ```

                    bufferRowLength = (outputRowPitch / _GetCompressedFormatBlockSizeInBytes(vk_texture_header->format)) * _GetCompressedFormatBlockWidth(vk_texture_header->format);
                    bufferImageHeight = outputNumRows * _GetCompressedFormatBlockHeight(vk_texture_header->format);

                    //bufferRowLength = roundUp(vk_texture_header->extent.width, _GetCompressedFormatBlockWidth(vk_texture_header->format));
                    //bufferImageHeight = roundUp(vk_texture_header->extent.height, _GetCompressedFormatBlockHeight(vk_texture_header->format));

                    allocationSize = roundUp(outputSlicePitch * outputNumSlices, optimalBufferCopyOffsetAlignment);
                }
                else if (_IsFormatRGBA(vk_texture_header->format))
                {

                    outputRowSize = _GetRGBAFormatPixelBytes(vk_texture_header->format) * w;
                    outputNumRows = h;
                    outputNumSlices = d;

                    outputRowPitch = roundUp(outputRowSize, optimalBufferCopyRowPitchAlignment);
                    outputSlicePitch = outputRowPitch * outputNumRows;

                    bufferRowLength = outputRowPitch / _GetRGBAFormatPixelBytes(vk_texture_header->format);
                    bufferImageHeight = outputNumRows;

                    allocationSize = roundUp(outputSlicePitch * outputNumSlices, optimalBufferCopyOffsetAlignment);
                }
                else
                {
                    assert(_IsFormatDepthStencil(vk_texture_header->format));
                    outputRowSize = _GetDepthStencilFormatPixelBytes(vk_texture_header->format, aspectIndex) * w;
                    outputNumRows = h;
                    outputNumSlices = d;

                    outputRowPitch = roundUp(outputRowSize, optimalBufferCopyRowPitchAlignment);
                    outputSlicePitch = outputRowPitch * outputNumRows;

                    bufferRowLength = outputRowPitch / _GetDepthStencilFormatPixelBytes(vk_texture_header->format, aspectIndex);
                    bufferImageHeight = outputNumRows;

                    allocationSize = roundUp(outputSlicePitch * outputNumSlices, optimalBufferCopyOffsetAlignment);
                }

                size_t DstSubresource = TextureLoader_CalcSubresource(mipLevel, arrayLayer, aspectIndex, vk_texture_header->mipLevels, vk_texture_header->arrayLayers);
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
                pRegions[DstSubresource].imageSubresource.aspectMask = _GetFormatAspectMask(vk_texture_header->format, aspectIndex);
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

// GetLoadFunctionsLoadFunctionsMap libANGLE/renderer/load_functions_table_autogen.cpp
// LoadToNative
// LoadCompressedToNative

struct _FormatInfo
{
    uint32_t _union_tag;

    union {
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

static struct _FormatInfo const gVulkanFormatInfoTable[] = {
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
static_assert(VK_FORMAT_RANGE_SIZE == (sizeof(gVulkanFormatInfoTable) / sizeof(gVulkanFormatInfoTable[0])), "gVulkanFormatInfoTable may not match!");

uint32_t TextureLoader_GetFormatAspectCount(VkFormat vkformat)
{
    assert(vkformat < (sizeof(gVulkanFormatInfoTable) / sizeof(gVulkanFormatInfoTable[0])));

    _FormatInfo vk_format_info = gVulkanFormatInfoTable[vkformat];

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

static inline uint32_t _GetFormatAspectMask(VkFormat vkformat, uint32_t aspectIndex)
{
    assert(vkformat < (sizeof(gVulkanFormatInfoTable) / sizeof(gVulkanFormatInfoTable[0])));

    _FormatInfo vk_format_info = gVulkanFormatInfoTable[vkformat];

    if (vk_format_info._union_tag == 3 || vk_format_info._union_tag == 1)
    {
        assert(aspectIndex < 1);
        VkImageAspectFlagBits aspectMasks[1] = {VK_IMAGE_ASPECT_COLOR_BIT};
        return aspectMasks[aspectIndex];
    }
    else if (vk_format_info._union_tag == 2)
    {
        if (vk_format_info.depthstencil.stencilBytes == 0)
        {
            assert(aspectIndex < 1);
            VkImageAspectFlagBits aspectMasks[1] = {VK_IMAGE_ASPECT_DEPTH_BIT};
            return aspectMasks[aspectIndex];
        }
        else if (vk_format_info.depthstencil.depthBytes != 0)
        {
            assert(aspectIndex < 2);
            VkImageAspectFlagBits aspectMasks[2] = {VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_ASPECT_STENCIL_BIT};
            return aspectMasks[aspectIndex];
        }
        else
        {
            assert(aspectIndex < 1);
            VkImageAspectFlagBits aspectMasks[1] = {VK_IMAGE_ASPECT_STENCIL_BIT};
            return aspectMasks[aspectIndex];
        }
    }
    else
    {
        return 0x7FFFFFFF;
    }
}

static inline bool _IsFormatCompressed(VkFormat vkformat)
{
    assert(vkformat < (sizeof(gVulkanFormatInfoTable) / sizeof(gVulkanFormatInfoTable[0])));

    _FormatInfo vk_format_info = gVulkanFormatInfoTable[vkformat];

    return (vk_format_info._union_tag == 3);
}

static inline bool _IsFormatRGBA(VkFormat vkformat)
{
    assert(vkformat < (sizeof(gVulkanFormatInfoTable) / sizeof(gVulkanFormatInfoTable[0])));

    _FormatInfo vk_format_info = gVulkanFormatInfoTable[vkformat];

    return (vk_format_info._union_tag == 1);
}

static inline bool _IsFormatDepthStencil(VkFormat vkformat)
{
    assert(vkformat < (sizeof(gVulkanFormatInfoTable) / sizeof(gVulkanFormatInfoTable[0])));

    _FormatInfo vk_format_info = gVulkanFormatInfoTable[vkformat];

    return (vk_format_info._union_tag == 2);
}

static inline uint32_t _GetCompressedFormatBlockWidth(VkFormat vkformat)
{
    assert(_IsFormatCompressed(vkformat));

    assert(vkformat < (sizeof(gVulkanFormatInfoTable) / sizeof(gVulkanFormatInfoTable[0])));

    _FormatInfo vk_format_info = gVulkanFormatInfoTable[vkformat];

    return vk_format_info.compressed.compressedBlockWidth;
}

static inline uint32_t _GetCompressedFormatBlockHeight(VkFormat vkformat)
{
    assert(_IsFormatCompressed(vkformat));

    assert(vkformat < (sizeof(gVulkanFormatInfoTable) / sizeof(gVulkanFormatInfoTable[0])));

    _FormatInfo vk_format_info = gVulkanFormatInfoTable[vkformat];

    return vk_format_info.compressed.compressedBlockHeight;
}

static inline uint32_t _GetCompressedFormatBlockDepth(VkFormat vkformat)
{
    assert(_IsFormatCompressed(vkformat));

    assert(vkformat < (sizeof(gVulkanFormatInfoTable) / sizeof(gVulkanFormatInfoTable[0])));

    _FormatInfo vk_format_info = gVulkanFormatInfoTable[vkformat];

    return vk_format_info.compressed.compressedBlockDepth;
}

static inline uint32_t _GetCompressedFormatBlockSizeInBytes(VkFormat vkformat)
{
    assert(_IsFormatCompressed(vkformat));

    assert(vkformat < (sizeof(gVulkanFormatInfoTable) / sizeof(gVulkanFormatInfoTable[0])));

    _FormatInfo vk_format_info = gVulkanFormatInfoTable[vkformat];

    return vk_format_info.compressed.compressedBlockSizeInBytes;
}

static inline uint32_t _GetRGBAFormatPixelBytes(VkFormat vkformat)
{
    assert(_IsFormatRGBA(vkformat));

    assert(vkformat < (sizeof(gVulkanFormatInfoTable) / sizeof(gVulkanFormatInfoTable[0])));

    _FormatInfo vk_format_info = gVulkanFormatInfoTable[vkformat];

    return vk_format_info.rgba.pixelBytes;
}

static inline uint32_t _GetDepthStencilFormatPixelBytes(VkFormat vkformat, uint32_t aspectIndex)
{
    assert(_IsFormatDepthStencil(vkformat));

    assert(vkformat < (sizeof(gVulkanFormatInfoTable) / sizeof(gVulkanFormatInfoTable[0])));

    _FormatInfo vk_format_info = gVulkanFormatInfoTable[vkformat];

    if (vk_format_info.depthstencil.stencilBytes == 0)
    {
        assert(aspectIndex < 1);
        uint32_t pixelBytes[1] = {vk_format_info.depthstencil.depthBytes};
        return pixelBytes[aspectIndex];
    }
    else if (vk_format_info.depthstencil.depthBytes != 0)
    {
        assert(aspectIndex < 2);
        uint32_t pixelBytes[2] = {vk_format_info.depthstencil.depthBytes, vk_format_info.depthstencil.stencilBytes};
        return pixelBytes[aspectIndex];
    }
    else
    {
        assert(aspectIndex < 1);
        uint32_t pixelBytes[1] = {vk_format_info.depthstencil.stencilBytes};
        return pixelBytes[aspectIndex];
    }
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