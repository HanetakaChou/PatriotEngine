#include "../../../../../Public/Graphic/Device/PTDevice.h"

#include <stdint.h>

namespace Patriot
{
	namespace Graphic
	{
		namespace Device
		{
			struct PTWriteToResourceInfo
			{
				//--------------------
				uint32_t DestPlaneSlice;
				uint32_t DestArraySize;
				uint32_t DestArraySlice;
				uint32_t DestMipSize;
				uint32_t DestMipSlice;
				//--------------------
				const void *pSrcData;
				uint32_t SrcRowPitch;
				uint32_t SrcRowNum;
				uint32_t SrcDepthPitch;
				uint32_t SrcDepthNum;
			};

			struct IPTQueueUpload
			{
				virtual void WriteToResource(IPTResource *pDestResource, uint32_t WriteInfoNum, PTWriteToResourceInfo pWriteInfo) = 0;
			};

		}
	}
}