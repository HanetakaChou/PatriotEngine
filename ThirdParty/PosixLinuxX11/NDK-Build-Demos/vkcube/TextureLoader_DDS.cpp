#include "TextureLoader_DDS.h"

//--------------------------------------------------------------------------------------
// Macros
//--------------------------------------------------------------------------------------
inline constexpr uint32_t DDS_MAKEFOURCC(char ch0, char ch1, char ch2, char ch3)
{
    return static_cast<uint32_t>(static_cast<uint8_t>(ch0)) | (static_cast<uint32_t>(static_cast<uint8_t>(ch1)) << 8) | (static_cast<uint32_t>(static_cast<uint8_t>(ch2)) << 16) | (static_cast<uint32_t>(static_cast<uint8_t>(ch3)) << 24);
}

//--------------------------------------------------------------------------------------
// DDS file structure definitions
//
// See DDS.h in the 'Texconv' sample and the 'DirectXTex' library
//--------------------------------------------------------------------------------------

struct DDS_PIXELFORMAT
{
    uint32_t size;
    uint32_t flags;
    uint32_t fourCC;
    uint32_t RGBBitCount;
    uint32_t RBitMask;
    uint32_t GBitMask;
    uint32_t BBitMask;
    uint32_t ABitMask;
};

#define DDS_FOURCC 0x00000004    // DDPF_FOURCC
#define DDS_RGB 0x00000040       // DDPF_RGB
#define DDS_LUMINANCE 0x00020000 // DDPF_LUMINANCE
#define DDS_ALPHA 0x00000002     // DDPF_ALPHA
#define DDS_BUMPDUDV 0x00080000  // DDPF_BUMPDUDV

#define DDS_HEADER_FLAGS_VOLUME 0x00800000 // DDSD_DEPTH

#define DDS_HEIGHT 0x00000002 // DDSD_HEIGHT

#define DDS_CUBEMAP_POSITIVEX 0x00000600 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEX
#define DDS_CUBEMAP_NEGATIVEX 0x00000a00 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEX
#define DDS_CUBEMAP_POSITIVEY 0x00001200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEY
#define DDS_CUBEMAP_NEGATIVEY 0x00002200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEY
#define DDS_CUBEMAP_POSITIVEZ 0x00004200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEZ
#define DDS_CUBEMAP_NEGATIVEZ 0x00008200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEZ

#define DDS_CUBEMAP_ALLFACES (DDS_CUBEMAP_POSITIVEX | DDS_CUBEMAP_NEGATIVEX | \
                              DDS_CUBEMAP_POSITIVEY | DDS_CUBEMAP_NEGATIVEY | \
                              DDS_CUBEMAP_POSITIVEZ | DDS_CUBEMAP_NEGATIVEZ)

#define DDS_CUBEMAP 0x00000200 // DDSCAPS2_CUBEMAP

enum DDS_MISC_FLAGS2
{
    DDS_MISC_FLAGS2_ALPHA_MODE_MASK = 0x7L,
};

struct DDS_HEADER
{
    uint32_t size;
    uint32_t flags;
    uint32_t height;
    uint32_t width;
    uint32_t pitchOrLinearSize;
    uint32_t depth; // only if DDS_HEADER_FLAGS_VOLUME is set in flags
    uint32_t mipMapCount;
    uint32_t reserved1[11];
    DDS_PIXELFORMAT ddspf;
    uint32_t caps;
    uint32_t caps2;
    uint32_t caps3;
    uint32_t caps4;
    uint32_t reserved2;
};

enum DDS_DXGI_FORMAT : uint32_t
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
    DDS_DXGI_FORMAT_FORCE_UINT = 0xffffffff
};

struct DDS_HEADER_DXT10
{
    DDS_DXGI_FORMAT dxgiFormat;
    uint32_t resourceDimension;
    uint32_t miscFlag; // see D3D11_RESOURCE_MISC_FLAG
    uint32_t arraySize;
    uint32_t miscFlags2;
};

//--------------------------------------------------------------------------------------

#include <assert.h>

bool LoadTextureDataFromStream(void *stream, ptrdiff_t (*stream_read)(void *stream, void *buf, size_t count), int64_t (*stream_seek)(void *stream, int64_t offset, int whence),
                               struct DDS_HEADER const **header, uint8_t const **bitData, size_t *bitSize)
{
    assert(header != NULL);
    assert(bitData != NULL);
    assert(bitSize != NULL);

    uint8_t ddsDataBuf[sizeof(uint32_t) + sizeof(struct DDS_HEADER) + sizeof(struct DDS_HEADER_DXT10)];
    uint8_t const *ddsData = ddsDataBuf;
    {
        ptrdiff_t BytesRead = stream_read(stream, ddsDataBuf, sizeof(uint32_t) + sizeof(struct DDS_HEADER));
        if (BytesRead < (sizeof(uint32_t) + sizeof(struct DDS_HEADER)))
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

    struct DDS_HEADER const *hdr = reinterpret_cast<struct DDS_HEADER const *>(ddsData + sizeof(uint32_t));
    // Verify header to validate DDS file
    if (hdr->size != sizeof(struct DDS_HEADER) || hdr->ddspf.size != sizeof(struct DDS_PIXELFORMAT))
    {
        return false;
    }

    // Check for DX10 extension
    struct DDS_HEADER_DXT10 const *hdrDXT10 = NULL;
    if ((hdr->ddspf.flags & DDS_FOURCC) && (DDS_MAKEFOURCC('D', 'X', '1', '0') == hdr->ddspf.fourCC))
    {
        // Must be long enough for both headers and magic value
        ptrdiff_t BytesRead = stream_read(stream, ddsDataBuf + (sizeof(uint32_t) + sizeof(struct DDS_HEADER)), sizeof(struct DDS_HEADER_DXT10));
        if (BytesRead < sizeof(struct DDS_HEADER_DXT10))
        {
            return false;
        }

        hdrDXT10 = reinterpret_cast<struct DDS_HEADER_DXT10 const *>(ddsData + (sizeof(uint32_t) + sizeof(struct DDS_HEADER)));
    }

    (*header) = hdr;
    size_t offset = sizeof(uint32_t) + sizeof(DDS_HEADER) + ((hdrDXT10 != NULL) ? sizeof(DDS_HEADER_DXT10) : 0);
    (*bitData) = ddsData + offset;
    //(*bitSize) = ddsDataSize - offset;

    return true;
}