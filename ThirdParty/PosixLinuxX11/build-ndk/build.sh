#!/bin/bash
rm -rf obj
rm -rf libs
ndk-build NDK_DEBUG=1 NDK_PROJECT_PATH:=null NDK_OUT:=obj NDK_LIBS_OUT:=libs NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=Android.mk 
