#include "../../../Public/Audio/PTADInstance.h"

#include <MMDeviceAPI.h>
#include <FunctionDiscovery.h>
#include <Audioclient.h>
#include <assert.h>

void * PTCALL PTADInstance_ForProcess()
{
	//MMDevAPI

	HRESULT hResult;

	hResult = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	assert(SUCCEEDED(hResult));

	IClassFactory *pDeviceEnumeratorClass;
	hResult = ::CoGetClassObject(__uuidof(MMDeviceEnumerator), CLSCTX_ALL, NULL, IID_PPV_ARGS(&pDeviceEnumeratorClass));
	assert(SUCCEEDED(hResult));

	IMMDeviceEnumerator *pDeviceEnumerator;
	hResult = pDeviceEnumeratorClass->CreateInstance(NULL, IID_PPV_ARGS(&pDeviceEnumerator));
	assert(SUCCEEDED(hResult));
	pDeviceEnumeratorClass->Release();


	IMMDevice *pDevice;
	//用户通过任务栏右下角选择
	hResult = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	assert(SUCCEEDED(hResult));

	WCHAR *FriendlyName = NULL;
	{
		IPropertyStore *pProperties;
		hResult = pDevice->OpenPropertyStore(STGM_READ, &pProperties);
		assert(SUCCEEDED(hResult));

		PROPVARIANT V;
		pProperties->GetValue(PKEY_DeviceInterface_FriendlyName, &V);
		assert(V.vt == VT_LPWSTR);
		FriendlyName = V.pwszVal;

		pProperties->Release();
	}

	IAudioClient *pAudioClient;
	hResult = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, reinterpret_cast<void **>(&pAudioClient));
	assert(SUCCEEDED(hResult));

	WAVEFORMATEX *pDeviceFormat;
	hResult = pAudioClient->GetMixFormat(&pDeviceFormat);
	assert(SUCCEEDED(hResult));

	return NULL;
}