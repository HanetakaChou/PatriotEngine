/*
 * Copyright (C) YuqiaoZhang(HanetakaCho)
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

#ifndef _WSI_WINDOW_POSIX_LINUX_X11_H_
#define _WSI_WINDOW_POSIX_LINUX_X11_H_ 1

#include <stddef.h>
#include <pt_mcrt_thread.h>
#include <pt_gfx_connection.h>
#include <xcb/xcb.h>
#include "pt_app.h"
#include <vector>

class shell_x11 : app_iwindow
{
    mcrt_native_thread_id m_wsi_window_thread_id;

    xcb_connection_t *m_xcb_connection;

    xcb_setup_t const *m_setup;
    xcb_screen_t *m_screen;
    xcb_visualid_t m_visual;

    xcb_window_t m_window;
    float m_window_width;
    float m_window_height;

    xcb_atom_t m_atom_wm_protocols;
    xcb_atom_t m_atom_wm_delete_window;

    bool m_loop;

    mcrt_native_thread_id m_draw_request_thread_id;
    static void *draw_request_main(void *);
    gfx_connection_ref m_gfx_connection;
    bool m_draw_request_thread_running;

    xcb_keycode_t m_min_keycode;
    xcb_keycode_t m_max_keycode;
    std::vector<xcb_keysym_t> m_keysym;
    uint8_t m_keysyms_per_keycode;
    void sync_keysyms();
    xcb_keysym_t keycode_to_keysym(xcb_keycode_t keycode);

    void (*m_input_event_callback)(struct input_event_t *input_event, void *user_data);
    void *m_input_event_callback_user_data;
    bool m_app_has_quit;

    void listen_input_event(void (*input_event_callback)(struct input_event_t *input_event, void *user_data), void *user_data) override;
    void mark_app_has_quit() override;

    static inline wsi_connection_ref wrap(xcb_connection_t *wsi_connection) { return reinterpret_cast<wsi_connection_ref>(wsi_connection); }
    static inline wsi_visual_ref wrap_visual(xcb_visualid_t wsi_visual) { return reinterpret_cast<wsi_visual_ref>(static_cast<uintptr_t>(wsi_visual)); }
    static inline wsi_window_ref wrap_window(xcb_window_t wsi_window) { return reinterpret_cast<wsi_window_ref>(static_cast<uintptr_t>(wsi_window)); }

public:
    void init();
    void run();
    void destroy();
};

#endif