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

LOCAL_CFLAGS += -fdiagnostics-format=msvc
LOCAL_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -DPT_MCRT_ATTR=PT_EXPORT
	
LOCAL_CPPFLAGS += -std=c++11

ifeq (x86,$(TARGET_ARCH))
LOCAL_CFLAGS += -mssse3
endif

LOCAL_LDFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_LDFLAGS += -Wl,--enable-new-dtags # the linker can't recognize the old dtags
LOCAL_LDFLAGS += -Wl,-rpath,/XXXXXX # chrpath can only make path shorter # fix me: define the $ORIGIN correctly in the Linux_X11.mk

include $(BUILD_SHARED_LIBRARY)

# pt_launcher

include $(CLEAR_VARS)

LOCAL_MODULE := pt_launcher

LOCAL_SRC_FILES:= \
	$(abspath $(LOCAL_PATH)/../../src)/pt_wsi_window_posix_linux_x11.cpp \

LOCAL_CFLAGS += -fdiagnostics-format=msvc
LOCAL_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -DPT_WSI_ATTR=PT_EXPORT

LOCAL_CPPFLAGS += -std=c++11

LOCAL_LDFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_LDFLAGS += -Wl,--enable-new-dtags # the linker can't recognize the old dtags
LOCAL_LDFLAGS += -Wl,-rpath,/XXXXXX # chrpath can only make path shorter

LOCAL_LDLIBS += -lxcb

LOCAL_SHARED_LIBRARIES := libpt_mcrt

#LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../ThirdParty/PosixLinuxX11/Bionic-Redistributable/include/

#LOCAL_C_INCLUDES += /system/include

#ifeq (x86_64,$(TARGET_ARCH))
#LOCAL_LDFLAGS += -L/system/lib64
#endif

#ifeq (x86,$(TARGET_ARCH))
#LOCAL_LDFLAGS += -L/system/lib
#endif

include $(BUILD_EXECUTABLE)