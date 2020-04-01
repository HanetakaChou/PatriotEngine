#ifndef _TEXTURELOADER_DDS_H_
#define _TEXTURELOADER_DDS_H_ 1

#include <stddef.h>
#include <stdint.h>

enum
{
    TEXTURE_MISC_CUBE_BIT = 0x00000010
};

enum
{
    TEXTURE_TYPE_1D = 0,
    TEXTURE_TYPE_2D = 1,
    TEXTURE_TYPE_3D = 2
};

enum
{
    TEXTURE_FORMAT_UNDEFINED,
    TEXTURE_FORMAT_BC7_UNORM_BLOCK,
    TEXTURE_FORMAT_BC7_SRGB_BLOCK,
    TEXTURE_FORMAT_RANGE_SIZE
};

struct Texture_Header
{
    uint32_t flags;
    uint32_t type;
    uint32_t format;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t mipLevels;
    uint32_t arrayLayers;
};

enum
{
    TEXTURE_LOADER_STREAM_SEEK_SET = 0,
    TEXTURE_LOADER_STREAM_SEEK_CUR = 1,
    TEXTURE_LOADER_STREAM_SEEK_END = 2
};

bool LoadTextureHeaderFromStream(void const *stream, ptrdiff_t (*stream_read)(void const *stream, void *buf, size_t count), int64_t (*stream_seek)(void const *stream, int64_t offset, int whence),
                                 struct Texture_Header *texture_desc, size_t *header_offset);
struct TextureLoader_MemcpyDest
{
    size_t stagingOffset;
    size_t outputRowPitch;
    size_t outputRowSize;
    size_t outputNumRows;
    size_t outputSlicePitch;
    size_t outputNumSlices;
};

bool FillTextureDataFromStream(void const *stream, ptrdiff_t (*stream_read)(void const *stream, void *buf, size_t count), int64_t (*stream_seek)(void const *stream, int64_t offset, int whence),
                               uint8_t *stagingPointer, size_t NumSubresources, struct TextureLoader_MemcpyDest const *pDest,
                               struct Texture_Header const *texture_desc_validate, size_t const *header_offset_validate);

#include <string.h>

inline bool LoadTextureHeaderFromMemory(uint8_t const *ddsData, size_t ddsDataSize,
                                        struct Texture_Header *texture_desc, size_t *header_offset)
{
    struct stream_memory
    {
        uint8_t const *m_ddsData;
        size_t m_ddsDataSize;
        mutable uint8_t const *m_p;
    } const stream = {ddsData, ddsDataSize, ddsData};

    return LoadTextureHeaderFromStream(
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
            case TEXTURE_LOADER_STREAM_SEEK_SET:
                static_cast<stream_memory const *>(stream)->m_p = static_cast<stream_memory const *>(stream)->m_ddsData + offset;
                break;
            case TEXTURE_LOADER_STREAM_SEEK_CUR:
                static_cast<stream_memory const *>(stream)->m_p = static_cast<stream_memory const *>(stream)->m_p + offset;
                break;
            case TEXTURE_LOADER_STREAM_SEEK_END:
                static_cast<stream_memory const *>(stream)->m_p = static_cast<stream_memory const *>(stream)->m_ddsData + offset;
                break;
            default:
                return -1;
            }

            return static_cast<stream_memory const *>(stream)->m_p - static_cast<stream_memory const *>(stream)->m_ddsData;
        },
        texture_desc,
        header_offset);
}

inline bool FillTextureDataFromMemory(uint8_t const *ddsData, size_t ddsDataSize,
                                      uint8_t *stagingPointer, size_t NumSubresources, struct TextureLoader_MemcpyDest const *pDest,
                                      struct Texture_Header const *texture_desc_validate, size_t const *header_offset_validate)
{
    struct stream_memory
    {
        uint8_t const *m_ddsData;
        size_t m_ddsDataSize;
        mutable uint8_t const *m_p;
    } const stream = {ddsData, ddsDataSize, ddsData};

    return FillTextureDataFromStream(
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
            case TEXTURE_LOADER_STREAM_SEEK_SET:
                static_cast<stream_memory const *>(stream)->m_p = static_cast<stream_memory const *>(stream)->m_ddsData + offset;
                break;
            case TEXTURE_LOADER_STREAM_SEEK_CUR:
                static_cast<stream_memory const *>(stream)->m_p = static_cast<stream_memory const *>(stream)->m_p + offset;
                break;
            case TEXTURE_LOADER_STREAM_SEEK_END:
                static_cast<stream_memory const *>(stream)->m_p = static_cast<stream_memory const *>(stream)->m_ddsData + offset;
                break;
            default:
                return -1;
            }

            return static_cast<stream_memory const *>(stream)->m_p - static_cast<stream_memory const *>(stream)->m_ddsData;
        },
        stagingPointer,
        NumSubresources,
        pDest,
        texture_desc_validate,
        header_offset_validate);
}

#endif