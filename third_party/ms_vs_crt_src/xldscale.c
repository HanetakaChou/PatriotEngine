/* _LDscale function -- IEEE 754 version */
#include "xmath.h"

#pragma warning(disable:4439)	// C4439: function with a managed parameter must have a __clrcall calling convention

_C_STD_BEGIN

 #if _DLONG == 0
_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _LDscale(long double *px, long lexp)
	{	/* scale *px by 2^lexp with checking -- 64-bit */
	return (_Dscale((double *)px, lexp));
	}

 #elif _DLONG == 1
_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _LDscale(long double *px, long lexp)
	{	/* scale *px by 2^lexp with checking -- 80-bit */
	unsigned short *ps = (unsigned short *)(char *)px;
	short xchar = ps[_L0] & _LMASK;

	if (xchar == _LMAX)
		return ((ps[_L1] & 0x7fff) != 0 || ps[_L2] != 0
			|| ps[_L3] != 0 || ps[_L4] != 0 ? _NANCODE : _INFCODE);
	else if (ps[_L1] == 0 && ps[_L2] == 0
		&& ps[_L3] == 0 && ps[_L4] == 0)
		{	/* zero fraction, clear any exponent */
		ps[_L0] &= _LSIGN;
		return (0);
		}

	if (xchar == 0)
		xchar = 1;	/* correct denormalized exponent */
	xchar += _LDnorm(ps);
	if (0 < lexp && _LMAX - xchar <= lexp)
		{	/* overflow, return +/-INF */
		*px = ps[_L0] & _LSIGN ? -_LInf._Long_double
			: _LInf._Long_double;
		return (_INFCODE);
		}
	else if (-xchar < lexp)
		{	/* finite result, repack */
		ps[_L0] = ps[_L0] & _LSIGN | (lexp + xchar);
		return (_FINITE);
		}
	else
		{	/* denormalized, scale */
		ps[_L0] &= _LSIGN;
		lexp += xchar - 1;
		if (lexp <= -(64 + 1) || 0 <= lexp)
			{	/* underflow, return +/-0 */
			ps[_L1] = 0;
			ps[_L2] = 0;
			ps[_L3] = 0;
			ps[_L4] = 0;
			return (0);
			}
		else
			{	/* nonzero, align fraction */
			short xexp = (short)lexp;
			unsigned short psx = 0;

			for (; xexp <= -16; xexp += 16)
				{	/* scale by words */
				psx = ps[_L4] | (psx != 0 ? 1 : 0);
				ps[_L4] = ps[_L3];
				ps[_L3] = ps[_L2];
				ps[_L2] = ps[_L1];
				ps[_L1] = 0;
				}
			if ((xexp = (short)-xexp) != 0)
				{	/* scale by bits */
				psx = (ps[_L4] << (16 - xexp)) | (psx != 0 ? 1 : 0);
				ps[_L4] = ps[_L4] >> xexp
					| ps[_L3] << 16 - xexp;
				ps[_L3] = ps[_L3] >> xexp
					| ps[_L2] << 16 - xexp;
				ps[_L2] = ps[_L2] >> xexp
					| ps[_L1] << 16 - xexp;
				ps[_L1] >>= xexp;
				}

			if ((0x8000 < psx || 0x8000 == psx && (ps[_L4] & 0x0001) != 0)
				&& (++ps[_L4] & 0xffff) == 0
				&& (++ps[_L3] & 0xffff) == 0
				&& (++ps[_L2] & 0xffff) == 0
				&& (++ps[_L1] & 0xffff) == 0x8000)
				++ps[_L0];	/* round up */
			else if (ps[_L1] == 0 && ps[_L2] == 0
				&& ps[_L3] == 0 && ps[_L4] == 0)
				return (0);
			return (_FINITE);
			}
		}
	}

 #else	/* 1 < _DLONG */
