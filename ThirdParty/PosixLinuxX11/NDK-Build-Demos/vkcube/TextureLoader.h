#ifndef _TEXTURELOADER_H_
#define _TEXTURELOADER_H_ 1

#include <stddef.h>
#include <stdint.h>

enum
{
    TEXTURE_LOADER_TYPE_1D,
    TEXTURE_LOADER_TYPE_2D,
    TEXTURE_LOADER_TYPE_3D,
    TEXTURE_LOADER_TYPE_RANGE_SIZE,
    TEXTURE_LOADER_TYPE_UNDEFINED
};

enum
{
    TEXTURE_LOADER_FORMAT_R4G4_UNORM_PACK8,
    TEXTURE_LOADER_FORMAT_R4G4B4A4_UNORM_PACK16,
    TEXTURE_LOADER_FORMAT_B4G4R4A4_UNORM_PACK16,
    TEXTURE_LOADER_FORMAT_R5G6B5_UNORM_PACK16,
    TEXTURE_LOADER_FORMAT_B5G6R5_UNORM_PACK16,
    TEXTURE_LOADER_FORMAT_R5G5B5A1_UNORM_PACK16,
    TEXTURE_LOADER_FORMAT_B5G5R5A1_UNORM_PACK16,
    TEXTURE_LOADER_FORMAT_A1R5G5B5_UNORM_PACK16,
    TEXTURE_LOADER_FORMAT_R8_UNORM,
    TEXTURE_LOADER_FORMAT_R8_SNORM,
    TEXTURE_LOADER_FORMAT_R8_USCALED,
    TEXTURE_LOADER_FORMAT_R8_SSCALED,
    TEXTURE_LOADER_FORMAT_R8_UINT,
    TEXTURE_LOADER_FORMAT_R8_SINT,
    TEXTURE_LOADER_FORMAT_R8_SRGB,
    TEXTURE_LOADER_FORMAT_R8G8_UNORM,
    TEXTURE_LOADER_FORMAT_R8G8_SNORM,
    TEXTURE_LOADER_FORMAT_R8G8_USCALED,
    TEXTURE_LOADER_FORMAT_R8G8_SSCALED,
    TEXTURE_LOADER_FORMAT_R8G8_UINT,
    TEXTURE_LOADER_FORMAT_R8G8_SINT,
    TEXTURE_LOADER_FORMAT_R8G8_SRGB,
    TEXTURE_LOADER_FORMAT_R8G8B8_UNORM,
    TEXTURE_LOADER_FORMAT_R8G8B8_SNORM,
    TEXTURE_LOADER_FORMAT_R8G8B8_USCALED,
    TEXTURE_LOADER_FORMAT_R8G8B8_SSCALED,
    TEXTURE_LOADER_FORMAT_R8G8B8_UINT,
    TEXTURE_LOADER_FORMAT_R8G8B8_SINT,
    TEXTURE_LOADER_FORMAT_R8G8B8_SRGB,
    TEXTURE_LOADER_FORMAT_B8G8R8_UNORM,
    TEXTURE_LOADER_FORMAT_B8G8R8_SNORM,
    TEXTURE_LOADER_FORMAT_B8G8R8_USCALED,
    TEXTURE_LOADER_FORMAT_B8G8R8_SSCALED,
    TEXTURE_LOADER_FORMAT_B8G8R8_UINT,
    TEXTURE_LOADER_FORMAT_B8G8R8_SINT,
    TEXTURE_LOADER_FORMAT_B8G8R8_SRGB,
    TEXTURE_LOADER_FORMAT_R8G8B8A8_UNORM,
    TEXTURE_LOADER_FORMAT_R8G8B8A8_SNORM,
    TEXTURE_LOADER_FORMAT_R8G8B8A8_USCALED,
    TEXTURE_LOADER_FORMAT_R8G8B8A8_SSCALED,
    TEXTURE_LOADER_FORMAT_R8G8B8A8_UINT,
    TEXTURE_LOADER_FORMAT_R8G8B8A8_SINT,
    TEXTURE_LOADER_FORMAT_R8G8B8A8_SRGB,
    TEXTURE_LOADER_FORMAT_B8G8R8A8_UNORM,
    TEXTURE_LOADER_FORMAT_B8G8R8A8_SNORM,
    TEXTURE_LOADER_FORMAT_B8G8R8A8_USCALED,
    TEXTURE_LOADER_FORMAT_B8G8R8A8_SSCALED,
    TEXTURE_LOADER_FORMAT_B8G8R8A8_UINT,
    TEXTURE_LOADER_FORMAT_B8G8R8A8_SINT,
    TEXTURE_LOADER_FORMAT_B8G8R8A8_SRGB,
    TEXTURE_LOADER_FORMAT_A8B8G8R8_UNORM_PACK32,
    TEXTURE_LOADER_FORMAT_A8B8G8R8_SNORM_PACK32,
    TEXTURE_LOADER_FORMAT_A8B8G8R8_USCALED_PACK32,
    TEXTURE_LOADER_FORMAT_A8B8G8R8_SSCALED_PACK32,
    TEXTURE_LOADER_FORMAT_A8B8G8R8_UINT_PACK32,
    TEXTURE_LOADER_FORMAT_A8B8G8R8_SINT_PACK32,
    TEXTURE_LOADER_FORMAT_A8B8G8R8_SRGB_PACK32,
    TEXTURE_LOADER_FORMAT_A2R10G10B10_UNORM_PACK32,
    TEXTURE_LOADER_FORMAT_A2R10G10B10_SNORM_PACK32,
    TEXTURE_LOADER_FORMAT_A2R10G10B10_USCALED_PACK32,
    TEXTURE_LOADER_FORMAT_A2R10G10B10_SSCALED_PACK32,
    TEXTURE_LOADER_FORMAT_A2R10G10B10_UINT_PACK32,
    TEXTURE_LOADER_FORMAT_A2R10G10B10_SINT_PACK32,
    TEXTURE_LOADER_FORMAT_A2B10G10R10_UNORM_PACK32,
    TEXTURE_LOADER_FORMAT_A2B10G10R10_SNORM_PACK32,
    TEXTURE_LOADER_FORMAT_A2B10G10R10_USCALED_PACK32,
    TEXTURE_LOADER_FORMAT_A2B10G10R10_SSCALED_PACK32,
    TEXTURE_LOADER_FORMAT_A2B10G10R10_UINT_PACK32,
    TEXTURE_LOADER_FORMAT_A2B10G10R10_SINT_PACK32,
    TEXTURE_LOADER_FORMAT_R16_UNORM,
    TEXTURE_LOADER_FORMAT_R16_SNORM,
    TEXTURE_LOADER_FORMAT_R16_USCALED,
    TEXTURE_LOADER_FORMAT_R16_SSCALED,
    TEXTURE_LOADER_FORMAT_R16_UINT,
    TEXTURE_LOADER_FORMAT_R16_SINT,
    TEXTURE_LOADER_FORMAT_R16_SFLOAT,
    TEXTURE_LOADER_FORMAT_R16G16_UNORM,
    TEXTURE_LOADER_FORMAT_R16G16_SNORM,
    TEXTURE_LOADER_FORMAT_R16G16_USCALED,
    TEXTURE_LOADER_FORMAT_R16G16_SSCALED,
    TEXTURE_LOADER_FORMAT_R16G16_UINT,
    TEXTURE_LOADER_FORMAT_R16G16_SINT,
    TEXTURE_LOADER_FORMAT_R16G16_SFLOAT,
    TEXTURE_LOADER_FORMAT_R16G16B16_UNORM,
    TEXTURE_LOADER_FORMAT_R16G16B16_SNORM,
    TEXTURE_LOADER_FORMAT_R16G16B16_USCALED,
    TEXTURE_LOADER_FORMAT_R16G16B16_SSCALED,
    TEXTURE_LOADER_FORMAT_R16G16B16_UINT,
    TEXTURE_LOADER_FORMAT_R16G16B16_SINT,
    TEXTURE_LOADER_FORMAT_R16G16B16_SFLOAT,
    TEXTURE_LOADER_FORMAT_R16G16B16A16_UNORM,
    TEXTURE_LOADER_FORMAT_R16G16B16A16_SNORM,
    TEXTURE_LOADER_FORMAT_R16G16B16A16_USCALED,
    TEXTURE_LOADER_FORMAT_R16G16B16A16_SSCALED,
    TEXTURE_LOADER_FORMAT_R16G16B16A16_UINT,
    TEXTURE_LOADER_FORMAT_R16G16B16A16_SINT,
    TEXTURE_LOADER_FORMAT_R16G16B16A16_SFLOAT,
    TEXTURE_LOADER_FORMAT_R32_UINT,
    TEXTURE_LOADER_FORMAT_R32_SINT,
    TEXTURE_LOADER_FORMAT_R32_SFLOAT,
    TEXTURE_LOADER_FORMAT_R32G32_UINT,
    TEXTURE_LOADER_FORMAT_R32G32_SINT,
    TEXTURE_LOADER_FORMAT_R32G32_SFLOAT,
    TEXTURE_LOADER_FORMAT_R32G32B32_UINT,
    TEXTURE_LOADER_FORMAT_R32G32B32_SINT,
    TEXTURE_LOADER_FORMAT_R32G32B32_SFLOAT,
    TEXTURE_LOADER_FORMAT_R32G32B32A32_UINT,
    TEXTURE_LOADER_FORMAT_R32G32B32A32_SINT,
    TEXTURE_LOADER_FORMAT_R32G32B32A32_SFLOAT,
    TEXTURE_LOADER_FORMAT_R64_UINT,
    TEXTURE_LOADER_FORMAT_R64_SINT,
    TEXTURE_LOADER_FORMAT_R64_SFLOAT,
    TEXTURE_LOADER_FORMAT_R64G64_UINT,
    TEXTURE_LOADER_FORMAT_R64G64_SINT,
    TEXTURE_LOADER_FORMAT_R64G64_SFLOAT,
    TEXTURE_LOADER_FORMAT_R64G64B64_UINT,
    TEXTURE_LOADER_FORMAT_R64G64B64_SINT,
    TEXTURE_LOADER_FORMAT_R64G64B64_SFLOAT,
    TEXTURE_LOADER_FORMAT_R64G64B64A64_UINT,
    TEXTURE_LOADER_FORMAT_R64G64B64A64_SINT,
    TEXTURE_LOADER_FORMAT_R64G64B64A64_SFLOAT,
    TEXTURE_LOADER_FORMAT_B10G11R11_UFLOAT_PACK32,
    TEXTURE_LOADER_FORMAT_E5B9G9R9_UFLOAT_PACK32,
    TEXTURE_LOADER_FORMAT_D16_UNORM,
    TEXTURE_LOADER_FORMAT_X8_D24_UNORM_PACK32,
    TEXTURE_LOADER_FORMAT_D32_SFLOAT,
    TEXTURE_LOADER_FORMAT_S8_UINT,
    TEXTURE_LOADER_FORMAT_D16_UNORM_S8_UINT,
    TEXTURE_LOADER_FORMAT_D24_UNORM_S8_UINT,
    TEXTURE_LOADER_FORMAT_D32_SFLOAT_S8_UINT,
    TEXTURE_LOADER_FORMAT_BC1_RGB_UNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_BC1_RGB_SRGB_BLOCK,
    TEXTURE_LOADER_FORMAT_BC1_RGBA_UNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_BC1_RGBA_SRGB_BLOCK,
    TEXTURE_LOADER_FORMAT_BC2_UNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_BC2_SRGB_BLOCK,
    TEXTURE_LOADER_FORMAT_BC3_UNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_BC3_SRGB_BLOCK,
    TEXTURE_LOADER_FORMAT_BC4_UNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_BC4_SNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_BC5_UNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_BC5_SNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_BC6H_UFLOAT_BLOCK,
    TEXTURE_LOADER_FORMAT_BC6H_SFLOAT_BLOCK,
    TEXTURE_LOADER_FORMAT_BC7_UNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_BC7_SRGB_BLOCK,
    TEXTURE_LOADER_FORMAT_ETC2_R8G8B8_UNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_ETC2_R8G8B8_SRGB_BLOCK,
    TEXTURE_LOADER_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK,
    TEXTURE_LOADER_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK,
    TEXTURE_LOADER_FORMAT_EAC_R11_UNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_EAC_R11_SNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_EAC_R11G11_UNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_EAC_R11G11_SNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_4x4_UNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_4x4_SRGB_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_5x4_UNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_5x4_SRGB_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_5x5_UNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_5x5_SRGB_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_6x5_UNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_6x5_SRGB_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_6x6_UNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_6x6_SRGB_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_8x5_UNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_8x5_SRGB_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_8x6_UNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_8x6_SRGB_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_8x8_UNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_8x8_SRGB_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_10x5_UNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_10x5_SRGB_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_10x6_UNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_10x6_SRGB_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_10x8_UNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_10x8_SRGB_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_10x10_UNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_10x10_SRGB_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_12x10_UNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_12x10_SRGB_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_12x12_UNORM_BLOCK,
    TEXTURE_LOADER_FORMAT_ASTC_12x12_SRGB_BLOCK,
    TEXTURE_LOADER_FORMAT_RANGE_SIZE,
    TEXTURE_LOADER_FORMAT_UNDEFINED
};

