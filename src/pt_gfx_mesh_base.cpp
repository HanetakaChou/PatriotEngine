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
#include "pt_gfx_mesh_base.h"

//--- export ---

inline gfx_connection_ref wrap(class gfx_connection_common *gfx_connection) { return reinterpret_cast<gfx_connection_ref>(gfx_connection); }
inline class gfx_connection_common *unwrap(gfx_connection_ref gfx_connection) { return reinterpret_cast<class gfx_connection_common *>(gfx_connection); }

inline gfx_mesh_ref wrap(class gfx_mesh_base *mesh) { return reinterpret_cast<gfx_mesh_ref>(mesh); }
inline class gfx_mesh_base *unwrap(gfx_mesh_ref mesh) { return reinterpret_cast<class gfx_mesh_base *>(mesh); }

PT_ATTR_GFX bool PT_CALL gfx_mesh_read_input_stream(gfx_connection_ref gfx_connection, gfx_mesh_ref mesh, uint32_t mesh_index, uint32_t material_index, char const *initial_filename, gfx_input_stream_ref(PT_PTR *input_stream_init_callback)(char const *initial_filename), intptr_t(PT_PTR *input_stream_read_callback)(gfx_input_stream_ref input_stream, void *buf, size_t count), int64_t(PT_PTR *input_stream_seek_callback)(gfx_input_stream_ref input_stream, int64_t offset, int whence), void(PT_PTR *input_stream_destroy_callback)(gfx_input_stream_ref input_stream))
{
    return unwrap(mesh)->read_input_stream(unwrap(gfx_connection), mesh_index, material_index, initial_filename, input_stream_init_callback, input_stream_read_callback, input_stream_seek_callback, input_stream_destroy_callback);
}

PT_ATTR_GFX void PT_CALL gfx_mesh_destroy(gfx_connection_ref gfx_connection, gfx_mesh_ref mesh)
{
    return unwrap(mesh)->destroy(unwrap(gfx_connection));
}