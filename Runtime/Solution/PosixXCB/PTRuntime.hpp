#define _PT_QUOTE(i) #i
#define PT_QUOTE(i) _PT_QUOTE(i)

#ifdef _MSC_VER
	#define _PT_SINGLEQUOTE(i) #@i
	#define PT_SINGLEQUOTE(i) _PT_SINGLEQUOTE(i)
#endif

#ifdef PT_TARGET_ARCH
	#if PT_TARGET_ARCH == 0
		#define PT_TARGET_ARCH_NAME ARM
		#define PT_LLVM_LIB_NAME libunknown
	#elif PT_TARGET_ARCH == 1
		#define PT_TARGET_ARCH_NAME ARM64
		#define PT_LLVM_LIB_NAME libunknown
	#elif PT_TARGET_ARCH == 2
		#define PT_TARGET_ARCH_NAME x86
		#define PT_LLVM_LIB_NAME lib
		#define PT_TARGET_ARCH_CPPFLAGS -m32
		#define PT_TARGET_ARCH_LDFLAGS -m32
	#elif PT_TARGET_ARCH == 3
		#define PT_TARGET_ARCH_NAME x64
		#define PT_LLVM_LIB_NAME lib64
		#define PT_TARGET_ARCH_CPPFLAGS 
		#define PT_TARGET_ARCH_LDFLAGS 
	#else
		#error PT_TARGET_ARCH Unknown
	#endif
#else
	#error PT_TARGET_ARCH Not Defined
#endif

#ifdef PT_DEBUG
	#if PT_DEBUG == 0
		#define PT_DEBUG_NAME Release
		#define PT_DEBUG_CPPFLAGS \
			-g0 /*General*/ \
			-O3 -fomit-frame-pointer /*Optimization*/ \
			-ffunction-sections -fdata-sections /*Code Generation */ 
		#define PT_DEBUG_LDFLAGS 
	#else
		#define PT_DEBUG_NAME Debug
		#define PT_DEBUG_CPPFLAGS \
				-g2 -gdwarf-2 /*General*/ \
				-O0 -fno-omit-frame-pointer /*Optimization*/
		#define PT_DEBUG_LDFLAGS 
	#endif
#else
	#error PT_DEBUG Not Defined
#endif

#ifndef PT_TARGET_ARCH_NAME
	#error PT_TARGET_ARCH_NAME Not Defined
#endif

#ifndef PT_LLVM_LIB_NAME
	#error PT_LLVM_LIB_NAME Not Defined
#endif

#ifndef PT_DEBUG_NAME
	#error PT_DEBUG_NAME Not Defined
#endif

#ifndef PT_DEBUG_CPPFLAGS
	#error PT_DEBUG_CPPFLAGS Not Defined
#endif

#ifndef PT_DEBUG_LDFLAGS
	#error PT_DEBUG_CPPFLAGS Not Defined
#endif

.RECIPEPREFIX = &

#define PT_RECIPEPREFIX &

#define PT_MAKEFILE PTRuntime.hpp

#//PTLauncher---------------------------------------------------------------------------------------------------------------------

#ifdef PT_CPP
	#error PT_CPP Has Been Defined
#endif

#ifdef PT_STRIP
	#error PT_STRIP Has Been Defined
#endif

#ifdef PT_CPPFLAGS
	#error PT_CPPFLAGS Has Been Defined
#endif

#ifdef PT_LDFLAGS
	#error PT_LDFLAGS Has Been Defined
#endif

#define PT_CPP ../../ThirdParty/llvm/bin/clang++

#define PT_STRIP ../../ThirdParty/llvm/bin/llvm-strip

#define PT_CPPFLAGS \
    -fdiagnostics-format=msvc \
    -stdlib=libc++ \
    -Wall /*General*/ \
    -fno-strict-aliasing /*Optimization*/ \
    -fno-exceptions -fstack-protector -fpie -fno-short-enums /*Code Generation*/\
    -fno-rtti -std=c++11 /*Language*/ \
    -x c++ /*Advanced*/ \
    -finput-charset=UTF-8 -fexec-charset=UTF-8 \
    -pthread \
    -fvisibility=hidden \
    PT_DEBUG_CPPFLAGS

