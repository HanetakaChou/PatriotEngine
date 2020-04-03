#include "../TextureLoader.h"
#include "TextureLoader_PVR.h"

//https://github.com/powervr-graphics/Native_SDK/blob/master/framework/PVRCore/textureio/FileDefinesPVR.h
//https://github.com/powervr-graphics/Native_SDK/blob/master/framework/PVRCore/textureio/TextureReaderPVR.h
//https://github.com/powervr-graphics/Native_SDK/blob/master/framework/PVRCore/textureio/TextureReaderPVR.cpp

static inline constexpr uint32_t Pvr_MakeFourCC(char ch0, char ch1, char ch2, char ch3)
{
    return static_cast<uint32_t>(static_cast<uint8_t>(ch0)) | (static_cast<uint32_t>(static_cast<uint8_t>(ch1)) << 8) | (static_cast<uint32_t>(static_cast<uint8_t>(ch2)) << 16) | (static_cast<uint32_t>(static_cast<uint8_t>(ch3)) << 24);
}

enum
{
    Pvr_HeaderSizeV1 = 44,                                          // Header sizes, used as identifiers for previous versions of the header.
    Pvr_HeaderSizeV2 = 52,                                          // Header sizes, used as identifiers for previous versions of the header.
    Pvr_HeaderVersionV3 = Pvr_MakeFourCC('P', 'V', 'R', 3),         // if endianess does match.
    Pvr_HeaderVersionV3_Reversed = Pvr_MakeFourCC(3, 'R', 'V', 'P') // if endianess does not match.
};

/// <summary>Version 1 of the PVR file format</summary>
struct Pvr_HeaderV1
{
    uint32_t headerSize;          //!< size of the structure
    uint32_t height;              //!< height of surface to be created
    uint32_t width;               //!< width of input surface
    uint32_t numMipMaps;          //!< number of mip-map levels requested
    uint32_t pixelFormatAndFlags; //!< pixel format flags
    uint32_t dataSize;            //!< Size of the compress data
    uint32_t bitCount;            //!< number of bits per pixel
    uint32_t redBitMask;          //!< mask for red bit
    uint32_t greenBitMask;        //!< mask for green bits
    uint32_t blueBitMask;         //!< mask for blue bits
    uint32_t alphaBitMask;        //!< mask for alpha channel
};

/// <summary>Version 2 of the PVR file format</summary>
struct Pvr_HeaderV2
{
    uint32_t headerSize;          //!< size of the structure
    uint32_t height;              //!< height of surface to be created
    uint32_t width;               //!< width of input surface
    uint32_t numMipMaps;          //!< number of mip-map levels requested
    uint32_t pixelFormatAndFlags; //!< pixel format flags
    uint32_t dataSize;            //!< Size of the compress data
    uint32_t bitCount;            //!< number of bits per pixel
    uint32_t redBitMask;          //!< mask for red bit
    uint32_t greenBitMask;        //!< mask for green bits
    uint32_t blueBitMask;         //!< mask for blue bits
    uint32_t alphaBitMask;        //!< mask for alpha channel
    uint32_t pvrMagic;            //!< magic number identifying pvr file
    uint32_t numSurfaces;         //!< the number of surfaces present in the pvr
};

enum
{
    Pvr_CompressedFlag = 0X01,   /// <summary>Sets whether or not the texture is compressed using PVRTexLib's FILE compression - this is independent of any texture compression. Currently unsupported.</summary>
    Pvr_PremultipliedFlag = 0X02 /// <summary>Sets whether or not the texture's color has been pre-multiplied by the alpha values.</summary>
};

enum
{
    Pvr_PixelTypeID_PVRTCI_2bpp_RGB = 0,
    Pvr_PixelTypeID_PVRTCI_2bpp_RGBA = 1,
    Pvr_PixelTypeID_PVRTCI_4bpp_RGB = 2,
    Pvr_PixelTypeID_PVRTCI_4bpp_RGBA = 3,
    Pvr_PixelTypeID_PVRTCII_2bpp = 4,
    Pvr_PixelTypeID_PVRTCII_4bpp = 5,
    Pvr_PixelTypeID_ETC1 = 6,
    Pvr_PixelTypeID_DXT1 = 7,
    Pvr_PixelTypeID_DXT2 = 8,
    Pvr_PixelTypeID_DXT3 = 9,
    Pvr_PixelTypeID_DXT4 = 10,
    Pvr_PixelTypeID_DXT5 = 11,

    // These formats are identical to some DXT formats.
    Pvr_PixelTypeID_BC1 = Pvr_PixelTypeID_DXT1,
    Pvr_PixelTypeID_BC2 = Pvr_PixelTypeID_DXT3,
    Pvr_PixelTypeID_BC3 = Pvr_PixelTypeID_DXT5,

    // These are currently unsupported:
    Pvr_PixelTypeID_BC4 = 12,
    Pvr_PixelTypeID_BC5 = 13,
    Pvr_PixelTypeID_BC6 = 14,
    Pvr_PixelTypeID_BC7 = 15,

    // These are supported
    Pvr_PixelTypeID_UYVY = 16,
    Pvr_PixelTypeID_YUY2 = 17,
    Pvr_PixelTypeID_BW1bpp = 18,
    Pvr_PixelTypeID_SharedExponentR9G9B9E5 = 19,
    Pvr_PixelTypeID_RGBG8888 = 20,
    Pvr_PixelTypeID_GRGB8888 = 21,
    Pvr_PixelTypeID_ETC2_RGB = 22,
    Pvr_PixelTypeID_ETC2_RGBA = 23,
    Pvr_PixelTypeID_ETC2_RGB_A1 = 24,
    Pvr_PixelTypeID_EAC_R11 = 25,
    Pvr_PixelTypeID_EAC_RG11 = 26,

    Pvr_PixelTypeID_ASTC_4x4 = 27,
    Pvr_PixelTypeID_ASTC_5x4 = 28,
    Pvr_PixelTypeID_ASTC_5x5 = 29,
    Pvr_PixelTypeID_ASTC_6x5 = 30,
    Pvr_PixelTypeID_ASTC_6x6 = 31,
    Pvr_PixelTypeID_ASTC_8x5 = 32,
    Pvr_PixelTypeID_ASTC_8x6 = 33,
    Pvr_PixelTypeID_ASTC_8x8 = 34,
    Pvr_PixelTypeID_ASTC_10x5 = 35,
    Pvr_PixelTypeID_ASTC_10x6 = 36,
    Pvr_PixelTypeID_ASTC_10x8 = 37,
    Pvr_PixelTypeID_ASTC_10x10 = 38,
    Pvr_PixelTypeID_ASTC_12x10 = 39,
    Pvr_PixelTypeID_ASTC_12x12 = 40,

    Pvr_PixelTypeID_ASTC_3x3x3 = 41,
    Pvr_PixelTypeID_ASTC_4x3x3 = 42,
    Pvr_PixelTypeID_ASTC_4x4x3 = 43,
    Pvr_PixelTypeID_ASTC_4x4x4 = 44,
    Pvr_PixelTypeID_ASTC_5x4x4 = 45,
    Pvr_PixelTypeID_ASTC_5x5x4 = 46,
    Pvr_PixelTypeID_ASTC_5x5x5 = 47,
    Pvr_PixelTypeID_ASTC_6x5x5 = 48,
    Pvr_PixelTypeID_ASTC_6x6x5 = 49,
    Pvr_PixelTypeID_ASTC_6x6x6 = 50,

    // Invalid value
    Pvr_PixelTypeID_NumCompressedPFs = 51
};

static inline constexpr uint64_t Pvr_MakePixelTypeChar(char C1Name, char C2Name, char C3Name, char C4Name, char C1Bits, char C2Bits, char C3Bits, char C4Bits)
{
    return static_cast<uint64_t>(static_cast<uint8_t>(C1Name)) | (static_cast<uint64_t>(static_cast<uint8_t>(C2Name)) << 8) | (static_cast<uint64_t>(static_cast<uint8_t>(C3Name)) << 16) | (static_cast<uint64_t>(static_cast<uint8_t>(C4Name)) << 24) | (static_cast<uint64_t>(static_cast<uint8_t>(C1Bits)) << 32) | (static_cast<uint64_t>(static_cast<uint8_t>(C2Bits)) << 40) | (static_cast<uint64_t>(static_cast<uint8_t>(C3Bits)) << 48) | (static_cast<uint64_t>(static_cast<uint8_t>(C4Bits)) << 56);
}

enum
{
    Pvr_PixelTypeChar_I8 = Pvr_MakePixelTypeChar('i', '\0', '\0', '\0', 8, 0, 0, 0),
    Pvr_PixelTypeChar_R8 = Pvr_MakePixelTypeChar('r', '\0', '\0', '\0', 8, 0, 0, 0),
    Pvr_PixelTypeChar_R8G8 = Pvr_MakePixelTypeChar('r', 'g', '\0', '\0', 8, 8, 0, 0),
    Pvr_PixelTypeChar_R8G8B8 = Pvr_MakePixelTypeChar('r', 'g', 'b', '\0', 8, 8, 8, 0),
    Pvr_PixelTypeChar_B8G8R8 = Pvr_MakePixelTypeChar('b', 'g', 'r', '\0', 8, 8, 8, 0),
    Pvr_PixelTypeChar_R8G8B8A8 = Pvr_MakePixelTypeChar('r', 'g', 'b', 'a', 8, 8, 8, 8),
    Pvr_PixelTypeChar_B8G8R8A8 = Pvr_MakePixelTypeChar('b', 'g', 'r', 'a', 8, 8, 8, 8),
    Pvr_PixelTypeChar_A8B8G8R8 = Pvr_MakePixelTypeChar('a', 'b', 'g', 'r', 8, 8, 8, 8),
    Pvr_PixelTypeChar_R16 = Pvr_MakePixelTypeChar('r', '\0', '\0', '\0', 16, 0, 0, 0),
    Pvr_PixelTypeChar_R16G16 = Pvr_MakePixelTypeChar('r', 'g', '\0', '\0', 16, 16, 0, 0),
    Pvr_PixelTypeChar_L16A16 = Pvr_MakePixelTypeChar('l', 'a', '\0', '\0', 16, 16, 0, 0),
    Pvr_PixelTypeChar_R16G16B16 = Pvr_MakePixelTypeChar('r', 'g', 'b', '\0', 16, 16, 16, 0),
    Pvr_PixelTypeChar_R16G16B16A16 = Pvr_MakePixelTypeChar('r', 'g', 'b', 'a', 16, 16, 16, 16),
    Pvr_PixelTypeChar_R32 = Pvr_MakePixelTypeChar('r', '\0', '\0', '\0', 32, 0, 0, 0),
    Pvr_PixelTypeChar_L32 = Pvr_MakePixelTypeChar('l', '\0', '\0', '\0', 32, 0, 0, 0),
    Pvr_PixelTypeChar_R32G32 = Pvr_MakePixelTypeChar('r', 'g', '\0', '\0', 32, 32, 0, 0),
    Pvr_PixelTypeChar_L32A32 = Pvr_MakePixelTypeChar('l', 'a', '\0', '\0', 32, 32, 0, 0),
    Pvr_PixelTypeChar_R32G32B32 = Pvr_MakePixelTypeChar('r', 'g', 'b', '\0', 32, 32, 32, 0),
    Pvr_PixelTypeChar_R32G32B32A32 = Pvr_MakePixelTypeChar('r', 'g', 'b', 'a', 32, 32, 32, 32),
    Pvr_PixelTypeChar_R5G6B5 = Pvr_MakePixelTypeChar('r', 'g', 'b', '\0', 5, 6, 5, 0),
    Pvr_PixelTypeChar_R4G4B4A4 = Pvr_MakePixelTypeChar('r', 'g', 'b', 'a', 4, 4, 4, 4),
    Pvr_PixelTypeChar_R5G5B5A1 = Pvr_MakePixelTypeChar('r', 'g', 'b', 'a', 5, 5, 5, 1),
    Pvr_PixelTypeChar_R11G11B10 = Pvr_MakePixelTypeChar('b', 'g', 'r', '\0', 10, 11, 11, 0),
    Pvr_PixelTypeChar_D8 = Pvr_MakePixelTypeChar('d', '\0', '\0', '\0', 8, 0, 0, 0),
    Pvr_PixelTypeChar_S8 = Pvr_MakePixelTypeChar('s', '\0', '\0', '\0', 8, 0, 0, 0),
    Pvr_PixelTypeChar_D16 = Pvr_MakePixelTypeChar('d', '\0', '\0', '\0', 16, 0, 0, 0),
    Pvr_PixelTypeChar_D24 = Pvr_MakePixelTypeChar('d', '\0', '\0', '\0', 24, 0, 0, 0),
    Pvr_PixelTypeChar_D32 = Pvr_MakePixelTypeChar('d', '\0', '\0', '\0', 32, 0, 0, 0),
    Pvr_PixelTypeChar_D16S8 = Pvr_MakePixelTypeChar('d', 's', '\0', '\0', 16, 8, 0, 0),
    Pvr_PixelTypeChar_D24S8 = Pvr_MakePixelTypeChar('d', 's', '\0', '\0', 24, 8, 0, 0),
    Pvr_PixelTypeChar_D32S8 = Pvr_MakePixelTypeChar('d', 's', '\0', '\0', 32, 8, 0, 0)
};

