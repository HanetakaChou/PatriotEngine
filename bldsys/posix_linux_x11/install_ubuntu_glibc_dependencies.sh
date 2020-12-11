#!/bin/bash  

#
# Copyright (C) YuqiaoZhang
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
    echo "Usage: build.sh platform"
    echo ""
    echo "Platforms:"
    echo "  x86     -   build with the x86 configuration"
    echo "  x64     -   build with the x64 configuration"
    echo ""
    exit 1
fi

if test ! \( \( \( -n "$1" \) -a \( "$1" = "x86" \) \) -o \( \( -n "$1" \) -a \( "$1" = "x64" \) \) \);
then
    echo "The platform \"$1\" is not supported!"
    echo ""
    echo "Platforms:"
    echo "  x86     -   build with the x86 configuration"
    echo "  x64     -   build with the x64 configuration"
    echo ""
    exit 1
fi



if test \( \( -n "$1" \) -a \( "$1" = "x86" \) \);
then
    apt install gcc
    apt install g++
    apt install clang
    apt install gdbserver
    apt install chrpath

    dpkg --add-architecture i386
    apt-get update

    apt install libc6-dev:i386
    apt install libstdc++-5-dev:i386
    apt install libxcb1-dev:i386
    apt install libx11-dev:i386
elif test \( \( -n "$1" \) -a \( "$1" = "x64" \) \);
then
    apt install libc6-dev
    apt install libstdc++-5-dev
    apt install libxcb1-dev
    apt install libx11-dev
    
    apt install gcc
    apt install g++
    apt install clang
    apt install gdbserver
    apt install chrpath
fi



