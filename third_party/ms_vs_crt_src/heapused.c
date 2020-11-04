/***
*heapused.c -
*
*   Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*
*******************************************************************************/

#include <cruntime.h>
#include <malloc.h>
#include <errno.h>

size_t __cdecl _heapused(
        size_t *pUsed,
        size_t *pCommit
        )
{
        errno = ENOSYS;
        return( 0 );
}
