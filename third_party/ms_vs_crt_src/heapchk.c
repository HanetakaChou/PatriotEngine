/***
*heapchk.c - perform a consistency check on the heap
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Defines the _heapchk() and _heapset() functions
*
*******************************************************************************/

#include <cruntime.h>
#include <windows.h>
#include <errno.h>
#include <malloc.h>
#include <mtdll.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <winheap.h>


/***
*int _heapchk()         - Validate the heap
*int _heapset(_fill)    - Obsolete function!
*
*Purpose:
*       Both functions perform a consistency check on the heap. The old
*       _heapset used to fill free blocks with _fill, in addition to
*       performing the consistency check. The current _heapset ignores the
*       passed parameter and just returns _heapchk.
*
*Entry:
*       For heapchk()
*           No arguments
*       For heapset()
*           int _fill - ignored
*
*Exit:
*       Returns one of the following values:
*
*           _HEAPOK         - completed okay
*           _HEAPEMPTY      - heap not initialized
*           _HEAPBADBEGIN   - can't find initial header info
*           _HEAPBADNODE    - malformed node somewhere
*
*       Debug version prints out a diagnostic message if an error is found
*       (see errmsg[] above).
*
*       NOTE:  Add code to support memory regions.
*
*Uses:
*
*Exceptions:
*
*******************************************************************************/

int __cdecl _heapchk (void)
{
        int retcode = _HEAPOK;

        if (!HeapValidate(_crtheap, 0, NULL))
        {
            retcode = _HEAPBADNODE;
        }
        return retcode;
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

int __cdecl _heapset (
        unsigned int _fill
        )
{
        return _heapchk();
}

