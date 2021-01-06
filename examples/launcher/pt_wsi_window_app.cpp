#include "pt_wsi_window_app.h"
#include "pt_gfx_connection_utils.h"

static gfx_connection_ref my_gfx_connection = NULL;

wsi_window_app_ref wsi_window_app_init(gfx_connection_ref gfx_connection)
{
    my_gfx_connection = gfx_connection;
    return NULL;
}

int wsi_window_app_main(wsi_window_app_ref wsi_window_app)
{
    gfx_texture_ref texture = gfx_connection_create_texture(my_gfx_connection);
    gfx_texture_read_file(texture, "third_party/assets/lenna/l_hires_directx_tex.dds");

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
}
