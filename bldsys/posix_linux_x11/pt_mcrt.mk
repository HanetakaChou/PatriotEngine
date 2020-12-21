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

LOCAL_PATH:= $(call my-dir)

# libpt_mcrt

include $(CLEAR_VARS)

LOCAL_MODULE := libpt_mcrt

LOCAL_SRC_FILES:= \
	$(abspath $(LOCAL_PATH)/../../src)/pt_mcrt_malloc.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_mcrt_task.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_mcrt_memcpy.cpp \

LOCAL_CFLAGS += -fdiagnostics-format=msvc
LOCAL_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -DPT_ATTR_MCRT=PT_ATTR_EXPORT
	
LOCAL_CPPFLAGS += -std=c++11

#-mavx512f 
#-mavx2
#-mssse3
ifeq (x86_64,$(TARGET_ARCH))
LOCAL_CFLAGS += -mavx2 
endif
ifeq (x86,$(TARGET_ARCH))
LOCAL_CFLAGS += -mavx2 
endif

LOCAL_C_INCLUDES += $(abspath $(LOCAL_PATH)/../../include)

LOCAL_LDFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_LDFLAGS += -Wl,--enable-new-dtags # the linker can't recognize the old dtags
LOCAL_LDFLAGS += -Wl,-rpath,XORIGIN # chrpath can only make path shorter # fix me: define the $ORIGIN correctly in the Linux_X11.mk
LOCAL_LDFLAGS += -Wl,--version-script,$(abspath $(LOCAL_PATH))/pt_mcrt.def

LOCAL_SHARED_LIBRARIES := libpt_tbbmalloc libpt_irml libpt_tbb

LOCAL_EXPORT_C_INCLUDES := $(abspath $(LOCAL_PATH)/../../include) 

include $(BUILD_SHARED_LIBRARY)


