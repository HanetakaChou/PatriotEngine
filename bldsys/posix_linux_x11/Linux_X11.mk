#
# Copyright (C) YuqiaoZhang
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

# https://developer.android.com/ndk/guides/android_mk

LOCAL_PATH:= $(call my-dir)

# libpt_mcrt

include $(CLEAR_VARS)

LOCAL_MODULE := libpt_mcrt

LOCAL_SRC_FILES:= \
	$(abspath $(LOCAL_PATH)/../../src)/pt_mcrt_malloc.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_mcrt_memcpy.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_mcrt_task.cpp \

LOCAL_CFLAGS += -fdiagnostics-format=msvc
LOCAL_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -DPT_MCRT_ATTR=PT_EXPORT
	
LOCAL_CPPFLAGS += -std=c++11

ifeq (x86,$(TARGET_ARCH))
LOCAL_CFLAGS += -mssse3
endif

LOCAL_C_INCLUDES += $(abspath $(LOCAL_PATH)/../../include)

LOCAL_LDFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_LDFLAGS += -Wl,--enable-new-dtags # the linker can't recognize the old dtags
LOCAL_LDFLAGS += -Wl,-rpath,XORIGIN # chrpath can only make path shorter # fix me: define the $ORIGIN correctly in the Linux_X11.mk
LOCAL_LDFLAGS += -Wl,--version-script,$(abspath $(LOCAL_PATH))/pt_mcrt.def

LOCAL_SHARED_LIBRARIES := pt_tbbmalloc pt_irml pt_tbb

LOCAL_EXPORT_C_INCLUDES := $(abspath $(LOCAL_PATH)/../../include) 

include $(BUILD_SHARED_LIBRARY)

# intel_tbb  

include $(CLEAR_VARS)

LOCAL_MODULE := pt_tbbmalloc

ifeq ($(TARGET_ARCH_ABI),x86)
  LOCAL_SRC_FILES := $(abspath $(LOCAL_PATH)/../../third_party/libs/intel_tbb)/lib/glibc_x86/libpt_tbbmalloc$(TARGET_SONAME_EXTENSION)
endif
ifeq ($(TARGET_ARCH_ABI),x86_64)
  LOCAL_SRC_FILES := $(abspath $(LOCAL_PATH)/../../third_party/libs/intel_tbb)/lib/glibc_x64/libpt_tbbmalloc$(TARGET_SONAME_EXTENSION)
endif

LOCAL_EXPORT_C_INCLUDES := $(abspath $(LOCAL_PATH)/../../third_party/libs/intel_tbb)/include 

include $(PREBUILT_SHARED_LIBRARY)

#--- 

include $(CLEAR_VARS)

LOCAL_MODULE := pt_irml

ifeq ($(TARGET_ARCH_ABI),x86)
  LOCAL_SRC_FILES := $(abspath $(LOCAL_PATH)/../../third_party/libs/intel_tbb)/lib/glibc_x86/libpt_irml$(TARGET_SONAME_EXTENSION)
endif
ifeq ($(TARGET_ARCH_ABI),x86_64)
  LOCAL_SRC_FILES := $(abspath $(LOCAL_PATH)/../../third_party/libs/intel_tbb)/lib/glibc_x64/libpt_irml$(TARGET_SONAME_EXTENSION)
endif

LOCAL_EXPORT_C_INCLUDES := $(abspath $(LOCAL_PATH)/../../third_party/libs/intel_tbb)/include 

include $(PREBUILT_SHARED_LIBRARY)

#--- 

include $(CLEAR_VARS)

LOCAL_MODULE := pt_tbb

ifeq ($(TARGET_ARCH_ABI),x86)
  LOCAL_SRC_FILES := $(abspath $(LOCAL_PATH)/../../third_party/libs/intel_tbb)/lib/glibc_x86/libpt_tbb$(TARGET_SONAME_EXTENSION)
endif
ifeq ($(TARGET_ARCH_ABI),x86_64)
  LOCAL_SRC_FILES := $(abspath $(LOCAL_PATH)/../../third_party/libs/intel_tbb)/lib/glibc_x64/libpt_tbb$(TARGET_SONAME_EXTENSION)
endif

