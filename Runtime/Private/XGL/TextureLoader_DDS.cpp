#include "../TextureLoader.h"
#include "TextureLoader_DDS.h"

//--------------------------------------------------------------------------------------
// Macros
//--------------------------------------------------------------------------------------
static inline constexpr uint32_t DDS_MAKEFOURCC(char ch0, char ch1, char ch2, char ch3)
{
    return static_cast<uint32_t>(static_cast<uint8_t>(ch0)) | (static_cast<uint32_t>(static_cast<uint8_t>(ch1)) << 8) | (static_cast<uint32_t>(static_cast<uint8_t>(ch2)) << 16) | (static_cast<uint32_t>(static_cast<uint8_t>(ch3)) << 24);
}

//--------------------------------------------------------------------------------------
// DDS file structure definitions
//
// https://docs.microsoft.com/en-us/windows/win32/direct3ddds/dx-graphics-dds
//--------------------------------------------------------------------------------------

enum
{
    DDPF_ALPHA = 0x00000002,
    DDPF_FOURCC = 0x00000004,
    DDPF_RGB = 0x00000040,
    DDPF_LUMINANCE = 0x00020000,
    DDPF_BUMPDUDV = 0x00080000
};

struct DDS_PIXELFORMAT
{
    uint32_t dwSize;
    uint32_t dwFlags;
    uint32_t dwFourCC;
    uint32_t dwRGBBitCount;
    uint32_t dwRBitMask;
    uint32_t dwGBitMask;
    uint32_t dwBBitMask;
    uint32_t dwABitMask;
};

enum
{
    DDSD_HEIGHT = 0x00000002,
    DDSD_DEPTH = 0x00800000
};

enum
{
    DDSCAPS2_CUBEMAP = 0x00000200,
    DDSCAPS2_CUBEMAP_POSITIVEX = 0x00000400,
    DDSCAPS2_CUBEMAP_NEGATIVEX = 0x00000800,
    DDSCAPS2_CUBEMAP_POSITIVEY = 0x00001000,
    DDSCAPS2_CUBEMAP_NEGATIVEY = 0x00002000,
    DDSCAPS2_CUBEMAP_POSITIVEZ = 0x00004000,
    DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x00008000
};

struct DDS_HEADER
{
    uint32_t dwSize;
    uint32_t dwFlags;
    uint32_t dwHeight;
    uint32_t dwWidth;
    uint32_t dwPitchOrLinearSize;
    uint32_t dwDepth; // only if DDSD_DEPTH is set in flags
    uint32_t dwMipMapCount;
    uint32_t dwReserved1[11];
    struct DDS_PIXELFORMAT ddspf;
    uint32_t dwCaps;
    uint32_t dwCaps2;
    uint32_t dwCaps3;
    uint32_t dwCaps4;
    uint32_t dwReserved2;
};

enum
{
    DDS_DXGI_FORMAT_UNKNOWN = 0,
    DDS_DXGI_FORMAT_R32G32B32A32_TYPELESS = 1,
    DDS_DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
    DDS_DXGI_FORMAT_R32G32B32A32_UINT = 3,
    DDS_DXGI_FORMAT_R32G32B32A32_SINT = 4,
    DDS_DXGI_FORMAT_R32G32B32_TYPELESS = 5,
    DDS_DXGI_FORMAT_R32G32B32_FLOAT = 6,
    DDS_DXGI_FORMAT_R32G32B32_UINT = 7,
    DDS_DXGI_FORMAT_R32G32B32_SINT = 8,
    DDS_DXGI_FORMAT_R16G16B16A16_TYPELESS = 9,
    DDS_DXGI_FORMAT_R16G16B16A16_FLOAT = 10,
    DDS_DXGI_FORMAT_R16G16B16A16_UNORM = 11,
    DDS_DXGI_FORMAT_R16G16B16A16_UINT = 12,
    DDS_DXGI_FORMAT_R16G16B16A16_SNORM = 13,
    DDS_DXGI_FORMAT_R16G16B16A16_SINT = 14,
    DDS_DXGI_FORMAT_R32G32_TYPELESS = 15,
    DDS_DXGI_FORMAT_R32G32_FLOAT = 16,
    DDS_DXGI_FORMAT_R32G32_UINT = 17,
    DDS_DXGI_FORMAT_R32G32_SINT = 18,
    DDS_DXGI_FORMAT_R32G8X24_TYPELESS = 19,
    DDS_DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 20,
    DDS_DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
    DDS_DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
    DDS_DXGI_FORMAT_R10G10B10A2_TYPELESS = 23,
    DDS_DXGI_FORMAT_R10G10B10A2_UNORM = 24,
    DDS_DXGI_FORMAT_R10G10B10A2_UINT = 25,
    DDS_DXGI_FORMAT_R11G11B10_FLOAT = 26,
    DDS_DXGI_FORMAT_R8G8B8A8_TYPELESS = 27,
    DDS_DXGI_FORMAT_R8G8B8A8_UNORM = 28,
    DDS_DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
    DDS_DXGI_FORMAT_R8G8B8A8_UINT = 30,
    DDS_DXGI_FORMAT_R8G8B8A8_SNORM = 31,
    DDS_DXGI_FORMAT_R8G8B8A8_SINT = 32,
    DDS_DXGI_FORMAT_R16G16_TYPELESS = 33,
    DDS_DXGI_FORMAT_R16G16_FLOAT = 34,
    DDS_DXGI_FORMAT_R16G16_UNORM = 35,
    DDS_DXGI_FORMAT_R16G16_UINT = 36,
    DDS_DXGI_FORMAT_R16G16_SNORM = 37,
    DDS_DXGI_FORMAT_R16G16_SINT = 38,
    DDS_DXGI_FORMAT_R32_TYPELESS = 39,
    DDS_DXGI_FORMAT_D32_FLOAT = 40,
    DDS_DXGI_FORMAT_R32_FLOAT = 41,
    DDS_DXGI_FORMAT_R32_UINT = 42,
    DDS_DXGI_FORMAT_R32_SINT = 43,
    DDS_DXGI_FORMAT_R24G8_TYPELESS = 44,
    DDS_DXGI_FORMAT_D24_UNORM_S8_UINT = 45,
    DDS_DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 46,
    DDS_DXGI_FORMAT_X24_TYPELESS_G8_UINT = 47,
    DDS_DXGI_FORMAT_R8G8_TYPELESS = 48,
    DDS_DXGI_FORMAT_R8G8_UNORM = 49,
    DDS_DXGI_FORMAT_R8G8_UINT = 50,
    DDS_DXGI_FORMAT_R8G8_SNORM = 51,
    DDS_DXGI_FORMAT_R8G8_SINT = 52,
    DDS_DXGI_FORMAT_R16_TYPELESS = 53,
    DDS_DXGI_FORMAT_R16_FLOAT = 54,
    DDS_DXGI_FORMAT_D16_UNORM = 55,
    DDS_DXGI_FORMAT_R16_UNORM = 56,
    DDS_DXGI_FORMAT_R16_UINT = 57,
    DDS_DXGI_FORMAT_R16_SNORM = 58,
    DDS_DXGI_FORMAT_R16_SINT = 59,
    DDS_DXGI_FORMAT_R8_TYPELESS = 60,
    DDS_DXGI_FORMAT_R8_UNORM = 61,
    DDS_DXGI_FORMAT_R8_UINT = 62,
    DDS_DXGI_FORMAT_R8_SNORM = 63,
    DDS_DXGI_FORMAT_R8_SINT = 64,
    DDS_DXGI_FORMAT_A8_UNORM = 65,
    DDS_DXGI_FORMAT_R1_UNORM = 66,
    DDS_DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 67,
    DDS_DXGI_FORMAT_R8G8_B8G8_UNORM = 68,
    DDS_DXGI_FORMAT_G8R8_G8B8_UNORM = 69,
    DDS_DXGI_FORMAT_BC1_TYPELESS = 70,
    DDS_DXGI_FORMAT_BC1_UNORM = 71,
    DDS_DXGI_FORMAT_BC1_UNORM_SRGB = 72,
    DDS_DXGI_FORMAT_BC2_TYPELESS = 73,
    DDS_DXGI_FORMAT_BC2_UNORM = 74,
    DDS_DXGI_FORMAT_BC2_UNORM_SRGB = 75,
    DDS_DXGI_FORMAT_BC3_TYPELESS = 76,
    DDS_DXGI_FORMAT_BC3_UNORM = 77,
    DDS_DXGI_FORMAT_BC3_UNORM_SRGB = 78,
    DDS_DXGI_FORMAT_BC4_TYPELESS = 79,
    DDS_DXGI_FORMAT_BC4_UNORM = 80,
    DDS_DXGI_FORMAT_BC4_SNORM = 81,
    DDS_DXGI_FORMAT_BC5_TYPELESS = 82,
    DDS_DXGI_FORMAT_BC5_UNORM = 83,
    DDS_DXGI_FORMAT_BC5_SNORM = 84,
    DDS_DXGI_FORMAT_B5G6R5_UNORM = 85,
    DDS_DXGI_FORMAT_B5G5R5A1_UNORM = 86,
    DDS_DXGI_FORMAT_B8G8R8A8_UNORM = 87,
    DDS_DXGI_FORMAT_B8G8R8X8_UNORM = 88,
    DDS_DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
    DDS_DXGI_FORMAT_B8G8R8A8_TYPELESS = 90,
    DDS_DXGI_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
    DDS_DXGI_FORMAT_B8G8R8X8_TYPELESS = 92,
    DDS_DXGI_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
    DDS_DXGI_FORMAT_BC6H_TYPELESS = 94,
    DDS_DXGI_FORMAT_BC6H_UF16 = 95,
    DDS_DXGI_FORMAT_BC6H_SF16 = 96,
    DDS_DXGI_FORMAT_BC7_TYPELESS = 97,
    DDS_DXGI_FORMAT_BC7_UNORM = 98,
    DDS_DXGI_FORMAT_BC7_UNORM_SRGB = 99,
    DDS_DXGI_FORMAT_AYUV = 100,
    DDS_DXGI_FORMAT_Y410 = 101,
    DDS_DXGI_FORMAT_Y416 = 102,
    DDS_DXGI_FORMAT_NV12 = 103,
    DDS_DXGI_FORMAT_P010 = 104,
    DDS_DXGI_FORMAT_P016 = 105,
    DDS_DXGI_FORMAT_420_OPAQUE = 106,
    DDS_DXGI_FORMAT_YUY2 = 107,
    DDS_DXGI_FORMAT_Y210 = 108,
    DDS_DXGI_FORMAT_Y216 = 109,
    DDS_DXGI_FORMAT_NV11 = 110,
    DDS_DXGI_FORMAT_AI44 = 111,
    DDS_DXGI_FORMAT_IA44 = 112,
    DDS_DXGI_FORMAT_P8 = 113,
    DDS_DXGI_FORMAT_A8P8 = 114,
    DDS_DXGI_FORMAT_B4G4R4A4_UNORM = 115,
    DDS_DXGI_FORMAT_P208 = 130,
    DDS_DXGI_FORMAT_V208 = 131,
    DDS_DXGI_FORMAT_V408 = 132,
    DDS_DXGI_FORMAT_BEGIN_RANGE = DDS_DXGI_FORMAT_UNKNOWN,
    DDS_DXGI_FORMAT_END_RANGE = DDS_DXGI_FORMAT_V408,
    DDS_DXGI_FORMAT_RANGE = (DDS_DXGI_FORMAT_END_RANGE - DDS_DXGI_FORMAT_BEGIN_RANGE + 1),
};

