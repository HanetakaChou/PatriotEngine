#ifndef _TEXTURELOADER_PVR_H_
#define _TEXTURELOADER_PVR_H_ 1

#include "TextureLoader.h"

bool PVRTextureLoader_LoadHeaderFromStream(void const *stream, ptrdiff_t (*stream_read)(void const *stream, void *buf, size_t count), int64_t (*stream_seek)(void const *stream, int64_t offset, int whence),
                                        struct TextureLoader_NeutralHeader *neutral_texture_header, size_t *neutral_header_offset);

bool PVRTextureLoader_FillDataFromStream(void const *stream, ptrdiff_t (*stream_read)(void const *stream, void *buf, size_t count), int64_t (*stream_seek)(void const *stream, int64_t offset, int whence),
                                      uint8_t *stagingPointer, size_t NumSubresources, struct TextureLoader_MemcpyDest const *pDest,
                                      struct TextureLoader_NeutralHeader const *neutral_texture_header_validate, size_t const *neutral_header_offset_validate);

#include <string.h>

inline bool PVRTextureLoader_LoadHeaderFromMemory(uint8_t const *ddsData, size_t ddsDataSize,
                                               struct TextureLoader_NeutralHeader *neutral_texture_header, size_t *neutral_header_offset)
{
    struct stream_memory
    {
        uint8_t const *m_ddsData;
        size_t m_ddsDataSize;
        mutable uint8_t const *m_p;
    } const stream = {ddsData, ddsDataSize, ddsData};

    return PVRTextureLoader_LoadHeaderFromStream(
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
        neutral_texture_header,
        neutral_header_offset);
}

inline bool PVRTextureLoader_FillDataFromMemory(uint8_t const *ddsData, size_t ddsDataSize,
                                             uint8_t *stagingPointer, size_t NumSubresources, struct TextureLoader_MemcpyDest const *pDest,
                                             struct TextureLoader_NeutralHeader const *neutral_texture_header_validate, size_t const *neutral_header_offset_validate)
{
    struct stream_memory
    {
        uint8_t const *m_ddsData;
        size_t m_ddsDataSize;
        mutable uint8_t const *m_p;
    } const stream = {ddsData, ddsDataSize, ddsData};

    return PVRTextureLoader_FillDataFromStream(
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
        neutral_texture_header_validate,
        neutral_header_offset_validate);
}

#endif