#include "TextureLoader_MTL.h"

//#include <TargetConditionals.h>
//__is_target_os

uint32_t TextureLoader_CalcSubresource(uint32_t mipLevel, uint32_t arrayLayer, uint32_t aspectIndex, uint32_t mipLevels, uint32_t arrayLayers)
{
    return mipLevel + arrayLayer * mipLevels + aspectIndex * mipLevels * arrayLayers;
}

static inline MTLTextureType _GetMetalType(bool isCubeCompatible, uint32_t neutraltype, NSUInteger arrayLength);

static inline MTLPixelFormat _GetMetalFormat(uint32_t neutralformat);

struct TextureLoader_SpecificHeader TextureLoader_ToSpecificHeader(struct TextureLoader_NeutralHeader const *neutral_texture_header)
{
    struct TextureLoader_SpecificHeader specific_texture_header;

    specific_texture_header.textureType = _GetMetalType(neutral_texture_header->isCubeMap, neutral_texture_header->type, neutral_texture_header->arrayLayers);
    specific_texture_header.pixelFormat = _GetMetalFormat(neutral_texture_header->format);
    specific_texture_header.width = neutral_texture_header->width;
    specific_texture_header.height = neutral_texture_header->height;
    specific_texture_header.depth = neutral_texture_header->depth;
    specific_texture_header.mipmapLevelCount = neutral_texture_header->mipLevels;
    assert((!neutral_texture_header->isCubeMap) || ((neutral_texture_header->type == TEXTURE_LOADER_TYPE_2D) && ((neutral_texture_header->arrayLayers % 6) == 0)));
    specific_texture_header.arrayLength = (!neutral_texture_header->isCubeMap) ? neutral_texture_header->arrayLayers : (neutral_texture_header->arrayLayers / 6);

    return specific_texture_header;
}

static inline uint32_t _GetFormatAspectCount(MTLPixelFormat mtlformat);

uint32_t TextureLoader_GetFormatAspectCount(MTLPixelFormat mtlformat)
{
    return _GetFormatAspectCount(mtlformat);
}

static inline bool _IsFormatCompressed(MTLPixelFormat mtlformat);

static inline bool _IsFormatRGBA(MTLPixelFormat mtlformat);

static inline bool _IsFormatDepthStencil(MTLPixelFormat mtlformat);

static inline uint32_t _GetCompressedFormatBlockWidth(MTLPixelFormat mtlformat);

static inline uint32_t _GetCompressedFormatBlockHeight(MTLPixelFormat mtlformat);

static inline uint32_t _GetCompressedFormatBlockDepth(MTLPixelFormat mtlformat);

static inline uint32_t _GetCompressedFormatBlockSizeInBytes(MTLPixelFormat mtlformat);

static inline uint32_t _GetRGBAFormatPixelBytes(MTLPixelFormat mtlformat);

static inline uint32_t _GetDepthStencilFormatPixelBytes(MTLPixelFormat mtlformat, uint32_t aspectIndex);

static inline uint32_t _GetSliceCount(MTLTextureType textureType, uint32_t arrayLength)
{
    return ((textureType != MTLTextureTypeCube && textureType != MTLTextureTypeCubeArray) ? (arrayLength) : (6 * arrayLength));
}

uint32_t TextureLoader_GetSliceCount(MTLTextureType textureType, uint32_t arrayLength)
{
    return _GetSliceCount(textureType, arrayLength);
}

template <typename T, typename T2>
static inline T roundUp(const T value, const T2 alignment)
{
    auto temp = value + alignment - static_cast<T>(1);
    return temp - temp % alignment;
}

