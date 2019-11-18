#!/bin/bash
cd "$(dirname "$0")"

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

../../ThirdParty/llvm/bin/clang++ -E -C PTRuntime.hpp -DPT_TARGET_ARCH=3 -DPT_DEBUG=1 -o .PTRuntime.mk

make -f .PTRuntime.mk