enum
{
    DDS_DIMENSION_UNKNOWN = 0,
    DDS_DIMENSION_UNKNOWN_BUFFER = 1,
    DDS_DIMENSION_TEXTURE1D = 2,
    DDS_DIMENSION_TEXTURE2D = 3,
    DDS_DIMENSION_TEXTURE3D = 4,
    DDS_DIMENSION_BEGIN_RANGE = DDS_DIMENSION_UNKNOWN,
    DDS_DIMENSION_END_RANGE = DDS_DIMENSION_TEXTURE3D,
    DDS_DIMENSION_RANGE = (DDS_DIMENSION_END_RANGE - DDS_DIMENSION_BEGIN_RANGE + 1),
};

enum
{
    DDS_RESOURCE_MISC_TEXTURECUBE = 0x4L
};

enum
{
    DDS_ALPHA_MODE_UNKNOWN = 0,
    DDS_ALPHA_MODE_STRAIGHT = 1,
    DDS_ALPHA_MODE_PREMULTIPLIED = 2,
    DDS_ALPHA_MODE_OPAQUE = 3,
    DDS_ALPHA_MODE_CUSTOM = 4
};

enum
{
    DDS_MISC_FLAGS2_ALPHA_MODE_MASK = 0x7L
};

struct DDS_HEADER_DXT10
{
    uint32_t dxgiFormat;
    uint32_t resourceDimension;
    uint32_t miscFlag;
    uint32_t arraySize;
    uint32_t miscFlags2;
};

//--------------------------------------------------------------------------------------

static inline uint32_t _GetNeutralType(uint32_t ddstype);

static inline uint32_t _GetNeutralFormat(uint32_t ddsformat);

static inline uint32_t GetDXGIFormat(struct DDS_PIXELFORMAT const *ddpf);

static inline size_t BitsPerPixel(uint32_t fmt);

static inline bool GetSurfaceInfo(size_t width, size_t height, uint32_t fmt, size_t *outNumBytes, size_t *outRowBytes, size_t *outNumRows);

static inline uint32_t _GetFormatPlaneCount(uint32_t ddsformat);

static inline bool IsDepthStencil(uint32_t ddsformat);

struct TextureLoader_DDSHeader
{
    bool isCubeMap;
    uint32_t resDim;
    uint32_t format;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t mipCount;
    uint32_t arraySize;
};

#include <assert.h>

//--------------------------------------------------------------------------------------
static inline bool LoadTextureHeaderFromStream(void const *stream, ptrdiff_t (*stream_read)(void const *stream, void *buf, size_t count), int64_t (*stream_seek)(void const *stream, int64_t offset, int whence),
                                               struct TextureLoader_DDSHeader *texture_header, size_t *texture_data_offset)
{
    assert(texture_header != NULL);
    assert(texture_data_offset != NULL);

    if (stream_seek(stream, 0, TEXTURE_LOADER_STREAM_SEEK_SET) == -1)
    {
        return false;
    }

    uint8_t ddsDataBuf[sizeof(uint32_t) + sizeof(struct DDS_HEADER) + sizeof(struct DDS_HEADER_DXT10)];
    uint8_t const *ddsData = ddsDataBuf;
    {
        ptrdiff_t BytesRead = stream_read(stream, ddsDataBuf, sizeof(uint32_t) + sizeof(struct DDS_HEADER));
        if (BytesRead == -1 || BytesRead < (sizeof(uint32_t) + sizeof(struct DDS_HEADER)))
        {
            return false;
        }
    }

    // DDS files always start with the same magic number ("DDS ")
    uint32_t const *dwMagicNumber = reinterpret_cast<const uint32_t *>(ddsData);
    if ((*dwMagicNumber) != DDS_MAKEFOURCC('D', 'D', 'S', ' '))
    {
        return false;
    }

    struct DDS_HEADER const *header = reinterpret_cast<struct DDS_HEADER const *>(ddsData + sizeof(uint32_t));
    // Verify header to validate DDS file
    if (header->dwSize != sizeof(struct DDS_HEADER) || header->ddspf.dwSize != sizeof(struct DDS_PIXELFORMAT))
    {
        return false;
    }

    // Check for DX10 extension
    struct DDS_HEADER_DXT10 const *d3d10ext = NULL;
    if ((header->ddspf.dwFlags & DDPF_FOURCC) && (DDS_MAKEFOURCC('D', 'X', '1', '0') == header->ddspf.dwFourCC))
    {
        // Must be long enough for both headers and magic value
        ptrdiff_t BytesRead = stream_read(stream, ddsDataBuf + (sizeof(uint32_t) + sizeof(struct DDS_HEADER)), sizeof(struct DDS_HEADER_DXT10));
        if (BytesRead == -1 || BytesRead < sizeof(struct DDS_HEADER_DXT10))
        {
            return false;
        }

        d3d10ext = reinterpret_cast<struct DDS_HEADER_DXT10 const *>(ddsData + (sizeof(uint32_t) + sizeof(struct DDS_HEADER)));
    }

    (*texture_data_offset) = (d3d10ext != NULL) ? (sizeof(uint32_t) + sizeof(struct DDS_HEADER) + sizeof(struct DDS_HEADER_DXT10)) : (sizeof(uint32_t) + sizeof(struct DDS_HEADER));

    texture_header->isCubeMap = false;
    texture_header->resDim = DDS_DIMENSION_TEXTURE2D;
    texture_header->format = DDS_DXGI_FORMAT_UNKNOWN;
    texture_header->width = header->dwWidth;
    texture_header->height = header->dwHeight;
    texture_header->depth = header->dwDepth;
    texture_header->mipCount = (header->dwMipMapCount != 0) ? (header->dwMipMapCount) : 1;
    texture_header->arraySize = 1;

    if (d3d10ext != NULL)
    {
        texture_header->arraySize = d3d10ext->arraySize;
        if (texture_header->arraySize == 0)
        {
            return false;
        }

        switch (d3d10ext->dxgiFormat)
        {
        case DDS_DXGI_FORMAT_AI44:
        case DDS_DXGI_FORMAT_IA44:
        case DDS_DXGI_FORMAT_P8:
        case DDS_DXGI_FORMAT_A8P8:
            return false;
        default:
            if (BitsPerPixel(d3d10ext->dxgiFormat) == 0)
            {
                return false;
            }
        }

        texture_header->format = d3d10ext->dxgiFormat;

        switch (d3d10ext->resourceDimension)
        {
        case DDS_DIMENSION_TEXTURE1D:
        {
            texture_header->resDim = DDS_DIMENSION_TEXTURE1D;
            // D3DX writes 1D textures with a fixed Height of 1
            if ((header->dwFlags & DDSD_HEIGHT) && header->dwHeight != 1)
            {
                return false;
            }
            texture_header->height = 1;
            texture_header->depth = 1;
        }
        break;
        case DDS_DIMENSION_TEXTURE2D:
        {
            texture_header->resDim = DDS_DIMENSION_TEXTURE2D;
            if (d3d10ext->miscFlag & DDS_RESOURCE_MISC_TEXTURECUBE)
            {
                texture_header->arraySize *= 6;
                texture_header->isCubeMap = true;
            }
            texture_header->depth = 1;
        }
        break;
        case DDS_DIMENSION_TEXTURE3D:
        {
            texture_header->resDim = DDS_DIMENSION_TEXTURE3D;
            if (!(header->dwFlags & DDSD_DEPTH))
            {
                return false;
            }
            if (texture_header->arraySize > 1)
            {
                return false;
            }
        }
        break;
        default:
            return false;
        }
    }
    else
    {
        texture_header->format = GetDXGIFormat(&header->ddspf);

        if (!(header->dwFlags & DDSD_DEPTH))
        {
            texture_header->resDim = DDS_DIMENSION_TEXTURE2D;
            texture_header->depth = 1;

            if (header->dwCaps2 & DDSCAPS2_CUBEMAP)
            {
                // We require all six faces to be defined
                if ((header->dwCaps2 & (DDSCAPS2_CUBEMAP_POSITIVEX | DDSCAPS2_CUBEMAP_NEGATIVEX | DDSCAPS2_CUBEMAP_POSITIVEY | DDSCAPS2_CUBEMAP_NEGATIVEY | DDSCAPS2_CUBEMAP_POSITIVEZ | DDSCAPS2_CUBEMAP_NEGATIVEZ)) != (DDSCAPS2_CUBEMAP_POSITIVEX | DDSCAPS2_CUBEMAP_NEGATIVEX | DDSCAPS2_CUBEMAP_POSITIVEY | DDSCAPS2_CUBEMAP_NEGATIVEY | DDSCAPS2_CUBEMAP_POSITIVEZ | DDSCAPS2_CUBEMAP_NEGATIVEZ))
                {
                    return false;
                }

                texture_header->arraySize = 6;
                texture_header->isCubeMap = true;
            }
        }
        else
        {
            texture_header->resDim = DDS_DIMENSION_TEXTURE3D;

            // Note there's no way for a legacy Direct3D 9 DDS to express a '1D' texture
        }

        assert(BitsPerPixel(texture_header->format) != 0);
    }

    return true;
}

//--------------------------------------------------------------------------------------
bool DDSTextureLoader_LoadHeaderFromStream(void const *stream, ptrdiff_t (*stream_read)(void const *stream, void *buf, size_t count), int64_t (*stream_seek)(void const *stream, int64_t offset, int whence),
                                           struct TextureLoader_NeutralHeader *neutral_texture_header, size_t *neutral_header_offset)
{

