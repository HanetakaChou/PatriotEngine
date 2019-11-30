#include "PTQueueUpload.h"

namespace Patriot
{
	namespace Graphic
	{
		namespace Device
		{
			class PTQueueUploadUMAImpl :public IPTQueueUpload
			{
				void WriteToResource(IPTResource *pDestResource, uint32_t WriteInfoNum, PTWriteToResourceInfo pWriteInfo) override;
			};
		}
	}
}