enum
{
    Pvr_ColorSpace_lRGB = 0,
    Pvr_ColorSpace_sRGB = 1,
    Pvr_ColorSpace_NumCSs = 2
};

enum
{
    Pvr_ChannelType_UnsignedByteNorm = 0,
    Pvr_ChannelType_SignedByteNorm = 1,
    Pvr_ChannelType_UnsignedByte = 2,
    Pvr_ChannelType_SignedByte = 3,
    Pvr_ChannelType_UnsignedShortNorm = 4,
    Pvr_ChannelType_SignedShortNorm = 5,
    Pvr_ChannelType_UnsignedShort = 6,
    Pvr_ChannelType_SignedShort = 7,
    Pvr_ChannelType_UnsignedIntegerNorm = 8,
    Pvr_ChannelType_SignedIntegerNorm = 9,
    Pvr_ChannelType_UnsignedInteger = 10,
    Pvr_ChannelType_SignedInteger = 11,
    Pvr_ChannelType_SignedFloat = 12,
    Pvr_ChannelType_Float = Pvr_ChannelType_SignedFloat, // the name PVRFloat is now deprecated.
    Pvr_ChannelType_UnsignedFloat = 13,
    Pvr_ChannelType_NumCTs = 14
};

// V3 Header Identifiers.
/// <summary>This header stores everything that you would ever need to load (but not necessarily use) a texture's
/// data accurately, but no more. Data that is provided but is not needed to read the data is stored in the
/// Metadata section (See TextureHeaderWithMetadata). Correct use of the texture may rely on meta data, but
/// accurate data loading can be done through the standard header alone.</summary>
struct Pvr_HeaderV3
{
    uint32_t version;      //!< PVR format v3 identifier
    uint32_t flags;        //!< Various format flags.
    uint64_t pixelFormat;  //!< The pixel format, 8cc value storing the 4 channel identifiers and their respective sizes.
    uint32_t colorSpace;   //!< The Color Space of the texture, currently either linear RGB or sRGB.
    uint32_t channelType;  //!< Variable type that the channel is stored in. Supports signed/unsigned int/short/char/float.
    uint32_t height;       //!< Height of the texture.
    uint32_t width;        //!< Width of the texture.
    uint32_t depth;        //!< Depth of the texture. (Z-slices)
    uint32_t numSurfaces;  //!< Number of members in a Texture Array.
    uint32_t numFaces;     //!< Number of faces in a Cube Map. Maybe be a value other than 6.
    uint32_t numMipMaps;   //!< Number of MIP Maps in the texture - NB: Includes top level.
    uint32_t metaDataSize; //!< Size of the accompanying meta data.
};

struct Pvr_MetaData
{
    uint32_t _fourCC;   // A 4cc descriptor of the data type's creator. // Values equating to values between 'P' 'V' 'R' 0 and 'P' 'V' 'R' 255 will be used by our headers.
    uint32_t _key;      // Enumeration key identifying the data type.
    uint32_t _dataSize; // Size of attached data.
    char _data[1];      // Data array, can be absolutely anything, the loader needs to know how to handle it based on fourCC and key.
};

//--------------------------------------------------------------------------------------

struct TextureLoader_PVRHeader
{
    uint64_t pixelFormat;
    uint32_t colorSpace;
    uint32_t channelType;

    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t numMipMaps;
    uint32_t numSurfaces;
    uint32_t numFaces;

    //The UV has been baked into meshes
    //bool _pvr3_0;
    //struct
    //{
    //    uint32_t XPosition;
    //    uint32_t YPosition;
    //    uint32_t Width;
    //    uint32_t Height;
    //} textureAtlasCoords[...];

    bool _pvr3_1;
    struct
    {
        float bumpScale;
        uint8_t bumpOrder[4];
    } bumpData;

    bool _pvr3_2;
    uint8_t cubeMapOrder[6];

    bool _pvr3_3;
    uint8_t textureOrientation[3];

    bool _pvr3_4;
    uint32_t borderData[3];

    //bool _pvr3_5;
    //uint8_t paddingData[...];
};

//--------------------------------------------------------------------------------------

static inline uint32_t _GetNeutralType(uint32_t height, uint32_t depth);

static inline uint32_t _GetNeutralFormat(uint64_t pixelFormat, uint32_t colorSpace, uint32_t channelType);

static inline uint32_t Pvr_GetPixelFormatPartHigh(uint64_t pixelFormat);

static inline bool Pvr_GetMinDimensionsForFormat(uint64_t pixelFormat, uint32_t *minX, uint32_t *minY, uint32_t *minZ);

static inline uint32_t Pvr_GetBitsPerPixel(uint64_t pixelFormat);

static inline uint32_t _GetFormatPlaneCount(uint64_t pixelFormat);

static inline bool IsDepthStencil(uint64_t pixelFormat);

#include <assert.h>

//--------------------------------------------------------------------------------------
static inline bool LoadTextureHeaderFromStream(void const *stream, ptrdiff_t (*stream_read)(void const *stream, void *buf, size_t count), int64_t (*stream_seek)(void const *stream, int64_t offset, int whence),
                                               struct TextureLoader_PVRHeader *texture_header, size_t *texture_data_offset)
{
    assert(texture_header != NULL);
    assert(texture_data_offset != NULL);

    if (stream_seek(stream, 0, TEXTURE_LOADER_STREAM_SEEK_SET) == -1)
    {
        return false;
    }

    // Read the texture header version
    uint32_t version;
    stream_read(stream, &version, sizeof(version));
    if (Pvr_HeaderVersionV3 == version)
    {
        Pvr_HeaderV3 header;
        header.version = version;
        {
            ptrdiff_t BytesRead = stream_read(stream, &header.flags, sizeof(header.flags) + sizeof(header.pixelFormat) + sizeof(header.colorSpace) + sizeof(header.channelType) + sizeof(header.height) + sizeof(header.width) + sizeof(header.depth) + sizeof(header.numSurfaces) + sizeof(header.numFaces) + sizeof(header.numMipMaps) + sizeof(header.metaDataSize));
            if (BytesRead == -1 || BytesRead < (sizeof(header.flags) + sizeof(header.pixelFormat) + sizeof(header.colorSpace) + sizeof(header.channelType) + sizeof(header.height) + sizeof(header.width) + sizeof(header.depth) + sizeof(header.numSurfaces) + sizeof(header.numFaces) + sizeof(header.numMipMaps) + sizeof(header.metaDataSize)))
            {
                return false;
            }
        }

        texture_header->pixelFormat = header.pixelFormat;
        texture_header->colorSpace = header.colorSpace;
        texture_header->channelType = header.channelType;

        texture_header->width = header.width;
        texture_header->height = header.height;
        texture_header->depth = header.depth;
        texture_header->numMipMaps = header.numMipMaps;
        texture_header->numSurfaces = header.numSurfaces;
        texture_header->numFaces = header.numFaces;

        texture_header->_pvr3_1 = false;
        texture_header->_pvr3_2 = false;
        texture_header->_pvr3_3 = false;
        texture_header->_pvr3_4 = false;

        uint32_t metaDataRead = 0;
        while (metaDataRead < header.metaDataSize)
        {
            Pvr_MetaData metadata;
            {
                ptrdiff_t BytesRead = stream_read(stream, &metadata, sizeof(metadata._fourCC) + sizeof(metadata._key) + sizeof(metadata._dataSize));
                if (BytesRead == -1 || BytesRead < (sizeof(metadata._fourCC) + sizeof(metadata._key) + sizeof(metadata._dataSize)))
                {
                    return false;
                }
                metaDataRead += (sizeof(metadata._fourCC) + sizeof(metadata._key) + sizeof(metadata._dataSize));
            }

            switch (metadata._fourCC)
            {
            case Pvr_HeaderVersionV3:
            {
                switch (metadata._key)
                {
                case 0:
                {
                    //The UV has been baked into meshes
                    assert(0 == (metadata._dataSize % (sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t))));
                    if (-1 == stream_seek(stream, metadata._dataSize, TEXTURE_LOADER_STREAM_SEEK_CUR))
                    {
                        return false;
                    }
                    metaDataRead += metadata._dataSize;
                }
                break;
                case 1:
                {
                    assert(sizeof(texture_header->bumpData) == metadata._dataSize);
                    ptrdiff_t BytesRead = stream_read(stream, &texture_header->bumpData, metadata._dataSize);
                    if (BytesRead == -1 || BytesRead < metadata._dataSize)
                    {
                        return false;
                    }
                    texture_header->_pvr3_1 = true;
                    metaDataRead += metadata._dataSize;
                }
                break;
                case 2:
                {
                    assert(sizeof(texture_header->cubeMapOrder) == metadata._dataSize);
                    ptrdiff_t BytesRead = stream_read(stream, &texture_header->cubeMapOrder, metadata._dataSize);
                    if (BytesRead == -1 || BytesRead < metadata._dataSize)
                    {
                        return false;
                    }
                    texture_header->_pvr3_2 = true;
                    metaDataRead += metadata._dataSize;
                }
                break;
                case 3:
                {
                    assert(sizeof(texture_header->textureOrientation) == metadata._dataSize);
                    ptrdiff_t BytesRead = stream_read(stream, &texture_header->textureOrientation, metadata._dataSize);
                    if (BytesRead == -1 || BytesRead < metadata._dataSize)
                    {
                        return false;
                    }
                    texture_header->_pvr3_3 = true;
                    metaDataRead += metadata._dataSize;
                }
                break;
                case 4:
                {
                    assert(sizeof(texture_header->borderData) == metadata._dataSize);
                    ptrdiff_t BytesRead = stream_read(stream, &texture_header->borderData, metadata._dataSize);
                    if (BytesRead == -1 || BytesRead < metadata._dataSize)
                    {
                        return false;
                    }
                    texture_header->_pvr3_4 = true;
                    metaDataRead += metadata._dataSize;
                }
                break;
                case 5:
                {
                    if (-1 == stream_seek(stream, metadata._dataSize, TEXTURE_LOADER_STREAM_SEEK_CUR))
                    {
                        return false;
                    }
                    metaDataRead += metadata._dataSize;
                }
                break;
                default:
                    return false;
                }
            }
            break;
            default:
                return false;
            }
        }
        assert(metaDataRead == header.metaDataSize);

        (*texture_data_offset) = (sizeof(version) + sizeof(header.flags) + sizeof(header.pixelFormat) + sizeof(header.colorSpace) + sizeof(header.channelType) + sizeof(header.height) + sizeof(header.width) + sizeof(header.depth) + sizeof(header.numSurfaces) + sizeof(header.numFaces) + sizeof(header.numMipMaps) + sizeof(header.metaDataSize) + metaDataRead);
        return true;
    }
    else
    {
        return false;
    }
}

