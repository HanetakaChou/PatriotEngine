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

BUILD_MODULE_NAME = pt_imaging_vk

include pt_template.mk

CXX_COMPILER_FLAGS += -I"$(abspath $(INCLUDE_DIR))"
CXX_COMPILER_FLAGS += -I"$(abspath $(SPIRV_DIR))"
CXX_COMPILER_FLAGS += -I"$(abspath $(THIRD_PARTY_DIR)/DirectXMath/Inc)"
CXX_COMPILER_FLAGS += -I"$(abspath $(THIRD_PARTY_DIR)/VulkanMemoryAllocator/include)"
CXX_COMPILER_FLAGS += -DPT_ATTR_GFX=PT_ATTR_EXPORT

CXX_LINKER_FLAGS += -shared -Wl,-soname,lib$(BUILD_MODULE_NAME).so
CXX_LINKER_FLAGS += -Wl,--version-script,"$(abspath $(BUILD_DIR)/$(BUILD_MODULE_NAME).def)"
CXX_LINKER_FLAGS += -lvulkan

all: \
    $(BIN_DIR)/lib$(BUILD_MODULE_NAME).so

.PHONY : \
    all

$(BIN_DIR)/lib$(BUILD_MODULE_NAME).so: \
	$(OBJ_DIR)/imaging/streaming_object.o \
    $(OBJ_DIR)/imaging/mesh.o \
    $(OBJ_DIR)/imaging/mesh_vertex_tangent.o \
    $(OBJ_DIR)/imaging/mesh_asset_load.o \
    $(OBJ_DIR)/imaging/mesh_asset_load_pmx.o \
    $(OBJ_DIR)/imaging/texture.o \
    $(OBJ_DIR)/imaging/texture_asset_load.o \
    $(OBJ_DIR)/imaging/texture_asset_load_dds.o \
	$(OBJ_DIR)/imaging/texture_asset_load_pvr.o \
	$(OBJ_DIR)/imaging/vk/vk_mesh.o \
	$(OBJ_DIR)/imaging/vk/vk_texture.o \
	$(OBJ_DIR)/scene/pt_gfx_node_base.o \
	$(OBJ_DIR)/scene/pt_gfx_node_vk.o \
	$(OBJ_DIR)/scene/pt_gfx_scene_base.o \
	$(OBJ_DIR)/scene/pt_gfx_scene_vk.o \
	$(OBJ_DIR)/pt_gfx_connection_base.o \
	$(OBJ_DIR)/pt_gfx_connection_d3d12.o \
	$(OBJ_DIR)/pt_gfx_connection_vk.o \
	$(OBJ_DIR)/pt_gfx_device_vk.o \
	$(OBJ_DIR)/pt_gfx_device_vk_wsi_x11.o \
	$(OBJ_DIR)/pt_gfx_frame_object_base.o \
	$(OBJ_DIR)/pt_gfx_malloc_vk.o \
	$(OBJ_DIR)/pt_gfx_malloc_vma.o \
	$(OBJ_DIR)/pt_gfx_material_base.o \
	$(OBJ_DIR)/pt_gfx_material_vk.o \
	$(BIN_DIR)/libpt_mcrt.so \
    $(SPIRV_DIR)/imaging/vk/forward_shading_mesh_vertex.inl \
    $(SPIRV_DIR)/imaging/vk/forward_shading_mesh_fragment.inl ; ${BUILD_VERBOSE} \
        mkdir -p "$(abspath $(BIN_DIR))"; \
        ${CXX_LINKER} $(CXX_LINKER_FLAGS) -o "$(abspath $(BIN_DIR)/lib$(BUILD_MODULE_NAME).so)" \
		"$(abspath $(OBJ_DIR)/imaging/streaming_object.o)" \
		"$(abspath $(OBJ_DIR)/imaging/mesh.o)" \
		"$(abspath $(OBJ_DIR)/imaging/mesh_vertex_tangent.o)" \
		"$(abspath $(OBJ_DIR)/imaging/mesh_asset_load.o)" \
		"$(abspath $(OBJ_DIR)/imaging/mesh_asset_load_pmx.o)" \
		"$(abspath $(OBJ_DIR)/imaging/texture.o)" \
		"$(abspath $(OBJ_DIR)/imaging/texture_asset_load.o)" \
		"$(abspath $(OBJ_DIR)/imaging/texture_asset_load_dds.o)" \
		"$(abspath $(OBJ_DIR)/imaging/texture_asset_load_pvr.o)" \
		"$(abspath $(OBJ_DIR)/imaging/vk/vk_mesh.o)" \
		"$(abspath $(OBJ_DIR)/imaging/vk/vk_texture.o)" \
		"$(abspath $(OBJ_DIR)/scene/pt_gfx_node_base.o)" \
		"$(abspath $(OBJ_DIR)/scene/pt_gfx_node_vk.o)" \
		"$(abspath $(OBJ_DIR)/scene/pt_gfx_scene_base.o)" \
		"$(abspath $(OBJ_DIR)/scene/pt_gfx_scene_vk.o)" \
		"$(abspath $(OBJ_DIR)/pt_gfx_connection_base.o)" \
		"$(abspath $(OBJ_DIR)/pt_gfx_connection_d3d12.o)" \
		"$(abspath $(OBJ_DIR)/pt_gfx_connection_vk.o)" \
		"$(abspath $(OBJ_DIR)/pt_gfx_device_vk.o)" \
		"$(abspath $(OBJ_DIR)/pt_gfx_device_vk_wsi_x11.o)" \
		"$(abspath $(OBJ_DIR)/pt_gfx_frame_object_base.o)" \
		"$(abspath $(OBJ_DIR)/pt_gfx_malloc_vk.o)" \
		"$(abspath $(OBJ_DIR)/pt_gfx_malloc_vma.o)" \
		"$(abspath $(OBJ_DIR)/pt_gfx_material_base.o)" \
		"$(abspath $(OBJ_DIR)/pt_gfx_material_vk.o)" \
		"$(abspath $(BIN_DIR)/libpt_mcrt.so)"

