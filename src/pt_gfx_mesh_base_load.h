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

#ifndef _PT_GFX_MESH_BASE_LOAD_H_
#define _PT_GFX_MESH_BASE_LOAD_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_gfx_connection.h>

enum gfx_mesh_neutral_index_type_t
{
    PT_GFX_MESH_NEUTRAL_INDEX_TYPE_UINT16 = 0,
    PT_GFX_MESH_NEUTRAL_INDEX_TYPE_UINT32 = 1
};

struct gfx_mesh_neutral_header_t
{
    uint32_t vertex_count;
    enum gfx_mesh_neutral_index_type_t index_type;
    uint32_t index_count;
};

struct gfx_mesh_neutral_memcpy_dest_t
{
    size_t staging_offset;
    size_t output_size;
};

#endif