    assert(neutral_texture_header != NULL);
    assert(neutral_header_offset != NULL);

    struct TextureLoader_DDSHeader dds_texture_header;
    size_t dds_texture_data_offset;
    if (LoadTextureHeaderFromStream(stream, stream_read, stream_seek, &dds_texture_header, &dds_texture_data_offset))
    {
        neutral_texture_header->type = _GetNeutralType(dds_texture_header.resDim);
        assert(TEXTURE_LOADER_TYPE_UNDEFINED != neutral_texture_header->type);

        neutral_texture_header->format = _GetNeutralFormat(dds_texture_header.format);
        assert(TEXTURE_LOADER_FORMAT_UNDEFINED != neutral_texture_header->format);

        neutral_texture_header->width = dds_texture_header.width;
        neutral_texture_header->height = dds_texture_header.height;
        neutral_texture_header->depth = dds_texture_header.depth;
        neutral_texture_header->mipLevels = dds_texture_header.mipCount;
        neutral_texture_header->arrayLayers = dds_texture_header.arraySize;
        neutral_texture_header->isCubeMap = dds_texture_header.isCubeMap;

        (*neutral_header_offset) = dds_texture_data_offset;

        return true;
    }
    else
    {
        return false;
    }
}

//--------------------------------------------------------------------------------------
bool DDSTextureLoader_FillDataFromStream(void const *stream, ptrdiff_t (*stream_read)(void const *stream, void *buf, size_t count), int64_t (*stream_seek)(void const *stream, int64_t offset, int whence),
                                         uint8_t *stagingPointer, size_t NumSubresources, struct TextureLoader_MemcpyDest const *pDest,
                                         struct TextureLoader_NeutralHeader const *neutral_texture_header_validate, size_t const *neutral_header_offset_validate)
{

    struct TextureLoader_DDSHeader texture_header;
    size_t texture_data_offset;
    if (!LoadTextureHeaderFromStream(stream, stream_read, stream_seek, &texture_header, &texture_data_offset))
    {
        return false;
    }

    assert(
        texture_header.isCubeMap == neutral_texture_header_validate->isCubeMap &&
        _GetNeutralType(texture_header.resDim) == neutral_texture_header_validate->type &&
        _GetNeutralFormat(texture_header.format) == neutral_texture_header_validate->format &&
        texture_header.width == neutral_texture_header_validate->width &&
        texture_header.height == neutral_texture_header_validate->height &&
        texture_header.depth == neutral_texture_header_validate->depth &&
        texture_header.mipCount == neutral_texture_header_validate->mipLevels &&
        texture_header.arraySize == neutral_texture_header_validate->arrayLayers //
    );
    assert(texture_data_offset == (*neutral_header_offset_validate));

    //if (stream_seek(stream, texture_data_offset, TEXTURE_LOADER_STREAM_SEEK_SET) == -1)
    //{
    //    return false;
    //}

    // Bound sizes (for security purposes we don't trust DDS file metadata larger than the D3D 11.x hardware requirements)
    if (texture_header.mipCount > 15) //D3D11_REQ_MIP_LEVELS
    {
        return false;
    }

    switch (texture_header.resDim)
    {
    case DDS_DIMENSION_TEXTURE1D:
        if ((texture_header.arraySize > 2048) || //D3D11_REQ_TEXTURE1D_ARRAY_AXIS_DIMENSION
            (texture_header.width > 16384))      //D3D11_REQ_TEXTURE1D_U_DIMENSION
        {
            return false;
        }
        break;

    case DDS_DIMENSION_TEXTURE2D:
        if (!(texture_header.isCubeMap))
        {
            if ((texture_header.arraySize > 2048) || //D3D11_REQ_TEXTURE1D_ARRAY_AXIS_DIMENSION
                (texture_header.width > 16384) ||    //D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION
                (texture_header.height > 16384))     //D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION
            {
                return false;
            }
        }
        else
        {
            // This is the right bound because we set arraySize to (NumCubes*6) above
            if ((texture_header.arraySize > 2048) || //D3D11_REQ_TEXTURE1D_ARRAY_AXIS_DIMENSION
                (texture_header.width > 16384) ||    //D3D11_REQ_TEXTURECUBE_DIMENSION
                (texture_header.height > 16384))     //D3D11_REQ_TEXTURECUBE_DIMENSION
            {
                return false;
            }
        }
        break;

    case DDS_DIMENSION_TEXTURE3D:
        if ((texture_header.arraySize > 1) ||
            (texture_header.width > 2048) ||  //D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION
            (texture_header.height > 2048) || //D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION
            (texture_header.depth > 2048))    //D3D11_REQ_TEXTURE3D_U_V_OR_W_DIMENSION
        {
            return false;
        }
        break;

    default:
        return false;
    }

    uint32_t numberOfPlanes = _GetFormatPlaneCount(texture_header.format);
    if (0 == numberOfPlanes)
    {
        return false;
    }

    if ((numberOfPlanes > 1) && IsDepthStencil(texture_header.format))
    {
        // DirectX 12 uses planes for stencil, DirectX 11 does not
        return false;
    }

    size_t numberOfResources = numberOfPlanes * texture_header.mipCount * texture_header.arraySize;

    if (numberOfResources > 30720) //D3D12_REQ_SUBRESOURCES
    {
        return false;
    }

    if (numberOfResources != NumSubresources)
    {
        return false;
    }

    size_t inputSkipBytes = texture_data_offset;

    // Create the texture
    for (uint32_t arraySlice = 0; arraySlice < texture_header.arraySize; ++arraySlice)
    {
        size_t w = texture_header.width;
        size_t h = texture_header.height;
        size_t d = texture_header.depth;
        for (uint32_t mipSlice = 0; mipSlice < texture_header.mipCount; ++mipSlice)
        {

            size_t NumBytes = 0;
            size_t RowBytes = 0;
            size_t NumRows = 0;
            if (!GetSurfaceInfo(w, h, texture_header.format, &NumBytes, &RowBytes, &NumRows))
            {
                return false;
            }

            if (NumBytes > UINT32_MAX || RowBytes > UINT32_MAX || NumRows > UINT32_MAX)
            {
                return false;
            }

            // GetLoadFunctionsLoadFunctionsMap libANGLE/renderer/load_functions_table_autogen.cpp
            // LoadToNative
            // LoadCompressedToNative
            size_t inputRowSize = RowBytes;
            size_t inputNumRows = NumRows;
            size_t inputSliceSize = NumBytes;
            size_t inputNumSlices = d;

            assert(1 == numberOfPlanes);
            for (uint32_t planeSlice = 0; planeSlice < numberOfPlanes; ++planeSlice)
            {
                // MemcpySubresource d3dx12.h
                uint32_t dstSubresource = TextureLoader_CalcSubresource(mipSlice, arraySlice, planeSlice, texture_header.mipCount, texture_header.arraySize);
                assert(dstSubresource < NumSubresources);

                assert(inputNumSlices == pDest[dstSubresource].outputNumSlices);
                assert(inputNumRows == pDest[dstSubresource].outputNumRows);
                assert(inputRowSize == pDest[dstSubresource].outputRowSize);

                if (inputSliceSize == pDest[dstSubresource].outputSlicePitch && inputRowSize == pDest[dstSubresource].outputRowPitch)
                {
                    int64_t offset_cur;
                    assert((offset_cur = stream_seek(stream, 0, TEXTURE_LOADER_STREAM_SEEK_CUR)) && (stream_seek(stream, inputSkipBytes, TEXTURE_LOADER_STREAM_SEEK_SET) == offset_cur));

                    ptrdiff_t BytesRead = stream_read(stream, stagingPointer + pDest[dstSubresource].stagingOffset, inputSliceSize * inputNumSlices);
                    if (BytesRead == -1 || BytesRead < (inputSliceSize * inputNumSlices))
                    {
                        return false;
                    }
                }
                else if (inputRowSize == pDest[dstSubresource].outputRowPitch)
                {
                    assert(inputSliceSize <= pDest[dstSubresource].outputSlicePitch);

                    for (size_t z = 0; z < inputNumSlices; ++z)
                    {
                        int64_t offset_cur;
                        assert((offset_cur = stream_seek(stream, 0, TEXTURE_LOADER_STREAM_SEEK_CUR)) && (stream_seek(stream, inputSkipBytes + inputSliceSize * z, TEXTURE_LOADER_STREAM_SEEK_SET) == offset_cur));

                        ptrdiff_t BytesRead = stream_read(stream, stagingPointer + (pDest[dstSubresource].stagingOffset + pDest[dstSubresource].outputSlicePitch * z), inputSliceSize);
                        if (BytesRead == -1 || BytesRead < inputSliceSize)
                        {
                            return false;
                        }
                    }
                }
                else
                {
                    assert(inputSliceSize <= pDest[dstSubresource].outputSlicePitch);
                    assert(inputRowSize <= pDest[dstSubresource].outputRowPitch);

                    for (size_t z = 0; z < inputNumSlices; ++z)
                    {
                        for (size_t y = 0; y < inputNumRows; ++y)
                        {
                            int64_t offset_cur;
                            assert((offset_cur = stream_seek(stream, 0, TEXTURE_LOADER_STREAM_SEEK_CUR)) && (stream_seek(stream, inputSkipBytes + inputSliceSize * z + inputRowSize * y, TEXTURE_LOADER_STREAM_SEEK_SET) == offset_cur));

                            ptrdiff_t BytesRead = stream_read(stream, stagingPointer + (pDest[dstSubresource].stagingOffset + pDest[dstSubresource].outputSlicePitch * z + pDest[dstSubresource].outputRowPitch * y), inputRowSize);
                            if (BytesRead == -1 || BytesRead < inputRowSize)
                            {
                                return false;
                            }
                        }
                    }
                }
            }

            inputSkipBytes += inputSliceSize * inputNumSlices;

            w = w >> 1;
            h = h >> 1;
            d = d >> 1;
            if (w == 0)
            {
                w = 1;
            }
            if (h == 0)
            {
                h = 1;
            }
            if (d == 0)
            {
                d = 1;
            }
        }
    }

    uint8_t u_assert_only[1];
    assert(0 == stream_read(stream, u_assert_only, sizeof(uint8_t)));
    return true;
}