//--------------------------------------------------------------------------------------
bool PVRTextureLoader_LoadHeaderFromStream(void const *stream, ptrdiff_t (*stream_read)(void const *stream, void *buf, size_t count), int64_t (*stream_seek)(void const *stream, int64_t offset, int whence),
                                           struct TextureLoader_NeutralHeader *neutral_texture_header, size_t *neutral_header_offset)
{

    struct TextureLoader_PVRHeader pvr_texture_header;
    size_t pvr_texture_data_offset;
    if (LoadTextureHeaderFromStream(stream, stream_read, stream_seek, &pvr_texture_header, &pvr_texture_data_offset))
    {
        neutral_texture_header->type = _GetNeutralType(pvr_texture_header.height, pvr_texture_header.depth);
        assert(TEXTURE_LOADER_TYPE_UNDEFINED != neutral_texture_header->type);

        neutral_texture_header->format = _GetNeutralFormat(pvr_texture_header.pixelFormat, pvr_texture_header.colorSpace, pvr_texture_header.channelType);
        assert(TEXTURE_LOADER_FORMAT_UNDEFINED != neutral_texture_header->format);

        neutral_texture_header->width = pvr_texture_header.width;
        neutral_texture_header->height = pvr_texture_header.height;
        neutral_texture_header->depth = pvr_texture_header.depth;
        neutral_texture_header->mipLevels = pvr_texture_header.numMipMaps;
        neutral_texture_header->arrayLayers = pvr_texture_header.numFaces * pvr_texture_header.numSurfaces;

        assert(0 != pvr_texture_header.numFaces);
        neutral_texture_header->isCubeMap = (pvr_texture_header.numFaces > 1);

        (*neutral_header_offset) = pvr_texture_data_offset;

        return true;
    }
    else
    {
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------
#include <algorithm>
bool PVRTextureLoader_FillDataFromStream(void const *stream, ptrdiff_t (*stream_read)(void const *stream, void *buf, size_t count), int64_t (*stream_seek)(void const *stream, int64_t offset, int whence),
                                         uint8_t *stagingPointer, size_t NumSubresources, struct TextureLoader_MemcpyDest const *pDest,
                                         struct TextureLoader_NeutralHeader const *neutral_texture_header_validate, size_t const *neutral_header_offset_validate)
{
    struct TextureLoader_PVRHeader texture_header;
    size_t texture_data_offset;
    if (!LoadTextureHeaderFromStream(stream, stream_read, stream_seek, &texture_header, &texture_data_offset))
    {
        return false;
    }

    assert(
        (0 != texture_header.numFaces) && ((texture_header.numFaces > 1) == neutral_texture_header_validate->isCubeMap) &&
        _GetNeutralType(texture_header.height, texture_header.depth) == neutral_texture_header_validate->type &&
        _GetNeutralFormat(texture_header.pixelFormat, texture_header.colorSpace, texture_header.channelType) == neutral_texture_header_validate->format &&
        texture_header.width == neutral_texture_header_validate->width &&
        texture_header.height == neutral_texture_header_validate->height &&
        texture_header.depth == neutral_texture_header_validate->depth &&
        texture_header.numMipMaps == neutral_texture_header_validate->mipLevels &&
        texture_header.numFaces * texture_header.numSurfaces == neutral_texture_header_validate->arrayLayers //
    );
    assert(texture_data_offset == (*neutral_header_offset_validate));

    uint32_t numberOfPlanes = _GetFormatPlaneCount(texture_header.pixelFormat);
    if (numberOfPlanes == 0)
    {
        return false;
    }

    if ((numberOfPlanes > 1) && IsDepthStencil(texture_header.pixelFormat))
    {
        // DirectX 12 uses planes for stencil, DirectX 11 does not
        return false;
    }

    size_t numberOfResources = numberOfPlanes * texture_header.numMipMaps * (texture_header.numFaces * texture_header.numSurfaces);

    if (numberOfResources != NumSubresources)
    {
        return false;
    }

    // The smallest divisible sizes for a pixel format
    uint32_t uiSmallestWidth = 1;
    uint32_t uiSmallestHeight = 1;
    uint32_t uiSmallestDepth = 1;

    // Get the pixel format's minimum dimensions.
    if (!Pvr_GetMinDimensionsForFormat(texture_header.pixelFormat, &uiSmallestWidth, &uiSmallestHeight, &uiSmallestDepth))
    {
        return false;
    }

    size_t inputSkipBytes = texture_data_offset;

    // Write the texture data
    for (uint32_t mipMap = 0; mipMap < texture_header.numMipMaps; ++mipMap)
    {
        // Get the dimensions of the current MIP Map level.
        uint32_t uiWidth = std::max<uint32_t>(texture_header.width >> mipMap, 1);
        uint32_t uiHeight = std::max<uint32_t>(texture_header.height >> mipMap, 1);
        uint32_t uiDepth = std::max<uint32_t>(texture_header.depth >> mipMap, 1);

        // If pixel format is compressed, the dimensions need to be padded.
        if (0 == Pvr_GetPixelFormatPartHigh(texture_header.pixelFormat))
        {
            uiWidth = uiWidth + ((-1 * uiWidth) % uiSmallestWidth);
            uiHeight = uiHeight + ((-1 * uiHeight) % uiSmallestHeight);
            uiDepth = uiDepth + ((-1 * uiDepth) % uiSmallestDepth);
        }

        uint32_t uiRowBytes;
        uint32_t uiNumRows;
        uint32_t uiNumSlices;

        if (texture_header.pixelFormat >= Pvr_PixelTypeID_ASTC_4x4 && texture_header.pixelFormat <= Pvr_PixelTypeID_ASTC_6x6x6)
        {
            uiRowBytes = (128 / 8) * (uiWidth / uiSmallestWidth);
            uiNumRows = (uiHeight / uiSmallestHeight);
            uiNumSlices = (uiDepth / uiSmallestDepth);
        }
        else
        {
            uint32_t bpp = Pvr_GetBitsPerPixel(texture_header.pixelFormat);
            if (0 == bpp)
            {
                return false;
            }

            uiRowBytes = ((bpp * uiWidth) / 8) * uiSmallestHeight * uiSmallestDepth;
            uiNumRows = (uiHeight / uiSmallestHeight);
            uiNumSlices = (uiDepth / uiSmallestDepth);
        }

        size_t inputRowSize = uiRowBytes;
        size_t inputNumRows = uiNumRows;
        size_t inputSliceSize = uiRowBytes * uiNumRows;
        size_t inputNumSlices = uiNumSlices;

        for (uint32_t surface = 0; surface < texture_header.numSurfaces; ++surface)
        {
            for (uint32_t face = 0; face < texture_header.numFaces; ++face)
            {
                assert(1 == numberOfPlanes);
                for (uint32_t plane = 0; plane < numberOfPlanes; ++plane)
                {
                    size_t dstSubresource = TextureLoader_CalcSubresource(mipMap, face * surface, plane, texture_header.numMipMaps, texture_header.numFaces * texture_header.numSurfaces);
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
            }
        }
    }

    uint8_t u_assert_only[1];
    assert(0 == stream_read(stream, u_assert_only, sizeof(uint8_t)));
    return true;
}

//--------------------------------------------------------------------------------------
static inline uint32_t Pvr_GetPixelFormatPartHigh(uint64_t pixelFormat)
{
    return (pixelFormat >> 32);
}

//--------------------------------------------------------------------------------------
static inline bool Pvr_GetMinDimensionsForFormat(uint64_t pixelFormat, uint32_t *minX, uint32_t *minY, uint32_t *minZ)
{
    assert(NULL != minX);
    assert(NULL != minY);
    assert(NULL != minZ);

    if (0 == Pvr_GetPixelFormatPartHigh(pixelFormat))
    {
        switch (pixelFormat)
        {
        case Pvr_PixelTypeID_DXT1:
        case Pvr_PixelTypeID_DXT2:
        case Pvr_PixelTypeID_DXT3:
        case Pvr_PixelTypeID_DXT4:
        case Pvr_PixelTypeID_DXT5:
        case Pvr_PixelTypeID_BC4:
        case Pvr_PixelTypeID_BC5:
        case Pvr_PixelTypeID_ETC1:
        case Pvr_PixelTypeID_ETC2_RGB:
        case Pvr_PixelTypeID_ETC2_RGBA:
        case Pvr_PixelTypeID_ETC2_RGB_A1:
        case Pvr_PixelTypeID_EAC_R11:
        case Pvr_PixelTypeID_EAC_RG11:
            (*minX) = 4;
            (*minY) = 4;
            (*minZ) = 1;
            break;
        case Pvr_PixelTypeID_PVRTCI_4bpp_RGB:
        case Pvr_PixelTypeID_PVRTCI_4bpp_RGBA:
            (*minX) = 8;
            (*minY) = 8;
            (*minZ) = 1;
            break;
        case Pvr_PixelTypeID_PVRTCI_2bpp_RGB:
        case Pvr_PixelTypeID_PVRTCI_2bpp_RGBA:
            (*minX) = 16;
            (*minY) = 8;
            (*minZ) = 1;
            break;
        case Pvr_PixelTypeID_PVRTCII_4bpp:
            (*minX) = 4;
            (*minY) = 4;
            (*minZ) = 1;
            break;
        case Pvr_PixelTypeID_PVRTCII_2bpp:
            (*minX) = 8;
            (*minY) = 4;
            (*minZ) = 1;
            break;
        case Pvr_PixelTypeID_UYVY:
        case Pvr_PixelTypeID_YUY2:
        case Pvr_PixelTypeID_RGBG8888:
        case Pvr_PixelTypeID_GRGB8888:
            (*minX) = 2;
            (*minY) = 1;
            (*minZ) = 1;
            break;
        case Pvr_PixelTypeID_BW1bpp:
            (*minX) = 8;
            (*minY) = 1;
            (*minZ) = 1;
            break;
            // Error
        case Pvr_PixelTypeID_ASTC_4x4:
            (*minX) = 4;
            (*minY) = 4;
            (*minZ) = 1;
            break;
        case Pvr_PixelTypeID_ASTC_5x4:
            (*minX) = 5;
            (*minY) = 4;
            (*minZ) = 1;
            break;
        case Pvr_PixelTypeID_ASTC_5x5:
            (*minX) = 5;
            (*minY) = 5;
            (*minZ) = 1;
            break;
        case Pvr_PixelTypeID_ASTC_6x5:
            (*minX) = 6;
            (*minY) = 5;
            (*minZ) = 1;
            break;
        case Pvr_PixelTypeID_ASTC_6x6:
            (*minX) = 6;
            (*minY) = 6;
            (*minZ) = 1;
            break;
        case Pvr_PixelTypeID_ASTC_8x5:
            (*minX) = 8;
            (*minY) = 5;
            (*minZ) = 1;
            break;
        case Pvr_PixelTypeID_ASTC_8x6:
            (*minX) = 8;
            (*minY) = 6;
            (*minZ) = 1;
            break;
        case Pvr_PixelTypeID_ASTC_8x8:
            (*minX) = 8;
            (*minY) = 8;
            (*minZ) = 1;
            break;
        case Pvr_PixelTypeID_ASTC_10x5:
            (*minX) = 10;
            (*minY) = 5;
            (*minZ) = 1;
            break;
        case Pvr_PixelTypeID_ASTC_10x6:
            (*minX) = 10;
            (*minY) = 6;
            (*minZ) = 1;
            break;
        case Pvr_PixelTypeID_ASTC_10x8:
            (*minX) = 10;
            (*minY) = 8;
            (*minZ) = 1;
            break;
        case Pvr_PixelTypeID_ASTC_10x10:
            (*minX) = 10;
            (*minY) = 10;
            (*minZ) = 1;
            break;
        case Pvr_PixelTypeID_ASTC_12x10:
            (*minX) = 12;
            (*minY) = 10;
            (*minZ) = 1;
            break;
        case Pvr_PixelTypeID_ASTC_12x12:
            (*minX) = 12;
            (*minY) = 12;
            (*minZ) = 1;
            break;
        case Pvr_PixelTypeID_ASTC_3x3x3:
            (*minX) = 3;
            (*minY) = 3;
            (*minZ) = 3;
            break;
        case Pvr_PixelTypeID_ASTC_4x3x3:
            (*minX) = 4;
            (*minY) = 3;
            (*minZ) = 3;
            break;
        case Pvr_PixelTypeID_ASTC_4x4x3:
            (*minX) = 4;
            (*minY) = 4;
            (*minZ) = 3;
            break;
        case Pvr_PixelTypeID_ASTC_4x4x4:
            (*minX) = 4;
            (*minY) = 4;
            (*minZ) = 4;
            break;
        case Pvr_PixelTypeID_ASTC_5x4x4:
            (*minX) = 5;
            (*minY) = 4;
            (*minZ) = 4;
            break;
        case Pvr_PixelTypeID_ASTC_5x5x4:
            (*minX) = 5;
            (*minY) = 5;
            (*minZ) = 4;
            break;
        case Pvr_PixelTypeID_ASTC_5x5x5:
            (*minX) = 5;
            (*minY) = 5;
            (*minZ) = 5;
            break;
        case Pvr_PixelTypeID_ASTC_6x5x5:
            (*minX) = 6;
            (*minY) = 5;
            (*minZ) = 5;
            break;
        case Pvr_PixelTypeID_ASTC_6x6x5:
            (*minX) = 6;
            (*minY) = 6;
            (*minZ) = 5;
            break;
        case Pvr_PixelTypeID_ASTC_6x6x6:
            (*minX) = 6;
            (*minY) = 6;
            (*minZ) = 6;
            break;
        default:
            return false;
        }

        return true;
    }
    else
    {
        (*minX) = 1;
        (*minY) = 1;
        (*minZ) = 1;
        return true;
    }
}

//--------------------------------------------------------------------------------------
static inline uint32_t Pvr_GetBitsPerPixel(uint64_t pixelFormat)
{
    if (0 == Pvr_GetPixelFormatPartHigh(pixelFormat))
    {
        switch (pixelFormat)
        {
        case Pvr_PixelTypeID_BW1bpp:
            return 1;
        case Pvr_PixelTypeID_PVRTCI_2bpp_RGB:
        case Pvr_PixelTypeID_PVRTCI_2bpp_RGBA:
        case Pvr_PixelTypeID_PVRTCII_2bpp:
            return 2;
        case Pvr_PixelTypeID_PVRTCI_4bpp_RGB:
        case Pvr_PixelTypeID_PVRTCI_4bpp_RGBA:
        case Pvr_PixelTypeID_PVRTCII_4bpp:
        case Pvr_PixelTypeID_ETC1:
        case Pvr_PixelTypeID_EAC_R11:
        case Pvr_PixelTypeID_ETC2_RGB:
        case Pvr_PixelTypeID_ETC2_RGB_A1:
        case Pvr_PixelTypeID_DXT1:
        case Pvr_PixelTypeID_BC4:
            return 4;
        case Pvr_PixelTypeID_DXT2:
        case Pvr_PixelTypeID_DXT3:
        case Pvr_PixelTypeID_DXT4:
        case Pvr_PixelTypeID_DXT5:
        case Pvr_PixelTypeID_BC5:
        case Pvr_PixelTypeID_EAC_RG11:
        case Pvr_PixelTypeID_ETC2_RGBA:
            return 8;
        case Pvr_PixelTypeID_YUY2:
        case Pvr_PixelTypeID_UYVY:
        case Pvr_PixelTypeID_RGBG8888:
        case Pvr_PixelTypeID_GRGB8888:
            return 16;
        case Pvr_PixelTypeID_SharedExponentR9G9B9E5:
            return 32;
        default:
            return 0;
        }
    }
    else
    {
        return reinterpret_cast<char *>(&pixelFormat)[4] + reinterpret_cast<char *>(&pixelFormat)[5] + reinterpret_cast<char *>(&pixelFormat)[6] + reinterpret_cast<char *>(&pixelFormat)[7];
    }
}

//--------------------------------------------------------------------------------------
static inline uint32_t _GetNeutralType(uint32_t height, uint32_t depth)
{
    assert(0 != height);
    assert(0 != depth);

    if (depth == 1)
    {
        if (height > 1)
        {
            return TEXTURE_LOADER_TYPE_2D;
        }
        else if (height == 1)
        {
            return TEXTURE_LOADER_TYPE_1D;
        }
        else
        {
            return TEXTURE_LOADER_TYPE_UNDEFINED;
        }
    }
    else if (depth > 1)
    {
        return TEXTURE_LOADER_TYPE_3D;
    }
    else
    {
        return TEXTURE_LOADER_TYPE_UNDEFINED;
    }
}

//--------------------------------------------------------------------------------------
static uint32_t const gPvrCompressedToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                                //Pvr_PixelTypeID_PVRTCI_2bpp_RGB
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                                //Pvr_PixelTypeID_PVRTCI_2bpp_RGBA
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                                //Pvr_PixelTypeID_PVRTCI_4bpp_RGB
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                                //Pvr_PixelTypeID_PVRTCI_4bpp_RGBA
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                                //Pvr_PixelTypeID_PVRTCII_2bpp
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                                //Pvr_PixelTypeID_PVRTCII_4bpp
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                                //Pvr_PixelTypeID_ETC1
    {TEXTURE_LOADER_FORMAT_BC1_RGBA_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_BC1_RGBA_SRGB_BLOCK},           //Pvr_PixelTypeID_DXT1
    {TEXTURE_LOADER_FORMAT_BC1_RGBA_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_BC1_RGBA_SRGB_BLOCK},           //Pvr_PixelTypeID_DXT2
    {TEXTURE_LOADER_FORMAT_BC2_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_BC2_SRGB_BLOCK},                     //Pvr_PixelTypeID_DXT3
    {TEXTURE_LOADER_FORMAT_BC2_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_BC2_SRGB_BLOCK},                     //Pvr_PixelTypeID_DXT4
    {TEXTURE_LOADER_FORMAT_BC3_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_BC3_SRGB_BLOCK},                     //Pvr_PixelTypeID_DXT5
    {TEXTURE_LOADER_FORMAT_BC4_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_UNDEFINED},                          //Pvr_PixelTypeID_BC4
    {TEXTURE_LOADER_FORMAT_BC5_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_UNDEFINED},                          //Pvr_PixelTypeID_BC5
    {TEXTURE_LOADER_FORMAT_BC6H_SFLOAT_BLOCK, TEXTURE_LOADER_FORMAT_UNDEFINED},                        //Pvr_PixelTypeID_BC6
    {TEXTURE_LOADER_FORMAT_BC7_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_BC7_SRGB_BLOCK},                     //Pvr_PixelTypeID_BC7
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                                //Pvr_PixelTypeID_UYVY
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                                //Pvr_PixelTypeID_YUY2
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                                //Pvr_PixelTypeID_BW1bpp
    {TEXTURE_LOADER_FORMAT_E5B9G9R9_UFLOAT_PACK32, TEXTURE_LOADER_FORMAT_UNDEFINED},                   //Pvr_PixelTypeID_SharedExponentR9G9B9E5,
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                                //Pvr_PixelTypeID_RGBG8888
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                                //Pvr_PixelTypeID_GRGB8888
    {TEXTURE_LOADER_FORMAT_ETC2_R8G8B8_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_ETC2_R8G8B8_SRGB_BLOCK},     //Pvr_PixelTypeID_ETC2_RGB
    {TEXTURE_LOADER_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK}, //Pvr_PixelTypeID_ETC2_RGBA
    {TEXTURE_LOADER_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK}, //Pvr_PixelTypeID_ETC2_RGB_A1
    {TEXTURE_LOADER_FORMAT_EAC_R11_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_EAC_R11_SNORM_BLOCK},            //Pvr_PixelTypeID_EAC_R11
    {TEXTURE_LOADER_FORMAT_EAC_R11G11_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_EAC_R11G11_SNORM_BLOCK},      //Pvr_PixelTypeID_EAC_RG11
    {TEXTURE_LOADER_FORMAT_ASTC_4x4_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_ASTC_4x4_SRGB_BLOCK},           //Pvr_PixelTypeID_ASTC_4x4
    {TEXTURE_LOADER_FORMAT_ASTC_5x4_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_ASTC_5x4_SRGB_BLOCK},           //Pvr_PixelTypeID_ASTC_5x4
    {TEXTURE_LOADER_FORMAT_ASTC_5x5_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_ASTC_5x5_SRGB_BLOCK},           //Pvr_PixelTypeID_ASTC_5x5
    {TEXTURE_LOADER_FORMAT_ASTC_6x5_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_ASTC_6x5_SRGB_BLOCK},           //Pvr_PixelTypeID_ASTC_6x5
    {TEXTURE_LOADER_FORMAT_ASTC_6x6_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_ASTC_6x6_SRGB_BLOCK},           //Pvr_PixelTypeID_ASTC_6x6
    {TEXTURE_LOADER_FORMAT_ASTC_8x5_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_ASTC_8x5_SRGB_BLOCK},           //Pvr_PixelTypeID_ASTC_8x5
    {TEXTURE_LOADER_FORMAT_ASTC_8x6_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_ASTC_8x6_SRGB_BLOCK},           //Pvr_PixelTypeID_ASTC_8x6
    {TEXTURE_LOADER_FORMAT_ASTC_8x8_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_ASTC_8x8_SRGB_BLOCK},           //Pvr_PixelTypeID_ASTC_8x8
    {TEXTURE_LOADER_FORMAT_ASTC_10x5_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_ASTC_10x5_SRGB_BLOCK},         //Pvr_PixelTypeID_ASTC_10x5
    {TEXTURE_LOADER_FORMAT_ASTC_10x6_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_ASTC_10x6_SRGB_BLOCK},         //Pvr_PixelTypeID_ASTC_10x6
    {TEXTURE_LOADER_FORMAT_ASTC_10x8_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_ASTC_10x8_SRGB_BLOCK},         //Pvr_PixelTypeID_ASTC_10x8
    {TEXTURE_LOADER_FORMAT_ASTC_10x10_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_ASTC_10x10_SRGB_BLOCK},       //Pvr_PixelTypeID_ASTC_10x10
    {TEXTURE_LOADER_FORMAT_ASTC_12x10_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_ASTC_12x10_SRGB_BLOCK},       //Pvr_PixelTypeID_ASTC_12x10
    {TEXTURE_LOADER_FORMAT_ASTC_12x12_UNORM_BLOCK, TEXTURE_LOADER_FORMAT_ASTC_12x12_SRGB_BLOCK},       //Pvr_PixelTypeID_ASTC_12x12
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                                //Pvr_PixelTypeID_ASTC_3x3x3
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                                //Pvr_PixelTypeID_ASTC_4x3x3
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                                //Pvr_PixelTypeID_ASTC_4x4x3
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                                //Pvr_PixelTypeID_ASTC_4x4x4
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                                //Pvr_PixelTypeID_ASTC_5x4x4
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                                //Pvr_PixelTypeID_ASTC_5x5x4
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                                //Pvr_PixelTypeID_ASTC_5x5x5
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                                //Pvr_PixelTypeID_ASTC_6x5x5
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                                //Pvr_PixelTypeID_ASTC_6x6x5
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}                                 //Pvr_PixelTypeID_ASTC_6x6x6
};
static_assert(Pvr_PixelTypeID_NumCompressedPFs == (sizeof(gPvrCompressedToNeutralFormatMap) / sizeof(gPvrCompressedToNeutralFormatMap[0])), "gPvrToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrCompressedToNeutralFormatMap[0]) / sizeof(gPvrCompressedToNeutralFormatMap[0][0])), "gPvrToNeutralFormatMap may not match!");

static uint32_t const gPvrR8ToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_R8_UNORM, TEXTURE_LOADER_FORMAT_R8_SRGB},    //Pvr_ChannelType_UnsignedByteNorm
    {TEXTURE_LOADER_FORMAT_R8_SNORM, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedByteNorm
    {TEXTURE_LOADER_FORMAT_R8_UINT, TEXTURE_LOADER_FORMAT_UNDEFINED},   //Pvr_ChannelType_UnsignedByte
    {TEXTURE_LOADER_FORMAT_R8_SINT, TEXTURE_LOADER_FORMAT_UNDEFINED},   //Pvr_ChannelType_SignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_UnsignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_UnsignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_UnsignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_UnsignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedFloat
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}  //Pvr_ChannelType_UnsignedFloat
};
static_assert(Pvr_ChannelType_NumCTs == (sizeof(gPvrR8ToNeutralFormatMap) / sizeof(gPvrR8ToNeutralFormatMap[0])), "gPvrR8ToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrR8ToNeutralFormatMap[0]) / sizeof(gPvrR8ToNeutralFormatMap[0][0])), "gPvrR8ToNeutralFormatMap may not match!");

static uint32_t const gPvrR8G8ToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_R8G8_UNORM, TEXTURE_LOADER_FORMAT_R8G8_SRGB}, //Pvr_ChannelType_UnsignedByteNorm
    {TEXTURE_LOADER_FORMAT_R8G8_SNORM, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedByteNorm
    {TEXTURE_LOADER_FORMAT_R8G8_UINT, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedByte
    {TEXTURE_LOADER_FORMAT_R8G8_SINT, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedFloat
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}   //Pvr_ChannelType_UnsignedFloat
};
static_assert(Pvr_ChannelType_NumCTs == (sizeof(gPvrR8G8ToNeutralFormatMap) / sizeof(gPvrR8G8ToNeutralFormatMap[0])), "gPvrR8G8ToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrR8G8ToNeutralFormatMap[0]) / sizeof(gPvrR8G8ToNeutralFormatMap[0][0])), "gPvrR8G8ToNeutralFormatMap may not match!");

static uint32_t const gPvrR8G8B8ToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_R8G8B8_UNORM, TEXTURE_LOADER_FORMAT_R8G8B8_SRGB}, //Pvr_ChannelType_UnsignedByteNorm
    {TEXTURE_LOADER_FORMAT_R8G8B8_SNORM, TEXTURE_LOADER_FORMAT_UNDEFINED},   //Pvr_ChannelType_SignedByteNorm
    {TEXTURE_LOADER_FORMAT_R8G8B8_UINT, TEXTURE_LOADER_FORMAT_UNDEFINED},    //Pvr_ChannelType_UnsignedByte
    {TEXTURE_LOADER_FORMAT_R8G8B8_SINT, TEXTURE_LOADER_FORMAT_UNDEFINED},    //Pvr_ChannelType_SignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},      //Pvr_ChannelType_UnsignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},      //Pvr_ChannelType_SignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},      //Pvr_ChannelType_UnsignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},      //Pvr_ChannelType_SignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},      //Pvr_ChannelType_UnsignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},      //Pvr_ChannelType_SignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},      //Pvr_ChannelType_UnsignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},      //Pvr_ChannelType_SignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},      //Pvr_ChannelType_SignedFloat
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}       //Pvr_ChannelType_UnsignedFloat
};
static_assert(Pvr_ChannelType_NumCTs == (sizeof(gPvrR8G8B8ToNeutralFormatMap) / sizeof(gPvrR8G8B8ToNeutralFormatMap[0])), "gPvrR8G8B8ToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrR8G8B8ToNeutralFormatMap[0]) / sizeof(gPvrR8G8B8ToNeutralFormatMap[0][0])), "gPvrR8G8B8ToNeutralFormatMap may not match!");

static uint32_t const gPvrB8G8R8ToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_B8G8R8_UNORM, TEXTURE_LOADER_FORMAT_B8G8R8_SRGB}, //Pvr_ChannelType_UnsignedByteNorm
    {TEXTURE_LOADER_FORMAT_B8G8R8_SNORM, TEXTURE_LOADER_FORMAT_UNDEFINED},   //Pvr_ChannelType_SignedByteNorm
    {TEXTURE_LOADER_FORMAT_B8G8R8_UINT, TEXTURE_LOADER_FORMAT_UNDEFINED},    //Pvr_ChannelType_UnsignedByte
    {TEXTURE_LOADER_FORMAT_B8G8R8_SINT, TEXTURE_LOADER_FORMAT_UNDEFINED},    //Pvr_ChannelType_SignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},      //Pvr_ChannelType_UnsignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},      //Pvr_ChannelType_SignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},      //Pvr_ChannelType_UnsignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},      //Pvr_ChannelType_SignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},      //Pvr_ChannelType_UnsignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},      //Pvr_ChannelType_SignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},      //Pvr_ChannelType_UnsignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},      //Pvr_ChannelType_SignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},      //Pvr_ChannelType_SignedFloat
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}       //Pvr_ChannelType_UnsignedFloat
};
static_assert(Pvr_ChannelType_NumCTs == (sizeof(gPvrB8G8R8ToNeutralFormatMap) / sizeof(gPvrB8G8R8ToNeutralFormatMap[0])), "gPvrB8G8R8ToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrB8G8R8ToNeutralFormatMap[0]) / sizeof(gPvrB8G8R8ToNeutralFormatMap[0][0])), "gPvrB8G8R8ToNeutralFormatMap may not match!");

static uint32_t const gPvrR8G8B8A8ToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_R8G8B8A8_UNORM, TEXTURE_LOADER_FORMAT_R8G8B8A8_SRGB}, //Pvr_ChannelType_UnsignedByteNorm
    {TEXTURE_LOADER_FORMAT_R8G8B8A8_SNORM, TEXTURE_LOADER_FORMAT_UNDEFINED},     //Pvr_ChannelType_SignedByteNorm
    {TEXTURE_LOADER_FORMAT_R8G8B8A8_UINT, TEXTURE_LOADER_FORMAT_UNDEFINED},      //Pvr_ChannelType_UnsignedByte
    {TEXTURE_LOADER_FORMAT_R8G8B8A8_SINT, TEXTURE_LOADER_FORMAT_UNDEFINED},      //Pvr_ChannelType_SignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_UnsignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_SignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_UnsignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_SignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_UnsignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_SignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_UnsignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_SignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_SignedFloat
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}           //Pvr_ChannelType_UnsignedFloat
};
static_assert(Pvr_ChannelType_NumCTs == (sizeof(gPvrR8G8B8A8ToNeutralFormatMap) / sizeof(gPvrR8G8B8A8ToNeutralFormatMap[0])), "gPvrR8G8B8A8ToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrR8G8B8A8ToNeutralFormatMap[0]) / sizeof(gPvrR8G8B8A8ToNeutralFormatMap[0][0])), "gPvrR8G8B8A8ToNeutralFormatMap may not match!");

static uint32_t const gPvrB8G8R8A8ToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_B8G8R8A8_UNORM, TEXTURE_LOADER_FORMAT_B8G8R8A8_SRGB}, //Pvr_ChannelType_UnsignedByteNorm
    {TEXTURE_LOADER_FORMAT_B8G8R8A8_SNORM, TEXTURE_LOADER_FORMAT_UNDEFINED},     //Pvr_ChannelType_SignedByteNorm
    {TEXTURE_LOADER_FORMAT_B8G8R8A8_UINT, TEXTURE_LOADER_FORMAT_UNDEFINED},      //Pvr_ChannelType_UnsignedByte
    {TEXTURE_LOADER_FORMAT_B8G8R8A8_SINT, TEXTURE_LOADER_FORMAT_UNDEFINED},      //Pvr_ChannelType_SignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_UnsignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_SignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_UnsignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_SignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_UnsignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_SignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_UnsignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_SignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_SignedFloat
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}           //Pvr_ChannelType_UnsignedFloat
};
static_assert(Pvr_ChannelType_NumCTs == (sizeof(gPvrB8G8R8A8ToNeutralFormatMap) / sizeof(gPvrB8G8R8A8ToNeutralFormatMap[0])), "gPvrB8G8R8A8ToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrB8G8R8A8ToNeutralFormatMap[0]) / sizeof(gPvrB8G8R8A8ToNeutralFormatMap[0][0])), "gPvrB8G8R8A8ToNeutralFormatMap may not match!");

static uint32_t const gPvrA8B8G8R8ToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_A8B8G8R8_UNORM_PACK32, TEXTURE_LOADER_FORMAT_A8B8G8R8_SRGB_PACK32}, //Pvr_ChannelType_UnsignedByteNorm
    {TEXTURE_LOADER_FORMAT_A8B8G8R8_SNORM_PACK32, TEXTURE_LOADER_FORMAT_UNDEFINED},            //Pvr_ChannelType_SignedByteNorm
    {TEXTURE_LOADER_FORMAT_A8B8G8R8_UINT_PACK32, TEXTURE_LOADER_FORMAT_UNDEFINED},             //Pvr_ChannelType_UnsignedByte
    {TEXTURE_LOADER_FORMAT_A8B8G8R8_SINT_PACK32, TEXTURE_LOADER_FORMAT_UNDEFINED},             //Pvr_ChannelType_SignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                        //Pvr_ChannelType_UnsignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                        //Pvr_ChannelType_SignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                        //Pvr_ChannelType_UnsignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                        //Pvr_ChannelType_SignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                        //Pvr_ChannelType_UnsignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                        //Pvr_ChannelType_SignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                        //Pvr_ChannelType_UnsignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                        //Pvr_ChannelType_SignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},                        //Pvr_ChannelType_SignedFloat
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}                         //Pvr_ChannelType_UnsignedFloat
};
static_assert(Pvr_ChannelType_NumCTs == (sizeof(gPvrA8B8G8R8ToNeutralFormatMap) / sizeof(gPvrA8B8G8R8ToNeutralFormatMap[0])), "gPvrA8B8G8R8ToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrA8B8G8R8ToNeutralFormatMap[0]) / sizeof(gPvrA8B8G8R8ToNeutralFormatMap[0][0])), "gPvrA8B8G8R8ToNeutralFormatMap may not match!");

static uint32_t const gPvrR16ToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedByte
    {TEXTURE_LOADER_FORMAT_R16_UNORM, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedShortNorm
    {TEXTURE_LOADER_FORMAT_R16_SNORM, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedShortNorm
    {TEXTURE_LOADER_FORMAT_R16_UINT, TEXTURE_LOADER_FORMAT_UNDEFINED},   //Pvr_ChannelType_UnsignedShort
    {TEXTURE_LOADER_FORMAT_R16_SINT, TEXTURE_LOADER_FORMAT_UNDEFINED},   //Pvr_ChannelType_SignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedInteger
    {TEXTURE_LOADER_FORMAT_R16_SFLOAT, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedFloat
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}   //Pvr_ChannelType_UnsignedFloat
};
static_assert(Pvr_ChannelType_NumCTs == (sizeof(gPvrR16ToNeutralFormatMap) / sizeof(gPvrR16ToNeutralFormatMap[0])), "gPvrR16ToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrR16ToNeutralFormatMap[0]) / sizeof(gPvrR16ToNeutralFormatMap[0][0])), "gPvrR16ToNeutralFormatMap may not match!");

static uint32_t const gPvrR16G16ToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},     //Pvr_ChannelType_UnsignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},     //Pvr_ChannelType_SignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},     //Pvr_ChannelType_UnsignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},     //Pvr_ChannelType_SignedByte
    {TEXTURE_LOADER_FORMAT_R16G16_UNORM, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedShortNorm
    {TEXTURE_LOADER_FORMAT_R16G16_SNORM, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedShortNorm
    {TEXTURE_LOADER_FORMAT_R16G16_UINT, TEXTURE_LOADER_FORMAT_UNDEFINED},   //Pvr_ChannelType_UnsignedShort
    {TEXTURE_LOADER_FORMAT_R16G16_SINT, TEXTURE_LOADER_FORMAT_UNDEFINED},   //Pvr_ChannelType_SignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},     //Pvr_ChannelType_UnsignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},     //Pvr_ChannelType_SignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},     //Pvr_ChannelType_UnsignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},     //Pvr_ChannelType_SignedInteger
    {TEXTURE_LOADER_FORMAT_R16G16_SFLOAT, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedFloat
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}      //Pvr_ChannelType_UnsignedFloat
};
static_assert(Pvr_ChannelType_NumCTs == (sizeof(gPvrR16G16ToNeutralFormatMap) / sizeof(gPvrR16G16ToNeutralFormatMap[0])), "gPvrR16G16ToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrR16G16ToNeutralFormatMap[0]) / sizeof(gPvrR16G16ToNeutralFormatMap[0][0])), "gPvrR16G16ToNeutralFormatMap may not match!");

static uint32_t const gPvrR16G16B16ToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},        //Pvr_ChannelType_UnsignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},        //Pvr_ChannelType_SignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},        //Pvr_ChannelType_UnsignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},        //Pvr_ChannelType_SignedByte
    {TEXTURE_LOADER_FORMAT_R16G16B16_UNORM, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedShortNorm
    {TEXTURE_LOADER_FORMAT_R16G16B16_SNORM, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedShortNorm
    {TEXTURE_LOADER_FORMAT_R16G16B16_UINT, TEXTURE_LOADER_FORMAT_UNDEFINED},   //Pvr_ChannelType_UnsignedShort
    {TEXTURE_LOADER_FORMAT_R16G16B16_SINT, TEXTURE_LOADER_FORMAT_UNDEFINED},   //Pvr_ChannelType_SignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},        //Pvr_ChannelType_UnsignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},        //Pvr_ChannelType_SignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},        //Pvr_ChannelType_UnsignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},        //Pvr_ChannelType_SignedInteger
    {TEXTURE_LOADER_FORMAT_R16G16B16_SFLOAT, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedFloat
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}         //Pvr_ChannelType_UnsignedFloat
};
static_assert(Pvr_ChannelType_NumCTs == (sizeof(gPvrR16G16B16ToNeutralFormatMap) / sizeof(gPvrR16G16B16ToNeutralFormatMap[0])), "gPvrR16G16B16ToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrR16G16B16ToNeutralFormatMap[0]) / sizeof(gPvrR16G16B16ToNeutralFormatMap[0][0])), "gPvrR16G16B16ToNeutralFormatMap may not match!");

static uint32_t const gPvrR16G16B16A16ToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_UnsignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_SignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_UnsignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_SignedByte
    {TEXTURE_LOADER_FORMAT_R16G16B16A16_UNORM, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedShortNorm
    {TEXTURE_LOADER_FORMAT_R16G16B16A16_SNORM, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedShortNorm
    {TEXTURE_LOADER_FORMAT_R16G16B16A16_UINT, TEXTURE_LOADER_FORMAT_UNDEFINED},   //Pvr_ChannelType_UnsignedShort
    {TEXTURE_LOADER_FORMAT_R16G16B16A16_SINT, TEXTURE_LOADER_FORMAT_UNDEFINED},   //Pvr_ChannelType_SignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_UnsignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_SignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_UnsignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_SignedInteger
    {TEXTURE_LOADER_FORMAT_R16G16B16A16_SFLOAT, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedFloat
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}            //Pvr_ChannelType_UnsignedFloat
};
static_assert(Pvr_ChannelType_NumCTs == (sizeof(gPvrR16G16B16A16ToNeutralFormatMap) / sizeof(gPvrR16G16B16A16ToNeutralFormatMap[0])), "gPvrR16G16B16A16ToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrR16G16B16A16ToNeutralFormatMap[0]) / sizeof(gPvrR16G16B16A16ToNeutralFormatMap[0][0])), "gPvrR16G16B16A16ToNeutralFormatMap may not match!");

static uint32_t const gPvrR32ToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedShortNorm
    {TEXTURE_LOADER_FORMAT_R32_UINT, TEXTURE_LOADER_FORMAT_UNDEFINED},   //Pvr_ChannelType_UnsignedShort
    {TEXTURE_LOADER_FORMAT_R32_SINT, TEXTURE_LOADER_FORMAT_UNDEFINED},   //Pvr_ChannelType_SignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedInteger
    {TEXTURE_LOADER_FORMAT_R32_SFLOAT, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedFloat
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}   //Pvr_ChannelType_UnsignedFloat
};
static_assert(Pvr_ChannelType_NumCTs == (sizeof(gPvrR32ToNeutralFormatMap) / sizeof(gPvrR32ToNeutralFormatMap[0])), "gPvrR32ToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrR32ToNeutralFormatMap[0]) / sizeof(gPvrR32ToNeutralFormatMap[0][0])), "gPvrR32ToNeutralFormatMap may not match!");

static uint32_t const gPvrR32G32ToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},     //Pvr_ChannelType_UnsignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},     //Pvr_ChannelType_SignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},     //Pvr_ChannelType_UnsignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},     //Pvr_ChannelType_SignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},     //Pvr_ChannelType_UnsignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},     //Pvr_ChannelType_SignedShortNorm
    {TEXTURE_LOADER_FORMAT_R32G32_UINT, TEXTURE_LOADER_FORMAT_UNDEFINED},   //Pvr_ChannelType_UnsignedShort
    {TEXTURE_LOADER_FORMAT_R32G32_SINT, TEXTURE_LOADER_FORMAT_UNDEFINED},   //Pvr_ChannelType_SignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},     //Pvr_ChannelType_UnsignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},     //Pvr_ChannelType_SignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},     //Pvr_ChannelType_UnsignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},     //Pvr_ChannelType_SignedInteger
    {TEXTURE_LOADER_FORMAT_R32G32_SFLOAT, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedFloat
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}      //Pvr_ChannelType_UnsignedFloat
};
static_assert(Pvr_ChannelType_NumCTs == (sizeof(gPvrR32G32ToNeutralFormatMap) / sizeof(gPvrR32G32ToNeutralFormatMap[0])), "gPvrR32G32ToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrR32G32ToNeutralFormatMap[0]) / sizeof(gPvrR32G32ToNeutralFormatMap[0][0])), "gPvrR32G32ToNeutralFormatMap may not match!");

static uint32_t const gPvrR32G32B32ToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},        //Pvr_ChannelType_UnsignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},        //Pvr_ChannelType_SignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},        //Pvr_ChannelType_UnsignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},        //Pvr_ChannelType_SignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},        //Pvr_ChannelType_UnsignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},        //Pvr_ChannelType_SignedShortNorm
    {TEXTURE_LOADER_FORMAT_R32G32B32_UINT, TEXTURE_LOADER_FORMAT_UNDEFINED},   //Pvr_ChannelType_UnsignedShort
    {TEXTURE_LOADER_FORMAT_R32G32B32_SINT, TEXTURE_LOADER_FORMAT_UNDEFINED},   //Pvr_ChannelType_SignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},        //Pvr_ChannelType_UnsignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},        //Pvr_ChannelType_SignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},        //Pvr_ChannelType_UnsignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},        //Pvr_ChannelType_SignedInteger
    {TEXTURE_LOADER_FORMAT_R32G32B32_SFLOAT, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedFloat
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}         //Pvr_ChannelType_UnsignedFloat
};
static_assert(Pvr_ChannelType_NumCTs == (sizeof(gPvrR32G32B32ToNeutralFormatMap) / sizeof(gPvrR32G32B32ToNeutralFormatMap[0])), "gPvrR32G32B32ToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrR32G32B32ToNeutralFormatMap[0]) / sizeof(gPvrR32G32B32ToNeutralFormatMap[0][0])), "gPvrR32G32B32ToNeutralFormatMap may not match!");

static uint32_t const gPvrR32G32B32A32ToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_UnsignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_SignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_UnsignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_SignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_UnsignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_SignedShortNorm
    {TEXTURE_LOADER_FORMAT_R32G32B32A32_UINT, TEXTURE_LOADER_FORMAT_UNDEFINED},   //Pvr_ChannelType_UnsignedShort
    {TEXTURE_LOADER_FORMAT_R32G32B32A32_SINT, TEXTURE_LOADER_FORMAT_UNDEFINED},   //Pvr_ChannelType_SignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_UnsignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_SignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_UnsignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_SignedInteger
    {TEXTURE_LOADER_FORMAT_R32G32B32A32_SFLOAT, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedFloat
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}            //Pvr_ChannelType_UnsignedFloat
};
static_assert(Pvr_ChannelType_NumCTs == (sizeof(gPvrR32G32B32A32ToNeutralFormatMap) / sizeof(gPvrR32G32B32A32ToNeutralFormatMap[0])), "gPvrR32G32B32A32ToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrR32G32B32A32ToNeutralFormatMap[0]) / sizeof(gPvrR32G32B32A32ToNeutralFormatMap[0][0])), "gPvrR32G32B32A32ToNeutralFormatMap may not match!");

static uint32_t const gPvrR5G6B5ToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_UnsignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_SignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_UnsignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_SignedByte
    {TEXTURE_LOADER_FORMAT_R5G6B5_UNORM_PACK16, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_UnsignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_SignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_UnsignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_SignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_UnsignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_SignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_UnsignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_SignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_SignedFloat
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}            //Pvr_ChannelType_UnsignedFloat
};
static_assert(Pvr_ChannelType_NumCTs == (sizeof(gPvrR5G6B5ToNeutralFormatMap) / sizeof(gPvrR5G6B5ToNeutralFormatMap[0])), "gPvrR5G6B5ToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrR5G6B5ToNeutralFormatMap[0]) / sizeof(gPvrR5G6B5ToNeutralFormatMap[0][0])), "gPvrR5G6B5ToNeutralFormatMap may not match!");

static uint32_t const gPvrR4G4B4A4ToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},             //Pvr_ChannelType_UnsignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},             //Pvr_ChannelType_SignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},             //Pvr_ChannelType_UnsignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},             //Pvr_ChannelType_SignedByte
    {TEXTURE_LOADER_FORMAT_R4G4B4A4_UNORM_PACK16, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_UnsignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},             //Pvr_ChannelType_SignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},             //Pvr_ChannelType_UnsignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},             //Pvr_ChannelType_SignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},             //Pvr_ChannelType_UnsignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},             //Pvr_ChannelType_SignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},             //Pvr_ChannelType_UnsignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},             //Pvr_ChannelType_SignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},             //Pvr_ChannelType_SignedFloat
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}              //Pvr_ChannelType_UnsignedFloat
};
static_assert(Pvr_ChannelType_NumCTs == (sizeof(gPvrR4G4B4A4ToNeutralFormatMap) / sizeof(gPvrR4G4B4A4ToNeutralFormatMap[0])), "gPvrR4G4B4A4ToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrR4G4B4A4ToNeutralFormatMap[0]) / sizeof(gPvrR4G4B4A4ToNeutralFormatMap[0][0])), "gPvrR4G4B4A4ToNeutralFormatMap may not match!");

static uint32_t const gPvrR11G11B10ToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},              //Pvr_ChannelType_UnsignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},              //Pvr_ChannelType_SignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},              //Pvr_ChannelType_UnsignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},              //Pvr_ChannelType_SignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},              //Pvr_ChannelType_UnsignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},              //Pvr_ChannelType_SignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},              //Pvr_ChannelType_UnsignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},              //Pvr_ChannelType_SignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},              //Pvr_ChannelType_UnsignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},              //Pvr_ChannelType_SignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},              //Pvr_ChannelType_UnsignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},              //Pvr_ChannelType_SignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},              //Pvr_ChannelType_SignedFloat
    {TEXTURE_LOADER_FORMAT_B10G11R11_UFLOAT_PACK32, TEXTURE_LOADER_FORMAT_UNDEFINED} //Pvr_ChannelType_UnsignedFloat
};
static_assert(Pvr_ChannelType_NumCTs == (sizeof(gPvrR11G11B10ToNeutralFormatMap) / sizeof(gPvrR11G11B10ToNeutralFormatMap[0])), "gPvrR11G11B10ToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrR11G11B10ToNeutralFormatMap[0]) / sizeof(gPvrR11G11B10ToNeutralFormatMap[0][0])), "gPvrR11G11B10ToNeutralFormatMap may not match!");

static uint32_t const gPvrD8ToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_UnsignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_UnsignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_UnsignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_UnsignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_UnsignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_UnsignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedInteger
    {TEXTURE_LOADER_FORMAT_R8_UNORM, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedFloat
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}  //Pvr_ChannelType_UnsignedFloat
};
static_assert(Pvr_ChannelType_NumCTs == (sizeof(gPvrD8ToNeutralFormatMap) / sizeof(gPvrD8ToNeutralFormatMap[0])), "gPvrD8ToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrD8ToNeutralFormatMap[0]) / sizeof(gPvrD8ToNeutralFormatMap[0][0])), "gPvrD8ToNeutralFormatMap may not match!");

static uint32_t const gPvrS8ToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_UnsignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_UnsignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_UnsignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_UnsignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_UnsignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_UnsignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedInteger
    {TEXTURE_LOADER_FORMAT_R8_UNORM, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedFloat
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}  //Pvr_ChannelType_UnsignedFloat
};
static_assert(Pvr_ChannelType_NumCTs == (sizeof(gPvrS8ToNeutralFormatMap) / sizeof(gPvrS8ToNeutralFormatMap[0])), "gPvrS8ToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrS8ToNeutralFormatMap[0]) / sizeof(gPvrS8ToNeutralFormatMap[0][0])), "gPvrS8ToNeutralFormatMap may not match!");

static uint32_t const gPvrD16ToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedShortNorm
    {TEXTURE_LOADER_FORMAT_R16_UNORM, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedInteger
    {TEXTURE_LOADER_FORMAT_R16_SFLOAT, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedFloat
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}   //Pvr_ChannelType_UnsignedFloat
};
static_assert(Pvr_ChannelType_NumCTs == (sizeof(gPvrD16ToNeutralFormatMap) / sizeof(gPvrD16ToNeutralFormatMap[0])), "gPvrD16ToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrD16ToNeutralFormatMap[0]) / sizeof(gPvrD16ToNeutralFormatMap[0][0])), "gPvrD16ToNeutralFormatMap may not match!");

static uint32_t const gPvrD24ToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_UnsignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_SignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_UnsignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_SignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_UnsignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_SignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_UnsignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_SignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_UnsignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_SignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_UnsignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},           //Pvr_ChannelType_SignedInteger
    {TEXTURE_LOADER_FORMAT_X8_D24_UNORM_PACK32, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedFloat
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}            //Pvr_ChannelType_UnsignedFloat
};
static_assert(Pvr_ChannelType_NumCTs == (sizeof(gPvrD24ToNeutralFormatMap) / sizeof(gPvrD24ToNeutralFormatMap[0])), "gPvrD24ToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrD24ToNeutralFormatMap[0]) / sizeof(gPvrD24ToNeutralFormatMap[0][0])), "gPvrD24ToNeutralFormatMap may not match!");

static uint32_t const gPvrD32ToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_UnsignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},  //Pvr_ChannelType_SignedInteger
    {TEXTURE_LOADER_FORMAT_R32_SFLOAT, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedFloat
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}   //Pvr_ChannelType_UnsignedFloat
};
static_assert(Pvr_ChannelType_NumCTs == (sizeof(gPvrD32ToNeutralFormatMap) / sizeof(gPvrD32ToNeutralFormatMap[0])), "gPvrD32ToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrD32ToNeutralFormatMap[0]) / sizeof(gPvrD32ToNeutralFormatMap[0][0])), "gPvrD32ToNeutralFormatMap may not match!");

static uint32_t const gPvrD16S8ToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},         //Pvr_ChannelType_UnsignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},         //Pvr_ChannelType_SignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},         //Pvr_ChannelType_UnsignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},         //Pvr_ChannelType_SignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},         //Pvr_ChannelType_UnsignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},         //Pvr_ChannelType_SignedShortNorm
    {TEXTURE_LOADER_FORMAT_D16_UNORM_S8_UINT, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_UnsignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},         //Pvr_ChannelType_SignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},         //Pvr_ChannelType_UnsignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},         //Pvr_ChannelType_SignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},         //Pvr_ChannelType_UnsignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},         //Pvr_ChannelType_SignedInteger
    {TEXTURE_LOADER_FORMAT_D16_UNORM_S8_UINT, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedFloat
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}          //Pvr_ChannelType_UnsignedFloat
};
static_assert(Pvr_ChannelType_NumCTs == (sizeof(gPvrD16S8ToNeutralFormatMap) / sizeof(gPvrD16S8ToNeutralFormatMap[0])), "gPvrD16S8ToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrD16S8ToNeutralFormatMap[0]) / sizeof(gPvrD16S8ToNeutralFormatMap[0][0])), "gPvrD16S8ToNeutralFormatMap may not match!");

static uint32_t const gPvrD24S8ToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},         //Pvr_ChannelType_UnsignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},         //Pvr_ChannelType_SignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},         //Pvr_ChannelType_UnsignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},         //Pvr_ChannelType_SignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},         //Pvr_ChannelType_UnsignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},         //Pvr_ChannelType_SignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},         //Pvr_ChannelType_UnsignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},         //Pvr_ChannelType_SignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},         //Pvr_ChannelType_UnsignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},         //Pvr_ChannelType_SignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_D24_UNORM_S8_UINT, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_UnsignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},         //Pvr_ChannelType_SignedInteger
    {TEXTURE_LOADER_FORMAT_D24_UNORM_S8_UINT, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedFloat
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}          //Pvr_ChannelType_UnsignedFloat
};
static_assert(Pvr_ChannelType_NumCTs == (sizeof(gPvrD24S8ToNeutralFormatMap) / sizeof(gPvrD24S8ToNeutralFormatMap[0])), "gPvrD24S8ToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrD24S8ToNeutralFormatMap[0]) / sizeof(gPvrD24S8ToNeutralFormatMap[0][0])), "gPvrD24S8ToNeutralFormatMap may not match!");

static uint32_t const gPvrD32S8ToNeutralFormatMap[][2] = {
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_UnsignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_SignedByteNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_UnsignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_SignedByte
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_UnsignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_SignedShortNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_UnsignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_SignedShort
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_UnsignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_SignedIntegerNorm
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_UnsignedInteger
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED},          //Pvr_ChannelType_SignedInteger
    {TEXTURE_LOADER_FORMAT_D32_SFLOAT_S8_UINT, TEXTURE_LOADER_FORMAT_UNDEFINED}, //Pvr_ChannelType_SignedFloat
    {TEXTURE_LOADER_FORMAT_UNDEFINED, TEXTURE_LOADER_FORMAT_UNDEFINED}           //Pvr_ChannelType_UnsignedFloat
};
static_assert(Pvr_ChannelType_NumCTs == (sizeof(gPvrD32S8ToNeutralFormatMap) / sizeof(gPvrD32S8ToNeutralFormatMap[0])), "gPvrD32S8ToNeutralFormatMap may not match!");
static_assert(Pvr_ColorSpace_NumCSs == (sizeof(gPvrD32S8ToNeutralFormatMap[0]) / sizeof(gPvrD32S8ToNeutralFormatMap[0][0])), "gPvrD32S8ToNeutralFormatMap may not match!");

