#!/bin/bash

cd "$(dirname "$(readlink -f "${0}")")"
 
# build by ndk
  
rm -rf obj
rm -rf libs
ndk-build NDK_PROJECT_PATH:=null NDK_OUT:=obj NDK_LIBS_OUT:=libs NDK_APPLICATION_MK:=Application-x86.mk APP_BUILD_SCRIPT:=Android.mk 

# before execute change the rpath to \$ORIGIN  
  
chrpath -r '$ORIGIN' libs/x86/a.out

  
# copy the dep libs to out dir  
  
cp -f ../Bionic/lib/libc.so libs/x86
cp -f ../Bionic/lib/libdl.so libs/x86
cp -f ../Bionic/lib/libm.so libs/x86
cp -f ../Bionic/lib/libstdc++.so libs/x86

# copy the linker to cwd  

cp -f ../Bionic/bin/linker .
  
# execute the generated a.out  

libs/x86/a.out
