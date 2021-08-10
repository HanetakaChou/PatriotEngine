#
# Copyright (C) YuqiaoZhang(HanetakaYuminaga)
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

# libpt_math

include $(CLEAR_VARS)

LOCAL_MODULE := libpt_math

LOCAL_SRC_FILES:= \
	$(abspath $(LOCAL_PATH)/../../src)/pt_math.cpp \

#LOCAL_CFLAGS += -fdiagnostics-format=msvc
LOCAL_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -DPT_ATTR_MATH=PT_ATTR_EXPORT
LOCAL_CFLAGS += -Wall

#LOCAL_CPPFLAGS += -std=c++11

LOCAL_C_INCLUDES += $(abspath $(LOCAL_PATH)/../../include)

LOCAL_LDFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_LDFLAGS += -Wl,--enable-new-dtags # the linker can't recognize the old dtags
LOCAL_LDFLAGS += -Wl,-rpath,XORIGIN # chrpath can only make path shorter # fix me: define the $ORIGIN correctly in the Linux_X11.mk
LOCAL_LDFLAGS += -Wl,--version-script,$(abspath $(LOCAL_PATH))/pt_math.def

LOCAL_SHARED_LIBRARIES := libpt_math_directx_math_x86_avx2 libpt_math_directx_math_x86_avx libpt_math_directx_math_x86_sse2

LOCAL_EXPORT_C_INCLUDES := $(abspath $(LOCAL_PATH)/../../include) 

include $(BUILD_SHARED_LIBRARY)


# -mavx2 / libpt_math_directx_math_x86_avx2

include $(CLEAR_VARS)

LOCAL_MODULE := libpt_math_directx_math_x86_avx2

LOCAL_SRC_FILES:= \
	$(abspath $(LOCAL_PATH)/../../src)/pt_math_directx_math_x86_avx2.cpp \

#LOCAL_CFLAGS += -fdiagnostics-format=msvc
LOCAL_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -DPT_ATTR_MATH=PT_ATTR_EXPORT
LOCAL_CFLAGS += -Wall
	
#LOCAL_CPPFLAGS += -std=c++11

ifeq (x86_64,$(TARGET_ARCH))
LOCAL_CFLAGS += -mavx2 
endif
ifeq (x86,$(TARGET_ARCH))
LOCAL_CFLAGS += -mavx2 
endif

LOCAL_C_INCLUDES += $(abspath $(LOCAL_PATH)/../../include)
LOCAL_C_INCLUDES += $(abspath $(LOCAL_PATH)/../../third_party/libs/DirectXMath)/Inc 

include $(BUILD_STATIC_LIBRARY)

# -mavx / libpt_math_directx_math_x86_avx

include $(CLEAR_VARS)

LOCAL_MODULE := libpt_math_directx_math_x86_avx

LOCAL_SRC_FILES:= \
	$(abspath $(LOCAL_PATH)/../../src)/pt_math_directx_math_x86_avx.cpp \

#LOCAL_CFLAGS += -fdiagnostics-format=msvc
LOCAL_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -DPT_ATTR_MATH=PT_ATTR_EXPORT
LOCAL_CFLAGS += -Wall
	
#LOCAL_CPPFLAGS += -std=c++11

ifeq (x86_64,$(TARGET_ARCH))
LOCAL_CFLAGS += -mavx 
endif
ifeq (x86,$(TARGET_ARCH))
LOCAL_CFLAGS += -mavx 
endif

LOCAL_C_INCLUDES += $(abspath $(LOCAL_PATH)/../../include)
LOCAL_C_INCLUDES += $(abspath $(LOCAL_PATH)/../../third_party/libs/DirectXMath)/Inc 

include $(BUILD_STATIC_LIBRARY)

# -mavx / libpt_math_directx_math_x86_sse2

include $(CLEAR_VARS)

LOCAL_MODULE := libpt_math_directx_math_x86_sse2

LOCAL_SRC_FILES:= \
	$(abspath $(LOCAL_PATH)/../../src)/pt_math_directx_math_x86_sse2.cpp \

#LOCAL_CFLAGS += -fdiagnostics-format=msvc
LOCAL_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -DPT_ATTR_MATH=PT_ATTR_EXPORT
LOCAL_CFLAGS += -Wall
	
#LOCAL_CPPFLAGS += -std=c++11

ifeq (x86_64,$(TARGET_ARCH))
LOCAL_CFLAGS += -msse2
endif
ifeq (x86,$(TARGET_ARCH))
LOCAL_CFLAGS += -msse2
endif

LOCAL_C_INCLUDES += $(abspath $(LOCAL_PATH)/../../include)
LOCAL_C_INCLUDES += $(abspath $(LOCAL_PATH)/../../third_party/libs/DirectXMath)/Inc 

include $(BUILD_STATIC_LIBRARY)