#!/bin/bash
cd "$(dirname "$0")"

# yum install glibc-devel libxcb-devel xcb-util-keysyms-devel 
# yum install glibc-devel.i686 libxcb-devel.i686 xcb-util-keysyms-devel.i686

# Conventions
# GNU make: Implicit Variables
# https://www.gnu.org/software/make/manual/html_node/Implicit-Variables.html
# ndk-build | Android NDK
# https://developer.android.com/ndk/guides/ndk-build
# Android.mk | Android NDK
# https://developer.android.com/ndk/guides/android_mk

# PT_TARGET_ARCH
# 0 ARM
# 1 ARM64
# 2 x86
# 3 x64

# PT_DEBUG
# 0 Release
# 1 Debug

rm -rf .PTExample.mk
../../../ThirdParty/PosixLinuxGlibc/bin64/clang++ -E -C PTExample.hpp -DPT_TARGET_ARCH=2 -DPT_DEBUG=1 -o .PTExample.mk

make -f .PTExample.mk .PHONY
make -f .PTExample.mk

# ../../ThirdParty/PosixLinuxGlibc/bin64/clang++ -E PT-Launch-Json.hpp -DPT_TARGET_ARCH=2 -DPT_DEBUG=1 -o .launch.json
