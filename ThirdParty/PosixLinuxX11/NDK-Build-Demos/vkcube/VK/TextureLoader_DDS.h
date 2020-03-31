#ifndef _VK_TEXTURELOADER_DDS_H_
#define _VK_TEXTURELOADER_DDS_H_ 1

#include <stddef.h>
#include <stdint.h>

#include "../TextureLoader_DDS.h"

uint64_t GetRequiredIntermediateSize(struct Texture_Header const *texture_desc);

#endif