#include "pt_wsi_window.h"

class shell_x11 : wsi_iwindow
{
    void listen_size_change(void (*size_change_callback)(float width, float height, void *user_data), void *user_data) override;
    void listen_draw_request(void (*draw_request_callback)(void *connection, void *window, void *user_data), void *user_data) override;

public:
    void init();

    void run();

    void destroy();
};