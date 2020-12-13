/*
 * Copyright (C) YuqiaoZhang
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <pt_wsi_window.h>

#include <xcb/xcb.h>
#include <pt_mcrt_thread.h>
#include <vector>

class shell_x11 : wsi_iwindow
{

    xcb_connection_t *m_connection;

    xcb_setup_t const *m_setup;
    xcb_screen_t *m_screen;

    xcb_window_t m_window;

    xcb_atom_t m_atom_wm_protocols;
    xcb_atom_t m_atom_wm_delete_window;

    mcrt_native_thread_id m_draw_request_thread;
    static void *draw_request_main(void *);

    bool m_loop;

    xcb_keycode_t m_min_keycode;
    xcb_keycode_t m_max_keycode;
    std::vector<xcb_keysym_t> m_keysym;
    uint8_t m_keysyms_per_keycode;
    void sync_keysyms();
    xcb_keysym_t keycode_to_keysym(xcb_keycode_t keycode);

    void listen_size_change(void (*size_change_callback)(float width, float height, void *user_data), void *user_data) override;
    void listen_draw_request(void (*draw_request_callback)(void *connection, void *window, void *user_data), void *user_data) override;

public:
    void init();
    void run();
    void destroy();
};