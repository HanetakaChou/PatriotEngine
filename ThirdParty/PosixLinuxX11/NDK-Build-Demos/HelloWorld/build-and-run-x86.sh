#!/bin/bash

cd "$(dirname "$(readlink -f "${0}")")"
 
# build by ndk
  
rm -rf obj/local/x86
rm -rf libs/x86
ndk-build APP_DEBUG:=false APP_ABI:=x86 NDK_PROJECT_PATH:=null NDK_OUT:=obj NDK_LIBS_OUT:=libs NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=LinuxX11.mk 

# before execute change the rpath to \$ORIGIN  
  
chrpath -r '$ORIGIN' libs/x86/a.out

  
# copy the dep libs to out dir  
  
cp -f ../../Bionic-Redistributable/lib/libc.so libs/x86
cp -f ../../Bionic-Redistributable/lib/libdl.so libs/x86
cp -f ../../Bionic-Redistributable/lib/libm.so libs/x86
cp -f ../../Bionic-Redistributable/lib/libstdc++.so libs/x86

# copy the linker to cwd  

cp -f ../../Bionic-Redistributable/lib/linker libs/x86
cd libs/x86
  
# execute the generated a.out  

./a.out
