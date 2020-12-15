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

#include <stdint.h>
#include "pt_gfx_connection_vk.h"
#include <xcb/xcb.h>

static_assert(sizeof(xcb_window_t) <= sizeof(void *), "sizeof(xcb_window_t) <= sizeof(void *)");

void gfx_connection_vk::size_change_callback(void *_wsi_connection, void *_window, float width, float height)
{
    xcb_connection_t *wsi_connection = static_cast<xcb_connection_t *>(_wsi_connection);
    xcb_window_t window = reinterpret_cast<uintptr_t>(_window);
}

void gfx_connection_vk::draw_request_callback(void *_wsi_connection, void *_window)
{
    xcb_connection_t *wsi_connection = static_cast<xcb_connection_t *>(_wsi_connection);
    xcb_window_t window = reinterpret_cast<uintptr_t>(_window);
}