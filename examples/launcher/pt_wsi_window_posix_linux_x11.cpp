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

#include <stdlib.h>
#include <assert.h>
#include <X11/keysym.h>
#include <pt_mcrt_memcpy.h>
#include <pt_mcrt_atomic.h>
#include "pt_wsi_window_posix_linux_x11.h"

int main(int argc, char **argv)
{
    shell_x11 shell;
    shell.init();
    shell.run();
    shell.destroy();
    return 0;
}

void shell_x11::init()
{
    m_wsi_window_thread_id = mono_native_thread_id_get();

    int scr;
    m_xcb_connection = xcb_connect(NULL, &scr);
    assert(xcb_connection_has_error(m_xcb_connection) == 0);

    m_setup = xcb_get_setup(m_xcb_connection);

    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(m_setup);
    for (int i = 0; i < scr; ++i)
    {
        xcb_screen_next(&iter);
    }
    m_screen = iter.data;

    m_visual = m_screen->root_visual;

    // CreateWindowExW
    m_window = xcb_generate_id(m_xcb_connection);

    uint32_t value_mask = XCB_CW_BACK_PIXEL | XCB_CW_BACKING_STORE | XCB_CW_EVENT_MASK;

    uint32_t value_list[3] = {m_screen->black_pixel,
                              XCB_BACKING_STORE_NOT_USEFUL,
                              XCB_EVENT_MASK_KEY_PRESS |
                                  XCB_EVENT_MASK_KEY_RELEASE |
                                  XCB_EVENT_MASK_BUTTON_PRESS |
                                  XCB_EVENT_MASK_BUTTON_RELEASE |
                                  XCB_EVENT_MASK_POINTER_MOTION |
                                  XCB_EVENT_MASK_BUTTON_MOTION |
                                  XCB_EVENT_MASK_EXPOSURE |
                                  XCB_EVENT_MASK_STRUCTURE_NOTIFY |
                                  XCB_EVENT_MASK_FOCUS_CHANGE};

    xcb_void_cookie_t cookie_create_window = xcb_create_window_checked(m_xcb_connection,
                                                                       m_screen->root_depth,
                                                                       m_window,
                                                                       m_screen->root, 0, 0, m_screen->width_in_pixels, m_screen->height_in_pixels, 0,
                                                                       XCB_WINDOW_CLASS_INPUT_OUTPUT,
                                                                       m_visual,
                                                                       value_mask, value_list);

    // Title
    // https://specifications.freedesktop.org/wm-spec/wm-spec-latest.html

    // Delete Window
    // https://www.x.org/releases/current/doc/xorg-docs/icccm/icccm.html

    xcb_intern_atom_cookie_t cookie_net_wm_name = xcb_intern_atom(m_xcb_connection, 0, 12U, "_NET_WM_NAME");
    xcb_intern_atom_cookie_t cookie_utf8_string = xcb_intern_atom(m_xcb_connection, 0, 11U, "UTF8_STRING");
    xcb_intern_atom_cookie_t cookie_wm_protocols = xcb_intern_atom(m_xcb_connection, 0, 12U, "WM_PROTOCOLS");
    xcb_intern_atom_cookie_t cookie_wm_delete_window = xcb_intern_atom(m_xcb_connection, 0, 16U, "WM_DELETE_WINDOW");

    xcb_generic_error_t *error_generic = xcb_request_check(m_xcb_connection, cookie_create_window); //implicit xcb_flush
    assert(error_generic == NULL);

    xcb_intern_atom_reply_t *reply_net_wm_name = xcb_intern_atom_reply(m_xcb_connection, cookie_net_wm_name, &error_generic);
    assert(error_generic == NULL);
    xcb_atom_t atom_net_wm_name = reply_net_wm_name->atom;
    free(reply_net_wm_name);

    xcb_intern_atom_reply_t *reply_utf8_string = xcb_intern_atom_reply(m_xcb_connection, cookie_utf8_string, &error_generic);
    assert(error_generic == NULL);
    xcb_atom_t atom_utf8_string = reply_utf8_string->atom;
    free(reply_utf8_string);

    xcb_intern_atom_reply_t *reply_wm_protocols = xcb_intern_atom_reply(m_xcb_connection, cookie_wm_protocols, &error_generic);
    assert(error_generic == NULL);
    m_atom_wm_protocols = reply_wm_protocols->atom;
    free(reply_wm_protocols);

    xcb_intern_atom_reply_t *reply_wm_delete_window = xcb_intern_atom_reply(m_xcb_connection, cookie_wm_delete_window, &error_generic);
    assert(error_generic == NULL);
    m_atom_wm_delete_window = reply_wm_delete_window->atom;
    free(reply_wm_delete_window);

    xcb_void_cookie_t cookie_change_property_net_wm_name = xcb_change_property_checked(m_xcb_connection, XCB_PROP_MODE_REPLACE, m_window, atom_net_wm_name, atom_utf8_string, 8U, 13U, "PatriotEngine");

    xcb_void_cookie_t cookie_change_property_wm_protocols = xcb_change_property_checked(m_xcb_connection, XCB_PROP_MODE_REPLACE, m_window, m_atom_wm_protocols, XCB_ATOM_ATOM, 32U, 1U, &m_atom_wm_delete_window);

    // ShowWindow
    xcb_void_cookie_t cookie_map_window = xcb_map_window_checked(m_xcb_connection, m_window);

    error_generic = xcb_request_check(m_xcb_connection, cookie_change_property_net_wm_name);
    assert(error_generic == NULL);

    error_generic = xcb_request_check(m_xcb_connection, cookie_change_property_wm_protocols);
    assert(error_generic == NULL);

    error_generic = xcb_request_check(m_xcb_connection, cookie_map_window);
    assert(error_generic == NULL);

    // member
    m_loop = true;

    // draw_request_thread
    m_gfx_connection = NULL;
    m_size_change_callback = NULL;
    m_size_change_callback_user_data = NULL;
    m_draw_request_callback = NULL;
    m_draw_request_callback_user_data = NULL;
    mcrt_atomic_store(&m_draw_request_thread_running, false);
    bool res_draw_request = mcrt_native_thread_create(&m_draw_request_thread_id, draw_request_main, this);
    assert(res_draw_request);
    while (!mcrt_atomic_load(&m_draw_request_thread_running))
    {
        mcrt_os_yield();
    }
    assert(m_gfx_connection != NULL);
    assert(m_size_change_callback != NULL);
    assert(m_size_change_callback_user_data != NULL);
    assert(m_draw_request_callback != NULL);
    assert(m_draw_request_callback_user_data != NULL);

    // app related
    m_input_event_callback = NULL;
    m_input_event_callback_user_data = NULL;
    mcrt_atomic_store(&m_app_has_quit, false);
    app_init(static_cast<app_iwindow *>(this), this->m_gfx_connection);
    assert(m_input_event_callback != NULL);

    //https://gitlab.freedesktop.org/xorg/lib/libxcb-keysyms
    //xcb_key_symbols_alloc
    //https://gitlab.freedesktop.org/xorg/lib/libxcb-keysyms
    //xcb_key_symbols_get_keysym
    this->sync_keysyms();
}

