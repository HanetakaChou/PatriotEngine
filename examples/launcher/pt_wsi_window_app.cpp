#include <stdlib.h>
#include <time.h>
#include <vector>
#include "pt_wsi_window_app.h"
#include "pt_gfx_connection_utils.h"

static gfx_connection_ref my_gfx_connection = NULL;
//static gfx_texture_ref my_texture1 = NULL;
//static gfx_texture_ref my_texture2 = NULL;

wsi_window_app_ref wsi_window_app_init(gfx_connection_ref gfx_connection)
{
    my_gfx_connection = gfx_connection;
    return (wsi_window_app_ref)0xdeadbeef;
}

int wsi_window_app_main(wsi_window_app_ref wsi_window_app)
{
    std::vector<gfx_texture_ref> my_textures;

    unsigned rand_buf = (unsigned)time(NULL);

    for (int i = 0; i < 500; ++i)
    {
        long int r1 = rand_r(&rand_buf);

        if (0 == r1 % 8 || 1 == r1 % 8 || 2 == r1 % 8)
        {
            gfx_texture_ref my_texture = gfx_connection_create_texture(my_gfx_connection);
            gfx_texture_read_file(my_texture, "third_party/assets/lenna/l_hires_rgba.pvr");
            my_textures.push_back(my_texture);
        }
        else if (3 == r1 % 8 || 4 == r1 % 8 || 5 == r1 % 8)
        {
            gfx_texture_ref my_texture = gfx_connection_create_texture(my_gfx_connection);
            gfx_texture_read_file(my_texture, "third_party/assets/lenna/l_hires_directx_tex.dds");
            my_textures.push_back(my_texture);
        }
        else
        {
            if (!my_textures.empty())
            {
                long int r2 = rand_r(&rand_buf);
                int vec_idx = (r2 % my_textures.size());
                gfx_texture_ref my_texture = my_textures[vec_idx];
                my_textures[vec_idx] = my_textures.back();
                my_textures.pop_back();
                gfx_texture_destroy(my_texture);
            }
            else if (6 == r1 % 8)
            {
                gfx_texture_ref my_texture = gfx_connection_create_texture(my_gfx_connection);
                gfx_texture_read_file(my_texture, "third_party/assets/lenna/l_hires_rgba.pvr");
                my_textures.push_back(my_texture);
            }
            else
            {
                gfx_texture_ref my_texture = gfx_connection_create_texture(my_gfx_connection);
                gfx_texture_read_file(my_texture, "third_party/assets/lenna/l_hires_directx_tex.dds");
                my_textures.push_back(my_texture);
            }
        }
    }

    for (gfx_texture_ref my_texture : my_textures)
    {
        gfx_texture_destroy(my_texture);
    }

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
    //gfx_texture_destroy(my_texture1);
    //gfx_texture_destroy(my_texture2);
}
