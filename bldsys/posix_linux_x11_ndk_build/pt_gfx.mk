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

LOCAL_PATH:= $(call my-dir)

# libpt_gfx

include $(CLEAR_VARS)

LOCAL_MODULE := libpt_gfx

LOCAL_SRC_FILES:= \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_connection_base.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_connection_vk.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_device_vk.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_device_vk_wsi_x11.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_malloc_vk.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_malloc_vma.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_streaming_object_base.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_frame_object_base.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_node_base.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_node_vk.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_mesh_base.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_mesh_base_load.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_mesh_base_load_pmx.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_mesh_vk.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_material_base.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_material_vk.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_texture_base.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_texture_base_load.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_texture_base_load_dds.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_texture_base_load_pvr.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_texture_vk.cpp \

#LOCAL_CFLAGS += -fdiagnostics-format=msvc
LOCAL_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_CFLAGS += -Werror=return-type
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -DPT_ATTR_GFX=PT_ATTR_EXPORT
LOCAL_CFLAGS += -Wall

ifeq (arm,$(TARGET_ARCH))
LOCAL_ARM_MODE := arm
LOCAL_ARM_NEON := true
endif

#LOCAL_CPPFLAGS += -std=c++11

LOCAL_C_INCLUDES += $(abspath $(LOCAL_PATH)/../../include)
LOCAL_C_INCLUDES := $(abspath $(LOCAL_PATH)/../../third_party/vma)/include 

LOCAL_LDFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_LDFLAGS += -Wl,--enable-new-dtags # the linker can't recognize the old dtags
LOCAL_LDFLAGS += -Wl,-rpath,XORIGIN # chrpath can only make path shorter # fix me: define the $ORIGIN correctly in the Linux_X11.mk
LOCAL_LDFLAGS += -Wl,--version-script,$(abspath $(LOCAL_PATH))/pt_gfx.def

LOCAL_LDLIBS += -lvulkan 

LOCAL_SHARED_LIBRARIES := libpt_mcrt libpt_math

LOCAL_EXPORT_C_INCLUDES := $(abspath $(LOCAL_PATH)/../../include) 

include $(BUILD_SHARED_LIBRARY)
