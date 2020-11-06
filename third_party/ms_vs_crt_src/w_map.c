/***
*w_map.c - W version of LCMapString.
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Wrapper for LCMapStringW.
*
*******************************************************************************/

#include <cruntime.h>
#include <internal.h>
#include <stdlib.h>
#include <locale.h>
#include <awint.h>
#include <dbgint.h>
#include <malloc.h>
#include <mtdll.h>
#include <setlocal.h>
#include <string.h>

/***
*int __cdecl __crtLCMapStringW - Get type information about a wide string.
*
*Purpose:
*       Internal support function. Assumes info in wide string format.
*
*Entry:
*       LCID     Locale      - locale context for the comparison.
*       DWORD    dwMapFlags  - see NT\Chicago docs
*       LPCWSTR  lpSrcStr    - pointer to string to be mapped
*       int      cchSrc      - wide char (word) count of input string
*                              (including NULL if any)
*                              (-1 if NULL terminated)
*       LPWSTR   lpDestStr   - pointer to memory to store mapping
*       int      cchDest     - wide char (word) count of buffer (including NULL)
*
*       NOTE:    if LCMAP_SORTKEY is specified, then cchDest refers to number
*                of BYTES, not number of wide chars. The return string will be
*                a series of bytes with a NULL byte terminator.
*
*Exit:
*       Success: if LCMAP_SORKEY:
*                   number of bytes written to lpDestStr (including NULL byte
*                   terminator)
*               else
*                   number of wide characters written to lpDestStr (including
*                   NULL)
*       Failure: 0
*
*Exceptions:
*
*******************************************************************************/

int __cdecl __crtLCMapStringW(
        LCID     Locale,
        DWORD    dwMapFlags,
        LPCWSTR  lpSrcStr,
        int      cchSrc,
        LPWSTR   lpDestStr,
        int      cchDest
        )
{
    /*
     * LCMapString will map past NULL. Must find NULL if in string
     * before cchSrc wide characters.
     */
    if (cchSrc > 0)
        cchSrc = (int) wcsnlen(lpSrcStr, cchSrc);

    return LCMapStringW( Locale, dwMapFlags, lpSrcStr, cchSrc,
                         lpDestStr, cchDest );
}
