#ifndef PT_SYSTEM_MEMORYALLOCATOR_H
#define PT_SYSTEM_MEMORYALLOCATOR_H

#include "../PTCommon.h"
#include "PTSCommon.h"

#include <stddef.h>
#include <stdint.h>

extern "C" PTMCRTAPI bool PTCALL PTSMemoryAllocator_Initialize();

extern "C" PTMCRTAPI void * PTCALL PTSMemoryAllocator_Alloc(uint32_t Size);

extern "C" PTMCRTAPI void PTCALL PTSMemoryAllocator_Free(void *pVoid);

extern "C" PTMCRTAPI void * PTCALL PTSMemoryAllocator_Alloc_Aligned(uint32_t Size, uint32_t Alignment);

extern "C" PTMCRTAPI void PTCALL PTSMemoryAllocator_Free_Aligned(void *pVoid);

extern "C" PTMCRTAPI uint32_t PTCALL PTSMemoryAllocator_Size(void *pVoid);

extern "C" PTMCRTAPI void * PTCALL PTSMemoryAllocator_Realloc(void *pVoid, uint32_t Size);

extern "C" PTMCRTAPI void * PTCALL PTSMemoryAllocator_Realloc_Aligned(void *pVoid, uint32_t Size, uint32_t Alignment);

//McRT(multi-core runtime)

inline void *PT_McRT_Malloc(uint32_t Size)
{
	return PTSMemoryAllocator_Alloc(Size);
}

inline void PT_McRT_Free(void *pVoid)
{
	return PTSMemoryAllocator_Free(pVoid);
}

inline void *PT_McRT_Aligned_Malloc(uint32_t Size, uint32_t Alignment)
{
	return PTSMemoryAllocator_Alloc_Aligned(Size, Alignment);
}

inline void PT_McRT_Aligned_Free(void *pVoid)
{
	return PTSMemoryAllocator_Free_Aligned(pVoid);
}

#include <memory>
#include <new>

template<typename T>
class PTS_CPP_Allocator 
{
public:
	typedef typename std::allocator<T>::value_type value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	template<class U> 
	struct rebind 
	{
		typedef PTS_CPP_Allocator<U> other;
	};

	inline PTS_CPP_Allocator() throw()
	{

	}

	inline PTS_CPP_Allocator(const PTS_CPP_Allocator&) throw()
	{

	}

	template<typename U> 
	inline PTS_CPP_Allocator(const PTS_CPP_Allocator<U>&) throw()
	{

	}

	inline pointer address(reference x) const 
	{ 
		return &x;
	}

	inline const_pointer address(const_reference x) const
	{
		return &x; 
	}

	inline pointer allocate(size_type n, const void * = 0)
	{
		pointer p = static_cast<pointer>(::PTSMemoryAllocator_Alloc(static_cast<uint32_t>(sizeof(value_type)*n)));
		assert(p != NULL);
		return p;
	}

	inline void deallocate(pointer p, size_type)
	{
		::PTSMemoryAllocator_Free(p);
	}

	inline size_type max_size() const throw()
	{
		size_type absolutemax = static_cast<size_type>(-1) / sizeof(value_type);
		return (absolutemax > 0 ? absolutemax : 1);
	}

	inline void construct(pointer p, const value_type& value)
	{ 
		::new((void*)(p)) value_type(value); 
	}
	
	void destroy(pointer p) 
	{ 
		p->~value_type(); 
	}
};

template<>
class PTS_CPP_Allocator<void> {
public:
	typedef void* pointer;
	typedef const void* const_pointer;
	typedef void value_type;
	template<class U> 
	struct rebind 
	{
		typedef PTS_CPP_Allocator<U> other;
	};
};

template<typename T, typename U>
inline bool operator==(const PTS_CPP_Allocator<T>&, const PTS_CPP_Allocator<U>&)
{ 
	return true;
}

template<typename T, typename U>
inline bool operator!=(const PTS_CPP_Allocator<T>&, const PTS_CPP_Allocator<U>&)
{ 
	return false;
}

#include <vector>
#include <string>
#include <set>
#include <map>

template <typename T>
using PTS_CPP_Vector = std::vector<T, ::PTS_CPP_Allocator<T>>;

using PTS_CPP_String = std::basic_string<char, std::char_traits<char>, ::PTS_CPP_Allocator<char>>;

template <typename Key, typename Compare = std::less<Key >>
using PTS_CPP_Set = std::set<Key, Compare, ::PTS_CPP_Allocator<Key>>;

template <typename Key, typename Compare = std::less<Key >>
using PTS_CPP_MultiSet = std::multiset<Key, Compare, ::PTS_CPP_Allocator<Key>>;

template <typename Key, typename T, typename Compare = std::less<Key>>
using PTS_CPP_Map = std::map<Key, T, Compare, ::PTS_CPP_Allocator<std::pair<const Key, T>>>;

template <typename Key, typename T, typename Compare = std::less<Key>>
using PTS_CPP_MultiMap = std::multimap<Key, T, Compare, ::PTS_CPP_Allocator<std::pair<const Key, T>>>;

#endif