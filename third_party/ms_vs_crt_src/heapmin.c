/***
*heapmin.c - Minimize the heap
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Minimize the heap freeing as much memory as possible back
*       to the OS.
*
*******************************************************************************/

#include <cruntime.h>
#include <windows.h>
#include <errno.h>
#include <malloc.h>
#include <mtdll.h>
#include <stdlib.h>
#include <winheap.h>

/***
*_heapmin() - Minimize the heap
*
*Purpose:
*       Minimize the heap freeing as much memory as possible back
*       to the OS.
*
*Entry:
*       (void)
*
*Exit:
*
*        0 = no error has occurred
*       -1 = an error has occurred (errno is set)
*
*Exceptions:
*
*******************************************************************************/

int __cdecl _heapmin(void)
{
        if ( HeapCompact( _crtheap, 0 ) == 0 ) {
            return -1;
        }
        else {
            return 0;
        }
}
