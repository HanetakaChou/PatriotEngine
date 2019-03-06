#include "../../../Public/Audio/PTADInstance.h"

#include "../../../ThirdParty/TinyALSA/Include/tinyalsa/asoundlib.h"
#include <stddef.h>
#include <stdio.h>


void * PTCALL PTADInstance_ForProcess()
{
	struct pcm_params *params = ::pcm_params_get(0, 0, PCM_OUT);
	//pcm_mask *m = pcm_params_get_mask(params, PCM_PARAM_ACCESS);

	return NULL;
}