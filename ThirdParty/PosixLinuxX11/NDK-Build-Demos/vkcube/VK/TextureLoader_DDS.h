#ifndef _VK_TEXTURELOADER_DDS_H_
#define _VK_TEXTURELOADER_DDS_H_ 1

#include <stddef.h>
#include <stdint.h>

#include "../TextureLoader_DDS.h"

//void TextureLoader_UpdateSubresources(struct Texture_Header const *texture_desc);

size_t TextureLoader_GetCopyableFootprints(struct Texture_Header const *texture_desc,
                                           VkDeviceSize optimalBufferCopyOffsetAlignment, VkDeviceSize optimalBufferCopyRowPitchAlignment,
                                           size_t NumSubresources, struct Texture_Loader_Memcpy_Dest *pDest, VkBufferImageCopy *pRegions);

#endif