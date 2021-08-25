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

#ifndef _PT_WSI_WINDOW_APP_H_
#define _PT_WSI_WINDOW_APP_H_ 1

#include <pt_gfx_connection.h>

typedef struct _wsi_window_app_t_ *wsi_window_app_ref;

wsi_window_app_ref wsi_window_app_init(gfx_connection_ref gfx_connection);

struct wsi_window_app_event_t
{
    enum
    {
        MESSAGE_CODE_KEY_PRESS,
        MESSAGE_CODE_QUIT
    };

    enum
    {
        KEY_SYM_W,
    };

    int32_t message_code;
    int32_t message_data1;
    int32_t message_data2;
};

int wsi_window_app_main(wsi_window_app_ref wsi_window_app);

void wsi_window_app_handle_event(wsi_window_app_ref wsi_window_app, struct wsi_window_app_event_t *wsi_window_app_event);

void wsi_window_app_destroy(wsi_window_app_ref wsi_window_app);

#endif