size_t TextureLoader_GetCopyableFootprints(struct TextureLoader_SpecificHeader const *mtl_texture_header,
                                           uint32_t NumSubresources, struct TextureLoader_MemcpyDest *pDest, TextureLoader_MTLCopyFromBuffer *pRegions)
{
    // MoltenVK/GPUObjects/MVKDevice.mm
#if __is_target_os(ios)
    NSUInteger optimalBufferCopyOffsetAlignment = 64;
    NSUInteger optimalBufferCopyRowPitchAlignment = 1;
    // MTLDevice_supportsFeatureSet(_mtlDevice, MTLFeatureSet_iOS_GPUFamily3_v1)
    // NSUInteger optimalBufferCopyOffsetAlignment) = 16;
    // NSUInteger optimalBufferCopyRowPitchAlignment) = 1;
#elif __is_target_os(macos)
    NSUInteger optimalBufferCopyOffsetAlignment = 256;
    NSUInteger optimalBufferCopyRowPitchAlignment = 1;
#else
#error Unknown Target
#endif

    // MVKCmdBufferImageCopy::encode // MoltenVK/Commands/MVKCmdTransfer.mm
    // TextureMtl::setSubImageImpl  // src/libANGLE/renderer/metal/TextureMtl.mm

    uint32_t aspectCount = _GetFormatAspectCount(mtl_texture_header->pixelFormat);
    uint32_t sliceCount = _GetSliceCount(mtl_texture_header->textureType, mtl_texture_header->arrayLength);

    size_t TotalBytes = 0;
    size_t stagingOffset = 0;

    for (uint32_t aspectIndex = 0; aspectIndex < aspectCount; ++aspectIndex)
    {
        for (uint32_t sliceIndex = 0; sliceIndex < sliceCount; ++sliceIndex)
        {
            size_t w = mtl_texture_header->width;
            size_t h = mtl_texture_header->height;
            size_t d = mtl_texture_header->depth;
            for (uint32_t mipLevel = 0; mipLevel < mtl_texture_header->mipmapLevelCount; ++mipLevel)
            {
                size_t outputRowPitch;
                size_t outputRowSize;
                size_t outputNumRows;
                size_t outputSlicePitch;
                size_t outputNumSlices;

                size_t allocationSize;
                if (_IsFormatCompressed(mtl_texture_header->pixelFormat))
                {
                    // https://developer.apple.com/documentation/metal/mtlblitcommandencoder/1400752-copyfrombuffer
                    //
                    // If the texture’s pixel format is a compressed format, then sourceSize must be a multiple of
                    // the pixel format’s block size or be clamped to the edge of the texture if the block extends
                    // outside the bounds of a texture. For a compressed format, sourceBytesPerRow is the
                    // number of bytes from the start of one row of blocks to the start of the next row of blocks.
                    //

                    assert(1 == _GetCompressedFormatBlockDepth(mtl_texture_header->pixelFormat));

                    outputRowSize = ((w + _GetCompressedFormatBlockWidth(mtl_texture_header->pixelFormat) - 1) / _GetCompressedFormatBlockWidth(mtl_texture_header->pixelFormat)) * _GetCompressedFormatBlockSizeInBytes(mtl_texture_header->pixelFormat);
                    outputNumRows = (h + _GetCompressedFormatBlockHeight(mtl_texture_header->pixelFormat) - 1) / _GetCompressedFormatBlockHeight(mtl_texture_header->pixelFormat);
                    outputNumSlices = d;

                    outputRowPitch = roundUp(outputRowSize, optimalBufferCopyRowPitchAlignment);
                    outputSlicePitch = outputRowPitch * outputNumRows;

                    allocationSize = roundUp(outputSlicePitch * outputNumSlices, optimalBufferCopyOffsetAlignment);
                }
                else if (_IsFormatRGBA(mtl_texture_header->pixelFormat))
                {

                    outputRowSize = _GetRGBAFormatPixelBytes(mtl_texture_header->pixelFormat) * w;
                    outputNumRows = h;
                    outputNumSlices = d;

                    outputRowPitch = roundUp(outputRowSize, optimalBufferCopyRowPitchAlignment);
                    outputSlicePitch = outputRowPitch * outputNumRows;

                    allocationSize = roundUp(outputSlicePitch * outputNumSlices, optimalBufferCopyOffsetAlignment);
                }
                else
                {
                    assert(_IsFormatDepthStencil(mtl_texture_header->pixelFormat));
                    outputRowSize = _GetDepthStencilFormatPixelBytes(mtl_texture_header->pixelFormat, aspectIndex) * w;
                    outputNumRows = h;
                    outputNumSlices = d;

                    outputRowPitch = roundUp(outputRowSize, optimalBufferCopyRowPitchAlignment);
                    outputSlicePitch = outputRowPitch * outputNumRows;

                    allocationSize = roundUp(outputSlicePitch * outputNumSlices, optimalBufferCopyOffsetAlignment);
                }

                uint32_t DstSubresource = TextureLoader_CalcSubresource(mipLevel, sliceIndex, aspectIndex, mtl_texture_header->mipmapLevelCount, sliceCount);
                assert(DstSubresource < NumSubresources);

                pDest[DstSubresource].stagingOffset = stagingOffset;
                pDest[DstSubresource].outputRowPitch = outputRowPitch;
                pDest[DstSubresource].outputRowSize = outputRowSize;
                pDest[DstSubresource].outputNumRows = outputNumRows;
                pDest[DstSubresource].outputSlicePitch = outputSlicePitch;
                pDest[DstSubresource].outputNumSlices = outputNumSlices;

                pRegions[DstSubresource].sourceOffset = stagingOffset;
                pRegions[DstSubresource].sourceBytesPerRow = outputRowPitch;
                pRegions[DstSubresource].sourceBytesPerImage = outputSlicePitch;
                pRegions[DstSubresource].sourceSize.width = w;
                pRegions[DstSubresource].sourceSize.height = h;
                pRegions[DstSubresource].sourceSize.depth = d;
                pRegions[DstSubresource].destinationSlice = sliceIndex;
                pRegions[DstSubresource].destinationLevel = mipLevel;
                pRegions[DstSubresource].destinationOrigin.x = 0;
                pRegions[DstSubresource].destinationOrigin.y = 0;
                pRegions[DstSubresource].destinationOrigin.z = 0;

                TotalBytes += allocationSize;
                stagingOffset += TotalBytes;

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
static inline MTLTextureType _GetMetalType(bool isCubeCompatible, uint32_t neutraltype, NSUInteger arrayLength)
{
    if (neutraltype == TEXTURE_LOADER_TYPE_2D)
    {
        if (!isCubeCompatible)
        {
            if (arrayLength == 1)
            {
                return MTLTextureType2D;
            }
            else if (arrayLength > 1)
            {
                return MTLTextureType2DArray;
            }
            else
            {
                return -1;
            }
        }
        else
        {
            if (arrayLength == 1)
            {
                return MTLTextureTypeCube;
            }
            else if (arrayLength > 1)
            {
                return MTLTextureTypeCubeArray;
            }
            else
            {
                return -1;
            }
        }
    }
    else if (neutraltype == TEXTURE_LOADER_TYPE_1D)
    {
        if (arrayLength == 1)
        {
            return MTLTextureType1D;
        }
        else if (arrayLength > 1)
        {
            return MTLTextureType1DArray;
        }
        else
        {
            return -1;
        }
    }
    else if (neutraltype == TEXTURE_LOADER_TYPE_3D)
    {
        if (arrayLength == 1)
        {
            return MTLTextureType3D;
        }
        else if (arrayLength > 1)
        {
            return -1;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
}

//--------------------------------------------------------------------------------------
#if __is_target_os(ios)
static MTLPixelFormat gNeutralToMetalFormatMap[] = {
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R4G4_UNORM_PACK8
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R4G4B4A4_UNORM_PACK16
    MTLPixelFormatABGR4Unorm,       //TEXTURE_LOADER_FORMAT_B4G4R4A4_UNORM_PACK16
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R5G6B5_UNORM_PACK16
    MTLPixelFormatB5G6R5Unorm,      //TEXTURE_LOADER_FORMAT_B5G6R5_UNORM_PACK16
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R5G5B5A1_UNORM_PACK16
    MTLPixelFormatBGR5A1Unorm,      //TEXTURE_LOADER_FORMAT_B5G5R5A1_UNORM_PACK16
    MTLPixelFormatA1BGR5Unorm,      //TEXTURE_LOADER_FORMAT_A1R5G5B5_UNORM_PACK16
    MTLPixelFormatR8Unorm,          //TEXTURE_LOADER_FORMAT_R8_UNORM
    MTLPixelFormatR8Snorm,          //TEXTURE_LOADER_FORMAT_R8_SNORM
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R8_USCALED
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R8_SSCALED
    MTLPixelFormatR8Uint,           //TEXTURE_LOADER_FORMAT_R8_UINT
    MTLPixelFormatR8Sint,           //TEXTURE_LOADER_FORMAT_R8_SINT
    MTLPixelFormatR8Unorm_sRGB,     //TEXTURE_LOADER_FORMAT_R8_SRGB
    MTLPixelFormatRG8Unorm,         //TEXTURE_LOADER_FORMAT_R8G8_UNORM
    MTLPixelFormatRG8Snorm,         //TEXTURE_LOADER_FORMAT_R8G8_SNORM
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R8G8_USCALED
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R8G8_SSCALED
    MTLPixelFormatRG8Uint,          //TEXTURE_LOADER_FORMAT_R8G8_UINT
    MTLPixelFormatRG8Sint,          //TEXTURE_LOADER_FORMAT_R8G8_SINT
    MTLPixelFormatRG8Unorm_sRGB,    //TEXTURE_LOADER_FORMAT_R8G8_SRGB
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R8G8B8_UNORM
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R8G8B8_SNORM
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R8G8B8_USCALED
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R8G8B8_SSCALED
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R8G8B8_UINT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R8G8B8_SINT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R8G8B8_SRGB
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_B8G8R8_UNORM
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_B8G8R8_SNORM
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_B8G8R8_USCALED
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_B8G8R8_SSCALED
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_B8G8R8_UINT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_B8G8R8_SINT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_B8G8R8_SRGB
    MTLPixelFormatRGBA8Unorm,       //TEXTURE_LOADER_FORMAT_R8G8B8A8_UNORM
    MTLPixelFormatRGBA8Snorm,       //TEXTURE_LOADER_FORMAT_R8G8B8A8_SNORM
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R8G8B8A8_USCALED
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R8G8B8A8_SSCALED
    MTLPixelFormatRGBA8Uint,        //TEXTURE_LOADER_FORMAT_R8G8B8A8_UINT
    MTLPixelFormatRGBA8Sint,        //TEXTURE_LOADER_FORMAT_R8G8B8A8_SINT
    MTLPixelFormatRGBA8Unorm_sRGB,  //TEXTURE_LOADER_FORMAT_R8G8B8A8_SRGB
    MTLPixelFormatBGRA8Unorm,       //TEXTURE_LOADER_FORMAT_B8G8R8A8_UNORM
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_B8G8R8A8_SNORM
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_B8G8R8A8_USCALED
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_B8G8R8A8_SSCALED
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_B8G8R8A8_UINT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_B8G8R8A8_SINT
    MTLPixelFormatBGRA8Unorm_sRGB,  //TEXTURE_LOADER_FORMAT_B8G8R8A8_SRGB
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_A8B8G8R8_UNORM_PACK32
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_A8B8G8R8_SNORM_PACK32
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_A8B8G8R8_USCALED_PACK32
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_A8B8G8R8_SSCALED_PACK32
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_A8B8G8R8_UINT_PACK32
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_A8B8G8R8_SINT_PACK32
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_A8B8G8R8_SRGB_PACK32
    MTLPixelFormatRGB10A2Unorm,     //TEXTURE_LOADER_FORMAT_A2R10G10B10_UNORM_PACK32
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_A2R10G10B10_SNORM_PACK32
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_A2R10G10B10_USCALED_PACK32
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_A2R10G10B10_SSCALED_PACK32
    MTLPixelFormatRGB10A2Uint,      //TEXTURE_LOADER_FORMAT_A2R10G10B10_UINT_PACK32
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_A2R10G10B10_SINT_PACK32
    MTLPixelFormatBGR10A2Unorm,     //TEXTURE_LOADER_FORMAT_A2B10G10R10_UNORM_PACK32
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_A2B10G10R10_SNORM_PACK32
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_A2B10G10R10_USCALED_PACK32
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_A2B10G10R10_SSCALED_PACK32
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_A2B10G10R10_UINT_PACK32
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_A2B10G10R10_SINT_PACK32
    MTLPixelFormatR16Unorm,         //TEXTURE_LOADER_FORMAT_R16_UNORM
    MTLPixelFormatR16Snorm,         //TEXTURE_LOADER_FORMAT_R16_SNORM
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R16_USCALED
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R16_SSCALED
    MTLPixelFormatR16Uint,          //TEXTURE_LOADER_FORMAT_R16_UINT
    MTLPixelFormatR16Sint,          //TEXTURE_LOADER_FORMAT_R16_SINT
    MTLPixelFormatR16Float,         //TEXTURE_LOADER_FORMAT_R16_SFLOAT
    MTLPixelFormatRG16Unorm,        //TEXTURE_LOADER_FORMAT_R16G16_UNORM
    MTLPixelFormatRG16Snorm,        //TEXTURE_LOADER_FORMAT_R16G16_SNORM
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R16G16_USCALED
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R16G16_SSCALED
    MTLPixelFormatRG16Uint,         //TEXTURE_LOADER_FORMAT_R16G16_UINT
    MTLPixelFormatRG16Sint,         //TEXTURE_LOADER_FORMAT_R16G16_SINT
    MTLPixelFormatRG16Float,        //TEXTURE_LOADER_FORMAT_R16G16_SFLOAT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R16G16B16_UNORM
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R16G16B16_SNORM
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R16G16B16_USCALED
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R16G16B16_SSCALED
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R16G16B16_UINT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R16G16B16_SINT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R16G16B16_SFLOAT
    MTLPixelFormatRGBA16Unorm,      //TEXTURE_LOADER_FORMAT_R16G16B16A16_UNORM
    MTLPixelFormatRGBA16Snorm,      //TEXTURE_LOADER_FORMAT_R16G16B16A16_SNORM
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R16G16B16A16_USCALED
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R16G16B16A16_SSCALED
    MTLPixelFormatRGBA16Uint,       //TEXTURE_LOADER_FORMAT_R16G16B16A16_UINT
    MTLPixelFormatRGBA16Sint,       //TEXTURE_LOADER_FORMAT_R16G16B16A16_SINT
    MTLPixelFormatRGBA16Float,      //TEXTURE_LOADER_FORMAT_R16G16B16A16_SFLOAT
    MTLPixelFormatR32Uint,          //TEXTURE_LOADER_FORMAT_R32_UINT
    MTLPixelFormatR32Sint,          //TEXTURE_LOADER_FORMAT_R32_SINT
    MTLPixelFormatR32Float,         //TEXTURE_LOADER_FORMAT_R32_SFLOAT
    MTLPixelFormatRG32Uint,         //TEXTURE_LOADER_FORMAT_R32G32_UINT
    MTLPixelFormatRG32Sint,         //TEXTURE_LOADER_FORMAT_R32G32_SINT
    MTLPixelFormatRG32Float,        //TEXTURE_LOADER_FORMAT_R32G32_SFLOAT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R32G32B32_UINT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R32G32B32_SINT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R32G32B32_SFLOAT
    MTLPixelFormatRGBA32Uint,       //TEXTURE_LOADER_FORMAT_R32G32B32A32_UINT
    MTLPixelFormatRGBA32Sint,       //TEXTURE_LOADER_FORMAT_R32G32B32A32_SINT
    MTLPixelFormatRGBA32Float,      //TEXTURE_LOADER_FORMAT_R32G32B32A32_SFLOAT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R64_UINT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R64_SINT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R64_SFLOAT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R64G64_UINT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R64G64_SINT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R64G64_SFLOAT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R64G64B64_UINT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R64G64B64_SINT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R64G64B64_SFLOAT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R64G64B64A64_UINT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R64G64B64A64_SINT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_R64G64B64A64_SFLOAT
    MTLPixelFormatRG11B10Float,     //TEXTURE_LOADER_FORMAT_B10G11R11_UFLOAT_PACK32
    MTLPixelFormatRGB9E5Float,      //TEXTURE_LOADER_FORMAT_E5B9G9R9_UFLOAT_PACK32
    MTLPixelFormatDepth16Unorm,     //TEXTURE_LOADER_FORMAT_D16_UNORM
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_X8_D24_UNORM_PACK32
    MTLPixelFormatDepth32Float,     //TEXTURE_LOADER_FORMAT_D32_SFLOAT
    MTLPixelFormatStencil8,         //TEXTURE_LOADER_FORMAT_S8_UINT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_D16_UNORM_S8_UINT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_D24_UNORM_S8_UINT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_D32_SFLOAT_S8_UINT
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_BC1_RGB_UNORM_BLOCK
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_BC1_RGB_SRGB_BLOCK
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_BC1_RGBA_UNORM_BLOCK
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_BC1_RGBA_SRGB_BLOCK
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_BC2_UNORM_BLOCK
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_BC2_SRGB_BLOCK
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_BC3_UNORM_BLOCK
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_BC3_SRGB_BLOCK
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_BC4_UNORM_BLOCK
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_BC4_SNORM_BLOCK
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_BC5_UNORM_BLOCK
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_BC5_SNORM_BLOCK
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_BC6H_UFLOAT_BLOCK
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_BC6H_SFLOAT_BLOCK
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_BC7_UNORM_BLOCK
    MTLPixelFormatInvalid,          //TEXTURE_LOADER_FORMAT_BC7_SRGB_BLOCK
    MTLPixelFormatETC2_RGB8,        //TEXTURE_LOADER_FORMAT_ETC2_R8G8B8_UNORM_BLOCK
    MTLPixelFormatETC2_RGB8_sRGB,   //TEXTURE_LOADER_FORMAT_ETC2_R8G8B8_SRGB_BLOCK
    MTLPixelFormatETC2_RGB8A1,      //TEXTURE_LOADER_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK
    MTLPixelFormatETC2_RGB8A1_sRGB, //TEXTURE_LOADER_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK
    MTLPixelFormatETC2_RGB8,        //TEXTURE_LOADER_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK
    MTLPixelFormatETC2_RGB8_sRGB,   //TEXTURE_LOADER_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK
    MTLPixelFormatEAC_R11Unorm,     //TEXTURE_LOADER_FORMAT_EAC_R11_UNORM_BLOCK
    MTLPixelFormatEAC_R11Snorm,     //TEXTURE_LOADER_FORMAT_EAC_R11_SNORM_BLOCK
    MTLPixelFormatEAC_RG11Unorm,    //TEXTURE_LOADER_FORMAT_EAC_R11G11_UNORM_BLOCK
    MTLPixelFormatEAC_RG11Snorm,    //TEXTURE_LOADER_FORMAT_EAC_R11G11_SNORM_BLOCK
    MTLPixelFormatASTC_4x4_LDR,     //TEXTURE_LOADER_FORMAT_ASTC_4x4_UNORM_BLOCK
    MTLPixelFormatASTC_4x4_sRGB,    //TEXTURE_LOADER_FORMAT_ASTC_4x4_SRGB_BLOCK
    MTLPixelFormatASTC_5x4_LDR,     //TEXTURE_LOADER_FORMAT_ASTC_5x4_UNORM_BLOCK
    MTLPixelFormatASTC_5x4_sRGB,    //TEXTURE_LOADER_FORMAT_ASTC_5x4_SRGB_BLOCK
    MTLPixelFormatASTC_5x5_LDR,     //TEXTURE_LOADER_FORMAT_ASTC_5x5_UNORM_BLOCK
    MTLPixelFormatASTC_5x5_sRGB,    //TEXTURE_LOADER_FORMAT_ASTC_5x5_SRGB_BLOCK
    MTLPixelFormatASTC_6x5_LDR,     //TEXTURE_LOADER_FORMAT_ASTC_6x5_UNORM_BLOCK
    MTLPixelFormatASTC_6x5_sRGB,    //TEXTURE_LOADER_FORMAT_ASTC_6x5_SRGB_BLOCK
    MTLPixelFormatASTC_6x6_LDR,     //TEXTURE_LOADER_FORMAT_ASTC_6x6_UNORM_BLOCK
    MTLPixelFormatASTC_6x6_sRGB,    //TEXTURE_LOADER_FORMAT_ASTC_6x6_SRGB_BLOCK
    MTLPixelFormatASTC_8x5_LDR,     //TEXTURE_LOADER_FORMAT_ASTC_8x5_UNORM_BLOCK
    MTLPixelFormatASTC_8x5_sRGB,    //TEXTURE_LOADER_FORMAT_ASTC_8x5_SRGB_BLOCK
    MTLPixelFormatASTC_8x6_LDR,     //TEXTURE_LOADER_FORMAT_ASTC_8x6_UNORM_BLOCK
    MTLPixelFormatASTC_8x6_sRGB,    //TEXTURE_LOADER_FORMAT_ASTC_8x6_SRGB_BLOCK
    MTLPixelFormatASTC_8x8_LDR,     //TEXTURE_LOADER_FORMAT_ASTC_8x8_UNORM_BLOCK
    MTLPixelFormatASTC_8x8_sRGB,    //TEXTURE_LOADER_FORMAT_ASTC_8x8_SRGB_BLOCK
    MTLPixelFormatASTC_10x5_LDR,    //TEXTURE_LOADER_FORMAT_ASTC_10x5_UNORM_BLOCK
    MTLPixelFormatASTC_10x5_sRGB,   //TEXTURE_LOADER_FORMAT_ASTC_10x5_SRGB_BLOCK
    MTLPixelFormatASTC_10x6_LDR,    //TEXTURE_LOADER_FORMAT_ASTC_10x6_UNORM_BLOCK
    MTLPixelFormatASTC_10x6_sRGB,   //TEXTURE_LOADER_FORMAT_ASTC_10x6_SRGB_BLOCK
    MTLPixelFormatASTC_10x8_LDR,    //TEXTURE_LOADER_FORMAT_ASTC_10x8_UNORM_BLOCK
    MTLPixelFormatASTC_10x8_sRGB,   //TEXTURE_LOADER_FORMAT_ASTC_10x8_SRGB_BLOCK
    MTLPixelFormatASTC_10x10_LDR,   //TEXTURE_LOADER_FORMAT_ASTC_10x10_UNORM_BLOCK
    MTLPixelFormatASTC_10x10_sRGB,  //TEXTURE_LOADER_FORMAT_ASTC_10x10_SRGB_BLOCK
    MTLPixelFormatASTC_12x10_LDR,   //TEXTURE_LOADER_FORMAT_ASTC_12x10_UNORM_BLOCK
    MTLPixelFormatASTC_12x10_sRGB,  //TEXTURE_LOADER_FORMAT_ASTC_12x10_SRGB_BLOCK
    MTLPixelFormatASTC_12x12_LDR,   //TEXTURE_LOADER_FORMAT_ASTC_12x12_UNORM_BLOCK
    MTLPixelFormatASTC_12x12_sRGB,  //TEXTURE_LOADER_FORMAT_ASTC_12x12_SRGB_BLOCK
};
#elif __is_target_os(macos)
static MTLPixelFormat gNeutralToMetalFormatMap[] = {
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R4G4_UNORM_PACK8
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R4G4B4A4_UNORM_PACK16
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_B4G4R4A4_UNORM_PACK16
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R5G6B5_UNORM_PACK16
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_B5G6R5_UNORM_PACK16
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R5G5B5A1_UNORM_PACK16
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_B5G5R5A1_UNORM_PACK16
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_A1R5G5B5_UNORM_PACK16
    MTLPixelFormatR8Unorm,               //TEXTURE_LOADER_FORMAT_R8_UNORM
    MTLPixelFormatR8Snorm,               //TEXTURE_LOADER_FORMAT_R8_SNORM
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R8_USCALED
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R8_SSCALED
    MTLPixelFormatR8Uint,                //TEXTURE_LOADER_FORMAT_R8_UINT
    MTLPixelFormatR8Sint,                //TEXTURE_LOADER_FORMAT_R8_SINT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R8_SRGB
    MTLPixelFormatRG8Unorm,              //TEXTURE_LOADER_FORMAT_R8G8_UNORM
    MTLPixelFormatRG8Snorm,              //TEXTURE_LOADER_FORMAT_R8G8_SNORM
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R8G8_USCALED
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R8G8_SSCALED
    MTLPixelFormatRG8Uint,               //TEXTURE_LOADER_FORMAT_R8G8_UINT
    MTLPixelFormatRG8Sint,               //TEXTURE_LOADER_FORMAT_R8G8_SINT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R8G8_SRGB
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R8G8B8_UNORM
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R8G8B8_SNORM
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R8G8B8_USCALED
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R8G8B8_SSCALED
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R8G8B8_UINT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R8G8B8_SINT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R8G8B8_SRGB
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_B8G8R8_UNORM
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_B8G8R8_SNORM
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_B8G8R8_USCALED
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_B8G8R8_SSCALED
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_B8G8R8_UINT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_B8G8R8_SINT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_B8G8R8_SRGB
    MTLPixelFormatRGBA8Unorm,            //TEXTURE_LOADER_FORMAT_R8G8B8A8_UNORM
    MTLPixelFormatRGBA8Snorm,            //TEXTURE_LOADER_FORMAT_R8G8B8A8_SNORM
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R8G8B8A8_USCALED
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R8G8B8A8_SSCALED
    MTLPixelFormatRGBA8Uint,             //TEXTURE_LOADER_FORMAT_R8G8B8A8_UINT
    MTLPixelFormatRGBA8Sint,             //TEXTURE_LOADER_FORMAT_R8G8B8A8_SINT
    MTLPixelFormatRGBA8Unorm_sRGB,       //TEXTURE_LOADER_FORMAT_R8G8B8A8_SRGB
    MTLPixelFormatBGRA8Unorm,            //TEXTURE_LOADER_FORMAT_B8G8R8A8_UNORM
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_B8G8R8A8_SNORM
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_B8G8R8A8_USCALED
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_B8G8R8A8_SSCALED
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_B8G8R8A8_UINT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_B8G8R8A8_SINT
    MTLPixelFormatBGRA8Unorm_sRGB,       //TEXTURE_LOADER_FORMAT_B8G8R8A8_SRGB
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_A8B8G8R8_UNORM_PACK32
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_A8B8G8R8_SNORM_PACK32
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_A8B8G8R8_USCALED_PACK32
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_A8B8G8R8_SSCALED_PACK32
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_A8B8G8R8_UINT_PACK32
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_A8B8G8R8_SINT_PACK32
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_A8B8G8R8_SRGB_PACK32
    MTLPixelFormatRGB10A2Unorm,          //TEXTURE_LOADER_FORMAT_A2R10G10B10_UNORM_PACK32
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_A2R10G10B10_SNORM_PACK32
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_A2R10G10B10_USCALED_PACK32
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_A2R10G10B10_SSCALED_PACK32
    MTLPixelFormatRGB10A2Uint,           //TEXTURE_LOADER_FORMAT_A2R10G10B10_UINT_PACK32
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_A2R10G10B10_SINT_PACK32
    MTLPixelFormatBGR10A2Unorm,          //TEXTURE_LOADER_FORMAT_A2B10G10R10_UNORM_PACK32
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_A2B10G10R10_SNORM_PACK32
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_A2B10G10R10_USCALED_PACK32
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_A2B10G10R10_SSCALED_PACK32
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_A2B10G10R10_UINT_PACK32
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_A2B10G10R10_SINT_PACK32
    MTLPixelFormatR16Unorm,              //TEXTURE_LOADER_FORMAT_R16_UNORM
    MTLPixelFormatR16Snorm,              //TEXTURE_LOADER_FORMAT_R16_SNORM
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R16_USCALED
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R16_SSCALED
    MTLPixelFormatR16Uint,               //TEXTURE_LOADER_FORMAT_R16_UINT
    MTLPixelFormatR16Sint,               //TEXTURE_LOADER_FORMAT_R16_SINT
    MTLPixelFormatR16Float,              //TEXTURE_LOADER_FORMAT_R16_SFLOAT
    MTLPixelFormatRG16Unorm,             //TEXTURE_LOADER_FORMAT_R16G16_UNORM
    MTLPixelFormatRG16Snorm,             //TEXTURE_LOADER_FORMAT_R16G16_SNORM
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R16G16_USCALED
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R16G16_SSCALED
    MTLPixelFormatRG16Uint,              //TEXTURE_LOADER_FORMAT_R16G16_UINT
    MTLPixelFormatRG16Sint,              //TEXTURE_LOADER_FORMAT_R16G16_SINT
    MTLPixelFormatRG16Float,             //TEXTURE_LOADER_FORMAT_R16G16_SFLOAT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R16G16B16_UNORM
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R16G16B16_SNORM
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R16G16B16_USCALED
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R16G16B16_SSCALED
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R16G16B16_UINT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R16G16B16_SINT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R16G16B16_SFLOAT
    MTLPixelFormatRGBA16Unorm,           //TEXTURE_LOADER_FORMAT_R16G16B16A16_UNORM
    MTLPixelFormatRGBA16Snorm,           //TEXTURE_LOADER_FORMAT_R16G16B16A16_SNORM
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R16G16B16A16_USCALED
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R16G16B16A16_SSCALED
    MTLPixelFormatRGBA16Uint,            //TEXTURE_LOADER_FORMAT_R16G16B16A16_UINT
    MTLPixelFormatRGBA16Sint,            //TEXTURE_LOADER_FORMAT_R16G16B16A16_SINT
    MTLPixelFormatRGBA16Float,           //TEXTURE_LOADER_FORMAT_R16G16B16A16_SFLOAT
    MTLPixelFormatR32Uint,               //TEXTURE_LOADER_FORMAT_R32_UINT
    MTLPixelFormatR32Sint,               //TEXTURE_LOADER_FORMAT_R32_SINT
    MTLPixelFormatR32Float,              //TEXTURE_LOADER_FORMAT_R32_SFLOAT
    MTLPixelFormatRG32Uint,              //TEXTURE_LOADER_FORMAT_R32G32_UINT
    MTLPixelFormatRG32Sint,              //TEXTURE_LOADER_FORMAT_R32G32_SINT
    MTLPixelFormatRG32Float,             //TEXTURE_LOADER_FORMAT_R32G32_SFLOAT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R32G32B32_UINT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R32G32B32_SINT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R32G32B32_SFLOAT
    MTLPixelFormatRGBA32Uint,            //TEXTURE_LOADER_FORMAT_R32G32B32A32_UINT
    MTLPixelFormatRGBA32Sint,            //TEXTURE_LOADER_FORMAT_R32G32B32A32_SINT
    MTLPixelFormatRGBA32Float,           //TEXTURE_LOADER_FORMAT_R32G32B32A32_SFLOAT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R64_UINT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R64_SINT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R64_SFLOAT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R64G64_UINT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R64G64_SINT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R64G64_SFLOAT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R64G64B64_UINT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R64G64B64_SINT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R64G64B64_SFLOAT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R64G64B64A64_UINT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R64G64B64A64_SINT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_R64G64B64A64_SFLOAT
    MTLPixelFormatRG11B10Float,          //TEXTURE_LOADER_FORMAT_B10G11R11_UFLOAT_PACK32
    MTLPixelFormatRGB9E5Float,           //TEXTURE_LOADER_FORMAT_E5B9G9R9_UFLOAT_PACK32
    MTLPixelFormatDepth16Unorm,          //TEXTURE_LOADER_FORMAT_D16_UNORM
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_X8_D24_UNORM_PACK32
    MTLPixelFormatDepth32Float,          //TEXTURE_LOADER_FORMAT_D32_SFLOAT
    MTLPixelFormatStencil8,              //TEXTURE_LOADER_FORMAT_S8_UINT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_D16_UNORM_S8_UINT
    MTLPixelFormatDepth24Unorm_Stencil8, //TEXTURE_LOADER_FORMAT_D24_UNORM_S8_UINT
    MTLPixelFormatDepth32Float_Stencil8, //TEXTURE_LOADER_FORMAT_D32_SFLOAT_S8_UINT
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_BC1_RGB_UNORM_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_BC1_RGB_SRGB_BLOCK
    MTLPixelFormatBC1_RGBA,              //TEXTURE_LOADER_FORMAT_BC1_RGBA_UNORM_BLOCK
    MTLPixelFormatBC1_RGBA_sRGB,         //TEXTURE_LOADER_FORMAT_BC1_RGBA_SRGB_BLOCK
    MTLPixelFormatBC2_RGBA,              //TEXTURE_LOADER_FORMAT_BC2_UNORM_BLOCK
    MTLPixelFormatBC2_RGBA_sRGB,         //TEXTURE_LOADER_FORMAT_BC2_SRGB_BLOCK
    MTLPixelFormatBC3_RGBA,              //TEXTURE_LOADER_FORMAT_BC3_UNORM_BLOCK
    MTLPixelFormatBC3_RGBA_sRGB,         //TEXTURE_LOADER_FORMAT_BC3_SRGB_BLOCK
    MTLPixelFormatBC4_RUnorm,            //TEXTURE_LOADER_FORMAT_BC4_UNORM_BLOCK
    MTLPixelFormatBC4_RSnorm,            //TEXTURE_LOADER_FORMAT_BC4_SNORM_BLOCK
    MTLPixelFormatBC5_RGUnorm,           //TEXTURE_LOADER_FORMAT_BC5_UNORM_BLOCK
    MTLPixelFormatBC5_RGSnorm,           //TEXTURE_LOADER_FORMAT_BC5_SNORM_BLOCK
    MTLPixelFormatBC6H_RGBFloat,         //TEXTURE_LOADER_FORMAT_BC6H_UFLOAT_BLOCK
    MTLPixelFormatBC6H_RGBUfloat,        //TEXTURE_LOADER_FORMAT_BC6H_SFLOAT_BLOCK
    MTLPixelFormatBC7_RGBAUnorm,         //TEXTURE_LOADER_FORMAT_BC7_UNORM_BLOCK
    MTLPixelFormatBC7_RGBAUnorm_sRGB,    //TEXTURE_LOADER_FORMAT_BC7_SRGB_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ETC2_R8G8B8_UNORM_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ETC2_R8G8B8_SRGB_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_EAC_R11_UNORM_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_EAC_R11_SNORM_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_EAC_R11G11_UNORM_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_EAC_R11G11_SNORM_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_4x4_UNORM_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_4x4_SRGB_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_5x4_UNORM_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_5x4_SRGB_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_5x5_UNORM_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_5x5_SRGB_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_6x5_UNORM_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_6x5_SRGB_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_6x6_UNORM_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_6x6_SRGB_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_8x5_UNORM_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_8x5_SRGB_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_8x6_UNORM_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_8x6_SRGB_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_8x8_UNORM_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_8x8_SRGB_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_10x5_UNORM_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_10x5_SRGB_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_10x6_UNORM_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_10x6_SRGB_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_10x8_UNORM_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_10x8_SRGB_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_10x10_UNORM_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_10x10_SRGB_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_12x10_UNORM_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_12x10_SRGB_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_12x12_UNORM_BLOCK
    MTLPixelFormatInvalid,               //TEXTURE_LOADER_FORMAT_ASTC_12x12_SRGB_BLOCK
};
#else
#error Unknown Target
#endif
static_assert(TEXTURE_LOADER_FORMAT_RANGE_SIZE == (sizeof(gNeutralToMetalFormatMap) / sizeof(gNeutralToMetalFormatMap[0])), "gNeutralToMetalFormatMap may not match!");
static inline MTLPixelFormat _GetMetalFormat(uint32_t neutralformat)
{
    assert(neutralformat < (sizeof(gNeutralToMetalFormatMap) / sizeof(gNeutralToMetalFormatMap[0])));
    return gNeutralToMetalFormatMap[neutralformat];
}

//--------------------------------------------------------------------------------------
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

static struct _FormatInfo const gMetelFormatInfoTable[] = {
    {0},                                       //MTLPixelFormatInvalid = 0
    {1, .rgba = {1}},                          //MTLPixelFormatA8Unorm = 1
    {0},                                       //MTLPixelFormat_2
    {0},                                       //MTLPixelFormat_3
    {0},                                       //MTLPixelFormat_4
    {0},                                       //MTLPixelFormat_5
    {0},                                       //MTLPixelFormat_6
    {0},                                       //MTLPixelFormat_7
    {0},                                       //MTLPixelFormat_8
    {0},                                       //MTLPixelFormat_9
    {1, .rgba = {1}},                          //MTLPixelFormatR8Unorm = 10
    {1, .rgba = {1}},                          //MTLPixelFormatR8Unorm_sRGB = 11
    {1, .rgba = {1}},                          //MTLPixelFormatR8Snorm = 12
    {1, .rgba = {1}},                          //MTLPixelFormatR8Uint = 13
    {1, .rgba = {1}},                          //MTLPixelFormatR8Sint = 14
    {0},                                       //MTLPixelFormat_15
    {0},                                       //MTLPixelFormat_16
    {0},                                       //MTLPixelFormat_17
    {0},                                       //MTLPixelFormat_18
    {0},                                       //MTLPixelFormat_19
    {1, .rgba = {2}},                          //MTLPixelFormatR16Unorm = 20
    {0},                                       //MTLPixelFormat_21
    {1, .rgba = {2}},                          //MTLPixelFormatR16Snorm = 22
    {1, .rgba = {2}},                          //MTLPixelFormatR16Uint = 23
    {1, .rgba = {2}},                          //MTLPixelFormatR16Sint = 24
    {1, .rgba = {2}},                          //MTLPixelFormatR16Float = 25
    {0},                                       //MTLPixelFormat_26
    {0},                                       //MTLPixelFormat_27
    {0},                                       //MTLPixelFormat_28
    {0},                                       //MTLPixelFormat_29
    {1, .rgba = {2}},                          //MTLPixelFormatRG8Unorm = 30
    {1, .rgba = {2}},                          //MTLPixelFormatRG8Unorm_sRGB = 31
    {1, .rgba = {2}},                          //MTLPixelFormatRG8Snorm = 32
    {1, .rgba = {2}},                          //MTLPixelFormatRG8Uint = 33
    {1, .rgba = {2}},                          //MTLPixelFormatRG8Sint = 34
    {0},                                       //MTLPixelFormat_35
    {0},                                       //MTLPixelFormat_36
    {0},                                       //MTLPixelFormat_37
    {0},                                       //MTLPixelFormat_38
    {0},                                       //MTLPixelFormat_39
    {1, .rgba = {2}},                          //MTLPixelFormatB5G6R5Unorm = 40
    {1, .rgba = {2}},                          //MTLPixelFormatA1BGR5Unorm = 41
    {1, .rgba = {2}},                          //MTLPixelFormatABGR4Unorm = 42
    {1, .rgba = {2}},                          //MTLPixelFormatBGR5A1Unorm = 43
    {0},                                       //MTLPixelFormat_44
    {0},                                       //MTLPixelFormat_45
    {0},                                       //MTLPixelFormat_46
    {0},                                       //MTLPixelFormat_47
    {0},                                       //MTLPixelFormat_48
    {0},                                       //MTLPixelFormat_49
    {0},                                       //MTLPixelFormat_50
    {0},                                       //MTLPixelFormat_51
    {0},                                       //MTLPixelFormat_52
    {1, .rgba = {4}},                          //MTLPixelFormatR32Uint = 53
    {1, .rgba = {4}},                          //MTLPixelFormatR32Sint = 54
    {1, .rgba = {4}},                          //MTLPixelFormatR32Float = 55
    {0},                                       //MTLPixelFormat_56
    {0},                                       //MTLPixelFormat_57
    {0},                                       //MTLPixelFormat_58
    {0},                                       //MTLPixelFormat_59
    {1, .rgba = {4}},                          //MTLPixelFormatRG16Unorm = 60
    {0},                                       //MTLPixelFormat_61
    {1, .rgba = {4}},                          //MTLPixelFormatRG16Snorm = 62
    {1, .rgba = {4}},                          //MTLPixelFormatRG16Uint = 63
    {1, .rgba = {4}},                          //MTLPixelFormatRG16Sint = 64
    {1, .rgba = {4}},                          //MTLPixelFormatRG16Float = 65
    {0},                                       //MTLPixelFormat_66
    {0},                                       //MTLPixelFormat_67
    {0},                                       //MTLPixelFormat_68
    {0},                                       //MTLPixelFormat_69
    {1, .rgba = {4}},                          //MTLPixelFormatRGBA8Unorm = 70
    {1, .rgba = {4}},                          //MTLPixelFormatRGBA8Unorm_sRGB = 71
    {1, .rgba = {4}},                          //MTLPixelFormatRGBA8Snorm = 72
    {1, .rgba = {4}},                          //MTLPixelFormatRGBA8Uint = 73
    {1, .rgba = {4}},                          //MTLPixelFormatRGBA8Sint = 74
    {0},                                       //MTLPixelFormat_75
    {0},                                       //MTLPixelFormat_76
    {0},                                       //MTLPixelFormat_77
    {0},                                       //MTLPixelFormat_78
    {0},                                       //MTLPixelFormat_79
    {1, .rgba = {4}},                          //MTLPixelFormatBGRA8Unorm = 80
    {1, .rgba = {4}},                          //MTLPixelFormatBGRA8Unorm_sRGB = 81
    {0},                                       //MTLPixelFormat_82
    {0},                                       //MTLPixelFormat_83
    {0},                                       //MTLPixelFormat_84
    {0},                                       //MTLPixelFormat_85
    {0},                                       //MTLPixelFormat_86
    {0},                                       //MTLPixelFormat_87
    {0},                                       //MTLPixelFormat_88
    {0},                                       //MTLPixelFormat_89
    {1, .rgba = {4}},                          //MTLPixelFormatRGB10A2Unorm = 90
    {1, .rgba = {4}},                          //MTLPixelFormatRGB10A2Uint = 91
    {1, .rgba = {4}},                          //MTLPixelFormatRG11B10Float = 92
    {1, .rgba = {4}},                          //MTLPixelFormatRGB9E5Float = 93
    {1, .rgba = {4}},                          //MTLPixelFormatBGR10A2Unorm = 94
    {0},                                       //MTLPixelFormat_95
    {0},                                       //MTLPixelFormat_96
    {0},                                       //MTLPixelFormat_97
    {0},                                       //MTLPixelFormat_98
    {0},                                       //MTLPixelFormat_99
    {0},                                       //MTLPixelFormat_100
    {0},                                       //MTLPixelFormat_101
    {0},                                       //MTLPixelFormat_102
    {1, .rgba = {8}},                          //MTLPixelFormatRG32Uint = 103
    {1, .rgba = {8}},                          //MTLPixelFormatRG32Sint = 104
    {1, .rgba = {8}},                          //MTLPixelFormatRG32Float = 105
    {0},                                       //MTLPixelFormat_106
    {0},                                       //MTLPixelFormat_107
    {0},                                       //MTLPixelFormat_108
    {0},                                       //MTLPixelFormat_109
    {1, .rgba = {8}},                          //MTLPixelFormatRGBA16Unorm = 110
    {0},                                       //MTLPixelFormat_111
    {1, .rgba = {8}},                          //MTLPixelFormatRGBA16Snorm = 112
    {1, .rgba = {8}},                          //MTLPixelFormatRGBA16Uint = 113
    {1, .rgba = {8}},                          //MTLPixelFormatRGBA16Sint = 114
    {1, .rgba = {8}},                          //MTLPixelFormatRGBA16Float = 115
    {0},                                       //MTLPixelFormat_116
    {0},                                       //MTLPixelFormat_117
    {0},                                       //MTLPixelFormat_118
    {0},                                       //MTLPixelFormat_119
    {0},                                       //MTLPixelFormat_120
    {0},                                       //MTLPixelFormat_121
    {0},                                       //MTLPixelFormat_122
    {1, .rgba = {16}},                         //MTLPixelFormatRGBA32Uint = 123
    {1, .rgba = {16}},                         //MTLPixelFormatRGBA32Sint = 124
    {1, .rgba = {16}},                         //MTLPixelFormatRGBA32Float = 125
    {0},                                       //MTLPixelFormat_126
    {0},                                       //MTLPixelFormat_127
    {0},                                       //MTLPixelFormat_128
    {0},                                       //MTLPixelFormat_129
    {3, .compressed = {4, 4, 1, (64 / 8)}},    //MTLPixelFormatBC1_RGBA = 130
    {3, .compressed = {4, 4, 1, (64 / 8)}},    //MTLPixelFormatBC1_RGBA_sRGB = 131
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //MTLPixelFormatBC2_RGBA = 132
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //MTLPixelFormatBC2_RGBA_sRGB = 133
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //MTLPixelFormatBC3_RGBA = 134
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //MTLPixelFormatBC3_RGBA_sRGB = 135
    {0},                                       //MTLPixelFormat_136
    {0},                                       //MTLPixelFormat_137
    {0},                                       //MTLPixelFormat_138
    {0},                                       //MTLPixelFormat_139
    {3, .compressed = {4, 4, 1, (64 / 8)}},    //MTLPixelFormatBC4_RUnorm = 140
    {3, .compressed = {4, 4, 1, (64 / 8)}},    //MTLPixelFormatBC4_RSnorm = 141
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //MTLPixelFormatBC5_RGUnorm = 142
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //MTLPixelFormatBC5_RGSnorm = 143
    {0},                                       //MTLPixelFormat_144
    {0},                                       //MTLPixelFormat_145
    {0},                                       //MTLPixelFormat_146
    {0},                                       //MTLPixelFormat_147
    {0},                                       //MTLPixelFormat_148
    {0},                                       //MTLPixelFormat_149
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //MTLPixelFormatBC6H_RGBFloat = 150
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //MTLPixelFormatBC6H_RGBUfloat = 151
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //MTLPixelFormatBC7_RGBAUnorm = 152
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //MTLPixelFormatBC7_RGBAUnorm_sRGB = 153
    {0},                                       //MTLPixelFormat_154
    {0},                                       //MTLPixelFormat_155
    {0},                                       //MTLPixelFormat_156
    {0},                                       //MTLPixelFormat_157
    {0},                                       //MTLPixelFormat_158
    {0},                                       //MTLPixelFormat_159
    {0},                                       //MTLPixelFormatPVRTC_RGB_2BPP = 160
    {0},                                       //MTLPixelFormatPVRTC_RGB_2BPP_sRGB = 161
    {0},                                       //MTLPixelFormatPVRTC_RGB_4BPP = 162
    {0},                                       //MTLPixelFormatPVRTC_RGB_4BPP_sRGB = 163
    {0},                                       //MTLPixelFormatPVRTC_RGBA_2BPP = 164
    {0},                                       //MTLPixelFormatPVRTC_RGBA_2BPP_sRGB = 165
    {0},                                       //MTLPixelFormatPVRTC_RGBA_4BPP = 166
    {0},                                       //MTLPixelFormatPVRTC_RGBA_4BPP_sRGB = 167
    {0},                                       //MTLPixelFormat_168
    {0},                                       //MTLPixelFormat_169
    {3, .compressed = {4, 4, 1, (64 / 8)}},    //MTLPixelFormatEAC_R11Unorm = 170
    {0},                                       //MTLPixelFormat_171
    {3, .compressed = {4, 4, 1, (64 / 8)}},    //MTLPixelFormatEAC_R11Snorm = 172
    {0},                                       //MTLPixelFormat_173
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //MTLPixelFormatEAC_RG11Unorm = 174
    {0},                                       //MTLPixelFormat_175
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //MTLPixelFormatEAC_RG11Snorm = 176
    {0},                                       //MTLPixelFormat_177
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //MTLPixelFormatEAC_RGBA8 = 178
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //MTLPixelFormatEAC_RGBA8_sRGB = 179
    {3, .compressed = {4, 4, 1, (64 / 8)}},    //MTLPixelFormatETC2_RGB8 = 180
    {3, .compressed = {4, 4, 1, (64 / 8)}},    //MTLPixelFormatETC2_RGB8_sRGB = 181
    {3, .compressed = {4, 4, 1, (64 / 8)}},    //MTLPixelFormatETC2_RGB8A1 = 182
    {3, .compressed = {4, 4, 1, (64 / 8)}},    //MTLPixelFormatETC2_RGB8A1_sRGB = 183
    {0},                                       //MTLPixelFormat_184
    {0},                                       //MTLPixelFormat_185
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //MTLPixelFormatASTC_4x4_sRGB = 186
    {3, .compressed = {5, 4, 1, (128 / 8)}},   //MTLPixelFormatASTC_5x4_sRGB = 187
    {3, .compressed = {5, 5, 1, (128 / 8)}},   //MTLPixelFormatASTC_5x5_sRGB = 188
    {3, .compressed = {6, 5, 1, (128 / 8)}},   //MTLPixelFormatASTC_6x5_sRGB = 189
    {3, .compressed = {6, 6, 1, (128 / 8)}},   //MTLPixelFormatASTC_6x6_sRGB = 190
    {0},                                       //MTLPixelFormat_191
    {3, .compressed = {8, 5, 1, (128 / 8)}},   //MTLPixelFormatASTC_8x5_sRGB = 192
    {3, .compressed = {8, 6, 1, (128 / 8)}},   //MTLPixelFormatASTC_8x6_sRGB = 193
    {3, .compressed = {8, 8, 1, (128 / 8)}},   //MTLPixelFormatASTC_8x8_sRGB = 194
    {3, .compressed = {10, 5, 1, (128 / 8)}},  //MTLPixelFormatASTC_10x5_sRGB = 195
    {3, .compressed = {10, 6, 1, (128 / 8)}},  //MTLPixelFormatASTC_10x6_sRGB = 196
    {3, .compressed = {10, 8, 1, (128 / 8)}},  //MTLPixelFormatASTC_10x8_sRGB = 197
    {3, .compressed = {10, 10, 1, (128 / 8)}}, //MTLPixelFormatASTC_10x10_sRGB = 198
    {3, .compressed = {12, 10, 1, (128 / 8)}}, //MTLPixelFormatASTC_12x10_sRGB = 199
    {3, .compressed = {12, 12, 1, (128 / 8)}}, //MTLPixelFormatASTC_12x12_sRGB = 200
    {0},                                       //MTLPixelFormat_201
    {0},                                       //MTLPixelFormat_202
    {0},                                       //MTLPixelFormat_203
    {3, .compressed = {4, 4, 1, (128 / 8)}},   //MTLPixelFormatASTC_4x4_LDR = 204
    {3, .compressed = {5, 4, 1, (128 / 8)}},   //MTLPixelFormatASTC_5x4_LDR = 205
    {3, .compressed = {5, 5, 1, (128 / 8)}},   //MTLPixelFormatASTC_5x5_LDR = 206
    {3, .compressed = {6, 5, 1, (128 / 8)}},   //MTLPixelFormatASTC_6x5_LDR = 207
    {3, .compressed = {6, 6, 1, (128 / 8)}},   //MTLPixelFormatASTC_6x6_LDR = 208
    {0},                                       //MTLPixelFormat_209
    {3, .compressed = {8, 5, 1, (128 / 8)}},   //MTLPixelFormatASTC_8x5_LDR = 210
    {3, .compressed = {8, 6, 1, (128 / 8)}},   //MTLPixelFormatASTC_8x6_LDR = 211
    {3, .compressed = {8, 8, 1, (128 / 8)}},   //MTLPixelFormatASTC_8x8_LDR = 212
    {3, .compressed = {10, 5, 1, (128 / 8)}},  //MTLPixelFormatASTC_10x5_LDR = 213
    {3, .compressed = {10, 6, 1, (128 / 8)}},  //MTLPixelFormatASTC_10x6_LDR = 214
    {3, .compressed = {10, 8, 1, (128 / 8)}},  //MTLPixelFormatASTC_10x8_LDR = 215
    {3, .compressed = {10, 10, 1, (128 / 8)}}, //MTLPixelFormatASTC_10x10_LDR = 216
    {3, .compressed = {12, 10, 1, (128 / 8)}}, //MTLPixelFormatASTC_12x10_LDR = 217
    {3, .compressed = {12, 12, 1, (128 / 8)}}, //MTLPixelFormatASTC_12x12_LDR = 218
    //{0}, //MTLPixelFormatGBGR422 = 240
    //{0}, //MTLPixelFormatBGRG422 = 241
    //{0}, //MTLPixelFormatDepth16Unorm = 250
    //{0}, //MTLPixelFormatDepth32Float = 252
    //{0}, //MTLPixelFormatStencil8 = 253
    //{0}, //MTLPixelFormatDepth24Unorm_Stencil8 = 255,
    //{0}, //MTLPixelFormatDepth32Float_Stencil8  = 260,
    //{0}, //MTLPixelFormatX32_Stencil8 = 261
    //{0}, //MTLPixelFormatBGRA10_XR  = 552
    //{0}, //MTLPixelFormatBGRA10_XR_sRGB  = 553
    //{0}, //MTLPixelFormatBGR10_XR  = 554
    //{0}, //MTLPixelFormatBGR10_XR_sRGB  = 555
};
static_assert(219 == (sizeof(gMetelFormatInfoTable) / sizeof(gMetelFormatInfoTable[0])), "gMetelFormatInfoTable may not match!");

static inline uint32_t _GetFormatAspectCount(MTLPixelFormat mtlformat)
{
    assert(mtlformat < (sizeof(gMetelFormatInfoTable) / sizeof(gMetelFormatInfoTable[0])));

    _FormatInfo vk_format_info = gMetelFormatInfoTable[mtlformat];

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

static inline bool _IsFormatCompressed(MTLPixelFormat mtlformat)
{
    assert(mtlformat < (sizeof(gMetelFormatInfoTable) / sizeof(gMetelFormatInfoTable[0])));

    _FormatInfo vk_format_info = gMetelFormatInfoTable[mtlformat];

    return (vk_format_info._union_tag == 3);
}

static inline bool _IsFormatRGBA(MTLPixelFormat mtlformat)
{
    assert(mtlformat < (sizeof(gMetelFormatInfoTable) / sizeof(gMetelFormatInfoTable[0])));

    _FormatInfo vk_format_info = gMetelFormatInfoTable[mtlformat];

    return (vk_format_info._union_tag == 1);
}

static inline bool _IsFormatDepthStencil(MTLPixelFormat mtlformat)
{
    assert(mtlformat < (sizeof(gMetelFormatInfoTable) / sizeof(gMetelFormatInfoTable[0])));

    _FormatInfo vk_format_info = gMetelFormatInfoTable[mtlformat];

    return (vk_format_info._union_tag == 2);
}

static inline uint32_t _GetCompressedFormatBlockWidth(MTLPixelFormat mtlformat)
{
    assert(_IsFormatCompressed(mtlformat));

    assert(mtlformat < (sizeof(gMetelFormatInfoTable) / sizeof(gMetelFormatInfoTable[0])));

    _FormatInfo vk_format_info = gMetelFormatInfoTable[mtlformat];

    return vk_format_info.compressed.compressedBlockWidth;
}

static inline uint32_t _GetCompressedFormatBlockHeight(MTLPixelFormat mtlformat)
{
    assert(_IsFormatCompressed(mtlformat));

    assert(mtlformat < (sizeof(gMetelFormatInfoTable) / sizeof(gMetelFormatInfoTable[0])));

    _FormatInfo vk_format_info = gMetelFormatInfoTable[mtlformat];

    return vk_format_info.compressed.compressedBlockHeight;
}

static inline uint32_t _GetCompressedFormatBlockDepth(MTLPixelFormat mtlformat)
{
    assert(_IsFormatCompressed(mtlformat));

    assert(mtlformat < (sizeof(gMetelFormatInfoTable) / sizeof(gMetelFormatInfoTable[0])));

    _FormatInfo vk_format_info = gMetelFormatInfoTable[mtlformat];

    return vk_format_info.compressed.compressedBlockDepth;
}

static inline uint32_t _GetCompressedFormatBlockSizeInBytes(MTLPixelFormat mtlformat)
{
    assert(_IsFormatCompressed(mtlformat));

    assert(mtlformat < (sizeof(gMetelFormatInfoTable) / sizeof(gMetelFormatInfoTable[0])));

    _FormatInfo vk_format_info = gMetelFormatInfoTable[mtlformat];

    return vk_format_info.compressed.compressedBlockSizeInBytes;
}

static inline uint32_t _GetRGBAFormatPixelBytes(MTLPixelFormat mtlformat)
{
    assert(_IsFormatRGBA(mtlformat));

    assert(mtlformat < (sizeof(gMetelFormatInfoTable) / sizeof(gMetelFormatInfoTable[0])));

    _FormatInfo vk_format_info = gMetelFormatInfoTable[mtlformat];

    return vk_format_info.rgba.pixelBytes;
}

static inline uint32_t _GetDepthStencilFormatPixelBytes(MTLPixelFormat mtlformat, uint32_t aspectIndex)
{
    assert(_IsFormatDepthStencil(mtlformat));

    assert(mtlformat < (sizeof(gMetelFormatInfoTable) / sizeof(gMetelFormatInfoTable[0])));

    _FormatInfo vk_format_info = gMetelFormatInfoTable[mtlformat];

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
