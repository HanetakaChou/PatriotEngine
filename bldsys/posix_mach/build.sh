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

# Sign Xcode applications
# https://docs.github.com/en/actions/guides/installing-an-apple-certificate-on-macos-runners-for-xcode-development

# configure
if test \( $# -ne 2 \);
then
    echo "Usage: build.sh config platform"
    echo ""
    echo "Configs:"
    echo "  debug       -   build with the debug configuration"
    echo "  release     -   build with the release configuration"
    echo ""
    echo "Platforms:"
    echo "  osx         -   build with the osx platform"
    echo "  ios         -   build with the ios platform"
    echo "  catalyst    -   build with the catalyst platform"
    echo ""
    exit 1
fi

if test \( \( -n "$1" \) -a \( "$1" = "debug" \) \);then 
    XCODE_BUILD_ARG_SCHEME_CONFIG="debug"
elif test \( \( -n "$1" \) -a \( "$1" = "release" \) \);then
    XCODE_BUILD_ARG_SCHEME_CONFIG="release"
else
    echo "The config \"$1\" is not supported!"
    echo ""
    echo "Configs:"
    echo "  debug       -   build with the debug configuration"
    echo "  release     -   build with the release configuration"
    echo ""
    exit 1
fi

# XCODE_BUILD_ARG_DESTINNATION
# xcodebuild -showdestinations -scheme ios_debug/release

if test \( \( -n "$2" \) -a \( "$2" = "osx" \) \);then
    XCODE_BUILD_ARG_SCHEME_PLATFORM="osx"
    XCODE_BUILD_ARG_DESTINNATION="generic/platform=macOS"
elif test \( \( -n "$2" \) -a \( "$2" = "ios" \) \);then
    XCODE_BUILD_ARG_SCHEME_PLATFORM="ios"
    XCODE_BUILD_ARG_DESTINNATION="generic/platform=iOS"
elif test \( \( -n "$2" \) -a \( "$2" = "catalyst" \) \);then
    XCODE_BUILD_ARG_SCHEME_PLATFORM="ios"
    XCODE_BUILD_ARG_DESTINNATION="generic/platform=macOS,variant=Mac Catalyst"
else
    echo "The platform \"$2\" is not supported!"
    echo ""
    echo "Platforms:"
    echo "  osx         -   build with the osx platform"
    echo "  ios         -   build with the ios platform"
    echo "  catalyst    -   build with the catalyst platform"
    echo ""
    exit 1
fi

# Arch
# Mach-O support universal binary

MY_DIR="$(cd "$(dirname "$0")" 1>/dev/null 2>/dev/null && pwd)"  

# build by xcode  
cd ${MY_DIR}

# https://developer.apple.com/library/archive/technotes/tn2339/_index.html
# xcode-select -switch /Applications/Xcode.app/Contents/Developer
if xcodebuild -scheme "${XCODE_BUILD_ARG_SCHEME_PLATFORM}_${XCODE_BUILD_ARG_SCHEME_CONFIG}" -destination "${XCODE_BUILD_ARG_DESTINNATION}" build  -allowProvisioningUpdates; then  
    echo "xcodebuild passed"
else
    echo "xcodebuild failed"
    exit 1
fi

