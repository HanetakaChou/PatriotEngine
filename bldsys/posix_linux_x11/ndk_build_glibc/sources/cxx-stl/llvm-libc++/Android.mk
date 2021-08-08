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
LOCAL_MODULE := ndk_build_glibc_llvm_cxx_static
LOCAL_EXPORT_CPPFLAGS += -rtlib=compiler-rt -unwindlib=libunwind -stdlib=libc++ -static-libstdc++ #-static-libgcc #-rtlib=compiler-rt -unwindlib=libunwind #-nostdlib -nodefaultlibs #-D_GLIBCXX_USE_CXX11_ABI=1 
ifeq ($(NDK_TOOLCHAIN_VERSION),clang)
LOCAL_EXPORT_CPPFLAGS += -I/usr/lib/llvm-10/include/c++/v1 # bug of the Ubuntu clang
LOCAL_EXPORT_CPPFLAGS += -I/usr/lib/llvm-11/include/c++/v1 # bug of the Debian clang
endif
LOCAL_EXPORT_LDFLAGS += -rtlib=compiler-rt -unwindlib=libunwind -stdlib=libc++ -static-libstdc++ #-static-libgcc  #-rtlib=compiler-rt -unwindlib=libunwind #-Wl,-Bstatic -lc++ -lunwind -llzma -Wl,-Bdynamic #-nostdlib -nodefaultlibs
ifeq ($(NDK_TOOLCHAIN_VERSION),clang)
ifeq (x86,$(TARGET_ARCH))
LOCAL_EXPORT_LDFLAGS += -L/usr/lib/llvm-10/lib # bug of the Ubuntu clang
LOCAL_EXPORT_LDFLAGS += -L/usr/lib/llvm-11/lib # bug of the Debian clang
LOCAL_EXPORT_LDFLAGS += -lc++abi
#LOCAL_EXPORT_LDFLAGS += -Wl,-Bstatic -lc++abi -Wl,-Bdynamic 
#LOCAL_EXPORT_LDFLAGS += -lunwind
#LOCAL_EXPORT_LDFLAGS += -llzma
endif
endif
include $(BUILD_STATIC_LIBRARY)

# TODO
# enable LOCAL_DISABLE_FATAL_LINKER_WARNINGS in build-binary.mk
# x86/Gos-linux.c:299: warning: sigreturn is not implemented and will always fail

include $(CLEAR_VARS)
LOCAL_MODULE := ndk_build_glibc_llvm_cxx_shared
LOCAL_EXPORT_CPPFLAGS += -rtlib=compiler-rt -unwindlib=libunwind -stdlib=libc++
ifeq ($(NDK_TOOLCHAIN_VERSION),clang) 
LOCAL_EXPORT_CPPFLAGS += -I/usr/lib/llvm-10/include/c++/v1 # bug of the Ubuntu clang
LOCAL_EXPORT_CPPFLAGS += -I/usr/lib/llvm-11/include/c++/v1 # bug of the Debian clang
endif
LOCAL_EXPORT_LDFLAGS += -rtlib=compiler-rt -unwindlib=libunwind -stdlib=libc++ #-lc++ -lunwind #-nostdlib -nodefaultlibs #-Wl,-Bstatic -lc++ -lunwind -llzma -Wl,-Bdynamic #-static-libgcc -static-libstdc++
ifeq ($(NDK_TOOLCHAIN_VERSION),clang) 
ifeq (x86,$(TARGET_ARCH))
LOCAL_EXPORT_LDFLAGS += -L/usr/lib/llvm-10/lib # bug of the Ubuntu clang
LOCAL_EXPORT_LDFLAGS += -L/usr/lib/llvm-11/lib # bug of the Debian clang
endif
endif
include $(BUILD_STATIC_LIBRARY)
