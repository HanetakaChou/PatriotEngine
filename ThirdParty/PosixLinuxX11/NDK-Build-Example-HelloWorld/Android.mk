# https://developer.android.com/ndk/guides/android_mk

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := a.out

LOCAL_SRC_FILES:= main.cpp

LOCAL_LDFLAGS += -Wl,-dynamic-linker,linker ### put the linker at cwd

LOCAL_LDFLAGS += -Wl,--enable-new-dtags ### the linker can't recognize the old dtags
LOCAL_LDFLAGS += -Wl,-rpath,/XXXXXX ### chrpath can only make path shorter

include $(BUILD_EXECUTABLE)

