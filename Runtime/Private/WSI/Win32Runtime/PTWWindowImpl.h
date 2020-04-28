#include "../../../Public/Window/PTWWindow.h"
#include "../../../Public/McRT/PTSThread.h"

#include <Windows.ApplicationModel.core.h>

#if 1
struct PTNativeInvoke_ApplicationView_Activated : public ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::ApplicationModel::Core::CoreApplicationView*, ABI::Windows::ApplicationModel::Activation::IActivatedEventArgs*>
{
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) override;
	HRESULT STDMETHODCALLTYPE Invoke(ABI::Windows::ApplicationModel::Core::ICoreApplicationView *, ABI::Windows::ApplicationModel::Activation::IActivatedEventArgs*) override;
};
struct PTNativeInvoke_Window_KeyDown :public ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Core::CoreWindow*, ABI::Windows::UI::Core::KeyEventArgs*>
{
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) override;
	HRESULT STDMETHODCALLTYPE Invoke(ABI::Windows::UI::Core::ICoreWindow*, ABI::Windows::UI::Core::IKeyEventArgs*) override;
};
struct PTNativeInvoke_Window_KeyUp :public ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Core::CoreWindow*, ABI::Windows::UI::Core::KeyEventArgs*>
{
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) override;
	HRESULT STDMETHODCALLTYPE Invoke(ABI::Windows::UI::Core::ICoreWindow*, ABI::Windows::UI::Core::IKeyEventArgs*) override;
};
struct PTNativeInvoke_Window_PointerMoved :public ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Core::CoreWindow*, ABI::Windows::UI::Core::PointerEventArgs*>
{
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) override;
	HRESULT STDMETHODCALLTYPE Invoke(ABI::Windows::UI::Core::ICoreWindow*, ABI::Windows::UI::Core::IPointerEventArgs*) override;
};
struct PTNativeInvoke_Window_PointerPressed :public ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Core::CoreWindow*, ABI::Windows::UI::Core::PointerEventArgs*>
{
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) override;
	HRESULT STDMETHODCALLTYPE Invoke(ABI::Windows::UI::Core::ICoreWindow*, ABI::Windows::UI::Core::IPointerEventArgs*) override;
};
struct PTNativeInvoke_Window_PointerReleased :public ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Core::CoreWindow*, ABI::Windows::UI::Core::PointerEventArgs*>
{
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) override;
	HRESULT STDMETHODCALLTYPE Invoke(ABI::Windows::UI::Core::ICoreWindow*, ABI::Windows::UI::Core::IPointerEventArgs*) override;
};
struct PTNativeInvoke_Window_SizeChanged :public ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Core::CoreWindow*, ABI::Windows::UI::Core::WindowSizeChangedEventArgs*>
{
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) override;
	HRESULT STDMETHODCALLTYPE Invoke(ABI::Windows::UI::Core::ICoreWindow*, ABI::Windows::UI::Core::IWindowSizeChangedEventArgs*) override;
};
#endif

