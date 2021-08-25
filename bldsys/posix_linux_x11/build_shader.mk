#
# Copyright (C) YuqiaoZhang(HanetakaYuminaga)
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

all: pt_gfx_shader_mesh_vertex_vk.inl pt_gfx_shader_mesh_fragment_vk.inl

pt_gfx_shader_mesh_vertex_vk.inl : pt_gfx_shader_mesh_vertex_vk.glsl
	glslangValidator -S vert -V100 -o pt_gfx_shader_mesh_vertex_vk.inl -x pt_gfx_shader_mesh_vertex_vk.glsl
pt_gfx_shader_mesh_fragment_vk.inl : pt_gfx_shader_mesh_fragment_vk.glsl
	glslangValidator -S frag -V100 -o pt_gfx_shader_mesh_fragment_vk.inl -x pt_gfx_shader_mesh_fragment_vk.glsl


