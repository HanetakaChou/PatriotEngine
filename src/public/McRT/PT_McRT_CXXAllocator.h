#ifndef PT_MCRT_CXXALLOCATOR_H
#define PT_MCRT_CXXALLOCATOR_H 1

#include "PTSMemoryAllocator.h"

template<typename T>
class PT_McRT_CXXAllocator 
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
		typedef PT_McRT_CXXAllocator<U> other;
	};

	inline PT_McRT_CXXAllocator() throw()
	{

	}

	inline PT_McRT_CXXAllocator(const PT_McRT_CXXAllocator&) throw()
	{

	}

	template<typename U> 
	inline PT_McRT_CXXAllocator(const PT_McRT_CXXAllocator<U>&) throw()
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
class PT_McRT_CXXAllocator<void> {
public:
	typedef void* pointer;
	typedef const void* const_pointer;
	typedef void value_type;
	template<class U> 
	struct rebind 
	{
		typedef PT_McRT_CXXAllocator<U> other;
	};
};

template<typename T, typename U>
inline bool operator==(const PT_McRT_CXXAllocator<T>&, const PT_McRT_CXXAllocator<U>&)
{ 
	return true;
}

template<typename T, typename U>
inline bool operator!=(const PT_McRT_CXXAllocator<T>&, const PT_McRT_CXXAllocator<U>&)
{ 
	return false;
}

#endif