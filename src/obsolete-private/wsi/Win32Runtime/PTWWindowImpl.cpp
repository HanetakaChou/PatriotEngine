#include "PTWWindowImpl.h"

#include <assert.h>

static unsigned __stdcall PTInvokeMain(void *pVoid);

#include <roapi.h>

//入口点
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR lpCmdLine, int nCmdShow)
{
	HRESULT hResult;

	hResult = ::RoInitialize(RO_INIT_MULTITHREADED);
	assert(SUCCEEDED(hResult));

	ABI::Windows::ApplicationModel::Core::ICoreApplication *pCoreApplication;
	{
		HSTRING_HEADER hString_Header;
		HSTRING hString;
		hResult = ::WindowsCreateStringReference(::RuntimeClass_Windows_ApplicationModel_Core_CoreApplication, _countof(::RuntimeClass_Windows_ApplicationModel_Core_CoreApplication) - 1U, &hString_Header, &hString);
		assert(SUCCEEDED(hResult));
		hResult = ::RoGetActivationFactory(hString, IID_PPV_ARGS(&pCoreApplication));
		assert(SUCCEEDED(hResult));
	}

	PTWWindowImpl l_WindowImpl_Singleton;
	l_WindowImpl_Singleton.m_pCmdLine = lpCmdLine;

	hResult = pCoreApplication->Run(&l_WindowImpl_Singleton);
	assert(SUCCEEDED(hResult));

	::RoUninitialize();
	return 0U;
}

#include "../../../Public/App/PTAExport.h"
#include "../../../Public/McRT/Win32/PTSConvUTF.h"

static unsigned __stdcall PTInvokeMain(void *pVoid)
{
	PTWWindowImpl *pWindow = static_cast<PTWWindowImpl *>(pVoid);

	char CmdLineA[0X10000] = { "PTLauncher " };
	{
		size_t InCharsLeft = static_cast<size_t>(::wcslen(pWindow->m_pCmdLine)) + 1U;//包括'\0'
		size_t OutCharsLeft = 4096 - 11;
		::PTSConv_UTF16ToUTF8(pWindow->m_pCmdLine, &InCharsLeft, CmdLineA + 11, &OutCharsLeft);
	}

	char *argv[0X10000];
	int argc = 0;

	enum
	{
		WhiteSpace,
		Text
	}StateMachine = WhiteSpace;

	for (size_t i = 0U; CmdLineA[i] != '\0'; ++i)
	{
		switch (StateMachine)
		{
		case WhiteSpace:
		{
			if (!((CmdLineA[i] == ' ' || CmdLineA[i] == '\t' || CmdLineA[i] == '\r' || CmdLineA[i] == '\n')))
			{
				assert(argc < 0X10000);
				argv[argc] = CmdLineA + i;
				++argc;
				StateMachine = Text;
			}
		}
		case Text:
		{
			if ((CmdLineA[i] == ' ' || CmdLineA[i] == '\t' || CmdLineA[i] == '\r' || CmdLineA[i] == '\n'))
			{
				CmdLineA[i] = '\0';
				StateMachine = WhiteSpace;
			}
		}
		}

		assert(i < 0X10000U);
	}

	argv[argc] = NULL;

	int iResult = ::PTAMain(static_cast<PT_WSI_IWindow *>(pWindow), argc, argv);

	pWindow->TermminateMessagePump();

	return static_cast<unsigned>(iResult);
}

inline PTWWindowImpl::PTWWindowImpl()
{
	//EventOutputCallback
	m_pEventOutputCallback = NULL;
	m_pEventOutputCallback_UserData = NULL;
	//EventInputCallback
	m_pEventInputCallback = NULL;
	m_pEventInputCallback_UserData = NULL;
	//Handle
	m_hWindow_Cache = NULL;
}

inline PTWWindowImpl::~PTWWindowImpl()
{

}

