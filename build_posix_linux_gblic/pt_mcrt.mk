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

include pt_template.mk

PREBUILT_LIBRARY_TBB_MALLOC := $(THIRD_PARTY_DIR)/TBB/build_posix_linux_gblic_$(BUILD_ARCHITECTURE)/libpt_tbbmalloc.so
PREBUILT_LIBRARY_TBB := $(THIRD_PARTY_DIR)/TBB/build_posix_linux_gblic_$(BUILD_ARCHITECTURE)/libpt_tbb.so
PREBUILT_LIBRARY_IRML := $(THIRD_PARTY_DIR)/TBB/build_posix_linux_gblic_$(BUILD_ARCHITECTURE)/libpt_irml.so

PREBUILT_LIBRARY_TBB_MALLOC_ABS_PATH := $(abspath $(PREBUILT_LIBRARY_TBB_MALLOC))
PREBUILT_LIBRARY_TBB_ABS_PATH := $(abspath $(PREBUILT_LIBRARY_TBB))
PREBUILT_LIBRARY_IRML_ABS_PATH := $(abspath $(PREBUILT_LIBRARY_IRML))

CXX_COMPILER_FLAGS += -DPT_ATTR_MCRT=PT_ATTR_EXPORT
CXX_COMPILER_FLAGS += -I"$(INCLUDE_ABS_DIR)"
CXX_COMPILER_FLAGS += -I"$(THIRD_PARTY_ABS_DIR)/TBB/include"

CXX_LINKER_FLAGS += -shared -Wl,-soname,libpt_mcrt.so
CXX_LINKER_FLAGS += -Wl,--version-script,"$(BUILD_ABS_DIR)/pt_mcrt.def"

all: \
    $(BIN_DIR)/libpt_tbbmalloc.so \
    $(BIN_DIR)/libpt_tbb.so \
    $(BIN_DIR)/libpt_irml.so \
    $(BIN_DIR)/libpt_mcrt.so

.PHONY : \
    all

# Copy Prebuilt Libraries
$(eval $(call BUILD_COPY_PREBUILT_LIBRARY,$(PREBUILT_LIBRARY_TBB_MALLOC)))

$(eval $(call BUILD_COPY_PREBUILT_LIBRARY,$(PREBUILT_LIBRARY_TBB)))

$(eval $(call BUILD_COPY_PREBUILT_LIBRARY,$(PREBUILT_LIBRARY_IRML)))

