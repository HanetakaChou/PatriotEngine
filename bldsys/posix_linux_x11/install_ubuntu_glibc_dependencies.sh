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
    # apt install gcc
    # apt install g++
    # apt install clang-3.8
    # apt install clang-6.0
    apt install clang
    apt install chrpath

    dpkg --add-architecture i386
    apt-get update

    apt install libc6-dev:i386
    apt install libgcc-5-dev:i386 # libc++ links libgcc_s # -nostdlib -nodefaultlibs 
    # apt install libstdc++-5-dev:i386
    # apt install libclang-common-3.8-dev:i386
    # apt install libclang-common-6.0-dev:i386
    apt install liblzma-dev:i386
    apt install libunwind8:i386
    apt install libunwind-dev:i386
    apt install libc++abi1:i386
    apt install libc++abi-dev:i386
    apt install libc++1:i386
    apt install libc++-dev:i386
    apt install libxcb1-dev:i386
    apt install libx11-dev:i386
    apt install libvulkan1:i386
    apt install mesa-vulkan-drivers:i386
    apt install libvulkan-dev:i386

elif test \( \( -n "$1" \) -a \( "$1" = "x64" \) \);then
    apt install libc6-dev
    apt install libgcc-5-dev # libc++ links libgcc_s # -nostdlib -nodefaultlibs 
    # apt install libstdc++-5-dev
    # apt install libclang-common-3.8-dev
    # apt install libclang-common-6.0-dev
    apt install liblzma-dev
    apt install libunwind8
    apt install libunwind-dev
    apt install libc++abi1
    apt install libc++abi-dev
    apt install libc++1
    apt install libc++-dev
    apt install libxcb1-dev
    apt install libx11-dev
    apt install libvulkan1
    apt install mesa-vulkan-drivers
    # apt install vulkan-validationlayers
    apt install libvulkan-dev

    # apt install gcc
    # apt install g++
    # apt install clang-3.8
    # apt install clang-6.0
    apt install clang
    apt install chrpath
else
    echo "The arch \"$1\" is not supported!"
    echo ""
    echo "Archs:"
    echo "  x86     -   build with the x86 arch"
    echo "  x64     -   build with the x64 arch"
    echo ""
    exit 1
fi



