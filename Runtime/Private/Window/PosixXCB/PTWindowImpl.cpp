#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>

#include <string.h>
#include <stdlib.h>

#include <xcb/xcb_keysyms.h>
#include <X11/keysym.h>

#include <assert.h>

#include "../../../Public/System/PTSThread.h"

#include "PTWindowImpl.h"
#include <limits.h>

void * PTInvokeMain(void *pVoid);

int main(int argc, char *argv[])
{		
	xcb_connection_t *hDisplay;
	xcb_window_t hWnd;
#if 0
	xcb_atom_t AtomWMPROTOCOLS;
	xcb_atom_t AtomWMDELETEWINDOW;
#endif
	{
		//Display
		int iScreenPreferred;
		//在使用IDE"Visual C++ For Linux Development"进行调试时，需要手动设置启动前命令export DISPLAY=:0
		hDisplay = ::xcb_connect(NULL, &iScreenPreferred);
		assert(::xcb_connection_has_error(hDisplay) == 0);
		
		//Screen
		const xcb_setup_t *pDisplaySetup = ::xcb_get_setup(hDisplay);
		assert(pDisplaySetup != NULL);
		xcb_screen_iterator_t iScreen = ::xcb_setup_roots_iterator(pDisplaySetup);
		for (int i = 0; i < iScreenPreferred; ++i)
		{
			::xcb_screen_next(&iScreen);
		}

		//CreateWindowExW
		xcb_window_t XID = ::xcb_generate_id(hDisplay);
		uint32_t valuemaskCW = XCB_CW_EVENT_MASK;
		uint32_t valuelistCW[1] = { XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY };
		xcb_void_cookie_t cookieCW = ::xcb_create_window_checked(hDisplay, XCB_COPY_FROM_PARENT, XID, iScreen.data->root, 0, 0, iScreen.data->width_in_pixels, iScreen.data->height_in_pixels, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, XCB_COPY_FROM_PARENT, valuemaskCW, valuelistCW);
		xcb_generic_error_t *pErrorCW = ::xcb_request_check(hDisplay, cookieCW);//隐式xcb_flush
		assert(pErrorCW == NULL);

		//Title
		xcb_atom_t AtomUTF8STRING;
		{
			xcb_intern_atom_cookie_t CookieAtomUTF8STRING = ::xcb_intern_atom(hDisplay, false, 11U, "UTF8_STRING");
			xcb_generic_error_t *pErrorInternAtom;
			xcb_intern_atom_reply_t *pReplyAtomUTF8STRING = ::xcb_intern_atom_reply(hDisplay, CookieAtomUTF8STRING, &pErrorInternAtom);//隐式xcb_flush
			assert(pErrorInternAtom == NULL);
			AtomUTF8STRING = pReplyAtomUTF8STRING->atom;
			::free(pReplyAtomUTF8STRING);
		}
		xcb_atom_t AtomWMNAME;
		{
			xcb_intern_atom_cookie_t CookieAtomWMNAME = ::xcb_intern_atom(hDisplay, false, 7U, "WM_NAME");
			xcb_generic_error_t *pErrorInternAtom;
			xcb_intern_atom_reply_t *pReplyAtomWMNAME = ::xcb_intern_atom_reply(hDisplay, CookieAtomWMNAME, &pErrorInternAtom);
			assert(pErrorInternAtom == NULL);
			AtomWMNAME = pReplyAtomWMNAME->atom;
			::free(pReplyAtomWMNAME);
		}
		xcb_void_cookie_t CookieChangePropertyWMNAME =::xcb_change_property_checked(hDisplay, XCB_PROP_MODE_REPLACE, XID, AtomWMNAME, AtomUTF8STRING, 8U, 13U, "PatriotEngine");
		xcb_generic_error_t *pErrorChangePropertyWMNAME = ::xcb_request_check(hDisplay, CookieChangePropertyWMNAME);//隐式xcb_flush
		assert(pErrorChangePropertyWMNAME == NULL);

#if 0
		//HookDeleteWindow
		//xcb_atom_t AtomWMPROTOCOLS;
		{
			xcb_intern_atom_cookie_t CookieAtomWMPROTOCOLS = ::xcb_intern_atom(hDisplay, false, 12U, "WM_PROTOCOLS");
			xcb_generic_error_t *pErrorInternAtom;
			xcb_intern_atom_reply_t *pReplyAtomWMPROTOCOLS = ::xcb_intern_atom_reply(hDisplay, CookieAtomWMPROTOCOLS, &pErrorInternAtom);//隐式xcb_flush
			assert(pErrorInternAtom == NULL);
			AtomWMPROTOCOLS = pReplyAtomWMPROTOCOLS->atom;
			::free(pReplyAtomWMPROTOCOLS);
		}
		//xcb_atom_t AtomWMDELETEWINDOW;
		{
			xcb_intern_atom_cookie_t CookieAtomWMDELETEWINDOW = ::xcb_intern_atom(hDisplay, false, 16U, "WM_DELETE_WINDOW");
			xcb_generic_error_t *pErrorInternAtom;
			xcb_intern_atom_reply_t *pReplyAtomWMDELETEWINDOW = ::xcb_intern_atom_reply(hDisplay, CookieAtomWMDELETEWINDOW, &pErrorInternAtom);//隐式xcb_flush
			assert(pErrorInternAtom == NULL);
			AtomWMDELETEWINDOW = pReplyAtomWMDELETEWINDOW->atom;
			::free(pReplyAtomWMDELETEWINDOW);
		}
		xcb_void_cookie_t CookieChangePropertyWMDELETEWINDOW = ::xcb_change_property_checked(hDisplay, XCB_PROP_MODE_REPLACE, XID, AtomWMPROTOCOLS, XCB_ATOM_ATOM, 32U, 1U, &AtomWMDELETEWINDOW);
		xcb_generic_error_t *pErrorChangePropertyWMDELETEWINDOW = ::xcb_request_check(hDisplay, CookieChangePropertyWMDELETEWINDOW);//隐式xcb_flush
		assert(pErrorChangePropertyWMDELETEWINDOW == NULL);
#endif

		//ShowWindow
		xcb_void_cookie_t cookieMW = ::xcb_map_window_checked(hDisplay, XID);
		xcb_generic_error_t *pErrorMW = ::xcb_request_check(hDisplay, cookieMW);//隐式xcb_flush
		assert(pErrorMW == NULL);

		hWnd = XID;
	}

	xcb_key_symbols_t * pKeySymbolTable = ::xcb_key_symbols_alloc(hDisplay);
	assert(pKeySymbolTable != NULL);

	PTWWindowImpl l_WindowImpl_Singleton;
	l_WindowImpl_Singleton.m_hDisplay_Cache = hDisplay;
	l_WindowImpl_Singleton.m_Argc_Cache = argc;
	l_WindowImpl_Singleton.m_Argv_Cache = argv;

	PTSThread hThreadInvoke;
	PTBOOL tbResult = ::PTSThread_Create(&PTInvokeMain, static_cast<PTWWindowImpl *>(&l_WindowImpl_Singleton), &hThreadInvoke);
	assert(tbResult != PTFALSE);

	xcb_generic_event_t *pGenericEvent = ::xcb_wait_for_event(hDisplay);
	while ((pGenericEvent = ::xcb_wait_for_event(hDisplay)) != NULL)
	{
		switch (pGenericEvent->response_type)
		{
		case XCB_KEY_PRESS:
		{
			xcb_key_press_event_t *pKeyPressEvent = reinterpret_cast<xcb_key_press_event_t *>(pGenericEvent);
			if (pKeyPressEvent->event == hWnd)
			{
				switch (::xcb_key_symbols_get_keysym(pKeySymbolTable, pKeyPressEvent->detail, 0))
				{
				case XK_W:
				case XK_w:
				{
					//W
				}
				break;
				}
			}
		}
		break;
		case XCB_EXPOSE:
		{
			static bool bFirstTime = true;
			if (bFirstTime)
			{
				//PTEventOutputType_WindowCreated
				{
					void (PTPTR * pHere_EventOutputCallback)(void *pUserData, void *pEventData);

					//SpinLock
					while ((pHere_EventOutputCallback = reinterpret_cast<void (PTPTR *)(void *pUserData, void *pEventData)>(
						::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(&l_WindowImpl_Singleton.m_pEventOutputCallback))
						)) == NULL)
					{
						::PTS_Yield();
					}

					void *pHere_EventOutputCallback_UserData = reinterpret_cast<void *>(
						::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(&l_WindowImpl_Singleton.m_pEventOutputCallback_UserData))
						);

					IPTWWindow::EventOutput_WindowCreated EventData;
					EventData.m_Type = IPTWWindow::EventOutput::Type_WindowCreated;
					EventData.m_hDisplay = hDisplay;
					EventData.m_hWindow = hWnd;

					pHere_EventOutputCallback(pHere_EventOutputCallback_UserData, &EventData);
				}

				bFirstTime = false;
			}

			xcb_expose_event_t *pExposeEvent = reinterpret_cast<xcb_expose_event_t *>(pGenericEvent);
			if (pExposeEvent->window == hWnd)
			{
				//IPTWWindow::EventOutput_WindowResized

				void (PTPTR * pHere_EventOutputCallback)(void *pUserData, void *pEventData);

				//SpinLock
				while ((pHere_EventOutputCallback = reinterpret_cast<void (PTPTR *)(void *pUserData, void *pEventData)>(
					::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(&l_WindowImpl_Singleton.m_pEventOutputCallback))
					)) == NULL)
				{
					::PTS_Yield();
				}

				void *pHere_EventOutputCallback_UserData = reinterpret_cast<void *>(
					::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(&l_WindowImpl_Singleton.m_pEventOutputCallback_UserData))
					);

				IPTWWindow::EventOutput_WindowResized EventData;
				EventData.m_Type = IPTWWindow::EventOutput::Type_WindowResized;
				EventData.m_hDisplay = hDisplay;
				EventData.m_hWindow = hWnd;
				EventData.m_Width = static_cast<uint32_t>(pExposeEvent->width);
				EventData.m_Height = static_cast<uint32_t>(pExposeEvent->height);

				pHere_EventOutputCallback(pHere_EventOutputCallback_UserData, &EventData);
			}
		}
		break;
