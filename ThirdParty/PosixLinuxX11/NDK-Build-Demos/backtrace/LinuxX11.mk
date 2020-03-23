# https://developer.android.com/ndk/guides/android_mk

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main.bundle

LOCAL_SRC_FILES:= main.cpp

LOCAL_LDFLAGS += -Wl,-dynamic-linker,linker ### put the linker at cwd

LOCAL_LDFLAGS += -Wl,--enable-new-dtags ### the linker can't recognize the old dtags
LOCAL_LDFLAGS += -Wl,-rpath,/XXXXXX ### chrpath can only make path shorter

LOCAL_SHARED_LIBRARIES := libmain2

include $(BUILD_EXECUTABLE)

# libmain2

include $(CLEAR_VARS)

LOCAL_MODULE := libmain2

LOCAL_SRC_FILES:= main2.cpp

LOCAL_LDFLAGS += -Wl,-dynamic-linker,linker ### put the linker at cwd

LOCAL_LDFLAGS += -Wl,--enable-new-dtags ### the linker can't recognize the old dtags
LOCAL_LDFLAGS += -Wl,-rpath,/XXXXXX ### chrpath can only make path shorter

LOCAL_SHARED_LIBRARIES := libc_malloc_debug_backtrace

include $(BUILD_SHARED_LIBRARY)

# libc_malloc_debug_backtrace

include $(CLEAR_VARS)

LOCAL_MODULE := libc_malloc_debug_backtrace

LOCAL_SRC_FILES:= \
	$(LOCAL_PATH)/libc_malloc_debug_backtrace/backtrace.cpp \
	$(LOCAL_PATH)/libc_malloc_debug_backtrace/MapData.cpp \

LOCAL_LDFLAGS += -Wl,--enable-new-dtags ### the linker can't recognize the old dtags
LOCAL_LDFLAGS += -Wl,-rpath,/XXXXXX ### chrpath can only make path shorter

include $(BUILD_SHARED_LIBRARY) # ndk-build will strip the so