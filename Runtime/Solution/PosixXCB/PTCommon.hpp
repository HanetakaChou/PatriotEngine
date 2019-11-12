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
	#elif PT_TARGET_ARCH == 3
		#define PT_TARGET_ARCH_NAME x64
		#define PT_LLVM_LIB_NAME lib64
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
		#define PT_DEBUG_LDFLAGS -g -O0
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