$(eval $(call BUILD_CXX_COMPILE,imaging/streaming_object))
$(eval $(call BUILD_CXX_COMPILE,imaging/mesh))
$(eval $(call BUILD_CXX_COMPILE,imaging/mesh_vertex_tangent))
$(eval $(call BUILD_CXX_COMPILE,imaging/mesh_asset_load))
$(eval $(call BUILD_CXX_COMPILE,imaging/mesh_asset_load_pmx))
$(eval $(call BUILD_CXX_COMPILE,imaging/texture))
$(eval $(call BUILD_CXX_COMPILE,imaging/texture_asset_load))
$(eval $(call BUILD_CXX_COMPILE,imaging/texture_asset_load_dds))
$(eval $(call BUILD_CXX_COMPILE,imaging/texture_asset_load_pvr))
$(eval $(call BUILD_CXX_COMPILE,imaging/vk/vk_mesh))
$(eval $(call BUILD_CXX_COMPILE,imaging/vk/vk_texture))
$(eval $(call BUILD_CXX_COMPILE,scene/pt_gfx_node_base))
$(eval $(call BUILD_CXX_COMPILE,scene/pt_gfx_node_vk))
$(eval $(call BUILD_CXX_COMPILE,scene/pt_gfx_scene_base))
$(eval $(call BUILD_CXX_COMPILE,scene/pt_gfx_scene_vk))
$(eval $(call BUILD_CXX_COMPILE,pt_gfx_connection_base))
$(eval $(call BUILD_CXX_COMPILE,pt_gfx_connection_d3d12))
$(eval $(call BUILD_CXX_COMPILE,pt_gfx_connection_vk))
$(eval $(call BUILD_CXX_COMPILE,pt_gfx_device_vk))
$(eval $(call BUILD_CXX_COMPILE,pt_gfx_device_vk_wsi_x11))
$(eval $(call BUILD_CXX_COMPILE,pt_gfx_frame_object_base))
$(eval $(call BUILD_CXX_COMPILE,pt_gfx_malloc_vk))
$(eval $(call BUILD_CXX_COMPILE,pt_gfx_malloc_vma))
$(eval $(call BUILD_CXX_COMPILE,pt_gfx_material_base))
$(eval $(call BUILD_CXX_COMPILE,pt_gfx_material_vk))

$(eval $(call BUILD_GLSL_COMPILE,vert,imaging/vk/forward_shading_mesh_vertex))
$(eval $(call BUILD_GLSL_COMPILE,frag,imaging/vk/forward_shading_mesh_fragment))

include \
	$(OBJ_DIR)/imaging/streaming_object.d \
    $(OBJ_DIR)/imaging/mesh.d \
    $(OBJ_DIR)/imaging/mesh_vertex_tangent.d \
    $(OBJ_DIR)/imaging/mesh_asset_load.d \
    $(OBJ_DIR)/imaging/mesh_asset_load_pmx.d \
    $(OBJ_DIR)/imaging/texture.d \
    $(OBJ_DIR)/imaging/texture_asset_load.d \
    $(OBJ_DIR)/imaging/texture_asset_load_dds.d \
	$(OBJ_DIR)/imaging/texture_asset_load_pvr.d \
	$(OBJ_DIR)/imaging/vk/vk_mesh.d \
	$(OBJ_DIR)/imaging/vk/vk_texture.d \
	$(OBJ_DIR)/scene/pt_gfx_node_base.d \
	$(OBJ_DIR)/scene/pt_gfx_node_vk.d \
	$(OBJ_DIR)/scene/pt_gfx_scene_base.d \
	$(OBJ_DIR)/scene/pt_gfx_scene_vk.d \
	$(OBJ_DIR)/pt_gfx_connection_base.d \
	$(OBJ_DIR)/pt_gfx_connection_d3d12.d \
	$(OBJ_DIR)/pt_gfx_connection_vk.d \
	$(OBJ_DIR)/pt_gfx_device_vk.d \
	$(OBJ_DIR)/pt_gfx_device_vk_wsi_x11.d \
	$(OBJ_DIR)/pt_gfx_frame_object_base.d \
	$(OBJ_DIR)/pt_gfx_malloc_vk.d \
	$(OBJ_DIR)/pt_gfx_malloc_vma.d \
	$(OBJ_DIR)/pt_gfx_material_base.d \
	$(OBJ_DIR)/pt_gfx_material_vk.d \
	$(SPIRV_DIR)/imaging/vk/forward_shading_mesh_vertex.d \
	$(SPIRV_DIR)/imaging/vk/forward_shading_mesh_fragment.d