#
# Copyright (C) YuqiaoZhang(HanetakaCho)
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
LOCAL_MODULE := libpt_gnu_stdcxx_static
LOCAL_SRC_FILES := $(LOCAL_PATH)/pt_gnu_stdcxx_fake.cpp
LOCAL_EXPORT_CPPFLAGS := -std=c++11 -D_GLIBCXX_USE_CXX11_ABI=1 -static-libgcc -static-libstdc++ 
LOCAL_EXPORT_LDFLAGS := -static-libgcc -static-libstdc++
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libpt_gnu_stdcxx_shared
LOCAL_SRC_FILES := $(LOCAL_PATH)/pt_gnu_stdcxx_fake.cpp
LOCAL_EXPORT_CPPFLAGS := -std=c++11 -D_GLIBCXX_USE_CXX11_ABI=1 -static-libgcc -static-libstdc++ 
LOCAL_EXPORT_LDFLAGS := -static-libgcc -static-libstdc++
include $(BUILD_STATIC_LIBRARY)

