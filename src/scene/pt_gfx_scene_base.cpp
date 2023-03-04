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
#include "pt_gfx_scene_base.h"

// API
static inline pt_gfx_connection_ref wrap(class gfx_connection_base *gfx_connection) { return reinterpret_cast<pt_gfx_connection_ref>(gfx_connection); }
static inline class gfx_connection_base *unwrap(pt_gfx_connection_ref gfx_connection) { return reinterpret_cast<class gfx_connection_base *>(gfx_connection); }

static inline pt_scene_ref wrap(class gfx_scene_base *gfx_scene) { return reinterpret_cast<pt_scene_ref>(gfx_scene); }
static inline class gfx_scene_base *unwrap(pt_scene_ref gfx_scene) { return reinterpret_cast<class gfx_scene_base *>(gfx_scene); }

PT_ATTR_GFX void PT_CALL pt_gfx_scene_set_camera(pt_gfx_connection_ref gfx_connection, pt_scene_ref gfx_scene, pt_math_vec3 const *eye_position, pt_math_vec3 const *eye_direction, pt_math_vec3 const *up_direction)
{

}