struct TextureLoader_NeutralHeader
{
    bool isCubeMap;
    uint32_t type;
    uint32_t format;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t mipLevels;
    uint32_t arrayLayers;
};

struct TextureLoader_MemcpyDest
{
    size_t stagingOffset;
    size_t outputRowPitch;
    size_t outputRowSize;
    size_t outputNumRows;
    size_t outputSlicePitch;
    size_t outputNumSlices;
};

enum
{
    TEXTURE_LOADER_STREAM_SEEK_SET = 0,
    TEXTURE_LOADER_STREAM_SEEK_CUR = 1,
    TEXTURE_LOADER_STREAM_SEEK_END = 2
};

bool TextureLoader_LoadHeaderFromStream(void const *stream, ptrdiff_t (*stream_read)(void const *stream, void *buf, size_t count), int64_t (*stream_seek)(void const *stream, int64_t offset, int whence),
                                        struct TextureLoader_NeutralHeader *neutral_texture_header, size_t *neutral_header_offset);

bool TextureLoader_FillDataFromStream(void const *stream, ptrdiff_t (*stream_read)(void const *stream, void *buf, size_t count), int64_t (*stream_seek)(void const *stream, int64_t offset, int whence),
                                      uint8_t *stagingPointer, size_t NumSubresources, struct TextureLoader_MemcpyDest const *pDest,
                                      struct TextureLoader_NeutralHeader const *neutral_texture_header_validate, size_t const *neutral_header_offset_validate);

