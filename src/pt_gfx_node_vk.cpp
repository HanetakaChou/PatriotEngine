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
#include <pt_mcrt_malloc.h>
#include <pt_mcrt_atomic.h>
#include "pt_gfx_node_vk.h"
#include "pt_gfx_mesh_vk.h"

void gfx_node_vk::post_stream_done_destroy_callback(class gfx_connection_vk *gfx_connection)
{
    this->~gfx_node_vk();
    mcrt_aligned_free(this);
}