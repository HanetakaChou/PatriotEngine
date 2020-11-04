/***
*memcmp.c - compare two blocks of memory
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines memcmp() - compare two memory blocks lexically and
*       find their order.
*
*******************************************************************************/

#include <cruntime.h>
#include <string.h>

#pragma function(memcmp)

/***
*int memcmp(buf1, buf2, count) - compare memory for lexical order
*
*Purpose:
*       Compares count bytes of memory starting at buf1 and buf2
*       and find if equal or which one is first in lexical order.
*
*Entry:
*       void *buf1, *buf2 - pointers to memory sections to compare
*       size_t count - length of sections to compare
*
*Exit:
*       returns < 0 if buf1 < buf2
*       returns  0  if buf1 == buf2
*       returns > 0 if buf1 > buf2
*
*Exceptions:
*
*******************************************************************************/

int __cdecl memcmp (
        const void * buf1,
        const void * buf2,
        size_t count
        )
{
        if (!count)
                return(0);

#if defined (_M_AMD64)

    {


        __declspec(dllimport)


        size_t RtlCompareMemory( const void * src1, const void * src2, size_t length );

        size_t length;

        if ( ( length = RtlCompareMemory( buf1, buf2, count ) ) == count ) {
            return(0);
        }

        buf1 = (char *)buf1 + length;
        buf2 = (char *)buf2 + length;
    }

#else  /* defined (_M_AMD64) */

        while ( --count && *(char *)buf1 == *(char *)buf2 ) {
                buf1 = (char *)buf1 + 1;
                buf2 = (char *)buf2 + 1;
        }

#endif  /* defined (_M_AMD64) */

        return( *((unsigned char *)buf1) - *((unsigned char *)buf2) );
}
