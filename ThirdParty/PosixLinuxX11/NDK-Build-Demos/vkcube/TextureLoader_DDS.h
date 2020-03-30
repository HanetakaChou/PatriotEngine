#ifndef _TEXTURELOADER_DDS_H_
#define _TEXTURELOADER_DDS_H_ 1

#include <stddef.h>
#include <stdint.h>

enum Texture_Misc_Flag_Bits : uint32_t
{
    TEXTURE_MISC_CUBE_BIT = 0x00000010,
    TEXTURE_MISC_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
};

enum Texture_Type : uint32_t
{
    TEXTURE_TYPE_1D = 0,
    TEXTURE_TYPE_2D = 1,
    TEXTURE_TYPE_3D = 2,
    TEXTURE_TYPE_MAX_ENUM = 0x7FFFFFFF
};

enum Texture_Format : uint32_t
{
    TEXTURE_FORMAT_UNDEFINED = 0,
    TEXTURE_FORMAT_BC7_UNORM_BLOCK = 145,
    TEXTURE_FORMAT_BC7_SRGB_BLOCK = 146,
    TEXTURE_FORMAT_MAX_ENUM = 0x7FFFFFFF
};

struct Texture_Header
{
    uint32_t flags;
    enum Texture_Type type;
    enum Texture_Format format;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t mipLevels;
    uint32_t arrayLayers;
};

// SEEK_SET 0
// SEEK_CUR 1
// SEEK_END 2

bool LoadTextureDataFromStream(void const *stream, ptrdiff_t (*stream_read)(void const *stream, void *buf, size_t count), int64_t (*stream_seek)(void const *stream, int64_t offset, int whence),
                               struct Texture_Header *header, size_t *inputSkipBytes);

#include <string.h>

inline bool LoadTextureDataFromMemory(uint8_t const *ddsData, size_t ddsDataSize,
                                      struct Texture_Header *header, size_t *inputSkipBytes)
{
    struct stream_memory
    {
        uint8_t const *m_ddsData;
        size_t m_ddsDataSize;
        mutable uint8_t const *m_p;
    } const stream = {ddsData, ddsDataSize, ddsData};

    return LoadTextureDataFromStream(
        &stream,
        [](void const *stream, void *buf, size_t count) -> ptrdiff_t {
            ptrdiff_t countread = ((static_cast<stream_memory const *>(stream)->m_p + count) <= (static_cast<stream_memory const *>(stream)->m_ddsData + static_cast<stream_memory const *>(stream)->m_ddsDataSize))
                                      ? count
                                      : ((static_cast<stream_memory const *>(stream)->m_p <= (static_cast<stream_memory const *>(stream)->m_ddsData + static_cast<stream_memory const *>(stream)->m_ddsDataSize))
                                             ? ((static_cast<stream_memory const *>(stream)->m_ddsData + static_cast<stream_memory const *>(stream)->m_ddsDataSize) - static_cast<stream_memory const *>(stream)->m_p)
                                             : 0);
            memcpy(buf, static_cast<stream_memory const *>(stream)->m_p, countread);
            static_cast<stream_memory const *>(stream)->m_p = (static_cast<stream_memory const *>(stream)->m_p + countread);
            return countread;
        },
        [](void const *stream, int64_t offset, int whence) -> int64_t {
            switch (whence)
            {
            case 0:
                static_cast<stream_memory const *>(stream)->m_p = static_cast<stream_memory const *>(stream)->m_ddsData + offset;
                break;
            case 1:
                static_cast<stream_memory const *>(stream)->m_p = static_cast<stream_memory const *>(stream)->m_p + offset;
                break;
            case 2:
                static_cast<stream_memory const *>(stream)->m_p = static_cast<stream_memory const *>(stream)->m_ddsData + offset;
                break;
            default:
                return -1;
            }

            return static_cast<stream_memory const *>(stream)->m_p - static_cast<stream_memory const *>(stream)->m_ddsData;
        },
        header,
        inputSkipBytes);
}

#endif