static inline uint32_t _GetNeutralFormat(uint64_t pixelFormat, uint32_t colorSpace, uint32_t channelType)
{
    uint32_t neutralFormat;

    if (0 == Pvr_GetPixelFormatPartHigh(pixelFormat))
    {
        assert(pixelFormat < (sizeof(gPvrCompressedToNeutralFormatMap) / sizeof(gPvrCompressedToNeutralFormatMap[0])));
        assert(colorSpace < (sizeof(gPvrCompressedToNeutralFormatMap[0]) / sizeof(gPvrCompressedToNeutralFormatMap[0][0])));
        neutralFormat = gPvrCompressedToNeutralFormatMap[pixelFormat][colorSpace];

        if (neutralFormat == TEXTURE_LOADER_FORMAT_BC6H_SFLOAT_BLOCK && channelType == Pvr_ChannelType_UnsignedFloat)
        {
            neutralFormat = TEXTURE_LOADER_FORMAT_BC6H_UFLOAT_BLOCK;
        }
    }
    else
    {
        switch (pixelFormat)
        {
        case Pvr_PixelTypeChar_I8:
        case Pvr_PixelTypeChar_R8:
        {
            assert(channelType < (sizeof(gPvrR8ToNeutralFormatMap) / sizeof(gPvrR8ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrR8ToNeutralFormatMap[0]) / sizeof(gPvrR8ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrR8ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        case Pvr_PixelTypeChar_R8G8:
        {
            assert(channelType < (sizeof(gPvrR8G8ToNeutralFormatMap) / sizeof(gPvrR8G8ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrR8G8ToNeutralFormatMap[0]) / sizeof(gPvrR8G8ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrR8G8ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        case Pvr_PixelTypeChar_R8G8B8:
        {
            assert(channelType < (sizeof(gPvrR8G8B8ToNeutralFormatMap) / sizeof(gPvrR8G8B8ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrR8G8B8ToNeutralFormatMap[0]) / sizeof(gPvrR8G8B8ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrR8G8B8ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        case Pvr_PixelTypeChar_B8G8R8:
        {
            assert(channelType < (sizeof(gPvrB8G8R8ToNeutralFormatMap) / sizeof(gPvrB8G8R8ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrB8G8R8ToNeutralFormatMap[0]) / sizeof(gPvrB8G8R8ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrB8G8R8ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        case Pvr_PixelTypeChar_R8G8B8A8:
        {
            assert(channelType < (sizeof(gPvrR8G8B8A8ToNeutralFormatMap) / sizeof(gPvrR8G8B8A8ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrR8G8B8A8ToNeutralFormatMap[0]) / sizeof(gPvrR8G8B8A8ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrR8G8B8A8ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        case Pvr_PixelTypeChar_B8G8R8A8:
        {
            assert(channelType < (sizeof(gPvrB8G8R8A8ToNeutralFormatMap) / sizeof(gPvrB8G8R8A8ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrB8G8R8A8ToNeutralFormatMap[0]) / sizeof(gPvrB8G8R8A8ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrB8G8R8A8ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        case Pvr_PixelTypeChar_A8B8G8R8:
        {
            assert(channelType < (sizeof(gPvrA8B8G8R8ToNeutralFormatMap) / sizeof(gPvrA8B8G8R8ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrA8B8G8R8ToNeutralFormatMap[0]) / sizeof(gPvrA8B8G8R8ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrA8B8G8R8ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        case Pvr_PixelTypeChar_R16:
        {
            assert(channelType < (sizeof(gPvrR16ToNeutralFormatMap) / sizeof(gPvrR16ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrR16ToNeutralFormatMap[0]) / sizeof(gPvrR16ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrR16ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        case Pvr_PixelTypeChar_R16G16:
        case Pvr_PixelTypeChar_L16A16:
        {
            assert(channelType < (sizeof(gPvrR16G16ToNeutralFormatMap) / sizeof(gPvrR16G16ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrR16G16ToNeutralFormatMap[0]) / sizeof(gPvrR16G16ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrR16G16ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        case Pvr_PixelTypeChar_R16G16B16:
        {
            assert(channelType < (sizeof(gPvrR16G16B16ToNeutralFormatMap) / sizeof(gPvrR16G16B16ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrR16G16B16ToNeutralFormatMap[0]) / sizeof(gPvrR16G16B16ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrR16G16B16ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        case Pvr_PixelTypeChar_R16G16B16A16:
        {
            assert(channelType < (sizeof(gPvrR16G16B16A16ToNeutralFormatMap) / sizeof(gPvrR16G16B16A16ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrR16G16B16A16ToNeutralFormatMap[0]) / sizeof(gPvrR16G16B16A16ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrR16G16B16A16ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        case Pvr_PixelTypeChar_R32:
        case Pvr_PixelTypeChar_L32:
        {
            assert(channelType < (sizeof(gPvrR32ToNeutralFormatMap) / sizeof(gPvrR32ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrR32ToNeutralFormatMap[0]) / sizeof(gPvrR32ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrR32ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        case Pvr_PixelTypeChar_R32G32:
        case Pvr_PixelTypeChar_L32A32:
        {
            assert(channelType < (sizeof(gPvrR32G32ToNeutralFormatMap) / sizeof(gPvrR32G32ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrR32G32ToNeutralFormatMap[0]) / sizeof(gPvrR32G32ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrR32G32ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        case Pvr_PixelTypeChar_R32G32B32:
        {
            assert(channelType < (sizeof(gPvrR32G32B32ToNeutralFormatMap) / sizeof(gPvrR32G32B32ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrR32G32B32ToNeutralFormatMap[0]) / sizeof(gPvrR32G32B32ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrR32G32B32ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        case Pvr_PixelTypeChar_R32G32B32A32:
        {
            assert(channelType < (sizeof(gPvrR32G32B32A32ToNeutralFormatMap) / sizeof(gPvrR32G32B32A32ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrR32G32B32A32ToNeutralFormatMap[0]) / sizeof(gPvrR32G32B32A32ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrR32G32B32A32ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        case Pvr_PixelTypeChar_R5G6B5:
        {
            assert(channelType < (sizeof(gPvrR5G6B5ToNeutralFormatMap) / sizeof(gPvrR5G6B5ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrR5G6B5ToNeutralFormatMap[0]) / sizeof(gPvrR5G6B5ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrR5G6B5ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        case Pvr_PixelTypeChar_R4G4B4A4:
        {
            assert(channelType < (sizeof(gPvrR5G6B5ToNeutralFormatMap) / sizeof(gPvrR5G6B5ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrR5G6B5ToNeutralFormatMap[0]) / sizeof(gPvrR5G6B5ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrR5G6B5ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        case Pvr_PixelTypeChar_R5G5B5A1:
        {
            assert(channelType < (sizeof(gPvrR4G4B4A4ToNeutralFormatMap) / sizeof(gPvrR4G4B4A4ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrR4G4B4A4ToNeutralFormatMap[0]) / sizeof(gPvrR4G4B4A4ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrR4G4B4A4ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        case Pvr_PixelTypeChar_R11G11B10:
        {
            assert(channelType < (sizeof(gPvrR11G11B10ToNeutralFormatMap) / sizeof(gPvrR11G11B10ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrR11G11B10ToNeutralFormatMap[0]) / sizeof(gPvrR11G11B10ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrR11G11B10ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        case Pvr_PixelTypeChar_D8:
        {
            assert(channelType < (sizeof(gPvrD8ToNeutralFormatMap) / sizeof(gPvrD8ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrD8ToNeutralFormatMap[0]) / sizeof(gPvrD8ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrD8ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        case Pvr_PixelTypeChar_S8:
        {
            assert(channelType < (sizeof(gPvrS8ToNeutralFormatMap) / sizeof(gPvrS8ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrS8ToNeutralFormatMap[0]) / sizeof(gPvrS8ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrS8ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        case Pvr_PixelTypeChar_D16:
        {
            assert(channelType < (sizeof(gPvrD16ToNeutralFormatMap) / sizeof(gPvrD16ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrD16ToNeutralFormatMap[0]) / sizeof(gPvrD16ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrD16ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        case Pvr_PixelTypeChar_D24:
        {
            assert(channelType < (sizeof(gPvrD32ToNeutralFormatMap) / sizeof(gPvrD32ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrD32ToNeutralFormatMap[0]) / sizeof(gPvrD32ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrD32ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        case Pvr_PixelTypeChar_D32:
        {
            assert(channelType < (sizeof(gPvrD32ToNeutralFormatMap) / sizeof(gPvrD32ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrD32ToNeutralFormatMap[0]) / sizeof(gPvrD32ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrD32ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        case Pvr_PixelTypeChar_D16S8:
        {
            assert(channelType < (sizeof(gPvrD16S8ToNeutralFormatMap) / sizeof(gPvrD16S8ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrD16S8ToNeutralFormatMap[0]) / sizeof(gPvrD16S8ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrD16S8ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        case Pvr_PixelTypeChar_D24S8:
        {
            assert(channelType < (sizeof(gPvrD24S8ToNeutralFormatMap) / sizeof(gPvrD24S8ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrD24S8ToNeutralFormatMap[0]) / sizeof(gPvrD24S8ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrD24S8ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        case Pvr_PixelTypeChar_D32S8:
        {
            assert(channelType < (sizeof(gPvrD32S8ToNeutralFormatMap) / sizeof(gPvrD32S8ToNeutralFormatMap[0])));
            assert(colorSpace < (sizeof(gPvrD32S8ToNeutralFormatMap[0]) / sizeof(gPvrD32S8ToNeutralFormatMap[0][0])));
            neutralFormat = gPvrD32S8ToNeutralFormatMap[channelType][colorSpace];
        }
        break;
        default:
            neutralFormat = TEXTURE_LOADER_TYPE_UNDEFINED;
        }
    }

    return neutralFormat;
}

//--------------------------------------------------------------------------------------
static uint32_t const gPvrCompressedFormatPlaneCountInfoTable[] = {
    1, //Pvr_PixelTypeID_PVRTCI_2bpp_RGB
    1, //Pvr_PixelTypeID_PVRTCI_2bpp_RGBA
    1, //Pvr_PixelTypeID_PVRTCI_4bpp_RGB
    1, //Pvr_PixelTypeID_PVRTCI_4bpp_RGBA
    1, //Pvr_PixelTypeID_PVRTCII_2bpp
    1, //Pvr_PixelTypeID_PVRTCII_4bpp
    1, //Pvr_PixelTypeID_ETC1
    1, //Pvr_PixelTypeID_DXT1
    1, //Pvr_PixelTypeID_DXT2
    1, //Pvr_PixelTypeID_DXT3
    1, //Pvr_PixelTypeID_DXT4
    1, //Pvr_PixelTypeID_DXT5
    1, //Pvr_PixelTypeID_BC4
    1, //Pvr_PixelTypeID_BC5
    1, //Pvr_PixelTypeID_BC6
    1, //Pvr_PixelTypeID_BC7
    1, //Pvr_PixelTypeID_UYVY
    1, //Pvr_PixelTypeID_YUY2
    1, //Pvr_PixelTypeID_BW1bpp
    1, //Pvr_PixelTypeID_SharedExponentR9G9B9E5,
    1, //Pvr_PixelTypeID_RGBG8888
    1, //Pvr_PixelTypeID_GRGB8888
    1, //Pvr_PixelTypeID_ETC2_RGB
    1, //Pvr_PixelTypeID_ETC2_RGBA
    1, //Pvr_PixelTypeID_ETC2_RGB_A1
    1, //Pvr_PixelTypeID_EAC_R11
    1, //Pvr_PixelTypeID_EAC_RG11
    1, //Pvr_PixelTypeID_ASTC_4x4
    1, //Pvr_PixelTypeID_ASTC_5x4
    1, //Pvr_PixelTypeID_ASTC_5x5
    1, //Pvr_PixelTypeID_ASTC_6x5
    1, //Pvr_PixelTypeID_ASTC_6x6
    1, //Pvr_PixelTypeID_ASTC_8x5
    1, //Pvr_PixelTypeID_ASTC_8x6
    1, //Pvr_PixelTypeID_ASTC_8x8
    1, //Pvr_PixelTypeID_ASTC_10x5
    1, //Pvr_PixelTypeID_ASTC_10x6
    1, //Pvr_PixelTypeID_ASTC_10x8
    1, //Pvr_PixelTypeID_ASTC_10x10
    1, //Pvr_PixelTypeID_ASTC_12x10
    1, //Pvr_PixelTypeID_ASTC_12x12
    1, //Pvr_PixelTypeID_ASTC_3x3x3
    1, //Pvr_PixelTypeID_ASTC_4x3x3
    1, //Pvr_PixelTypeID_ASTC_4x4x3
    1, //Pvr_PixelTypeID_ASTC_4x4x4
    1, //Pvr_PixelTypeID_ASTC_5x4x4
    1, //Pvr_PixelTypeID_ASTC_5x5x4
    1, //Pvr_PixelTypeID_ASTC_5x5x5
    1, //Pvr_PixelTypeID_ASTC_6x5x5
    1, //Pvr_PixelTypeID_ASTC_6x6x5
    1  //Pvr_PixelTypeID_ASTC_6x6x6
};
static_assert(Pvr_PixelTypeID_NumCompressedPFs == (sizeof(gPvrCompressedFormatPlaneCountInfoTable) / sizeof(gPvrCompressedFormatPlaneCountInfoTable[0])), "gPvrCompressedFormatPlaneCountInfoTable may not match!");

static inline uint32_t _GetFormatPlaneCount(uint64_t pixelFormat)
{
    uint32_t planeCount;

    if (Pvr_GetPixelFormatPartHigh(pixelFormat) == 0)
    {
        assert(pixelFormat < (sizeof(gPvrCompressedFormatPlaneCountInfoTable) / sizeof(gPvrCompressedFormatPlaneCountInfoTable[0])));
        planeCount = gPvrCompressedFormatPlaneCountInfoTable[pixelFormat];
    }
    else
    {
        switch (pixelFormat)
        {
        case Pvr_PixelTypeChar_I8:
        case Pvr_PixelTypeChar_R8:
        case Pvr_PixelTypeChar_R8G8:
        case Pvr_PixelTypeChar_R8G8B8:
        case Pvr_PixelTypeChar_B8G8R8:
        case Pvr_PixelTypeChar_R8G8B8A8:
        case Pvr_PixelTypeChar_B8G8R8A8:
        case Pvr_PixelTypeChar_A8B8G8R8:
        case Pvr_PixelTypeChar_R16:
        case Pvr_PixelTypeChar_R16G16:
        case Pvr_PixelTypeChar_L16A16:
        case Pvr_PixelTypeChar_R16G16B16:
        case Pvr_PixelTypeChar_R16G16B16A16:
        case Pvr_PixelTypeChar_R32:
        case Pvr_PixelTypeChar_L32:
        case Pvr_PixelTypeChar_R32G32:
        case Pvr_PixelTypeChar_L32A32:
        case Pvr_PixelTypeChar_R32G32B32:
        case Pvr_PixelTypeChar_R32G32B32A32:
        case Pvr_PixelTypeChar_R5G6B5:
        case Pvr_PixelTypeChar_R4G4B4A4:
        case Pvr_PixelTypeChar_R5G5B5A1:
        case Pvr_PixelTypeChar_R11G11B10:
        case Pvr_PixelTypeChar_D8:
        case Pvr_PixelTypeChar_S8:
        case Pvr_PixelTypeChar_D16:
        case Pvr_PixelTypeChar_D24:
        case Pvr_PixelTypeChar_D32:
            planeCount = 1;
            break;
        case Pvr_PixelTypeChar_D16S8:
        case Pvr_PixelTypeChar_D24S8:
        case Pvr_PixelTypeChar_D32S8:
            planeCount = 2;
            break;
        default:
            planeCount = 0;
        }
    }

    return planeCount;
}

//--------------------------------------------------------------------------------------
static inline bool IsDepthStencil(uint64_t pixelFormat)
{
    switch (pixelFormat)
    {
    case Pvr_PixelTypeChar_D32S8:
    case Pvr_PixelTypeChar_D32:
    case Pvr_PixelTypeChar_D24S8:
    case Pvr_PixelTypeChar_D24:
    case Pvr_PixelTypeChar_D16S8:
    case Pvr_PixelTypeChar_D16:
    case Pvr_PixelTypeChar_S8:
    case Pvr_PixelTypeChar_D8:
        return true;
    default:
        return false;
    }
}