#if 0
		case XCB_CONFIGURE_NOTIFY:
		{
			xcb_configure_notify_event_t *pConfigureNotifyEvent = reinterpret_cast<xcb_configure_notify_event_t *>(pGenericEvent);
			if (pConfigureNotifyEvent->window == hWnd)
			{
				//IPTWWindow::EventOutput_WindowResized

				void (PTPTR * pHere_EventOutputCallback)(void *pUserData, void *pEventData);

				//SpinLock
				while ((pHere_EventOutputCallback = reinterpret_cast<void (PTPTR *)(void *pUserData, void *pEventData)>(
					::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(&l_WindowImpl_Singleton.m_pEventOutputCallback))
					)) == NULL)
				{
					::PTS_Yield();
				}

				void *pHere_EventOutputCallback_UserData = reinterpret_cast<void *>(
					::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(&l_WindowImpl_Singleton.m_pEventOutputCallback_UserData))
					);

				IPTWWindow::EventOutput_WindowResized EventData;
				EventData.m_Type = IPTWWindow::EventOutput::Type_WindowResized;
				EventData.m_hDisplay = hDisplay;
				EventData.m_hWindow = hWnd;
				EventData.m_Width = static_cast<uint32_t>(pConfigureNotifyEvent->width);
				EventData.m_Height = static_cast<uint32_t>(pConfigureNotifyEvent->height);

				pHere_EventOutputCallback(pHere_EventOutputCallback_UserData, &EventData);
			}
		}
		break;
		case XCB_CLIENT_MESSAGE:
		{
			xcb_client_message_event_t *pClientMessageEvent = reinterpret_cast<xcb_client_message_event_t *>(pGenericEvent);
			if (pClientMessageEvent->window == hWnd)
			{
				if (pClientMessageEvent->format == 32U && pClientMessageEvent->type == AtomWMPROTOCOLS && pClientMessageEvent->data.data32[0] == AtomWMDELETEWINDOW)
				{
					//Window Manager 关闭对端套接字
					int fd = ::xcb_get_file_descriptor(hDisplay);
					int iResult = ::shutdown(fd, SHUT_WR);
					assert(iResult == 0);
				}
			}
		}
