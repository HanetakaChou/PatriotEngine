#!/bin/bash  

#
# Copyright (C) YuqiaoZhang(HanetakaChou)
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
    echo "Usage: build.sh architecture" 
    echo ""
    echo "Architectures:"
    echo "  x64 - build with the x86 architecture"
    echo "  x86 - build with the x64 architecture"
    echo ""
    exit 1
fi

if test \( \( -n "$1" \) -a \( "$1" = "x64" \) \);then
    wget -qO- https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo tee /etc/apt/trusted.gpg.d/lunarg.asc
    wget -qO /etc/apt/sources.list.d/lunarg-vulkan-jammy.list http://packages.lunarg.com/vulkan/lunarg-vulkan-jammy.list
    apt update

    apt install vulkan-sdk
    apt install glslang-tools
    apt install libvulkan-dev

    apt install libxcb1-dev
    apt install libx11-dev

    apt install clang
elif test \( \( -n "$1" \) -a \( "$1" = "x86" \) \);then
    dpkg --add-architecture i386
    apt-get update

    # bug ?
    apt install libc6-dev:i386
    apt install libgcc-11-dev:i386 # libc++ links libgcc_s # -nostdlib -nodefaultlibs 
    apt install libstdc++-11-dev:i386
    apt install libxcb1-dev:i386
    apt install libx11-dev:i386
    apt install libvulkan1:i386
    apt install mesa-vulkan-drivers:i386
    apt install libvulkan-dev:i386

    apt install glslang-tools
else
    echo "The arch \"$1\" is not supported!"
    echo ""
    echo "Usage: build.sh architecture" 
    echo ""
    echo "Architectures:"
    echo "  x64 - build with the x86 architecture"
    echo "  x86 - build with the x64 architecture"
    echo ""
    exit 1
fi



