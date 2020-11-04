// newaop -- operator new[](size_t) REPLACEABLE
#include <new>

void *__CRTDECL operator new[](size_t count) _THROW1(std::bad_alloc)
	{	// try to allocate count bytes for an array
	return (operator new(count));
	}

/*
 * Copyright (c) 1992-2007 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
 V5.03:0009 */
