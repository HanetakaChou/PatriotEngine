/* _Stold function */
#include <stdlib.h>
#include "xmath.h"
#include "xxcctype.h"
#include "xxlftype.h"
_C_STD_BEGIN

#if defined(__cplusplus) && !defined(MRTDLL)
extern "C"
{
#endif
_CRTIMP2_PURE double __CLRCALL_PURE_OR_CDECL _Stodx(const char *s, char **endptr, long pten, int *perr);

_CRTIMP2_PURE double __CLRCALL_PURE_OR_CDECL _Stod(const char *s, char **endptr, long pten);

_CRTIMP2_PURE long double __CLRCALL_PURE_OR_CDECL _Stoldx(const char *s, char **endptr, long pten, int *perr)
	{	/* convert string to long double */
	return ((long double)_Stodx(s, endptr, pten, perr));
	}

_CRTIMP2_PURE long double __CLRCALL_PURE_OR_CDECL _Stold(const char *s, char **endptr, long pten, int *perr)
	{	/* convert string to long double, discard error code */
	return ((long double)_Stod(s, endptr, pten));
	}

#if defined(__cplusplus) && !defined(MRTDLL)
}
#endif

_C_STD_END

/*
 * Copyright (c) 1992-2004 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
 V4.04:0009 */
