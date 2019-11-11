#define _PT_QUOTE(i) #i
#define PT_QUOTE(i) _PT_QUOTE(i)

#ifdef PT_TARGET_ARCH
	#if PT_TARGET_ARCH == 0
		#define PT_TARGET_ARCH_NAME ARM
	#elif PT_TARGET_ARCH == 1
		#define PT_TARGET_ARCH_NAME ARM64
	#elif PT_TARGET_ARCH == 2
		#define PT_TARGET_ARCH_NAME x86
	#elif PT_TARGET_ARCH == 3
		#define PT_TARGET_ARCH_NAME x64
	#else
		#error PT_TARGET_ARCH Unknown
	#endif
#else
	#error PT_TARGET_ARCH Not Defined
#endif

#ifdef PT_DEBUG
	#if PT_DEBUG == 0
		#define PT_DEBUG_NAME Release
		#define PT_DEBUG_CPPFLAGS -O3
		#define PT_DEBUG_LDFLAGS -O3
	#else
		#define PT_DEBUG_NAME Debug
		#define PT_DEBUG_CPPFLAGS -g -O0
		#define PT_DEBUG_LDFLAGS -O0
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

#define PT_GENERAL_CPPFLAGS -x c++ -std=c++11 -pthread -fpie -stdlib=libc++ -fvisibility=hidden
#define PT_GENERAL_LDFLAGS -x c++ -std=c++11 -pthread  -pie -stdlib=libc++ -lxcb -lxcb-keysyms -Wl,-rpath,"$ORIGIN" //-Wl,--enable-new-dtags

#define PT_CPP ../../ThirdParty/llvm/bin/clang++

#define PT_CPPFLAGS PT_GENERAL_CPPFLAGS PT_DEBUG_CPPFLAGS
#define PT_LDFLAGS PT_GENERAL_LDFLAGS PT_DEBUG_LDFLAGS

.RECIPEPREFIX = $

#define PT_RECIPEPREFIX $

PT_QUOTE(../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTWindowImpl.o): PT_QUOTE(../../Private/Window/PosixXCB/PTWindowImpl.cpp)
PT_RECIPEPREFIX PT_CPP -c PT_QUOTE(../../Private/Window/PosixXCB/PTWindowImpl.cpp) PT_CPPFLAGS -o PT_QUOTE(../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME/PTWindowImpl.o)

.PHONY:
PT_RECIPEPREFIX mkdir -p PT_QUOTE(../../../Intermediate/PTLauncher/PT_TARGET_ARCH_NAME/PT_DEBUG_NAME)

PT_QUOTE(../../Private/Window/PosixXCB/PTWindowImpl.cpp):

#if 0
PT_LDFLAGS = -x c++ -std=c++11 -pthread  -pie -stdlib=libc++ -lxcb -lxcb-keysyms -Wl,-rpath,"$ORIGIN" //-Wl,--enable-new-dtags

ifndef PT_DEBUG
	
endif

ifndef PT_DEBUG
	
endif

ifneq ($(PT_DEBUG),0)
PT_CPPFLAGS := $(PT_CPPFLAGS) -g -O0
else
PT_CPPFLAGS := $(PT_CPPFLAGS) -o3
endif

$(PT_TARGET_ARCH):
	echo $(PT_CPPFLAGS)
#endif