# This file is dual licensed under the MIT and the University of Illinois Open
# Source Licenses. See LICENSE.TXT for details.

LOCAL_PATH := $(call my-dir)

$(call ndk_log,Using prebuilt libc++ libraries)

include $(CLEAR_VARS)
LOCAL_MODULE := c++_static
LOCAL_SRC_FILES := $(LOCAL_PATH)/libs/$(TARGET_ARCH_ABI)/libstdc++$(TARGET_LIB_EXTENSION)
#LOCAL_EXPORT_C_INCLUDES :=  
LOCAL_EXPORT_CFLAGS := -fPIC -fPIE -pthread  
LOCAL_EXPORT_CPPFLAGS := -std=c++11 -static-libstdc++ -D_GLIBCXX_USE_CXX11_ABI=0
LOCAL_EXPORT_LDFLAGS := -pthread -static-libstdc++ -L$(LOCAL_PATH)/libs/$(TARGET_ARCH_ABI)
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := c++_shared
LOCAL_SRC_FILES := $(LOCAL_PATH)/libs/$(TARGET_ARCH_ABI)/libstdc++$(TARGET_LIB_EXTENSION)
#LOCAL_EXPORT_C_INCLUDES :=  
LOCAL_EXPORT_CFLAGS := -fPIC -fPIE -pthread 
LOCAL_EXPORT_CPPFLAGS := -std=c++11 -static-libstdc++ -D_GLIBCXX_USE_CXX11_ABI=0
LOCAL_EXPORT_LDFLAGS := -pthread -static-libstdc++ -L$(LOCAL_PATH)/libs/$(TARGET_ARCH_ABI)
include $(PREBUILT_STATIC_LIBRARY)

$(call import-module, android/support)