#endif
		case XCB_MAPPING_NOTIFY:
		{
			xcb_mapping_notify_event_t *pMappingNotifyEvent = reinterpret_cast<xcb_mapping_notify_event_t *>(pGenericEvent);
			int iResult = ::xcb_refresh_keyboard_mapping(pKeySymbolTable, pMappingNotifyEvent);
			assert(iResult == 0);
		}
		break;
		default:
			//Unknown event type, ignore it
			break;
		}
		::free(pGenericEvent);//xcb频繁访问堆——低效的！！！
	}

	{
		//IPTWWindow::EventOutput_WindowResized

		void (PTPTR * pHere_EventOutputCallback)(void *pUserData, void *pEventData);

		//SpinLock
		while ((pHere_EventOutputCallback = reinterpret_cast<void (PTPTR *)(void *pUserData, void *pEventData)>(
			::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(&l_WindowImpl_Singleton.m_pEventOutputCallback))
			)) == NULL)
		{
			::PTS_Yield();
		}

		void *pHere_EventOutputCallback_UserData = reinterpret_cast<void *>(
			::PTSAtomic_Get(reinterpret_cast<uintptr_t volatile *>(&l_WindowImpl_Singleton.m_pEventOutputCallback_UserData))
			);

		IPTWWindow::EventOutput_WindowResized EventData;
		EventData.m_Type = PTWWindowImpl::IPTWWindow::EventOutput::Type_WindowDestroyed;

		pHere_EventOutputCallback(pHere_EventOutputCallback_UserData, &EventData);
	}

	//确保栈中的内存 PTInvokeParam ParamInvoke 在PTInvokeMain的整个生命期内是有效的
	tbResult = ::PTSThread_Join(&hThreadInvoke);
	assert(tbResult != PTFALSE);

	::xcb_key_symbols_free(pKeySymbolTable);

	::xcb_disconnect(hDisplay);
	
	return 0;
}

