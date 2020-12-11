# This file is dual licensed under the MIT and the University of Illinois Open
# Source Licenses. See LICENSE.TXT for details.

LOCAL_PATH := $(call my-dir)

$(call ndk_log,Using prebuilt libc++ libraries)

include $(CLEAR_VARS)
LOCAL_MODULE := c++_static
LOCAL_SRC_FILES := $(LOCAL_PATH)/libs/$(TARGET_ARCH_ABI)/libgcc_s$(TARGET_SONAME_EXTENSION)
LOCAL_EXPORT_CPPFLAGS := -std=c++11 -D_GLIBCXX_USE_CXX11_ABI=0 -static-libgcc -static-libstdc++ #-rtlib=compiler-rt -unwindlib=libunwind
LOCAL_EXPORT_LDFLAGS := -static-libgcc -static-libstdc++ #-rtlib=compiler-rt -unwindlib=libunwind
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := c++_shared
LOCAL_SRC_FILES := $(LOCAL_PATH)/libs/$(TARGET_ARCH_ABI)/libgcc_s$(TARGET_SONAME_EXTENSION)
LOCAL_EXPORT_CPPFLAGS := -std=c++11 -D_GLIBCXX_USE_CXX11_ABI=0 -static-libgcc -static-libstdc++ #-rtlib=compiler-rt -unwindlib=libunwind 
LOCAL_EXPORT_LDFLAGS := -static-libgcc -static-libstdc++ #-rtlib=compiler-rt -unwindlib=libunwind
include $(PREBUILT_SHARED_LIBRARY)

