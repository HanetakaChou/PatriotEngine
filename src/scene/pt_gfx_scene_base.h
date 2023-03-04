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

#ifndef _PT_GFX_SCENE_BASE_H_
#define _PT_GFX_SCENE_BASE_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_math.h>
#include "pt_gfx_node_base.h"
#include "pt_gfx_connection.h"

class gfx_scene_base
{
    class gfx_node_base *m_root_node;

protected:
    pt_math_alignas16_mat4x4 m_camera_view_perspective_transform;
    
    inline gfx_scene_base() {};

public:
    virtual void set_camera(class gfx_connection_base *gfx_connection, pt_math_vec3 const *eye_position, pt_math_vec3 const *eye_direction, pt_math_vec3 const *up_direction) = 0;
};

#endif