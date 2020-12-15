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

#ifndef _GFX_IMAGING_VK_H_
#define _GFX_IMAGING_VK_H_ 1

#include <pt_gfx_connection.h>

class gfx_iconnection_vk : public gfx_iconnection
{
    ~gfx_iconnection_vk();

public:
    bool init();
    void destroy() override;
    void size_change_callback(void *wsi_connection, void *window, float width, float height);
    void draw_request_callback(void *wsi_connection, void *window);
};

gfx_iconnection_vk *gfx_connection_vk_init(struct wsi_iwindow *window);

#endif