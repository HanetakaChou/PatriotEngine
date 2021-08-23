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

#ifndef _PT_GFX_NODE_BASE_H_
#define _PT_GFX_NODE_BASE_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_gfx_connection.h>
#include <pt_math.h>
#include "pt_gfx_mesh_base.h"
#include "pt_gfx_material_base.h"

class gfx_node_base
{
    math_alignas16_mat4x4 m_mat_m;

    class gfx_mesh_base *m_gfx_mesh;

    class gfx_material_base *m_gfx_material;

protected:
    inline gfx_node_base() : m_gfx_mesh(NULL), m_gfx_material(NULL)
    {
        math_store_mat4x4(&this->m_mat_m, math_mat_identity());
    }

public:
    void set_mesh(class gfx_connection_base *gfx_connection, class gfx_mesh_base *gfx_mesh);

    inline class gfx_mesh_base *get_mesh() const { return this->m_gfx_mesh; }

    void set_material(class gfx_connection_base *gfx_connection, class gfx_material_base *gfx_material);

    void destroy(class gfx_connection_base *gfx_connection);
};

#endif