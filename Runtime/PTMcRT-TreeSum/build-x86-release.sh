#!/bin/bash

cd "$(dirname "$(readlink -f "${0}")")"
 
# build by ndk  
rm -rf obj
rm -rf libs
ndk-build APP_DEBUG:=false APP_ABI:=x86 NDK_PROJECT_PATH:=null NDK_OUT:=obj NDK_LIBS_OUT:=libs NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=LinuxX11.mk 

# before execute change the rpath to \$ORIGIN    
chrpath -r '$ORIGIN' libs/x86/PTMcRT-TreeSum.bundle

# mkdir the out dir if necessary
mkdir -p ../../../Binary/PosixLinuxX11/x86/Release/

# copy the unstriped pie to out dir
rm -rf ../../Binary/PosixLinuxX11/x86/Release/PTMcRT-TreeSum.bundle
cp -f libs/x86/PTMcRT-TreeSum.bundle ../../Binary/PosixLinuxX11/x86/Release/

# place the linker at cwd  
cp -f ../../ThirdParty/PosixLinuxX11/Bionic-Redistributable/lib64/linker ../../Binary/PosixLinuxX11/x86/Release/
cd ../../Binary/PosixLinuxX11/x86/Release/

# execute the generated pie 
./PTMcRT-TreeSum.bundle
