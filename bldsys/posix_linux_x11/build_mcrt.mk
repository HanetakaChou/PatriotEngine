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
OBJ_DIR := $(abspath ./obj/mcrt/x64/debug/)
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
CXX_FLAGS += -I$(abspath ../../third_party/intel_tbb/include)

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

PREBUILT_TBB_MALLOC_LIBRARY := $(abspath ../../third_party/intel_tbb/lib/glibc_x64/libpt_tbbmalloc.so)
PREBUILT_TBB_LIBRARY := $(abspath ../../third_party/intel_tbb/lib/glibc_x64/libpt_tbb.so)
PREBUILT_IRML_LIBRARY := $(abspath ../../third_party/intel_tbb/lib/glibc_x64/libpt_irml.so)

VERBOSE := @

.PHONY: \
	$(abspath $(BIN_DIR)/libmcrt.so) \
	$(abspath $(BIN_DIR)/libpt_tbbmalloc.so) \
	$(abspath $(BIN_DIR)/libpt_tbb.so) \
	$(abspath $(BIN_DIR)/libpt_irml.so)

$(abspath $(BIN_DIR)/libmcrt.so): \
	$(abspath $(OBJ_DIR)/pt_mcrt_log.o) \
	$(abspath $(OBJ_DIR)/pt_mcrt_intrin.o) \
	$(abspath $(OBJ_DIR)/pt_mcrt_intrin_x86_sse2.o) \
	$(abspath $(OBJ_DIR)/pt_mcrt_memset.o) \
	$(abspath $(OBJ_DIR)/pt_mcrt_memcmp.o) \
	$(abspath $(OBJ_DIR)/pt_mcrt_memcpy.o) \
	$(abspath $(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_ssse3.o) \
	$(abspath $(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx.o) \
	$(abspath $(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx512f.o) \
	$(abspath $(OBJ_DIR)/pt_mcrt_malloc.o) \
	$(abspath $(OBJ_DIR)/pt_mcrt_task.o) \
	${PREBUILT_TBB_MALLOC_LIBRARY} \
	${PREBUILT_TBB_LIBRARY} ; ${VERBOSE} \
		mkdir -p $(dir $(abspath $(BIN_DIR)/libmcrt.so)); \
		${LINKER} $(LINKER_FLAGS) -o $(abspath $(BIN_DIR)/libmcrt.so) \
		${PREBUILT_TBB_MALLOC_LIBRARY} \
		${PREBUILT_TBB_LIBRARY} \
		$(abspath $(OBJ_DIR)/pt_mcrt_log.o) \
		$(abspath $(OBJ_DIR)/pt_mcrt_intrin.o) \
		$(abspath $(OBJ_DIR)/pt_mcrt_intrin_x86_sse2.o) \
		$(abspath $(OBJ_DIR)/pt_mcrt_memset.o) \
		$(abspath $(OBJ_DIR)/pt_mcrt_memcmp.o) \
		$(abspath $(OBJ_DIR)/pt_mcrt_memcpy.o) \
		$(abspath $(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_ssse3.o) \
		$(abspath $(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx.o) \
		$(abspath $(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx512f.o) \
		$(abspath $(OBJ_DIR)/pt_mcrt_malloc.o) \
		$(abspath $(OBJ_DIR)/pt_mcrt_task.o) ; \

$(abspath $(BIN_DIR)/libpt_tbbmalloc.so) : \
	${PREBUILT_TBB_MALLOC_LIBRARY} ; ${VERBOSE} \
		mkdir -p $(dir $(abspath $(BIN_DIR)/libpt_tbbmalloc.so)); \
		cp -f ${PREBUILT_TBB_MALLOC_LIBRARY} \
		$(abspath $(BIN_DIR)/libpt_tbbmalloc.so)

$(abspath $(BIN_DIR)/libpt_tbb.so) : \
	${PREBUILT_TBB_LIBRARY} ; ${VERBOSE} \
		mkdir -p $(dir $(abspath $(BIN_DIR)/libpt_tbb.so)); \
		cp -f ${PREBUILT_TBB_LIBRARY} \
		$(abspath $(BIN_DIR)/libpt_tbb.so)

$(abspath $(BIN_DIR)/libpt_irml.so) : \
	${PREBUILT_IRML_LIBRARY} ; ${VERBOSE} \
		mkdir -p $(dir $(abspath $(BIN_DIR)/libpt_irml.so)); \
		cp -f ${PREBUILT_IRML_LIBRARY} \
		$(abspath $(BIN_DIR)/libpt_irml.so)

$(abspath $(OBJ_DIR)/pt_mcrt_log.o) \
$(abspath $(OBJ_DIR)/pt_mcrt_log.d) : \
$(abspath $(SRC_DIR)/pt_mcrt_log.cpp) ; ${VERBOSE} \
	mkdir -p $(dir $(abspath $(OBJ_DIR)/pt_mcrt_log.d)); \
	$(CXX) -MMD -MP -MF $(abspath $(OBJ_DIR)/pt_mcrt_log.d) \
	-c $(CXX_FLAGS) -o $(abspath $(OBJ_DIR)/pt_mcrt_log.o) \
	$(abspath $(SRC_DIR)/pt_mcrt_log.cpp)

$(abspath $(OBJ_DIR)/pt_mcrt_intrin.o) \
$(abspath $(OBJ_DIR)/pt_mcrt_intrin.d) : \
$(abspath $(SRC_DIR)/pt_mcrt_intrin.cpp) ; ${VERBOSE} \
	mkdir -p $(dir $(abspath $(OBJ_DIR)/pt_mcrt_intrin.d)); \
	$(CXX) -MMD -MP -MF $(abspath $(OBJ_DIR)/pt_mcrt_intrin.d) \
	-c $(CXX_FLAGS) -o $(abspath $(OBJ_DIR)/pt_mcrt_intrin.o) \
	$(abspath $(SRC_DIR)/pt_mcrt_intrin.cpp)

$(abspath $(OBJ_DIR)/pt_mcrt_intrin_x86_sse2.o) \
$(abspath $(OBJ_DIR)/pt_mcrt_intrin_x86_sse2.d) : \
$(abspath $(SRC_DIR)/pt_mcrt_intrin_x86_sse2.cpp) ; ${VERBOSE} \
	mkdir -p $(dir $(abspath $(OBJ_DIR)/pt_mcrt_intrin_x86_sse2.d)); \
	$(CXX) -MMD -MP -MF $(abspath $(OBJ_DIR)/pt_mcrt_intrin_x86_sse2.d) \
	-c $(CXX_FLAGS) -msse2 -o $(abspath $(OBJ_DIR)/pt_mcrt_intrin_x86_sse2.o) \
	$(abspath $(SRC_DIR)/pt_mcrt_intrin_x86_sse2.cpp)

$(abspath $(OBJ_DIR)/pt_mcrt_memset.o) \
$(abspath $(OBJ_DIR)/pt_mcrt_memset.d) : \
$(abspath $(SRC_DIR)/pt_mcrt_memset.cpp) ; ${VERBOSE} \
	mkdir -p $(dir $(abspath $(OBJ_DIR)/pt_mcrt_memset.d)); \
	$(CXX) -MMD -MP -MF $(abspath $(OBJ_DIR)/pt_mcrt_memset.d) \
	-c $(CXX_FLAGS) -o $(abspath $(OBJ_DIR)/pt_mcrt_memset.o) \
	$(abspath $(SRC_DIR)/pt_mcrt_memset.cpp)

$(abspath $(OBJ_DIR)/pt_mcrt_memcmp.o) \
$(abspath $(OBJ_DIR)/pt_mcrt_memcmp.d) : \
$(abspath $(SRC_DIR)/pt_mcrt_memcmp.cpp) ; ${VERBOSE} \
	mkdir -p $(dir $(abspath $(OBJ_DIR)/pt_mcrt_memcmp.d)); \
	$(CXX) -MMD -MP -MF $(abspath $(OBJ_DIR)/pt_mcrt_memcmp.d) \
	-c $(CXX_FLAGS) -o $(abspath $(OBJ_DIR)/pt_mcrt_memcmp.o) \
	$(abspath $(SRC_DIR)/pt_mcrt_memcmp.cpp)

$(abspath $(OBJ_DIR)/pt_mcrt_memcpy.o) \
$(abspath $(OBJ_DIR)/pt_mcrt_memcpy.d) : \
$(abspath $(SRC_DIR)/pt_mcrt_memcpy.cpp) ; ${VERBOSE} \
	mkdir -p $(dir $(abspath $(OBJ_DIR)/pt_mcrt_memcpy.d)); \
	$(CXX) -MMD -MP -MF $(abspath $(OBJ_DIR)/pt_mcrt_memcpy.d) \
	-c $(CXX_FLAGS) -o $(abspath $(OBJ_DIR)/pt_mcrt_memcpy.o) \
	$(abspath $(SRC_DIR)/pt_mcrt_memcpy.cpp)

$(abspath $(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_ssse3.o) \
$(abspath $(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_ssse3.d) : \
$(abspath $(SRC_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_ssse3.cpp) ; ${VERBOSE} \
	mkdir -p $(dir $(abspath $(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_ssse3.d)); \
	$(CXX) -MMD -MP -MF $(abspath $(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_ssse3.d) \
	-c $(CXX_FLAGS) -mssse3 -o $(abspath $(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_ssse3.o) \
	$(abspath $(SRC_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_ssse3.cpp)

$(abspath $(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx.o) \
$(abspath $(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx.d) : \
$(abspath $(SRC_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx.cpp) ; ${VERBOSE} \
	mkdir -p $(dir $(abspath $(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx.d)); \
	$(CXX) -MMD -MP -MF $(abspath $(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx.d) \
	-c $(CXX_FLAGS) -mavx -o $(abspath $(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx.o) \
	$(abspath $(SRC_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx.cpp)

$(abspath $(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx512f.o) \
$(abspath $(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx512f.d) : \
$(abspath $(SRC_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx512f.cpp) ; ${VERBOSE} \
	mkdir -p $(dir $(abspath $(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx512f.d)); \
	$(CXX) -MMD -MP -MF $(abspath $(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx512f.d) \
	-c $(CXX_FLAGS) -mavx512f -o $(abspath $(OBJ_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx512f.o) \
	$(abspath $(SRC_DIR)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx512f.cpp)

$(abspath $(OBJ_DIR)/pt_mcrt_malloc.o) \
$(abspath $(OBJ_DIR)/pt_mcrt_malloc.d) : \
$(abspath $(SRC_DIR)/pt_mcrt_malloc.cpp) ; ${VERBOSE} \
	mkdir -p $(dir $(abspath $(OBJ_DIR)/pt_mcrt_malloc.d)); \
	$(CXX) -MMD -MP -MF $(abspath $(OBJ_DIR)/pt_mcrt_malloc.d) \
	-c $(CXX_FLAGS) -o $(abspath $(OBJ_DIR)/pt_mcrt_malloc.o) \
	$(abspath $(SRC_DIR)/pt_mcrt_malloc.cpp)

$(abspath $(OBJ_DIR)/pt_mcrt_task.o) \
$(abspath $(OBJ_DIR)/pt_mcrt_task.d) : \
$(abspath $(SRC_DIR)/pt_mcrt_task.cpp) ; ${VERBOSE} \
	mkdir -p $(dir $(abspath $(OBJ_DIR)/pt_mcrt_task.d)); \
	$(CXX) -MMD -MP -MF $(abspath $(OBJ_DIR)/pt_mcrt_task.d) \
	-c $(CXX_FLAGS) -o $(abspath $(OBJ_DIR)/pt_mcrt_task.o) \
	$(abspath $(SRC_DIR)/pt_mcrt_task.cpp)

include \
	$(abspath $(OBJ_DIR)/pt_mcrt_log.d) \
	$(abspath $(OBJ_DIR)/pt_mcrt_intrin.d) \
	$(abspath $(OBJ_DIR)/pt_mcrt_memset.d) \
	$(abspath $(OBJ_DIR)/pt_mcrt_memcmp.d) \
	$(abspath $(OBJ_DIR)/pt_mcrt_memcpy.d) \
	$(abspath $(OBJ_DIR)/pt_mcrt_malloc.d) \
	$(abspath $(OBJ_DIR)/pt_mcrt_task.d)