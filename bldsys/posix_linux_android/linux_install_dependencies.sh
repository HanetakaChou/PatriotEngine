 
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

# apt install curl
# apt install unzip

MY_DIR="$(cd "$(dirname "$0")" 1>/dev/null 2>/dev/null && pwd)"  
cd ${MY_DIR}

rm -rf "${MY_DIR}/android-ndk-r14b-linux-x86_64.zip"
if curl -o "${MY_DIR}/android-ndk-r14b-linux-x86_64.zip" -L https://dl.google.com/android/repository/android-ndk-r14b-linux-x86_64.zip; then
    echo "curl android-ndk-r14b-linux-x86_64.zip passed"
else
    echo "curl android-ndk-r14b-linux-x86_64.zip failed"
    exit 1
fi

# https://developer.android.com/studio/releases/build-tools
rm -rf "${MY_DIR}/build-tools_r29.0.3-linux.zip"
if curl -o "${MY_DIR}/build-tools_r29.0.3-linux.zip" -L https://dl.google.com/android/repository/build-tools_r29.0.3-linux.zip; then
    echo "curl build-tools_r29.0.3-linux.zip passed"
else
    echo "curl build-tools_r29.0.3-linux.zip failed"
    exit 1
fi

rm -rf  "${MY_DIR}/platform-24_r02.zip"
if curl -o "${MY_DIR}/platform-24_r02.zip" -L https://dl.google.com/android/repository/platform-24_r02.zip; then
    echo "curl platform-24_r02.zip passed"
else
    echo "curl platform-24_r02.zip failed"
    exit 1
fi

# https://developer.android.com/studio/releases/platform-tools
rm -rf "${MY_DIR}/platform-tools_r29.0.6-linux.zip"
if curl -o "${MY_DIR}/platform-tools_r29.0.6-linux.zip" -L https://dl.google.com/android/repository/platform-tools_r29.0.6-linux.zip; then
    echo "curl platform-tools_r29.0.6-linux.zip passed"
else
    echo "curl platform-tools_r29.0.6-linux.zip failed"
    exit 1
fi

rm -rf "${MY_DIR}/android-ndk-r14b"
if unzip "${MY_DIR}/android-ndk-r14b-linux-x86_64.zip"; then 
    echo "unzip android-ndk-r14b-linux-x86_64.zip passed"
else
    echo "unzip android-ndk-r14b-linux-x86_64.zip failed"
    exit 1
fi
rm -rf "${MY_DIR}/android-ndk-r14b-linux-x86_64.zip"

rm -rf  "${MY_DIR}/android-10"
if unzip "${MY_DIR}/build-tools_r29.0.3-linux.zip"; then 
    echo "unzip build-tools_r29.0.3-linux.zip passed"
else
    echo "unzip build-tools_r29.0.3-linux.zip failed"
    exit 1
fi
rm -rf "${MY_DIR}/build-tools_r29.0.3-linux.zip"

rm -rf "${MY_DIR}/android-sdk/build-tools/29.0.3"
mkdir -p "${MY_DIR}/android-sdk/build-tools/"
if mv -f "${MY_DIR}/android-10" "${MY_DIR}/android-sdk/build-tools/29.0.3"; then 
    echo "mv build-tools_r29.0.3-linux.zip passed"
else
    echo "mv build-tools_r29.0.3-linux.zip failed"
    exit 1
fi

rm -rf  "${MY_DIR}/android-7.0"
if unzip "${MY_DIR}/platform-24_r02.zip"; then 
    echo "unzip platform-24_r02.zip passed"
else
    echo "unzip platform-24_r02.zip failed"
    exit 1
fi
rm -rf "${MY_DIR}/platform-24_r02.zip"

rm -rf "${MY_DIR}/android-sdk/platforms/android-24"
mkdir -p "${MY_DIR}/android-sdk/platforms/"
if mv -f "${MY_DIR}/android-7.0" "${MY_DIR}/android-sdk/platforms/android-24"; then 
    echo "mv platform-24_r02.zip passed"
else
    echo "mv platform-24_r02.zip failed"
    exit 1
fi

rm -rf  "${MY_DIR}/platform-tools"
if unzip "${MY_DIR}/platform-tools_r29.0.6-linux.zip"; then 
    echo "unzip platform-tools_r29.0.6-linux.zip passed"
else
    echo "unzip platform-tools_r29.0.6-linux.zip failed"
    exit 1
fi
rm -rf "${MY_DIR}/platform-tools_r29.0.6-linux.zip"

rm -rf "${MY_DIR}/android-sdk/platform-tools"
mkdir -p "${MY_DIR}/android-sdk/"
if mv -f "${MY_DIR}/platform-tools" "${MY_DIR}/android-sdk/platform-tools"; then 
    echo "mv platform-tools_r29.0.6-linux.zip passed"
else
    echo "mv platform-tools_r29.0.6-linux.zip failed"
    exit 1
fi