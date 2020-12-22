 
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

# apt install curl
# apt install unzip

MY_DIR="$(readlink -f "$(dirname "$0")")"
cd ${MY_DIR}

rm -rf ${MY_DIR}/android-ndk-r14b-linux-x86_64.zip
if curl -L https://dl.google.com/android/repository/android-ndk-r14b-linux-x86_64.zip -o ${MY_DIR}/android-ndk-r14b-linux-x86_64.zip; then
    echo "curl android-ndk-r14b-linux-x86_64.zip passed"
else
    echo "curl android-ndk-r14b-linux-x86_64.zip failed"
    exit 1
fi

rm -rf ${MY_DIR}/android-ndk-r14b
if unzip ${MY_DIR}/android-ndk-r14b-linux-x86_64.zip; then 
    echo "unzip android-ndk-r14b-linux-x86_64.zip passed"
else
    echo "unzip android-ndk-r14b-linux-x86_64.zip failed"
    exit 1
fi
rm -rf android-ndk-r14b-linux-x86_64.zip

rm -rf ${MY_DIR}/Bionic-based-Linux-7.1.2-39.zip
if curl -L https://github.com/YuqiaoZhang/Bionic-based-Linux/archive/7.1.2-39.zip -o ${MY_DIR}/Bionic-based-Linux-7.1.2-39.zip; then
    echo "curl Bionic-based-Linux-7.1.2-39.zip passed"
else
    echo "curl Bionic-based-Linux-7.1.2-39.zip failed"
    exit 1
fi

rm -rf ${MY_DIR}/Bionic-based-Linux-7.1.2-39
if unzip ${MY_DIR}/Bionic-based-Linux-7.1.2-39.zip; then
    echo "unzip Bionic-based-Linux-7.1.2-39.zip passed"
else
    echo "unzip Bionic-based-Linux-7.1.2-39.zip failed"
    exit 1
fi
rm -rf ${MY_DIR}/Bionic-based-Linux-7.1.2-39.zip

rm -rf /system
mv ${MY_DIR}/Bionic-based-Linux-7.1.2-39/system /
rm -rf ${MY_DIR}/Bionic-based-Linux-7.1.2-39
