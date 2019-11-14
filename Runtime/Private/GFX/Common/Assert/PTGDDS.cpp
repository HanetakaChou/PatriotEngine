#include "PTGDDS.h"

#include <stdlib.h>
#include <assert.h>

#include "PTGEndian.h"

#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d12.h>
#include <d3d11.h>

void DDSParseHeader(const void *pHeader, D3D12_RESOURCE_DESC *pDesc, const void **ppData);

void LoadDDS(const void *pDDS)
{
	const uint32_t *pMagic = static_cast<const uint32_t *>(pDDS);
	assert(*pMagic == DDS_MAGIC);
	//D3D11CalcSubresource
	const void *pData;
	D3D12_RESOURCE_DESC Desc;
	DDSParseHeader(reinterpret_cast<const void *>(reinterpret_cast<uintptr_t>(pDDS) + sizeof(uint32_t)), &Desc, &pData);
	int huhu = 0;
}

void DDSParseHeader(const void *pHeader, D3D12_RESOURCE_DESC *pD3D12ResourceDesc ,const void **ppData)
{
	const DDS_HEADER *pDDSHeader = static_cast<const DDS_HEADER *>(pHeader);

	if (pDDSHeader->ddspf.dwFourCC == FOURCC_DX10)
	{
		const DDS_HEADER_DXT10 *pDDSHeaderDXT10 = reinterpret_cast<const DDS_HEADER_DXT10 *>(reinterpret_cast<uintptr_t>(pHeader) + 124U);//sizeof(DDS_HEADER)
		*ppData = reinterpret_cast<const void *>(reinterpret_cast<uintptr_t>(pDDSHeaderDXT10) + 20U);//sizeof(DDS_HEADER_DXT10)
		
		//Format
		switch (pDDSHeaderDXT10->Format)
		{
		case DXT10_FORMAT_D32_FLOAT:
		case DXT10_FORMAT_R32_FLOAT:
			pD3D12ResourceDesc->Format = DXGI_FORMAT_R32_FLOAT;
			break;
		case DXT10_FORMAT_R32G32B32A32_FLOAT:
			pD3D12ResourceDesc->Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;
		}

		//Dimension
		if (pDDSHeaderDXT10->Dimension == DXT10_DIMENSION_TEXTURE2D)
		{
			pD3D12ResourceDesc->Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			if (pDDSHeaderDXT10->MiscFlags&DXT10_RESOURCE_MISC_TEXTURECUBE)
			{
				pD3D12ResourceDesc->DepthOrArraySize = pDDSHeaderDXT10->ArraySize * 6U;
			}
			else
			{
				pD3D12ResourceDesc->DepthOrArraySize = pDDSHeaderDXT10->ArraySize;
			}
		}
		else if (pDDSHeaderDXT10->Dimension == DXT10_DIMENSION_TEXTURE3D)
		{
			pD3D12ResourceDesc->Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
			assert(pDDSHeader->dwFlags & DDSD_DEPTH);
			pD3D12ResourceDesc->DepthOrArraySize = pDDSHeader->dwDepth;
		}
		else
		{
			assert(0);
		}

	}
	else
	{
		*ppData = reinterpret_cast<const void *>(reinterpret_cast<uintptr_t>(pHeader) + 124U);//sizeof(DDS_HEADER)
		
																							  //Format
		if (pDDSHeader->ddspf.dwFlags & DDPF_FOURCC)
		{
			switch (pDDSHeader->ddspf.dwFourCC)
			{
			case FOURCC_DXT1:
				pD3D12ResourceDesc->Format = DXGI_FORMAT_BC1_UNORM;
				break;
			case FOURCC_DXT2:
			case FOURCC_DXT3:
				pD3D12ResourceDesc->Format = DXGI_FORMAT_BC2_UNORM;
				break;
			case FOURCC_DXT4:
			case FOURCC_DXT5:
				pD3D12ResourceDesc->Format = DXGI_FORMAT_BC3_UNORM;
				break;
			case FOURCC_FMTR16F:
				pD3D12ResourceDesc->Format = DXGI_FORMAT_R16_FLOAT;
				break;
			case FOURCC_FMTG16R16F:
				pD3D12ResourceDesc->Format = DXGI_FORMAT_R16G16_FLOAT;
				break;
			case FOURCC_FMTA16B16G16R16F:
				pD3D12ResourceDesc->Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
				break;
			case FOURCC_FMTR32F:
				pD3D12ResourceDesc->Format = DXGI_FORMAT_R32_FLOAT;
				break;
			case FOURCC_FMTG32R32F:
				pD3D12ResourceDesc->Format = DXGI_FORMAT_R32G32_FLOAT;
				break;
			case FOURCC_FMTA32B32G32R32F:
				pD3D12ResourceDesc->Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				break;
			default:
				pD3D12ResourceDesc->Format = DXGI_FORMAT_UNKNOWN;
				break;
			}
		}
		else
		{
			if (pDDSHeader->ddspf.dwRBitMask == 0X000000FFU && pDDSHeader->ddspf.dwGBitMask == 0X0000FF00U && pDDSHeader->ddspf.dwBBitMask == 0X00FF0000U && pDDSHeader->ddspf.dwABitMask == 0XFF000000U)
			{
				//D3DFMT_A8R8G8B8
				pD3D12ResourceDesc->Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			}
			else if (pDDSHeader->ddspf.dwRBitMask == 0X000000FFU && pDDSHeader->ddspf.dwGBitMask == 0X0000FF00U && pDDSHeader->ddspf.dwBBitMask == 0X00FF0000U && pDDSHeader->ddspf.dwABitMask == 0X00000000U)
			{
				//D3DFMT_X8R8G8B8
				pD3D12ResourceDesc->Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			}
			else if (pDDSHeader->ddspf.dwRBitMask == 0X00FF0000U && pDDSHeader->ddspf.dwGBitMask == 0X0000FF00U && pDDSHeader->ddspf.dwBBitMask == 0X000000FFU && pDDSHeader->ddspf.dwABitMask == 0XFF000000U)
			{
				//D3DFMT_A8B8G8R8
				pD3D12ResourceDesc->Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			}
			else if (pDDSHeader->ddspf.dwRBitMask == 0X00FF0000U && pDDSHeader->ddspf.dwGBitMask == 0X0000FF00U && pDDSHeader->ddspf.dwBBitMask == 0X000000FFU && pDDSHeader->ddspf.dwABitMask == 0X00000000U)
			{
				//D3DFMT_X8B8G8R8
				pD3D12ResourceDesc->Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			}
			else if (pDDSHeader->ddspf.dwRBitMask == 0X000003FFU && pDDSHeader->ddspf.dwGBitMask == 0x000FFC00U && pDDSHeader->ddspf.dwBBitMask == 0x3FF00000U && pDDSHeader->ddspf.dwABitMask == 0XC0000000U)
			{
				//D3DFMT_A2R10G10B10
				pD3D12ResourceDesc->Format = DXGI_FORMAT_R10G10B10A2_UNORM;
			}
			else if (pDDSHeader->ddspf.dwRBitMask == 0x3FF00000U && pDDSHeader->ddspf.dwGBitMask == 0x000FFC00U && pDDSHeader->ddspf.dwBBitMask == 0X000003FFU && pDDSHeader->ddspf.dwABitMask == 0XC0000000U)
			{
				//D3DFMT_A2B10G10R10
				pD3D12ResourceDesc->Format = DXGI_FORMAT_R10G10B10A2_UNORM;
			}
			else if (pDDSHeader->ddspf.dwRBitMask == 0X0000FFFFU && pDDSHeader->ddspf.dwGBitMask == 0XFFFF0000U && pDDSHeader->ddspf.dwBBitMask == 0X00000000U && pDDSHeader->ddspf.dwABitMask == 0X00000000U)
			{
				//D3DFMT_G16R16
				pD3D12ResourceDesc->Format = DXGI_FORMAT_R16G16_UNORM;
			}
			else if (pDDSHeader->ddspf.dwRBitMask == 0XFFFFFFFFU && pDDSHeader->ddspf.dwGBitMask == 0X00000000U && pDDSHeader->ddspf.dwBBitMask == 0X00000000U && pDDSHeader->ddspf.dwABitMask == 0X00000000U)
			{
				//D3DFMT_R32F
				pD3D12ResourceDesc->Format = DXGI_FORMAT_R32_FLOAT;
			}
			else if (pDDSHeader->ddspf.dwRBitMask == 0X0000FFFFU && pDDSHeader->ddspf.dwGBitMask == 0X00000000U && pDDSHeader->ddspf.dwBBitMask == 0X00000000U && pDDSHeader->ddspf.dwABitMask == 0X00000000U)
			{
				//D3DFMT_L16
				pD3D12ResourceDesc->Format = DXGI_FORMAT_R16_UNORM;
			}
			else if (pDDSHeader->ddspf.dwRBitMask == 0X000000FFU && pDDSHeader->ddspf.dwGBitMask == 0X00000000U && pDDSHeader->ddspf.dwBBitMask == 0X00000000U && pDDSHeader->ddspf.dwABitMask == 0X00000000U)
			{
				//D3DFMT_L8
				pD3D12ResourceDesc->Format = DXGI_FORMAT_R8_UNORM;
			}
			else if (pDDSHeader->ddspf.dwRBitMask == 0X00000000U && pDDSHeader->ddspf.dwGBitMask == 0X00000000U && pDDSHeader->ddspf.dwBBitMask == 0X00000000U && pDDSHeader->ddspf.dwABitMask == 0X000000FFU)
			{
				//D3DFMT_A8
				pD3D12ResourceDesc->Format = DXGI_FORMAT_R8_UNORM;
			}
			else
			{
				pD3D12ResourceDesc->Format = DXGI_FORMAT_UNKNOWN;
			}
		}
		
		//Dimension
		if ((pDDSHeader->dwFlags & DDSD_CAPS) && (pDDSHeader->dwCaps2&DDSCAPS2_CUBEMAP))
		{
			pD3D12ResourceDesc->Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			pD3D12ResourceDesc->DepthOrArraySize = 6U;
		}
		else if(pDDSHeader->dwFlags & DDSD_DEPTH)
		{
			pD3D12ResourceDesc->Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
			pD3D12ResourceDesc->DepthOrArraySize = pDDSHeader->dwDepth;
		}
		else
		{
			pD3D12ResourceDesc->Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			pD3D12ResourceDesc->DepthOrArraySize = 1U;
		}
	}

	//MipLevels
	if (pDDSHeader->dwFlags&DDSD_MIPMAPCOUNT)
	{
		pD3D12ResourceDesc->MipLevels = pDDSHeader->dwMipMapCount;
	}
	else
	{
		pD3D12ResourceDesc->MipLevels = 1U;
	}

	pD3D12ResourceDesc->Width = pDDSHeader->dwWidth;
	pD3D12ResourceDesc->Height = pDDSHeader->dwHeight;

	pD3D12ResourceDesc->Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	pD3D12ResourceDesc->SampleDesc.Count = 1U;
	pD3D12ResourceDesc->SampleDesc.Quality = 0U;
	pD3D12ResourceDesc->Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	pD3D12ResourceDesc->Flags = D3D12_RESOURCE_FLAG_NONE;
}