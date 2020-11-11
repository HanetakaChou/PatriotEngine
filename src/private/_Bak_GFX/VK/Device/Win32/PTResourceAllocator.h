#include "../../../../../Public/Graphic/Device/PTDevice.h"

namespace Patriot
{
	namespace Graphic
	{
		namespace Device
		{
			struct IPTResourceAllocator
			{
				virtual IPTResource *CreateTexture2D(uint32_t WriteInfoNum, PTWriteToResourceInfo pWriteInfo) = 0;
			};
		}
	}
}