
#include <wayland-client.h>

#include <string.h>
#include <assert.h>

struct WLGlobalObject
{
	wl_compositor *hCompositor;
	wl_shell *hShell;
	wl_output *hOutput;
	wl_seat *hSeat;
};

void WLRegistryGlobal(void *data, wl_registry *registry, uint32_t name, const char *interface, uint32_t version);
void WLRegistryGlobalRemove(void *data, wl_registry *registry, uint32_t name);


void WLShellSurfacePing(void *data, wl_shell_surface *shell_surface, uint32_t serial);
void WLShellSurfaceConfigure(void *data, wl_shell_surface *shell_surface, uint32_t edges, int32_t width, int32_t height);
void WLShellSurfacePopupDone(void *data, wl_shell_surface *shell_surface);

int main(int argc, char *argv[])
{
	wl_display *hDisplay;
	{
		char *DisplayName = NULL;
		for (int i = 1; i < argc; ++i)
		{
			DisplayName = ::strstr(argv[i], "-PTDisplay");
			if (DisplayName != NULL)
			{
				break;
			}
		}
		if (DisplayName != NULL)
		{
			assert((*(DisplayName + 10)) == '=');
			DisplayName += 11;
		}
		hDisplay = ::wl_display_connect(DisplayName);//同时创建事件队列
		assert(hDisplay != NULL);
	}

	wl_registry *hRegistry = ::wl_display_get_registry(hDisplay);//同时发出全局事件

	//int iResult = ::wl_display_dispatch(hDisplay);//如果没有监听器，那么相应的事件被忽略

	WLGlobalObject  TableGlobalObject;
	TableGlobalObject.hCompositor = NULL;
	TableGlobalObject.hShell = NULL;
	TableGlobalObject.hSeat = NULL;
	TableGlobalObject.hOutput = NULL;
	{
		wl_registry_listener listener;
		listener.global = ::WLRegistryGlobal;
		listener.global_remove = WLRegistryGlobalRemove;
		int iResult = ::wl_registry_add_listener(hRegistry, &listener, &TableGlobalObject);
		assert(iResult == 0);
	}

	int iResult = ::wl_display_dispatch(hDisplay);
	assert(iResult != -1);

	assert(TableGlobalObject.hCompositor != NULL);
	wl_surface *hSurface = ::wl_compositor_create_surface(TableGlobalObject.hCompositor);
	assert(hSurface != NULL);

	assert(TableGlobalObject.hShell != NULL);
	wl_shell_surface *hShellSurface = ::wl_shell_get_shell_surface(TableGlobalObject.hShell, hSurface);//即Win32中的非客户区
	assert(hShellSurface != NULL);

	{
		wl_shell_surface_listener listener;
		listener.ping = WLShellSurfacePing;
		listener.configure = WLShellSurfaceConfigure;
		listener.popup_done = WLShellSurfacePopupDone;
		int iResult = ::wl_shell_surface_add_listener(hShellSurface, &listener, NULL);
		assert(iResult == 0);
	}

	::wl_shell_surface_set_title(hShellSurface, "爱国者引擎");

	//wl_pointer *hPoint = ::wl_seat_get_pointer(TableGlobalObject.hSeat);//AInputQueue????

	//wl_keyboard *hKeyboard = ::wl_seat_get_keyboard(TableGlobalObject.hSeat);

	//::wl_shell_surface_add_listener()

	assert(TableGlobalObject.hOutput != NULL);
	
	::wl_shell_surface_set_toplevel(hShellSurface);
	
	//::wl_shell_surface_resize(hShellSurface,TableGlobalObject.hSeat,)

	//::wl_shell_surface_set_maximized(hShellSurface, TableGlobalObject.hOutput);

	iResult = ::wl_display_roundtrip(hDisplay);
	assert(iResult != -1);

	//::wl_shell_surface_set_transient() //PTEditor

	iResult = ::wl_display_dispatch(hDisplay);
	assert(iResult != -1);

	::wl_display_disconnect(hDisplay);
}


void WLRegistryGlobal(void *data, wl_registry *registry, uint32_t name, const char *interface, uint32_t version)
{
	if (::strcmp(interface, "wl_compositor") == 0)
	{
		static_cast<WLGlobalObject *>(data)->hCompositor = static_cast<wl_compositor *>(::wl_registry_bind(registry, name, &wl_compositor_interface, version));
		assert(static_cast<WLGlobalObject *>(data)->hCompositor != NULL);
	}
	else if (strcmp(interface, "wl_shell") == 0)
	{
		static_cast<WLGlobalObject *>(data)->hShell = static_cast<wl_shell *>(::wl_registry_bind(registry, name, &wl_shell_interface, version));
		assert(static_cast<WLGlobalObject *>(data)->hShell != NULL);
	}
	else if (strcmp(interface, "wl_output") == 0)
	{
		static_cast<WLGlobalObject *>(data)->hOutput = static_cast<wl_output *>(::wl_registry_bind(registry, name, &wl_output_interface, version));
		assert(static_cast<WLGlobalObject *>(data)->hOutput != NULL);
	}
	else if (strcmp(interface, "wl_seat") == 0)
	{
		static_cast<WLGlobalObject *>(data)->hSeat = static_cast<wl_seat *>(::wl_registry_bind(registry, name, &wl_seat_interface, version));
		assert(static_cast<WLGlobalObject *>(data)->hSeat != NULL);
	}
}

void WLRegistryGlobalRemove(void *data, wl_registry *registry, uint32_t name)
{
	
}


void WLShellSurfacePing(void *data, wl_shell_surface *shell_surface, uint32_t serial)
{
	::wl_shell_surface_pong(shell_surface, serial);
}

void WLShellSurfaceConfigure(void *data, wl_shell_surface *shell_surface, uint32_t edges, int32_t width, int32_t height)
{
	//::wl_shell_surface_resize()
	int huhu = 0;
}

void WLShellSurfacePopupDone(void *data, wl_shell_surface *shell_surface)
{
	int huhu = 0;
}