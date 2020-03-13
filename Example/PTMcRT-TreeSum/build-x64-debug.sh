#!/bin/bash

cd "$(dirname "$(readlink -f "${0}")")"
 
# build by ndk
rm -rf obj
rm -rf libs
ndk-build APP_DEBUG:=true APP_ABI:=x86_64 NDK_PROJECT_PATH:=null NDK_OUT:=obj NDK_LIBS_OUT:=libs NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=LinuxX11.mk 

# before execute change the rpath to \$ORIGIN  
chrpath -r '$ORIGIN' obj/local/x86_64/PTMcRT-TreeSum.bundle

# mkdir the out dir if necessary
mkdir -p ../../Binary/PosixLinuxX11/x64/Debug/

# copy the unstriped pie to out dir
rm -rf ../../Binary/PosixLinuxX11/x64/Debug/PTMcRT-TreeSum.bundle
cp -f obj/local/x86_64/PTMcRT-TreeSum.bundle ../../Binary/PosixLinuxX11/x64/Debug/

# copy the gdb related
cp -f libs/x86_64/gdbserver ../../Binary/PosixLinuxX11/x64/Debug/
cp -f libs/x86_64/gdb.setup ../../Binary/PosixLinuxX11/x64/Debug/

# place the linker at cwd  
cp -f ../../ThirdParty/PosixLinuxX11/Bionic-Redistributable/lib64/linker ../../Binary/PosixLinuxX11/x64/Debug/
cd ../../Binary/PosixLinuxX11/x64/Debug/

# execute the generated pie  
# gdbserver :27077 ./PTMcRT-TreeSum.bundle
./gdbserver :27077 ./PTMcRT-TreeSum.bundle ### //either gdbserver from ndk or your linux distribution is OK

