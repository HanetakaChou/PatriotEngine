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

# configure
if test \( $# -ne 2 \);
then
    echo "Usage: build.sh config arch"
    echo ""
    echo "Configs:"
    echo "  debug   -   test the debug configuration"
    echo "  release -   test the release configuration"
    echo ""
    echo "Archs:"
    echo "  x86     -   test the x86 arch"
    echo "  x64     -   test the x64 arch"
    echo ""
    exit 1
fi

if test \( \( -n "$1" \) -a \( "$1" = "debug" \) \);then 
    OUT_DIR_CONFIG="debug"
elif test \( \( -n "$1" \) -a \( "$1" = "release" \) \);then
    OUT_DIR_CONFIG="release"
else
    echo "The config \"$1\" is not supported!"
    echo ""
    echo "Configs:"
    echo "  debug   -   test the debug configuration"
    echo "  release -   test the release configuration"
    echo ""
    exit 1
fi

if test \( \( -n "$2" \) -a \( "$2" = "x86" \) \);then
    OUT_DIR_ARCH="x86"
elif test \( \( -n "$2" \) -a \( "$2" = "x64" \) \);then
    OUT_DIR_ARCH="x64"
else
    echo "The architecture \"$2\" is not supported!"
    echo ""
    echo "Archs:"
    echo "  x86     -   test the x86 arch"
    echo "  x64     -   test the x64 arch"
    echo ""
    exit 1
fi

MY_DIR="$(cd "$(dirname "$0")" 1>/dev/null 2>/dev/null && pwd)"  
OUT_DIR="${MY_DIR}/../../bin/posix_linux_x11/${OUT_DIR_ARCH}/${OUT_DIR_CONFIG}/"

# run tests 
if "${OUT_DIR}/pt_gfx_malloc_test"; then
    echo "pt_gfx_malloc_test passed"
else
    echo "pt_gfx_malloc_test failed"
    exit 1
fi
