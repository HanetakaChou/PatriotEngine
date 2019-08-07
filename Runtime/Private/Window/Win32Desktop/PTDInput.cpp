#include "PTDInput.h"

HRESULT STDMETHODCALLTYPE NoRegCoCreate(wchar_t const *dllName, REFCLSID rclsid, REFIID riid, void **ppv)
{
	HINSTANCE hLibrary = ::LoadLibraryW(dllName);
	if (!hLibrary)
	{
		return HRESULT_FROM_WIN32(::GetLastError());
	}

	decltype(::DllGetClassObject) *pFn_DllGetClassObject = reinterpret_cast<decltype(::DllGetClassObject) *>(::GetProcAddress(hLibrary, "DllGetClassObject"));
	if (!pFn_DllGetClassObject)
	{
		return HRESULT_FROM_WIN32(::GetLastError());
	}

	IClassFactory *pClassFactory;
	HRESULT rsDllGetClassObject = pFn_DllGetClassObject(rclsid, IID_PPV_ARGS(&pClassFactory));
	if (FAILED(rsDllGetClassObject))
	{
		return rsDllGetClassObject;
	}

	HRESULT rsCreateInstance = pClassFactory->CreateInstance(NULL, riid, ppv);
	if (FAILED(rsCreateInstance))
	{
		return rsCreateInstance;
	}

	pClassFactory->Release();

	return S_OK;
}

DWORD const DIRECTINPUT_VERSION = 0x0800;

DWORD const DI8DEVCLASS_GAMECTRL = 4;

DWORD const DIENUM_CONTINUE = 1;

DWORD const DIEDFL_ATTACHEDONLY = 0x00000001;

GUID const GUID_XAxis = { 0xA36D02E0, 0xC9F3, 0x11CF, { 0xBF, 0xC7,  0x44,  0x45,  0x53,  0x54,  0x00,  0x00 } };
GUID const GUID_YAxis = { 0xA36D02E1, 0xC9F3, 0x11CF, { 0xBF, 0xC7,  0x44,  0x45,  0x53,  0x54,  0x00,  0x00 } };
GUID const GUID_ZAxis = { 0xA36D02E2, 0xC9F3, 0x11CF, { 0xBF, 0xC7,  0x44,  0x45,  0x53,  0x54,  0x00,  0x00 } };
GUID const GUID_RxAxis = { 0xA36D02F4, 0xC9F3, 0x11CF, { 0xBF, 0xC7,  0x44,  0x45,  0x53,  0x54,  0x00,  0x00 } };
GUID const GUID_RyAxis = { 0xA36D02F5, 0xC9F3, 0x11CF, { 0xBF, 0xC7,  0x44,  0x45,  0x53,  0x54,  0x00,  0x00 } };
GUID const GUID_RzAxis = { 0xA36D02E3, 0xC9F3, 0x11CF, { 0xBF, 0xC7,  0x44,  0x45,  0x53,  0x54,  0x00,  0x00 } };
GUID const GUID_POV = { 0xA36D02F2, 0xC9F3, 0x11CF, { 0xBF, 0xC7,  0x44,  0x45,  0x53,  0x54,  0x00,  0x00 } };

//GUID const GUID_Slider = { 0xA36D02E4, 0xC9F3, 0x11CF,{ 0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 } };

DWORD const DIDFT_AXIS = 0x00000003;
DWORD const DIDFT_POV = 0x00000010;
DWORD const DIDFT_BUTTON = 0x0000000C;

DWORD const	DIDFT_ANYINSTANCE = 0x00FFFF00;
static inline DWORD DIDFT_MAKEINSTANCE(WORD n) { return ((WORD)(n) << 8); };

DWORD const DIDOI_ASPECTPOSITION = 0x00000100;

DWORD const DIDF_ABSAXIS = 0x00000001;

GUID const &DIPROP_DEADZONE = (*(GUID const*)(5));

DWORD const DIPH_BYOFFSET = 1;

struct DIOBJECTDATAFORMAT {
	const GUID *pguid;
	DWORD   dwOfs;
	DWORD   dwType;
	DWORD   dwFlags;
};

struct DIDATAFORMAT {
	DWORD   dwSize;
	DWORD   dwObjSize;
	DWORD   dwFlags;
	DWORD   dwDataSize;
	DWORD   dwNumObjs;
	DIOBJECTDATAFORMAT *rgodf;
};

