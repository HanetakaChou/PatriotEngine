#!/bin/bash

# yum install gcc  
# yum install gcc-c++
# yum install glibc-devel
# yum install libstdc++-devel
# yum install libstdc++-static
# yum install compiler-rt 
# yum install libunwind-devel
# yum install libcxx-devel

# yum install glibc-devel.i686
# yum install libstdc++-devel.i686
# yum install libstdc++-static.i686
# yum install compiler-rt.i686 
# yum install libunwind-devel.i686
# yum install libcxx-devel.i686

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

# configure
MY_DIR="$(dirname "$(readlink -f "${0}")")"
cd ${MY_DIR}

if test \( \( \( -n "$1" \) -a \( "$1" = "debug" \) \) -a \( \( -n "$2" \) -a \( "$2" = "x86" \) \) \);
then
    NDK_BUILD_ARGS="APP_DEBUG:=true APP_ABI:=x86 NDK_PROJECT_PATH:=null NDK_OUT:=obj NDK_LIBS_OUT:=libs NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=Linux_X11.mk"
    INTERMEDIATE_DIR="${MY_DIR}/obj/local/x86/"
    OUT_DIR="${MY_DIR}/../../bin/posix_linux_x11/x86/debug/"
elif test \( \( \( -n "$1" \) -a \( "$1" = "debug" \) \) -a \( \( -n "$2" \) -a \( "$2" = "x64" \) \) \);
then
    NDK_BUILD_ARGS="APP_DEBUG:=true APP_ABI:=x86_64 NDK_PROJECT_PATH:=null NDK_OUT:=obj NDK_LIBS_OUT:=libs NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=Linux_X11.mk"
    INTERMEDIATE_DIR="${MY_DIR}/obj/local/x86_64/"
    OUT_DIR="${MY_DIR}/../../bin/posix_linux_x11/x64/debug/"
elif test \( \( \( -n "$1" \) -a \( "$1" = "release" \) \) -a \( \( -n "$2" \) -a \( "$2" = "x86" \) \) \);
then
    NDK_BUILD_ARGS="APP_DEBUG:=false APP_ABI:=x86 NDK_PROJECT_PATH:=null NDK_OUT:=obj NDK_LIBS_OUT:=libs NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=Linux_X11.mk"
    INTERMEDIATE_DIR="${MY_DIR}/libs/x86/"
    OUT_DIR="${MY_DIR}/../../bin/posix_linux_x11/x86/release/"
elif test \( \( \( -n "$1" \) -a \( "$1" = "release" \) \) -a \( \( -n "$2" \) -a \( "$2" = "x64" \) \) \);
then
    NDK_BUILD_ARGS="APP_DEBUG:=false APP_ABI:=x86_64 NDK_PROJECT_PATH:=null NDK_OUT:=obj NDK_LIBS_OUT:=libs NDK_APPLICATION_MK:=Application.mk APP_BUILD_SCRIPT:=Linux_X11.mk"
    INTERMEDIATE_DIR="${MY_DIR}/libs/x86_64/"
    OUT_DIR="${MY_DIR}/../../bin/posix_linux_x11/x64/release/"
else
    echo "Unsupported config \"$1\" and platform \"$2\"!"
    exit 1
fi

#OUT_BINS="PTLauncher.bundle libPTMcRT.so libPTApp.so"
OUT_BINS="libpt_mcrt.so"

# build by ndk  
rm -rf obj
rm -rf libs
${MY_DIR}/ndk_build_centos_glibc/ndk-build V=1 VERBOSE=1 ${NDK_BUILD_ARGS}

# before execute change the rpath to \$ORIGIN    
# fix me: define the $ORIGIN correctly in the Linux_X11.mk
for i in ${OUT_BINS}
do
    chrpath -r '$ORIGIN' ${INTERMEDIATE_DIR}/${i} 
done

# mkdir the out dir if necessary
mkdir -p ${OUT_DIR}

# copy the unstriped so to out dir
for i in ${OUT_BINS}
do
    rm -rf ${OUT_DIR}/${i}
    cp -f ${INTERMEDIATE_DIR}/${i} ${OUT_DIR}/
done