//--------------------------------------------------------------------------------------
static inline uint32_t GetDXGIFormat(struct DDS_PIXELFORMAT const *ddpf)
{
    if (ddpf->dwFlags & DDPF_RGB)
    {
        // Note that sRGB formats are written using the "DX10" extended header

        switch (ddpf->dwRGBBitCount)
        {
        case 32:
            if (ddpf->dwRBitMask == 0x000000ff && ddpf->dwGBitMask == 0x0000ff00 && ddpf->dwBBitMask == 0x00ff0000 && ddpf->dwABitMask == 0xff000000)
            {
                return DDS_DXGI_FORMAT_R8G8B8A8_UNORM;
            }

            if (ddpf->dwRBitMask == 0x00ff0000 && ddpf->dwGBitMask == 0x0000ff00 && ddpf->dwBBitMask == 0x000000ff && ddpf->dwABitMask == 0xff000000)
            {
                return DDS_DXGI_FORMAT_B8G8R8A8_UNORM;
            }

            if (ddpf->dwRBitMask == 0x00ff0000 && ddpf->dwGBitMask == 0x0000ff00 && ddpf->dwBBitMask == 0x000000ff && ddpf->dwABitMask == 0x00000000)
            {
                return DDS_DXGI_FORMAT_B8G8R8X8_UNORM;
            }

            // No DXGI format maps to ISBITMASK(0x000000ff,0x0000ff00,0x00ff0000,0x00000000) aka D3DFMT_X8B8G8R8

            // Note that many common DDS reader/writers (including D3DX) swap the
            // the RED/BLUE masks for 10:10:10:2 formats. We assume
            // below that the 'backwards' header mask is being used since it is most
            // likely written by D3DX. The more robust solution is to use the 'DX10'
            // header extension and specify the DXGI_FORMAT_R10G10B10A2_UNORM format directly

            // For 'correct' writers, this should be 0x000003ff,0x000ffc00,0x3ff00000 for RGB data
            if (ddpf->dwRBitMask == 0x3ff00000 && ddpf->dwGBitMask == 0x000ffc00 && ddpf->dwBBitMask == 0x000003ff && ddpf->dwABitMask == 0xc0000000)
            {
                return DDS_DXGI_FORMAT_R10G10B10A2_UNORM;
            }

            // No DXGI format maps to ISBITMASK(0x000003ff,0x000ffc00,0x3ff00000,0xc0000000) aka D3DFMT_A2R10G10B10

            if (ddpf->dwRBitMask == 0x0000ffff && ddpf->dwGBitMask == 0xffff0000 && ddpf->dwBBitMask == 0x00000000 && ddpf->dwABitMask == 0x00000000)
            {
                return DDS_DXGI_FORMAT_R16G16_UNORM;
            }

            if (ddpf->dwRBitMask == 0xffffffff && ddpf->dwGBitMask == 0x00000000 && ddpf->dwBBitMask == 0x00000000 && ddpf->dwABitMask == 0x00000000)
            {
                // Only 32-bit color channel format in D3D9 was R32F
                return DDS_DXGI_FORMAT_R32_FLOAT; // D3DX writes this out as a FourCC of 114
            }
            break;

        case 24:
            // No 24bpp DXGI formats aka D3DFMT_R8G8B8
            break;

        case 16:
            if (ddpf->dwRBitMask == 0x7c00 && ddpf->dwGBitMask == 0x03e0 && ddpf->dwBBitMask == 0x001f && ddpf->dwABitMask == 0x8000)
            {
                return DDS_DXGI_FORMAT_B5G5R5A1_UNORM;
            }
            if (ddpf->dwRBitMask == 0xf800 && ddpf->dwGBitMask == 0x07e0 && ddpf->dwBBitMask == 0x001f && ddpf->dwABitMask == 0x0000)
            {
                return DDS_DXGI_FORMAT_B5G6R5_UNORM;
            }

            // No DXGI format maps to ISBITMASK(0x7c00,0x03e0,0x001f,0x0000) aka D3DFMT_X1R5G5B5

            if (ddpf->dwRBitMask == 0x0f00 && ddpf->dwGBitMask == 0x00f0 && ddpf->dwBBitMask == 0x000f && ddpf->dwABitMask == 0xf000)
            {
                return DDS_DXGI_FORMAT_B4G4R4A4_UNORM;
            }

            // No DXGI format maps to ISBITMASK(0x0f00,0x00f0,0x000f,0x0000) aka D3DFMT_X4R4G4B4

            // No 3:3:2, 3:3:2:8, or paletted DXGI formats aka D3DFMT_A8R3G3B2, D3DFMT_R3G3B2, D3DFMT_P8, D3DFMT_A8P8, etc.
            break;
        }
    }
    else if (ddpf->dwFlags & DDPF_LUMINANCE)
    {
        if (8 == ddpf->dwRGBBitCount)
        {
            if (ddpf->dwRBitMask == 0x000000ff && ddpf->dwGBitMask == 0x00000000 && ddpf->dwBBitMask == 0x00000000 && ddpf->dwABitMask == 0x00000000)
            {
                return DDS_DXGI_FORMAT_R8_UNORM; // D3DX10/11 writes this out as DX10 extension
            }

            // No DXGI format maps to ISBITMASK(0x0f,0x00,0x00,0xf0) aka D3DFMT_A4L4

            if (ddpf->dwRBitMask == 0x000000ff && ddpf->dwGBitMask == 0x00000000 && ddpf->dwBBitMask == 0x00000000 && ddpf->dwABitMask == 0x0000ff00)
            {
                return DDS_DXGI_FORMAT_R8G8_UNORM; // Some DDS writers assume the bitcount should be 8 instead of 16
            }
        }

        if (16 == ddpf->dwRGBBitCount)
        {
            if (ddpf->dwRBitMask == 0x0000ffff && ddpf->dwGBitMask == 0x00000000 && ddpf->dwBBitMask == 0x00000000 && ddpf->dwABitMask == 0x00000000)
            {
                return DDS_DXGI_FORMAT_R16_UNORM; // D3DX10/11 writes this out as DX10 extension
            }

            if (ddpf->dwRBitMask == 0x000000ff && ddpf->dwGBitMask == 0x00000000 && ddpf->dwBBitMask == 0x00000000 && ddpf->dwABitMask == 0x0000ff00)
            {
                return DDS_DXGI_FORMAT_R8G8_UNORM; // D3DX10/11 writes this out as DX10 extension
            }
        }
    }
    else if (ddpf->dwFlags & DDPF_ALPHA)
    {
        if (8 == ddpf->dwRGBBitCount)
        {
            return DDS_DXGI_FORMAT_A8_UNORM;
        }
    }
    else if (ddpf->dwFlags & DDPF_BUMPDUDV)
    {
        if (16 == ddpf->dwRGBBitCount)
        {
            if (ddpf->dwRBitMask == 0x00ff && ddpf->dwGBitMask == 0xff00 && ddpf->dwBBitMask == 0x0000 && ddpf->dwABitMask == 0x0000)
            {
                return DDS_DXGI_FORMAT_R8G8_SNORM; // D3DX10/11 writes this out as DX10 extension
            }
        }

        if (32 == ddpf->dwRGBBitCount)
        {
            if (ddpf->dwRBitMask == 0x000000ff && ddpf->dwGBitMask == 0x0000ff00 && ddpf->dwBBitMask == 0x00ff0000 && ddpf->dwABitMask == 0xff000000)
            {
                return DDS_DXGI_FORMAT_R8G8B8A8_SNORM; // D3DX10/11 writes this out as DX10 extension
            }

            if (ddpf->dwRBitMask == 0x0000ffff && ddpf->dwGBitMask == 0xffff0000 && ddpf->dwBBitMask == 0x00000000 && ddpf->dwABitMask == 0x00000000)
            {
                return DDS_DXGI_FORMAT_R16G16_SNORM; // D3DX10/11 writes this out as DX10 extension
            }

            // No DXGI format maps to ISBITMASK(0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000) aka D3DFMT_A2W10V10U10
        }
    }
    else if (ddpf->dwFlags & DDPF_FOURCC)
    {
        if (DDS_MAKEFOURCC('D', 'X', 'T', '1') == ddpf->dwFourCC)
        {
            return DDS_DXGI_FORMAT_BC1_UNORM;
        }
        if (DDS_MAKEFOURCC('D', 'X', 'T', '3') == ddpf->dwFourCC)
        {
            return DDS_DXGI_FORMAT_BC2_UNORM;
        }
        if (DDS_MAKEFOURCC('D', 'X', 'T', '5') == ddpf->dwFourCC)
        {
            return DDS_DXGI_FORMAT_BC3_UNORM;
        }

        // While pre-multiplied alpha isn't directly supported by the DXGI formats,
        // they are basically the same as these BC formats so they can be mapped
        if (DDS_MAKEFOURCC('D', 'X', 'T', '2') == ddpf->dwFourCC)
        {
            return DDS_DXGI_FORMAT_BC2_UNORM;
        }
        if (DDS_MAKEFOURCC('D', 'X', 'T', '4') == ddpf->dwFourCC)
        {
            return DDS_DXGI_FORMAT_BC3_UNORM;
        }

        if (DDS_MAKEFOURCC('A', 'T', 'I', '1') == ddpf->dwFourCC)
        {
            return DDS_DXGI_FORMAT_BC4_UNORM;
        }
        if (DDS_MAKEFOURCC('B', 'C', '4', 'U') == ddpf->dwFourCC)
        {
            return DDS_DXGI_FORMAT_BC4_UNORM;
        }
        if (DDS_MAKEFOURCC('B', 'C', '4', 'S') == ddpf->dwFourCC)
        {
            return DDS_DXGI_FORMAT_BC4_SNORM;
        }

        if (DDS_MAKEFOURCC('A', 'T', 'I', '2') == ddpf->dwFourCC)
        {
            return DDS_DXGI_FORMAT_BC5_UNORM;
        }
        if (DDS_MAKEFOURCC('B', 'C', '5', 'U') == ddpf->dwFourCC)
        {
            return DDS_DXGI_FORMAT_BC5_UNORM;
        }
        if (DDS_MAKEFOURCC('B', 'C', '5', 'S') == ddpf->dwFourCC)
        {
            return DDS_DXGI_FORMAT_BC5_SNORM;
        }

        // BC6H and BC7 are written using the "DX10" extended header

        if (DDS_MAKEFOURCC('R', 'G', 'B', 'G') == ddpf->dwFourCC)
        {
            return DDS_DXGI_FORMAT_R8G8_B8G8_UNORM;
        }
        if (DDS_MAKEFOURCC('G', 'R', 'G', 'B') == ddpf->dwFourCC)
        {
            return DDS_DXGI_FORMAT_G8R8_G8B8_UNORM;
        }

        if (DDS_MAKEFOURCC('Y', 'U', 'Y', '2') == ddpf->dwFourCC)
        {
            return DDS_DXGI_FORMAT_YUY2;
        }

        // Check for D3DFORMAT enums being set here
        switch (ddpf->dwFourCC)
        {
        case 36: // D3DFMT_A16B16G16R16
            return DDS_DXGI_FORMAT_R16G16B16A16_UNORM;

        case 110: // D3DFMT_Q16W16V16U16
            return DDS_DXGI_FORMAT_R16G16B16A16_SNORM;

        case 111: // D3DFMT_R16F
            return DDS_DXGI_FORMAT_R16_FLOAT;

        case 112: // D3DFMT_G16R16F
            return DDS_DXGI_FORMAT_R16G16_FLOAT;

        case 113: // D3DFMT_A16B16G16R16F
            return DDS_DXGI_FORMAT_R16G16B16A16_FLOAT;

        case 114: // D3DFMT_R32F
            return DDS_DXGI_FORMAT_R32_FLOAT;

        case 115: // D3DFMT_G32R32F
            return DDS_DXGI_FORMAT_R32G32_FLOAT;

        case 116: // D3DFMT_A32B32G32R32F
            return DDS_DXGI_FORMAT_R32G32B32A32_FLOAT;
        }
    }

    return DDS_DXGI_FORMAT_UNKNOWN;
}

