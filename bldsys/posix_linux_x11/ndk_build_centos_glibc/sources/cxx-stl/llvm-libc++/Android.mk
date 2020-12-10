# This file is dual licensed under the MIT and the University of Illinois Open
# Source Licenses. See LICENSE.TXT for details.

LOCAL_PATH := $(call my-dir)

$(call ndk_log,Using prebuilt libc++ libraries)

include $(CLEAR_VARS)
LOCAL_MODULE := c++_static
LOCAL_SRC_FILES := $(LOCAL_PATH)/libs/$(TARGET_ARCH_ABI)/libc$(TARGET_SONAME_EXTENSION)
LOCAL_EXPORT_CPPFLAGS := -std=c++11 -D_GLIBCXX_USE_CXX11_ABI=0 -static-libstdc++ 
LOCAL_EXPORT_LDFLAGS := -static-libstdc++ -L$(LOCAL_PATH)/libs/$(TARGET_ARCH_ABI)
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := c++_shared
LOCAL_SRC_FILES := $(LOCAL_PATH)/libs/$(TARGET_ARCH_ABI)/libc$(TARGET_SONAME_EXTENSION)
#LOCAL_EXPORT_C_INCLUDES :=  
LOCAL_EXPORT_CPPFLAGS := -std=c++11 -D_GLIBCXX_USE_CXX11_ABI=0 -static-libstdc++ 
LOCAL_EXPORT_LDFLAGS := -static-libstdc++ -L$(LOCAL_PATH)/libs/$(TARGET_ARCH_ABI)
include $(PREBUILT_SHARED_LIBRARY)

$(call import-module, android/support)
