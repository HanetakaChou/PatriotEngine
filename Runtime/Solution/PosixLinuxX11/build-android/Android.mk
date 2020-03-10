LOCAL_PATH:= $(call my-dir)


include $(CLEAR_VARS)

LOCAL_LDFLAGS += -Wl,-dynamic-linker,linker64

LOCAL_LDFLAGS += -Wl,--enable-new-dtags
LOCAL_LDFLAGS += -Wl,-rpath,/XXXXXX


LOCAL_SRC_FILES:= main.cpp

LOCAL_MODULE := a.out

include $(BUILD_EXECUTABLE)

