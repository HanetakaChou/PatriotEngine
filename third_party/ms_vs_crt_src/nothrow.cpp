// nothrow -- define nothrow object
#ifdef MRTDLL
#undef MRTDLL
#endif

#include <new>
#include <yvals.h>
_STD_BEGIN

const nothrow_t nothrow = nothrow_t();	// define nothrow
_STD_END

/*
 * Copyright (c) 1992-2007 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
 V5.03:0009 */
