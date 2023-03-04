//
// Copyright (C) YuqiaoZhang(HanetakaChou)
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

void gfx_node_base::set_mesh(class gfx_connection_base *gfx_connection, class gfx_mesh_base *future_mesh)
{
    class gfx_mesh_base *current_mesh = mcrt_atomic_xchg_ptr(&this->m_mesh, future_mesh);

    if (future_mesh != current_mesh)
    {
        if (NULL != future_mesh)
        {
            future_mesh->addref();
        }

        if (NULL != current_mesh)
        {
            current_mesh->release(gfx_connection);
        }
    }
}

void gfx_node_base::set_material(class gfx_connection_base *gfx_connection, uint32_t material_index, class gfx_material_base *future_material)
{
    if (material_index < this->m_material_count)
    {
        class gfx_material_base *current_material = mcrt_atomic_xchg_ptr(&this->m_materials[material_index], future_material);
        if (future_material != current_material)
        {
            if (NULL != future_material)
            {
                future_material->addref();
            }

            if (NULL != current_material)
            {
                current_material->release(gfx_connection);
            }
        }
    }
    else
    {
        assert(0);
    }

}

class gfx_material_base* gfx_node_base::get_material(uint32_t material_index) const
{ 
    if (material_index < this->m_material_count)
    {
        return this->m_materials[material_index];
    }
    else
    {
        assert(0);
        return NULL;
    }
}

void gfx_node_base::destroy(class gfx_connection_base *gfx_connection)
{
    this->set_mesh(gfx_connection, NULL);

    for (uint32_t material_index = 0U; material_index < this->m_material_count; ++material_index)
    {
        this->set_material(gfx_connection, material_index, NULL);
    }

    mcrt_aligned_free(this->m_materials);

    gfx_connection->frame_node_destroy_list_push(this);
}

// API
static inline pt_gfx_connection_ref wrap(class gfx_connection_base *gfx_connection) { return reinterpret_cast<pt_gfx_connection_ref>(gfx_connection); }
static inline class gfx_connection_base *unwrap(pt_gfx_connection_ref gfx_connection) { return reinterpret_cast<class gfx_connection_base *>(gfx_connection); }

static inline pt_gfx_node_ref wrap(class gfx_node_base *node) { return reinterpret_cast<pt_gfx_node_ref>(node); }
static inline class gfx_node_base *unwrap(pt_gfx_node_ref node) { return reinterpret_cast<class gfx_node_base *>(node); }

static inline pt_gfx_mesh_ref wrap(class gfx_mesh_base *mesh) { return reinterpret_cast<pt_gfx_mesh_ref>(mesh); }
static inline class gfx_mesh_base *unwrap(pt_gfx_mesh_ref mesh) { return reinterpret_cast<class gfx_mesh_base *>(mesh); }

static inline pt_gfx_material_ref wrap(class gfx_material_base *gfx_material) { return reinterpret_cast<pt_gfx_material_ref>(gfx_material); }
static inline class gfx_material_base *unwrap(pt_gfx_material_ref gfx_material) { return reinterpret_cast<class gfx_material_base *>(gfx_material); }

PT_ATTR_GFX void PT_CALL pt_gfx_node_set_mesh(pt_gfx_connection_ref gfx_connection, pt_gfx_node_ref gfx_node, pt_gfx_mesh_ref gfx_mesh)
{
    return unwrap(gfx_node)->set_mesh(unwrap(gfx_connection), unwrap(gfx_mesh));
}

PT_ATTR_GFX void PT_CALL pt_gfx_node_set_material(pt_gfx_connection_ref gfx_connection, pt_gfx_node_ref gfx_node, uint32_t material_index, pt_gfx_material_ref gfx_material)
{
    return unwrap(gfx_node)->set_material(unwrap(gfx_connection), material_index, unwrap(gfx_material));
}

PT_ATTR_GFX void PT_CALL pt_gfx_node_destroy(pt_gfx_connection_ref gfx_connection, pt_gfx_node_ref gfx_node)
{
    return unwrap(gfx_node)->destroy(unwrap(gfx_connection));
}
