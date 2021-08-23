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

#ifndef _PT_GFX_MATERIAL_BASE_H_
#define _PT_GFX_MATERIAL_BASE_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_gfx_connection.h>

class gfx_material_base
{

protected:
    inline gfx_material_base() {}

public:
    virtual bool init_with_texture(class gfx_connection_base *gfx_connection, uint32_t material_model, uint32_t texture_count, gfx_texture_ref *gfx_textures) = 0;

    virtual void destroy(class gfx_connection_base *gfx_connection) = 0;
};

#endif