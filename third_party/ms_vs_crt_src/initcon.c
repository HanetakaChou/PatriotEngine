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

void __cdecl __termconout(void);

_CRTALLOC(".CRT$XPX") static  _PVFV pterm = __termconout;

/*
 * define console handles. these definitions cause this file to be linked
 * in if one of the direct console I/O functions is referenced.
 * The value (-2) is used to indicate the un-initialized state.
 */
intptr_t _confh = -2;       /* console output */


/***
*void __initconout(void) - open handles for console output
*
*Purpose:
*       Opens handle for console output.
*
*Entry:
*       None.
*
*Exit:
*       No return value. If successful, the handle value is copied into the
*       global variable _confh.  Otherwise _confh is set to -1.
*
*Exceptions:
*
*******************************************************************************/

void __cdecl __initconout (
        void
        )
{
    _confh = (intptr_t) CreateFileW(L"CONOUT$",
                                    GENERIC_WRITE,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    NULL,
                                    OPEN_EXISTING,
                                    0,
                                    NULL);
}


/***
*void __termconout(void) - close console output handle
*
*Purpose:
*       Closes _confh.
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

void __cdecl __termconout (
        void
        )
{
    if ((_confh != -1) && (_confh != -2)) {
        CloseHandle((HANDLE) _confh);
    }
}
