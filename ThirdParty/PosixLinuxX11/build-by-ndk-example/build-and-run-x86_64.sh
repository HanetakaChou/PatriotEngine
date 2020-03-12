#!/bin/bash

cd "$(dirname "$(readlink -f "${0}")")"
 
# build by ndk
  
rm -rf obj
rm -rf libs
ndk-build APP_DEBUG:=false APP_ABI:=x86_64 NDK_PROJECT_PATH:=null NDK_OUT:=obj NDK_LIBS_OUT:=libs NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=Android.mk 

# before execute change the rpath to \$ORIGIN  
  
chrpath -r '$ORIGIN' libs/x86_64/a.out

  
# copy the dep libs to out dir  
  
cp -f ../Bionic/lib64/libc.so libs/x86_64
cp -f ../Bionic/lib64/libdl.so libs/x86_64
cp -f ../Bionic/lib64/libm.so libs/x86_64
cp -f ../Bionic/lib64/libstdc++.so libs/x86_64  

# copy the linker to cwd  

cp -f ../Bionic/bin64/linker libs/x86_64
cd libs/x86_64
  
# execute the generated a.out  

./a.out
