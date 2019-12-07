#define _PT_QUOTE(i) #i
#define PT_QUOTE(i) _PT_QUOTE(i)

#ifdef _MSC_VER
	#define _PT_SINGLEQUOTE(i) #@i
	#define PT_SINGLEQUOTE(i) _PT_SINGLEQUOTE(i)
#endif

#ifdef PT_TARGET_ARCH
	#if PT_TARGET_ARCH == 0
		#define PT_TARGET_ARCH_NAME ARM
	#elif PT_TARGET_ARCH == 1
		#define PT_TARGET_ARCH_NAME ARM64
	#elif PT_TARGET_ARCH == 2
		#define PT_TARGET_ARCH_NAME x86
		#define PT_TARGET_ARCH_CPPFLAGS -m32 -mssse3
		#define PT_TARGET_ARCH_LDFLAGS -m32
	#elif PT_TARGET_ARCH == 3
		#define PT_TARGET_ARCH_NAME x64
		#define PT_TARGET_ARCH_CPPFLAGS -mssse3
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
			-ffunction-sections -fdata-sections /*Code Generation */ \
            -DNDEBUG 
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

#define PT_MAKEFILE PTExample.hpp

#//Test-ParallelMap---------------------------------------------------------------------------------------------------------------------

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

#define PT_CPP ../../../ThirdParty/PosixLinuxGlibc/bin64/clang++

#define PT_STRIP ../../../ThirdParty/PosixLinuxGlibc/bin64/llvm-strip

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
    PT_TARGET_ARCH_CPPFLAGS \
    PT_DEBUG_CPPFLAGS

#define PT_LDFLAGS \
    -fdiagnostics-format=msvc \
	-fuse-ld=lld \
    -stdlib=libc++ -lc++ \
    -Wl,--no-undefined /*General*/ \
    -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack /*Advanced*/ \
    -pie \
    -pthread  \
    -finput-charset=UTF-8 -fexec-charset=UTF-8 \
    -Wl,-Bsymbolic \
    /*-Wl,--enable-new-dtags*/ -Wl,-rpath,'$$ORIGIN' \
    -L../../../Binary/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME -lPTMcRT \
    PT_TARGET_ARCH_LDFLAGS \
    PT_DEBUG_LDFLAGS 

#define PT_MODULE Test-ParallelMap

#//STRIP---------------------------------------------------------------------------------------------------------------------

../../../Binary/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/Test-ParallelMap.bundle: \
    ../../../Intermediate/PT_MODULE/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/Test-ParallelMap.bundle \
    PT_MAKEFILE
PT_RECIPEPREFIX \
    PT_STRIP \
        ../../../Intermediate/PT_MODULE/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/Test-ParallelMap.bundle \
        --strip-unneeded \
        -o ../../../Binary/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/Test-ParallelMap.bundle 
       

#//LD---------------------------------------------------------------------------------------------------------------------

../../../Intermediate/PT_MODULE/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/Test-ParallelMap.bundle: \
    ../../../Intermediate/PT_MODULE/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/main.o \
    ../../../Binary/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libPTMcRT.so \
    ../../../Binary/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libunwind.so.1 \
    ../../../Binary/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++abi.so.1 \
    ../../../Binary/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++.so.1 \
    PT_MAKEFILE
PT_RECIPEPREFIX \
    PT_CPP \
        ../../../Intermediate/PT_MODULE/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/main.o \
        PT_LDFLAGS \
        -o ../../../Intermediate/PT_MODULE/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/Test-ParallelMap.bundle

#//CPP--------------------------------------------------------------------------------------------------------------------

../../../Intermediate/PT_MODULE/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/main.o: \
    ../../Test/ParallelMap/main.cpp \
	../../../Runtime/Public/McRT/PTSTaskScheduler.h \
	../../../Runtime/Public/McRT/PT_McRT_Parallel_Map.h \
    PT_MAKEFILE
PT_RECIPEPREFIX \
    PT_CPP -c \
        ../../Test/ParallelMap/main.cpp \
        PT_CPPFLAGS \
        -o ../../../Intermediate/PT_MODULE/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/main.o

#undef PT_CPP

#undef PT_STRIP

#undef PT_CPPFLAGS

#undef PT_LDFLAGS

#undef PT_MODULE

#//ThirdParty--------------------------------------------------------------------------------------------------------------------- 
#ifdef PT_TARGET_ARCH
	#if PT_TARGET_ARCH == 0
		#define PT_LLVM_LIB_NAME libunknown
        #define PT_VULKANSDK_LIB_NAME libunknown
	#elif PT_TARGET_ARCH == 1
		#define PT_LLVM_LIB_NAME libunknown
        #define PT_VULKANSDK_LIB_NAME libunknown
	#elif PT_TARGET_ARCH == 2
		#define PT_LLVM_LIB_NAME lib
        #define PT_VULKANSDK_LIB_NAME lib
	#elif PT_TARGET_ARCH == 3
		#define PT_LLVM_LIB_NAME lib64
        #define PT_VULKANSDK_LIB_NAME lib64
	#else
		#error PT_TARGET_ARCH Unknown
	#endif
#else
	#error PT_TARGET_ARCH Not Defined
#endif

#ifndef PT_LLVM_LIB_NAME
	#error PT_LLVM_LIB_NAME Not Defined
#endif

#ifndef PT_VULKANSDK_LIB_NAME
	#error PT_LLVM_LIB_NAME Not Defined
#endif

../../../Binary/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libunwind.so.1: ../../../ThirdParty/PosixLinuxGlibc/PT_LLVM_LIB_NAME/libunwind.so.1
PT_RECIPEPREFIX cp -f ../../ThirdParty/PosixLinuxGlibc/PT_LLVM_LIB_NAME/libunwind.so.1 ../../../Binary/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libunwind.so.1

../../../Binary/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++abi.so.1: ../../../ThirdParty/PosixLinuxGlibc/PT_LLVM_LIB_NAME/libc++abi.so.1
PT_RECIPEPREFIX cp -f ../../ThirdParty/PosixLinuxGlibc/PT_LLVM_LIB_NAME/libc++abi.so.1 ../../../Binary/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++abi.so.1

../../../Binary/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++.so.1: ../../../ThirdParty/PosixLinuxGlibc/PT_LLVM_LIB_NAME/libc++.so.1
PT_RECIPEPREFIX cp -f ../../ThirdParty/PosixLinuxGlibc/PT_LLVM_LIB_NAME/libc++.so.1 ../../../Binary/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++.so.1

#//
.PHONY: \
    ../../../Binary/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME \
    ../../../Intermediate/Test-ParallelMap/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME 

../../../Binary/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME:
PT_RECIPEPREFIX mkdir -p ../../../Binary/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME

../../../Intermediate/Test-ParallelMap/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME:
PT_RECIPEPREFIX mkdir -p ../../../Intermediate/Test-ParallelMap/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME

