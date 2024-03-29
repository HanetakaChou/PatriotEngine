//
// Copyright (C) YuqiaoZhang(HanetakaChou)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#ifndef _PT_COMMON_H_
#define _PT_COMMON_H_ 1

#if defined(__GNUC__)
//https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html

//Compiler
#define PT_GCC 1

//Platform
#if defined(__linux__)
#define PT_POSIX 1
#define PT_POSIX_LINUX 1
#ifdef __ANDROID__
#define PT_POSIX_LINUX_ANDROID 1
#else
#define PT_POSIX_LINUX_X11 1
#endif
#elif defined(__MACH__)
//https://developer.apple.com/library/archive/documentation/Porting/Conceptual/PortingUnix/compiling/compiling.html
#define PT_POSIX 1
#define PT_POSIX_MACH 1
//#include <TargetConditionals.h>
#if __is_target_os(ios) //TARGET_OS_IPHONE
#define PT_POSIX_MATH_IOS 1
#elif __is_target_os(macos) //TARGET_OS_OSX
#define PT_POSIX_MATH_OSX 1
#else
#error Unknown Mach Platform
#endif
#else
#error Unknown Platform
#endif

//Architecture //Mach-O Support Universal Binary, We Must Depend On The Compiler Predefined Macros To Infer The Architecture.
#if defined(__x86_64__)
#define PT_X64 1 //Compiler May Define Both __x86_64__ and __i386__ Because Of Bugs.
#elif defined(__i386__)
#define PT_X86 1
#elif defined(__aarch64__)
#define PT_ARM64 1
#elif defined(__arm__) //&& defined(__thumb__)
#define PT_ARM 1
#else
#error Unknown Architecture
#endif

//Module
#define PT_CALL
#define PT_PTR
#define PT_ATTR_IMPORT
#define PT_ATTR_EXPORT __attribute__((visibility("default")))

//Likely
#define PT_LIKELY(x) __builtin_expect(!!(x), 1)
#define PT_UNLIKELY(x) __builtin_expect(!!(x), 0)

#define PT_MAYBE_UNUSED __attribute__((unused))

#define PT_ALIGN_AS(x) __attribute__((aligned(x)))

#define PT_ALWAYS_INLINE __attribute__((always_inline))

#elif defined(_MSC_VER)
//https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros

#define PT_MSVC 1

//Platform
#if defined(_WINRT_DLL)
#define PT_WIN32 1
#define PT_WIN32_RUNTIME 1
#elif defined(_WIN32) || defined(_WIN64)
#define PT_WIN32 1
#define PT_WIN32_DESKTOP 1
#else
#error Unknown Platform
#endif

//Architecture
#if defined(_M_X64)
#define PT_X64 1
#elif defined(_M_IX86)
#define PT_X86 1
#elif defined(_M_ARM64)
#define PT_ARM64 1
#elif defined(_M_ARM)
#define PT_ARM 1
#else
#error Unknown Architecture
#endif

//Module
#define PT_CALL __stdcall
#define PT_PTR __stdcall
#define PT_ATTR_IMPORT __declspec(dllimport)
#define PT_ATTR_EXPORT __declspec(dllexport)

//Likely
#define PT_LIKELY(x) (!!(x))
#define PT_UNLIKELY(x) (!!(x))

#define PT_MAYBE_UNUSED 

#define PT_ALIGN_AS(x) __declspec(align(x))

#define PT_ALWAYS_INLINE __declspec(__forceinline)

#else
#error Unknown Compiler
#endif

#endif