void *shell_x11::draw_request_main(void *arg)
{
    shell_x11 *self = static_cast<shell_x11 *>(arg);

    self->m_gfx_connection = gfx_connection_init(self);
    assert(self->m_gfx_connection != NULL);
    assert(self->m_size_change_callback != NULL);
    assert(self->m_size_change_callback_user_data != NULL);
    assert(self->m_draw_request_callback != NULL);
    assert(self->m_draw_request_callback_user_data != NULL);
    mcrt_atomic_store(&self->m_draw_request_thread_running, true);

    while (self->m_loop)
    {
        self->m_draw_request_callback(self->m_xcb_connection, reinterpret_cast<void *>(static_cast<uintptr_t>(self->m_visual)), reinterpret_cast<void *>(static_cast<uintptr_t>(self->m_window)), self->m_draw_request_callback_user_data);
    }

    self->m_gfx_connection->destroy();
    mcrt_atomic_store(&self->m_draw_request_thread_running, false);
    return NULL;
}

void shell_x11::listen_size_change(void (*size_change_callback)(void *wsi_connection, void *visual, void *window, float width, float height, void *user_data), void *user_data)
{
    assert(mono_native_thread_id_get() == m_draw_request_thread_id);

    assert(m_size_change_callback == NULL);
    assert(m_size_change_callback_user_data == NULL);
    m_size_change_callback = size_change_callback;
    m_size_change_callback_user_data = user_data;

    m_size_change_callback(m_xcb_connection, reinterpret_cast<void *>(static_cast<uintptr_t>(m_visual)), reinterpret_cast<void *>(static_cast<uintptr_t>(m_window)), 1, 1, m_size_change_callback_user_data);
}

