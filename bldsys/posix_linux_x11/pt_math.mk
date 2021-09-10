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
LOCAL_CFLAGS += -Werror=return-type
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -DPT_ATTR_MATH=PT_ATTR_EXPORT
LOCAL_CFLAGS += -Wall

ifeq (arm,$(TARGET_ARCH))
LOCAL_ARM_MODE := arm
LOCAL_ARM_NEON := true
endif

#LOCAL_CPPFLAGS += -std=c++11

LOCAL_C_INCLUDES += $(abspath $(LOCAL_PATH)/../../include)

LOCAL_LDFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_LDFLAGS += -Wl,--enable-new-dtags # the linker can't recognize the old dtags
LOCAL_LDFLAGS += -Wl,-rpath,XORIGIN # chrpath can only make path shorter # fix me: define the $ORIGIN correctly in the Linux_X11.mk
LOCAL_LDFLAGS += -Wl,--version-script,$(abspath $(LOCAL_PATH))/pt_math.def

ifeq ($(filter $(TARGET_ARCH),x86_64 x86),$(TARGET_ARCH))
LOCAL_SHARED_LIBRARIES += libpt_math_directx_math_x86_avx2 libpt_math_directx_math_x86_avx libpt_math_directx_math_x86_sse2
endif

ifeq ($(filter $(TARGET_ARCH),arm64 arm),$(TARGET_ARCH))
LOCAL_SHARED_LIBRARIES += libpt_math_directx_math_arm_neon
endif

LOCAL_EXPORT_C_INCLUDES := $(abspath $(LOCAL_PATH)/../../include) 

include $(BUILD_SHARED_LIBRARY)


# -mavx2 -mfma / libpt_math_directx_math_x86_avx2

include $(CLEAR_VARS)

LOCAL_MODULE := libpt_math_directx_math_x86_avx2

LOCAL_SRC_FILES:= \
	$(abspath $(LOCAL_PATH)/../../src)/pt_math_directx_math_x86_avx2.cpp \

#LOCAL_CFLAGS += -fdiagnostics-format=msvc
LOCAL_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_CFLAGS += -Werror=return-type
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -DPT_ATTR_MATH=PT_ATTR_EXPORT
LOCAL_CFLAGS += -Wall
LOCAL_CFLAGS += -Wno-unknown-pragmas
	
LOCAL_CFLAGS += -mavx2
LOCAL_CFLAGS += -mfma

#LOCAL_CPPFLAGS += -std=c++11

LOCAL_C_INCLUDES += $(abspath $(LOCAL_PATH)/../../include)
LOCAL_C_INCLUDES += $(abspath $(LOCAL_PATH)/../../third_party/libs/DirectXMath)/Inc 

include $(BUILD_STATIC_LIBRARY)

# -mavx / libpt_math_directx_math_x86_avx

ifeq ($(filter $(TARGET_ARCH),x86_64 x86),$(TARGET_ARCH))

include $(CLEAR_VARS)

LOCAL_MODULE := libpt_math_directx_math_x86_avx

LOCAL_SRC_FILES:= \
	$(abspath $(LOCAL_PATH)/../../src)/pt_math_directx_math_x86_avx.cpp \

#LOCAL_CFLAGS += -fdiagnostics-format=msvc
LOCAL_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_CFLAGS += -Werror=return-type
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -DPT_ATTR_MATH=PT_ATTR_EXPORT
LOCAL_CFLAGS += -Wall
LOCAL_CFLAGS += -Wno-unknown-pragmas
	
LOCAL_CFLAGS += -mavx 

#LOCAL_CPPFLAGS += -std=c++11

LOCAL_C_INCLUDES += $(abspath $(LOCAL_PATH)/../../include)
LOCAL_C_INCLUDES += $(abspath $(LOCAL_PATH)/../../third_party/libs/DirectXMath)/Inc 

include $(BUILD_STATIC_LIBRARY)

endif

# -mavx / libpt_math_directx_math_x86_sse2

ifeq ($(filter $(TARGET_ARCH),x86_64 x86),$(TARGET_ARCH))

include $(CLEAR_VARS)

LOCAL_MODULE := libpt_math_directx_math_x86_sse2

LOCAL_SRC_FILES:= \
	$(abspath $(LOCAL_PATH)/../../src)/pt_math_directx_math_x86_sse2.cpp \

#LOCAL_CFLAGS += -fdiagnostics-format=msvc
LOCAL_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_CFLAGS += -Werror=return-type
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -DPT_ATTR_MATH=PT_ATTR_EXPORT
LOCAL_CFLAGS += -Wall
LOCAL_CFLAGS += -Wno-unknown-pragmas
	
ifeq (x86,$(TARGET_ARCH))
LOCAL_CFLAGS += -msse2
endif

#LOCAL_CPPFLAGS += -std=c++11

LOCAL_C_INCLUDES += $(abspath $(LOCAL_PATH)/../../include)
LOCAL_C_INCLUDES += $(abspath $(LOCAL_PATH)/../../third_party/libs/DirectXMath)/Inc 

include $(BUILD_STATIC_LIBRARY)

endif

# -neon / pt_math_directx_math_arm_neon

ifeq ($(filter $(TARGET_ARCH),arm64 arm),$(TARGET_ARCH))

include $(CLEAR_VARS)

LOCAL_MODULE := libpt_math_directx_math_arm_neon

LOCAL_SRC_FILES:= \
	$(abspath $(LOCAL_PATH)/../../src)/pt_math_directx_math_arm_neon.cpp \

#LOCAL_CFLAGS += -fdiagnostics-format=msvc
LOCAL_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_CFLAGS += -Werror=return-type
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -DPT_ATTR_MATH=PT_ATTR_EXPORT
LOCAL_CFLAGS += -Wall
LOCAL_CFLAGS += -Wno-unknown-pragmas
	
ifeq (arm,$(TARGET_ARCH))
LOCAL_ARM_MODE := arm
LOCAL_ARM_NEON := true
endif

#LOCAL_CPPFLAGS += -std=c++11

LOCAL_C_INCLUDES += $(abspath $(LOCAL_PATH)/../../include)
LOCAL_C_INCLUDES += $(abspath $(LOCAL_PATH)/../../third_party/libs/DirectXMath)/Inc 

include $(BUILD_STATIC_LIBRARY)

endif
