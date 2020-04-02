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
    Pvr_HeaderSizeV1 = 44, // Header sizes, used as identifiers for previous versions of the header.
    Pvr_HeaderSizeV2 = 52, // Header sizes, used as identifiers for previous versions of the header.
    Pvr_HeaderVersionV3 = Pvr_MakeFourCC('P', 'V', 'R', 3)
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
    uint32_t pvrMagic;            /*!< magic number identifying pvr file */
    uint32_t numSurfaces;         /*!< the number of surfaces present in the pvr */
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
    Pvr_PixelTypeID_BC1 = 7,
    Pvr_PixelTypeID_BC2 = 9,
    Pvr_PixelTypeID_BC3 = 11,

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
    Pvr_PixelTypeID_ASTC_6x6x6 = 50
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
    Pvr_PixelTypeChar_R8G8B8A8 = Pvr_MakePixelTypeChar('r', 'g', 'b', 'a', 8, 8, 8, 8),
    Pvr_PixelTypeChar_A8B8G8R8 = Pvr_MakePixelTypeChar('a', 'b', 'g', 'r', 8, 8, 8, 8),
    Pvr_PixelTypeChar_R16 = Pvr_MakePixelTypeChar('r', '\0', '\0', '\0', 16, 0, 0, 0),
    Pvr_PixelTypeChar_R16G16 = Pvr_MakePixelTypeChar('r', 'g', '\0', '\0', 16, 16, 0, 0),
    Pvr_PixelTypeChar_R32 = Pvr_MakePixelTypeChar('r', '\0', '\0', '\0', 32, 0, 0, 0),
    Pvr_PixelTypeChar_R32G32 = Pvr_MakePixelTypeChar('r', 'g', '\0', '\0', 32, 32, 0, 0),
    Pvr_PixelTypeChar_R32G32B32 = Pvr_MakePixelTypeChar('r', 'g', 'b', '\0', 32, 32, 32, 0),
    Pvr_PixelTypeChar_R32G32B32A32 = Pvr_MakePixelTypeChar('r', 'g', 'b', 'a', 32, 32, 32, 32),
    Pvr_PixelTypeChar_R5G6B5 = Pvr_MakePixelTypeChar('r', 'g', 'b', '\0', 5, 6, 5, 0),
    Pvr_PixelTypeChar_R4G4B4A4 = Pvr_MakePixelTypeChar('r', 'g', 'b', 'a', 4, 4, 4, 4),
    Pvr_PixelTypeChar_R5G5B5A1 = Pvr_MakePixelTypeChar('r', 'g', 'b', 'a', 5, 5, 5, 1),
    Pvr_PixelTypeChar_B8G8R8 = Pvr_MakePixelTypeChar('b', 'g', 'r', '\0', 8, 8, 8, 0),
    Pvr_PixelTypeChar_B8G8R8A8 = Pvr_MakePixelTypeChar('b', 'g', 'r', 'a', 8, 8, 8, 8),
    Pvr_PixelTypeChar_D8 = Pvr_MakePixelTypeChar('d', '\0', '\0', '\0', 8, 0, 0, 0),
    Pvr_PixelTypeChar_D16 = Pvr_MakePixelTypeChar('d', '\0', '\0', '\0', 16, 0, 0, 0),
    Pvr_PixelTypeChar_D24 = Pvr_MakePixelTypeChar('d', '\0', '\0', '\0', 24, 0, 0, 0),
    Pvr_PixelTypeChar_D32 = Pvr_MakePixelTypeChar('d', '\0', '\0', '\0', 32, 0, 0, 0),
    Pvr_PixelTypeChar_D16S8 = Pvr_MakePixelTypeChar('d', 's', '\0', '\0', 16, 8, 0, 0),
    Pvr_PixelTypeChar_D24S8 = Pvr_MakePixelTypeChar('d', 's', '\0', '\0', 24, 8, 0, 0),
    Pvr_PixelTypeChar_D32S8 = Pvr_MakePixelTypeChar('d', 's', '\0', '\0', 32, 8, 0, 0),
    Pvr_PixelTypeChar_S8 = Pvr_MakePixelTypeChar('s', '\0', '\0', '\0', 8, 0, 0, 0),
    Pvr_PixelTypeChar_L32 = Pvr_MakePixelTypeChar('l', '\0', '\0', '\0', 32, 0, 0, 0),
    Pvr_PixelTypeChar_L16A16 = Pvr_MakePixelTypeChar('l', 'a', '\0', '\0', 16, 16, 0, 0),
    Pvr_PixelTypeChar_L32A32 = Pvr_MakePixelTypeChar('l', 'a', '\0', '\0', 32, 32, 0, 0),
    Pvr_PixelTypeChar_R16G16B16A16 = Pvr_MakePixelTypeChar('r', 'g', 'b', 'a', 16, 16, 16, 16),
    Pvr_PixelTypeChar_R16G16B16 = Pvr_MakePixelTypeChar('r', 'g', 'b', '\0', 16, 16, 16, 0),
    Pvr_PixelTypeChar_R11G11B10 = Pvr_MakePixelTypeChar('b', 'g', 'r', '\0', 10, 11, 11, 0)
};

enum
{
    Pvr_ColorSpace_lRGB = 0,
    Pvr_ColorSpace_sRGB = 1
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
    bool _pvr3_0;
    uint8_t textureAtlasCount;
    struct
    {
        uint32_t XPosition;
        uint32_t YPosition;
        uint32_t Width;
        uint32_t Height;
    } textureAtlasCoords[255];

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
};

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

    auto huhu = Pvr_PixelTypeChar_R8G8B8A8;

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

        texture_header->_pvr3_0 = false;
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
                    if (metadata._dataSize <= sizeof(texture_header->textureAtlasCoords))
                    {
                        assert(0 == (metadata._dataSize % sizeof(texture_header->textureAtlasCoords[0])));
                        ptrdiff_t BytesRead = stream_read(stream, &texture_header->textureAtlasCoords, metadata._dataSize);
                        if (BytesRead == -1 || BytesRead < metadata._dataSize)
                        {
                            return false;
                        }
                        texture_header->textureAtlasCount = metadata._dataSize / sizeof(texture_header->textureAtlasCoords[0]);
                        texture_header->_pvr3_0 = true;
                        metaDataRead += metadata._dataSize;
                    }
                    else
                    {
                        return false;
                    }
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
    }

    return true;
}

//--------------------------------------------------------------------------------------
bool PVRTextureLoader_LoadHeaderFromStream(void const *stream, ptrdiff_t (*stream_read)(void const *stream, void *buf, size_t count), int64_t (*stream_seek)(void const *stream, int64_t offset, int whence),
                                           struct TextureLoader_NeutralHeader *neutral_texture_header, size_t *neutral_header_offset)
{

    struct TextureLoader_PVRHeader pvr_texture_header;
    size_t pvr_texture_data_offset;
    if (LoadTextureHeaderFromStream(stream, stream_read, stream_seek, &pvr_texture_header, &pvr_texture_data_offset))
    {
    }

    return true;
}
