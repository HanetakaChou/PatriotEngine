/*
    Copyright (c) 2005-2019 Intel Corporation

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/** @file include/tbb/tbb_config.h */

#if defined(_PT_MCRT_SCALABLE_ALLOCATOR_H_) && !defined(_PT_MCRT_SCALABLE_ALLOCATOR_TBB_TBB_CONFIG_H_)
#define _PT_MCRT_SCALABLE_ALLOCATOR_TBB_TBB_CONFIG_H_ 1

#if defined(__GNUC__)

#define __PT_MCRT_USE_EXCEPTIONS __EXCEPTIONS

// Note that when Clang is in use, __PT_MCRT_GCC_VERSION might not fully match
// the actual GCC version on the system.
#define __PT_MCRT_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

// Prior to GCC 7, GNU libstdc++ did not have a convenient version macro.
// Therefore we use different ways to detect its version.
#if defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE != __GNUC__
// Reported versions of GCC and libstdc++ do not match; trust the latter
#define __PT_MCRT_GLIBCXX_VERSION (_GLIBCXX_RELEASE * 10000)
#elif __GLIBCPP__ || __GLIBCXX__
// The version macro is not defined or matches the GCC version; use __TBB_GCC_VERSION
#define __PT_MCRT_GLIBCXX_VERSION __PT_MCRT_GCC_VERSION
#endif

#if defined(__clang__)
#define __PT_MCRT_CPP11_VARIADIC_TEMPLATES_PRESENT __has_feature(__cxx_variadic_templates__)
#define __PT_MCRT_CPP11_RVALUE_REF_PRESENT (__has_feature(__cxx_rvalue_references__) && (_LIBCPP_VERSION || __PT_MCRT_GLIBCXX_VERSION >= 40500))
#else
#define __PT_MCRT_CPP11_VARIADIC_TEMPLATES_PRESENT __GXX_EXPERIMENTAL_CXX0X__
#define __PT_MCRT_CPP11_RVALUE_REF_PRESENT (__GXX_EXPERIMENTAL_CXX0X__ && __PT_MCRT_GLIBCXX_VERSION >= 40500)
#endif

#elif defined(_MSC_VER)

#define __PT_MCRT_USE_EXCEPTIONS (defined(_CPPUNWIND))

#define __PT_MCRT_CPP11_VARIADIC_TEMPLATES_PRESENT (_MSC_VER >= 1800)
#define __PT_MCRT_CPP11_RVALUE_REF_PRESENT (_MSC_VER >= 1700)

#else
#error Unknown Compiler
#endif

#define __PT_MCRT_ALLOCATOR_CONSTRUCT_VARIADIC (__PT_MCRT_CPP11_VARIADIC_TEMPLATES_PRESENT && __PT_MCRT_CPP11_RVALUE_REF_PRESENT)

#else
#error "Never use <pt_mcrt_scalable_allocator_tbb_tbb_config.h> directly; include <pt_mcrt_scalable_allocator.h> instead."
#endif