inline PTWWindowImpl::PTWWindowImpl()
{
	//EventOutputCallback
	m_pEventOutputCallback = NULL;
	m_pEventOutputCallback_UserData = NULL;
	//EventInputCallback
	m_pEventInputCallback = NULL;
	m_pEventInputCallback_UserData = NULL;
	//TermminateMessagePump
	m_hDisplay_Cache = NULL;
}

inline PTWWindowImpl::~PTWWindowImpl()
{

}

void PTWWindowImpl::EventOutputCallback_Hook(void *pUserData, void(PTPTR *pEventOutputCallback)(void *pUserData, void *pOutputData))
{
	::PTSAtomic_Set(reinterpret_cast<uintptr_t volatile *>(&m_pEventOutputCallback_UserData), reinterpret_cast<uintptr_t>(pUserData));
	::PTSAtomic_Set(reinterpret_cast<uintptr_t volatile *>(&m_pEventOutputCallback), reinterpret_cast<uintptr_t>(pEventOutputCallback));
}

void PTWWindowImpl::EventInputCallback_Hook(void *pUserData, void(PTPTR *pEventInputCallback)(void *pUserData, void *pInputData))
{
	::PTSAtomic_Set(reinterpret_cast<uintptr_t volatile *>(&m_pEventInputCallback_UserData), reinterpret_cast<uintptr_t>(pUserData));
	::PTSAtomic_Set(reinterpret_cast<uintptr_t volatile *>(&m_pEventInputCallback), reinterpret_cast<uintptr_t>(pEventInputCallback));
}

void PTWWindowImpl::Parent_Set(PTWHWindow hWindowParent)
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
	//WM_PROTOCOLS
	//WM_DELETE_WINDOW
	//Window Manager 关闭对端套接字
	int fd = ::xcb_get_file_descriptor(m_hDisplay_Cache);
	int iResult = ::shutdown(fd, SHUT_WR);
	assert(iResult == 0);
}

#include "../../../Public/App/PTAExport.h"

void * PTInvokeMain(void *pVoid)
{
	PTWWindowImpl *pWindow = static_cast<PTWWindowImpl *>(pVoid);

	int iResult = ::PTAMain(static_cast<IPTWWindow *>(pWindow), pWindow->m_Argc_Cache, pWindow->m_Argv_Cache);
	assert(iResult == 0);

	pWindow->TermminateMessagePump();

	return NULL;
}