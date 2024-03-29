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

BUILD_MODULE_NAME = pt_launcher

include pt_template.mk

CXX_COMPILER_FLAGS += -I"$(abspath $(INCLUDE_DIR))"
CXX_COMPILER_FLAGS += -I"$(abspath $(THIRD_PARTY_DIR)/DirectXMath/Inc)"
CXX_COMPILER_FLAGS += -DPT_ATTR_WSI=PT_ATTR_EXPORT

CXX_LINKER_FLAGS += -pie -Wl,-z,nocopyreloc
CXX_LINKER_FLAGS += -Wl,--version-script,"$(abspath $(BUILD_DIR)/$(BUILD_MODULE_NAME).def)"

all: \
    $(BIN_DIR)/$(BUILD_MODULE_NAME)

.PHONY : \
    all

$(BIN_DIR)/$(BUILD_MODULE_NAME): \
	$(OBJ_DIR)/../samples/asset/material_asset_load_pmx.o \
	$(OBJ_DIR)/../samples/launcher/pt_wsi_app_base.o \
	$(OBJ_DIR)/../samples/launcher/pt_wsi_app_linux_x11.o \
	$(BIN_DIR)/libpt_wsi.so \
	$(BIN_DIR)/libpt_imaging_vk.so \
	$(BIN_DIR)/libpt_mcrt.so ; ${BUILD_VERBOSE} \
        mkdir -p "$(abspath $(BIN_DIR))"; \
        ${CXX_LINKER} $(CXX_LINKER_FLAGS) -o "$(abspath $(BIN_DIR)/$(BUILD_MODULE_NAME))" \
		"$(abspath $(OBJ_DIR)/../samples/asset/material_asset_load_pmx.o)" \
		"$(abspath $(OBJ_DIR)/../samples/launcher/pt_wsi_app_base.o)" \
		"$(abspath $(OBJ_DIR)/../samples/launcher/pt_wsi_app_linux_x11.o)" \
		"$(abspath $(BIN_DIR)/libpt_wsi.so)" \
		"$(abspath $(BIN_DIR)/libpt_imaging_vk.so)" \
		"$(abspath $(BIN_DIR)/libpt_mcrt.so)"

$(eval $(call BUILD_CXX_COMPILE,../samples/asset/material_asset_load_pmx))
$(eval $(call BUILD_CXX_COMPILE,../samples/launcher/pt_wsi_app_base))
$(eval $(call BUILD_CXX_COMPILE,../samples/launcher/pt_wsi_app_linux_x11))

include \
	$(OBJ_DIR)/../samples/asset/material_asset_load_pmx.d \
	$(OBJ_DIR)/../samples/launcher/pt_wsi_app_base.d \
	$(OBJ_DIR)/../samples/launcher/pt_wsi_app_linux_x11.d
