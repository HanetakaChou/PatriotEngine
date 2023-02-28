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

CXX_FLAGS:= -g -Od -fPIE -fPIC 
LINKER_FLAGS:= -shared -pie -Wl,-soname,imaging_vk -Wl,--enable-new-dtags -Wl,-rpath,/XXXXXX -Wl,--no-undefined
GLSL_FLAGS:= -g -Od
INT_DIR:= $(abspath ./obj/imaging_vk/debug/)
SRC_DIR:= $(abspath ../../src/)

imaging_vk: \
	$(abspath $(SRC_DIR)/imaging/vk/forward_shading_mesh_vertex.inl)\
	$(abspath $(SRC_DIR)/imaging/vk/forward_shading_mesh_fragment.inl)

$(abspath $(SRC_DIR)/imaging/vk/forward_shading_mesh_vertex.inl) \
$(abspath $(INT_DIR)/imaging/vk/forward_shading_mesh_vertex.d) : \
$(abspath $(SRC_DIR)/imaging/vk/forward_shading_mesh_vertex.glsl) ; \
	mkdir -p $(dir $(abspath $(INT_DIR)/imaging/vk/forward_shading_mesh_vertex.d)); \
	glslangValidator --depfile $(abspath $(INT_DIR)/imaging/vk/forward_shading_mesh_vertex.d) \
	-V100 -x $(GLSL_FLAGS) -S vert -o $(abspath $(SRC_DIR)/imaging/vk/forward_shading_mesh_vertex.inl) \
	$(abspath $(SRC_DIR)/imaging/vk/forward_shading_mesh_vertex.glsl)

$(abspath $(SRC_DIR)/imaging/vk/forward_shading_mesh_fragment.inl) \
$(abspath $(INT_DIR)/imaging/vk/forward_shading_mesh_fragment.d) : \
$(abspath $(SRC_DIR)/imaging/vk/forward_shading_mesh_fragment.glsl) ; \
	mkdir -p $(dir $(abspath $(INT_DIR)/imaging/vk/forward_shading_mesh_fragment.d)); \
	glslangValidator --depfile $(abspath $(INT_DIR)/imaging/vk/forward_shading_mesh_fragment.d) \
	-V100 -x $(GLSL_FLAGS) -S frag -o $(abspath $(SRC_DIR)/imaging/vk/forward_shading_mesh_fragment.inl) \
	$(abspath $(SRC_DIR)/imaging/vk/forward_shading_mesh_fragment.glsl)

include \
	$(abspath $(INT_DIR)/imaging/vk/forward_shading_mesh_vertex.d) \
	$(abspath $(INT_DIR)/imaging/vk/forward_shading_mesh_fragment.d)