void shell_x11::listen_draw_request(void (*draw_request_callback)(void *wsi_connection, void *visual, void *window, void *user_data), void *user_data)
{
    assert(mono_native_thread_id_get() == m_draw_request_thread_id);

    assert(m_draw_request_callback == NULL);
    assert(m_draw_request_callback_user_data == NULL);
    m_draw_request_callback = draw_request_callback;
    m_draw_request_callback_user_data = user_data;
}

void shell_x11::listen_input_event(void (*input_event_callback)(struct input_event_t *input_event, void *user_data), void *user_data)
{
    assert(mono_native_thread_id_get() == m_wsi_window_thread_id);

    assert(m_input_event_callback == NULL);
    assert(m_input_event_callback_user_data == NULL);
    m_input_event_callback = input_event_callback;
    m_input_event_callback_user_data = user_data;
}

void shell_x11::sync_keysyms()
{
    m_min_keycode = m_setup->min_keycode;
    m_max_keycode = m_setup->max_keycode;

    xcb_get_keyboard_mapping_cookie_t cookie_get_keyboard_mapping = xcb_get_keyboard_mapping(m_xcb_connection, m_min_keycode, (m_max_keycode - m_min_keycode) + 1);

    xcb_generic_error_t *error_generic;
    xcb_get_keyboard_mapping_reply_t *reply_get_keyboard_mapping = xcb_get_keyboard_mapping_reply(m_xcb_connection, cookie_get_keyboard_mapping, &error_generic);
    assert(error_generic == NULL);

    xcb_keysym_t *keysym_begin = xcb_get_keyboard_mapping_keysyms(reply_get_keyboard_mapping);
    xcb_generic_iterator_t iter_keysym_end = xcb_get_keyboard_mapping_keysyms_end(reply_get_keyboard_mapping);
    xcb_keysym_t *keysym_end = static_cast<xcb_keysym_t *>(iter_keysym_end.data);
    assert((sizeof(xcb_keysym_t) * (keysym_end - keysym_begin)) == (iter_keysym_end.index - sizeof(xcb_get_keyboard_mapping_reply_t)));

    m_keysym.resize(keysym_end - keysym_begin);
    assert(sizeof(m_keysym[0]) == sizeof(xcb_keysym_t));
    mcrt_memcpy(&m_keysym[0], keysym_begin, sizeof(xcb_keysym_t) * (keysym_end - keysym_begin));

    m_keysyms_per_keycode = reply_get_keyboard_mapping->keysyms_per_keycode;
    assert((keysym_end - keysym_begin) == m_keysyms_per_keycode * ((m_max_keycode - m_min_keycode) + 1));

    free(reply_get_keyboard_mapping);

    //To Do: Use the keycode directly?
    // https://www.x.org/releases/X11R7.7/doc/xorg-docs/input/XKB-Config.html
    // On Linux systems, the evdev rules are most commonly used, on other systems the base rules are used.
    // /usr/share/X11/xkb/keycodes/evdev
    // #define KEY_ESC 9
    // libxkbcommon/tools/interactive-x11.c
}

xcb_keysym_t shell_x11::keycode_to_keysym(xcb_keycode_t keycode)
{
    //https://gitlab.freedesktop.org/xorg/lib/libxcb-keysyms
    //xcb_key_symbols_get_keysym

    if (keycode >= m_min_keycode && keycode <= m_max_keycode)
    {
        assert((m_keysyms_per_keycode * (keycode - m_min_keycode)) < m_keysym.size());
        return m_keysym[m_keysyms_per_keycode * (keycode - m_min_keycode)];
    }
    else
    {
        return XCB_NO_SYMBOL;
    }
}

