/***
*llabs.c - find absolute value of a long long integer
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines llabs() - find absolute value of a long long integer.
*
*******************************************************************************/

#include <cruntime.h>
#include <stdlib.h>

#if _MSC_FULL_VER >= 160020117LL
#pragma function(llabs)
#endif  /* _MSC_FULL_VER >= 160020117LL */

/***
*long long llabs(lnumber) - find absolute value of long long.
*
*Purpose:
*       Find the absolute value of a long long integer (lnumber if lnumber >= 0),
*       -lnumber if lnumber < 0).
*
*Entry:
*       long long lnumber - number to find absolute value of
*
*Exit:
*       returns the absolute value of lnumber
*
*Exceptions:
*
*******************************************************************************/

long long __cdecl llabs (
        long long lnumber
        )
{
        return( lnumber>=0LL ? lnumber : -lnumber );
}
