 
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

# https://developer.android.com/studio/command-line/aapt2
# https://docs.oracle.com/javase/tutorial/deployment/jar/unpack.html

MY_DIR="$(readlink -f "$(dirname "$0")")"
cd ${MY_DIR}

rm -rf ${MY_DIR}/aapt2-4.1.1-6503028-linux.jar
if curl -L https://dl.google.com/android/maven2/com/android/tools/build/aapt2/4.1.1-6503028/aapt2-4.1.1-6503028-linux.jar -o ${MY_DIR}/aapt2-4.1.1-6503028-linux.jar; then
    echo "curl aapt2-4.1.1-6503028-linux.jar passed"
else
    echo "curl aapt2-4.1.1-6503028-linux.jar failed"
    exit 1
fi

rm -rf ${MY_DIR}/META-INF
rm -rf ${MY_DIR}/aapt2
rm -rf ${MY_DIR}/NOTICE
if jar xf ${MY_DIR}/aapt2-4.1.1-6503028-linux.jar; then 
    echo "extract aapt2-4.1.1-6503028-linux.jar passed"
else
    echo "extract aapt2-4.1.1-6503028-linux.jar failed"
    exit 1
fi
rm -rf ${MY_DIR}/aapt2-4.1.1-6503028-linux.jar
rm -rf ${MY_DIR}/META-INF
rm -rf ${MY_DIR}/NOTICE

if test -e ${MY_DIR}/aapt2; then
    echo "${MY_DIR}/aapt2 exists"
else
    echo "${MY_DIR}/aapt2 doesn't exist"
    exit 1
fi



