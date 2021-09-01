//
// Copyright (C) YuqiaoZhang(HanetakaYuminaga)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <vector>
#include <X11/keysym.h>
#include <xcb/xcb.h>
#include <pt_mcrt_memcpy.h>
#include <pt_mcrt_atomic.h>
#include <pt_mcrt_thread.h>
#include <pt_mcrt_scalable_allocator.h>
#include <pt_wsi_main.h>

class wsi_linux_x11
{
    template <typename T>
    using mcrt_vector = std::vector<T, mcrt::scalable_allocator<T>>;

    xcb_connection_t *m_xcb_connection;
    xcb_setup_t const *m_setup;
    xcb_screen_t *m_screen;
    xcb_visualid_t m_visual;
    xcb_window_t m_window;
    float m_window_width;
    float m_window_height;
    xcb_atom_t m_atom_wm_protocols;
    xcb_atom_t m_atom_wm_delete_window;
    mcrt_native_thread_id m_draw_main_thread_id;
    mcrt_native_thread_id m_app_main_thread_id;
    bool m_x11_main_running;
    xcb_keycode_t m_min_keycode;
    xcb_keycode_t m_max_keycode;
    mcrt_vector<xcb_keysym_t> m_keysym;
    uint8_t m_keysyms_per_keycode;
    void sync_keysyms();
    xcb_keysym_t keycode_to_keysym(xcb_keycode_t keycode);

    // TODO
    // padding for cache line

    pt_gfx_connection_ref m_gfx_connection;
    bool m_draw_main_running;
    static void *draw_main(void *);

    struct app_main_argument_t
    {
        class wsi_linux_x11 *m_instance;
        pt_wsi_app_ref(PT_PTR *m_wsi_app_init_callback)(pt_gfx_connection_ref);
        int(PT_PTR *m_wsi_app_main_callback)(pt_wsi_app_ref);
    };
    pt_wsi_app_ref m_wsi_app;
    bool m_app_main_running;
    static void *app_main(void *);

    static inline pt_gfx_wsi_connection_ref wrap_wsi_connection(xcb_connection_t *wsi_connection);
    static inline pt_gfx_wsi_visual_ref wrap_wsi_visual(xcb_visualid_t wsi_visual);
    static inline pt_gfx_wsi_window_ref wrap_wsi_window(xcb_window_t wsi_window);

public:
    void init(pt_wsi_app_ref(PT_PTR *wsi_app_init_callback)(pt_gfx_connection_ref), int(PT_PTR *wsi_app_main_callback)(pt_wsi_app_ref));
    int main();
};

PT_ATTR_WSI int PT_CALL pt_wsi_main(int argc, char *argv[], pt_wsi_app_ref(PT_PTR *wsi_app_init_callback)(pt_gfx_connection_ref), int(PT_PTR *wsi_app_main_callback)(pt_wsi_app_ref))
{
    wsi_linux_x11 instance;
    instance.init(wsi_app_init_callback, wsi_app_main_callback);
    return instance.main();
}

