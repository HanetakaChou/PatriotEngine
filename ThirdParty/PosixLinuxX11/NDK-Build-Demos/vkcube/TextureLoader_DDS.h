#ifndef _TEXTURELOADER_DDS_H_
#define _TEXTURELOADER_DDS_H_ 1

#include <stddef.h>
#include <stdint.h>

enum Texture_Format : uint32_t
{
    TEX_FORMAT_BC7_UNORM_BLOCK = 145,
    TEX_FORMAT_BC7_SRGB_BLOCK = 146,
    TEX_FORMAT_MAX_ENUM = 0x7FFFFFFF
};

// SEEK_SET 0
// SEEK_CUR 1
// SEEK_END 2

bool LoadTextureDataFromStream(void *stream, ptrdiff_t (*stream_read)(void *stream, void *buf, size_t count), int64_t (*stream_seek)(void *stream, int64_t offset, int whence),
                               struct DDS_HEADER const **header, uint8_t const **bitData, size_t *bitSize);

#include <string.h>

inline bool LoadTextureDataFromMemory(uint8_t const *ddsData, size_t ddsDataSize,
                                      struct DDS_HEADER const **header, uint8_t const **bitData, size_t *bitSize)
{
    struct stream_memory
    {
        uint8_t const *m_ddsData;
        size_t m_ddsDataSize;
        uint8_t const *m_p;
    } stream = {ddsData, ddsDataSize, ddsData};

    return LoadTextureDataFromStream(
        &stream,
        [](void *stream, void *buf, size_t count) -> ptrdiff_t {
            ptrdiff_t countread = ((static_cast<stream_memory *>(stream)->m_p + count) <= (static_cast<stream_memory *>(stream)->m_ddsData + static_cast<stream_memory *>(stream)->m_ddsDataSize))
                                      ? count
                                      : ((static_cast<stream_memory *>(stream)->m_p <= (static_cast<stream_memory *>(stream)->m_ddsData + static_cast<stream_memory *>(stream)->m_ddsDataSize))
                                             ? ((static_cast<stream_memory *>(stream)->m_ddsData + static_cast<stream_memory *>(stream)->m_ddsDataSize) - static_cast<stream_memory *>(stream)->m_p)
                                             : 0);
            memcpy(buf, static_cast<stream_memory *>(stream)->m_p, countread);
            static_cast<stream_memory *>(stream)->m_p = (static_cast<stream_memory *>(stream)->m_p + countread);
            return countread;
        },
        [](void *stream, int64_t offset, int whence) -> int64_t {
            switch (whence)
            {
            case 0:
                static_cast<stream_memory *>(stream)->m_p = static_cast<stream_memory *>(stream)->m_ddsData + offset;
                break;
            case 1:
                static_cast<stream_memory *>(stream)->m_p = static_cast<stream_memory *>(stream)->m_p + offset;
                break;
            case 2:
                static_cast<stream_memory *>(stream)->m_p = static_cast<stream_memory *>(stream)->m_ddsData + offset;
                break;
            default:
                return -1;
            }

            return static_cast<stream_memory *>(stream)->m_p - static_cast<stream_memory *>(stream)->m_ddsData;
        },
        header,
        bitData,
        bitSize);
}

#endif