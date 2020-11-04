/***
*msize.c - calculate the size of a memory block in the heap
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Defines the following function:
*           _msize()    - calculate the size of a block in the heap
*
*******************************************************************************/

#include <cruntime.h>
#include <malloc.h>
#include <mtdll.h>
#include <winheap.h>
#include <windows.h>
#include <dbgint.h>
#include <internal.h>

/***
*size_t _msize(pblock) - calculate the size of specified block in the heap
*
*Purpose:
*       Calculates the size of memory block (in the heap) pointed to by
*       pblock.
*
*Entry:
*       void *pblock - pointer to a memory block in the heap
*
*Return:
*       size of the block
*
*Exceptions:
*       Input parameters are validated. Refer to the validation section of the function.
*
*******************************************************************************/

size_t __cdecl _msize_base (void * pblock)
{
        size_t      retval;

        /* validation section */
        _VALIDATE_RETURN(pblock != NULL, EINVAL, -1);


        retval = (size_t)HeapSize(_crtheap, 0, pblock);

        return retval;

}