void wsi_linux_x11::init(pt_wsi_app_ref(PT_PTR *wsi_app_init_callback)(pt_gfx_connection_ref), int(PT_PTR *wsi_app_main_callback)(pt_wsi_app_ref))
{
    int scr;
    this->m_xcb_connection = xcb_connect(NULL, &scr);
    assert(xcb_connection_has_error(this->m_xcb_connection) == 0);

    this->m_setup = xcb_get_setup(this->m_xcb_connection);

    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(this->m_setup);
    for (int i = 0; i < scr; ++i)
    {
        xcb_screen_next(&iter);
    }
    this->m_screen = iter.data;

    this->m_visual = m_screen->root_visual;

    // CreateWindowExW
    this->m_window = xcb_generate_id(m_xcb_connection);

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

    xcb_intern_atom_cookie_t cookie_net_wm_name = xcb_intern_atom(this->m_xcb_connection, 0, 12U, "_NET_WM_NAME");
    xcb_intern_atom_cookie_t cookie_utf8_string = xcb_intern_atom(this->m_xcb_connection, 0, 11U, "UTF8_STRING");
    xcb_intern_atom_cookie_t cookie_wm_protocols = xcb_intern_atom(this->m_xcb_connection, 0, 12U, "WM_PROTOCOLS");
    xcb_intern_atom_cookie_t cookie_wm_delete_window = xcb_intern_atom(this->m_xcb_connection, 0, 16U, "WM_DELETE_WINDOW");

    xcb_generic_error_t *error_generic = xcb_request_check(this->m_xcb_connection, cookie_create_window); //implicit xcb_flush
    assert(error_generic == NULL);

    xcb_intern_atom_reply_t *reply_net_wm_name = xcb_intern_atom_reply(this->m_xcb_connection, cookie_net_wm_name, &error_generic);
    assert(error_generic == NULL);
    xcb_atom_t atom_net_wm_name = reply_net_wm_name->atom;
    free(reply_net_wm_name);

    xcb_intern_atom_reply_t *reply_utf8_string = xcb_intern_atom_reply(this->m_xcb_connection, cookie_utf8_string, &error_generic);
    assert(error_generic == NULL);
    xcb_atom_t atom_utf8_string = reply_utf8_string->atom;
    free(reply_utf8_string);

    xcb_intern_atom_reply_t *reply_wm_protocols = xcb_intern_atom_reply(this->m_xcb_connection, cookie_wm_protocols, &error_generic);
    assert(error_generic == NULL);
    this->m_atom_wm_protocols = reply_wm_protocols->atom;
    free(reply_wm_protocols);

    xcb_intern_atom_reply_t *reply_wm_delete_window = xcb_intern_atom_reply(m_xcb_connection, cookie_wm_delete_window, &error_generic);
    assert(error_generic == NULL);
    this->m_atom_wm_delete_window = reply_wm_delete_window->atom;
    free(reply_wm_delete_window);

    xcb_void_cookie_t cookie_change_property_net_wm_name = xcb_change_property_checked(this->m_xcb_connection, XCB_PROP_MODE_REPLACE, this->m_window, atom_net_wm_name, atom_utf8_string, 8U, 13U, "PatriotEngine");

    xcb_void_cookie_t cookie_change_property_wm_protocols = xcb_change_property_checked(this->m_xcb_connection, XCB_PROP_MODE_REPLACE, this->m_window, this->m_atom_wm_protocols, XCB_ATOM_ATOM, 32U, 1U, &this->m_atom_wm_delete_window);

    // ShowWindow
    xcb_void_cookie_t cookie_map_window = xcb_map_window_checked(this->m_xcb_connection, this->m_window);

    error_generic = xcb_request_check(this->m_xcb_connection, cookie_change_property_net_wm_name);
    assert(error_generic == NULL);

    error_generic = xcb_request_check(this->m_xcb_connection, cookie_change_property_wm_protocols);
    assert(error_generic == NULL);

    error_generic = xcb_request_check(this->m_xcb_connection, cookie_map_window);
    assert(error_generic == NULL);

    // draw_main
    {
        this->m_gfx_connection = NULL;
        mcrt_atomic_store(&this->m_draw_main_running, false);

        PT_MAYBE_UNUSED bool res_native_thread_create = mcrt_native_thread_create(&m_draw_main_thread_id, draw_main, this);
        assert(res_native_thread_create);

        while (!mcrt_atomic_load(&this->m_draw_main_running))
        {
            mcrt_os_yield();
        }

        assert(this->m_gfx_connection != NULL);
    }

    // app_main
    {
        struct app_main_argument_t app_main_argument;
        app_main_argument.m_instance = this;
        app_main_argument.m_wsi_app_init_callback = wsi_app_init_callback;
        app_main_argument.m_wsi_app_main_callback = wsi_app_main_callback;
        mcrt_atomic_store(&this->m_app_main_running, false);

        PT_MAYBE_UNUSED bool res_native_thread_create = mcrt_native_thread_create(&m_app_main_thread_id, app_main, &app_main_argument);
        assert(res_native_thread_create);

        while (!mcrt_atomic_load(&this->m_app_main_running))
        {
            mcrt_os_yield();
        }

        assert(NULL != this->m_wsi_app);
    }

    //https://gitlab.freedesktop.org/xorg/lib/libxcb-keysyms
    //xcb_key_symbols_alloc
    //https://gitlab.freedesktop.org/xorg/lib/libxcb-keysyms
    //xcb_key_symbols_get_keysym
    this->sync_keysyms();

    // x11_main
    mcrt_atomic_store(&this->m_x11_main_running, true);
}

