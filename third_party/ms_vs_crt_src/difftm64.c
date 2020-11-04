/***
*difftm64.c - return difference between two times as a double
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Find difference between two time in seconds.
*
*******************************************************************************/


#include <cruntime.h>
#include <time.h>
#include <internal.h>

/***
*double _difftime64(b, a) - find difference between two times
*
*Purpose:
*       returns difference between two times (b-a)
*
*Entry:
*       __time64_t a, b - times to difference
*
*Exit:
*       returns a double with the time in seconds between two times
*       0 if input is invalid
*
*Exceptions:
*
*******************************************************************************/

double __cdecl _difftime64 (
        __time64_t b,
        __time64_t a
        )
{
        _VALIDATE_RETURN_NOEXC(
            ( ( a >= 0 ) && ( b >= 0 ) ),
            EINVAL,
            0
        )

        return( (double)( b - a ) );
}

