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

BUILD_MODULE_NAME = pt_mcrt

include pt_template.mk

PREBUILT_LIBRARY_TBB_MALLOC = $(THIRD_PARTY_DIR)/TBB/build_posix_linux_gblic_$(BUILD_ARCHITECTURE)/libpt_tbbmalloc.so
PREBUILT_LIBRARY_TBB = $(THIRD_PARTY_DIR)/TBB/build_posix_linux_gblic_$(BUILD_ARCHITECTURE)/libpt_tbb.so
PREBUILT_LIBRARY_IRML = $(THIRD_PARTY_DIR)/TBB/build_posix_linux_gblic_$(BUILD_ARCHITECTURE)/libpt_irml.so

CXX_COMPILER_FLAGS += -I"$(abspath $(INCLUDE_DIR))"
CXX_COMPILER_FLAGS += -I"$(abspath $(THIRD_PARTY_DIR)/TBB/include)"
CXX_COMPILER_FLAGS += -DPT_ATTR_MCRT=PT_ATTR_EXPORT

CXX_LINKER_FLAGS += -shared -Wl,-soname,lib$(BUILD_MODULE_NAME).so
CXX_LINKER_FLAGS += -Wl,--version-script,"$(abspath $(BUILD_DIR)/$(BUILD_MODULE_NAME).def)"

all: \
    $(BIN_DIR)/$(notdir $(PREBUILT_LIBRARY_TBB_MALLOC)) \
    $(BIN_DIR)/$(notdir $(PREBUILT_LIBRARY_TBB)) \
    $(BIN_DIR)/$(notdir $(PREBUILT_LIBRARY_IRML)) \
    $(BIN_DIR)/lib$(BUILD_MODULE_NAME).so

.PHONY : \
    all

$(eval $(call BUILD_COPY_PREBUILT_LIBRARY,$(PREBUILT_LIBRARY_TBB_MALLOC)))
$(eval $(call BUILD_COPY_PREBUILT_LIBRARY,$(PREBUILT_LIBRARY_TBB)))
$(eval $(call BUILD_COPY_PREBUILT_LIBRARY,$(PREBUILT_LIBRARY_IRML)))

$(BIN_DIR)/lib$(BUILD_MODULE_NAME).so: \
    $(OBJ_DIR)/pt_mcrt_log.o \
    $(OBJ_DIR)/pt_mcrt_intrin.o \
    $(OBJ_DIR)/pt_mcrt_memset.o \
    $(OBJ_DIR)/pt_mcrt_memcmp.o \
    $(OBJ_DIR)/pt_mcrt_memcpy.o \
    $(OBJ_DIR)/pt_mcrt_malloc.o \
    $(OBJ_DIR)/pt_mcrt_task.o \
    $(PREBUILT_LIBRARY_TBB_MALLOC) \
    $(PREBUILT_LIBRARY_TBB) ; ${BUILD_VERBOSE} \
        mkdir -p "$(abspath $(BIN_DIR))"; \
        ${CXX_LINKER} $(CXX_LINKER_FLAGS) -o "$(abspath $(BIN_DIR)/lib$(BUILD_MODULE_NAME).so)" \
        "$(abspath $(OBJ_DIR))/pt_mcrt_log.o" \
        "$(abspath $(OBJ_DIR))/pt_mcrt_intrin.o" \
        "$(abspath $(OBJ_DIR))/pt_mcrt_memset.o" \
        "$(abspath $(OBJ_DIR))/pt_mcrt_memcmp.o" \
        "$(abspath $(OBJ_DIR))/pt_mcrt_memcpy.o" \
        "$(abspath $(OBJ_DIR))/pt_mcrt_malloc.o" \
        "$(abspath $(OBJ_DIR))/pt_mcrt_task.o" \
        "$(abspath $(PREBUILT_LIBRARY_TBB_MALLOC))" \
        "$(abspath $(PREBUILT_LIBRARY_TBB))"

$(eval $(call BUILD_CXX_COMPILE,pt_mcrt_log))
$(eval $(call BUILD_CXX_COMPILE,pt_mcrt_intrin))
$(eval $(call BUILD_CXX_COMPILE,pt_mcrt_memset))
$(eval $(call BUILD_CXX_COMPILE,pt_mcrt_memcmp))
$(eval $(call BUILD_CXX_COMPILE,pt_mcrt_memcpy))
$(eval $(call BUILD_CXX_COMPILE,pt_mcrt_malloc))
$(eval $(call BUILD_CXX_COMPILE,pt_mcrt_task))

include \
    $(OBJ_DIR)/pt_mcrt_log.d \
    $(OBJ_DIR)/pt_mcrt_intrin.d \
    $(OBJ_DIR)/pt_mcrt_memset.d \
    $(OBJ_DIR)/pt_mcrt_memcmp.d \
    $(OBJ_DIR)/pt_mcrt_memcpy.d \
    $(OBJ_DIR)/pt_mcrt_malloc.d \
    $(OBJ_DIR)/pt_mcrt_task.d