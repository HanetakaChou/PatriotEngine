#!/bin/bash

cd "$(dirname "$(readlink -f "${0}")")"
 
# build by ndk
  
rm -rf obj
rm -rf libs
ndk-build APP_DEBUG:=false APP_ABI:=x86 NDK_PROJECT_PATH:=null NDK_OUT:=obj NDK_LIBS_OUT:=libs NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=Android.mk 

# before execute change the rpath to \$ORIGIN  
  
chrpath -r '$ORIGIN' libs/x86/a.out

  
# copy the dep libs to out dir  
  
cp -f ../Bionic/lib/libc.so libs/x86
cp -f ../Bionic/lib/libdl.so libs/x86
cp -f ../Bionic/lib/libm.so libs/x86
cp -f ../Bionic/lib/libstdc++.so libs/x86

# copy the linker to cwd  

cp -f ../Bionic/bin/linker libs/x86
cd libs/x86
  
# execute the generated a.out  

./a.out
