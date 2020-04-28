
#include "../../../../Public/Graphic/Render/PTLightAmbient.h"

class PTLightAmbientImpl :IPTGraphicLightAmbient
{
	PTGraphicLightAmbient_Status Render(IPTGraphicDevice *pDevice, PTGraphicLightAmbient_Iutput *pInput, PTGraphicLightAmbient_Output *pOutput) override;
};