//--------------------------------------------------------------------------------------
// Get surface information for a particular format
//--------------------------------------------------------------------------------------
#include <algorithm>
static inline bool GetSurfaceInfo(size_t width, size_t height, uint32_t fmt, size_t *outNumBytes, size_t *outRowBytes, size_t *outNumRows)
{
    assert(outNumBytes != NULL);
    assert(outRowBytes != NULL);
    assert(outNumRows != NULL);

    uint64_t numBytes = 0;
    uint64_t rowBytes = 0;
    uint64_t numRows = 0;

    bool bc = false;
    bool packed = false;
    bool planar = false;
    size_t bpe = 0;

    switch (fmt)
    {
    case DDS_DXGI_FORMAT_BC1_TYPELESS:
    case DDS_DXGI_FORMAT_BC1_UNORM:
    case DDS_DXGI_FORMAT_BC1_UNORM_SRGB:
    case DDS_DXGI_FORMAT_BC4_TYPELESS:
    case DDS_DXGI_FORMAT_BC4_UNORM:
    case DDS_DXGI_FORMAT_BC4_SNORM:
        bc = true;
        bpe = 8;
        break;

    case DDS_DXGI_FORMAT_BC2_TYPELESS:
    case DDS_DXGI_FORMAT_BC2_UNORM:
    case DDS_DXGI_FORMAT_BC2_UNORM_SRGB:
    case DDS_DXGI_FORMAT_BC3_TYPELESS:
    case DDS_DXGI_FORMAT_BC3_UNORM:
    case DDS_DXGI_FORMAT_BC3_UNORM_SRGB:
    case DDS_DXGI_FORMAT_BC5_TYPELESS:
    case DDS_DXGI_FORMAT_BC5_UNORM:
    case DDS_DXGI_FORMAT_BC5_SNORM:
    case DDS_DXGI_FORMAT_BC6H_TYPELESS:
    case DDS_DXGI_FORMAT_BC6H_UF16:
    case DDS_DXGI_FORMAT_BC6H_SF16:
    case DDS_DXGI_FORMAT_BC7_TYPELESS:
    case DDS_DXGI_FORMAT_BC7_UNORM:
    case DDS_DXGI_FORMAT_BC7_UNORM_SRGB:
        bc = true;
        bpe = 16;
        break;

    case DDS_DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DDS_DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DDS_DXGI_FORMAT_YUY2:
        packed = true;
        bpe = 4;
        break;

    case DDS_DXGI_FORMAT_Y210:
    case DDS_DXGI_FORMAT_Y216:
        packed = true;
        bpe = 8;
        break;

    case DDS_DXGI_FORMAT_NV12:
    case DDS_DXGI_FORMAT_420_OPAQUE:
    case DDS_DXGI_FORMAT_P208:
        planar = true;
        bpe = 2;
        break;

    case DDS_DXGI_FORMAT_P010:
    case DDS_DXGI_FORMAT_P016:
        planar = true;
        bpe = 4;
        break;

    default:
        break;
    }

    if (bc)
    {
        uint64_t numBlocksWide = 0;
        if (width > 0)
        {
            numBlocksWide = std::max<uint64_t>(1u, (uint64_t(width) + 3u) / 4u);
        }
        uint64_t numBlocksHigh = 0;
        if (height > 0)
        {
            numBlocksHigh = std::max<uint64_t>(1u, (uint64_t(height) + 3u) / 4u);
        }
        rowBytes = numBlocksWide * bpe;
        numRows = numBlocksHigh;
        numBytes = rowBytes * numBlocksHigh;
    }
    else if (packed)
    {
        rowBytes = ((uint64_t(width) + 1u) >> 1) * bpe;
        numRows = uint64_t(height);
        numBytes = rowBytes * height;
    }
    else if (fmt == DDS_DXGI_FORMAT_NV11)
    {
        rowBytes = ((uint64_t(width) + 3u) >> 2) * 4u;
        numRows = uint64_t(height) * 2u; // Direct3D makes this simplifying assumption, although it is larger than the 4:1:1 data
        numBytes = rowBytes * numRows;
    }
    else if (planar)
    {
        rowBytes = ((uint64_t(width) + 1u) >> 1) * bpe;
        numBytes = (rowBytes * uint64_t(height)) + ((rowBytes * uint64_t(height) + 1u) >> 1);
        numRows = height + ((uint64_t(height) + 1u) >> 1);
    }
    else
    {
        size_t bpp = BitsPerPixel(fmt);
        if (0 == bpp)
        {
            return false;
        }
        rowBytes = (uint64_t(width) * bpp + 7u) / 8u; // round up to nearest byte
        numRows = uint64_t(height);
        numBytes = rowBytes * height;
    }

#if defined(_MSC_VER) //https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros
#if defined(_M_IX86) || defined(_M_ARM)
    static_assert(sizeof(size_t) == 4, "Not a 32-bit platform!");
    if (numBytes > UINT32_MAX || rowBytes > UINT32_MAX || numRows > UINT32_MAX)
    {
        return false;
    }
#elif defined(_M_IX86) || defined(_M_ARM)
    static_assert(sizeof(size_t) == 8, "Not a 64-bit platform!");
#else
#error Unknown Architecture //未知的架构
#endif
#elif defined(__GNUC__) //https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html
#if defined(__i386__) || defined(__arm__)
    static_assert(sizeof(size_t) == 4, "Not a 32-bit platform!");
    if (numBytes > UINT32_MAX || rowBytes > UINT32_MAX || numRows > UINT32_MAX)
    {
        return false;
    }
#elif defined(__x86_64__) || defined(__aarch64__)
    static_assert(sizeof(size_t) == 8, "Not a 64-bit platform!");
#else
#error Unknown Architecture //未知的架构
#endif
#else
#error Unknown Compiler
#endif

    (*outNumBytes) = static_cast<size_t>(numBytes);
    (*outRowBytes) = static_cast<size_t>(rowBytes);
    (*outNumRows) = static_cast<size_t>(numRows);
    return true;
}

