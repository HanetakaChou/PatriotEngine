#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>

#include <xcb/xcb.h>
#include <X11/keysym.h>

#include <assert.h>

#include "../../../Public/McRT/PTSThread.h"

#include "PTWindowImpl.h"
#include <limits.h>

static inline struct PT_WSI_Display_T *wrap(xcb_connection_t *v) { return reinterpret_cast<struct PT_WSI_Display_T *>(v); }
static inline xcb_connection_t *unwrap(struct PT_WSI_Display_T *v) { return reinterpret_cast<xcb_connection_t *>(v); }

static inline struct PT_WSI_Window_T *wrap(xcb_window_t v) { return reinterpret_cast<PT_WSI_Window_T *>(static_cast<uintptr_t>(v)); }
static inline xcb_window_t unwrap(struct PT_WSI_Window_T *v) { return static_cast<uintptr_t>(reinterpret_cast<uintptr_t>(v)); }

void *PTInvokeMain(void *pVoid);

int main(int argc, char *argv[])
{
	xcb_connection_t *hDisplay;
	xcb_window_t hWnd;
	xcb_atom_t Atom_WMPROTOCOLS;
	xcb_atom_t Atom_WMDELETEWINDOW;
	xcb_keycode_t min_keycode;
	xcb_keycode_t max_keycode;
	{
		// Display
		int iScreenPreferred;
		//在使用IDE"Visual C++ For Linux
		// Development"进行调试时，需要手动设置启动前命令export DISPLAY=:0
		hDisplay = ::xcb_connect(NULL, &iScreenPreferred);
		assert(::xcb_connection_has_error(hDisplay) == 0);

		// Screen
		xcb_setup_t const *pDisplaySetup = ::xcb_get_setup(hDisplay);
		assert(pDisplaySetup != NULL);
		xcb_screen_iterator_t iScreen = ::xcb_setup_roots_iterator(pDisplaySetup);
		for (int i = 0; i < iScreenPreferred; ++i)
		{
			::xcb_screen_next(&iScreen);
		}

		// CreateWindowExW
		xcb_window_t XID = ::xcb_generate_id(hDisplay);
		uint32_t valuemaskCW = XCB_CW_EVENT_MASK;
		uint32_t valuelistCW[1] = {
			XCB_EVENT_MASK_KEY_PRESS |
			XCB_EVENT_MASK_KEY_RELEASE |
			XCB_EVENT_MASK_BUTTON_PRESS |
			XCB_EVENT_MASK_BUTTON_RELEASE |
			XCB_EVENT_MASK_POINTER_MOTION |
			XCB_EVENT_MASK_BUTTON_MOTION |
			XCB_EVENT_MASK_EXPOSURE |
			XCB_EVENT_MASK_STRUCTURE_NOTIFY |
			XCB_EVENT_MASK_FOCUS_CHANGE};
		xcb_void_cookie_t cookieCW = ::xcb_create_window_checked(hDisplay, XCB_COPY_FROM_PARENT, XID, iScreen.data->root, 0, 0, iScreen.data->width_in_pixels, iScreen.data->height_in_pixels, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, XCB_COPY_FROM_PARENT, valuemaskCW, valuelistCW);
		xcb_generic_error_t *pErrorCW = ::xcb_request_check(hDisplay, cookieCW); //隐式xcb_flush
		assert(pErrorCW == NULL);

		// Delete Window
		// xcb_atom_t Atom_WMPROTOCOLS;
		{
			xcb_intern_atom_cookie_t CookieAtomWMPROTOCOLS =
				::xcb_intern_atom(hDisplay, 1, 12U, "WM_PROTOCOLS");
			xcb_generic_error_t *pErrorInternAtom;
			xcb_intern_atom_reply_t *pReplyAtomWMPROTOCOLS = ::xcb_intern_atom_reply(
				hDisplay, CookieAtomWMPROTOCOLS, &pErrorInternAtom); //隐式xcb_flush
			assert(pErrorInternAtom == NULL);
			Atom_WMPROTOCOLS = pReplyAtomWMPROTOCOLS->atom;
			::free(pReplyAtomWMPROTOCOLS);
		}
		// xcb_atom_t Atom_WMDELETEWINDOW;
		{
			xcb_intern_atom_cookie_t CookieAtomWMDELETEWINDOW =
				::xcb_intern_atom(hDisplay, 0, 16U, "WM_DELETE_WINDOW");
			xcb_generic_error_t *pErrorInternAtom;
			xcb_intern_atom_reply_t *pReplyAtomWMDELETEWINDOW =
				::xcb_intern_atom_reply(hDisplay, CookieAtomWMDELETEWINDOW,
										&pErrorInternAtom); //隐式xcb_flush
			assert(pErrorInternAtom == NULL);
			Atom_WMDELETEWINDOW = pReplyAtomWMDELETEWINDOW->atom;
			::free(pReplyAtomWMDELETEWINDOW);
		}
		xcb_void_cookie_t CookieChangePropertyWMPROTOCOLS = ::xcb_change_property_checked(hDisplay, XCB_PROP_MODE_REPLACE, XID, Atom_WMPROTOCOLS, XCB_ATOM_ATOM, 32U, 1U, &Atom_WMDELETEWINDOW);
		xcb_generic_error_t *pErrorChangePropertyWMPROTOCOLS = ::xcb_request_check(hDisplay, CookieChangePropertyWMPROTOCOLS); //隐式xcb_flush
		assert(pErrorChangePropertyWMPROTOCOLS == NULL);

		// Title
		xcb_void_cookie_t CookieChangePropertyWMNAME = ::xcb_change_property_checked(hDisplay, XCB_PROP_MODE_REPLACE, XID, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8U, 13U, "PatriotEngine");
		xcb_generic_error_t *pErrorChangePropertyWMNAME = ::xcb_request_check(hDisplay, CookieChangePropertyWMNAME); //隐式xcb_flush
		assert(pErrorChangePropertyWMNAME == NULL);

		// ShowWindow
		xcb_void_cookie_t cookieMW = ::xcb_map_window_checked(hDisplay, XID);
		xcb_generic_error_t *pErrorMW =
			::xcb_request_check(hDisplay, cookieMW); //隐式xcb_flush
		assert(pErrorMW == NULL);

		min_keycode = pDisplaySetup->min_keycode;
		max_keycode = pDisplaySetup->max_keycode;

		hWnd = XID;
	}

	xcb_get_keyboard_mapping_reply_t *pReplyGetKeyboardMapping;
	{
		//https://gitlab.freedesktop.org/xorg/lib/libxcb-keysyms
		//xcb_key_symbols_alloc
		xcb_get_keyboard_mapping_cookie_t Cookie_GetKeyboardMapping = xcb_get_keyboard_mapping(hDisplay, min_keycode, (max_keycode - min_keycode) + 1);

		//https://gitlab.freedesktop.org/xorg/lib/libxcb-keysyms
		//xcb_key_symbols_get_keysym
		xcb_generic_error_t *pErrorGetKeyboardMapping;
		pReplyGetKeyboardMapping = ::xcb_get_keyboard_mapping_reply(hDisplay, Cookie_GetKeyboardMapping, &pErrorGetKeyboardMapping);
		assert(pErrorGetKeyboardMapping == NULL);
	}

	PTWWindowImpl l_WindowImpl_Singleton(argc, argv);

	PTSThread hThreadInvoke;
	bool tbResult = ::PTSThread_Create(
		&PTInvokeMain, static_cast<PTWWindowImpl *>(&l_WindowImpl_Singleton),
		&hThreadInvoke);
	assert(tbResult != false);

	// Wait for PTApp
	while (
		(::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(
			 &l_WindowImpl_Singleton.m_pEventOutputCallback_UserData)) == NULL) ||
		(::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(
			 &l_WindowImpl_Singleton.m_pEventOutputCallback)) == NULL) ||
		(::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(
			 &l_WindowImpl_Singleton.m_pEventInputCallback_UserData)) == NULL) ||
		(::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(
			 &l_WindowImpl_Singleton.m_pEventInputCallback)) == NULL))
	{
		::PTS_Yield();
	}

	xcb_generic_event_t *pGenericEvent = ::xcb_wait_for_event(hDisplay);
	while (l_WindowImpl_Singleton.m_bMessagePump &&
		   ((pGenericEvent = ::xcb_wait_for_event(hDisplay)) != NULL))
	{
		switch (pGenericEvent->response_type & ~0x80)
		{
		case XCB_KEY_PRESS:
		{
			xcb_key_press_event_t *pKeyPressEvent = reinterpret_cast<xcb_key_press_event_t *>(pGenericEvent);
			if (pKeyPressEvent->event == hWnd)
			{
				xcb_keycode_t keycode = pKeyPressEvent->detail;
				//https://gitlab.freedesktop.org/xorg/lib/libxcb-keysyms
				//xcb_key_symbols_get_keysym
				if (keycode >= min_keycode && keycode <= max_keycode)
				{
					xcb_keysym_t *pKeysymsGetKeyboardMapping = xcb_get_keyboard_mapping_keysyms(pReplyGetKeyboardMapping);
					switch (pKeysymsGetKeyboardMapping[pReplyGetKeyboardMapping->keysyms_per_keycode * (keycode - min_keycode)])
					{
					case XK_W:
					case XK_w:
					{
						// W
						int huhu = 0;
					}
					break;
					}
				}
			}
		}
		break;
		case XCB_EXPOSE:
		{
			static bool bFirstTime = true;
			if (bFirstTime)
			{
				// PTEventOutputType_WindowCreated
				{
					void(PTPTR * pHere_EventOutputCallback)(void *pUserData, void *pEventData);

					// SpinLock
					while ((pHere_EventOutputCallback = reinterpret_cast<void(PTPTR *)(
								void *pUserData, void *pEventData)>(
								::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(
									&l_WindowImpl_Singleton.m_pEventOutputCallback)))) ==
						   NULL)
					{
						::PTS_Yield();
					}

					void *pHere_EventOutputCallback_UserData = reinterpret_cast<void *>(
						::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(
							&l_WindowImpl_Singleton.m_pEventOutputCallback_UserData)));

					PT_WSI_IWindow::EventOutput_WindowCreated EventData;
					EventData.m_Type = PT_WSI_IWindow::EventOutput::Type_WindowCreated;
					EventData.m_hDisplay = wrap(hDisplay);
					EventData.m_hWindow = wrap(hWnd);

					pHere_EventOutputCallback(pHere_EventOutputCallback_UserData,
											  &EventData);
				}

				bFirstTime = false;
			}

			xcb_expose_event_t *pExposeEvent =
				reinterpret_cast<xcb_expose_event_t *>(pGenericEvent);
			if (pExposeEvent->window == hWnd)
			{
				// PT_WSI_IWindow::EventOutput_WindowResized

				void(PTPTR * pHere_EventOutputCallback)(void *pUserData,
														void *pEventData);

				// SpinLock
				while ((pHere_EventOutputCallback = reinterpret_cast<void(PTPTR *)(
							void *pUserData, void *pEventData)>(
							::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(
								&l_WindowImpl_Singleton.m_pEventOutputCallback)))) ==
					   NULL)
				{
					::PTS_Yield();
				}

				void *pHere_EventOutputCallback_UserData = reinterpret_cast<void *>(
					::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(
						&l_WindowImpl_Singleton.m_pEventOutputCallback_UserData)));

				PT_WSI_IWindow::EventOutput_WindowResized EventData;
				EventData.m_Type = PT_WSI_IWindow::EventOutput::Type_WindowResized;
				EventData.m_hDisplay = wrap(hDisplay);
				EventData.m_hWindow = wrap(hWnd);
				EventData.m_Width = static_cast<uint32_t>(pExposeEvent->width);
				EventData.m_Height = static_cast<uint32_t>(pExposeEvent->height);

				pHere_EventOutputCallback(pHere_EventOutputCallback_UserData,
										  &EventData);
			}
		}
		break;
		case XCB_CONFIGURE_NOTIFY:
		{
			xcb_configure_notify_event_t *pConfigureNotifyEvent = reinterpret_cast<xcb_configure_notify_event_t *>(pGenericEvent);
			if (pConfigureNotifyEvent->window == hWnd)
			{
				PT_WSI_IWindow::EventOutput_WindowResized EventData;
				EventData.m_Type = PT_WSI_IWindow::EventOutput::Type_WindowResized;
				EventData.m_hDisplay = wrap(hDisplay);
				EventData.m_hWindow = wrap(hWnd);
				EventData.m_Width = static_cast<uint32_t>(pConfigureNotifyEvent->width);
				EventData.m_Height = static_cast<uint32_t>(pConfigureNotifyEvent->height);

				l_WindowImpl_Singleton.m_pEventOutputCallback(l_WindowImpl_Singleton.m_pEventOutputCallback_UserData, &EventData);
			}
		}
		break;
		case XCB_CLIENT_MESSAGE:
		{
			xcb_client_message_event_t *pClientMessageEvent =
				reinterpret_cast<xcb_client_message_event_t *>(pGenericEvent);
			// WM_DESTROY
			if (pClientMessageEvent->window == hWnd &&
				pClientMessageEvent->type == Atom_WMPROTOCOLS &&
				pClientMessageEvent->format == 32U &&
				pClientMessageEvent->data.data32[0] == Atom_WMDELETEWINDOW)
			{
				l_WindowImpl_Singleton.m_bMessagePump = false;
			}
		}
		break;
		case XCB_MAPPING_NOTIFY:
		{
			xcb_mapping_notify_event_t *pMappingNotifyEvent = reinterpret_cast<xcb_mapping_notify_event_t *>(pGenericEvent);

			//https://gitlab.freedesktop.org/xorg/lib/libxcb-keysyms
			//xcb_refresh_keyboard_mapping
			if (pMappingNotifyEvent->request == XCB_MAPPING_KEYBOARD)
			{
				xcb_setup_t const *pDisplaySetup = ::xcb_get_setup(hDisplay);
				min_keycode = pDisplaySetup->min_keycode;
				max_keycode = pDisplaySetup->max_keycode;

				xcb_get_keyboard_mapping_cookie_t Cookie_GetKeyboardMapping = xcb_get_keyboard_mapping(hDisplay, min_keycode, (max_keycode - min_keycode) + 1);

				::free(pReplyGetKeyboardMapping);

				xcb_generic_error_t *pErrorGetKeyboardMapping;
				pReplyGetKeyboardMapping = ::xcb_get_keyboard_mapping_reply(hDisplay, Cookie_GetKeyboardMapping, &pErrorGetKeyboardMapping);
				assert(pErrorGetKeyboardMapping == NULL);
			}
		}
		break;
		default:
			// Unknown event type, ignore it
			break;
		}

		::free(pGenericEvent); // xcb频繁访问堆——低效的！！！
	}

	//Window Destroyed
	{
		PT_WSI_IWindow::EventOutput_WindowDestroyed EventData;
		EventData.m_Type = PT_WSI_IWindow::EventOutput::Type_WindowDestroyed;

		l_WindowImpl_Singleton.m_pEventOutputCallback(l_WindowImpl_Singleton.m_pEventOutputCallback_UserData, &EventData);
	}

	//确保栈中的内存 PTInvokeParam ParamInvoke
	//在PTInvokeMain的整个生命期内是有效的
	tbResult = ::PTSThread_Join(&hThreadInvoke);
	assert(tbResult != false);

	::free(pReplyGetKeyboardMapping);

	::xcb_disconnect(hDisplay);

	return 0;
}

