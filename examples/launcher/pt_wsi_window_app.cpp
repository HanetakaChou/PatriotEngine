#include "pt_wsi_window_app.h"
#include "pt_gfx_connection_utils.h"

static gfx_connection_ref my_gfx_connection = NULL;
static gfx_texture_ref my_texture1 = NULL;
static gfx_texture_ref my_texture2 = NULL;

wsi_window_app_ref wsi_window_app_init(gfx_connection_ref gfx_connection)
{
    my_gfx_connection = gfx_connection;
    return (wsi_window_app_ref)0xdeadbeef;
}

int wsi_window_app_main(wsi_window_app_ref wsi_window_app)
{
    my_texture1 = gfx_connection_create_texture(my_gfx_connection);
    gfx_texture_read_file(my_texture1, "third_party/assets/lenna/l_hires_rgba.pvr");
    my_texture2 = gfx_connection_create_texture(my_gfx_connection);
    gfx_texture_read_file(my_texture2, "third_party/assets/lenna/l_hires_directx_tex.dds");
    return 0;
}

void wsi_window_app_handle_event(wsi_window_app_ref wsi_window_app, struct wsi_window_app_event_t *wsi_window_app_event)
{
    switch (wsi_window_app_event->message_code)
    {
    case wsi_window_app_event_t::KEY_SYM_W:
    {
        //camera move forward
    }
    break;

        // case MESSAGE_CODE_QUIT
        // may clean up here
        // window->mark_app_has_quit();

    default:
        break;
    }
}

void wsi_window_app_destroy(wsi_window_app_ref wsi_window_app)
{
    gfx_texture_destroy(my_texture1);
    gfx_texture_destroy(my_texture2);
}