//--------------------------------------------------------------------------------------
// Return the BPP for a particular format
//--------------------------------------------------------------------------------------
static inline size_t BitsPerPixel(uint32_t fmt)
{
    switch (fmt)
    {
    case DDS_DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DDS_DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DDS_DXGI_FORMAT_R32G32B32A32_UINT:
    case DDS_DXGI_FORMAT_R32G32B32A32_SINT:
        return 128;

    case DDS_DXGI_FORMAT_R32G32B32_TYPELESS:
    case DDS_DXGI_FORMAT_R32G32B32_FLOAT:
    case DDS_DXGI_FORMAT_R32G32B32_UINT:
    case DDS_DXGI_FORMAT_R32G32B32_SINT:
        return 96;

    case DDS_DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DDS_DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DDS_DXGI_FORMAT_R16G16B16A16_UNORM:
    case DDS_DXGI_FORMAT_R16G16B16A16_UINT:
    case DDS_DXGI_FORMAT_R16G16B16A16_SNORM:
    case DDS_DXGI_FORMAT_R16G16B16A16_SINT:
    case DDS_DXGI_FORMAT_R32G32_TYPELESS:
    case DDS_DXGI_FORMAT_R32G32_FLOAT:
    case DDS_DXGI_FORMAT_R32G32_UINT:
    case DDS_DXGI_FORMAT_R32G32_SINT:
    case DDS_DXGI_FORMAT_R32G8X24_TYPELESS:
    case DDS_DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DDS_DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DDS_DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
    case DDS_DXGI_FORMAT_Y416:
    case DDS_DXGI_FORMAT_Y210:
    case DDS_DXGI_FORMAT_Y216:
        return 64;

    case DDS_DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DDS_DXGI_FORMAT_R10G10B10A2_UNORM:
    case DDS_DXGI_FORMAT_R10G10B10A2_UINT:
    case DDS_DXGI_FORMAT_R11G11B10_FLOAT:
    case DDS_DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DDS_DXGI_FORMAT_R8G8B8A8_UNORM:
    case DDS_DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DDS_DXGI_FORMAT_R8G8B8A8_UINT:
    case DDS_DXGI_FORMAT_R8G8B8A8_SNORM:
    case DDS_DXGI_FORMAT_R8G8B8A8_SINT:
    case DDS_DXGI_FORMAT_R16G16_TYPELESS:
    case DDS_DXGI_FORMAT_R16G16_FLOAT:
    case DDS_DXGI_FORMAT_R16G16_UNORM:
    case DDS_DXGI_FORMAT_R16G16_UINT:
    case DDS_DXGI_FORMAT_R16G16_SNORM:
    case DDS_DXGI_FORMAT_R16G16_SINT:
    case DDS_DXGI_FORMAT_R32_TYPELESS:
    case DDS_DXGI_FORMAT_D32_FLOAT:
    case DDS_DXGI_FORMAT_R32_FLOAT:
    case DDS_DXGI_FORMAT_R32_UINT:
    case DDS_DXGI_FORMAT_R32_SINT:
    case DDS_DXGI_FORMAT_R24G8_TYPELESS:
    case DDS_DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DDS_DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DDS_DXGI_FORMAT_X24_TYPELESS_G8_UINT:
    case DDS_DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
    case DDS_DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DDS_DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DDS_DXGI_FORMAT_B8G8R8A8_UNORM:
    case DDS_DXGI_FORMAT_B8G8R8X8_UNORM:
    case DDS_DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
    case DDS_DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DDS_DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DDS_DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DDS_DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
    case DDS_DXGI_FORMAT_AYUV:
    case DDS_DXGI_FORMAT_Y410:
    case DDS_DXGI_FORMAT_YUY2:
        return 32;

    case DDS_DXGI_FORMAT_P010:
    case DDS_DXGI_FORMAT_P016:
    case DDS_DXGI_FORMAT_V408:
        return 24;

    case DDS_DXGI_FORMAT_R8G8_TYPELESS:
    case DDS_DXGI_FORMAT_R8G8_UNORM:
    case DDS_DXGI_FORMAT_R8G8_UINT:
    case DDS_DXGI_FORMAT_R8G8_SNORM:
    case DDS_DXGI_FORMAT_R8G8_SINT:
    case DDS_DXGI_FORMAT_R16_TYPELESS:
    case DDS_DXGI_FORMAT_R16_FLOAT:
    case DDS_DXGI_FORMAT_D16_UNORM:
    case DDS_DXGI_FORMAT_R16_UNORM:
    case DDS_DXGI_FORMAT_R16_UINT:
    case DDS_DXGI_FORMAT_R16_SNORM:
    case DDS_DXGI_FORMAT_R16_SINT:
    case DDS_DXGI_FORMAT_B5G6R5_UNORM:
    case DDS_DXGI_FORMAT_B5G5R5A1_UNORM:
    case DDS_DXGI_FORMAT_A8P8:
    case DDS_DXGI_FORMAT_B4G4R4A4_UNORM:
    case DDS_DXGI_FORMAT_P208:
    case DDS_DXGI_FORMAT_V208:
        return 16;

    case DDS_DXGI_FORMAT_NV12:
    case DDS_DXGI_FORMAT_420_OPAQUE:
    case DDS_DXGI_FORMAT_NV11:
        return 12;

    case DDS_DXGI_FORMAT_R8_TYPELESS:
    case DDS_DXGI_FORMAT_R8_UNORM:
    case DDS_DXGI_FORMAT_R8_UINT:
    case DDS_DXGI_FORMAT_R8_SNORM:
    case DDS_DXGI_FORMAT_R8_SINT:
    case DDS_DXGI_FORMAT_A8_UNORM:
    case DDS_DXGI_FORMAT_AI44:
    case DDS_DXGI_FORMAT_IA44:
    case DDS_DXGI_FORMAT_P8:
        return 8;

    case DDS_DXGI_FORMAT_R1_UNORM:
        return 1;

    case DDS_DXGI_FORMAT_BC1_TYPELESS:
    case DDS_DXGI_FORMAT_BC1_UNORM:
    case DDS_DXGI_FORMAT_BC1_UNORM_SRGB:
    case DDS_DXGI_FORMAT_BC4_TYPELESS:
    case DDS_DXGI_FORMAT_BC4_UNORM:
    case DDS_DXGI_FORMAT_BC4_SNORM:
        return 4;

    case DDS_DXGI_FORMAT_BC2_TYPELESS:
    case DDS_DXGI_FORMAT_BC2_UNORM:
    case DDS_DXGI_FORMAT_BC2_UNORM_SRGB:
    case DDS_DXGI_FORMAT_BC3_TYPELESS:
    case DDS_DXGI_FORMAT_BC3_UNORM:
    case DDS_DXGI_FORMAT_BC3_UNORM_SRGB:
    case DDS_DXGI_FORMAT_BC5_TYPELESS:
    case DDS_DXGI_FORMAT_BC5_UNORM:
    case DDS_DXGI_FORMAT_BC5_SNORM:
    case DDS_DXGI_FORMAT_BC6H_TYPELESS:
    case DDS_DXGI_FORMAT_BC6H_UF16:
    case DDS_DXGI_FORMAT_BC6H_SF16:
    case DDS_DXGI_FORMAT_BC7_TYPELESS:
    case DDS_DXGI_FORMAT_BC7_UNORM:
    case DDS_DXGI_FORMAT_BC7_UNORM_SRGB:
        return 8;

    default:
        return 0;
    }
}

//--------------------------------------------------------------------------------------
static uint32_t const gDdsToNeutralTypeMap[] = {
    TEXTURE_LOADER_TYPE_UNDEFINED,
    TEXTURE_LOADER_TYPE_UNDEFINED,
    TEXTURE_LOADER_TYPE_1D,
    TEXTURE_LOADER_TYPE_2D,
    TEXTURE_LOADER_TYPE_3D,
};
static_assert(DDS_DIMENSION_RANGE == (sizeof(gDdsToNeutralTypeMap) / sizeof(gDdsToNeutralTypeMap[0])), "gDdsToNeutralTypeMap may not match!");

static inline uint32_t _GetNeutralType(uint32_t ddstype)
{
    assert(ddstype < (sizeof(gDdsToNeutralTypeMap) / sizeof(gDdsToNeutralTypeMap[0])));
    return gDdsToNeutralTypeMap[ddstype];
}

