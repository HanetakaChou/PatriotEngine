#ifndef PT_GFX_ASSETDDS_H
#define PT_GFX_ASSETDDS_H 1


//Microsoft DirectXTex
//https://github.com/microsoft/DirectXTex

//Block Compression
//https://github.com/MicrosoftDocs/win32/blob/docs/desktop-src/direct3d11/texture-block-compression-in-direct3d-11.md
//| Source data               | Recommended format | D3DFORMAT                 | DXGI_FORMAT                | VkFormat                       | MTLPixelFormat                   |
//|---------------------------| ------------------ |---------------------------| ---------------------------|--------------------------------|----------------------------------|
//| R5 G6 B5 A0-1 UNORM       | BC1                | D3DFMT_DXT1               | DXGI_FORMAT_BC1_UNORM      | VK_FORMAT_BC1_RGBA_UNORM_BLOCK | MTLPixelFormatBC1_RGBA           |
//|                           |                    |                           | DXGI_FORMAT_BC1_UNORM_SRGB | VK_FORMAT_BC1_RGBA_SRGB_BLOCK  | MTLPixelFormatBC1_RGBA_sRGB      |
//| R5 G6 B5 A4 UNORM         | BC2                | D3DFMT_DXT3 / D3DFMT_DXT2 | DXGI_FORMAT_BC2_UNORM      | VK_FORMAT_BC2_UNORM_BLOCK      | MTLPixelFormatBC2_RGBA           |
//|                           |                    |                           | DXGI_FORMAT_BC2_UNORM_SRGB | VK_FORMAT_BC2_SRGB_BLOCK       | MTLPixelFormatBC2_RGBA_sRGB      |
//| R5 G6 B5 A8 UNORM         | BC3                | D3DFMT_DXT5 / D3DFMT_DXT4 | DXGI_FORMAT_BC3_UNORM      | VK_FORMAT_BC3_UNORM_BLOCK      | MTLPixelFormatBC3_RGBA           |
//|                           |                    |                           | DXGI_FORMAT_BC3_UNORM_SRGB | VK_FORMAT_BC3_SRGB_BLOCK       | MTLPixelFormatBC3_RGBA_sRGB      |
//| R8 UNORM                  | BC4                | FOURCC_ATI1               | DXGI_FORMAT_BC4_UNORM      | VK_FORMAT_BC4_UNORM_BLOCK      | MTLPixelFormatBC4_RUnorm         |
//| R8 SNORM                  |                    |                           | DXGI_FORMAT_BC4_SNORM      | VK_FORMAT_BC4_SNORM_BLOCK      | MTLPixelFormatBC4_RSnorm         |
//| R8 G8 UNORM               | BC5                | FOURCC_ATI2               | DXGI_FORMAT_BC5_UNORM      | VK_FORMAT_BC5_UNORM_BLOCK      | MTLPixelFormatBC5_RGUnorm        |
//| R8 G8 SNORM               |                    |                           | DXGI_FORMAT_BC5_SNORM      | VK_FORMAT_BC5_SNORM_BLOCK      | MTLPixelFormatBC5_RGSnorm        |
//| R16 G16 B16 UFLOAT (HDR)  | BC6H               |                           | DXGI_FORMAT_BC6H_UF16      | VK_FORMAT_BC6H_UFLOAT_BLOCK    | MTLPixelFormatBC6H_RGBUfloat     |
//| R16 G16 B16 SFLOAT (HDR)  |                    |                           | DXGI_FORMAT_BC6H_SF16      | VK_FORMAT_BC6H_SFLOAT_BLOCK    | MTLPixelFormatBC6H_RGBFloat      |
//| B4-7 G4-7 R4-7 A0-8 UNORM | BC7                |                           | DXGI_FORMAT_BC7_UNORM      | VK_FORMAT_BC7_UNORM_BLOCK      | MTLPixelFormatBC7_RGBAUnorm      |
//|                           |                    |                           | DXGI_FORMAT_BC7_UNORM_SRGB | VK_FORMAT_BC7_SRGB_BLOCK       | MTLPixelFormatBC7_RGBAUnorm_sRGB |

#include "../../Public/PTCommon.h"
#include <stddef.h>
#include <stdint.h>

typedef enum DDS_FORMAT
{
	DDS_FORMAT_BC1_UNORM = 71,
	DDS_FORMAT_BC1_UNORM_SRGB = 72,
	DDS_FORMAT_BC2_UNORM = 74,
	DDS_FORMAT_BC2_UNORM_SRGB = 75,
	DDS_FORMAT_BC3_UNORM = 77,
	DDS_FORMAT_BC3_UNORM_SRGB = 78,
	DDS_FORMAT_BC4_UNORM = 80,
	DDS_FORMAT_BC4_SNORM = 81,
	DDS_FORMAT_BC5_UNORM = 83,
	DDS_FORMAT_BC5_SNORM = 84,
	DDS_FORMAT_BC6H_UF16 = 95,
	DDS_FORMAT_BC6H_SF16 = 96,
	DDS_FORMAT_BC7_UNORM = 98,
	DDS_FORMAT_BC7_UNORM_SRGB = 99
} DDS_FORMAT;

bool PT_GFX_Asset_LoadDDS(
	void *assetData,
	size_t assetData_Maxsize,
	void *textureDesc_pUserData,
	void(*textureDesc_fill)(
		void *textureDesc_pUserData,
		DDS_FORMAT textureDesc_PixelFormat,
		uint32_t textureDesc_Width, uint32_t textureDesc_Height, uint32_t textureDesc_Depth,
		uint32_t textureDesc_MipmapLevelCount, uint32_t textureDesc_ArrayLength
		),
	void *assetSubresourceData_pUserData,
	void(*assetSubresourceData_emplace_back)(
		void *assetSubresourceData_pUserData,
		uint64_t assetSubresourceData_Offset, uint32_t assetSubresourceData_BytesPerRow, uint32_t assetSubresourceData_BytesPerImage
		)
);

#endif