static DIOBJECTDATAFORMAT c_odfDIXBOXONECONTROLLER[15] =
{
	//D-Pad(Directional Pad)
	{
		&GUID_POV,
		0,
		DIDFT_ANYINSTANCE | DIDFT_POV,
		0U
	},
	//A
	{
		NULL,
		sizeof(DWORD) * 1,
		DIDFT_MAKEINSTANCE(0) | DIDFT_BUTTON,
		0U
	},
	//B
	{
		NULL,
		sizeof(DWORD) * 1 + sizeof(BYTE) * 1,
		DIDFT_MAKEINSTANCE(1) | DIDFT_BUTTON,
		0U
	},
	//X
	{
		NULL,
		sizeof(DWORD) * 1 + sizeof(BYTE) * 2,
		DIDFT_MAKEINSTANCE(2) | DIDFT_BUTTON,
		0U
	},
	//Y
	{
		NULL,
		sizeof(DWORD) * 1 + sizeof(BYTE) * 3,
		DIDFT_MAKEINSTANCE(3) | DIDFT_BUTTON,
		0U
	},
	//LB(Left Bumper)
	{
		NULL,
		sizeof(DWORD) * 1 + sizeof(BYTE) * 4,
		 DIDFT_MAKEINSTANCE(4) | DIDFT_BUTTON,
		 0U
	},
	//RB(Right Bumper)
	{
		NULL,
		sizeof(DWORD) * 1 + sizeof(BYTE) * 5,
		DIDFT_MAKEINSTANCE(5) | DIDFT_BUTTON,
		0U
	},
	//View
	{
		NULL,
		sizeof(DWORD) * 1 + sizeof(BYTE) * 6,
		 DIDFT_MAKEINSTANCE(6) | DIDFT_BUTTON,
		 0U
	},
	//Menu
	{
		NULL,
		sizeof(DWORD) * 1 + sizeof(BYTE) * 7,
		 DIDFT_MAKEINSTANCE(7) | DIDFT_BUTTON,
		 0U
	},
	//LT(Left Trigger) And RT(Right Trigger)
	{
		&GUID_ZAxis,
		sizeof(DWORD) * 1 + sizeof(BYTE) * 8,
		DIDFT_ANYINSTANCE | DIDFT_AXIS,
		DIDOI_ASPECTPOSITION
	},
	//L(Left Stick)_X(AxisX)
	{
		&GUID_XAxis,
		sizeof(DWORD) * 1 + sizeof(BYTE) * 8 + sizeof(DWORD) * 1,
		DIDFT_ANYINSTANCE | DIDFT_AXIS,
		DIDOI_ASPECTPOSITION
	},
	//L(Left Stick)_Y(AxisY)
	{
		&GUID_YAxis,
		sizeof(DWORD) * 1 + sizeof(BYTE) * 8 + sizeof(DWORD) * 2,
		DIDFT_ANYINSTANCE | DIDFT_AXIS,
		DIDOI_ASPECTPOSITION
	},
	//R(Right Stick)_X(AxisX)
	{
		&GUID_RxAxis,
		sizeof(DWORD) * 1 + sizeof(BYTE) * 8 + sizeof(DWORD) * 3,
		DIDFT_ANYINSTANCE | DIDFT_AXIS,
		DIDOI_ASPECTPOSITION
	},
	//R(Right Stick)_Y(AxisY)
	{
		&GUID_RyAxis,
		sizeof(DWORD) * 1 + sizeof(BYTE) * 8 + sizeof(DWORD) * 4,
		DIDFT_ANYINSTANCE | DIDFT_AXIS,
		DIDOI_ASPECTPOSITION
	},
};

DIDATAFORMAT const c_dfDIXBOXONECONTROLLER =
{
	sizeof(DIDATAFORMAT),
	sizeof(DIOBJECTDATAFORMAT),
	DIDF_ABSAXIS,
	sizeof(DWORD) * 1 + sizeof(BYTE) * 8 + sizeof(DWORD) * 5,
	14,
	c_odfDIXBOXONECONTROLLER
};