class PTWWindowImpl :
	public PT_WSI_IWindow,
	public ABI::Windows::ApplicationModel::Core::IFrameworkViewSource,
	public ABI::Windows::ApplicationModel::Core::IFrameworkView,
	public PTNativeInvoke_ApplicationView_Activated,
	public PTNativeInvoke_Window_KeyDown,
	public PTNativeInvoke_Window_KeyUp,
	public PTNativeInvoke_Window_PointerMoved,
	public PTNativeInvoke_Window_PointerPressed,
	public PTNativeInvoke_Window_PointerReleased,
	public PTNativeInvoke_Window_SizeChanged
{
	//IPTWindow
	void EventOutputCallback_Hook(void *pUserData, void(PTPTR *pOutputEventCallback)(void *pUserData, void *pEventData)) override;
	void EventInputCallback_Hook(void *pUserData, void(PTPTR *pInputCallback)(void *pUserData, void *pEventData)) override;
	void Parent_Set(struct PT_WSI_Window_T * hWindowParent) override;
	void Position_Set(uint32_t TopLeftX, uint32_t TopLeftY) override;
	void Size_Set(uint32_t Width, uint32_t Height) override;

	//ASTA�̵߳���
	//PTWWindowImpl
	void * volatile m_pEventOutputCallback_UserData;
	void (PTPTR * volatile m_pEventOutputCallback)(void *pUserData, void *pEventData);
	void * volatile m_pEventInputCallback_UserData;
	void (PTPTR * volatile m_pEventInputCallback)(void *pUserData, void *pEventData);

	//#include <CoreWindow.h>
	//ICoreWindowInterop
	ABI::Windows::UI::Core::ICoreWindow * volatile m_hWindow_Cache;

public:
	//wWinMain�̵߳���
	inline PTWWindowImpl();
	inline ~PTWWindowImpl();

	//Cache CmdLine
	wchar_t *m_pCmdLine;

	//PTInvokeMain�̵߳���
	void TermminateMessagePump();

private:
	//IUnknown
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) override;
	ULONG STDMETHODCALLTYPE AddRef(void) override;
	ULONG STDMETHODCALLTYPE Release(void) override;
	//IInspectable
	HRESULT STDMETHODCALLTYPE GetIids(ULONG *iidCount, IID **iids) override;
	HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING *className) override;
	HRESULT STDMETHODCALLTYPE GetTrustLevel(TrustLevel* trustLevel) override;
	//IFrameworkViewSource
	HRESULT STDMETHODCALLTYPE CreateView(ABI::Windows::ApplicationModel::Core::IFrameworkView **viewProvider) override;

	//PTInvokeMain�߳�
	PTSThread m_hThreadInvoke;

	//IFrameworkView
	HRESULT STDMETHODCALLTYPE Initialize(ABI::Windows::ApplicationModel::Core::ICoreApplicationView *pApplicationView) override;
	HRESULT STDMETHODCALLTYPE SetWindow(ABI::Windows::UI::Core::ICoreWindow *pWindow) override;
	HRESULT STDMETHODCALLTYPE Load(HSTRING entryPoint) override;
	HRESULT STDMETHODCALLTYPE Run(void) override;
	HRESULT STDMETHODCALLTYPE Uninitialize(void) override;

	//ITypedEventHandler
	friend HRESULT PTNativeInvoke_ApplicationView_Activated::Invoke(ABI::Windows::ApplicationModel::Core::ICoreApplicationView *, ABI::Windows::ApplicationModel::Activation::IActivatedEventArgs*);
	inline HRESULT STDMETHODCALLTYPE Invoke_ApplicationView_Activated(ABI::Windows::ApplicationModel::Core::ICoreApplicationView *pSender, ABI::Windows::ApplicationModel::Activation::IActivatedEventArgs *pArgs);
	
	inline static uint32_t RTKeyType_To_PTKeyType(ABI::Windows::System::VirtualKey Value);
	inline static uint32_t RTKeyType_To_PTPointButtonType(ABI::Windows::System::VirtualKey Value);

	friend HRESULT PTNativeInvoke_Window_KeyDown::Invoke(ABI::Windows::UI::Core::ICoreWindow*, ABI::Windows::UI::Core::IKeyEventArgs*);
	inline HRESULT STDMETHODCALLTYPE Invoke_Window_KeyDown(ABI::Windows::UI::Core::ICoreWindow *pSender, ABI::Windows::UI::Core::IKeyEventArgs *pArgs);
	friend HRESULT PTNativeInvoke_Window_KeyUp::Invoke(ABI::Windows::UI::Core::ICoreWindow*, ABI::Windows::UI::Core::IKeyEventArgs*);
	inline HRESULT STDMETHODCALLTYPE Invoke_Window_KeyUp(ABI::Windows::UI::Core::ICoreWindow *pSender, ABI::Windows::UI::Core::IKeyEventArgs *pArgs);
	
	friend HRESULT PTNativeInvoke_Window_PointerMoved::Invoke(ABI::Windows::UI::Core::ICoreWindow*, ABI::Windows::UI::Core::IPointerEventArgs*);
	inline HRESULT STDMETHODCALLTYPE Invoke_Window_PointerMoved(ABI::Windows::UI::Core::ICoreWindow *pSender, ABI::Windows::UI::Core::IPointerEventArgs *pArgs);
	
	inline static uint32_t RTPointPressButtonType_To_PTPointButtonType(ABI::Windows::UI::Input::PointerUpdateKind Value);
	inline static uint32_t RTPointReleaseButtonType_To_PTPointButtonType(ABI::Windows::UI::Input::PointerUpdateKind Value);

	friend HRESULT PTNativeInvoke_Window_PointerPressed::Invoke(ABI::Windows::UI::Core::ICoreWindow*, ABI::Windows::UI::Core::IPointerEventArgs*);
	inline HRESULT STDMETHODCALLTYPE Invoke_Window_PointerPressed(ABI::Windows::UI::Core::ICoreWindow *pSender, ABI::Windows::UI::Core::IPointerEventArgs *pArgs);
	friend HRESULT PTNativeInvoke_Window_PointerReleased::Invoke(ABI::Windows::UI::Core::ICoreWindow*, ABI::Windows::UI::Core::IPointerEventArgs*);
	inline HRESULT STDMETHODCALLTYPE Invoke_Window_PointerReleased(ABI::Windows::UI::Core::ICoreWindow *pSender, ABI::Windows::UI::Core::IPointerEventArgs *pArgs);
	friend HRESULT PTNativeInvoke_Window_SizeChanged::Invoke(ABI::Windows::UI::Core::ICoreWindow*, ABI::Windows::UI::Core::IWindowSizeChangedEventArgs*);
	inline HRESULT STDMETHODCALLTYPE Invoke_Window_SizeChanged(ABI::Windows::UI::Core::ICoreWindow *pSender, ABI::Windows::UI::Core::IWindowSizeChangedEventArgs *pArgs);	
};

