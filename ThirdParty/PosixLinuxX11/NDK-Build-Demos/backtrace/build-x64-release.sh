#!/bin/bash

cd "$(dirname "$(readlink -f "${0}")")"

int_dir="libs/x86_64"
out_dir="../../../../Binary/x64/Release"
target_name="main.bundle"
target_name2="libmain2.so"
target_name3="libc_malloc_debug_backtrace.so"

# build by ndk
# rm -rf obj/local/x86_64
# rm -rf libs/x86_64
ndk-build APP_DEBUG:=false APP_ABI:=x86_64 NDK_PROJECT_PATH:=null NDK_OUT:=obj NDK_LIBS_OUT:=libs NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=LinuxX11.mk 

# before execute change the rpath to \$ORIGIN    
chrpath -r '$ORIGIN' ${int_dir}/${target_name}
chrpath -r '$ORIGIN' ${int_dir}/${target_name2}
chrpath -r '$ORIGIN' ${int_dir}/${target_name3}

# mkdir the out dir if necessary
mkdir -p ${out_dir}

# copy the unstriped so to out dir
rm -rf ${out_dir}/${target_name}
rm -rf ${out_dir}/${target_name2}
rm -rf ${out_dir}/${target_name3}
cp -f ${int_dir}/${target_name} ${out_dir}/
cp -f ${int_dir}/${target_name2} ${out_dir}/
cp -f ${int_dir}/${target_name3} ${out_dir}/

# copy the dep libs to out dir  
cp -f ../../Bionic-Redistributable/lib64/libc.so ${out_dir}/
cp -f ../../Bionic-Redistributable/lib64/libdl.so ${out_dir}/
cp -f ../../Bionic-Redistributable/lib64/libm.so ${out_dir}/
cp -f ../../Bionic-Redistributable/lib64/libstdc++.so ${out_dir}/  
cp -f ../../Bionic-Redistributable/lib64/libc++_shared.so ${out_dir}/  

# place the linker at cwd   
cp -f ../../Bionic-Redistributable/lib64/linker ${out_dir}/
cd ${out_dir}
  
# execute the generated a.out  
./${target_name} # --validate