void wsi_linux_x11::sync_keysyms()
{
    this->m_min_keycode = this->m_setup->min_keycode;
    this->m_max_keycode = this->m_setup->max_keycode;

    xcb_get_keyboard_mapping_cookie_t cookie_get_keyboard_mapping = xcb_get_keyboard_mapping(this->m_xcb_connection, this->m_min_keycode, (this->m_max_keycode - this->m_min_keycode) + 1);

    xcb_generic_error_t *error_generic;
    xcb_get_keyboard_mapping_reply_t *reply_get_keyboard_mapping = xcb_get_keyboard_mapping_reply(this->m_xcb_connection, cookie_get_keyboard_mapping, &error_generic);
    assert(error_generic == NULL);

    xcb_keysym_t *keysym_begin = xcb_get_keyboard_mapping_keysyms(reply_get_keyboard_mapping);
    xcb_generic_iterator_t iter_keysym_end = xcb_get_keyboard_mapping_keysyms_end(reply_get_keyboard_mapping);
    xcb_keysym_t *keysym_end = static_cast<xcb_keysym_t *>(iter_keysym_end.data);
    assert((sizeof(xcb_keysym_t) * (keysym_end - keysym_begin)) == (iter_keysym_end.index - sizeof(xcb_get_keyboard_mapping_reply_t)));

    this->m_keysym.resize(keysym_end - keysym_begin);
    assert(sizeof(this->m_keysym[0]) == sizeof(xcb_keysym_t));
    mcrt_memcpy(&this->m_keysym[0], keysym_begin, sizeof(xcb_keysym_t) * (keysym_end - keysym_begin));

    this->m_keysyms_per_keycode = reply_get_keyboard_mapping->keysyms_per_keycode;
    assert((keysym_end - keysym_begin) == this->m_keysyms_per_keycode * ((this->m_max_keycode - this->m_min_keycode) + 1));

    free(reply_get_keyboard_mapping);

    //To Do: Use the keycode directly?
    // https://www.x.org/releases/X11R7.7/doc/xorg-docs/input/XKB-Config.html
    // On Linux systems, the evdev rules are most commonly used, on other systems the base rules are used.
    // /usr/share/X11/xkb/keycodes/evdev
    // #define KEY_ESC 9
    // libxkbcommon/tools/interactive-x11.c
}

xcb_keysym_t wsi_linux_x11::keycode_to_keysym(xcb_keycode_t keycode)
{
    //https://gitlab.freedesktop.org/xorg/lib/libxcb-keysyms
    //xcb_key_symbols_get_keysym

    if (keycode >= this->m_min_keycode && keycode <= this->m_max_keycode)
    {
        assert((this->m_keysyms_per_keycode * (keycode - this->m_min_keycode)) < static_cast<uint8_t>(this->m_keysym.size()));
        return this->m_keysym[this->m_keysyms_per_keycode * (keycode - this->m_min_keycode)];
    }
    else
    {
        return XCB_NO_SYMBOL;
    }
}

int wsi_linux_x11::main()
{
    xcb_generic_event_t *event;

    while (mcrt_atomic_load(&this->m_x11_main_running) && ((event = xcb_wait_for_event(this->m_xcb_connection)) != NULL))
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

                    //struct wsi_neutral_app_input_event_t wsi_neutral_app_input_event = {MESSAGE_CODE_KEY_PRESS, KEY_SYM_W, 0};
                    //wsi_neutral_app_handle_input_event(&wsi_neutral_app_input_event, this->m_neutral_app_void_instance);
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
            pt_gfx_connection_on_wsi_window_resized(this->m_gfx_connection, this->m_window_width, this->m_window_height);
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
                mcrt_atomic_store(&this->m_x11_main_running, false);
                mcrt_atomic_store(&this->m_draw_main_running, false);
                mcrt_atomic_store(&this->m_app_main_running, false);

                //mcrt_atomic_store(&m_app_has_quit, false);
                //struct wsi_neutral_app_input_event_t wsi_neutral_app_input_event = {MESSAGE_CODE_QUIT, 0, 0};
                //wsi_neutral_app_handle_input_event(&wsi_neutral_app_input_event, this->m_neutral_app_void_instance);

                // we can't destory window here
                // we need to destory API related vksurface etc first

                // xcb_destroy_window_checked
                // xcb_request_check
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

    mcrt_native_thread_join(this->m_app_main_thread_id);

    mcrt_native_thread_join(this->m_draw_main_thread_id);

    {
        xcb_void_cookie_t cookie_destroy_window = xcb_destroy_window_checked(m_xcb_connection, m_window);
        PT_MAYBE_UNUSED xcb_generic_error_t *error_generic = xcb_request_check(m_xcb_connection, cookie_destroy_window); //implicit xcb_flush
        assert(error_generic == NULL);
    }

    xcb_disconnect(m_xcb_connection);

    return 0;
}

