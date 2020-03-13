#!/bin/bash

cd "$(dirname "$(readlink -f "${0}")")"
 
# build by ndk  
rm -rf obj
rm -rf libs
ndk-build APP_DEBUG:=true APP_ABI:=x86 NDK_PROJECT_PATH:=null NDK_OUT:=obj NDK_LIBS_OUT:=libs NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=LinuxX11.mk 

# before execute change the rpath to \$ORIGIN    
chrpath -r '$ORIGIN' obj/local/x86/libPTMcRT.so

# mkdir the out dir if necessary
mkdir -p ../../../Binary/PosixLinuxX11/x86/Debug/

# copy the unstriped so to out dir
rm -rf ../../../Binary/PosixLinuxX11/x86/Debug/libPTMcRT.so
cp -f obj/local/x86/libPTMcRT.so ../../../Binary/PosixLinuxX11/x86/Debug/

# copy the dep libs to out dir   
cp -f ../../../ThirdParty/PosixLinuxX11/Bionic-Redistributable/lib/libc.so ../../../Binary/PosixLinuxX11/x86/Debug/
cp -f ../../../ThirdParty/PosixLinuxX11/Bionic-Redistributable/lib/libdl.so ../../../Binary/PosixLinuxX11/x86/Debug/
cp -f ../../../ThirdParty/PosixLinuxX11/Bionic-Redistributable/lib/libm.so ../../../Binary/PosixLinuxX11/x86/Debug/
cp -f ../../../ThirdParty/PosixLinuxX11/Bionic-Redistributable/lib/libstdc++.so ../../../Binary/PosixLinuxX11/x86/Debug/
