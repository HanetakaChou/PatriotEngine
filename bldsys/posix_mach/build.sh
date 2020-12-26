#!/bin/zsh

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

# configure
if test \( $# -ne 2 \);
then
    echo "Usage: build.sh config platform"
    echo ""
    echo "Configs:"
    echo "  debug   -   build with the debug configuration"
    echo "  release -   build with the release configuration"
    echo ""
    echo "Platforms:"
    echo "  osx     -   build with the osx platform"
    echo "  ios     -   build with the ios platform"
    echo ""
    exit 1
fi

if test \( \( -n "$1" \) -a \( "$1" = "debug" \) \);then 
    XCODE_BUILD_ARG_CONFIG="Debug"
elif test \( \( -n "$1" \) -a \( "$1" = "release" \) \);then
    XCODE_BUILD_ARG_CONFIG="Release"
else
    echo "The config \"$1\" is not supported!"
    echo ""
    echo "Configs:"
    echo "  debug   -   build with the debug configuration"
    echo "  release -   build with the release configuration"
    echo ""
    exit 1
fi

if test \( \( -n "$2" \) -a \( "$2" = "osx" \) \);then
    XCODE_BUILD_ARG_PLATFORM="MacOSX"
elif test \( \( -n "$2" \) -a \( "$2" = "ios" \) \);then
    XCODE_BUILD_ARG_PLATFORM="iPhoneOS"
else
    echo "The platform \"$2\" is not supported!"
    echo ""
    echo "Platforms:"
    echo "  osx     -   build with the osx platform"
    echo "  ios     -   build with the ios platform"
    echo ""
    exit 1
fi

# Arch
# Mach-O support universal binary

MY_DIR="$(dirname "$0")"

# build by xcode  
cd ${MY_DIR}

# https://developer.apple.com/library/archive/technotes/tn2339/_index.html
# xcode-select -switch /Applications/Xcode.app/Contents/Developer
if xcodebuild -scheme "${XCODE_BUILD_ARG_PLATFORM}_${XCODE_BUILD_ARG_CONFIG}" build; then  
    echo "xcodebuild passed"
else
    echo "xcodebuild failed"
    exit 1
fi

