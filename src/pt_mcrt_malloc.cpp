//
// Copyright (C) YuqiaoZhang(HanetakaYuminaga)
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

#include <pt_mcrt_malloc.h>
#include <tbb/scalable_allocator.h>

#include <sys/cdefs.h> //__BIONIC__
//#include <features.h> //__GLIBC__

#if defined(PT_MCRT_DEBUG_MALLOC) && PT_MCRT_DEBUG_MALLOC && defined(PT_POSIX_LINUX) && defined(__GLIBC__)
#include <map>
#include <utility>
#include <pt_mcrt_spinlock.h>

#include <dlfcn.h>
#include <execinfo.h>

class mcrt_malloc_verify_support
{
    mcrt_spinlock_t m_spin_lock;
    std::map<void *, void *> m_memory_objects;

public:
    inline mcrt_malloc_verify_support()
    {
        mcrt_spin_init(&this->m_spin_lock);
    }

    inline void insert(void *ptr)
    {
        mcrt_spin_lock(&this->m_spin_lock);
        assert(this->m_memory_objects.end() == this->m_memory_objects.find(ptr));

        void *addrs_ptr[3];
        int num_levels = backtrace(addrs_ptr, 3);
        if (num_levels >= 3)
        {
            this->m_memory_objects.emplace(std::piecewise_construct, std::forward_as_tuple(ptr), std::forward_as_tuple(addrs_ptr[2]));
        }
        else
        {
            this->m_memory_objects.emplace(std::piecewise_construct, std::forward_as_tuple(ptr), std::forward_as_tuple(static_cast<void *>(NULL)));
        }

        mcrt_spin_unlock(&this->m_spin_lock);
    }

    inline void erase(void *ptr)
    {
        mcrt_spin_lock(&this->m_spin_lock);
        auto iter = this->m_memory_objects.find(ptr);
        assert(this->m_memory_objects.end() != iter);
        this->m_memory_objects.erase(iter);
        mcrt_spin_unlock(&this->m_spin_lock);
    }

    inline ~mcrt_malloc_verify_support()
    {
        for (auto const &item : this->m_memory_objects)
        {
            void *object_addr = item.first;
            void *funtion_addr = item.second;
            // set breakpoint here and the debugger can tell you the name of the function by the address
            assert(0);
            object_addr = NULL;
            funtion_addr = NULL;
        }
    }

} instance_mcrt_malloc_verify_support;
#endif

PT_ATTR_MCRT void *PT_CALL mcrt_aligned_malloc(size_t size, size_t alignment)
{
    void *ptr = scalable_aligned_malloc(size, alignment);
#if defined(PT_MCRT_DEBUG_MALLOC) && PT_MCRT_DEBUG_MALLOC && defined(PT_POSIX_LINUX) && defined(__GLIBC__)
    instance_mcrt_malloc_verify_support.insert(ptr);
#endif
    return ptr;
}

PT_ATTR_MCRT void *PT_CALL mcrt_aligned_realloc(void *ptr, size_t size, size_t alignment)
{
    return scalable_aligned_realloc(ptr, size, alignment);
}

PT_ATTR_MCRT void PT_CALL mcrt_aligned_free(void *ptr)
{
#if defined(PT_MCRT_DEBUG_MALLOC) && PT_MCRT_DEBUG_MALLOC && defined(PT_POSIX_LINUX) && defined(__GLIBC__)
    if (NULL != ptr)
    {
        instance_mcrt_malloc_verify_support.erase(ptr);
    }
#endif
    return scalable_aligned_free(ptr);
}
