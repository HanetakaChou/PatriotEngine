# https://developer.android.com/ndk/guides/android_mk

LOCAL_PATH:= $(call my-dir)

# libPTMcRT

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


# PTLauncher.bundle

include $(CLEAR_VARS)

LOCAL_MODULE := PTLauncher.bundle

LOCAL_SRC_FILES:= \
	$(LOCAL_PATH)/../../Private/Launcher/PosixLinuxX11/PTWindowImpl.cpp

LOCAL_CFLAGS += -fdiagnostics-format=msvc
LOCAL_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -DPTWSIAPI=PTEXPORT

LOCAL_CPPFLAGS += -std=c++11

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../ThirdParty/PosixLinuxX11/Bionic-Redistributable/include/

LOCAL_LDFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_LDFLAGS += -Wl,--enable-new-dtags # the linker can't recognize the old dtags
LOCAL_LDFLAGS += -Wl,-rpath,/XXXXXX # chrpath can only make path shorter

LOCAL_SHARED_LIBRARIES := libxcb libPTApp

include $(BUILD_EXECUTABLE)

# libxcb

include $(CLEAR_VARS)

LOCAL_MODULE := libxcb

ifeq (x86_64,$(TARGET_ARCH))
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../ThirdParty/PosixLinuxX11/Bionic-Redistributable/lib64/libxcb.so
endif

ifeq (x86,$(TARGET_ARCH))
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../../ThirdParty/PosixLinuxX11/Bionic-Redistributable/lib/libxcb.so
endif

include $(PREBUILT_SHARED_LIBRARY) # ndk-build will strip the so

# libPTApp

include $(CLEAR_VARS)

LOCAL_MODULE := libPTApp

LOCAL_SRC_FILES:= \
	$(LOCAL_PATH)/../../Private/App/PTAExport.cpp
	
LOCAL_CFLAGS += -fdiagnostics-format=msvc
LOCAL_CFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_CFLAGS += -fvisibility=hidden
LOCAL_CFLAGS += -DPTAPPAPI=PTEXPORT
	
LOCAL_CPPFLAGS += -std=c++11

LOCAL_LDFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8
LOCAL_LDFLAGS += -Wl,--enable-new-dtags # the linker can't recognize the old dtags
LOCAL_LDFLAGS += -Wl,-rpath,/XXXXXX # chrpath can only make path shorter

include $(BUILD_SHARED_LIBRARY)