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
		#define PT_TARGET_ARCH_CPPFLAGS -m32 -msse3
		#define PT_TARGET_ARCH_LDFLAGS -m32
	#elif PT_TARGET_ARCH == 3
		#define PT_TARGET_ARCH_NAME x64
		#define PT_LLVM_LIB_NAME lib64
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
    PT_TARGET_ARCH_CPPFLAGS \
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
    PT_TARGET_ARCH_LDFLAGS \
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
    ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libPTApp.so \
    ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libPTSystem.so \
    ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libunwind.so.1 \
    ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++abi.so.1 \
    ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++.so.1 \
    PT_MAKEFILE
PT_RECIPEPREFIX \
    PT_CPP \
        ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTWindowImpl.o \
        PT_LDFLAGS \
        -o ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTLauncher.bundle 

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

#if 1 //PTApp

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
        PT_TARGET_ARCH_CPPFLAGS \
        PT_DEBUG_CPPFLAGS

    #define PT_LDFLAGS \
        -fdiagnostics-format=msvc \
        -stdlib=libc++ -lc++ \
        -Wl,--no-undefined /*General*/ \
        -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack /*Advanced*/ \
        -shared -Wl,-soname="libPTApp.so" \
        -pthread  \
        -finput-charset=UTF-8 -fexec-charset=UTF-8 \
        /*-Wl,--enable-new-dtags*/ -Wl,-rpath,'$$ORIGIN' \
        -L../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME -lPTSystem\
        PT_TARGET_ARCH_LDFLAGS \
        PT_DEBUG_LDFLAGS 

    #//STRIP---------------------------------------------------------------------------------------------------------------------

../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libPTApp.so: \
    ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libPTApp.so \
    PT_MAKEFILE
PT_RECIPEPREFIX \
    PT_STRIP \
        ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libPTApp.so \
        --strip-unneeded \
        -o ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libPTApp.so 
       

#//LD---------------------------------------------------------------------------------------------------------------------

../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libPTApp.so: \
    ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTAExport.o \
    ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libPTSystem.so \
    ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libunwind.so.1 \
    ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++abi.so.1 \
    ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++.so.1 \
    PT_MAKEFILE
PT_RECIPEPREFIX \
    PT_CPP \
        ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTAExport.o \
        PT_LDFLAGS \
        -o ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libPTApp.so

#//CPP--------------------------------------------------------------------------------------------------------------------

../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTAExport.o: \
    ../../Private/App/PTAExport.cpp \
    ../../Public/App/PTAExport.h \
    PT_MAKEFILE
PT_RECIPEPREFIX \
    PT_CPP -c \
        ../../Private/App/PTAExport.cpp \
        PT_CPPFLAGS \
        -o ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTAExport.o

#undef PT_CPP

#undef PT_STRIP

#undef PT_CPPFLAGS

#undef PT_LDFLAGS

#endif //PTApp

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
    -DPTSYSTEMAPI=PTEXPORT \
    PT_TARGET_ARCH_CPPFLAGS \
    PT_DEBUG_CPPFLAGS

#define PT_LDFLAGS \
    -fdiagnostics-format=msvc \
    -stdlib=libc++ -lc++ \
    -Wl,--no-undefined /*General*/ \
    -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack /*Advanced*/ \
    -shared -Wl,-soname="libPTSystem.so" \
    -pthread  \
    -finput-charset=UTF-8 -fexec-charset=UTF-8 \
    /*-Wl,--enable-new-dtags*/ -Wl,-rpath,'$$ORIGIN' \
    PT_TARGET_ARCH_LDFLAGS \
    PT_DEBUG_LDFLAGS 

#//STRIP---------------------------------------------------------------------------------------------------------------------

../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libPTSystem.so: \
    ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libPTSystem.so \
    PT_MAKEFILE
PT_RECIPEPREFIX \
    PT_STRIP \
        ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libPTSystem.so \
        --strip-unneeded \
        -o ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libPTSystem.so 
       

#//LD---------------------------------------------------------------------------------------------------------------------

../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libPTSystem.so: \
    ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTSMemory.o \
    ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTSMemoryAllocator.o \
    ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTSTaskSchedulerImpl.o \
    ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libunwind.so.1 \
    ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++abi.so.1 \
    ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++.so.1 \
    PT_MAKEFILE
PT_RECIPEPREFIX \
    PT_CPP \
        ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTSMemory.o \
        ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTSMemoryAllocator.o \
        ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTSTaskSchedulerImpl.o \
        PT_LDFLAGS \
        -o ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libPTSystem.so

#//CPP--------------------------------------------------------------------------------------------------------------------

../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTSMemory.o: \
    ../../Private/System/PTSMemory.cpp \
    ../../Public/System/PTSMemory.h \
    PT_MAKEFILE
PT_RECIPEPREFIX \
    PT_CPP -c \
        ../../Private/System/PTSMemory.cpp \
        PT_CPPFLAGS \
        -o ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTSMemory.o

../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTSMemoryAllocator.o: \
    ../../Private/System/PTSMemoryAllocator.cpp \
    ../../Private/System/Posix/PTSMemoryAllocator.inl \
    ../../Public/System/PTSMemoryAllocator.h \
    PT_MAKEFILE
PT_RECIPEPREFIX \
    PT_CPP -c \
        ../../Private/System/PTSMemoryAllocator.cpp \
        PT_CPPFLAGS \
        -o ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTSMemoryAllocator.o

../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTSTaskSchedulerImpl.o: \
    ../../Private/System/PTSTaskSchedulerImpl.cpp \
    ../../Private/System/Posix/PTSTaskSchedulerImpl.inl \
    ../../Private/System/PTSTaskSchedulerImpl.h \
    ../../Public/System/PTSTaskScheduler.h \
    PT_MAKEFILE
PT_RECIPEPREFIX \
    PT_CPP -c \
        ../../Private/System/PTSTaskSchedulerImpl.cpp \
        PT_CPPFLAGS \
        -o ../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTSTaskSchedulerImpl.o

#undef PT_CPP

#undef PT_STRIP

#undef PT_CPPFLAGS

#undef PT_LDFLAGS

#//ThirdParty--------------------------------------------------------------------------------------------------------------------- 
../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libunwind.so.1: ../../ThirdParty/llvm/PT_LLVM_LIB_NAME/libunwind.so.1
PT_RECIPEPREFIX cp -f ../../ThirdParty/llvm/PT_LLVM_LIB_NAME/libunwind.so.1 ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libunwind.so.1

../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++abi.so.1: ../../ThirdParty/llvm/PT_LLVM_LIB_NAME/libc++abi.so.1
PT_RECIPEPREFIX cp -f ../../ThirdParty/llvm/PT_LLVM_LIB_NAME/libc++abi.so.1 ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++abi.so.1

../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++.so.1: ../../ThirdParty/llvm/PT_LLVM_LIB_NAME/libc++.so.1
PT_RECIPEPREFIX cp -f ../../ThirdParty/llvm/PT_LLVM_LIB_NAME/libc++.so.1 ../../../Binary/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/libc++.so.1