LOCAL_EXPORT_C_INCLUDES := $(abspath $(LOCAL_PATH)/../../third_party/libs/intel_tbb)/include 

include $(PREBUILT_SHARED_LIBRARY)

# libpt_gfx

include $(CLEAR_VARS)

LOCAL_MODULE := libpt_gfx

LOCAL_SRC_FILES:= \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_connection_common.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_connection_d3d12.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_connection_mtl.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_connection_vk.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_connection_vk_posix_linux_x11.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_texture_common.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_texture_common_load_dds.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_texture_common_load_pvr.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_gfx_texture_vk.cpp   

LOCAL_CFLAGS += -fdiagnostics-format=msvc
LOCAL_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -DPT_GFX_ATTR=PT_EXPORT
	
LOCAL_CPPFLAGS += -std=c++11

ifeq (x86,$(TARGET_ARCH))
LOCAL_CFLAGS += -mssse3
endif

LOCAL_C_INCLUDES += $(abspath $(LOCAL_PATH)/../../include)

LOCAL_LDFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_LDFLAGS += -Wl,--enable-new-dtags # the linker can't recognize the old dtags
LOCAL_LDFLAGS += -Wl,-rpath,XORIGIN # chrpath can only make path shorter # fix me: define the $ORIGIN correctly in the Linux_X11.mk
LOCAL_LDFLAGS += -Wl,--version-script,$(abspath $(LOCAL_PATH))/pt_gfx.def

LOCAL_LDLIBS += -lvulkan 

LOCAL_SHARED_LIBRARIES := libpt_mcrt

LOCAL_EXPORT_C_INCLUDES := $(abspath $(LOCAL_PATH)/../../include) 

include $(BUILD_SHARED_LIBRARY)

# examples / pt_launcher

include $(CLEAR_VARS)

LOCAL_MODULE := pt_launcher

LOCAL_SRC_FILES:= \
	$(abspath $(LOCAL_PATH)/../../examples/launcher)/pt_wsi_window_posix_linux_x11.cpp \
	$(abspath $(LOCAL_PATH)/../../examples/launcher)/pt_app_main.cpp \

LOCAL_CFLAGS += -fdiagnostics-format=msvc
LOCAL_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -DPT_WSI_ATTR=PT_EXPORT

LOCAL_CPPFLAGS += -std=c++11

LOCAL_LDFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_LDFLAGS += -Wl,--enable-new-dtags # the linker can't recognize the old dtags
LOCAL_LDFLAGS += -Wl,-rpath,XORIGIN # chrpath can only make path shorter
LOCAL_LDFLAGS += -Wl,--version-script,$(abspath $(LOCAL_PATH))/pt_launcher.def

LOCAL_LDLIBS += -lxcb

LOCAL_SHARED_LIBRARIES := libpt_mcrt libpt_gfx

#LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../ThirdParty/PosixLinuxX11/Bionic-Redistributable/include/

#LOCAL_C_INCLUDES += /system/include

#ifeq (x86_64,$(TARGET_ARCH))
#LOCAL_LDFLAGS += -L/system/lib64
#endif

#ifeq (x86,$(TARGET_ARCH))
#LOCAL_LDFLAGS += -L/system/lib
#endif

include $(BUILD_EXECUTABLE)

# examples / general_acyclic_graphs_of_tasks

include $(CLEAR_VARS)

LOCAL_MODULE := pt_general_acyclic_graphs_of_tasks

LOCAL_SRC_FILES:= \
	$(abspath $(LOCAL_PATH)/../../examples/mcrt/general_acyclic_graphs_of_tasks)/main.cpp \

LOCAL_CFLAGS += -fdiagnostics-format=msvc
LOCAL_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_CFLAGS += -fvisibility=hidden

LOCAL_CPPFLAGS += -std=c++11

LOCAL_LDFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_LDFLAGS += -Wl,--enable-new-dtags # the linker can't recognize the old dtags
LOCAL_LDFLAGS += -Wl,-rpath,XORIGIN # chrpath can only make path shorter
LOCAL_LDFLAGS += -Wl,--version-script,$(abspath $(LOCAL_PATH))/pt_general_acyclic_graphs_of_tasks.def

LOCAL_SHARED_LIBRARIES := libpt_mcrt

include $(BUILD_EXECUTABLE)
