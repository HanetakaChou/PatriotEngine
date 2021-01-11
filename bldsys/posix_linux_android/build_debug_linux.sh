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

# ndk-build

if "${MY_DIR}/android-ndk-r14b/ndk-build" APP_DEBUG:=true NDK_PROJECT_PATH:=null NDK_OUT:=obj/debug NDK_LIBS_OUT:=libs/debug/lib NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=build.mk; then
    echo "ndk-build passed"
else
    echo "ndk-build failed"
    exit 1
fi

# Packaging

mkdir -p "${MY_DIR}/bin"

# help vscode find symbol
rm -rf "${MY_DIR}/libs/debug"
mkdir -p "${MY_DIR}/libs/debug"
cp -rf "${MY_DIR}/obj/debug/local" "${MY_DIR}/libs/debug/lib"

"${MY_DIR}/android-sdk/build-tools/29.0.3/aapt" package -f --debug-mode -0 apk -M "${MY_DIR}/AndroidManifest.xml"  -S "${MY_DIR}/res" -I "${MY_DIR}/android-sdk/platforms/android-24/android.jar" -F "${MY_DIR}/bin/Android.Packaging-debug-unaligned.apk" "${MY_DIR}/libs/debug"    

"${MY_DIR}/android-sdk/build-tools/29.0.3/zipalign" -f 4 "${MY_DIR}/bin/Android.Packaging-debug-unaligned.apk" "${MY_DIR}/bin/Android.Packaging-debug.apk"

# https://docs.microsoft.com/en-us/xamarin/android/deploy-test/signing/manually-signing-the-apk#sign-the-apk
"${MY_DIR}/android-sdk/build-tools/29.0.3/apksigner" sign -v --ks "${MY_DIR}/debug.keystore" --ks-pass pass:android --ks-key-alias androiddebugkey "${MY_DIR}/bin/Android.Packaging-debug.apk"