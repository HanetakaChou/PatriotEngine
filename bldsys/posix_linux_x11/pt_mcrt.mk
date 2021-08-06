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

# libpt_mcrt

include $(CLEAR_VARS)

LOCAL_MODULE := libpt_mcrt

LOCAL_SRC_FILES:= \
	$(abspath $(LOCAL_PATH)/../../src)/pt_mcrt_malloc.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_mcrt_task.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_mcrt_memcpy.cpp \
	$(abspath $(LOCAL_PATH)/../../src)/pt_mcrt_log.cpp \

#LOCAL_CFLAGS += -fdiagnostics-format=msvc
LOCAL_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -DPT_ATTR_MCRT=PT_ATTR_EXPORT

LOCAL_CPPFLAGS += -std=c++11

LOCAL_C_INCLUDES += $(abspath $(LOCAL_PATH)/../../include)

LOCAL_LDFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_LDFLAGS += -Wl,--enable-new-dtags # the linker can't recognize the old dtags
LOCAL_LDFLAGS += -Wl,-rpath,XORIGIN # chrpath can only make path shorter # fix me: define the $ORIGIN correctly in the Linux_X11.mk
LOCAL_LDFLAGS += -Wl,--version-script,$(abspath $(LOCAL_PATH))/pt_mcrt.def

LOCAL_SHARED_LIBRARIES := libpt_tbbmalloc libpt_irml libpt_tbb

LOCAL_STATIC_LIBRARIES := libpt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx512f libpt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx pt_mcrt_memcpy_dpdk_rte_memcpy_x86_ssse3 libpt_mcrt_memcpy_dpdk_rte_memcpy_arm32_neon

LOCAL_EXPORT_C_INCLUDES := $(abspath $(LOCAL_PATH)/../../include) 

include $(BUILD_SHARED_LIBRARY)

# -mavx512f / libpt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx512f

include $(CLEAR_VARS)

LOCAL_MODULE := libpt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx512f

LOCAL_SRC_FILES:= \
	$(abspath $(LOCAL_PATH)/../../src)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx512f.cpp \

#LOCAL_CFLAGS += -fdiagnostics-format=msvc
LOCAL_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -DPT_ATTR_MCRT=PT_ATTR_EXPORT
	
LOCAL_CPPFLAGS += -std=c++11

ifeq (x86_64,$(TARGET_ARCH))
LOCAL_CFLAGS += -mavx512f 
endif
ifeq (x86,$(TARGET_ARCH))
LOCAL_CFLAGS += -mavx512f 
endif

LOCAL_C_INCLUDES += $(abspath $(LOCAL_PATH)/../../include)

include $(BUILD_STATIC_LIBRARY)

# -mavx / libpt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx

include $(CLEAR_VARS)

LOCAL_MODULE := libpt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx

LOCAL_SRC_FILES:= \
	$(abspath $(LOCAL_PATH)/../../src)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_avx.cpp \

#LOCAL_CFLAGS += -fdiagnostics-format=msvc
LOCAL_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -DPT_ATTR_MCRT=PT_ATTR_EXPORT
	
LOCAL_CPPFLAGS += -std=c++11

ifeq (x86_64,$(TARGET_ARCH))
LOCAL_CFLAGS += -mavx 
endif
ifeq (x86,$(TARGET_ARCH))
LOCAL_CFLAGS += -mavx 
endif

LOCAL_C_INCLUDES += $(abspath $(LOCAL_PATH)/../../include)

include $(BUILD_STATIC_LIBRARY)

# -mssse3 / libpt_mcrt_memcpy_dpdk_rte_memcpy_x86_ssse3

include $(CLEAR_VARS)

LOCAL_MODULE := libpt_mcrt_memcpy_dpdk_rte_memcpy_x86_ssse3

LOCAL_SRC_FILES:= \
	$(abspath $(LOCAL_PATH)/../../src)/pt_mcrt_memcpy_dpdk_rte_memcpy_x86_ssse3.cpp \

#LOCAL_CFLAGS += -fdiagnostics-format=msvc
LOCAL_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -DPT_ATTR_MCRT=PT_ATTR_EXPORT
	
LOCAL_CPPFLAGS += -std=c++11

ifeq (x86_64,$(TARGET_ARCH))
LOCAL_CFLAGS += -mssse3 
endif
ifeq (x86,$(TARGET_ARCH))
LOCAL_CFLAGS += -mssse3 
endif

LOCAL_C_INCLUDES += $(abspath $(LOCAL_PATH)/../../include)

include $(BUILD_STATIC_LIBRARY)

# neon / libpt_mcrt_memcpy_dpdk_rte_memcpy_arm32_neon

include $(CLEAR_VARS)

LOCAL_MODULE := libpt_mcrt_memcpy_dpdk_rte_memcpy_arm32_neon

LOCAL_SRC_FILES:= \
	$(abspath $(LOCAL_PATH)/../../src)/pt_mcrt_memcpy_dpdk_rte_memcpy_arm32_neon.cpp \

#LOCAL_CFLAGS += -fdiagnostics-format=msvc
LOCAL_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -DPT_ATTR_MCRT=PT_ATTR_EXPORT
	
LOCAL_CPPFLAGS += -std=c++11

ifeq (arm,$(TARGET_ARCH))
LOCAL_ARM_NEON := true
endif

LOCAL_C_INCLUDES += $(abspath $(LOCAL_PATH)/../../include)

include $(BUILD_STATIC_LIBRARY)