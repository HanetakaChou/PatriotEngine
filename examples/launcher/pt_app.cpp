#include "pt_app.h"
#include "pt_gfx_connection_utils.h"

void app_init(struct app_iwindow *window, struct gfx_iconnection *connection)
{
    window->listen_input_event([](struct app_iwindow::input_event_t *input_event, void *user_data) -> void {}, NULL);

    gfx_itexture *texture = connection->create_texture();
    gfx_itexture_read_file(texture, "third_party/assets/lenna/l_hires_directx_tex.dds");

    window->mark_app_has_destroyed();
}