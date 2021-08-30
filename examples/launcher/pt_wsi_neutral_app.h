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

#ifndef _PT_WSI_NEUTRAL_APP_H_
#define _PT_WSI_NEUTRAL_APP_H_ 1

#include <pt_gfx_connection.h>

enum
{
    MESSAGE_CODE_KEY_PRESS,
    MESSAGE_CODE_QUIT
};

enum
{
    KEY_SYM_W,
};

struct wsi_neutral_app_input_event_t
{

    int32_t message_code;
    int32_t message_data1;
    int32_t message_data2;
};

// This is the native instance of the application.  It is not used by the framework, but can be set by the application to its own instance state.
bool wsi_neutral_app_init(pt_gfx_connection_ref gfx_connection, void **void_instance);

int wsi_neutral_app_main(void *void_instance);

void wsi_neutral_app_handle_input_event(struct wsi_neutral_app_input_event_t const *wsi_neutral_app_input_event, void *void_instance);

#endif