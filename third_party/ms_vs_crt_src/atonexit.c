/***
*atonexit.c - _onexit/atexit for using the MSVCRT* model of C run-time
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       atexit and _onexit are handled differently for EXE's and DLL's linked
*       with MSVCRT.LIB to call MSVCRT*.DLL.  Specifically, the _onexit/atexit
*       list for DLL's must be maintained on a per-module basis and must be
*       processed at DLL process detach .  For EXE's the atexit/_onexit list
*       should be maintained by MSVCRT*.DLL and processed at process exit.
*
*******************************************************************************/

/*
 * SPECIAL BUILD MACRO! Note that atonexit.c is linked in with the client's
 * code. It does not go into crtdll.dll! Therefore, it must be built under
 * the _DLL switch (like user code) and CRTDLL must be undefined.
 */

#undef  CRTDLL
#ifndef _DLL
#define _DLL
#endif  /* _DLL */

#include <cruntime.h>
#include <oscalls.h>
#include <internal.h>
#include <stdlib.h>
#include <mtdll.h>

/*
 * Pointers to beginning and end of the table of function pointers manipulated
 * by _onexit()/atexit().  If this module is an EXE, _onexitbegin will be -1.
 * Otherwise _onexitbegin will point to a block of malloc-ed memory used to
 * maintain the DLL module's private onexit list of terminator routines.
 * NOTE - the pointers are stored encoded.
 */

_PVFV *__onexitbegin;
_PVFV *__onexitend;


/***
*_onexit, atexit - calls to _onexit & atexit in MSVCRT*.DLL
*
*Purpose:
*       A DLL linked with MSVCRT.LIB must not call the standard _onexit or
*       atexit exported from MSVCRT*.DLL, but an EXE linked with MSVCRT.LIB
*       will call the standard versions of those two routines.  The standard
*       names are not exported from MSVCRT*.DLL, but the _imp__* names are,
*       so this module can just invoke the standard versions if linked into
*       an EXE module (indicated by __onexitbegin == -1).  If this module is
*       linked into a DLL (indicated by __onexitbegin != -1), it will call a
*       helper routine in MSVCRT*.DLL called __dllonexit to manage the DLL's
*       private onexit list.
*
*Entry:
*       Same as the regular versions of _onexit, atexit.
*
*Exit:
*       Same as the regular versions of _onexit, atexit.
*
*Exceptions:
*
*******************************************************************************/

extern _onexit_t __dllonexit(_onexit_t, _PVFV**, _PVFV**);

#ifdef _M_IX86
extern _onexit_t (__cdecl *_imp___onexit)(_onexit_t func);
#else  /* _M_IX86 */
extern _onexit_t (__cdecl *__imp__onexit)(_onexit_t func);
#endif  /* _M_IX86 */

_onexit_t __cdecl _onexit (
        _onexit_t func
        )
{
        _PVFV * onexitbegin;
        _PVFV * onexitend;
        _onexit_t retval;

        onexitbegin = (_PVFV *) DecodePointer(__onexitbegin);

        if (onexitbegin == (_PVFV *)-1)
        {
            /* EXE */
#ifdef _M_IX86
            return (*_imp___onexit)(func);
#else  /* _M_IX86 */
            return (*__imp__onexit)(func);
#endif  /* _M_IX86 */
        }

        /*
         * Note that we decode/encode the onexit array pointers on the
         * client side, not the CRT DLL side, to ease backwards compatibility.
         * That means we have to take a lock on this side, making the lock
         * found in __dllonexit redundant.
         */

        _lock(_EXIT_LOCK1);

        __try
        {
            onexitbegin = (_PVFV *) DecodePointer(__onexitbegin);
            onexitend   = (_PVFV *) DecodePointer(__onexitend);

            retval = __dllonexit((_onexit_t) EncodePointer(func), &onexitbegin, &onexitend);

            __onexitbegin = (_PVFV *) EncodePointer(onexitbegin);
            __onexitend   = (_PVFV *) EncodePointer(onexitend);
        }
        __finally
        {
            _unlock(_EXIT_LOCK1);
        }

        return retval;
}

int __cdecl atexit (
        _PVFV func
        )
{
        return (_onexit((_onexit_t)func) == NULL) ? -1 : 0;
}
