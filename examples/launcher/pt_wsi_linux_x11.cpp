/*
 * Copyright (C) YuqiaoZhang(HanetakaYuminaga)
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
#include <string>
#include <X11/keysym.h>
#include <pt_mcrt_memcpy.h>
#include <pt_mcrt_atomic.h>
#include <pt_mcrt_scalable_allocator.h>
#include "pt_wsi_linux_x11.h"

using mcrt_string = std::basic_string<char, std::char_traits<char>, mcrt::scalable_allocator<char>>;

int main(int argc, char **argv)
{
    wsi_window_x11 window_x11;
    window_x11.init();
    window_x11.run();
    window_x11.destroy();
    return 0;
}

void wsi_window_x11::init()
{
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

    this->m_window_width = 1280;
    this->m_window_height = 720;
    xcb_void_cookie_t cookie_create_window = xcb_create_window_checked(m_xcb_connection,
                                                                       m_screen->root_depth,
                                                                       m_window,
                                                                       m_screen->root, 0, 0, this->m_window_width, this->m_window_height, 0,
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
    mcrt_atomic_store(&this->m_loop, true);

    // draw_request_thread
    m_gfx_connection = NULL;
    mcrt_atomic_store(&m_draw_request_thread_running, false);
    PT_MAYBE_UNUSED bool res_draw_request_thread = mcrt_native_thread_create(&m_draw_request_thread_id, draw_request_main, this);
    assert(res_draw_request_thread);
    while (!mcrt_atomic_load(&m_draw_request_thread_running))
    {
        mcrt_os_yield();
    }
    assert(m_gfx_connection != NULL);

    // app_thread
    mcrt_atomic_store(&m_app_thread_running, false);
    PT_MAYBE_UNUSED bool res_app_thread = mcrt_native_thread_create(&m_app_thread_id, app_main, this);
    assert(res_app_thread);
    while (!mcrt_atomic_load(&m_app_thread_running))
    {
        mcrt_os_yield();
    }

    //https://gitlab.freedesktop.org/xorg/lib/libxcb-keysyms
    //xcb_key_symbols_alloc
    //https://gitlab.freedesktop.org/xorg/lib/libxcb-keysyms
    //xcb_key_symbols_get_keysym
    this->sync_keysyms();
}

inline wsi_connection_ref wsi_window_x11::wrap_wsi_connection(xcb_connection_t *wsi_connection)
{
    return reinterpret_cast<wsi_connection_ref>(wsi_connection);
}

inline wsi_visual_ref wsi_window_x11::wrap_wsi_visual(xcb_visualid_t wsi_visual)
{
    return reinterpret_cast<wsi_visual_ref>(static_cast<uintptr_t>(wsi_visual));
}

inline wsi_window_ref wsi_window_x11::wrap_wsi_window(xcb_window_t wsi_window)
{
    return reinterpret_cast<wsi_window_ref>(static_cast<uintptr_t>(wsi_window));
}

void *wsi_window_x11::draw_request_main(void *arg)
{
    wsi_window_x11 *self = static_cast<wsi_window_x11 *>(arg);

    self->m_gfx_connection = gfx_connection_init(wrap_wsi_connection(self->m_xcb_connection), wrap_wsi_visual(self->m_visual), ".");
    assert(self->m_gfx_connection != NULL);
    mcrt_atomic_store(&self->m_draw_request_thread_running, true);

    gfx_connection_on_wsi_window_created(self->m_gfx_connection, wrap_wsi_connection(self->m_xcb_connection), wrap_wsi_window(self->m_window), self->m_window_height, self->m_window_width);

    while (mcrt_atomic_load(&self->m_loop))
    {
        // usage
        // on_redraw_needed
        // app update scenetree //maybe in other thread //not depend on accurate time
        // gfx acquire //sync and flatten scenetree //then frame throttling
        // app update time-related (animation etc) //frame throttling make the time here less latency //scenetree modify will impact on the next frame
        // gfx release //draw and present //draw //not sync scenetree

        // update scenetree
        gfx_connection_on_wsi_redraw_needed_acquire(self->m_gfx_connection);
        // update animation etc
        gfx_connection_on_wsi_redraw_needed_release(self->m_gfx_connection);
    }

    mcrt_atomic_store(&self->m_draw_request_thread_running, false);
    return NULL;
}

void *wsi_window_x11::app_main(void *arg)
{
    wsi_window_x11 *self = static_cast<wsi_window_x11 *>(arg);

    PT_MAYBE_UNUSED bool res_neutral_app_init = wsi_neutral_app_init(self->m_gfx_connection, &self->m_neutral_app_void_instance);
    assert(res_neutral_app_init);
    mcrt_atomic_store(&self->m_app_thread_running, true);

    int res_neutral_app_main = wsi_neutral_app_main(self->m_neutral_app_void_instance);

    //mcrt_atomic_store(&self->m_loop, false);

    //wsi_window_app_destroy() //used in run //wsi_window_app_handle_event

    return reinterpret_cast<void *>(static_cast<intptr_t>(res_neutral_app_main));
}

void wsi_window_x11::sync_keysyms()
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

xcb_keysym_t wsi_window_x11::keycode_to_keysym(xcb_keycode_t keycode)
{
    //https://gitlab.freedesktop.org/xorg/lib/libxcb-keysyms
    //xcb_key_symbols_get_keysym

    if (keycode >= m_min_keycode && keycode <= m_max_keycode)
    {
        assert((m_keysyms_per_keycode * (keycode - m_min_keycode)) < static_cast<uint8_t>(m_keysym.size()));
        return m_keysym[m_keysyms_per_keycode * (keycode - m_min_keycode)];
    }
    else
    {
        return XCB_NO_SYMBOL;
    }
}

void wsi_window_x11::run()
{
    xcb_generic_event_t *event;

    while (mcrt_atomic_load(&this->m_loop) && ((event = xcb_wait_for_event(m_xcb_connection)) != NULL))
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

                    struct wsi_neutral_app_input_event_t wsi_neutral_app_input_event = {MESSAGE_CODE_KEY_PRESS, KEY_SYM_W, 0};
                    wsi_neutral_app_handle_input_event(&wsi_neutral_app_input_event, this->m_neutral_app_void_instance);
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
            this->m_window_width = configure_notify->width;
            this->m_window_height = configure_notify->height;
            gfx_connection_on_wsi_resized(m_gfx_connection, this->m_window_width, this->m_window_height);
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
                mcrt_atomic_store(&this->m_loop, false);

                //mcrt_atomic_store(&m_app_has_quit, false);
                struct wsi_neutral_app_input_event_t wsi_neutral_app_input_event = {MESSAGE_CODE_QUIT, 0, 0};
                wsi_neutral_app_handle_input_event(&wsi_neutral_app_input_event, this->m_neutral_app_void_instance);

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

void wsi_window_x11::destroy()
{
    mcrt_native_thread_join(m_app_thread_id);
    while (!mcrt_atomic_load(&m_app_thread_running))
    {
        mcrt_os_yield();
    }

    mcrt_native_thread_join(m_draw_request_thread_id);
    while (mcrt_atomic_load(&m_draw_request_thread_running))
    {
        mcrt_os_yield();
    }
    gfx_connection_on_wsi_window_destroyed(this->m_gfx_connection);
    gfx_connection_destroy(this->m_gfx_connection);

    xcb_void_cookie_t cookie_destroy_window = xcb_destroy_window_checked(m_xcb_connection, m_window);

    PT_MAYBE_UNUSED xcb_generic_error_t *error_generic = xcb_request_check(m_xcb_connection, cookie_destroy_window); //implicit xcb_flush
    assert(error_generic == NULL);

    xcb_disconnect(m_xcb_connection);
}

// neutral app file system
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

bool gfx_texture_read_file(gfx_connection_ref gfx_connection, gfx_texture_ref texture, char const *initial_filename)
{
    mcrt_string path = "../third_party/assets/";
    path += initial_filename;

    return gfx_texture_read_input_stream(
        gfx_connection,
        texture,
        path.c_str(),
        [](char const *initial_filename) -> gfx_input_stream_ref
        {
            int fd = openat(AT_FDCWD, initial_filename, O_RDONLY);
            return reinterpret_cast<gfx_input_stream_ref>(static_cast<intptr_t>(fd));
        },
        [](gfx_input_stream_ref gfx_input_stream, void *buf, size_t count) -> intptr_t
        {
            ssize_t res_read = read(static_cast<int>(reinterpret_cast<intptr_t>(gfx_input_stream)), buf, count);
            return res_read;
        },
        [](gfx_input_stream_ref gfx_input_stream, int64_t offset, int whence) -> int64_t
        {
            off_t res_lseek = lseek(static_cast<int>(reinterpret_cast<intptr_t>(gfx_input_stream)), offset, whence);
            return res_lseek;
        },
        [](gfx_input_stream_ref gfx_input_stream) -> void
        {
            close(static_cast<int>(reinterpret_cast<intptr_t>(gfx_input_stream)));
        });
}

bool gfx_mesh_read_file(gfx_connection_ref gfx_connection, gfx_mesh_ref mesh, uint32_t mesh_index, uint32_t material_index, char const *initial_filename)
{
    mcrt_string path = "../third_party/assets/";
    path += initial_filename;

    return gfx_mesh_read_input_stream(
        gfx_connection,
        mesh,
        mesh_index,
        material_index,
        path.c_str(),
        [](char const *initial_filename) -> gfx_input_stream_ref
        {
            int fd = openat(AT_FDCWD, initial_filename, O_RDONLY);
            return reinterpret_cast<gfx_input_stream_ref>(static_cast<intptr_t>(fd));
        },
        [](gfx_input_stream_ref gfx_input_stream, void *buf, size_t count) -> intptr_t
        {
            ssize_t res_read = read(static_cast<int>(reinterpret_cast<intptr_t>(gfx_input_stream)), buf, count);
            return res_read;
        },
        [](gfx_input_stream_ref gfx_input_stream, int64_t offset, int whence) -> int64_t
        {
            off_t res_lseek = lseek(static_cast<int>(reinterpret_cast<intptr_t>(gfx_input_stream)), offset, whence);
            return res_lseek;
        },
        [](gfx_input_stream_ref gfx_input_stream) -> void
        {
            close(static_cast<int>(reinterpret_cast<intptr_t>(gfx_input_stream)));
        });
}

static_assert(SEEK_SET == PT_GFX_INPUT_STREAM_SEEK_SET, "");
static_assert(SEEK_CUR == PT_GFX_INPUT_STREAM_SEEK_CUR, "");
static_assert(SEEK_END == PT_GFX_INPUT_STREAM_SEEK_END, "");