void shell_x11::run()
{
    xcb_generic_event_t *event;

    while (m_loop && ((event = xcb_wait_for_event(m_xcb_connection)) != NULL))
    {

        // The most significant bit(uint8_t(0X80)) in this code is set if the event was generated from a SendEvent request.
        // https://www.x.org/releases/current/doc/xproto/x11protocol.html#event_format
        switch (event->response_type & (~uint8_t(0X80)))
        {
        case XCB_KEY_PRESS:
        {
            assert(XCB_KEY_PRESS == (event->response_type & (~uint8_t(0X80))));

            xcb_key_press_event_t *key_press = reinterpret_cast<xcb_key_press_event_t *>(event);
            if (key_press->event == m_window)
            {
                xcb_keycode_t keycode = key_press->detail;
                xcb_keysym_t keysym = this->keycode_to_keysym(keycode);
                switch (keysym)
                {
                case XK_W:
                case XK_w:
                {
                    assert(XK_W == keysym || XK_w == keysym);

                    struct input_event_t input_event = {app_iwindow::input_event_t::MESSAGE_CODE_KEY_PRESS, app_iwindow::input_event_t::KEY_SYM_W, 0};
                    assert(m_input_event_callback != NULL);
                    m_input_event_callback(&input_event, m_input_event_callback_user_data);
                }
                break;
                case XK_Shift_L:
                {
                    assert(XK_Shift_L == keysym);
                }
                break;
                }
            }
        }
        break;
        case XCB_CONFIGURE_NOTIFY:
        {
            assert(XCB_CONFIGURE_NOTIFY == (event->response_type & (~uint8_t(0X80))));

            xcb_configure_notify_event_t *configure_notify = reinterpret_cast<xcb_configure_notify_event_t *>(event);

            assert(m_size_change_callback != NULL);
            m_size_change_callback(m_xcb_connection, reinterpret_cast<void *>(static_cast<uintptr_t>(m_visual)), reinterpret_cast<void *>(static_cast<uintptr_t>(m_window)), configure_notify->width, configure_notify->height, m_size_change_callback_user_data);
        }
        break;
        case XCB_MAPPING_NOTIFY:
        {
            assert(XCB_MAPPING_NOTIFY == (event->response_type & (~uint8_t(0X80))));

            xcb_mapping_notify_event_t *mapping_notify = reinterpret_cast<xcb_mapping_notify_event_t *>(event);

            if (mapping_notify->request == XCB_MAPPING_KEYBOARD)
            {
                // https://gitlab.freedesktop.org/xorg/lib/libxcb-keysyms
                // xcb_refresh_keyboard_mapping
                this->sync_keysyms();
            }
        }
        break;
        case XCB_CLIENT_MESSAGE:
        {
            assert(XCB_CLIENT_MESSAGE == (event->response_type & (~uint8_t(0X80))));

            xcb_client_message_event_t *client_message = reinterpret_cast<xcb_client_message_event_t *>(event);
            // WM_DESTROY
            if (client_message->window == m_window && client_message->type == m_atom_wm_protocols && client_message->format == 32U && client_message->data.data32[0] == m_atom_wm_delete_window)
            {
                m_loop = false;

                //mcrt_atomic_store(&m_app_has_quit, false);
                struct input_event_t input_event = {app_iwindow::input_event_t::MESSAGE_CODE_QUIT, 0, 0};
                assert(m_input_event_callback != NULL);
                m_input_event_callback(&input_event, m_input_event_callback_user_data);

                //move xcb_destroy_window_checked here?
            }
        }
        break;
        default:
        {
            // Unknown event type, ignore it
        }
        }

        free(event);
    }
}

void shell_x11::mark_app_has_quit()
{
    //thread_id pending

    mcrt_atomic_store(&m_app_has_quit, true);
}

void shell_x11::destroy()
{
    while (!mcrt_atomic_load(&m_app_has_quit))
    {
        mcrt_os_yield();
    }

    mcrt_native_thread_join(m_draw_request_thread_id);
    while (mcrt_atomic_load(&m_draw_request_thread_running))
    {
        mcrt_os_yield();
    }

    xcb_void_cookie_t cookie_destroy_window = xcb_destroy_window_checked(m_xcb_connection, m_window);

    xcb_generic_error_t *error_generic = xcb_request_check(m_xcb_connection, cookie_destroy_window); //implicit xcb_flush
    assert(error_generic == NULL);

    xcb_disconnect(m_xcb_connection);
}
