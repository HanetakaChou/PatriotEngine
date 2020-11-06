/***
*heapadd.c - Add a block of memory to the heap
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Add a block of memory to the heap.
*
*******************************************************************************/

#include <cruntime.h>
#include <errno.h>
#include <malloc.h>
#include <winheap.h>

int __cdecl _heapadd (
        void * block,
        size_t size
        )
{
        errno = ENOSYS;
        return(-1);
}
