/***
*mbsnbcpy_s_l.c - Copy one string to another, n bytes only (MBCS)
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Copy one string to another, n bytes only (MBCS)
*
*******************************************************************************/

#ifdef _MBCS

#include <mbstring.h>
#include <internal_securecrt.h>
#include <mtdll.h>
#include <setlocal.h>

#define _FUNC_PROLOGUE
#define _FUNC_NAME _mbsnbcpy_s_l
#define _SB_FUNC_NAME strncpy_s
#define _DEST _Dst
#define _SRC _Src
#define _COUNT _CountInBytes
#define _COUNT_IN_BYTES 1

#include <mbsncpy_s.inl>

#endif  /* _MBCS */