_CRTIMP2_PURE short __CLRCALL_PURE_OR_CDECL _LDscale(long double *px, long lexp)
	{	/* scale *px by 2^lexp with checking -- 128-bit SPARC */
	unsigned short *ps = (unsigned short *)(char *)px;
	short xchar = ps[_L0] & _LMASK;

	if (xchar == _LMAX)
		return (ps[_L1] != 0 || ps[_L2] != 0 || ps[_L3] != 0
			|| ps[_L4] != 0 || ps[_L5] != 0 || ps[_L6] != 0
			|| ps[_L7] != 0 ? _NANCODE : _INFCODE);
	else if (xchar == 0 && 0 < (xchar = _LDnorm(ps)))
		return (0);

	if (0 < lexp && _LMAX - xchar <= lexp)
		{	/* overflow, return +/-INF */
		*px = ps[_L0] & _LSIGN ? -_LInf._Long_double
			: _LInf._Long_double;
		return (_INFCODE);
		}
	else if (-xchar <  lexp)
		{	/* finite result, repack */
		ps[_L0] = ps[_L0] & _LSIGN | (lexp + xchar);
		return (_FINITE);
		}
	else
		{	/* denormalized, scale */
		unsigned short sign = ps[_L0] & _LSIGN;

		ps[_L0] = 1;
		lexp += xchar - 1;
		if (lexp <= -113 || 0 <= lexp)
			{	/* underflow, return +/-0 */
			ps[_L7] = 0;
			ps[_L6] = 0;
			ps[_L5] = 0;
			ps[_L4] = 0;
			ps[_L3] = 0;
			ps[_L2] = 0;
			ps[_L1] = 0;
			ps[_L0] = sign;
			return (0);
			}
		else
			{	/* nonzero, align fraction */
			short xexp = (short)lexp;
			unsigned short psx = 0;

			for (; xexp <= -16; xexp += 16)
				{	/* scale by words */
				psx = ps[_L7] | (psx != 0 ? 1 : 0);
				ps[_L7] = ps[_L6];
				ps[_L6] = ps[_L5];
				ps[_L5] = ps[_L4];
				ps[_L4] = ps[_L3];
				ps[_L3] = ps[_L2];
				ps[_L2] = ps[_L1];
				ps[_L1] = ps[_L0];
				ps[_L0] = 0;
				}
			if ((xexp = (short)-xexp) != 0)
				{	/* scale by bits */
				psx = (ps[_L7] << (16 - xexp)) | (psx != 0 ? 1 : 0);
				ps[_L7] = ps[_L7] >> xexp
					| ps[_L6] << 16 - xexp;
				ps[_L6] = ps[_L6] >> xexp
					| ps[_L5] << 16 - xexp;
				ps[_L5] = ps[_L5] >> xexp
					| ps[_L4] << 16 - xexp;
				ps[_L4] = ps[_L4] >> xexp
					| ps[_L3] << 16 - xexp;
				ps[_L3] = ps[_L3] >> xexp
					| ps[_L2] << 16 - xexp;
				ps[_L2] = ps[_L2] >> xexp
					| ps[_L1] << 16 - xexp;
				ps[_L1] = ps[_L1] >> xexp
					| ps[_L0] << 16 - xexp;
				ps[_L0] = 0;
				}

			ps[_L0] = sign;
			if ((0x8000 < psx || 0x8000 == psx && (ps[_L7] & 0x0001) != 0)
				&& (++ps[_L7] & 0xffff) == 0
				&& (++ps[_L6] & 0xffff) == 0
				&& (++ps[_L5] & 0xffff) == 0
				&& (++ps[_L4] & 0xffff) == 0
				&& (++ps[_L3] & 0xffff) == 0
				&& (++ps[_L2] & 0xffff) == 0
				&& (++ps[_L1] & 0xffff) == 0)
				++ps[_L0];	/* round up */
			else if (ps[_L1] == 0 && ps[_L2] == 0
				&& ps[_L3] == 0 && ps[_L4] == 0
				&& ps[_L5] == 0 && ps[_L6] == 0
				&& ps[_L7] == 0)
				return (0);
			return (_FINITE);
			}
		}
	}
 #endif /* _DLONG */

_C_STD_END

/*
 * Copyright (c) 1992-2007 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
 V5.03:0009 */
