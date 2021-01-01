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
LOCAL_MODULE := libpt_llvm_cxx_static
LOCAL_SRC_FILES := $(LOCAL_PATH)/pt_llvm_cxx_fake.cpp
LOCAL_EXPORT_CPPFLAGS := -std=c++11 -rtlib=compiler-rt -unwindlib=libunwind -stdlib=libc++ -static-libgcc -static-libstdc++ #-D_GLIBCXX_USE_CXX11_ABI=1 
LOCAL_EXPORT_LDFLAGS := -rtlib=compiler-rt -unwindlib=libunwind -stdlib=libc++ -lunwind -static-libgcc -static-libstdc++ 
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libpt_llvm_cxx_shared
LOCAL_SRC_FILES := $(LOCAL_PATH)/pt_llvm_cxx_fake.cpp
LOCAL_EXPORT_CPPFLAGS := -rtlib=compiler-rt -std=c++11 -unwindlib=libunwind -stdlib=libc++ #-static-libgcc -static-libstdc++ #-D_GLIBCXX_USE_CXX11_ABI=1  
LOCAL_EXPORT_LDFLAGS := -rtlib=compiler-rt -unwindlib=libunwind -stdlib=libc++ #-static-libgcc -static-libstdc++ 
include $(BUILD_STATIC_LIBRARY)

