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

#ifndef _IMAGING_MESH_TANGENT_H_
#define _IMAGING_MESH_TANGENT_H_ 1

#include <stddef.h>
#include <stdint.h>
#include <pt_math.h>

// [Frey 2011] Ivo Frey. "Spherical Skinning withDual-Quaternions and Qtangents." SIGGRAPH 2011.

extern void mesh_calculate_tangents(
	size_t face_count,
	uint32_t const *indices,
	size_t vertex_count,
	pt_math_vec3 const *positions,
	pt_math_vec3 const *normals,
	pt_math_vec2 const *uvs,
	pt_math_vec4 *out_qtangents,
	float *out_reflections);

#endif