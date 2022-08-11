#
# Copyright (C) YuqiaoZhang(HanetakaChou)
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

# -g -Od should only be used in debug version

all: \
	pt_gfx_glsl_vulkan_lighting_mesh_vert.inl \
	pt_gfx_glsl_vulkan_lighting_mesh_frag.inl

pt_gfx_glsl_vulkan_lighting_mesh_vert.inl : pt_gfx_glsl_vulkan_lighting_mesh_vert.glsl pt_gfx_shader_brdf.glslangh
	glslangValidator -S vert -V100 -o pt_gfx_glsl_vulkan_lighting_mesh_vert.inl -x pt_gfx_glsl_vulkan_lighting_mesh_vert.glsl
pt_gfx_glsl_vulkan_lighting_mesh_frag.inl : pt_gfx_glsl_vulkan_lighting_mesh_frag.glsl pt_gfx_shader_brdf.glslangh
	glslangValidator -S frag -V100 -o pt_gfx_glsl_vulkan_lighting_mesh_frag.inl -x pt_gfx_glsl_vulkan_lighting_mesh_frag.glsl
