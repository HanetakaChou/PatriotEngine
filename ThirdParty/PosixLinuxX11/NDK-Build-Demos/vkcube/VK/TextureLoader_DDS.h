#ifndef _VK_TEXTURELOADER_DDS_H_
#define _VK_TEXTURELOADER_DDS_H_ 1

#include <stddef.h>
#include <stdint.h>

#include "../TextureLoader_DDS.h"

#include <vulkan/vulkan.h>

//void TextureLoader_UpdateSubresources(struct Texture_Header const *texture_desc);

struct TextureLoader_ImageInfo
{
    bool isCubeCompatible;
    VkImageType imageType;
    VkFormat format;
    VkExtent3D extent;
    uint32_t mipLevels;
    uint32_t arrayLayers;
};

struct TextureLoader_ImageInfo TextureLoader_ToImageInfo(struct Texture_Header const *neutral_texture_header);

size_t TextureLoader_GetCopyableFootprints(struct TextureLoader_ImageInfo *pImageInfo,
                                           VkDeviceSize optimalBufferCopyOffsetAlignment, VkDeviceSize optimalBufferCopyRowPitchAlignment,
                                           size_t NumSubresources, struct TextureLoader_MemcpyDest *pDest, VkBufferImageCopy *pRegions);

#endif