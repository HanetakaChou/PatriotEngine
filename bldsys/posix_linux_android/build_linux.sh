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

MY_DIR="$(readlink -f "$(dirname "$0")")"

cd ${MY_DIR}
${MY_DIR}/aapt2 package -f -M AndroidManifest.xml -I "$ANDROID_SDK_HOME/platforms/android-24/android.jar" -S res -F bin/$1-unaligned.apk bin/libs
# jarsigner -verbose -keystore ${MY_DIR}/.android/debug.keystore -storepass android -keypass android  bin/$1-unaligned.apk androiddebugkey
# zipalign -f 4 bin/$1-unaligned.apk bin/$1.apk