inline pt_gfx_wsi_connection_ref wsi_linux_x11::wrap_wsi_connection(xcb_connection_t *wsi_connection)
{
    return reinterpret_cast<pt_gfx_wsi_connection_ref>(wsi_connection);
}

inline pt_gfx_wsi_visual_ref wsi_linux_x11::wrap_wsi_visual(xcb_visualid_t wsi_visual)
{
    return reinterpret_cast<pt_gfx_wsi_visual_ref>(static_cast<uintptr_t>(wsi_visual));
}

inline pt_gfx_wsi_window_ref wsi_linux_x11::wrap_wsi_window(xcb_window_t wsi_window)
{
    return reinterpret_cast<pt_gfx_wsi_window_ref>(static_cast<uintptr_t>(wsi_window));
}

void *wsi_linux_x11::draw_main(void *argument)
{
    class wsi_linux_x11 *instance = static_cast<class wsi_linux_x11 *>(argument);

    instance->m_gfx_connection = pt_gfx_connection_init(wrap_wsi_connection(instance->m_xcb_connection), wrap_wsi_visual(instance->m_visual), ".");
    assert(instance->m_gfx_connection != NULL);
    mcrt_atomic_store(&instance->m_draw_main_running, true);

    pt_gfx_connection_on_wsi_window_created(instance->m_gfx_connection, wrap_wsi_connection(instance->m_xcb_connection), wrap_wsi_window(instance->m_window), instance->m_window_height, instance->m_window_width);

    while (mcrt_atomic_load(&instance->m_draw_main_running))
    {
        // usage
        // on_redraw_needed
        // app update scenetree //maybe in other thread //not depend on accurate time
        // gfx acquire //sync and flatten scenetree //then frame throttling
        // app update time-related (animation etc) //frame throttling make the time here less latency //scenetree modify will impact on the next frame
        // gfx release //draw and present //draw //not sync scenetree

        // update scenetree
        pt_gfx_connection_draw_acquire(instance->m_gfx_connection);

        // TODO
        // output visibility set

        // update animation etc
        pt_gfx_connection_draw_release(instance->m_gfx_connection);
    }

    pt_gfx_connection_on_wsi_window_destroyed(instance->m_gfx_connection);

    pt_gfx_connection_destroy(instance->m_gfx_connection);

    return NULL;
}

void *wsi_linux_x11::app_main(void *argument_void)
{
    pt_wsi_app_ref wsi_app;
    int(PT_PTR * wsi_app_main_callback)(pt_wsi_app_ref);
    // app_init
    {
        struct app_main_argument_t *argument = static_cast<struct app_main_argument_t *>(argument_void);
        wsi_app = argument->m_wsi_app_init_callback(argument->m_instance->m_gfx_connection);
        wsi_app_main_callback = argument->m_wsi_app_main_callback;
        argument->m_instance->m_wsi_app = wsi_app;
        mcrt_atomic_store(&argument->m_instance->m_app_main_running, true);
    }

    // app_main
    int res_app_main_callback = wsi_app_main_callback(wsi_app);

    // mcrt_atomic_store(&self->m_loop, false);

    // wsi_window_app_destroy() //used in run //wsi_window_app_handle_event

    return reinterpret_cast<void *>(static_cast<intptr_t>(res_app_main_callback));
}