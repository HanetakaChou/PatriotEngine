#!/bin/bash  

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

# 18.04 LTS
# 16.04 LTS
# cat /etc/os-release 

if test \( $# -ne 1 \);
then
    echo "Usage: build.sh arch"
    echo ""
    echo "Archs:"
    echo "  x86     -   build with the x86 arch"
    echo "  x64     -   build with the x64 arch"
    echo ""
    exit 1
fi

if test \( \( -n "$1" \) -a \( "$1" = "x86" \) \);then
    yum install glibc-devel.i686
    yum install libstdc++-devel.i686
    yum install libstdc++-static.i686
    yum install libxcb-devel.i686
    yum install vulkan-loader.i686
    yum install mesa-vulkan-drivers.i686

    yum install gcc  
    yum install gcc-c++
    yum install clang
    yum install chrpath
elif test \( \( -n "$1" \) -a \( "$1" = "x64" \) \);then
    yum install glibc-devel
    yum install libstdc++-devel
    yum install libstdc++-static
    # yum install compiler-rt 
    # yum install libunwind-devel
    # yum install libcxxabi https://kojipkgs.fedoraproject.org//packages/libcxxabi/10.0.1/1.fc32/x86_64/libcxxabi-10.0.1-1.fc32.x86_64.rpm
    # yum install libcxxabi-static https://kojipkgs.fedoraproject.org//packages/libcxxabi/10.0.1/1.fc32/x86_64/libcxxabi-static-10.0.1-1.fc32.x86_64.rpm
    # yum install libcxxabi-devel https://kojipkgs.fedoraproject.org//packages/libcxxabi/10.0.1/1.fc32/x86_64/libcxxabi-devel-10.0.1-1.fc32.x86_64.rpm
    # yum install libcxx https://kojipkgs.fedoraproject.org//packages/libcxx/10.0.1/1.fc32/x86_64/libcxx-10.0.1-1.fc32.x86_64.rpm
    # yum install libcxx-static https://kojipkgs.fedoraproject.org//packages/libcxx/10.0.1/1.fc32/x86_64/libcxx-static-10.0.1-1.fc32.x86_64.rpm
    # yum install libcxx-devel https://kojipkgs.fedoraproject.org//packages/libcxx/10.0.1/1.fc32/x86_64/libcxx-devel-10.0.1-1.fc32.x86_64.rpm
    yum install libxcb-devel
    yum install vulkan-loader
    yum install mesa-vulkan-drivers
    yum install vulkan-validation-layers
    yum install vulkan-loader-devel

    yum install gcc  
    yum install gcc-c++
    yum install clang
    yum install chrpath
else
    echo "The arch \"$1\" is not supported!"
    echo ""
    echo "Archs:"
    echo "  x86     -   build with the x86 arch"
    echo "  x64     -   build with the x64 arch"
    echo ""
    exit 1
fi
