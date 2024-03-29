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
if test \( $# -ne 1 \);
then
    echo "Usage: build.sh config"
    echo ""
    echo "Configs:"
    echo "  debug   -   build with the debug configuration"
    echo "  release -   build with the release configuration"
    echo ""
    exit 1
fi

if test \( \( -n "$1" \) -a \( "$1" = "debug" \) \);then 
    NDK_BUILD_ARG_CONFIG="APP_DEBUG:=true"
    OBJ_DIR_CONFIG="obj/debug"
    LIBS_DIR_CONFIG="libs/debug/lib"
elif test \( \( -n "$1" \) -a \( "$1" = "release" \) \);then
    NDK_BUILD_ARG_CONFIG="APP_DEBUG:=false"
    OBJ_DIR_CONFIG="obj/release"
    LIBS_DIR_CONFIG="libs/release/lib"
else
    echo "The config \"$1\" is not supported!"
    echo ""
    echo "Configs:"
    echo "  debug   -   build with the debug configuration"
    echo "  release -   build with the release configuration"
    echo ""
    exit 1
fi

MY_DIR="$(cd "$(dirname "$0")" 1>/dev/null 2>/dev/null && pwd)"  

NDK_BUILD_ARGS="${NDK_BUILD_ARG_CONFIG} NDK_PROJECT_PATH:=null NDK_OUT:=${OBJ_DIR_CONFIG} NDK_LIBS_OUT:=${LIBS_DIR_CONFIG} NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=Android.mk"

# build by ndk-build 
cd ${MY_DIR}

if "${MY_DIR}/android-ndk-r14b/ndk-build" ${NDK_BUILD_ARGS}; then
    echo "ndk-build passed"
else
    echo "ndk-build failed"
    exit 1
fi

# Packaging

mkdir -p "${MY_DIR}/bin"

if test \( \( -n "$1" \) -a \( "$1" = "debug" \) \);then 
    # help vscode find the symbol
    OUT_BINS=("libpt_mcrt.so" "libpt_gfx.so" "libpt_wsi.so" "libpt_launcher.so")
    for i in "${OUT_BINS[@]}"
    do
        chrpath -r '$ORIGIN' "${MY_DIR}/${OBJ_DIR_CONFIG}/local/arm64-v8a/${i}"
        chrpath -r '$ORIGIN' "${MY_DIR}/${OBJ_DIR_CONFIG}/local/armeabi-v7a/${i}"
        chrpath -r '$ORIGIN' "${MY_DIR}/${OBJ_DIR_CONFIG}/local/x86_64/${i}"
        chrpath -r '$ORIGIN' "${MY_DIR}/${OBJ_DIR_CONFIG}/local/x86/${i}"
        cp -f "${MY_DIR}/${OBJ_DIR_CONFIG}/local/arm64-v8a/${i}" "${MY_DIR}/${LIBS_DIR_CONFIG}/arm64-v8a/${i}"
        cp -f "${MY_DIR}/${OBJ_DIR_CONFIG}/local/armeabi-v7a/${i}" "${MY_DIR}/${LIBS_DIR_CONFIG}/armeabi-v7a/${i}"
        cp -f "${MY_DIR}/${OBJ_DIR_CONFIG}/local/x86_64/${i}" "${MY_DIR}/${LIBS_DIR_CONFIG}/x86_64/${i}"
        cp -f "${MY_DIR}/${OBJ_DIR_CONFIG}/local/x86/${i}" "${MY_DIR}/${LIBS_DIR_CONFIG}/x86/${i}"
    done

    # we upload the gdbserver manually in ndk-gdb.py
    rm -rf "${MY_DIR}/${LIBS_DIR_CONFIG}/arm64-v8a/gdb.setup"

    # used by ndk_gdbserver_linux.sh
    mkdir -p "${MY_DIR}/bin/arm64-v8a"
    mv -f "${MY_DIR}/${LIBS_DIR_CONFIG}/arm64-v8a/gdbserver" "${MY_DIR}/bin/arm64-v8a/gdbserver"

    rm -rf "${MY_DIR}/${LIBS_DIR_CONFIG}/armeabi-v7a/gdb.setup"
    rm -rf "${MY_DIR}/${LIBS_DIR_CONFIG}/armeabi-v7a/gdbserver"
    rm -rf "${MY_DIR}/${LIBS_DIR_CONFIG}/x86_64/gdb.setup"
    rm -rf "${MY_DIR}/${LIBS_DIR_CONFIG}/x86_64/gdbserver"
    rm -rf "${MY_DIR}/${LIBS_DIR_CONFIG}/x86/gdb.setup"
    rm -rf "${MY_DIR}/${LIBS_DIR_CONFIG}/x86/gdbserver"

    # vulkan_sdk / libVkLayer_khronos_validation 
    cp -f "${MY_DIR}/../../third_party/vulkan_sdk/lib/bionic_arm/libVkLayer_khronos_validation.so" "${MY_DIR}/${LIBS_DIR_CONFIG}/armeabi-v7a/libVkLayer_khronos_validation.so"
    cp -f "${MY_DIR}/../../third_party/vulkan_sdk/lib/bionic_arm64/libVkLayer_khronos_validation.so" "${MY_DIR}/${LIBS_DIR_CONFIG}/arm64-v8a/libVkLayer_khronos_validation.so"
    cp -f "${MY_DIR}/../../third_party/vulkan_sdk/lib/bionic_x64/libVkLayer_khronos_validation.so" "${MY_DIR}/${LIBS_DIR_CONFIG}/x86_64/libVkLayer_khronos_validation.so"
    cp -f "${MY_DIR}/../../third_party/vulkan_sdk/lib/bionic_x86/libVkLayer_khronos_validation.so" "${MY_DIR}/${LIBS_DIR_CONFIG}/x86/libVkLayer_khronos_validation.so"

    AAPT_ARG_CONFIG="--debug-mode"
    APK_LIBS_DIR_CONFIG="libs/debug"
    APK_UNSIGNED_NAME_CONFIG="Android.Packaging-debug-unaligned.apk"
    APK_NAME_CONFIG="Android.Packaging-debug.apk"
elif test \( \( -n "$1" \) -a \( "$1" = "release" \) \);then
    AAPT_ARG_CONFIG=""
    APK_LIBS_DIR_CONFIG="libs/release"
    APK_UNSIGNED_NAME_CONFIG="Android.Packaging-release-unaligned.apk"
    APK_NAME_CONFIG="Android.Packaging-release.apk"
else
    echo "The config \"$1\" is not supported!"
    echo ""
    echo "Configs:"
    echo "  debug   -   build with the debug configuration"
    echo "  release -   build with the release configuration"
    echo ""
    exit 1
fi


"${MY_DIR}/android-sdk/build-tools/29.0.3/aapt" package -f ${AAPT_ARG_CONFIG} -0 apk -M "${MY_DIR}/AndroidManifest.xml"  -S "${MY_DIR}/res" -I "${MY_DIR}/android-sdk/platforms/android-24/android.jar" -F "${MY_DIR}/bin/${APK_UNSIGNED_NAME_CONFIG}" "${MY_DIR}/${APK_LIBS_DIR_CONFIG}"    

"${MY_DIR}/android-sdk/build-tools/29.0.3/zipalign" -f 4 "${MY_DIR}/bin/${APK_UNSIGNED_NAME_CONFIG}" "${MY_DIR}/bin/${APK_NAME_CONFIG}"

# https://docs.microsoft.com/en-us/xamarin/android/deploy-test/signing/manually-signing-the-apk#sign-the-apk
"${MY_DIR}/android-sdk/build-tools/29.0.3/apksigner" sign -v --ks "${MY_DIR}/debug.keystore" --ks-pass pass:android --ks-key-alias androiddebugkey "${MY_DIR}/bin/${APK_NAME_CONFIG}"
