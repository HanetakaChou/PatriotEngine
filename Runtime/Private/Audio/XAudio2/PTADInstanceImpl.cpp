#include "../../../Public/Audio/PTADInstance.h"

#define WIN32_LEAN_AND_MEAN
#include "../../../ThirdParty/XAudio2/Include/XAudio2.h"
#include <assert.h>

#include <mmdeviceapi.h>

void * PTCALL PTADInstance_ForProcess()
{
	HRESULT hResult;

	hResult = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	assert(SUCCEEDED(hResult));

	IClassFactory *pAudio2Class;
#ifndef NDEBUG
	hResult = DllGetClassObject(__uuidof(XAudio2_Debug), IID_PPV_ARGS(&pAudio2Class));
#else
	hResult = DllGetClassObject(__uuidof(XAudio2), IID_PPV_ARGS(&pAudioClass));
#endif
	assert(SUCCEEDED(hResult));

	IXAudio2 *pXAudio2;
	hResult = pAudio2Class->CreateInstance(NULL, IID_PPV_ARGS(&pXAudio2));
	assert(SUCCEEDED(hResult));
	pAudio2Class->Release();

#ifndef NDEBUG
	hResult = pXAudio2->Initialize(XAUDIO2_DEBUG_ENGINE, XAUDIO2_ANY_PROCESSOR);
#else
	hResult = pXAudio2->Initialize(0U, XAUDIO2_ANY_PROCESSOR);
#endif
	assert(SUCCEEDED(hResult));

	UINT32 Count;
	hResult = pXAudio2->GetDeviceCount(&Count);
	assert(SUCCEEDED(hResult));

	for (UINT32 Index = 0U; Index < Count; ++Index)
	{
		XAUDIO2_DEVICE_DETAILS DeviceDetailsd;
		hResult = pXAudio2->GetDeviceDetails(Index, &DeviceDetailsd);
		assert(SUCCEEDED(hResult));

		//用户通过任务栏右下角选择
		if (DeviceDetailsd.Role & GlobalDefaultDevice)
		{
			break;
		}

	}

	return NULL;
}