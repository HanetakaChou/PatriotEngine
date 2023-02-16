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

#ifndef _PT_MCRT_ALLOCATOR_H_
#define _PT_MCRT_ALLOCATOR_H_ 1

#include "pt_mcrt_malloc.h"

// Copyright (c) 2005-2019 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// include/tbb/mcrt_allocator.h

#include <new>

// Meets "allocator" requirements of ISO C++ Standard, Section 20.1.5
// The members are ordered the same way they are in section 20.4.1 of the ISO C++ standard.
template <typename T>
class mcrt_allocator
{
    // Class for determining type of std::allocator<T>::value_type.
    template <typename U>
    struct internal_allocator_type
    {
        typedef U value_type;
    };

#if defined(_MSC_VER)
    // Microsoft std::allocator has non-standard extension that strips const from a type.
    template <typename U>
    struct internal_allocator_type<const U>
    {
        typedef U value_type;
    };
#endif

public:
    typedef typename internal_allocator_type<T>::value_type value_type;
    typedef value_type *pointer;
    typedef const value_type *const_pointer;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    template <class U>
    struct rebind
    {
        typedef mcrt_allocator<U> other;
    };

    mcrt_allocator() throw() {}
    mcrt_allocator(const mcrt_allocator &) throw() {}
    template <typename U>
    mcrt_allocator(const mcrt_allocator<U> &) throw() {}

    pointer address(reference x) const { return &x; }
    const_pointer address(const_reference x) const { return &x; }

    // Allocate space for n objects.
    pointer allocate(size_type n, const void * = 0)
    {
        pointer p = static_cast<pointer>(mcrt_aligned_malloc(n * sizeof(value_type), alignof(value_type)));
        return p;
    }

    // Free previously allocated block of memory
    void deallocate(pointer p, size_type)
    {
        mcrt_aligned_free(p);
    }

    // Largest value for which method allocate might succeed.
    size_type max_size() const throw()
    {
        size_type absolutemax = static_cast<size_type>(-1) / sizeof(value_type);
        return (absolutemax > 0 ? absolutemax : 1);
    }

    // ALLOCATOR_CONSTRUCT_VARIADIC
    template <typename U, typename... Args>
    void construct(U *p, Args &&...args)
    {
        ::new ((void *)p) U(std::forward<Args>(args)...);
    }

    // CPP11_RVALUE_REF_PRESENT
    void construct(pointer p, value_type &&value)
    {
        ::new ((void *)(p)) value_type(std::move(value));
    }

    // CPP98 LVALUE
    void construct(pointer p, const value_type &value)
    {
        ::new ((void *)(p)) value_type(value);
    }

    void destroy(pointer p)
    {
        p->~value_type();
    }
};

// Analogous to std::allocator<void>, as defined in ISO C++ Standard, Section 20.4.1
template <>
class mcrt_allocator<void>
{
public:
    typedef void *pointer;
    typedef const void *const_pointer;
    typedef void value_type;
    template <class U>
    struct rebind
    {
        typedef mcrt_allocator<U> other;
    };
};

template <typename T, typename U>
inline bool operator==(const mcrt_allocator<T> &, const mcrt_allocator<U> &)
{
    return true;
}

template <typename T, typename U>
inline bool operator!=(const mcrt_allocator<T> &, const mcrt_allocator<U> &)
{
    return false;
}
#endif