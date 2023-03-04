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

BUILD_MODULE_NAME = pt_wsi

include pt_template.mk

CXX_COMPILER_FLAGS += -I"$(abspath $(INCLUDE_DIR))"
CXX_COMPILER_FLAGS += -I"$(abspath $(THIRD_PARTY_DIR)/DirectXMath/Inc)"
CXX_COMPILER_FLAGS += -DPT_ATTR_WSI=PT_ATTR_EXPORT

CXX_LINKER_FLAGS += -shared -Wl,-soname,lib$(BUILD_MODULE_NAME).so
CXX_LINKER_FLAGS += -Wl,--version-script,"$(abspath $(BUILD_DIR)/$(BUILD_MODULE_NAME).def)"
CXX_LINKER_FLAGS += -lxcb

all: \
    $(BIN_DIR)/lib$(BUILD_MODULE_NAME).so

.PHONY : \
    all

$(BIN_DIR)/lib$(BUILD_MODULE_NAME).so: \
	$(OBJ_DIR)/pt_wsi_main_linux_x11.o \
	$(BIN_DIR)/libpt_imaging_vk.so \
	$(BIN_DIR)/libpt_mcrt.so ; ${BUILD_VERBOSE} \
        mkdir -p "$(abspath $(BIN_DIR))"; \
        ${CXX_LINKER} $(CXX_LINKER_FLAGS) -o "$(abspath $(BIN_DIR)/lib$(BUILD_MODULE_NAME).so)" \
		"$(abspath $(OBJ_DIR)/pt_wsi_main_linux_x11.o)" \
		"$(abspath $(BIN_DIR)/libpt_imaging_vk.so)" \
		"$(abspath $(BIN_DIR)/libpt_mcrt.so)"

$(eval $(call BUILD_CXX_COMPILE,pt_wsi_main_linux_x11))

include \
	$(OBJ_DIR)/pt_wsi_main_linux_x11.d