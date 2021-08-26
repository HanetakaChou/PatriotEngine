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

include $(LOCAL_PATH)/pt_math.mk

include $(LOCAL_PATH)/pt_mcrt.mk

include $(LOCAL_PATH)/pt_launcher.mk

# intel_tbb / libpt_tbbmalloc 

include $(CLEAR_VARS)

LOCAL_MODULE := libpt_tbbmalloc

ifeq (x86_64,$(TARGET_ARCH))
  LOCAL_SRC_FILES := $(abspath $(LOCAL_PATH)/../../third_party/libs/intel_tbb)/lib/bionic_x64/libpt_tbbmalloc$(TARGET_SONAME_EXTENSION)
endif
ifeq (x86,$(TARGET_ARCH))
  LOCAL_SRC_FILES := $(abspath $(LOCAL_PATH)/../../third_party/libs/intel_tbb)/lib/bionic_x86/libpt_tbbmalloc$(TARGET_SONAME_EXTENSION)
endif
ifeq (arm64,$(TARGET_ARCH))
  LOCAL_SRC_FILES := $(abspath $(LOCAL_PATH)/../../third_party/libs/intel_tbb)/lib/bionic_arm64/libpt_tbbmalloc$(TARGET_SONAME_EXTENSION)
endif
ifeq (arm,$(TARGET_ARCH))
  LOCAL_SRC_FILES := $(abspath $(LOCAL_PATH)/../../third_party/libs/intel_tbb)/lib/bionic_arm/libpt_tbbmalloc$(TARGET_SONAME_EXTENSION)
endif

LOCAL_EXPORT_C_INCLUDES := $(abspath $(LOCAL_PATH)/../../third_party/libs/intel_tbb)/include 

include $(PREBUILT_SHARED_LIBRARY)

# intel_tbb / libpt_irml 

include $(CLEAR_VARS)

LOCAL_MODULE := libpt_irml

ifeq (x86_64,$(TARGET_ARCH))
  LOCAL_SRC_FILES := $(abspath $(LOCAL_PATH)/../../third_party/libs/intel_tbb)/lib/bionic_x64/libpt_irml$(TARGET_SONAME_EXTENSION)
endif
ifeq (x86,$(TARGET_ARCH))
  LOCAL_SRC_FILES := $(abspath $(LOCAL_PATH)/../../third_party/libs/intel_tbb)/lib/bionic_x86/libpt_irml$(TARGET_SONAME_EXTENSION)
endif
ifeq (arm64,$(TARGET_ARCH))
  LOCAL_SRC_FILES := $(abspath $(LOCAL_PATH)/../../third_party/libs/intel_tbb)/lib/bionic_arm64/libpt_irml$(TARGET_SONAME_EXTENSION)
endif
ifeq (arm,$(TARGET_ARCH))
  LOCAL_SRC_FILES := $(abspath $(LOCAL_PATH)/../../third_party/libs/intel_tbb)/lib/bionic_arm/libpt_irml$(TARGET_SONAME_EXTENSION)
endif

LOCAL_EXPORT_C_INCLUDES := $(abspath $(LOCAL_PATH)/../../third_party/libs/intel_tbb)/include 

include $(PREBUILT_SHARED_LIBRARY)

# intel_tbb / libpt_tbb 

include $(CLEAR_VARS)

LOCAL_MODULE := libpt_tbb

ifeq (x86_64,$(TARGET_ARCH))
  LOCAL_SRC_FILES := $(abspath $(LOCAL_PATH)/../../third_party/libs/intel_tbb)/lib/bionic_x64/libpt_tbb$(TARGET_SONAME_EXTENSION)
endif
ifeq (x86,$(TARGET_ARCH))
  LOCAL_SRC_FILES := $(abspath $(LOCAL_PATH)/../../third_party/libs/intel_tbb)/lib/bionic_x86/libpt_tbb$(TARGET_SONAME_EXTENSION)
endif
ifeq (arm64,$(TARGET_ARCH))
  LOCAL_SRC_FILES := $(abspath $(LOCAL_PATH)/../../third_party/libs/intel_tbb)/lib/bionic_arm64/libpt_tbb$(TARGET_SONAME_EXTENSION)
endif
ifeq (arm,$(TARGET_ARCH))
  LOCAL_SRC_FILES := $(abspath $(LOCAL_PATH)/../../third_party/libs/intel_tbb)/lib/bionic_arm/libpt_tbb$(TARGET_SONAME_EXTENSION)
endif

LOCAL_EXPORT_C_INCLUDES := $(abspath $(LOCAL_PATH)/../../third_party/libs/intel_tbb)/include 

include $(PREBUILT_SHARED_LIBRARY)
