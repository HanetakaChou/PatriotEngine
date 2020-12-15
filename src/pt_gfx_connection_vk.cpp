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

#include <stddef.h>
#include <new>
#include <pt_mcrt_malloc.h>
#include "pt_gfx_connection_vk.h"
#include "pt_gfx_texture_vk.h"

gfx_connection_vk *gfx_connection_vk_init(struct wsi_iwindow *window)
{
    gfx_connection_vk *connection = new (mcrt_aligned_malloc(sizeof(gfx_connection_vk), alignof(gfx_connection_vk))) gfx_connection_vk();
    window->listen_size_change(
        [](void *connection, void *window, float width, float height, void *user_data) -> void {
            static_cast<gfx_connection_vk *>(user_data)->size_change_callback(connection, window, width, height);
        },
        connection);
    window->listen_draw_request(
        [](void *connection, void *window, void *user_data) -> void {
            static_cast<gfx_connection_vk *>(user_data)->draw_request_callback(connection, window);
        },
        connection);

    if (connection->init())
    {
        return connection;
    }
    else
    {
        mcrt_free(connection);
        return NULL;
    }
}

bool gfx_connection_vk::init()
{
    return true;
}

struct gfx_itexture *gfx_connection_vk::create_texture()
{
    gfx_texture_vk *texture = new (mcrt_aligned_malloc(sizeof(gfx_texture_vk), alignof(gfx_texture_vk))) gfx_texture_vk();
    return texture;
}

gfx_connection_vk::~gfx_connection_vk()
{

}

void gfx_connection_vk::destroy()
{
    this->~gfx_connection_vk();
    mcrt_free(this);
}