uint32_t TextureLoader_CalcSubresource(uint32_t mipLevel, uint32_t arrayLayer, uint32_t aspectIndex, uint32_t mipLevels, uint32_t arrayLayers);

#include <string.h>

inline bool TextureLoader_LoadHeaderFromMemory(uint8_t const *ddsData, size_t ddsDataSize,
                                               struct TextureLoader_NeutralHeader *neutral_texture_header, size_t *neutral_header_offset)
{
    struct stream_memory
    {
        uint8_t const *m_ddsData;
        size_t m_ddsDataSize;
        mutable uint8_t const *m_p;
    } const stream = {ddsData, ddsDataSize, ddsData};

    return TextureLoader_LoadHeaderFromStream(
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

inline bool TextureLoader_FillDataFromMemory(uint8_t const *ddsData, size_t ddsDataSize,
                                             uint8_t *stagingPointer, size_t NumSubresources, struct TextureLoader_MemcpyDest const *pDest,
                                             struct TextureLoader_NeutralHeader const *neutral_texture_header_validate, size_t const *neutral_header_offset_validate)
{
    struct stream_memory
    {
        uint8_t const *m_ddsData;
        size_t m_ddsDataSize;
        mutable uint8_t const *m_p;
    } const stream = {ddsData, ddsDataSize, ddsData};

    return TextureLoader_FillDataFromStream(
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