# https://developer.android.com/ndk/guides/android_mk

LOCAL_PATH:= $(call my-dir)

PT_SRC_ROOT_DIR:= $(abspath $(LOCAL_PATH)/../../src/)

# libPTMcRT

include $(CLEAR_VARS)

LOCAL_MODULE := libpt_mcrt

LOCAL_SRC_FILES:= \
	$(PT_SRC_ROOT_DIR)/pt_mcrt_malloc.cpp \
	
LOCAL_CFLAGS += -fdiagnostics-format=msvc
LOCAL_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -DPT_MCRT_ATTR=PT_EXPORT
	
LOCAL_CPPFLAGS += -std=c++11

ifeq (x86,$(TARGET_ARCH))
LOCAL_CFLAGS += -mssse3
endif

LOCAL_LDFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_LDFLAGS += -Wl,--enable-new-dtags # the linker can't recognize the old dtags
LOCAL_LDFLAGS += -Wl,-rpath,/XXXXXX # chrpath can only make path shorter # fix me: define the $ORIGIN correctly in the Linux_X11.mk

include $(BUILD_SHARED_LIBRARY)

