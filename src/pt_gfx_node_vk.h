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

#ifndef _PT_GFX_NODE_VK_H_
#define _PT_GFX_NODE_VK_H_ 1

#include <stddef.h>
#include <stdint.h>
#include "pt_gfx_node_base.h"

class gfx_node_vk final : public gfx_node_base
{
    class gfx_mesh_vk *m_gfx_mesh;

    void set_mesh(class gfx_connection_base *gfx_connection, class gfx_mesh_base *gfx_mesh) override;

    void destroy(class gfx_connection_base *gfx_connection) override;

public:
    size_t m_frame_node_index;

    void frame_destroy_callback(class gfx_connection_vk *gfx_connection);

    inline class gfx_mesh_vk *get_mesh() { return m_gfx_mesh; }

    inline gfx_node_vk() : gfx_node_base(), m_gfx_mesh(NULL), m_frame_node_index(-1){};
};

#endif