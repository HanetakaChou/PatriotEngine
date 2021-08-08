#
# Copyright (C) YuqiaoZhang(HanetakaYuminaga)
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := ndk_build_glibc_gnu_stdcxx_static
LOCAL_EXPORT_CPPFLAGS += -D_GLIBCXX_USE_CXX11_ABI=1 -static-libgcc -static-libstdc++ 
ifeq ($(NDK_TOOLCHAIN_VERSION),clang) # bug for Ubuntu clang?
LOCAL_EXPORT_CPPFLAGS += -I/usr/include/c++/9 
LOCAL_EXPORT_CPPFLAGS += -I/usr/include/x86_64-linux-gnu/c++/9
endif
LOCAL_EXPORT_LDFLAGS += -static-libgcc -static-libstdc++
ifeq ($(NDK_TOOLCHAIN_VERSION),clang)  # bug for Ubuntu clang?
ifeq (x86_64,$(TARGET_ARCH))
LOCAL_EXPORT_LDFLAGS += -L/usr/lib/gcc/x86_64-linux-gnu/9
endif
ifeq (x86,$(TARGET_ARCH))
LOCAL_EXPORT_LDFLAGS += -L/usr/lib/gcc/i686-linux-gnu//9
endif
endif
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := ndk_build_glibc_gnu_stdcxx_shared
LOCAL_EXPORT_CPPFLAGS := -D_GLIBCXX_USE_CXX11_ABI=1 #-static-libgcc -static-libstdc++ 
ifeq ($(NDK_TOOLCHAIN_VERSION),clang) # bug for Ubuntu clang?
LOCAL_EXPORT_CPPFLAGS += -I/usr/include/c++/9
LOCAL_EXPORT_CPPFLAGS += -I/usr/include/x86_64-linux-gnu/c++/9
endif
#LOCAL_EXPORT_LDFLAGS := #-static-libgcc -static-libstdc++
ifeq ($(NDK_TOOLCHAIN_VERSION),clang)  # bug for Ubuntu clang?
ifeq (x86_64,$(TARGET_ARCH))
LOCAL_EXPORT_LDFLAGS += -L/usr/lib/gcc/x86_64-linux-gnu/9
endif
ifeq (x86,$(TARGET_ARCH))
LOCAL_EXPORT_LDFLAGS += -L/usr/lib/gcc/i686-linux-gnu//9
endif
endif
include $(BUILD_STATIC_LIBRARY)

