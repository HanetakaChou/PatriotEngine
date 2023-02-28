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

all: ../../src/imaging/vk/forward_shading_mesh_vertex.inl ../../src/imaging/vk/forward_shading_mesh_fragment.inl

# -g -Od should only be used in debug version

../../src/imaging/vk/forward_shading_mesh_vertex.inl obj/imaging/vk/forward_shading_mesh_vertex.d : ../../src/imaging/vk/forward_shading_mesh_vertex.glsl
	glslangValidator --depfile obj/imaging/vk/forward_shading_mesh_vertex.d -g -Od -S vert -V100 -o ../../src/imaging/vk/forward_shading_mesh_vertex.inl -x ../../src/imaging/vk/forward_shading_mesh_vertex.glsl

../../src/imaging/vk/forward_shading_mesh_fragment.inl obj/imaging/vk/forward_shading_mesh_fragment.d : ../../src/imaging/vk/forward_shading_mesh_fragment.glsl 
	glslangValidator --depfile obj/imaging/vk/forward_shading_mesh_fragment.d -g -Od -S frag -V100 -o ../../src/imaging/vk/forward_shading_mesh_fragment.inl -x ../../src/imaging/vk/forward_shading_mesh_fragment.glsl

# automatic create directory

obj/imaging/vk/forward_shading_mesh_vertex.d obj/imaging/vk/forward_shading_mesh_fragment.d : obj/imaging/vk

obj/imaging/vk :
	mkdir -p obj/imaging/vk

include obj/imaging/vk/forward_shading_mesh_vertex.d obj/imaging/vk/forward_shading_mesh_fragment.d