# Build
$(BIN_DIR)/libpt_mcrt.so: \
    $(OBJ_DIR)/pt_mcrt_log.o \
    $(OBJ_DIR)/pt_mcrt_intrin.o \
    $(OBJ_DIR)/pt_mcrt_intrin_x86_sse2.o \
    $(OBJ_DIR)/pt_mcrt_memset.o \
    $(OBJ_DIR)/pt_mcrt_memcmp.o \
    $(OBJ_DIR)/pt_mcrt_memcpy.o \
    $(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_ssse3.o \
    $(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx.o \
    $(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx512f.o \
    $(OBJ_DIR)/pt_mcrt_malloc.o \
    $(OBJ_DIR)/pt_mcrt_task.o \
    ${PREBUILT_LIBRARY_TBB_MALLOC} \
    ${PREBUILT_LIBRARY_TBB} ; ${BUILD_VERBOSE} \
        mkdir -p "$(BIN_ABS_DIR)"; \
        ${CXX_LINKER} $(CXX_LINKER_FLAGS) -o "$(BIN_ABS_DIR)/libpt_mcrt.so" \
        "$(OBJ_ABS_DIR)/pt_mcrt_log.o" \
        "$(OBJ_ABS_DIR)/pt_mcrt_intrin.o" \
        "$(OBJ_ABS_DIR)/pt_mcrt_intrin_x86_sse2.o" \
        "$(OBJ_ABS_DIR)/pt_mcrt_memset.o" \
        "$(OBJ_ABS_DIR)/pt_mcrt_memcmp.o" \
        "$(OBJ_ABS_DIR)/pt_mcrt_memcpy.o" \
        "$(OBJ_ABS_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_ssse3.o" \
        "$(OBJ_ABS_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx.o" \
        "$(OBJ_ABS_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx512f.o" \
        "$(OBJ_ABS_DIR)/pt_mcrt_malloc.o" \
        "$(OBJ_ABS_DIR)/pt_mcrt_task.o" \
        "${PREBUILT_LIBRARY_TBB_MALLOC_ABS_PATH}" \
        "${PREBUILT_LIBRARY_TBB_ABS_PATH}"

$(eval $(call BUILD_CXX_COMPILE,pt_mcrt_log))

$(eval $(call BUILD_CXX_COMPILE,pt_mcrt_intrin))

$(eval $(call BUILD_CXX_COMPILE,pt_mcrt_memset))

$(eval $(call BUILD_CXX_COMPILE,pt_mcrt_memcmp))

$(eval $(call BUILD_CXX_COMPILE,pt_mcrt_memcpy))

$(eval $(call BUILD_CXX_COMPILE,pt_mcrt_malloc))

$(eval $(call BUILD_CXX_COMPILE,pt_mcrt_task))

$(OBJ_DIR)/pt_mcrt_intrin_x86_sse2.o \
$(OBJ_DIR)/pt_mcrt_intrin_x86_sse2.d : \
$(SRC_DIR)/pt_mcrt_intrin_x86_sse2.cpp ; ${BUILD_VERBOSE} \
    mkdir -p "$(OBJ_ABS_DIR)"; \
    $(CXX_COMPILER) -MMD -MP -MF "$(OBJ_ABS_DIR)/pt_mcrt_intrin_x86_sse2.d" \
    -c $(CXX_COMPILER_FLAGS) -msse2 -o "$(OBJ_ABS_DIR)/pt_mcrt_intrin_x86_sse2.o" \
    "$(SRC_ABS_DIR)/pt_mcrt_intrin_x86_sse2.cpp"

$(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_ssse3.o \
$(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_ssse3.d : \
$(SRC_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_ssse3.cpp ; ${BUILD_VERBOSE} \
    mkdir -p "$(OBJ_ABS_DIR)"; \
    $(CXX_COMPILER) -MMD -MP -MF "$(OBJ_ABS_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_ssse3.d" \
    -c $(CXX_COMPILER_FLAGS) -mssse3 -o "$(OBJ_ABS_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_ssse3.o" \
    "$(SRC_ABS_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_ssse3.cpp"

$(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx.o \
$(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx.d : \
$(SRC_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx.cpp ; ${BUILD_VERBOSE} \
    mkdir -p "$(OBJ_ABS_DIR)"; \
    $(CXX_COMPILER) -MMD -MP -MF "$(OBJ_ABS_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx.d" \
    -c $(CXX_COMPILER_FLAGS) -mavx -o "$(OBJ_ABS_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx.o" \
    "$(SRC_ABS_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx.cpp"

$(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx512f.o \
$(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx512f.d : \
$(SRC_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx512f.cpp ; ${BUILD_VERBOSE} \
    mkdir -p "$(OBJ_ABS_DIR)"; \
    $(CXX_COMPILER) -MMD -MP -MF "$(OBJ_ABS_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx512f.d" \
    -c $(CXX_COMPILER_FLAGS) -mavx512f -o "$(OBJ_ABS_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx512f.o" \
    "$(SRC_ABS_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx512f.cpp"

include \
    $(OBJ_DIR)/pt_mcrt_log.d \
    $(OBJ_DIR)/pt_mcrt_intrin.d \
    $(OBJ_DIR)/pt_mcrt_memset.d \
    $(OBJ_DIR)/pt_mcrt_memcmp.d \
    $(OBJ_DIR)/pt_mcrt_memcpy.d \
    $(OBJ_DIR)/pt_mcrt_malloc.d \
    $(OBJ_DIR)/pt_mcrt_task.d