void PTWWindowImpl::EventOutputCallback_Hook(void *pUserData, void(PTPTR *pEventOutputCallback)(void *pUserData, void *pOutputData))
{
	::PTSAtomic_Set(reinterpret_cast<uintptr_t volatile *>(&m_pEventOutputCallback_UserData), reinterpret_cast<uintptr_t>(pUserData));
	::PTSAtomic_Set(reinterpret_cast<uintptr_t volatile *>(&m_pEventOutputCallback), reinterpret_cast<uintptr_t>(pEventOutputCallback));

	//PTInvokeMain线程调用
	//一定发生在ASTA线程执行m_hWindow_Cache = pWindow之后
	assert(m_hWindow_Cache != NULL);

	HRESULT hResult;
	ABI::Windows::UI::Core::ICoreDispatcher *pCoreDispatcher;
	hResult = m_hWindow_Cache->get_Dispatcher(&pCoreDispatcher);
	assert(SUCCEEDED(hResult));

	static class :public ABI::Windows::UI::Core::IDispatchedHandler, public IAgileObject
	{
		PTWWindowImpl *m_pPTWWindow;

		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) override
		{
			if (riid == __uuidof(IUnknown) || riid == __uuidof(ABI::Windows::UI::Core::IDispatchedHandler))
			{
				*ppvObject = static_cast<ABI::Windows::UI::Core::IDispatchedHandler *>(this);
				return S_OK;
			}
			else if (riid == __uuidof(IAgileObject))
			{
				*ppvObject = static_cast<IAgileObject *>(this);
				return S_OK;
			}
			else
			{
				return E_NOINTERFACE;
			}
		}
		ULONG STDMETHODCALLTYPE AddRef(void) override
		{
			return 1U;
		}
		ULONG STDMETHODCALLTYPE Release(void) override
		{
			return 1U;
		}
		HRESULT STDMETHODCALLTYPE Invoke(void) override
		{
			assert(m_pPTWWindow != NULL);

			//ASTA线程
			if (m_pPTWWindow->m_hWindow_Cache != NULL)
			{
				ABI::Windows::Foundation::Rect Bounds;
				HRESULT hResult = m_pPTWWindow->m_hWindow_Cache->get_Bounds(&Bounds);
				assert(SUCCEEDED(hResult));

				struct
				{
					uint32_t m_Type;
					struct PT_WSI_Display_T * m_hDisplay;
					struct PT_WSI_Window_T * m_hWindow;
				}EventOutput_WindowCreated;
				EventOutput_WindowCreated.m_Type = PTWWindowImpl::PTEventOutputType_WindowCreated;
				EventOutput_WindowCreated.m_hDisplay = NULL;
				EventOutput_WindowCreated.m_hWindow = m_pPTWWindow->m_hWindow_Cache;

				m_pPTWWindow->m_pEventOutputCallback(m_pPTWWindow->m_pEventOutputCallback_UserData, &EventOutput_WindowCreated);

			}
			return S_OK;
		}

	public:
		inline void This_Cache(PTWWindowImpl *pPTWWindow)
		{
			m_pPTWWindow = pPTWWindow;
		}
	} s_Lambda;//必须使用静态存储区，因为必须保证函数对象在SATA线程访问时仍然存在
	
	s_Lambda.This_Cache(this);

	ABI::Windows::Foundation::IAsyncAction *pAsyncAction;
	hResult = pCoreDispatcher->RunAsync(ABI::Windows::UI::Core::CoreDispatcherPriority_Normal, &s_Lambda, &pAsyncAction);//唤醒ASTA线程
	assert(SUCCEEDED(hResult));

	//由于属于调试器有Bug，在查看调用堆栈后，应用程序会意外中断

	pAsyncAction->Release();

	pCoreDispatcher->Release();
}

void PTWWindowImpl::EventInputCallback_Hook(void *pUserData, void(PTPTR *pEventInputCallback)(void *pUserData, void *pInputData))
{
	::PTSAtomic_Set(reinterpret_cast<uintptr_t volatile *>(&m_pEventInputCallback_UserData), reinterpret_cast<uintptr_t>(pUserData));
	::PTSAtomic_Set(reinterpret_cast<uintptr_t volatile *>(&m_pEventInputCallback), reinterpret_cast<uintptr_t>(pEventInputCallback));
}

void PTWWindowImpl::Parent_Set(struct PT_WSI_Window_T *)
{
	assert(0);
}

void PTWWindowImpl::Position_Set(uint32_t, uint32_t)
{
	assert(0);
}

