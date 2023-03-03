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

# configure
if test \( $# -ne 2 \);
then
    echo "Usage: pt_build.sh configuration architecture"
    echo ""
    echo "Configurations:"
    echo " debug - build with the debug configuration"
    echo " release - build with the release configuration"
    echo ""
    echo "Architectures:"
    echo " x86 - build with the x86 architecture"
    echo " x64 - build with the x64 architecture"
    echo ""
    exit 1
fi

# build
BUILD_DIR="$(realpath -s "$(dirname "$0")")"  

make -C "${BUILD_DIR}" -f "${BUILD_DIR}/pt_mcrt.mk" "BUILD_CONFIGURATION:="$1"" "BUILD_ARCHITECTURE:="$2""