//--------------------------------------------------------------------------------------
static uint32_t const gDdsToNeutralFormatMap[] = {
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_UNKNOWN
    TEXTURE_LOADER_FORMAT_R32G32B32A32_SFLOAT,      //DDS_DXGI_FORMAT_R32G32B32A32_TYPELESS
    TEXTURE_LOADER_FORMAT_R32G32B32A32_SFLOAT,      //DDS_DXGI_FORMAT_R32G32B32A32_FLOAT
    TEXTURE_LOADER_FORMAT_R32G32B32A32_UINT,        //DDS_DXGI_FORMAT_R32G32B32A32_UINT
    TEXTURE_LOADER_FORMAT_R32G32B32A32_SINT,        //DDS_DXGI_FORMAT_R32G32B32A32_SINT
    TEXTURE_LOADER_FORMAT_R32G32B32_SFLOAT,         //DDS_DXGI_FORMAT_R32G32B32_TYPELESS
    TEXTURE_LOADER_FORMAT_R32G32B32_SFLOAT,         //DDS_DXGI_FORMAT_R32G32B32_FLOAT
    TEXTURE_LOADER_FORMAT_R32G32B32_UINT,           //DDS_DXGI_FORMAT_R32G32B32_UINT
    TEXTURE_LOADER_FORMAT_R32G32B32_SINT,           //DDS_DXGI_FORMAT_R32G32B32_SINT
    TEXTURE_LOADER_FORMAT_R16G16B16A16_SFLOAT,      //DDS_DXGI_FORMAT_R16G16B16A16_TYPELESS
    TEXTURE_LOADER_FORMAT_R16G16B16A16_SFLOAT,      //DDS_DXGI_FORMAT_R16G16B16A16_FLOAT
    TEXTURE_LOADER_FORMAT_R16G16B16A16_UNORM,       //DDS_DXGI_FORMAT_R16G16B16A16_UNORM
    TEXTURE_LOADER_FORMAT_R16G16B16A16_UINT,        //DDS_DXGI_FORMAT_R16G16B16A16_UINT
    TEXTURE_LOADER_FORMAT_R16G16B16A16_SNORM,       //DDS_DXGI_FORMAT_R16G16B16A16_SNORM
    TEXTURE_LOADER_FORMAT_R16G16B16A16_SINT,        //DDS_DXGI_FORMAT_R16G16B16A16_SINT
    TEXTURE_LOADER_FORMAT_R32G32_SFLOAT,            //DDS_DXGI_FORMAT_R32G32_TYPELESS
    TEXTURE_LOADER_FORMAT_R32G32_SFLOAT,            //DDS_DXGI_FORMAT_R32G32_FLOAT
    TEXTURE_LOADER_FORMAT_R32G32_UINT,              //DDS_DXGI_FORMAT_R32G32_UINT
    TEXTURE_LOADER_FORMAT_R32G32_SINT,              //DDS_DXGI_FORMAT_R32G32_SINT
    TEXTURE_LOADER_FORMAT_D32_SFLOAT_S8_UINT,       //DDS_DXGI_FORMAT_R32G8X24_TYPELESS
    TEXTURE_LOADER_FORMAT_D32_SFLOAT_S8_UINT,       //DDS_DXGI_FORMAT_D32_FLOAT_S8X24_UINT
    TEXTURE_LOADER_FORMAT_D32_SFLOAT_S8_UINT,       //DDS_DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS
    TEXTURE_LOADER_FORMAT_D32_SFLOAT_S8_UINT,       //DDS_DXGI_FORMAT_X32_TYPELESS_G8X24_UINT
    TEXTURE_LOADER_FORMAT_D32_SFLOAT_S8_UINT,       //DDS_DXGI_FORMAT_R10G10B10A2_TYPELESS
    TEXTURE_LOADER_FORMAT_A2R10G10B10_UNORM_PACK32, //DDS_DXGI_FORMAT_R10G10B10A2_UNORM
    TEXTURE_LOADER_FORMAT_A2R10G10B10_UINT_PACK32,  //DDS_DXGI_FORMAT_R10G10B10A2_UINT
    TEXTURE_LOADER_FORMAT_B10G11R11_UFLOAT_PACK32,  //DDS_DXGI_FORMAT_R11G11B10_FLOAT
    TEXTURE_LOADER_FORMAT_R8G8B8A8_UNORM,           //DDS_DXGI_FORMAT_R8G8B8A8_TYPELESS
    TEXTURE_LOADER_FORMAT_R8G8B8A8_UNORM,           //DDS_DXGI_FORMAT_R8G8B8A8_UNORM
    TEXTURE_LOADER_FORMAT_R8G8B8A8_SRGB,            //DDS_DXGI_FORMAT_R8G8B8A8_UNORM_SRGB
    TEXTURE_LOADER_FORMAT_R8G8B8A8_UINT,            //DDS_DXGI_FORMAT_R8G8B8A8_UINT
    TEXTURE_LOADER_FORMAT_R8G8B8A8_SNORM,           //DDS_DXGI_FORMAT_R8G8B8A8_SNORM
    TEXTURE_LOADER_FORMAT_R8G8B8A8_SINT,            //DDS_DXGI_FORMAT_R8G8B8A8_SINT
    TEXTURE_LOADER_FORMAT_R16G16_SFLOAT,            //DDS_DXGI_FORMAT_R16G16_TYPELESS
    TEXTURE_LOADER_FORMAT_R16G16_SFLOAT,            //DDS_DXGI_FORMAT_R16G16_FLOAT
    TEXTURE_LOADER_FORMAT_R16G16_UNORM,             //DDS_DXGI_FORMAT_R16G16_UNORM
    TEXTURE_LOADER_FORMAT_R16G16_UINT,              //DDS_DXGI_FORMAT_R16G16_UINT
    TEXTURE_LOADER_FORMAT_R16G16_SNORM,             //DDS_DXGI_FORMAT_R16G16_SNORM
    TEXTURE_LOADER_FORMAT_R16G16_SINT,              //DDS_DXGI_FORMAT_R16G16_SINT
    TEXTURE_LOADER_FORMAT_R32_SFLOAT,               //DDS_DXGI_FORMAT_R32_TYPELESS
    TEXTURE_LOADER_FORMAT_D32_SFLOAT,               //DDS_DXGI_FORMAT_D32_FLOAT
    TEXTURE_LOADER_FORMAT_R32_SFLOAT,               //DDS_DXGI_FORMAT_R32_FLOAT
    TEXTURE_LOADER_FORMAT_R32_UINT,                 //DDS_DXGI_FORMAT_R32_UINT
    TEXTURE_LOADER_FORMAT_R32_SINT,                 //DDS_DXGI_FORMAT_R32_SINT
    TEXTURE_LOADER_FORMAT_D24_UNORM_S8_UINT,        //DDS_DXGI_FORMAT_R24G8_TYPELESS
    TEXTURE_LOADER_FORMAT_D24_UNORM_S8_UINT,        //DDS_DXGI_FORMAT_D24_UNORM_S8_UINT
    TEXTURE_LOADER_FORMAT_D24_UNORM_S8_UINT,        //DDS_DXGI_FORMAT_R24_UNORM_X8_TYPELESS
    TEXTURE_LOADER_FORMAT_D24_UNORM_S8_UINT,        //DDS_DXGI_FORMAT_X24_TYPELESS_G8_UINT
    TEXTURE_LOADER_FORMAT_R8G8_UNORM,               //DDS_DXGI_FORMAT_R8G8_TYPELESS
    TEXTURE_LOADER_FORMAT_R8G8_UNORM,               //DDS_DXGI_FORMAT_R8G8_UNORM
    TEXTURE_LOADER_FORMAT_R8G8_UINT,                //DDS_DXGI_FORMAT_R8G8_UINT
    TEXTURE_LOADER_FORMAT_R8G8_SNORM,               //DDS_DXGI_FORMAT_R8G8_SNORM
    TEXTURE_LOADER_FORMAT_R8G8_SINT,                //DDS_DXGI_FORMAT_R8G8_SINT
    TEXTURE_LOADER_FORMAT_R16_SFLOAT,               //DDS_DXGI_FORMAT_R16_TYPELESS
    TEXTURE_LOADER_FORMAT_R16_SFLOAT,               //DDS_DXGI_FORMAT_R16_FLOAT
    TEXTURE_LOADER_FORMAT_D16_UNORM,                //DDS_DXGI_FORMAT_D16_UNORM
    TEXTURE_LOADER_FORMAT_R16_UNORM,                //DDS_DXGI_FORMAT_R16_UNORM
    TEXTURE_LOADER_FORMAT_R16_UINT,                 //DDS_DXGI_FORMAT_R16_UINT
    TEXTURE_LOADER_FORMAT_R16_SNORM,                //DDS_DXGI_FORMAT_R16_SNORM
    TEXTURE_LOADER_FORMAT_R16_SINT,                 //DDS_DXGI_FORMAT_R16_SINT
    TEXTURE_LOADER_FORMAT_R8_UNORM,                 //DDS_DXGI_FORMAT_R8_TYPELESS
    TEXTURE_LOADER_FORMAT_R8_UNORM,                 //DDS_DXGI_FORMAT_R8_UNORM
    TEXTURE_LOADER_FORMAT_R8_UINT,                  //DDS_DXGI_FORMAT_R8_UINT
    TEXTURE_LOADER_FORMAT_R8_SNORM,                 //DDS_DXGI_FORMAT_R8_SNORM
    TEXTURE_LOADER_FORMAT_R8_SINT,                  //DDS_DXGI_FORMAT_R8_SINT
    TEXTURE_LOADER_FORMAT_R8_UNORM,                 //DDS_DXGI_FORMAT_A8_UNORM
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_R1_UNORM
    TEXTURE_LOADER_FORMAT_E5B9G9R9_UFLOAT_PACK32,   //DDS_DXGI_FORMAT_R9G9B9E5_SHAREDEXP
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_R8G8_B8G8_UNORM
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_G8R8_G8B8_UNORM
    TEXTURE_LOADER_FORMAT_BC1_RGBA_UNORM_BLOCK,     //DDS_DXGI_FORMAT_BC1_TYPELESS
    TEXTURE_LOADER_FORMAT_BC1_RGBA_UNORM_BLOCK,     //DDS_DXGI_FORMAT_BC1_UNORM
    TEXTURE_LOADER_FORMAT_BC1_RGBA_SRGB_BLOCK,      //DDS_DXGI_FORMAT_BC1_UNORM_SRGB
    TEXTURE_LOADER_FORMAT_BC2_UNORM_BLOCK,          //DDS_DXGI_FORMAT_BC2_TYPELESS
    TEXTURE_LOADER_FORMAT_BC2_UNORM_BLOCK,          //DDS_DXGI_FORMAT_BC2_UNORM
    TEXTURE_LOADER_FORMAT_BC2_SRGB_BLOCK,           //DDS_DXGI_FORMAT_BC2_UNORM_SRGB
    TEXTURE_LOADER_FORMAT_BC3_UNORM_BLOCK,          //DDS_DXGI_FORMAT_BC3_TYPELESS
    TEXTURE_LOADER_FORMAT_BC3_UNORM_BLOCK,          //DDS_DXGI_FORMAT_BC3_UNORM
    TEXTURE_LOADER_FORMAT_BC3_SRGB_BLOCK,           //DDS_DXGI_FORMAT_BC3_UNORM_SRGB
    TEXTURE_LOADER_FORMAT_BC4_UNORM_BLOCK,          //DDS_DXGI_FORMAT_BC4_TYPELESS
    TEXTURE_LOADER_FORMAT_BC4_UNORM_BLOCK,          //DDS_DXGI_FORMAT_BC4_UNORM
    TEXTURE_LOADER_FORMAT_BC4_SNORM_BLOCK,          //DDS_DXGI_FORMAT_BC4_SNORM
    TEXTURE_LOADER_FORMAT_BC5_UNORM_BLOCK,          //DDS_DXGI_FORMAT_BC5_TYPELESS
    TEXTURE_LOADER_FORMAT_BC5_UNORM_BLOCK,          //DDS_DXGI_FORMAT_BC5_UNORM
    TEXTURE_LOADER_FORMAT_BC5_SNORM_BLOCK,          //DDS_DXGI_FORMAT_BC5_SNORM
    TEXTURE_LOADER_FORMAT_B5G6R5_UNORM_PACK16,      //DDS_DXGI_FORMAT_B5G6R5_UNORM
    TEXTURE_LOADER_FORMAT_B5G5R5A1_UNORM_PACK16,    //DDS_DXGI_FORMAT_B5G5R5A1_UNORM
    TEXTURE_LOADER_FORMAT_B8G8R8A8_UNORM,           //DDS_DXGI_FORMAT_B8G8R8A8_UNORM
    TEXTURE_LOADER_FORMAT_B8G8R8A8_UNORM,           //DDS_DXGI_FORMAT_B8G8R8X8_UNORM
    TEXTURE_LOADER_FORMAT_A2R10G10B10_UNORM_PACK32, //DDS_DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM
    TEXTURE_LOADER_FORMAT_B8G8R8A8_UNORM,           //DDS_DXGI_FORMAT_B8G8R8A8_TYPELESS
    TEXTURE_LOADER_FORMAT_R8G8B8A8_SRGB,            //DDS_DXGI_FORMAT_B8G8R8A8_UNORM_SRGB
    TEXTURE_LOADER_FORMAT_B8G8R8A8_UNORM,           //DDS_DXGI_FORMAT_B8G8R8X8_TYPELESS
    TEXTURE_LOADER_FORMAT_R8G8B8A8_SRGB,            //DDS_DXGI_FORMAT_B8G8R8X8_UNORM_SRGB
    TEXTURE_LOADER_FORMAT_BC6H_UFLOAT_BLOCK,        //DDS_DXGI_FORMAT_BC6H_TYPELESS
    TEXTURE_LOADER_FORMAT_BC6H_UFLOAT_BLOCK,        //DDS_DXGI_FORMAT_BC6H_UF16
    TEXTURE_LOADER_FORMAT_BC6H_SFLOAT_BLOCK,        //DDS_DXGI_FORMAT_BC6H_SF16
    TEXTURE_LOADER_FORMAT_BC7_UNORM_BLOCK,          //DDS_DXGI_FORMAT_BC7_TYPELESS
    TEXTURE_LOADER_FORMAT_BC7_UNORM_BLOCK,          //DDS_DXGI_FORMAT_BC7_UNORM
    TEXTURE_LOADER_FORMAT_BC7_SRGB_BLOCK,           //DDS_DXGI_FORMAT_BC7_UNORM_SRGB
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_AYUV
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_Y410
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_Y416
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_NV12
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_P010
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_P016
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_420_OPAQUE
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_YUY2
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_Y210
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_Y216
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_NV11
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_AI44
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_IA44
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_P8
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_A8P8
    TEXTURE_LOADER_FORMAT_B4G4R4A4_UNORM_PACK16,    //DDS_DXGI_FORMAT_B4G4R4A4_UNORM //115
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_116
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_117
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_118
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_119
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_120
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_121
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_122
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_123
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_124
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_125
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_126
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_127
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_128
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_129
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_P208 //130
    TEXTURE_LOADER_FORMAT_UNDEFINED,                //DDS_DXGI_FORMAT_V208
    TEXTURE_LOADER_FORMAT_UNDEFINED                 //DDS_DXGI_FORMAT_V408
};
static_assert(DDS_DXGI_FORMAT_RANGE == (sizeof(gDdsToNeutralFormatMap) / sizeof(gDdsToNeutralFormatMap[0])), "gDdsToNeutralFormatMap may not match!");

static inline uint32_t _GetNeutralFormat(uint32_t ddsformat)
{
    assert(ddsformat < (sizeof(gDdsToNeutralFormatMap) / sizeof(gDdsToNeutralFormatMap[0])));
    return gDdsToNeutralFormatMap[ddsformat];
}

