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

BIN_DIR := $(abspath ../../bin/x64/debug/)
OBJ_DIR := $(abspath ./obj/imaging_vk/x64/debug/)
SPIRV_DIR := $(abspath ./spirv/imaging_vk/debug/)
SRC_DIR := $(abspath ../../src/)


CXX := c++

CXX_FLAGS := -g -O0 -UNDEBUG #-fno-limit-debug-info
CXX_FLAGS += -fPIE -fPIC 
CXX_FLAGS += -pthread
CXX_FLAGS += -no-canonical-prefixes
# CXX_FLAGS += -ffunction-sections -funwind-tables -fstack-protector-strong -Wno-invalid-command-line-argument -Wno-unused-command-line-argument -Wa,--noexecstack -Wformat -Werror=format-security 
CXX_FLAGS += -fno-exceptions -fno-rtti
CXX_FLAGS += -Werror=return-type 
CXX_FLAGS += -Wall 
CXX_FLAGS += -fvisibility=hidden 
CXX_FLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8 
CXX_FLAGS += -DPT_ATTR_MCRT=PT_ATTR_EXPORT
CXX_FLAGS += -I$(abspath ../../include)
CXX_FLAGS += -I$(SPIRV_DIR)

LINKER := c++

LINKER_FLAGS := -shared -Wl,-soname,libmcrt.so
LINKER_FLAGS += -fPIE -fPIC 
LINKER_FLAGS += -pthread
LINKER_FLAGS += -no-canonical-prefixes
# LINKER_FLAGS += -fuse-ld=gold
# LINKER_FLAGS += -Wl,--build-id -Wl,--no-undefined -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -Wl,--warn-shared-textrel -Wl,--fatal-warnings
LINKER_FLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8 
LINKER_FLAGS += -Wl,--enable-new-dtags '-Wl,-rpath,$$ORIGIN'
LINKER_FLAGS += -Wl,--no-undefined -Wl,--version-script,$(abspath ./mcrt.def)

GLSLANG := glslangValidator

GLSLANG_FLAGS:= -g -Od

VERBOSE := @

imaging_vk: \
	$(abspath $(SPIRV_DIR)/imaging/vk/forward_shading_mesh_vertex.inl)\
	$(abspath $(SPIRV_DIR)/imaging/vk/forward_shading_mesh_fragment.inl)

$(abspath $(SPIRV_DIR)/imaging/vk/forward_shading_mesh_vertex.inl) \
$(abspath $(OBJ_DIR)/imaging/vk/forward_shading_mesh_vertex.d) : \
$(abspath $(SRC_DIR)/imaging/vk/forward_shading_mesh_vertex.glsl) ; $(VERBOSE) \
	mkdir -p $(dir $(abspath $(SPIRV_DIR)/imaging/vk/forward_shading_mesh_vertex.inl)); \
	mkdir -p $(dir $(abspath $(OBJ_DIR)/imaging/vk/forward_shading_mesh_vertex.d)); \
	$(GLSLANG) --depfile $(abspath $(OBJ_DIR)/imaging/vk/forward_shading_mesh_vertex.d) \
	-V100 -x $(GLSL_FLAGS) -S vert -o $(abspath $(SPIRV_DIR)/imaging/vk/forward_shading_mesh_vertex.inl) \
	$(abspath $(SRC_DIR)/imaging/vk/forward_shading_mesh_vertex.glsl)

$(abspath $(SPIRV_DIR)/imaging/vk/forward_shading_mesh_fragment.inl) \
$(abspath $(OBJ_DIR)/imaging/vk/forward_shading_mesh_fragment.d) : \
$(abspath $(SRC_DIR)/imaging/vk/forward_shading_mesh_fragment.glsl) ; $(VERBOSE) \
	mkdir -p $(dir $(abspath $(SPIRV_DIR)/imaging/vk/forward_shading_mesh_fragment.inl)); \
	mkdir -p $(dir $(abspath $(OBJ_DIR)/imaging/vk/forward_shading_mesh_fragment.d)); \
	$(GLSLANG) --depfile $(abspath $(OBJ_DIR)/imaging/vk/forward_shading_mesh_fragment.d) \
	-V100 -x $(GLSL_FLAGS) -S frag -o $(abspath $(SPIRV_DIR)/imaging/vk/forward_shading_mesh_fragment.inl) \
	$(abspath $(SRC_DIR)/imaging/vk/forward_shading_mesh_fragment.glsl)

include \
	$(abspath $(OBJ_DIR)/imaging/vk/forward_shading_mesh_vertex.d) \
	$(abspath $(OBJ_DIR)/imaging/vk/forward_shading_mesh_fragment.d)