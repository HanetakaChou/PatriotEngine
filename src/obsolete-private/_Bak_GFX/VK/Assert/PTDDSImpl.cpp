#include "PTDDS.h"
#include "PTDDSImpl.h"

#include <stdlib.h>
#include <assert.h>
#if PTARM || PTARM64
#define PTLE32TOH(x) (_byteswap_ulong(x))
#elif PTX86 || PTX86_64
#define PTLE32TOH(x) (x)
#else
#error δ֪��ƽ̨
#endif

#include "../../../../ThirdParty/vulkan/Include/vulkan/vulkan.h"

void const *DDSParseHeader(void const *pSrcDataCurrent,  VkImageCreateInfo *pImageInfo);

void Patriot::Graphic::Assert::DDS::Load(const void *pSrcData)
{
	const void *pSrcDataCurrent = pSrcData;
	
	//Magic
	assert(
		PTLE32TOH(*static_cast<uint32_t const *>(pSrcDataCurrent))
		== 
		DDS_MAGIC
	);
	pSrcDataCurrent = static_cast<uint8_t const *>(pSrcDataCurrent) + 4U;//sizeof(uint32_t)

	//Header
	VkImageCreateInfo ImageInfo;
	pSrcDataCurrent = DDSParseHeader(pSrcDataCurrent, &ImageInfo);
	
	//UMA
	//ID3D12Resource::WriteToSubresource
	//vkGetImageSubresourceLayout
	int huhu = 0;
}

