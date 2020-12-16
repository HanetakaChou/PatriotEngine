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

#ifndef _PT_APP_MAIN_H_
#define _PT_APP_MAIN_H_ 1

#include <pt_wsi_window.h>
#include <pt_gfx_connection.h>

struct app_iwindow
{
    struct input_event_t
    {
        
    };

    virtual void listen_input_event(void (*input_event_callback)(struct input_event_t *input_event, void *user_data), void *user_data) = 0;

    virtual void mark_app_running() = 0;
};

void app_main(struct app_iwindow *input, struct gfx_iconnection *connection);

#endif