//--------------------------------------------------------------------------------------
static uint32_t const gDdsFormatPlaneCountInfoTable[] = {
    0, //DDS_DXGI_FORMAT_UNKNOWN
    1, //DDS_DXGI_FORMAT_R32G32B32A32_TYPELESS
    1, //DDS_DXGI_FORMAT_R32G32B32A32_FLOAT
    1, //DDS_DXGI_FORMAT_R32G32B32A32_UINT
    1, //DDS_DXGI_FORMAT_R32G32B32A32_SINT
    1, //DDS_DXGI_FORMAT_R32G32B32_TYPELESS
    1, //DDS_DXGI_FORMAT_R32G32B32_FLOAT
    1, //DDS_DXGI_FORMAT_R32G32B32_UINT
    1, //DDS_DXGI_FORMAT_R32G32B32_SINT
    1, //DDS_DXGI_FORMAT_R16G16B16A16_TYPELESS
    1, //DDS_DXGI_FORMAT_R16G16B16A16_FLOAT
    1, //DDS_DXGI_FORMAT_R16G16B16A16_UNORM
    1, //DDS_DXGI_FORMAT_R16G16B16A16_UINT
    1, //DDS_DXGI_FORMAT_R16G16B16A16_SNORM
    1, //DDS_DXGI_FORMAT_R16G16B16A16_SINT
    1, //DDS_DXGI_FORMAT_R32G32_TYPELESS
    1, //DDS_DXGI_FORMAT_R32G32_FLOAT
    1, //DDS_DXGI_FORMAT_R32G32_UINT
    1, //DDS_DXGI_FORMAT_R32G32_SINT
    2, //DDS_DXGI_FORMAT_R32G8X24_TYPELESS
    2, //DDS_DXGI_FORMAT_D32_FLOAT_S8X24_UINT
    2, //DDS_DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS
    2, //DDS_DXGI_FORMAT_X32_TYPELESS_G8X24_UINT
    1, //DDS_DXGI_FORMAT_R10G10B10A2_TYPELESS
    1, //DDS_DXGI_FORMAT_R10G10B10A2_UNORM
    1, //DDS_DXGI_FORMAT_R10G10B10A2_UINT
    1, //DDS_DXGI_FORMAT_R11G11B10_FLOAT
    1, //DDS_DXGI_FORMAT_R8G8B8A8_TYPELESS
    1, //DDS_DXGI_FORMAT_R8G8B8A8_UNORM
    1, //DDS_DXGI_FORMAT_R8G8B8A8_UNORM_SRGB
    1, //DDS_DXGI_FORMAT_R8G8B8A8_UINT
    1, //DDS_DXGI_FORMAT_R8G8B8A8_SNORM
    1, //DDS_DXGI_FORMAT_R8G8B8A8_SINT
    1, //DDS_DXGI_FORMAT_R16G16_TYPELESS
    1, //DDS_DXGI_FORMAT_R16G16_FLOAT
    1, //DDS_DXGI_FORMAT_R16G16_UNORM
    1, //DDS_DXGI_FORMAT_R16G16_UINT
    1, //DDS_DXGI_FORMAT_R16G16_SNORM
    1, //DDS_DXGI_FORMAT_R16G16_SINT
    1, //DDS_DXGI_FORMAT_R32_TYPELESS
    1, //DDS_DXGI_FORMAT_D32_FLOAT
    1, //DDS_DXGI_FORMAT_R32_FLOAT
    1, //DDS_DXGI_FORMAT_R32_UINT
    1, //DDS_DXGI_FORMAT_R32_SINT
    2, //DDS_DXGI_FORMAT_R24G8_TYPELESS
    2, //DDS_DXGI_FORMAT_D24_UNORM_S8_UINT
    2, //DDS_DXGI_FORMAT_R24_UNORM_X8_TYPELESS
    2, //DDS_DXGI_FORMAT_X24_TYPELESS_G8_UINT
    1, //DDS_DXGI_FORMAT_R8G8_TYPELESS
    1, //DDS_DXGI_FORMAT_R8G8_UNORM
    1, //DDS_DXGI_FORMAT_R8G8_UINT
    1, //DDS_DXGI_FORMAT_R8G8_SNORM
    1, //DDS_DXGI_FORMAT_R8G8_SINT
    1, //DDS_DXGI_FORMAT_R16_TYPELESS
    1, //DDS_DXGI_FORMAT_R16_FLOAT
    1, //DDS_DXGI_FORMAT_D16_UNORM
    1, //DDS_DXGI_FORMAT_R16_UNORM
    1, //DDS_DXGI_FORMAT_R16_UINT
    1, //DDS_DXGI_FORMAT_R16_SNORM
    1, //DDS_DXGI_FORMAT_R16_SINT
    1, //DDS_DXGI_FORMAT_R8_TYPELESS
    1, //DDS_DXGI_FORMAT_R8_UNORM
    1, //DDS_DXGI_FORMAT_R8_UINT
    1, //DDS_DXGI_FORMAT_R8_SNORM
    1, //DDS_DXGI_FORMAT_R8_SINT
    1, //DDS_DXGI_FORMAT_A8_UNORM
    1, //DDS_DXGI_FORMAT_R1_UNORM
    1, //DDS_DXGI_FORMAT_R9G9B9E5_SHAREDEXP
    1, //DDS_DXGI_FORMAT_R8G8_B8G8_UNORM
    1, //DDS_DXGI_FORMAT_G8R8_G8B8_UNORM
    1, //DDS_DXGI_FORMAT_BC1_TYPELESS
    1, //DDS_DXGI_FORMAT_BC1_UNORM
    1, //DDS_DXGI_FORMAT_BC1_UNORM_SRGB
    1, //DDS_DXGI_FORMAT_BC2_TYPELESS
    1, //DDS_DXGI_FORMAT_BC2_UNORM
    1, //DDS_DXGI_FORMAT_BC2_UNORM_SRGB
    1, //DDS_DXGI_FORMAT_BC3_TYPELESS
    1, //DDS_DXGI_FORMAT_BC3_UNORM
    1, //DDS_DXGI_FORMAT_BC3_UNORM_SRGB
    1, //DDS_DXGI_FORMAT_BC4_TYPELESS
    1, //DDS_DXGI_FORMAT_BC4_UNORM
    1, //DDS_DXGI_FORMAT_BC4_SNORM
    1, //DDS_DXGI_FORMAT_BC5_TYPELESS
    1, //DDS_DXGI_FORMAT_BC5_UNORM
    1, //DDS_DXGI_FORMAT_BC5_SNORM
    1, //DDS_DXGI_FORMAT_B5G6R5_UNORM
    1, //DDS_DXGI_FORMAT_B5G5R5A1_UNORM
    1, //DDS_DXGI_FORMAT_B8G8R8A8_UNORM
    1, //DDS_DXGI_FORMAT_B8G8R8X8_UNORM
    1, //DDS_DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM
    1, //DDS_DXGI_FORMAT_B8G8R8A8_TYPELESS
    1, //DDS_DXGI_FORMAT_B8G8R8A8_UNORM_SRGB
    1, //DDS_DXGI_FORMAT_B8G8R8X8_TYPELESS
    1, //DDS_DXGI_FORMAT_B8G8R8X8_UNORM_SRGB
    1, //DDS_DXGI_FORMAT_BC6H_TYPELESS
    1, //DDS_DXGI_FORMAT_BC6H_UF16
    1, //DDS_DXGI_FORMAT_BC6H_SF16
    1, //DDS_DXGI_FORMAT_BC7_TYPELESS
    1, //DDS_DXGI_FORMAT_BC7_UNORM
    1, //DDS_DXGI_FORMAT_BC7_UNORM_SRGB
    0, //DDS_DXGI_FORMAT_AYUV
    0, //DDS_DXGI_FORMAT_Y410
    0, //DDS_DXGI_FORMAT_Y416
    0, //DDS_DXGI_FORMAT_NV12
    0, //DDS_DXGI_FORMAT_P010
    0, //DDS_DXGI_FORMAT_P016
    0, //DDS_DXGI_FORMAT_420_OPAQUE
    0, //DDS_DXGI_FORMAT_YUY2
    0, //DDS_DXGI_FORMAT_Y210
    0, //DDS_DXGI_FORMAT_Y216
    0, //DDS_DXGI_FORMAT_NV11
    0, //DDS_DXGI_FORMAT_AI44
    0, //DDS_DXGI_FORMAT_IA44
    0, //DDS_DXGI_FORMAT_P8
    0, //DDS_DXGI_FORMAT_A8P8
    1, //DDS_DXGI_FORMAT_B4G4R4A4_UNORM //115
    0, //DDS_DXGI_FORMAT_116
    0, //DDS_DXGI_FORMAT_117
    0, //DDS_DXGI_FORMAT_118
    0, //DDS_DXGI_FORMAT_119
    0, //DDS_DXGI_FORMAT_120
    0, //DDS_DXGI_FORMAT_121
    0, //DDS_DXGI_FORMAT_122
    0, //DDS_DXGI_FORMAT_123
    0, //DDS_DXGI_FORMAT_124
    0, //DDS_DXGI_FORMAT_125
    0, //DDS_DXGI_FORMAT_126
    0, //DDS_DXGI_FORMAT_127
    0, //DDS_DXGI_FORMAT_128
    0, //DDS_DXGI_FORMAT_129
    0, //DDS_DXGI_FORMAT_P208 //130
    0, //DDS_DXGI_FORMAT_V208
    0  //DDS_DXGI_FORMAT_V408
};
static_assert(DDS_DXGI_FORMAT_RANGE == (sizeof(gDdsFormatPlaneCountInfoTable) / sizeof(gDdsFormatPlaneCountInfoTable[0])), "gDdsFormatPlaneCountInfoTable may not match!");

static inline uint32_t _GetFormatPlaneCount(uint32_t ddsformat)
{
    assert(ddsformat < (sizeof(gDdsFormatPlaneCountInfoTable) / sizeof(gDdsFormatPlaneCountInfoTable[0])));
    return gDdsFormatPlaneCountInfoTable[ddsformat];
}

static inline bool IsDepthStencil(uint32_t ddsformat)
{
    switch (ddsformat)
    {
    case DDS_DXGI_FORMAT_R32G8X24_TYPELESS:
    case DDS_DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DDS_DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DDS_DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
    case DDS_DXGI_FORMAT_D32_FLOAT:
    case DDS_DXGI_FORMAT_R24G8_TYPELESS:
    case DDS_DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DDS_DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DDS_DXGI_FORMAT_X24_TYPELESS_G8_UINT:
    case DDS_DXGI_FORMAT_D16_UNORM:
        return true;
    default:
        return false;
    }
}
