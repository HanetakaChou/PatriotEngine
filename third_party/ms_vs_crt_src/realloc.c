/***
*realloc.c - Reallocate a block of memory in the heap
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Defines the realloc() and _expand() functions.
*
*******************************************************************************/

#include <cruntime.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <winheap.h>
#include <windows.h>
#include <internal.h>
#include <mtdll.h>
#include <dbgint.h>
#include <rtcsup.h>


/***
*void *realloc(pblock, newsize) - reallocate a block of memory in the heap
*
*Purpose:
*       Reallocates a block in the heap to newsize bytes. newsize may be
*       either greater or less than the original size of the block. The
*       reallocation may result in moving the block as well as changing
*       the size. If the block is moved, the contents of the original block
*       are copied over.
*
*       Special ANSI Requirements:
*
*       (1) realloc(NULL, newsize) is equivalent to malloc(newsize)
*
*       (2) realloc(pblock, 0) is equivalent to free(pblock) (except that
*           NULL is returned)
*
*       (3) if the realloc() fails, the object pointed to by pblock is left
*           unchanged
*
*Entry:
*       void *pblock    - pointer to block in the heap previously allocated
*                         by a call to malloc(), realloc() or _expand().
*
*       size_t newsize  - requested size for the re-allocated block
*
*Exit:
*       Success:  Pointer to the re-allocated memory block
*       Failure:  NULL
*
*Uses:
*
*Exceptions:
*       If pblock does not point to a valid allocation block in the heap,
*       realloc() will behave unpredictably and probably corrupt the heap.
*
*******************************************************************************/

void * __cdecl _realloc_base (void * pBlock, size_t newsize)
{
        void *      pvReturn;
        size_t      origSize = newsize;

        //  if ptr is NULL, call malloc
        if (pBlock == NULL)
            return(_malloc_base(newsize));

        //  if ptr is nonNULL and size is zero, call free and return NULL
        if (newsize == 0)
        {
            _free_base(pBlock);
            return(NULL);
        }


        for (;;) {

            pvReturn = NULL;
            if (newsize <= _HEAP_MAXREQ)
            {
                if (newsize == 0)
                    newsize = 1;
                pvReturn = HeapReAlloc(_crtheap, 0, pBlock, newsize);
            }
            else
            {
                _callnewh(newsize);
                errno = ENOMEM;
                return NULL;
            }

            if ( pvReturn || _newmode == 0)
            {
                if (pvReturn)
                {
                    RTCCALLBACK(_RTC_Free_hook, (pBlock, 0));
                    RTCCALLBACK(_RTC_Allocate_hook, (pvReturn, newsize, 0));
                }
                else
                {
                    errno = _get_errno_from_oserr(GetLastError());
                }
                return pvReturn;
            }

            //  call installed new handler
            if (!_callnewh(newsize))
            {
                errno = _get_errno_from_oserr(GetLastError());
                return NULL;
            }

            //  new handler was successful -- try to allocate again
        }
}
