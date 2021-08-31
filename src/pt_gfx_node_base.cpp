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

#include <stddef.h>
#include <stdint.h>
#include "pt_gfx_node_base.h"

void gfx_node_base::set_mesh(class gfx_connection_base *gfx_connection, class gfx_mesh_base *gfx_mesh)
{
    class gfx_mesh_base *gfx_mesh_old = mcrt_atomic_xchg_ptr(&this->m_gfx_mesh, gfx_mesh);

    if (gfx_mesh != gfx_mesh_old)
    {
        if (NULL != gfx_mesh)
        {
            gfx_mesh->addref();
        }

        if (NULL != gfx_mesh_old)
        {
            gfx_mesh_old->release(gfx_connection);
        }
    }
}

void gfx_node_base::set_material(class gfx_connection_base *gfx_connection, class gfx_material_base *gfx_material)
{
    class gfx_material_base *gfx_material_old = mcrt_atomic_xchg_ptr(&this->m_gfx_material, gfx_material);

    if (gfx_material != gfx_material_old)
    {
        if (NULL != gfx_material)
        {
            gfx_material->addref();
        }

        if (NULL != gfx_material_old)
        {
            gfx_material_old->release(gfx_connection);
        }
    }
}

void gfx_node_base::destroy(class gfx_connection_base *gfx_connection)
{
    class gfx_mesh_base *gfx_mesh_old = mcrt_atomic_xchg_ptr(&this->m_gfx_mesh, static_cast<class gfx_mesh_base *>(NULL));
    gfx_mesh_old->release(gfx_connection);

    class gfx_material_base *gfx_material_old = mcrt_atomic_xchg_ptr(&this->m_gfx_material, static_cast<class gfx_material_base *>(NULL));
    gfx_material_old->release(gfx_connection);

    gfx_connection->frame_node_destroy_list_push(this);
}

// API
inline pt_gfx_connection_ref wrap(class gfx_connection_base *gfx_connection) { return reinterpret_cast<pt_gfx_connection_ref>(gfx_connection); }
inline class gfx_connection_base *unwrap(pt_gfx_connection_ref gfx_connection) { return reinterpret_cast<class gfx_connection_base *>(gfx_connection); }

inline pt_gfx_node_ref wrap(class gfx_node_base *node) { return reinterpret_cast<pt_gfx_node_ref>(node); }
inline class gfx_node_base *unwrap(pt_gfx_node_ref node) { return reinterpret_cast<class gfx_node_base *>(node); }

inline pt_gfx_mesh_ref wrap(class gfx_mesh_base *mesh) { return reinterpret_cast<pt_gfx_mesh_ref>(mesh); }
inline class gfx_mesh_base *unwrap(pt_gfx_mesh_ref mesh) { return reinterpret_cast<class gfx_mesh_base *>(mesh); }

inline pt_gfx_material_ref wrap(class gfx_material_base *gfx_material) { return reinterpret_cast<pt_gfx_material_ref>(gfx_material); }
inline class gfx_material_base *unwrap(pt_gfx_material_ref gfx_material) { return reinterpret_cast<class gfx_material_base *>(gfx_material); }

PT_ATTR_GFX void PT_CALL pt_gfx_node_set_mesh(pt_gfx_connection_ref gfx_connection, pt_gfx_node_ref gfx_node, pt_gfx_mesh_ref gfx_mesh)
{
    return unwrap(gfx_node)->set_mesh(unwrap(gfx_connection), unwrap(gfx_mesh));
}

PT_ATTR_GFX void PT_CALL pt_gfx_node_set_material(pt_gfx_connection_ref gfx_connection, pt_gfx_node_ref gfx_node, pt_gfx_material_ref gfx_material)
{
    return unwrap(gfx_node)->set_material(unwrap(gfx_connection), unwrap(gfx_material));
}

PT_ATTR_GFX void PT_CALL pt_gfx_node_destroy(pt_gfx_connection_ref gfx_connection, pt_gfx_node_ref gfx_node)
{
    return unwrap(gfx_node)->destroy(unwrap(gfx_connection));
}