#if 1
HRESULT STDMETHODCALLTYPE PTNativeInvoke_ApplicationView_Activated::QueryInterface(REFIID riid, void **ppvObject)
{
	if (riid == __uuidof(IUnknown) || riid == __uuidof(ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::ApplicationModel::Core::CoreApplicationView*, ABI::Windows::ApplicationModel::Activation::IActivatedEventArgs*>))
	{
		*ppvObject = static_cast<ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::ApplicationModel::Core::CoreApplicationView*, ABI::Windows::ApplicationModel::Activation::IActivatedEventArgs*> *>(this);
		return S_OK;
	}
	else
	{
		return E_NOINTERFACE;
	}
}
HRESULT STDMETHODCALLTYPE PTNativeInvoke_ApplicationView_Activated::Invoke(ABI::Windows::ApplicationModel::Core::ICoreApplicationView *pSender, ABI::Windows::ApplicationModel::Activation::IActivatedEventArgs *pArgs)
{
	return static_cast<PTWWindowImpl *>(this)->Invoke_ApplicationView_Activated(pSender, pArgs);
}
HRESULT STDMETHODCALLTYPE PTNativeInvoke_Window_KeyDown::QueryInterface(REFIID riid, void **ppvObject)
{
	if (riid == __uuidof(IUnknown) || riid == __uuidof(ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Core::CoreWindow*, ABI::Windows::UI::Core::KeyEventArgs*>))
	{
		*ppvObject = static_cast<ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Core::CoreWindow*, ABI::Windows::UI::Core::KeyEventArgs*> *>(this);
		return S_OK;
	}
	else
	{
		return E_NOINTERFACE;
	}
}
HRESULT STDMETHODCALLTYPE PTNativeInvoke_Window_KeyDown::Invoke(ABI::Windows::UI::Core::ICoreWindow *pSender, ABI::Windows::UI::Core::IKeyEventArgs *pArgs)
{
	return static_cast<PTWWindowImpl *>(this)->Invoke_Window_KeyDown(pSender, pArgs);
}
HRESULT STDMETHODCALLTYPE PTNativeInvoke_Window_KeyUp::QueryInterface(REFIID riid, void **ppvObject)
{
	if (riid == __uuidof(IUnknown) || riid == __uuidof(ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Core::CoreWindow*, ABI::Windows::UI::Core::KeyEventArgs*>))
	{
		*ppvObject = static_cast<ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Core::CoreWindow*, ABI::Windows::UI::Core::KeyEventArgs*> *>(this);
		return S_OK;
	}
	else
	{
		return E_NOINTERFACE;
	}
}
HRESULT STDMETHODCALLTYPE PTNativeInvoke_Window_KeyUp::Invoke(ABI::Windows::UI::Core::ICoreWindow *pSender, ABI::Windows::UI::Core::IKeyEventArgs *pArgs)
{
	return static_cast<PTWWindowImpl *>(this)->Invoke_Window_KeyUp(pSender, pArgs);
}
HRESULT STDMETHODCALLTYPE PTNativeInvoke_Window_PointerMoved::QueryInterface(REFIID riid, void **ppvObject)
{
	if (riid == __uuidof(IUnknown) || riid == __uuidof(ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Core::CoreWindow*, ABI::Windows::UI::Core::PointerEventArgs*>))
	{
		*ppvObject = static_cast<ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Core::CoreWindow*, ABI::Windows::UI::Core::PointerEventArgs*> *>(this);
		return S_OK;
	}
	else
	{
		return E_NOINTERFACE;
	}
}
HRESULT STDMETHODCALLTYPE PTNativeInvoke_Window_PointerMoved::Invoke(ABI::Windows::UI::Core::ICoreWindow *pSender, ABI::Windows::UI::Core::IPointerEventArgs *pArgs)
{
	return static_cast<PTWWindowImpl *>(this)->Invoke_Window_PointerMoved(pSender, pArgs);
}
HRESULT STDMETHODCALLTYPE PTNativeInvoke_Window_PointerPressed::QueryInterface(REFIID riid, void **ppvObject)
{
	if (riid == __uuidof(IUnknown) || riid == __uuidof(ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Core::CoreWindow*, ABI::Windows::UI::Core::PointerEventArgs*>))
	{
		*ppvObject = static_cast<ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Core::CoreWindow*, ABI::Windows::UI::Core::PointerEventArgs*> *>(this);
		return S_OK;
	}
	else
	{
		return E_NOINTERFACE;
	}
}
HRESULT STDMETHODCALLTYPE PTNativeInvoke_Window_PointerPressed::Invoke(ABI::Windows::UI::Core::ICoreWindow *pSender, ABI::Windows::UI::Core::IPointerEventArgs *pArgs)
{
	return static_cast<PTWWindowImpl *>(this)->Invoke_Window_PointerPressed(pSender, pArgs);
}
HRESULT STDMETHODCALLTYPE PTNativeInvoke_Window_PointerReleased::QueryInterface(REFIID riid, void **ppvObject)
{
	if (riid == __uuidof(IUnknown) || riid == __uuidof(ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Core::CoreWindow*, ABI::Windows::UI::Core::PointerEventArgs*>))
	{
		*ppvObject = static_cast<ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Core::CoreWindow*, ABI::Windows::UI::Core::PointerEventArgs*> *>(this);
		return S_OK;
	}
	else
	{
		return E_NOINTERFACE;
	}
}
HRESULT STDMETHODCALLTYPE PTNativeInvoke_Window_PointerReleased::Invoke(ABI::Windows::UI::Core::ICoreWindow *pSender, ABI::Windows::UI::Core::IPointerEventArgs *pArgs)
{
	return static_cast<PTWWindowImpl *>(this)->Invoke_Window_PointerReleased(pSender, pArgs);
}
HRESULT STDMETHODCALLTYPE PTNativeInvoke_Window_SizeChanged::QueryInterface(REFIID riid, void **ppvObject)
{
	if (riid == __uuidof(IUnknown) || riid == __uuidof(ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Core::CoreWindow*, ABI::Windows::UI::Core::WindowSizeChangedEventArgs*>))
	{
		*ppvObject = static_cast<ABI::Windows::Foundation::ITypedEventHandler<ABI::Windows::UI::Core::CoreWindow*, ABI::Windows::UI::Core::WindowSizeChangedEventArgs*> *>(this);
		return S_OK;
	}
	else
	{
		return E_NOINTERFACE;
	}
}
HRESULT STDMETHODCALLTYPE PTNativeInvoke_Window_SizeChanged::Invoke(ABI::Windows::UI::Core::ICoreWindow *pSender, ABI::Windows::UI::Core::IWindowSizeChangedEventArgs *pArgs)
{
	return static_cast<PTWWindowImpl *>(this)->Invoke_Window_SizeChanged(pSender, pArgs);
}
#endif