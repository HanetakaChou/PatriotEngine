#!/bin/bash

cd "$(dirname "$(readlink -f "${0}")")"
 
intermediate_dir="obj/local/x86/"
out_dir="../../../Binary/PosixLinuxX11/x86/Debug/"
out_name="PTLauncher.bundle"
out_name1="libPTMcRT.so"
out_name2="libPTApp.so"

# build by ndk  
rm -rf obj
rm -rf libs
ndk-build APP_DEBUG:=true APP_ABI:=x86 NDK_PROJECT_PATH:=null NDK_OUT:=obj NDK_LIBS_OUT:=libs NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=LinuxX11.mk 

# before execute change the rpath to \$ORIGIN    
chrpath -r '$ORIGIN' ${intermediate_dir}/${out_name}
chrpath -r '$ORIGIN' ${intermediate_dir}/${out_name1}
chrpath -r '$ORIGIN' ${intermediate_dir}/${out_name2}

# mkdir the out dir if necessary
mkdir -p ${out_dir}

# copy the unstriped so to out dir
rm -rf ${out_dir}/${out_name}
rm -rf ${out_dir}/${out_name1}
rm -rf ${out_dir}/${out_name2}
cp -f ${intermediate_dir}/${out_name} ${out_dir}/
cp -f ${intermediate_dir}/${out_name1} ${out_dir}/
cp -f ${intermediate_dir}/${out_name2} ${out_dir}/

# copy the dep libs to out dir   
cp -f ../../../ThirdParty/PosixLinuxX11/Bionic-Redistributable/lib/libxcb.so ${out_dir}/
cp -f ../../../ThirdParty/PosixLinuxX11/Bionic-Redistributable/lib/libXau.so ${out_dir}/
cp -f ../../../ThirdParty/PosixLinuxX11/Bionic-Redistributable/lib/libc.so ${out_dir}/
cp -f ../../../ThirdParty/PosixLinuxX11/Bionic-Redistributable/lib/libdl.so ${out_dir}/
cp -f ../../../ThirdParty/PosixLinuxX11/Bionic-Redistributable/lib/libm.so ${out_dir}/
cp -f ../../../ThirdParty/PosixLinuxX11/Bionic-Redistributable/lib/libstdc++.so ${out_dir}/

# copy the gdb related
cp -f libs/x86/gdbserver ${out_dir}/
cp -f libs/x86/gdb.setup ${out_dir}/

# place the linker at cwd  
cp -f ../../../ThirdParty/PosixLinuxX11/Bionic-Redistributable/lib/linker ${out_dir}/
cd ${out_dir}/

# execute the generated pie  
# gdbserver :27077 ./${out_name}
./gdbserver :27077 ./${out_name} ### //either gdbserver from ndk or your linux distribution is OK