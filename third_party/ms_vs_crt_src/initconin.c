/***
*initcon.c - direct console I/O initialization and termination for Win32
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Defines __initconin() and _initconout() and __termcon() routines.
*       The first two are called on demand to initialize _coninpfh and
*       _confh, and the third is called indirectly by CRTL termination.
*
*       NOTE:   The __termcon() routine is called indirectly by the C/C++
*               Run-Time Library termination code.
*
*******************************************************************************/

#include <sect_attribs.h>
#include <cruntime.h>
#include <internal.h>
#include <oscalls.h>

void __cdecl __termconin(void);

_CRTALLOC(".CRT$XPX") static  _PVFV pterm = __termconin;

/*
 * define console handles. these definitions cause this file to be linked
 * in if one of the direct console I/O functions is referenced.
 * The value (-2) is used to indicate the un-initialized state.
 */
intptr_t _coninpfh = -2;    /* console input */


/***
*void __initconin(void) - open handles for console input
*
*Purpose:
*       Opens handle for console input.
*
*Entry:
*       None.
*
*Exit:
*       No return value. If successful, the handle value is copied into the
*       global variable _coninpfh.  Otherwise _coninpfh is set to -1.
*
*Exceptions:
*
*******************************************************************************/

void __cdecl __initconin (
        void
        )
{
    _coninpfh = (intptr_t) CreateFileW(L"CONIN$",
                                       GENERIC_READ | GENERIC_WRITE,
                                       FILE_SHARE_READ | FILE_SHARE_WRITE,
                                       NULL,
                                       OPEN_EXISTING,
                                       0,
                                       NULL);
}


/***
*void __termconin(void) - close console input handle
*
*Purpose:
*       Closes _coninpfh
*
*Entry:
*       None.
*
*Exit:
*       No return value.
*
*Exceptions:
*
*******************************************************************************/

void __cdecl __termconin (
        void
        )
{
    if ((_coninpfh != -1) && (_coninpfh != -2)) {
        CloseHandle((HANDLE) _coninpfh);
    }
}