inline PTWWindowImpl::PTWWindowImpl(int argc, char *argv[])
{
	// EventOutputCallback
	m_pEventOutputCallback = NULL;
	m_pEventOutputCallback_UserData = NULL;
	// EventInputCallback
	m_pEventInputCallback = NULL;
	m_pEventInputCallback_UserData = NULL;
	// TermminateMessagePump
	m_bMessagePump = true;
	m_Argc_Cache = argc;
	m_Argv_Cache = argv;
}

inline PTWWindowImpl::~PTWWindowImpl() {}

void PTWWindowImpl::EventOutputCallback_Hook(
	void *pUserData,
	void(PTPTR *pEventOutputCallback)(void *pUserData, void *pOutputData))
{
	assert(m_pEventOutputCallback_UserData == NULL);
	assert(m_pEventOutputCallback == NULL);
	m_pEventOutputCallback_UserData = pUserData;
	m_pEventOutputCallback = pEventOutputCallback;
}

void PTWWindowImpl::EventInputCallback_Hook(
	void *pUserData,
	void(PTPTR *pEventInputCallback)(void *pUserData, void *pInputData))
{
	assert(m_pEventInputCallback_UserData == NULL);
	assert(m_pEventInputCallback == NULL);
	m_pEventInputCallback_UserData = pUserData;
	m_pEventInputCallback = pEventInputCallback;
}

void PTWWindowImpl::Parent_Set(struct PT_WSI_Window_T *hWindowParent)
{
	assert(0);
}

void PTWWindowImpl::Position_Set(uint32_t TopLeftX, uint32_t TopLeftY)
{
	assert(0);
}

void PTWWindowImpl::Size_Set(uint32_t Width, uint32_t Height)
{
	assert(0);
}

void PTWWindowImpl::TermminateMessagePump()
{
	m_bMessagePump = false;
}

#include "../../../Public/APP/PT_APP_Export.h"

void *PTInvokeMain(void *pVoid)
{
	PTWWindowImpl *pWindow = static_cast<PTWWindowImpl *>(pVoid);

	int iResult = ::PTAMain(static_cast<PT_WSI_IWindow *>(pWindow),
							pWindow->m_Argc_Cache, pWindow->m_Argv_Cache);
	assert(iResult == 0);
	for (int i = 0; i < 6666; ++i)
	{
		sched_yield();
	}

	//pWindow->TermminateMessagePump();

	return NULL;
}