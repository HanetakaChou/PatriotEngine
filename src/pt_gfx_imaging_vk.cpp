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

#include "pt_gfx_imaging_vk.h"
#include "pt_mcrt_malloc.h"
#include <new>

gfx_iimaging_vk *gfx_imaging_vk_init(struct wsi_iwindow *window)
{
    gfx_iimaging_vk *imaging = new (mcrt_aligned_malloc(sizeof(gfx_iimaging_vk), alignof(gfx_iimaging_vk))) gfx_iimaging_vk();
    window->listen_size_change(
        [](void *connection, void *window, float width, float height, void *user_data) -> void {
            static_cast<gfx_iimaging_vk *>(user_data)->size_change_callback(connection, window, width, height);
        },
        imaging);
    window->listen_draw_request(
        [](void *connection, void *window, void *user_data) -> void {
            static_cast<gfx_iimaging_vk *>(user_data)->draw_request_callback(connection, window);
        },
        imaging);

    if (imaging->init())
    {
        return imaging;
    }
    else
    {
        imaging->destroy();
        return NULL;
    }
}

bool gfx_iimaging_vk::init()
{
    return true;
}

gfx_iimaging_vk::~gfx_iimaging_vk()
{
    
}

void gfx_iimaging_vk::destroy()
{
    this->~gfx_iimaging_vk();
    mcrt_free(this);
}
