/***
*putwch.c - write a wide character to console
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines _putwch() - writes a wide character to a console
*
*******************************************************************************/


#include <stdlib.h>
#include <conio.h>
#include <io.h>
#include <errno.h>
#include <cruntime.h>
#include <stdio.h>
#include <dbgint.h>
#include <file2.h>
#include <internal.h>
#include <mtdll.h>
#include <limits.h>

/*
 * declaration for console handle
 */
extern intptr_t _confh;

/***
*wint_t _putwch(ch) - write a wide character to a console
*
*Purpose:
*       Writes a wide character to a console.
*
*Entry:
*       wchar_t ch - wide character to write
*
*Exit:
*       returns the wide character if successful
*       returns WEOF if fails
*
*Exceptions:
*
*******************************************************************************/

wint_t _CRTIMP __cdecl _putwch (
        wchar_t ch
        )
{
        REG2 wint_t retval;

        _mlock(_CONIO_LOCK);
        __try {

        retval = _putwch_nolock(ch);

        }
        __finally {
                _munlock(_CONIO_LOCK);
        }

        return(retval);
}

/***
*_putwch_nolock() -  _putwch() core routine (locked version)
*
*Purpose:
*       Core _putwch() routine; assumes stream is already locked.
*
*       [See _putwch() above for more info.]
*
*Entry: [See _putwch()]
*
*Exit:  [See _putwch()]
*
*Exceptions:
*
*******************************************************************************/

wint_t __cdecl _putwch_nolock (
        wchar_t ch
        )
{
    DWORD cchWritten;

    if (_confh == -2)
        __initconout();

    if (_confh == -1)
        return WEOF;

    /* write character to console file handle */

    if (!WriteConsoleW((HANDLE) _confh,
                       &ch,
                       1,
                       &cchWritten,
                       NULL))
    {
        return WEOF;
    }

    return ch;
}

