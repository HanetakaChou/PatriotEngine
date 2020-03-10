LOCAL_PATH:= $(call my-dir)


include $(CLEAR_VARS)

LOCAL_LDFLAGS += -Wl,-dynamic-linker,linker ### add the linker to PATH

LOCAL_LDFLAGS += -Wl,--enable-new-dtags ### linker can't process old dtags
LOCAL_LDFLAGS += -Wl,-rpath,/XXXXXX ### chrpath can only make path shorter


LOCAL_SRC_FILES:= main.cpp

LOCAL_MODULE := a.out

include $(BUILD_EXECUTABLE)

