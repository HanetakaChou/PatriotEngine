# https://developer.android.com/ndk/guides/android_mk

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libPTMcRT

LOCAL_SRC_FILES:= \
	$(LOCAL_PATH)/../../Private/McRT/PTSMemory.cpp \
	$(LOCAL_PATH)/../../Private/McRT/PTSMemoryAllocator.cpp \
	$(LOCAL_PATH)/../../Private/McRT/PTSTaskSchedulerImpl.cpp \
	
LOCAL_CFLAGS += -fdiagnostics-format=msvc
LOCAL_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -DPTMCRTAPI=PTEXPORT
	
LOCAL_CPPFLAGS += -std=c++11

ifeq (x86,$(TARGET_ARCH))
LOCAL_CFLAGS += -mssse3
endif

LOCAL_LDFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_LDFLAGS += -Wl,--enable-new-dtags # the linker can't recognize the old dtags
LOCAL_LDFLAGS += -Wl,-rpath,/XXXXXX # chrpath can only make path shorter

include $(BUILD_SHARED_LIBRARY)

