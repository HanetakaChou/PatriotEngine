LOCAL_PATH := $(call my-dir)

LIBCXX_LIBS := $(LOCAL_PATH)/../../cxx-stl/llvm-libc++/libs/$(TARGET_ARCH_ABI)

include $(CLEAR_VARS)
LOCAL_MODULE := android_support
LOCAL_SRC_FILES := $(LIBCXX_LIBS)/libm$(TARGET_SONAME_EXTENSION)
include $(PREBUILT_SHARED_LIBRARY)


