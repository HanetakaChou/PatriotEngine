/* _Getwctype -- return character classification flags for wide character */
#include <xlocinfo.h>
#include <wchar.h>
#include <awint.h>
#include <yvals.h>

_C_STD_BEGIN
_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _Getwctype(wchar_t _Ch,
	const _Ctypevec *_Ctype)
	{	/* return character classification flags for _Ch */
	short _Mask;
	return ((short)(GetStringTypeW(CT_CTYPE1, &_Ch, 1,
		(LPWORD)&_Mask) == 0
		? 0 : _Mask));
	}

_CRTIMP2_PURE const wchar_t * __CLRCALL_PURE_OR_CDECL _Getwctypes(
	const wchar_t *_First, const wchar_t *_Last,
		short *_Dest, const _Ctypevec *_Ctype)
	{	/* get mask sequence for elements in [_First, _Last) */
	GetStringTypeW(CT_CTYPE1, _First, (int)(_Last - _First),
		(LPWORD)_Dest);
	return (_Last);
	}

#ifdef MRTDLL
_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _Getwctype(unsigned short _Ch,
	const _Ctypevec *_Ctype)
    {
    return _Getwctype((wchar_t) _Ch, _Ctype);
    }

_CRTIMP2_PURE const unsigned short * __CLRCALL_PURE_OR_CDECL _Getwctypes(
	const unsigned short *_First, const unsigned short *_Last,
		short *_Dest, const _Ctypevec *_Ctype)
    {
    return (const unsigned short *)_Getwctypes((wchar_t *)_First, (wchar_t *) _Last,
                       _Dest, _Ctype);
    }
#endif

_C_STD_END

/*
 * Copyright (c) 1992-2007 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
 V5.03:0009 */
