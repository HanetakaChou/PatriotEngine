#!/bin/bash

if test \( $# -ne 2 \);
then
    echo "Usage: build.sh config platform"
    echo ""
    echo "Configs:"
    echo "  debug   -   build with the debug configuration"
    echo "  release -   build with the release configuration"
    echo ""
    echo "Platforms:"
    echo "  x86     -   build with the x86 configuration"
    echo "  x64     -   build with the x64 configuration"
    echo ""
    exit 1
fi

if test ! \( \( \( -n "$1" \) -a \( "$1" = "debug" \) \) -o \( \( -n "$1" \) -a \( "$1" = "release" \) \) \);
then
    echo "The config \"$1\" is not supported!"
    echo ""
    echo "Configs:"
    echo "  debug   -   build with the debug configuration"
    echo "  release -   build with the release configuration"
    echo ""
    exit 1
fi

if test ! \( \( \( -n "$2" \) -a \( "$2" = "x86" \) \) -o \( \( -n "$2" \) -a \( "$2" = "x64" \) \) \);
then
    echo "The platform \"$2\" is not supported!"
    echo ""
    echo "Platforms:"
    echo "  x86     -   build with the x86 configuration"
    echo "  x64     -   build with the x64 configuration"
    echo ""
    exit 1
fi

DIR="$(dirname "$(readlink -f "${0}")")"
cd ${DIR}

if test \( \( \( -n "$1" \) -a \( "$1" = "debug" \) \) -a \( \( -n "$2" \) -a \( "$2" = "x86" \) \) \);
then
    NDK_BUILD_ARGS="APP_DEBUG:=true APP_ABI:=x86 NDK_PROJECT_PATH:=null NDK_OUT:=obj NDK_LIBS_OUT:=libs NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=Linux_X11.mk"
    intermediate_dir="${DIR}/obj/local/x86/"
    out_dir="${DIR}/../../bin/posix_linux_x11/x86/debug/"
    #out_name1="PTLauncher.bundle"
    out_name2="libPTMcRT.so"
    #out_name3="libPTApp.so"
elif test \( \( \( -n "$1" \) -a \( "$1" = "debug" \) \) -a \( \( -n "$2" \) -a \( "$2" = "x64" \) \) \);
then
    NDK_BUILD_ARGS="APP_DEBUG:=true APP_ABI:=x86_64 NDK_PROJECT_PATH:=null NDK_OUT:=obj NDK_LIBS_OUT:=libs NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=Linux_X11.mk"
    intermediate_dir="${DIR}/obj/local/x86_64/"
    out_dir="${DIR}/../../bin/posix_linux_x11/x64/debug/"
    #out_name1="PTLauncher.bundle"
    out_name2="libPTMcRT.so"
    #out_name3="libPTApp.so"
elif test \( \( \( -n "$1" \) -a \( "$1" = "release" \) \) -a \( \( -n "$2" \) -a \( "$2" = "x86" \) \) \);
then
    NDK_BUILD_ARGS="APP_DEBUG:=false APP_ABI:=x86 NDK_PROJECT_PATH:=null NDK_OUT:=obj NDK_LIBS_OUT:=libs NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=Linux_X11.mk"
    intermediate_dir="${DIR}/libs/x86/"
    out_dir="${DIR}/../../bin/posix_linux_x11/x86/release/"
    #out_name1="PTLauncher.bundle"
    out_name2="libPTMcRT.so"
    #out_name3="libPTApp.so"
elif test \( \( \( -n "$1" \) -a \( "$1" = "release" \) \) -a \( \( -n "$2" \) -a \( "$2" = "x64" \) \) \);
then
    NDK_BUILD_ARGS="APP_DEBUG:=false APP_ABI:=x86_64 NDK_PROJECT_PATH:=null NDK_OUT:=obj NDK_LIBS_OUT:=libs NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=LinuxX11.mk"
    intermediate_dir="${DIR}/libs/x86_64/"
    out_dir="${DIR}/../../bin/posix_linux_x11/x64/release/"
    #out_name1="PTLauncher.bundle"
    out_name2="libPTMcRT.so"
    #out_name3="libPTApp.so"
else
    echo "Unsupported config \"$1\" and platform \"$2\"!"
    exit 1
fi

# build by ndk  
rm -rf obj
rm -rf libs
ndk-build ${NDK_BUILD_ARGS}

# before execute change the rpath to \$ORIGIN    
# fix me: define the $ORIGIN correctly in the Linux_X11.mk

# https://www.gnu.org/software/bash/manual/html_node/Shell-Parameter-Expansion.html
CHRPATH_PATH=""
#CHRPATH_PATH=${CHRPATH_PATH}${CHRPATH_PATH:+" "}${intermediate_dir}${out_name1}
CHRPATH_PATH=${CHRPATH_PATH}${CHRPATH_PATH:+" "}${intermediate_dir}${out_name2}
#CHRPATH_PATH=${CHRPATH_PATH}${CHRPATH_PATH:+" "}${intermediate_dir}${out_name3}

for i in ${CHRPATH_PATH}
do
chrpath -r '$ORIGIN' $i 
done

# mkdir the out dir if necessary
mkdir -p ${out_dir}

# copy the unstriped so to out dir
#rm -rf ${out_dir}/${out_name1}
rm -rf ${out_dir}${out_name2}
#rm -rf ${out_dir}/${out_name3}
#cp -f ${intermediate_dir}/${out_name1} ${out_dir}/
cp -f ${intermediate_dir}/${out_name2} ${out_dir}/
#cp -f ${intermediate_dir}/${out_name3} ${out_dir}/