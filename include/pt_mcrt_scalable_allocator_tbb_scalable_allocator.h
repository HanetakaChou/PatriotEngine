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

/** @file include/tbb/scalable_allocator.h */

#if defined(_PT_MCRT_SCALABLE_ALLOCATOR_H_) && !defined(_PT_MCRT_SCALABLE_ALLOCATOR_TBB_SCALABLE_ALLOCATOR_H_)
#define _PT_MCRT_SCALABLE_ALLOCATOR_TBB_SCALABLE_ALLOCATOR_H_ 1

#include <new>

namespace mcrt
{
    //! @cond INTERNAL
    namespace internal
    {

#if __PT_MCRT_USE_EXCEPTIONS
        // forward declaration is for inlining prevention
        template <typename E>
        __PT_MCRT_NOINLINE(void throw_exception(const E &e));
#endif

        // keep throw in a separate function to prevent code bloat
        template <typename E>
        void throw_exception(const E &e)
        {
            __PT_MCRT_THROW(e);
        }

    } // namespace internal
    //! @endcond

    //! Meets "allocator" requirements of ISO C++ Standard, Section 20.1.5
    /** The members are ordered the same way they are in section 20.4.1
    of the ISO C++ standard.
    @ingroup memory_allocation */
    template <typename T>
    class scalable_allocator
    {
    public:
        typedef typename internal::allocator_type<T>::value_type value_type;
        typedef value_type *pointer;
        typedef const value_type *const_pointer;
        typedef value_type &reference;
        typedef const value_type &const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        template <class U>
        struct rebind
        {
            typedef scalable_allocator<U> other;
        };

        scalable_allocator() throw() {}
        scalable_allocator(const scalable_allocator &) throw() {}
        template <typename U>
        scalable_allocator(const scalable_allocator<U> &) throw() {}

        pointer address(reference x) const { return &x; }
        const_pointer address(const_reference x) const { return &x; }

        //! Allocate space for n objects.
        pointer allocate(size_type n, const void * /*hint*/ = 0)
        {
            pointer p = static_cast<pointer>(mcrt_aligned_malloc(n * sizeof(value_type), alignof(value_type)));
            if (!p)
                internal::throw_exception(std::bad_alloc());
            return p;
        }

        //! Free previously allocated block of memory
        void deallocate(pointer p, size_type)
        {
            mcrt_free(p);
        }

        //! Largest value for which method allocate might succeed.
        size_type max_size() const throw()
        {
            size_type absolutemax = static_cast<size_type>(-1) / sizeof(value_type);
            return (absolutemax > 0 ? absolutemax : 1);
        }
#if __PT_MCRT_ALLOCATOR_CONSTRUCT_VARIADIC
        template <typename U, typename... Args>
        void construct(U *p, Args &&... args)
        {
            ::new ((void *)p) U(std::forward<Args>(args)...);
        }
#else /* __PT_MCRT_ALLOCATOR_CONSTRUCT_VARIADIC */
#if __PT_MCRT_CPP11_RVALUE_REF_PRESENT
        void construct(pointer p, value_type &&value)
        {
            ::new ((void *)(p)) value_type(std::move(value));
        }
#endif
        void construct(pointer p, const value_type &value)
        {
            ::new ((void *)(p)) value_type(value);
        }
#endif /* __PT_MCRT_CPP11_RVALUE_REF_PRESENT */
        void destroy(pointer p)
        {
            p->~value_type();
        }
    };

    //! Analogous to std::allocator<void>, as defined in ISO C++ Standard, Section 20.4.1
    /** @ingroup memory_allocation */
    template <>
    class scalable_allocator<void>
    {
    public:
        typedef void *pointer;
        typedef const void *const_pointer;
        typedef void value_type;
        template <class U>
        struct rebind
        {
            typedef scalable_allocator<U> other;
        };
    };

    template <typename T, typename U>
    inline bool operator==(const scalable_allocator<T> &, const scalable_allocator<U> &) { return true; }

    template <typename T, typename U>
    inline bool operator!=(const scalable_allocator<T> &, const scalable_allocator<U> &) { return false; }
} // namespace mcrt

#else
#error "Never use <pt_mcrt_scalable_allocator_tbb_tbb_config.h> directly; include <pt_mcrt_scalable_allocator.h> instead."
#endif