void const *DDSParseHeader(void const *pSrcDataCurrent, VkImageCreateInfo *pImageInfo)
{
	DDS_HEADER const *pDDSHeader = static_cast<DDS_HEADER const *>(pSrcDataCurrent);
	pSrcDataCurrent = static_cast<uint8_t const *>(pSrcDataCurrent) + 124U;//sizeof(DDS_HEADER)

	if (pDDSHeader->ddspf.dwFourCC == FOURCC_DX10)
	{
		DDS_HEADER_DXT10 const *pDDSHeaderDXT10 = static_cast<DDS_HEADER_DXT10 const *>(pSrcDataCurrent);
		pSrcDataCurrent = static_cast<uint8_t const*>(pSrcDataCurrent) + 20U;//sizeof(DDS_HEADER_DXT10)
		
		//Format
		switch (pDDSHeaderDXT10->Format)
		{
		case DXT10_FORMAT_D32_FLOAT:
		case DXT10_FORMAT_R32_FLOAT:
			pImageInfo->format = VK_FORMAT_R32_SFLOAT;
			break;
		case DXT10_FORMAT_R32G32B32A32_FLOAT:
			pImageInfo->format = VK_FORMAT_R32G32B32A32_SFLOAT;
			break;
		default:
			assert(0);
		}

		//Dimension
		if (pDDSHeaderDXT10->Dimension == DXT10_DIMENSION_TEXTURE2D)
		{
			pImageInfo->imageType = VK_IMAGE_TYPE_2D;
			pImageInfo->extent.depth = 1U;
			if (pDDSHeaderDXT10->MiscFlags&DXT10_RESOURCE_MISC_TEXTURECUBE)
			{
				pImageInfo->array_layers = pDDSHeaderDXT10->ArraySize * 6U;
			}
			else
			{
				pImageInfo->array_layers = pDDSHeaderDXT10->ArraySize;
			}
		}
		else if (pDDSHeaderDXT10->Dimension == DXT10_DIMENSION_TEXTURE3D)
		{
			pImageInfo->imageType = VK_IMAGE_TYPE_3D;
			pImageInfo->array_layers = 1U;
			assert(pDDSHeader->dwFlags & DDSD_DEPTH);
			pImageInfo->extent.depth = pDDSHeader->dwDepth;
		}
		else
		{
			assert(0);
		}

	}
	else
	{
		
		//Format
		if (pDDSHeader->ddspf.dwFlags & DDPF_FOURCC)
		{
			switch (pDDSHeader->ddspf.dwFourCC)
			{
			case FOURCC_DXT1:
				pImageInfo->format = VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
				break;
			case FOURCC_DXT2:
			case FOURCC_DXT3:
				pImageInfo->format = VK_FORMAT_BC2_UNORM_BLOCK;
				break;
			case FOURCC_DXT4:
			case FOURCC_DXT5:
				pImageInfo->format = VK_FORMAT_BC3_UNORM_BLOCK;
				break;
			case FOURCC_FMTR16F:
				pImageInfo->format = VK_FORMAT_R16_SFLOAT;
				break;
			case FOURCC_FMTG16R16F:
				pImageInfo->format = VK_FORMAT_R16G16_SFLOAT;
				break;
			case FOURCC_FMTA16B16G16R16F:
				pImageInfo->format = VK_FORMAT_R16G16B16A16_SFLOAT;
				break;
			case FOURCC_FMTR32F:
				pImageInfo->format = VK_FORMAT_R32_SFLOAT;
				break;
			case FOURCC_FMTG32R32F:
				pImageInfo->format = VK_FORMAT_R32G32_SFLOAT;
				break;
			case FOURCC_FMTA32B32G32R32F:
				pImageInfo->format = VK_FORMAT_R32G32B32A32_SFLOAT;
				break;
			default:
				assert(0);
			}
		}
		else
		{
			if (pDDSHeader->ddspf.dwRBitMask == 0X000000FFU && pDDSHeader->ddspf.dwGBitMask == 0X0000FF00U && pDDSHeader->ddspf.dwBBitMask == 0X00FF0000U && pDDSHeader->ddspf.dwABitMask == 0XFF000000U)
			{
				//D3DFMT_A8R8G8B8
				pImageInfo->format = VK_FORMAT_R8G8B8A8_UNORM;
			}
			else if (pDDSHeader->ddspf.dwRBitMask == 0X000000FFU && pDDSHeader->ddspf.dwGBitMask == 0X0000FF00U && pDDSHeader->ddspf.dwBBitMask == 0X00FF0000U && pDDSHeader->ddspf.dwABitMask == 0X00000000U)
			{
				//D3DFMT_X8R8G8B8
				pImageInfo->format = VK_FORMAT_R8G8B8A8_UNORM;
			}
			else if (pDDSHeader->ddspf.dwRBitMask == 0X00FF0000U && pDDSHeader->ddspf.dwGBitMask == 0X0000FF00U && pDDSHeader->ddspf.dwBBitMask == 0X000000FFU && pDDSHeader->ddspf.dwABitMask == 0XFF000000U)
			{
				//D3DFMT_A8B8G8R8
				pImageInfo->format = VK_FORMAT_B8G8R8A8_UNORM;
			}
			else if (pDDSHeader->ddspf.dwRBitMask == 0X00FF0000U && pDDSHeader->ddspf.dwGBitMask == 0X0000FF00U && pDDSHeader->ddspf.dwBBitMask == 0X000000FFU && pDDSHeader->ddspf.dwABitMask == 0X00000000U)
			{
				//D3DFMT_X8B8G8R8
				pImageInfo->format = VK_FORMAT_B8G8R8A8_UNORM;
			}
			else if (pDDSHeader->ddspf.dwRBitMask == 0X000003FFU && pDDSHeader->ddspf.dwGBitMask == 0x000FFC00U && pDDSHeader->ddspf.dwBBitMask == 0x3FF00000U && pDDSHeader->ddspf.dwABitMask == 0XC0000000U)
			{
				//D3DFMT_A2R10G10B10
				pImageInfo->format = VK_FORMAT_A2R10G10B10_UNORM_PACK32;
			}
			else if (pDDSHeader->ddspf.dwRBitMask == 0x3FF00000U && pDDSHeader->ddspf.dwGBitMask == 0x000FFC00U && pDDSHeader->ddspf.dwBBitMask == 0X000003FFU && pDDSHeader->ddspf.dwABitMask == 0XC0000000U)
			{
				//D3DFMT_A2B10G10R10
				pImageInfo->format = VK_FORMAT_A2B10G10R10_UNORM_PACK32;
			}
			else if (pDDSHeader->ddspf.dwRBitMask == 0X0000FFFFU && pDDSHeader->ddspf.dwGBitMask == 0XFFFF0000U && pDDSHeader->ddspf.dwBBitMask == 0X00000000U && pDDSHeader->ddspf.dwABitMask == 0X00000000U)
			{
				//D3DFMT_G16R16
				pImageInfo->format = VK_FORMAT_R16G16_UNORM;
			}
			else if (pDDSHeader->ddspf.dwRBitMask == 0XFFFFFFFFU && pDDSHeader->ddspf.dwGBitMask == 0X00000000U && pDDSHeader->ddspf.dwBBitMask == 0X00000000U && pDDSHeader->ddspf.dwABitMask == 0X00000000U)
			{
				//D3DFMT_R32F
				pImageInfo->format = VK_FORMAT_R32_SFLOAT;
			}
			else if (pDDSHeader->ddspf.dwRBitMask == 0X0000FFFFU && pDDSHeader->ddspf.dwGBitMask == 0X00000000U && pDDSHeader->ddspf.dwBBitMask == 0X00000000U && pDDSHeader->ddspf.dwABitMask == 0X00000000U)
			{
				//D3DFMT_L16
				pImageInfo->format = VK_FORMAT_R16_UNORM;
			}
			else if (pDDSHeader->ddspf.dwRBitMask == 0X000000FFU && pDDSHeader->ddspf.dwGBitMask == 0X00000000U && pDDSHeader->ddspf.dwBBitMask == 0X00000000U && pDDSHeader->ddspf.dwABitMask == 0X00000000U)
			{
				//D3DFMT_L8
				pImageInfo->format = VK_FORMAT_R8_UNORM;
			}
			else if (pDDSHeader->ddspf.dwRBitMask == 0X00000000U && pDDSHeader->ddspf.dwGBitMask == 0X00000000U && pDDSHeader->ddspf.dwBBitMask == 0X00000000U && pDDSHeader->ddspf.dwABitMask == 0X000000FFU)
			{
				//D3DFMT_A8
				pImageInfo->format = VK_FORMAT_R8_UNORM;
			}
			else
			{
				assert(0);
			}
		}

		//Dimension
		if ((pDDSHeader->dwFlags & DDSD_CAPS) && (pDDSHeader->dwCaps2&DDSCAPS2_CUBEMAP))
		{
			pImageInfo->imageType = VK_IMAGE_TYPE_2D;
			pImageInfo->extent.depth = 1U;
			pImageInfo->array_layers = 6U;
		}
		else if(pDDSHeader->dwFlags & DDSD_DEPTH)
		{
			pImageInfo->imageType = VK_IMAGE_TYPE_3D;
			pImageInfo->extent.depth = pDDSHeader->dwDepth;
			pImageInfo->array_layers = 1U;
		}
		else
		{
			pImageInfo->imageType = VK_IMAGE_TYPE_2D;
			pImageInfo->extent.depth = 1U;
			pImageInfo->array_layers = 1U;
		}
	}

	//MipLevels
	if (pDDSHeader->dwFlags&DDSD_MIPMAPCOUNT)
	{
		pImageInfo->mip_levels = pDDSHeader->dwMipMapCount;
	}
	else
	{
		pImageInfo->mip_levels = 1U;
	}

	pImageInfo->extent.width = pDDSHeader->dwWidth;
	pImageInfo->extent.height = pDDSHeader->dwHeight;


	//Vulkan
	pImageInfo->sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	pImageInfo->pNext = NULL;
	pImageInfo->flags = 0U;
	pImageInfo->samples = VK_SAMPLE_COUNT_1_BIT;
	pImageInfo->tiling = VK_IMAGE_TILING_OPTIMAL;
	pImageInfo->usage = VK_IMAGE_USAGE_SAMPLED_BIT;

	//VkMemoryAllocateInfo

	//pD3D12ResourceDesc->Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	//pD3D12ResourceDesc->Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	//pD3D12ResourceDesc->Flags = D3D12_RESOURCE_FLAG_NONE;

	return pSrcDataCurrent;
}