#include "PTGFXAssetDDS.h"

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
)
{
	return NULL;
}

/*

//Align Requirement

// Row-by-row memcpy
inline void MemcpySubresource(
	_In_ const D3D12_MEMCPY_DEST* pDest,
	_In_ const D3D12_SUBRESOURCE_DATA* pSrc,
	SIZE_T RowSizeInBytes,
	UINT NumRows,
	UINT NumSlices)
{
	for (UINT z = 0; z < NumSlices; ++z)
	{
		BYTE* pDestSlice = reinterpret_cast<BYTE*>(pDest->pData) + pDest->SlicePitch * z;
		const BYTE* pSrcSlice = reinterpret_cast<const BYTE*>(pSrc->pData) + pSrc->SlicePitch * z;
		for (UINT y = 0; y < NumRows; ++y)
		{
			memcpy(pDestSlice + pDest->RowPitch * y,
				   pSrcSlice + pSrc->RowPitch * y,
				   RowSizeInBytes);
		}
	}
}
*/