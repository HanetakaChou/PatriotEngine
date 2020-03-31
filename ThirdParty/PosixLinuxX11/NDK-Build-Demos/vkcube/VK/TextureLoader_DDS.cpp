#include "TextureLoader_DDS.h"

#include <vulkan/vulkan.h>

#include <assert.h>

struct FormatInfo
{
    VkFormat format;
    bool isBlock;
    union {
        struct
        {
            uint32_t compressedBlockWidth;
            uint32_t compressedBlockHeight;
            uint32_t compressedBlockDepth;
            uint32_t compressedBlockSize;
        } _compressed
    };

    inline uint32_t computeCompressedImageSize(uint32_t width, uint32_t height, uint32_t depth)
    {
        assert(isBlock);
        uint32_t pixelBytes = _compressed.compressedBlockSize / 8;
        uint32_t numBlocksWide = (width + _compressed.compressedBlockWidth - 1) / _compressed.compressedBlockWidth;
        uint32_t numBlocksHigh = (height + _compressed.compressedBlockHeight - 1) / _compressed.compressedBlockHeight;
        uint32_t bytes = numBlocksWide * numBlocksHigh * pixelBytes * depth;
    }
};

static struct FormatInfo const gFormatInfoTable[] = {
    // clang-format off
    {VK_FORMAT_UNDEFINED, false},
    {VK_FORMAT_BC7_UNORM_BLOCK, true, 4, 4, 1, 128},
    {VK_FORMAT_BC7_SRGB_BLOCK, true, 4, 4, 1, 128}
    // clang-format on
};

template <typename T>
static inline T roundUp(const T value, const T alignment)
{
    auto temp = value + alignment - static_cast<T>(1);
    return temp - temp % alignment;
}

void TextureLoader_UpdateSubresources(struct Texture_Header const *texture_desc)
{
    // Context::texSubImage2D libANGLE/Context.cpp
    // Texture::setSubImage libANGLE/Texture.cpp
    // TextureVk::setSubImage libANGLE/renderer/vulkan/TextureVk.cpp
    // TextureVk::setSubImageImpl libANGLE/renderer/vulkan/TextureVk.cpp
    // ImageHelper::stageSubresourceUpdate libANGLE/renderer/vulkan/vk_helpers.cpp
    // ImageHelper::stageSubresourceUpdateImpl libANGLE/renderer/vulkan/vk_helpers.cpp
    // ImageHelper::CalculateBufferInfo libANGLE/renderer/vulkan/vk_helpers.cpp
    // ImageHelper::stageSubresourceUpdateImpl libANGLE/renderer/vulkan/vk_helpers.cpp

    // gFormatInfoTable libANGLE/renderer/Format_table_autogen.cpp
    // { FormatID::R8G8B8A8_UNORM, GL_RGBA8, GL_RGBA8, GenerateMip<R8G8B8A8>, NoCopyFunctions, ReadColor<R8G8B8A8, GLfloat>, WriteColor<R8G8B8A8, GLfloat>, GL_UNSIGNED_NORMALIZED, 8, 8, 8, 8, 0, 0, 0, 4, 0, false, false, false, gl::VertexAttribType::UnsignedByte },

    // BuildInternalFormatInfoMap libANGLE/formatutils.cpp
    // From ES 3.0.1 spec, table 3.12
    //                    | Internal format     |sized| R | G | B | A |S | Format         | Type                             | Component type        | SRGB | Texture supported                                | Filterable     | Texture attachment                               | Renderbuffer                                   | Blend
    // AddRGBAFormat(&map, GL_RGBA8,             true,  8,  8,  8,  8, 0, GL_RGBA,         GL_UNSIGNED_BYTE,                  GL_UNSIGNED_NORMALIZED, false, RequireESOrExt<3, 0, &Extensions::textureStorage>, AlwaysSupported, RequireESOrExt<3, 0, &Extensions::textureStorage>, RequireESOrExt<3, 0, &Extensions::rgb8rgba8OES>, RequireESOrExt<3, 0, &Extensions::rgb8rgba8OES>);
    // From EXT_texture_compression_bptc
    //                          | Internal format                         | W | H |D | BS |CC| SRGB | Texture supported                              | Filterable     | Texture attachment | Renderbuffer  | Blend
    // AddCompressedFormat(&map, GL_COMPRESSED_RGBA_BPTC_UNORM_EXT,         4,  4, 1, 128, 4, false, RequireExt<&Extensions::textureCompressionBPTC>, AlwaysSupported, NeverSupported,      NeverSupported, NeverSupported);

    struct FormatInfo storageFormat = gFormatInfoTable[texture_desc->format];

    size_t outputRowPitch;
    size_t outputDepthPitch;

    uint32_t bufferRowLength;
    uint32_t bufferImageHeight;

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

        bufferRowLength = roundUp(texture_desc->width, storageFormat.compressedBlockWidth);
        bufferImageHeight = roundUp(texture_desc->height, storageFormat.compressedBlockHeight);
    }
    else
    {
        
    }

#if 0
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
#endif

    return true;
}