static DIOBJECTDATAFORMAT c_odfDIPS4CONTROLLER[16] =
{
	//D-Pad(Directional Pad)
	{
		&GUID_POV,
		0,
		DIDFT_ANYINSTANCE | DIDFT_POV,
		0U
	},
	//A
	{
		NULL,
		sizeof(DWORD) * 1,
		DIDFT_MAKEINSTANCE(1) | DIDFT_BUTTON,
		0U
	},
	//B
	{
		NULL,
		sizeof(DWORD) * 1 + sizeof(BYTE) * 1,
		DIDFT_MAKEINSTANCE(2) | DIDFT_BUTTON,
		0U
	},
	//X
	{
		NULL,
		sizeof(DWORD) * 1 + sizeof(BYTE) * 2,
		DIDFT_MAKEINSTANCE(0) | DIDFT_BUTTON,
		0U
	},
	//Y
	{
		NULL,
		sizeof(DWORD) * 1 + sizeof(BYTE) * 3,
		DIDFT_MAKEINSTANCE(3) | DIDFT_BUTTON,
		0U
	},
	//LB(Left Bumper)
	{
		NULL,
		sizeof(DWORD) * 1 + sizeof(BYTE) * 4,
		DIDFT_MAKEINSTANCE(4) | DIDFT_BUTTON,
		0U
	},
	//RB(Right Bumper)
	{
		NULL,
		sizeof(DWORD) * 1 + sizeof(BYTE) * 5,
		DIDFT_MAKEINSTANCE(5) | DIDFT_BUTTON,
		0U
	},
	//Share
	{
		NULL,
		sizeof(DWORD) * 1 + sizeof(BYTE) * 6,
		DIDFT_MAKEINSTANCE(8) | DIDFT_BUTTON,
		0U
	},
	//Options
	{
		NULL,
		sizeof(DWORD) * 1 + sizeof(BYTE) * 7,
		DIDFT_MAKEINSTANCE(9) | DIDFT_BUTTON,
		0U
	},
	//LT(Left Trigger)
	{
		&GUID_RxAxis,
		sizeof(DWORD) * 1 + sizeof(BYTE) * 8,
		DIDFT_ANYINSTANCE | DIDFT_AXIS,
		DIDOI_ASPECTPOSITION
	},
	//RT(Right Trigger)
	{
		&GUID_RyAxis,
		sizeof(DWORD) * 1 + sizeof(BYTE) * 8 + sizeof(DWORD) * 1,
		DIDFT_ANYINSTANCE | DIDFT_AXIS,
		DIDOI_ASPECTPOSITION
	},
	//L(Left Stick)_X(AxisX)
	{
		&GUID_XAxis,
		sizeof(DWORD) * 1 + sizeof(BYTE) * 8 + sizeof(DWORD) * 2,
		DIDFT_ANYINSTANCE | DIDFT_AXIS,
		DIDOI_ASPECTPOSITION
	},
	//L(Left Stick)_Y(AxisY)
	{
		&GUID_YAxis,
		sizeof(DWORD) * 1 + sizeof(BYTE) * 8 + sizeof(DWORD) * 3,
		DIDFT_ANYINSTANCE | DIDFT_AXIS,
		DIDOI_ASPECTPOSITION
	},
	//R(Right Stick)_X(AxisX)
	{
		&GUID_ZAxis,
		sizeof(DWORD) * 1 + sizeof(BYTE) * 8 + sizeof(DWORD) * 4,
		DIDFT_ANYINSTANCE | DIDFT_AXIS,
		DIDOI_ASPECTPOSITION
	},
	//R(Right Stick)_Y(AxisY)
	{
		&GUID_RzAxis,
		sizeof(DWORD) * 1 + sizeof(BYTE) * 8 + sizeof(DWORD) * 5,
		DIDFT_ANYINSTANCE | DIDFT_AXIS,
		DIDOI_ASPECTPOSITION
	},
	//PS4
	{
		NULL,
		sizeof(DWORD) * 1 + sizeof(BYTE) * 8 + sizeof(DWORD) * 6,
		DIDFT_MAKEINSTANCE(12) | DIDFT_BUTTON,
		0U
	}
};

DIDATAFORMAT const c_dfDIPS4CONTROLLER =
{
	sizeof(DIDATAFORMAT),
	sizeof(DIOBJECTDATAFORMAT),
	DIDF_ABSAXIS,
	sizeof(DWORD) * 1 + sizeof(BYTE) * 8 + sizeof(DWORD) * 6 + sizeof(BYTE) * 4,
	16,
	c_odfDIPS4CONTROLLER
};

DICONTROLLERTYPE WINAPI DIGetControllerTypeForGUID(REFGUID rguidProduct)
{
	//BOOL IsXInputDevice(const GUID *pGuidProductFromDirectInput)
	//https://docs.microsoft.com/en-us/windows/win32/xinput/xinput-and-directinput
	LONG VidPid = rguidProduct.Data1;

	//Supported Controller Database
	//https://support.steampowered.com/kb_article.php?ref=5199-TOKV-4426

	//To Do //难道要人工编码所有可能？

	DICONTROLLERTYPE ControllerType;
	switch (VidPid)
	{
	case MAKELONG(0x045e, 0x02ff): //Microsoft X-Box One Elite pad		
		ControllerType = DICT_XBOXONE;
		break;
	case MAKELONG(0x054c, 0x09cc): //Sony PS4 Slim Controller
		ControllerType = DICT_PS4;
		break;
	default:
		ControllerType = DICT_UNKNOWN;
		break;
	}

	return ControllerType;
}