#define PT_LDFLAGS \
    -fdiagnostics-format=msvc \
    -stdlib=libc++ -lc++ \
    -Wl,--no-undefined /*General*/ \
    -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack /*Advanced*/ \
    -pie \
    -pthread  \
    -finput-charset=UTF-8 -fexec-charset=UTF-8 \
    -lxcb -lxcb-keysyms \
    /*-Wl,--enable-new-dtags*/ -Wl,-rpath,'$$ORIGIN' \
    PT_DEBUG_LDFLAGS 

#//STRIP---------------------------------------------------------------------------------------------------------------------

../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTLauncher.bundle: \
    ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTLauncher.bundle \
    PT_MAKEFILE
PT_RECIPEPREFIX \
    PT_STRIP \
        ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTLauncher.bundle \
        --strip-unneeded \
        -o ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTLauncher.bundle 
        

#//LD---------------------------------------------------------------------------------------------------------------------

../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTLauncher.bundle: \
    ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTWindowImpl.o \
    ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libunwind.so.1 \
    ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++abi.so.1 \
    ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++.so.1 \
    PT_MAKEFILE
PT_RECIPEPREFIX \
    PT_CPP \
        ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTWindowImpl.o \
        PT_LDFLAGS \
        -o ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTLauncher.bundle 

../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libunwind.so.1: ../../ThirdParty/llvm/PT_LLVM_LIB_NAME/libunwind.so.1
PT_RECIPEPREFIX cp -f ../../ThirdParty/llvm/PT_LLVM_LIB_NAME/libunwind.so.1 ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libunwind.so.1

../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++abi.so.1: ../../ThirdParty/llvm/PT_LLVM_LIB_NAME/libc++abi.so.1
PT_RECIPEPREFIX cp -f ../../ThirdParty/llvm/PT_LLVM_LIB_NAME/libc++abi.so.1 ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++abi.so.1

../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++.so.1: ../../ThirdParty/llvm/PT_LLVM_LIB_NAME/libc++.so.1
PT_RECIPEPREFIX cp -f ../../ThirdParty/llvm/PT_LLVM_LIB_NAME/libc++.so.1 ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++.so.1

#//CPP--------------------------------------------------------------------------------------------------------------------

../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTWindowImpl.o: \
    ../../Private/Launcher/PosixXCB/PTWindowImpl.cpp \
    ../../Private/Launcher/PosixXCB/PTWindowImpl.h \
    PT_MAKEFILE
PT_RECIPEPREFIX \
    PT_CPP -c \
        ../../Private/Launcher/PosixXCB/PTWindowImpl.cpp \
        PT_CPPFLAGS \
        -o ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTWindowImpl.o

#undef PT_CPP

#undef PT_STRIP

#undef PT_CPPFLAGS

#undef PT_LDFLAGS

#//PTSystem---------------------------------------------------------------------------------------------------------------------

#ifdef PT_CPP
	#error PT_CPP Has Been Defined
#endif

#ifdef PT_STRIP
	#error PT_STRIP Has Been Defined
#endif

#ifdef PT_CPPFLAGS
	#error PT_CPPFLAGS Has Been Defined
#endif

#ifdef PT_LDFLAGS
	#error PT_LDFLAGS Has Been Defined
#endif

#define PT_CPP ../../ThirdParty/llvm/bin/clang++

#define PT_STRIP ../../ThirdParty/llvm/bin/llvm-strip

#define PT_CPPFLAGS \
    -fdiagnostics-format=msvc \
    -stdlib=libc++ \
    -Wall /*General*/ \
    -fno-strict-aliasing /*Optimization*/ \
    -fno-exceptions -fstack-protector -fpic -fno-short-enums /*Code Generation*/\
    -fno-rtti -std=c++11 /*Language*/ \
    -x c++ /*Advanced*/ \
    -finput-charset=UTF-8 -fexec-charset=UTF-8 \
    -pthread \
    -fvisibility=hidden \
    PT_DEBUG_CPPFLAGS

#define PT_LDFLAGS \
    -fdiagnostics-format=msvc \
    -stdlib=libc++ -lc++ \
    -Wl,--no-undefined /*General*/ \
    -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack /*Advanced*/ \
    -shared -Wl,-soname="libPTSystem.so" \
    -pthread  \
    -finput-charset=UTF-8 -fexec-charset=UTF-8 \
    -lxcb -lxcb-keysyms \
    /*-Wl,--enable-new-dtags*/ -Wl,-rpath,'$$ORIGIN' \
    PT_DEBUG_LDFLAGS 

#//STRIP---------------------------------------------------------------------------------------------------------------------