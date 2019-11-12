#include "PTCommon.hpp"

#define PT_CPP ../../ThirdParty/llvm/bin/clang++

#define PT_CPPFLAGS \
    -finput-charset=UTF-8 -fexec-charset=UTF-8 \
    -x c++ -std=c++11 \
    -fpie -pthread -stdlib=libc++ \
    -fvisibility=hidden \
    PT_DEBUG_CPPFLAGS

#define PT_LDFLAGS \
    -finput-charset=UTF-8 -fexec-charset=UTF-8 \
    -x c++ -std=c++11 \
    -pie -pthread -stdlib=libc++ \
    -lc++ \
    -lxcb -lxcb-keysyms \
    /*-Wl,--enable-new-dtags*/ -Wl,-rpath,'$$ORIGIN' \
    PT_DEBUG_LDFLAGS 

#define PT_MAKEFILE PTLauncher.hpp PTCommon.hpp

#//PT_NONE---------------------------------------------------------------------------------------------------------------------

PT_NONE:



#//LD---------------------------------------------------------------------------------------------------------------------

PTLauncher_LD: \
    ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTLauncher.bundle


../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTLauncher.bundle: \
    ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTWindowImpl.o \
    ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libunwind.so.1 \
    ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++abi.so.1 \
    ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++.so.1 \
    PT_MAKEFILE
PT_RECIPEPREFIX \
    PT_CPP \
        ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTWindowImpl.o \
        PT_LDFLAGS \
        -o ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTLauncher.bundle

../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libunwind.so.1: ../../ThirdParty/llvm/PT_LLVM_LIB_NAME/libunwind.so.1
PT_RECIPEPREFIX cp -f ../../ThirdParty/llvm/PT_LLVM_LIB_NAME/libunwind.so.1 ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libunwind.so.1

../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++abi.so.1: ../../ThirdParty/llvm/PT_LLVM_LIB_NAME/libc++abi.so.1
PT_RECIPEPREFIX cp -f ../../ThirdParty/llvm/PT_LLVM_LIB_NAME/libc++abi.so.1 ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++abi.so.1

../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++.so.1: ../../ThirdParty/llvm/PT_LLVM_LIB_NAME/libc++.so.1
PT_RECIPEPREFIX cp -f ../../ThirdParty/llvm/PT_LLVM_LIB_NAME/libc++.so.1 ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++.so.1

#//CPP--------------------------------------------------------------------------------------------------------------------

PTLauncher_CPP: \
    ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTWindowImpl.o


../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTWindowImpl.o: \
    ../../Private/Window/PosixXCB/PTWindowImpl.cpp \
    ../../Private/Window/PosixXCB/PTWindowImpl.h \
    PT_MAKEFILE
PT_RECIPEPREFIX \
    PT_CPP -c \
        ../../Private/Window/PosixXCB/PTWindowImpl.cpp \
        PT_CPPFLAGS \
        -o ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTWindowImpl.o