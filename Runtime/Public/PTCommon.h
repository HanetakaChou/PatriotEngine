#ifndef PT_COMMON_H
#define PT_COMMON_H

#if defined(_MSC_VER) //Compiler
//https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros

//Platform
#define PTWIN32 1
#if defined(WINAPI_FAMILY_APP) || defined(WINAPI_FAMILY_DESKTOP_APP)
//Has Include <winapifamily.h>
#if (WINAPI_FAMILY==WINAPI_FAMILY_DESKTOP_APP)
#define PTWIN32DESKTOP 1
#elif (WINAPI_FAMILY==WINAPI_FAMILY_APP)
#define PTWIN32RUNTIME 1
#endif
#else
//Has Not Include <winapifamily.h>
#if defined(WINAPI_FAMILY)
//WINAPI_FAMILY=WINAPI_FAMILY_APP
#define PTWIN32RUNTIME 1
#else
#define PTWIN32DESKTOP 1
#endif
#endif

//Architecture
#if defined(_M_IX86)
#define PTX86 1
#elif defined(_M_X64)
#define PTX64 1
#elif defined(_M_ARM)
#define PTARM 1
#elif defined(_M_ARM64)
#define PTARM64 1
#else
#error Unknown Architecture //未知的架构
#endif

//CompileAndLink
#define PTCALL __stdcall
#define PTPTR __stdcall
#define PTIMPORT __declspec(dllimport)
#define PTEXPORT __declspec(dllexport)

#elif defined(__GNUC__)
//https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html

//Platform
#define PTPOSIX 1

#if defined(__linux__)
#define PTPOSIXLINUX 1
#if defined(__ANDROID__)
#define PTPOSIXLINUXBIONIC 1
#else
#define PTPOSIXLINUXGLIBC 1
#endif
#elif defined(__MACH__) 
//https://developer.apple.com/library/archive/documentation/Porting/Conceptual/PortingUnix/compiling/compiling.html
#define PTPOSIXMACH 1
#else
#error Unknown Platform //未知的平台
#endif

//Architecture //Mach-O Support Universal Binary, We Must Depend On The Compiler Predefined Macros To Infer The Architecture.
#if defined(__x86_64__)
#define PTX64 1 //Compiler May Define Both __x86_64__ and __i386__ Because Of Bugs.
#elif defined(__i386__)
#define PTX86 1
#elif defined(__arm__)
#define PTARM 1
#elif defined(__aarch64__)
#define PTARM64 1
#else
#error Unknown Architecture //未知的架构
#endif

//CompileAndLink
#define PTCALL
#define PTPTR
#define PTIMPORT
#define PTEXPORT __attribute__((visibility("default")))

#else
#error Unknown Compiler //未知的编译器
#endif

#endif
