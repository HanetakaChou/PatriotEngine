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
#include "pt_gfx_material_base.h"

//--- export ---

inline gfx_connection_ref wrap(class gfx_connection_base *gfx_connection) { return reinterpret_cast<gfx_connection_ref>(gfx_connection); }
inline class gfx_connection_base *unwrap(gfx_connection_ref gfx_connection) { return reinterpret_cast<class gfx_connection_base *>(gfx_connection); }

inline gfx_material_ref wrap(class gfx_material_base *gfx_material) { return reinterpret_cast<gfx_material_ref>(gfx_material); }
inline class gfx_material_base *unwrap(gfx_material_ref gfx_material) { return reinterpret_cast<class gfx_material_base *>(gfx_material); }

PT_ATTR_GFX bool PT_CALL gfx_material_init_with_texture(gfx_connection_ref gfx_connection, gfx_material_ref gfx_material, uint32_t material_model, uint32_t texture_count, gfx_texture_ref *gfx_textures)
{
    return unwrap(gfx_material)->init_with_texture(unwrap(gfx_connection), material_model, texture_count, gfx_textures);
}

PT_ATTR_GFX void PT_CALL gfx_material_destroy(gfx_connection_ref gfx_connection, gfx_material_ref gfx_material)
{
    return unwrap(gfx_material)->destroy(unwrap(gfx_connection));
}
