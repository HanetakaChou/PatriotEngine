#include "pt_app.h"
#include "pt_gfx_connection_utils.h"


void app_init(struct app_iwindow *window, gfx_connection_ref gfx_connection)
{
    //may create thread here

    window->listen_input_event(
        [](struct app_iwindow::input_event_t *input_event, void *user_data) -> void {
            switch (input_event->message_code)
            {
            case app_iwindow::input_event_t::KEY_SYM_W:
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
        },
        NULL);

    gfx_texture_ref texture = gfx_connection_create_texture(gfx_connection);
    gfx_texture_read_file(texture, "third_party/assets/lenna/l_hires_directx_tex.dds");

    window->mark_app_has_quit();
}