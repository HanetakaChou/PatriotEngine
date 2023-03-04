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

ifneq ($(filter $(BUILD_CONFIGURATION),debug release), $(BUILD_CONFIGURATION))
    define TEXT_ERROR_BUILD_CONFIGURATION =

    The configuration "$(BUILD_CONFIGURATION)" is NOT supported!

    Usage: pt_build.sh configuration architecture

    Configurations:
        debug    build with the debug configuration
        release    build with the release configuration

    Architectures:
        x64    build with the x64 architecture
        x86    build with the x86 architecture

    endef
    $(error "$(TEXT_ERROR_BUILD_CONFIGURATION)")
endif

ifneq ($(filter $(BUILD_ARCHITECTURE),x64 x86), $(BUILD_ARCHITECTURE))
    define TEXT_ERROR_BUILD_ARCHITECTURE =

    The architecture "$(BUILD_ARCHITECTURE)" is NOT supported!

    Usage: pt_build.sh configuration architecture

    Configurations:
        debug    build with the debug configuration
        release    build with the release configuration

    Architectures:
        x64    build with the x64 architecture
        x86    build with the x86 architecture

    endef
    $(error "$(TEXT_ERROR_BUILD_ARCHITECTURE)")
endif

BUILD_DIR = ./
OBJ_DIR = ./obj/$(BUILD_MODULE_NAME)/$(BUILD_CONFIGURATION)/$(BUILD_ARCHITECTURE)/
SPIRV_DIR = ./spirv/$(BUILD_MODULE_NAME)/$(BUILD_CONFIGURATION)/
BIN_DIR = ../bin/$(BUILD_CONFIGURATION)/$(BUILD_ARCHITECTURE)/
INCLUDE_DIR = ../include/
SRC_DIR = ../src/
THIRD_PARTY_DIR = ../third_party/

CXX_COMPILER = c++
CXX_LINKER = c++

CXX_COMPILER_FLAGS = 
CXX_LINKER_FLAGS = 

CXX_COMPILER_FLAGS += -pthread
CXX_LINKER_FLAGS += -pthread

CXX_COMPILER_FLAGS += -fPIE -fPIC 
CXX_LINKER_FLAGS += -Wl,-z,relro -Wl,-z,now

CXX_COMPILER_FLAGS += -fstack-protector-strong -Wa,--noexecstack
CXX_LINKER_FLAGS += -Wl,-z,noexecstack

CXX_COMPILER_FLAGS += -Wall -Werror=return-type
CXX_LINKER_FLAGS += -Wl,--no-undefined -Wl,--warn-shared-textrel -Wl,--fatal-warnings

CXX_COMPILER_FLAGS += -ffunction-sections -fdata-sections
CXX_LINKER_FLAGS += -Wl,--gc-sections

CXX_COMPILER_FLAGS += -x c++ -std=c++11 -D_GLIBCXX_USE_CXX11_ABI=1 -static-libgcc -static-libstdc++ 
CXX_LINKER_FLAGS += -static-libgcc -static-libstdc++

ifeq (debug, $(BUILD_CONFIGURATION))
    CXX_COMPILER_FLAGS += -g -O0 -UNDEBUG
    GLSL_COMPILER_FLAGS += -g -Od
else ifeq (release, $(BUILD_CONFIGURATION))
    CXX_COMPILER_FLAGS += -O3 -DNDEBUG -flto
    CXX_LINKER_FLAGS += -flto
else
    $(error The configuration "$(BUILD_CONFIGURATION)" is NOT supported!)
endif

ifeq (x64, $(BUILD_ARCHITECTURE))
    CXX_COMPILER_FLAGS += -m64 -mavx2 -mf16c -mfma 
    CXX_LINKER_FLAGS += -m64
else ifeq (x86, $(BUILD_ARCHITECTURE))
    CXX_COMPILER_FLAGS += -m32 -mavx2 -mf16c -mfma
    CXX_LINKER_FLAGS += -m32
else
    $(error The architecture "$(BUILD_ARCHITECTURE)" is NOT supported!)
endif

CXX_COMPILER_FLAGS += -fexec-charset=UTF-8 -finput-charset=UTF-8
CXX_COMPILER_FLAGS += -fno-unwind-tables -fno-exceptions -fno-rtti
CXX_COMPILER_FLAGS += -fvisibility=hidden

CXX_LINKER_FLAGS += -Wl,--enable-new-dtags -Wl,-rpath,'$$ORIGIN'

GLSL_COMPILER = glslangValidator

GLSL_COMPILER_FLAGS =

ifeq (debug, $(BUILD_CONFIGURATION))
    GLSL_COMPILER_FLAGS += -g -Od
else ifeq (release, $(BUILD_CONFIGURATION))
    GLSL_COMPILER_FLAGS += -g0 -Os
else
    $(error The configuration "$(BUILD_CONFIGURATION)" is NOT supported!)
endif

BUILD_VERBOSE = @

define BUILD_COPY_PREBUILT_LIBRARY =
$(BIN_DIR)/$(notdir $(1)) : \
$(1) ; ${BUILD_VERBOSE} \
    mkdir -p "$(abspath  $(dir $(BIN_DIR)/$(notdir $(1))))"; \
    cp -f "$(abspath $(1))" \
    "$(abspath $(BIN_DIR))/$(notdir $(1))"
endef

define BUILD_CXX_COMPILE =
$(OBJ_DIR)/$(1).o \
$(OBJ_DIR)/$(1).d : \
$(SRC_DIR)/$(1).cpp ; ${BUILD_VERBOSE} \
    mkdir -p "$(abspath $(dir $(OBJ_DIR)/$(1).o))"; \
    mkdir -p "$(abspath $(dir $(OBJ_DIR)/$(1).d))"; \
    $(CXX_COMPILER) -MMD -MP -MF "$(abspath $(OBJ_DIR)/$(1).d)" \
    -c $(CXX_COMPILER_FLAGS) -o "$(abspath $(OBJ_DIR)/$(1).o)" \
    "$(abspath $(SRC_DIR))/$(1).cpp"
endef

define BUILD_GLSL_COMPILE =
$(SPIRV_DIR)/$(2).inl \
$(SPIRV_DIR)/$(2).d : \
$(SRC_DIR)/$(2).glsl ; $(BUILD_VERBOSE) \
	mkdir -p "$(abspath $(dir $(SPIRV_DIR)/$(2).inl))"; \
	mkdir -p "$(abspath $(dir $(SPIRV_DIR)/$(2).d))"; \
	$(GLSL_COMPILER) --depfile "$(abspath $(SPIRV_DIR)/$(2).d)" \
	-V100 -x $(GLSL_COMPILER_FLAGS) -S $(1) -o "$(abspath $(SPIRV_DIR)/$(2).inl)" \
	"$(abspath $(SRC_DIR)/$(2).glsl)"
endef
