/***
*expand.c - Win32 expand heap routine
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*
*******************************************************************************/

#include <cruntime.h>
#include <malloc.h>
#include <winheap.h>
#include <windows.h>
#include <mtdll.h>
#include <dbgint.h>
#include <rtcsup.h>
#include <internal.h>

// Check if the low fragmentation heap is enabled
static BOOL _is_LFH_enabled ()
{
    LONG heaptype = -1;

    return ((HeapQueryInformation(_crtheap, HeapCompatibilityInformation, &heaptype, sizeof(heaptype), NULL)) && (heaptype == 2));
}

/***
*void *_expand(void *pblck, size_t newsize) - expand/contract a block of memory
*       in the heap
*
*Purpose:
*       Resizes a block in the heap to newsize bytes. newsize may be either
*       greater (expansion) or less (contraction) than the original size of
*       the block. The block is NOT moved.
*
*       NOTES:
*
*       (1) In this implementation, if the block cannot be grown to the
*       desired size, the resulting block will NOT be grown to the max
*       possible size.  (That is, either it works or it doesn't.)
*
*       (2) Unlike other implementations, you can NOT pass a previously
*       freed block to this routine and expect it to work.
*
*Entry:
*       void *pblck - pointer to block in the heap previously allocated
*                 by a call to malloc(), realloc() or _expand().
*
*       size_t newsize  - requested size for the resized block
*
*Exit:
*       Success:  Pointer to the resized memory block (i.e., pblck)
*       Failure:  NULL, errno is set
*
*Uses:
*
*Exceptions:
*       Input parameters are validated. Refer to the validation section of the function.
*
*       If pblck does not point to a valid allocation block in the heap,
*       _expand() will behave unpredictably and probably corrupt the heap.
*
*******************************************************************************/

void * __cdecl _expand_base (void * pBlock, size_t newsize)
{
        void *      pvReturn;

        size_t oldsize;

        /* validation section */
        _VALIDATE_RETURN(pBlock != NULL, EINVAL, NULL);
        if (newsize > _HEAP_MAXREQ) {
            errno = ENOMEM;
            return NULL;
        }

        if (newsize == 0)
        {
            newsize = 1;
        }

        oldsize = (size_t)HeapSize(_crtheap, 0, pBlock);

        pvReturn = HeapReAlloc(_crtheap, HEAP_REALLOC_IN_PLACE_ONLY, pBlock, newsize);

        if (pvReturn == NULL)
        {
            /* If the failure is caused by the use of the LFH, just return the original block. */
            if (oldsize <= 0x4000 /* LFH can only allocate blocks up to 16 KB. */
                    && newsize <= oldsize && _is_LFH_enabled())
                pvReturn = pBlock;
            else
                errno = _get_errno_from_oserr(GetLastError());
        }

        if (pvReturn)
        {
            RTCCALLBACK(_RTC_Free_hook, (pBlock, 0));
            RTCCALLBACK(_RTC_Allocate_hook, (pvReturn, newsize, 0));
        }

        return pvReturn;
}
