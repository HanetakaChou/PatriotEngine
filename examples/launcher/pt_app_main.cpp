#include "pt_app_main.h"
#include "pt_gfx_connection_utils.h"

void app_main(struct wsi_iwindow *window, struct gfx_iconnection *connection)
{
    gfx_itexture *texture = connection->create_texture();
    gfx_itexture_read_file(texture, "third_party/assets/lenna/l_hires_directx_tex.dds");

    int huhu = 0;
}