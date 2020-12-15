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

#include <pt_mcrt_malloc.h>
#include "pt_gfx_connection_vk.h"
#include <new>

gfx_iconnection_vk *gfx_connection_vk_init(struct wsi_iwindow *window)
{
    gfx_iconnection_vk *connection = new (mcrt_aligned_malloc(sizeof(gfx_iconnection_vk), alignof(gfx_iconnection_vk))) gfx_iconnection_vk();
    window->listen_size_change(
        [](void *connection, void *window, float width, float height, void *user_data) -> void {
            static_cast<gfx_iconnection_vk *>(user_data)->size_change_callback(connection, window, width, height);
        },
        connection);
    window->listen_draw_request(
        [](void *connection, void *window, void *user_data) -> void {
            static_cast<gfx_iconnection_vk *>(user_data)->draw_request_callback(connection, window);
        },
        connection);

    if (connection->init())
    {
        return connection;
    }
    else
    {
        connection->destroy();
        return NULL;
    }
}

bool gfx_iconnection_vk::init()
{
    return true;
}

gfx_iconnection_vk::~gfx_iconnection_vk()
{
    
}

void gfx_iconnection_vk::destroy()
{
    this->~gfx_iconnection_vk();
    mcrt_free(this);
}
