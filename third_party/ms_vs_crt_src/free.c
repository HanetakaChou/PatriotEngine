/***
*free.c - free an entry in the heap
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Defines the following functions:
*           free()     - free a memory block in the heap
*
*******************************************************************************/

#include <cruntime.h>
#include <malloc.h>
#include <winheap.h>
#include <windows.h>
#include <internal.h>
#include <mtdll.h>
#include <dbgint.h>
#include <rtcsup.h>

/***
*void free(pblock) - free a block in the heap
*
*Purpose:
*       Free a memory block in the heap.
*
*       Special ANSI Requirements:
*
*       (1) free(NULL) is benign.
*
*Entry:
*       void *pblock - pointer to a memory block in the heap
*
*Return:
*       <void>
*
*******************************************************************************/

void __cdecl _free_base (void * pBlock)
{

        int retval = 0;


        if (pBlock == NULL)
            return;

        RTCCALLBACK(_RTC_Free_hook, (pBlock, 0));

        retval = HeapFree(_crtheap, 0, pBlock);
        if (retval == 0)
        {
            errno = _get_errno_from_oserr(GetLastError());
        }
}
