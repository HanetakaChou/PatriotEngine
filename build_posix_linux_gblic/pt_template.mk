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

BIN_DIR := ../bin/$(BUILD_CONFIGURATION)/$(BUILD_ARCHITECTURE)/
OBJ_DIR := ./obj/mcrt/$(BUILD_CONFIGURATION)/$(BUILD_ARCHITECTURE)/
BUILD_DIR := ./
INCLUDE_DIR := ../include/
SRC_DIR := ../src/
THIRD_PARTY_DIR := ../third_party/

BIN_ABS_DIR := $(abspath ../bin/$(BUILD_CONFIGURATION)/$(BUILD_ARCHITECTURE)/)
OBJ_ABS_DIR := $(abspath ./obj/mcrt/$(BUILD_CONFIGURATION)/$(BUILD_ARCHITECTURE)/)
BUILD_ABS_DIR := $(abspath ./)
INCLUDE_ABS_DIR := $(abspath ../include/)
SRC_ABS_DIR := $(abspath ../src/)
THIRD_PARTY_ABS_DIR := $(abspath ../third_party/)

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

CXX_COMPILER_FLAGS += -D_GLIBCXX_USE_CXX11_ABI=1 -static-libgcc -static-libstdc++ 
CXX_LINKER_FLAGS += -static-libgcc -static-libstdc++

ifeq (debug, $(BUILD_CONFIGURATION))
    CXX_COMPILER_FLAGS += -g -O0 -UNDEBUG
else ifeq (release, $(BUILD_CONFIGURATION))
    CXX_COMPILER_FLAGS += -O3 -DNDEBUG -flto
    CXX_LINKER_FLAGS += -flto
else
    The configuration "$(BUILD_CONFIGURATION)" is NOT supported!
endif

ifeq (x64, $(BUILD_ARCHITECTURE))
    CXX_COMPILER_FLAGS += -m64
    CXX_LINKER_FLAGS += -m64
else ifeq (x86, $(BUILD_ARCHITECTURE))
    CXX_COMPILER_FLAGS += -m32
    CXX_LINKER_FLAGS += -m32
else
    The architecture "$(BUILD_ARCHITECTURE)" is NOT supported!
endif

CXX_COMPILER_FLAGS += -fexec-charset=UTF-8 -finput-charset=UTF-8
CXX_COMPILER_FLAGS += -fno-unwind-tables -fno-exceptions -fno-rtti
CXX_COMPILER_FLAGS += -fvisibility=hidden

CXX_LINKER_FLAGS += -Wl,--enable-new-dtags -Wl,-rpath,'$$ORIGIN'

BUILD_VERBOSE = @

define BUILD_COPY_PREBUILT_LIBRARY =
$(BIN_DIR)/$(notdir $(1)) : \
    $(1) ; ${BUILD_VERBOSE} \
        mkdir -p "$(BIN_ABS_DIR)"; \
        cp -f "$(abspath $(1))" \
        "$(BIN_ABS_DIR)/$(notdir $(1))"
endef

define BUILD_CXX_COMPILE =
$(OBJ_DIR)/$(1).o \
$(OBJ_DIR)/$(1).d : \
$(SRC_DIR)/$(1).cpp ; ${BUILD_VERBOSE} \
    mkdir -p "$(OBJ_ABS_DIR)"; \
    $(CXX_COMPILER) -MMD -MP -MF "$(OBJ_ABS_DIR)/$(1).d" \
    -c $(CXX_COMPILER_FLAGS) -o "$(OBJ_ABS_DIR)/$(1).o" \
    "$(SRC_ABS_DIR)/$(1).cpp"
endef
