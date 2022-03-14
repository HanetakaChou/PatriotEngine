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
#include "pt_gfx_scene_vk.h"
#include "pt_gfx_connection_vk.h"

void gfx_scene_vk::set_camera(class gfx_connection_base *gfx_connection, pt_math_vec3 const *eye_position, pt_math_vec3 const *eye_direction, pt_math_vec3 const *up_direction)
{
  pt_math_simd_mat camera_view_transform = pt_math_mat_look_to_rh(pt_math_load_vec3(eye_position), pt_math_load_vec3(eye_direction), pt_math_load_vec3(up_direction));

  // vulkan viewport flip y
  pt_math_alignas16_mat4x4 flip_y_transform;
  flip_y_transform.m[0][0] = 1.0f;
  flip_y_transform.m[0][1] = 0.0f;
  flip_y_transform.m[0][2] = 0.0f;
  flip_y_transform.m[0][3] = 0.0f;
  flip_y_transform.m[1][0] = 0.0f;
  flip_y_transform.m[1][1] = -1.0f;
  flip_y_transform.m[1][2] = 0.0f;
  flip_y_transform.m[1][3] = 0.0f;
  flip_y_transform.m[2][0] = 0.0f;
  flip_y_transform.m[2][1] = 0.0f;
  flip_y_transform.m[2][2] = 1.0f;
  flip_y_transform.m[2][3] = 0.0f;
  flip_y_transform.m[3][0] = 0.0f;
  flip_y_transform.m[3][1] = 0.0f;
  flip_y_transform.m[3][2] = 0.0f;
  flip_y_transform.m[3][3] = 1.0f;
  pt_math_simd_mat camera_perspective_transform = pt_math_mat_multiply(pt_math_mat_perspective_fov_rh(0.785f, gfx_connection->get_aspect_ratio(), 0.1f, 100.f), pt_math_load_alignas16_mat4x4(&flip_y_transform));

  // directxmath // row vector + row major
  // glsl // colum_major equivalent transpose
  pt_math_store_alignas16_mat4x4(&this->m_camera_view_perspective_transform, pt_math_mat_multiply(camera_view_transform, camera_perspective_transform));
}