#include "TextureLoader.h"

static inline constexpr uint32_t TextureLoader_MakeFourCC(char ch0, char ch1, char ch2, char ch3)
{
    return static_cast<uint32_t>(static_cast<uint8_t>(ch0)) | (static_cast<uint32_t>(static_cast<uint8_t>(ch1)) << 8) | (static_cast<uint32_t>(static_cast<uint8_t>(ch2)) << 16) | (static_cast<uint32_t>(static_cast<uint8_t>(ch3)) << 24);
}

enum
{
    DDS_MAGIC = TextureLoader_MakeFourCC('D', 'D', 'S', ' '),
    PVR_HEADER_V1 = 44,
    PVR_HEADER_V2 = 52,
    PVR_HEADER_V3 = TextureLoader_MakeFourCC('P', 'V', 'R', 3)
};

#include "DDS/TextureLoader_DDS.h"
#include "PVR/TextureLoader_PVR.h"

bool TextureLoader_LoadHeaderFromStream(void const *stream, ptrdiff_t (*stream_read)(void const *stream, void *buf, size_t count), int64_t (*stream_seek)(void const *stream, int64_t offset, int whence),
                                        struct TextureLoader_NeutralHeader *neutral_texture_header, size_t *neutral_header_offset)
{
    uint32_t dwMagicNumber;
    {
        ptrdiff_t BytesRead = stream_read(stream, &dwMagicNumber, sizeof(uint32_t));
        if (BytesRead == -1 || BytesRead < sizeof(uint32_t))
        {
            return false;
        }
    }

    if (stream_seek(stream, 0, TEXTURE_LOADER_STREAM_SEEK_SET) == -1)
    {
        return false;
    }

    switch (dwMagicNumber)
    {
    case DDS_MAGIC:
        return DDSTextureLoader_LoadHeaderFromStream(stream, stream_read, stream_seek, neutral_texture_header, neutral_header_offset);
    case PVR_HEADER_V3:
    case PVR_HEADER_V2:
    case PVR_HEADER_V1:
        return PVRTextureLoader_LoadHeaderFromStream(stream, stream_read, stream_seek, neutral_texture_header, neutral_header_offset);
    default:
        return false;
    }
}

bool TextureLoader_FillDataFromStream(void const *stream, ptrdiff_t (*stream_read)(void const *stream, void *buf, size_t count), int64_t (*stream_seek)(void const *stream, int64_t offset, int whence),
                                      uint8_t *stagingPointer, size_t NumSubresources, struct TextureLoader_MemcpyDest const *pDest,
                                      struct TextureLoader_NeutralHeader const *neutral_texture_header_validate, size_t const *neutral_header_offset_validate)
{
    uint32_t dwMagicNumber;
    {
        ptrdiff_t BytesRead = stream_read(stream, &dwMagicNumber, sizeof(uint32_t));
        if (BytesRead == -1 || BytesRead < sizeof(uint32_t))
        {
            return false;
        }
    }

    if (stream_seek(stream, 0, TEXTURE_LOADER_STREAM_SEEK_SET) == -1)
    {
        return false;
    }

    switch (dwMagicNumber)
    {
    case DDS_MAGIC:
        return DDSTextureLoader_FillDataFromStream(stream, stream_read, stream_seek, stagingPointer, NumSubresources, pDest, neutral_texture_header_validate, neutral_header_offset_validate);
    case PVR_HEADER_V3:
    case PVR_HEADER_V2:
    case PVR_HEADER_V1:
        return PVRTextureLoader_FillDataFromStream(stream, stream_read, stream_seek, stagingPointer, NumSubresources, pDest, neutral_texture_header_validate, neutral_header_offset_validate);
    default:
        return false;
    }
}