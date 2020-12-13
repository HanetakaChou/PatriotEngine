# https://developer.android.com/ndk/guides/android_mk

LOCAL_PATH:= $(call my-dir)

# PTMcRT-TreeSum.bundle

include $(CLEAR_VARS)

LOCAL_MODULE := PTMcRT-TreeSum.bundle

LOCAL_SRC_FILES:= $(LOCAL_PATH)/main.cpp
	
LOCAL_CFLAGS += -fdiagnostics-format=msvc
LOCAL_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_CFLAGS += -fvisibility=hidden

LOCAL_CPPFLAGS += -std=c++11

LOCAL_LDFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_LDFLAGS += -Wl,--enable-new-dtags # the linker can't recognize the old dtags
LOCAL_LDFLAGS += -Wl,-rpath,/XXXXXX # chrpath can only make path shorter

LOCAL_SHARED_LIBRARIES := libPTMcRT

include $(BUILD_EXECUTABLE)

# libPTMcRT

include $(CLEAR_VARS)

LOCAL_MODULE := libPTMcRT

ifeq (true,$(APP_DEBUG))
ifeq (x86_64,$(TARGET_ARCH))
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../Binary/PosixLinuxX11/x64/Debug/libPTMcRT.so
endif
endif

ifeq (true,$(APP_DEBUG))
ifeq (x86,$(TARGET_ARCH))
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../Binary/PosixLinuxX11/x86/Debug/libPTMcRT.so
endif
endif

ifneq (true,$(APP_DEBUG))
ifeq (x86_64,$(TARGET_ARCH))
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../Binary/PosixLinuxX11/x64/Release/libPTMcRT.so
endif
endif

ifneq (true,$(APP_DEBUG))
ifeq (x86,$(TARGET_ARCH))
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../Binary/PosixLinuxX11/x86/Release/libPTMcRT.so
endif
endif

include $(PREBUILT_SHARED_LIBRARY) # ndk-build will strip the so