void PTWWindowImpl::Size_Set(uint32_t, uint32_t)
{
	assert(0);
}

void PTWWindowImpl::TermminateMessagePump()
{
	//PTInvokeMain线程调用
	//一定发生在ASTA线程执行m_hWindow_Cache = pWindow之后
	assert(m_hWindow_Cache != NULL);

	HRESULT hResult;
	ABI::Windows::UI::Core::ICoreDispatcher *pCoreDispatcher;
	hResult = m_hWindow_Cache->get_Dispatcher(&pCoreDispatcher);
	assert(SUCCEEDED(hResult));

	static class :public ABI::Windows::UI::Core::IDispatchedHandler, public IAgileObject
	{
		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) override
		{
			if (riid == __uuidof(IUnknown) || riid == __uuidof(ABI::Windows::UI::Core::IDispatchedHandler))
			{
				*ppvObject = static_cast<ABI::Windows::UI::Core::IDispatchedHandler *>(this);
				return S_OK;
			}
			else if (riid == __uuidof(IAgileObject))
			{
				*ppvObject = static_cast<IAgileObject *>(this);
				return S_OK;
			}
			else
			{
				return E_NOINTERFACE;
			}
		}
		ULONG STDMETHODCALLTYPE AddRef(void) override
		{
			return 1U;
		}
		ULONG STDMETHODCALLTYPE Release(void) override
		{
			return 1U;
		}
		HRESULT STDMETHODCALLTYPE Invoke(void) override
		{
			//ASTA线程
			::RaiseException(0XE06D7363U, 0U, 0U, NULL);//模拟UNIX信号
			return S_OK;
		}
	} s_Lambda;//必须使用静态存储区，因为必须保证函数对象在SATA线程访问时仍然存在

	ABI::Windows::Foundation::IAsyncAction *pAsyncAction;
	hResult = pCoreDispatcher->RunAsync(ABI::Windows::UI::Core::CoreDispatcherPriority_Normal, &s_Lambda, &pAsyncAction);//唤醒ASTA线程
	assert(SUCCEEDED(hResult));
	pAsyncAction->Release();

	pCoreDispatcher->Release();
}

