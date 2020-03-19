#!/bin/bash

cd "$(dirname "$(readlink -f "${0}")")"

target_name="a.out"
int_dir="libs/x86_64"

# build by ndk
rm -rf obj/local/x86_64
rm -rf libs/x86_64
ndk-build APP_DEBUG:=false APP_ABI:=x86_64 NDK_PROJECT_PATH:=null NDK_OUT:=obj NDK_LIBS_OUT:=libs NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=LinuxX11.mk 

# before execute change the rpath to \$ORIGIN  
chrpath -r '$ORIGIN' ${int_dir}/${target_name}

  
# copy the dep libs to out dir  
cp -f ../../Bionic-Redistributable/lib64/libc.so ${int_dir}
cp -f ../../Bionic-Redistributable/lib64/libdl.so ${int_dir}
cp -f ../../Bionic-Redistributable/lib64/libm.so ${int_dir}
cp -f ../../Bionic-Redistributable/lib64/libstdc++.so ${int_dir}  

# place the linker at cwd   
cp -f ../../Bionic-Redistributable/lib64/linker ${int_dir}
cd ${int_dir}
  
# execute the generated a.out  
./${target_name}
