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

#include <stddef.h>
#include <stdint.h>
#include "pt_gfx_node_base.h"

//--- export ---

inline gfx_connection_ref wrap(class gfx_connection_base *gfx_connection) { return reinterpret_cast<gfx_connection_ref>(gfx_connection); }
inline class gfx_connection_base *unwrap(gfx_connection_ref gfx_connection) { return reinterpret_cast<class gfx_connection_base *>(gfx_connection); }

inline gfx_node_ref wrap(class gfx_node_base *node) { return reinterpret_cast<gfx_node_ref>(node); }
inline class gfx_node_base *unwrap(gfx_node_ref node) { return reinterpret_cast<class gfx_node_base *>(node); }

inline gfx_mesh_ref wrap(class gfx_mesh_base *mesh) { return reinterpret_cast<gfx_mesh_ref>(mesh); }
inline class gfx_mesh_base *unwrap(gfx_mesh_ref mesh) { return reinterpret_cast<class gfx_mesh_base *>(mesh); }

PT_ATTR_GFX void PT_CALL gfx_node_set_mesh(gfx_connection_ref gfx_connection, gfx_node_ref node, gfx_mesh_ref gfx_mesh)
{
    return unwrap(node)->set_mesh(unwrap(gfx_connection), unwrap(gfx_mesh));
}

PT_ATTR_GFX void PT_CALL gfx_node_destroy(gfx_connection_ref gfx_connection, gfx_node_ref node)
{
    return unwrap(node)->destroy(unwrap(gfx_connection));
}