HRESULT STDMETHODCALLTYPE PTWWindowImpl::QueryInterface(REFIID riid, void **ppvObject)
{
	if (riid == __uuidof(IUnknown) || riid == __uuidof(IInspectable) || riid == __uuidof(ABI::Windows::ApplicationModel::Core::IFrameworkViewSource))
	{
		*ppvObject = static_cast<ABI::Windows::ApplicationModel::Core::IFrameworkViewSource *>(this);
		return S_OK;
	}
	else if (riid == __uuidof(ABI::Windows::ApplicationModel::Core::IFrameworkView))
	{
		*ppvObject = static_cast<ABI::Windows::ApplicationModel::Core::IFrameworkView *>(this);
		return S_OK;
	}
	else
	{
		return E_NOINTERFACE;
	}
}
ULONG STDMETHODCALLTYPE PTWWindowImpl::AddRef(void)
{
	return 1U;
}
ULONG STDMETHODCALLTYPE PTWWindowImpl::Release(void)
{
	return 1U;
}
//IInspectable
HRESULT STDMETHODCALLTYPE PTWWindowImpl::GetIids(ULONG *iidCount, IID **iids)
{
	(*iids) = static_cast<IID *>(::CoTaskMemAlloc(sizeof(IID) * 2));
	if ((*iids) == NULL)
		return E_OUTOFMEMORY;

	*iidCount = 2U;
	(*iids)[0] = __uuidof(ABI::Windows::ApplicationModel::Core::IFrameworkViewSource);
	(*iids)[1] = __uuidof(ABI::Windows::ApplicationModel::Core::IFrameworkView);
	return S_OK;
}
HRESULT STDMETHODCALLTYPE PTWWindowImpl::GetRuntimeClassName(HSTRING *className)
{
	return ::WindowsCreateString(L"PTWindowImpl", _countof(L"PTWindowImpl") - 1U, className);
}
HRESULT STDMETHODCALLTYPE PTWWindowImpl::GetTrustLevel(TrustLevel* trustLevel)
{
	*trustLevel = ::BaseTrust;
	return S_OK;
}
HRESULT STDMETHODCALLTYPE PTWWindowImpl::CreateView(ABI::Windows::ApplicationModel::Core::IFrameworkView **viewProvider)
{
	//严格意义上应该使用QueryInterface
	(*viewProvider) = static_cast<ABI::Windows::ApplicationModel::Core::IFrameworkView *>(this);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE PTWWindowImpl::Initialize(ABI::Windows::ApplicationModel::Core::ICoreApplicationView *pApplicationView)
{
	EventRegistrationToken m_Token_ApplicationView_Activated;
	//严格意义上应该使用QueryInterface
	pApplicationView->add_Activated(static_cast<PTNativeInvoke_ApplicationView_Activated*>(this), &m_Token_ApplicationView_Activated);
	return S_OK;
}
HRESULT STDMETHODCALLTYPE PTWWindowImpl::SetWindow(ABI::Windows::UI::Core::ICoreWindow *pWindow)
{
	//注册监听器
	EventRegistrationToken m_Token_Window_KeyDown;
	pWindow->add_KeyDown(static_cast<PTNativeInvoke_Window_KeyDown*>(this), &m_Token_Window_KeyDown);
	EventRegistrationToken m_Token_Window_KeyUp;
	pWindow->add_KeyUp(static_cast<PTNativeInvoke_Window_KeyUp*>(this), &m_Token_Window_KeyUp);
	EventRegistrationToken m_Token_Window_PointerMoved;
	pWindow->add_PointerMoved(static_cast<PTNativeInvoke_Window_PointerMoved*>(this), &m_Token_Window_PointerMoved);
	EventRegistrationToken m_Token_Window_PointerPressed;
	pWindow->add_PointerPressed(static_cast<PTNativeInvoke_Window_PointerPressed*>(this), &m_Token_Window_PointerPressed);
	EventRegistrationToken m_Token_Window_PointerReleased;
	pWindow->add_PointerReleased(static_cast<PTNativeInvoke_Window_PointerReleased*>(this), &m_Token_Window_PointerReleased);
	EventRegistrationToken m_Token_Window_SizeChanged;
	pWindow->add_SizeChanged(static_cast<PTNativeInvoke_Window_SizeChanged*>(this), &m_Token_Window_SizeChanged);

	m_hWindow_Cache = pWindow;

	assert(m_pEventOutputCallback == NULL);

#if 0
	if (m_pOutputEventCallback != NULL)
	{
		ABI::Windows::Foundation::Rect Bounds;
		HRESULT hResult = pWindow->get_Bounds(&Bounds);
		assert(SUCCEEDED(hResult));

		struct
		{
			uint32_t m_Type;
			struct PT_WSI_Display_T * m_hDisplay;
			struct PT_WSI_Window_T * m_hWindow;
			uint32_t m_Width;
			uint32_t m_Height;
		}OutputEventData;
		OutputEventData.m_Type = PTWWindowImpl::PTOutputEventType_WindowCreated;
		OutputEventData.m_hDisplay = NULL;
		OutputEventData.m_hWindow = pWindow;
		OutputEventData.m_Width = static_cast<uint32_t>(Bounds.Width);
		OutputEventData.m_Height = static_cast<uint32_t>(Bounds.Height);

		m_pOutputEventCallback(m_pOutputEventCallback_UserData, &OutputEventData);
	}
#endif

	return S_OK;
}
HRESULT STDMETHODCALLTYPE PTWWindowImpl::Load(HSTRING entryPoint)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE PTWWindowImpl::Run(void)
{
	//ICoreDispatcher::RunAsync可能发生在Load和Run之间
	//确保SATA线程在ICoreDispatcher->ProcessEvents中执行IDispatchedHandler::Invoke
	bool bResult;
	bResult = ::PTSThread_Create(&PTInvokeMain, static_cast<void *>(this), &m_hThreadInvoke);
	assert(bResult);

	HRESULT hResult;
	ABI::Windows::UI::Core::ICoreDispatcher *pCoreDispatcher;
	hResult = m_hWindow_Cache->get_Dispatcher(&pCoreDispatcher);//SATA线程Get不需要加锁
	assert(SUCCEEDED(hResult));
	//模拟UNIX信号
	__try
	{
		hResult = pCoreDispatcher->ProcessEvents(ABI::Windows::UI::Core::CoreProcessEventsOption_ProcessUntilQuit);//消息泵
		assert(SUCCEEDED(hResult));
	}
	__except ((GetExceptionCode() == 0XE06D7363U) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{

	}
	pCoreDispatcher->Release();

	bResult = ::PTSThread_Join(&m_hThreadInvoke);
	assert(bResult);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE PTWWindowImpl::Uninitialize(void)
{
	return S_OK;
}

inline HRESULT STDMETHODCALLTYPE PTWWindowImpl::Invoke_ApplicationView_Activated(ABI::Windows::ApplicationModel::Core::ICoreApplicationView *pSender, ABI::Windows::ApplicationModel::Activation::IActivatedEventArgs *pArgs)
{
	ABI::Windows::UI::Core::ICoreWindow *pValue;
	pSender->get_CoreWindow(&pValue);
	pValue->Activate();
	return S_OK;
}

inline uint32_t PTWWindowImpl::RTKeyType_To_PTKeyType(ABI::Windows::System::VirtualKey Value)
{
	switch (Value)
	{
	case ABI::Windows::System::VirtualKey_A:
		return KeyType_A;
	case ABI::Windows::System::VirtualKey_B:
		return KeyType_B;
	case ABI::Windows::System::VirtualKey_C:
		return KeyType_C;
	case ABI::Windows::System::VirtualKey_D:
		return KeyType_D;
	case ABI::Windows::System::VirtualKey_E:
		return KeyType_E;
	case ABI::Windows::System::VirtualKey_F:
		return KeyType_F;
	case ABI::Windows::System::VirtualKey_G:
		return KeyType_G;
	case ABI::Windows::System::VirtualKey_H:
		return KeyType_H;
	case ABI::Windows::System::VirtualKey_I:
		return KeyType_I;
	case ABI::Windows::System::VirtualKey_J:
		return KeyType_J;
	case ABI::Windows::System::VirtualKey_K:
		return KeyType_K;
	case ABI::Windows::System::VirtualKey_L:
		return KeyType_L;
	case ABI::Windows::System::VirtualKey_M:
		return KeyType_M;
	case ABI::Windows::System::VirtualKey_N:
		return KeyType_N;
	case ABI::Windows::System::VirtualKey_O:
		return KeyType_O;
	case ABI::Windows::System::VirtualKey_P:
		return KeyType_P;
	case ABI::Windows::System::VirtualKey_Q:
		return KeyType_Q;
	case ABI::Windows::System::VirtualKey_R:
		return KeyType_R;
	case ABI::Windows::System::VirtualKey_S:
		return KeyType_S;
	case ABI::Windows::System::VirtualKey_T:
		return KeyType_T;
	case ABI::Windows::System::VirtualKey_U:
		return KeyType_U;
	case ABI::Windows::System::VirtualKey_V:
		return KeyType_V;
	case ABI::Windows::System::VirtualKey_W:
		return KeyType_W;
	case ABI::Windows::System::VirtualKey_X:
		return KeyType_X;
	case ABI::Windows::System::VirtualKey_Y:
		return KeyType_Y;
	case ABI::Windows::System::VirtualKey_Z:
		return KeyType_Z;
	case ABI::Windows::System::VirtualKey_Number0:
		return KeyType_0;
	case ABI::Windows::System::VirtualKey_Number1:
		return KeyType_1;
	case ABI::Windows::System::VirtualKey_Number2:
		return KeyType_2;
	case ABI::Windows::System::VirtualKey_Number3:
		return KeyType_3;
	case ABI::Windows::System::VirtualKey_Number4:
		return KeyType_4;
	case ABI::Windows::System::VirtualKey_Number5:
		return KeyType_5;
	case ABI::Windows::System::VirtualKey_Number6:
		return KeyType_6;
	case ABI::Windows::System::VirtualKey_Number7:
		return KeyType_7;
	case ABI::Windows::System::VirtualKey_Number8:
		return KeyType_8;
	case ABI::Windows::System::VirtualKey_Number9:
		return KeyType_9;
	case ABI::Windows::System::VirtualKey_F1:
		return KeyType_F1;
	case ABI::Windows::System::VirtualKey_F2:
		return KeyType_F2;
	case ABI::Windows::System::VirtualKey_F3:
		return KeyType_F3;
	case ABI::Windows::System::VirtualKey_F4:
		return KeyType_F4;
	case ABI::Windows::System::VirtualKey_F5:
		return KeyType_F5;
	case ABI::Windows::System::VirtualKey_F6:
		return KeyType_F6;
	case ABI::Windows::System::VirtualKey_F7:
		return KeyType_F7;
	case ABI::Windows::System::VirtualKey_F8:
		return KeyType_F8;
	case ABI::Windows::System::VirtualKey_F9:
		return KeyType_F9;
	case ABI::Windows::System::VirtualKey_F10:
		return KeyType_F10;
	case ABI::Windows::System::VirtualKey_F11:
		return KeyType_F11;
	case ABI::Windows::System::VirtualKey_F12:	
		return KeyType_F12;
	case ABI::Windows::System::VirtualKey_Space:
		return KeyType_Space;
	default:
		return KeyType_Unknown;
	}
}

inline  uint32_t PTWWindowImpl::RTKeyType_To_PTPointButtonType(ABI::Windows::System::VirtualKey Value)
{
	switch (Value)
	{
	case ABI::Windows::System::VirtualKey_LeftButton:
		return PointButtonType_Left;
	case ABI::Windows::System::VirtualKey_RightButton:
		return PointButtonType_Right;
	case ABI::Windows::System::VirtualKey_MiddleButton:
		return PointButtonType_Middle;
	default:
		return PointButtonType_Unknown;
	}
}

inline HRESULT STDMETHODCALLTYPE PTWWindowImpl::Invoke_Window_KeyDown(ABI::Windows::UI::Core::ICoreWindow *pSender, ABI::Windows::UI::Core::IKeyEventArgs *pArgs)
{
	if (m_pEventInputCallback != NULL)
	{
		ABI::Windows::System::VirtualKey RTKeyType;
		HRESULT hResult = pArgs->get_VirtualKey(&RTKeyType);
		assert(SUCCEEDED(hResult));

		uint32_t PTKeyType = RTKeyType_To_PTKeyType(RTKeyType);
		if (PTKeyType != KeyType_Unknown)
		{
			struct
			{
				uint32_t m_InputType;
				uint32_t m_KeyType;
			}InputData;
			InputData.m_InputType = InputType_KeyDown;
			InputData.m_KeyType = PTKeyType;
			m_pEventInputCallback(m_pEventInputCallback_UserData, &InputData);
		}
		else 
		{
			uint32_t PointButtonType = RTKeyType_To_PTPointButtonType(RTKeyType);
			if (PointButtonType != PointButtonType_Unknown)
			{
				struct
				{
					uint32_t m_InputType;
					uint32_t m_PointButtonType;
				}InputData;
				InputData.m_InputType = InputType_PointButtonPress;//为什么不是Release？？？
				InputData.m_PointButtonType = PointButtonType;
				m_pEventInputCallback(m_pEventInputCallback_UserData, &InputData);
			}
		}
	}
	return S_OK;
}

inline HRESULT STDMETHODCALLTYPE PTWWindowImpl::Invoke_Window_KeyUp(ABI::Windows::UI::Core::ICoreWindow *pSender, ABI::Windows::UI::Core::IKeyEventArgs *pArgs)
{
	if (m_pEventInputCallback != NULL)
	{
		ABI::Windows::System::VirtualKey RTKeyType;
		HRESULT hResult = pArgs->get_VirtualKey(&RTKeyType);
		assert(SUCCEEDED(hResult));

		uint32_t PTKeyType = RTKeyType_To_PTKeyType(RTKeyType);
		if (PTKeyType != KeyType_Unknown)
		{
			struct
			{
				uint32_t m_InputType;
				uint32_t m_KeyType;
			}InputData;
			InputData.m_InputType = InputType_KeyUp;
			InputData.m_KeyType = PTKeyType;
			m_pEventInputCallback(m_pEventInputCallback_UserData, &InputData);
		}
		else
		{
			uint32_t PointButtonType = RTKeyType_To_PTPointButtonType(RTKeyType);
			if (PointButtonType != PointButtonType_Unknown)
			{
				//实际中从未发生过
				struct
				{
					uint32_t m_InputType;
					uint32_t m_PointButtonType;
				}InputData;
				InputData.m_InputType = InputType_PointButtonPress;//为什么不是Release？？？
				InputData.m_PointButtonType = PointButtonType;
				m_pEventInputCallback(m_pEventInputCallback_UserData, &InputData);
			}
		}
	}
	return S_OK;
}

inline HRESULT STDMETHODCALLTYPE PTWWindowImpl::Invoke_Window_PointerMoved(ABI::Windows::UI::Core::ICoreWindow *pSender, ABI::Windows::UI::Core::IPointerEventArgs *pArgs)
{
	if (m_pEventInputCallback != NULL)
	{
		HRESULT hResult;

		ABI::Windows::UI::Input::IPointerPoint *pPointerPoint;
		hResult = pArgs->get_CurrentPoint(&pPointerPoint);
		assert(SUCCEEDED(hResult));

		ABI::Windows::Foundation::Point Position;
		hResult = pPointerPoint->get_Position(&Position);
		assert(SUCCEEDED(hResult));

		UINT32 PointerId;
		hResult = pPointerPoint->get_PointerId(&PointerId);
		assert(SUCCEEDED(hResult));

		struct
		{
			uint32_t m_InputType;
			float m_Position_X;
			float m_Position_Y;
			uint32_t m_PointId;
		}InputData;
		InputData.m_InputType = InputType_PointMove;
		InputData.m_Position_X = Position.X;
		InputData.m_Position_Y = Position.Y;
		InputData.m_PointId = PointerId;
		m_pEventInputCallback(m_pEventInputCallback_UserData, &InputData);
	}
	return S_OK;
}

inline uint32_t PTWWindowImpl::RTPointPressButtonType_To_PTPointButtonType(ABI::Windows::UI::Input::PointerUpdateKind Value)
{
	switch (Value)
	{
	case ABI::Windows::UI::Input::PointerUpdateKind_LeftButtonPressed:
		return PointButtonType_Left;
	case ABI::Windows::UI::Input::PointerUpdateKind_RightButtonPressed:
		return PointButtonType_Right;
	case ABI::Windows::UI::Input::PointerUpdateKind_MiddleButtonPressed:
		return PointButtonType_Middle;
	default:
		return PointButtonType_Unknown;
	}
}

inline uint32_t PTWWindowImpl::RTPointReleaseButtonType_To_PTPointButtonType(ABI::Windows::UI::Input::PointerUpdateKind Value)
{
	switch (Value)
	{
	case ABI::Windows::UI::Input::PointerUpdateKind_LeftButtonReleased:
		return PointButtonType_Left;
	case ABI::Windows::UI::Input::PointerUpdateKind_RightButtonReleased:
		return PointButtonType_Right;
	case ABI::Windows::UI::Input::PointerUpdateKind_MiddleButtonReleased:
		return PointButtonType_Middle;
	default:
		return PointButtonType_Unknown;
	}
}

inline HRESULT STDMETHODCALLTYPE PTWWindowImpl::Invoke_Window_PointerPressed(ABI::Windows::UI::Core::ICoreWindow *pSender, ABI::Windows::UI::Core::IPointerEventArgs *pArgs)
{
	if (m_pEventInputCallback != NULL)
	{
		HRESULT hResult;

		ABI::Windows::UI::Input::IPointerPoint *pPointerPoint;
		hResult = pArgs->get_CurrentPoint(&pPointerPoint);
		assert(SUCCEEDED(hResult));

		ABI::Windows::Foundation::Point Position;
		hResult = pPointerPoint->get_Position(&Position);
		assert(SUCCEEDED(hResult));

		ABI::Windows::UI::Input::IPointerPointProperties *pPointerPointProperties;
		hResult = pPointerPoint->get_Properties(&pPointerPointProperties);
		assert(SUCCEEDED(hResult));

		ABI::Windows::UI::Input::PointerUpdateKind PointerUpdateKind;
		hResult = pPointerPointProperties->get_PointerUpdateKind(&PointerUpdateKind);
		assert(SUCCEEDED(hResult));

		uint32_t PointButtonType = RTPointPressButtonType_To_PTPointButtonType(PointerUpdateKind);
		assert(PointButtonType != PointButtonType_Unknown);

		UINT32 PointerId;
		hResult = pPointerPoint->get_PointerId(&PointerId);
		assert(SUCCEEDED(hResult));

		struct
		{
			uint32_t m_InputType;
			float m_Position_X;
			float m_Position_Y;
			uint32_t m_PointButtonType;
			uint32_t m_PointId;
		}InputData;
		InputData.m_InputType = InputType_PointButtonPress;
		InputData.m_Position_X = Position.X;
		InputData.m_Position_Y = Position.Y;
		InputData.m_PointButtonType = PointButtonType;
		InputData.m_PointId = PointerId;
		m_pEventInputCallback(m_pEventInputCallback_UserData, &InputData);
	}
	return S_OK;
}

inline HRESULT STDMETHODCALLTYPE PTWWindowImpl::Invoke_Window_PointerReleased(ABI::Windows::UI::Core::ICoreWindow *pSender, ABI::Windows::UI::Core::IPointerEventArgs *pArgs)
{
	if (m_pEventInputCallback != NULL)
	{
		HRESULT hResult;

		ABI::Windows::UI::Input::IPointerPoint *pPointerPoint;
		hResult = pArgs->get_CurrentPoint(&pPointerPoint);
		assert(SUCCEEDED(hResult));

		ABI::Windows::Foundation::Point Position;
		hResult = pPointerPoint->get_Position(&Position);
		assert(SUCCEEDED(hResult));

		ABI::Windows::UI::Input::IPointerPointProperties *pPointerPointProperties;
		hResult = pPointerPoint->get_Properties(&pPointerPointProperties);
		assert(SUCCEEDED(hResult));

		ABI::Windows::UI::Input::PointerUpdateKind PointerUpdateKind;
		hResult = pPointerPointProperties->get_PointerUpdateKind(&PointerUpdateKind);
		assert(SUCCEEDED(hResult));

		uint32_t PointButtonType = RTPointReleaseButtonType_To_PTPointButtonType(PointerUpdateKind);
		assert(PointButtonType != PointButtonType_Unknown);

		UINT32 PointerId;
		hResult = pPointerPoint->get_PointerId(&PointerId);
		assert(SUCCEEDED(hResult));

		struct
		{
			uint32_t m_InputType;
			float m_Position_X;
			float m_Position_Y;
			uint32_t m_PointButtonType;
			uint32_t m_PointId;
		}InputData;
		InputData.m_InputType = InputType_PointButtonRelease;
		InputData.m_Position_X = Position.X;
		InputData.m_Position_Y = Position.Y;
		InputData.m_PointButtonType = PointButtonType;
		InputData.m_PointId = PointerId;
		m_pEventInputCallback(m_pEventInputCallback_UserData, &InputData);
	}
	return S_OK;
}

inline HRESULT STDMETHODCALLTYPE PTWWindowImpl::Invoke_Window_SizeChanged(ABI::Windows::UI::Core::ICoreWindow *pSender, ABI::Windows::UI::Core::IWindowSizeChangedEventArgs *pArgs)
{
	ABI::Windows::Foundation::Size value;
	HRESULT hResult = pArgs->get_Size(&value);
	assert(SUCCEEDED(hResult));

	if (m_pEventOutputCallback != NULL)
	{
		struct
		{
			uint32_t m_Type;
			uint32_t m_Width;
			uint32_t m_Height;
		}EventOutput_WindowResized;
		EventOutput_WindowResized.m_Type = PTWWindowImpl::PTEventOutputType_WindowResized;
		EventOutput_WindowResized.m_Width = static_cast<uint32_t>(value.Width);
		EventOutput_WindowResized.m_Height = static_cast<uint32_t>(value.Height);

		m_pEventOutputCallback(m_pEventOutputCallback_UserData, &EventOutput_WindowResized);
	}

	return S_OK;
}