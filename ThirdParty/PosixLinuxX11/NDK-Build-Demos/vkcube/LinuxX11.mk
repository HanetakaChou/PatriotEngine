# https://developer.android.com/ndk/guides/android_mk

LOCAL_PATH:= $(call my-dir)

# vkcube

include $(CLEAR_VARS)

LOCAL_MODULE := vkcube

LOCAL_SRC_FILES := \
	main.cpp \
	TextureLoader.cpp \
	DDS/TextureLoader_DDS.cpp \
	PVR/TextureLoader_PVR.cpp \
	VK/TextureLoader_VK.cpp \
	VK/StagingBuffer.cpp \
	asset.cpp

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../Bionic-Redistributable/include

LOCAL_LDFLAGS += -Wl,-dynamic-linker,linker ### put the linker at cwd

LOCAL_LDFLAGS += -Wl,--enable-new-dtags ### the linker can't recognize the old dtags
LOCAL_LDFLAGS += -Wl,-rpath,/XXXXXX ### chrpath can only make path shorter

LOCAL_LDFLAGS += -lvulkan

LOCAL_SHARED_LIBRARIES := libxcb

include $(BUILD_EXECUTABLE)

# libxcb

include $(CLEAR_VARS)

LOCAL_MODULE := libxcb

ifeq (x86_64,$(TARGET_ARCH))
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../Bionic-Redistributable/lib64/libxcb.so
endif

ifeq (x86,$(TARGET_ARCH))
LOCAL_SRC_FILES := $(LOCAL_PATH)/../../Bionic-Redistributable/lib/libxcb.so
endif

include $(PREBUILT_SHARED_LIBRARY) # ndk-build will strip the so