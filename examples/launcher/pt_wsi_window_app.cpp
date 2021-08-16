#include <stdlib.h>
#include <time.h>
#include <vector>
#include "pt_wsi_window_app.h"
#include "pt_gfx_connection_utils.h"
#include <pt_mcrt_thread.h>

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
    gfx_mesh_ref my_mesh = gfx_connection_create_mesh(my_gfx_connection);
    //gfx_mesh_read_file(my_gfx_connection, my_mesh, 0, 0, "third_party/assets/glTF-Sample-Models/AnimatedCube/glTF/AnimatedCube.gltf");
    gfx_mesh_read_file(my_gfx_connection, my_mesh, 0, 0, "third_party/assets/glTF-Sample-Models/AnimatedCube/glTF/AnimatedCube.bin");
    gfx_mesh_destroy(my_gfx_connection, my_mesh);

#if 0
    std::vector<gfx_texture_ref> my_textures;

    unsigned rand_buf = (unsigned)time(NULL);

    for (int i = 0; i < 500; ++i)
    {
        long int r1 = rand_r(&rand_buf);

        if (0 == r1 % 9 || 1 == r1 % 9 || 2 == r1 % 9 || 3 == r1 % 9)
        {
            gfx_texture_ref my_texture = gfx_connection_create_texture(my_gfx_connection);
            gfx_texture_read_file(my_gfx_connection, my_texture, "third_party/assets/lenna/l_hires_rgba.pvr");
            my_textures.push_back(my_texture);
        }
        else if (4 == r1 % 9 || 5 == r1 % 9 || 6 == r1 % 9 || 7 == r1 % 9)
        {
            gfx_texture_ref my_texture = gfx_connection_create_texture(my_gfx_connection);
            gfx_texture_read_file(my_gfx_connection, my_texture, "third_party/assets/lenna/l_hires_directx_tex.dds");
            my_textures.push_back(my_texture);
        }
        else
        {
            if (!my_textures.empty())
            {
                long int r2 = rand_r(&rand_buf);
                int vec_idx = (r2 % (my_textures.size() / 2 + 1));
                gfx_texture_ref my_texture = my_textures[vec_idx];
                my_textures[vec_idx] = my_textures.back();
                my_textures.pop_back();
                gfx_texture_destroy(my_gfx_connection, my_texture);
            }
            else
            {
                gfx_texture_ref my_texture = gfx_connection_create_texture(my_gfx_connection);
                gfx_texture_read_file(my_gfx_connection, my_texture, "third_party/assets/lenna/l_hires_nvidia_texture_tools.dds");
                my_textures.push_back(my_texture);
            }
        }
    }

    sleep(20);

    for (gfx_texture_ref my_texture : my_textures)
    {
        gfx_texture_destroy(my_gfx_connection, my_texture);
    }
#endif
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
