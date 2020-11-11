#ifndef _MTL_TEXTURELOADER_MTL_H_
#define _MTL_TEXTURELOADER_MTL_H_ 1

#include "../TextureLoader.h"

#include "../../Headers_CXX/Metal_CXX.h"

struct TextureLoader_SpecificHeader
{
    MTLTextureType textureType;
    MTLPixelFormat pixelFormat;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t mipmapLevelCount;
    uint32_t arrayLength;
};

struct TextureLoader_SpecificHeader TextureLoader_ToSpecificHeader(struct TextureLoader_NeutralHeader const *neutral_texture_header);

uint32_t TextureLoader_GetFormatAspectCount(MTLPixelFormat mtlformat);

uint32_t TextureLoader_GetSliceCount(MTLTextureType textureType, uint32_t arrayLength);

struct TextureLoader_MTLCopyFromBuffer
{
    NSUInteger sourceOffset;
    NSUInteger sourceBytesPerRow;
    NSUInteger sourceBytesPerImage;
    MTLSize sourceSize;
    NSUInteger destinationSlice;
    NSUInteger destinationLevel;
    MTLOrigin destinationOrigin;
};

size_t TextureLoader_GetCopyableFootprints(struct TextureLoader_SpecificHeader const *mtl_texture_header,
                                           uint32_t NumSubresources, struct TextureLoader_MemcpyDest *pDest, TextureLoader_MTLCopyFromBuffer *pRegions);

#endif
