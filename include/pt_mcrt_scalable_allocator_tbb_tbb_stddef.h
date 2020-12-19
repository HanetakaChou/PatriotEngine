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

/** @file include/tbb/tbb_stddef.h */

#ifndef _PT_MCRT_SCALABLE_ALLOCATOR_TBB_TBB_STDDEF_H_
#define _PT_MCRT_SCALABLE_ALLOCATOR_TBB_TBB_STDDEF_H_ 1

#if defined(__GNUC__)
#define __PT_MCRT_NOINLINE(decl) decl __attribute__((noinline))
#elif defined(_MSC_VER)
#define __PT_MCRT_NOINLINE(decl) __declspec(noinline) decl
#else
#error Unknown Compiler
#endif

#if __PT_MCRT_USE_EXCEPTIONS
#define __PT_MCRT_THROW(e) throw e
#else /* !__PT_MCRT_USE_EXCEPTIONS */
#define __PT_MCRT_THROW(e) mcrt::internal::suppress_unused_warning(e)
#endif /* !__PT_MCRT_USE_EXCEPTIONS */

namespace mcrt
{
    namespace internal
    {

        //! Utility template function to prevent "unused" warnings by various compilers.
        template <typename T1>
        void suppress_unused_warning(const T1 &) {}

        //! Class for determining type of std::allocator<T>::value_type.
        template <typename T>
        struct allocator_type
        {
            typedef T value_type;
        };

#if defined(_MSC_VER)
        //! Microsoft std::allocator has non-standard extension that strips const from a type.
        template <typename T>
        struct allocator_type<const T>
        {
            typedef T value_type;
        };
#endif
    } // namespace internal
} // namespace mcrt

#endif