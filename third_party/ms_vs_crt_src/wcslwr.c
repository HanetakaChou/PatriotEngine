/***
*wcslwr.c - routine to map upper-case characters in a wchar_t string
*       to lower-case
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Converts all the upper case characters in a wchar_t string
*       to lower case, in place.
*
*******************************************************************************/


#include <cruntime.h>
#include <string.h>
#include <malloc.h>
#include <locale.h>
#include <ctype.h>
#include <awint.h>
#include <dbgint.h>
#include <internal.h>
#include <internal_securecrt.h>
#include <mtdll.h>
#include <setlocal.h>

/***
*wchar_t *_wcslwr(string) - map upper-case characters in a string to lower-case
*
*Purpose:
*       wcslwr converts upper-case characters in a null-terminated wchar_t
*       string to their lower-case equivalents.  The result may be longer or
*       shorter than the original string.  Assumes enough space in string
*       to hold the result.
*
*Entry:
*       wchar_t *wsrc - wchar_t string to change to lower case
*
*Exit:
*       input string address
*
*Exceptions:
*       on an error, the original string is unaltered, and errno is set
*
*******************************************************************************/

extern "C" wchar_t * __cdecl _wcslwr_l (
        wchar_t * wsrc,
        _locale_t plocinfo
        )
{
    _wcslwr_s_l(wsrc, (size_t)(-1), plocinfo);
    return wsrc;
}

extern "C" wchar_t * __cdecl _wcslwr (
        wchar_t * wsrc
        )
{
    if (__locale_changed == 0)
    {
        wchar_t * p;

        /* validation section */
        _VALIDATE_RETURN(wsrc != NULL, EINVAL, NULL);

        for (p=wsrc; *p; ++p)
        {
            if (L'A' <= *p && *p <= L'Z')
                *p += (wchar_t)L'a' - (wchar_t)L'A';
        }
    } else {
        _wcslwr_s_l(wsrc, (size_t)(-1), NULL);
        return wsrc;
    }

    return(wsrc);
}

/***
*errno_t _wcslwr_s(string, size_t) - map upper-case characters in a string to lower-case
*
*Purpose:
*       wcslwr_s converts upper-case characters in a null-terminated wchar_t
*       string to their lower-case equivalents.  The result may be longer or
*       shorter than the original string.
*
*Entry:
*       wchar_t *wsrc - wchar_t string to change to lower case
*       size_t sizeInWords - size of the destination buffer
*
*Exit:
*       the error code
*
*Exceptions:
*       on an error, the original string is unaltered, and errno is set
*
*******************************************************************************/

static errno_t __cdecl _wcslwr_s_l_stat (
        wchar_t * wsrc,
        size_t sizeInWords,
        _locale_t plocinfo
        )
{

    wchar_t *p;             /* traverses string for C locale conversion */
    wchar_t *wdst;          /* wide version of string in alternate case */
    int dstsize;            /* size in wide chars of wdst string buffer (include null) */
    errno_t e = 0;
    size_t stringlen;

    /* validation section */
    _VALIDATE_RETURN_ERRCODE(wsrc != NULL, EINVAL);
    stringlen = wcsnlen(wsrc, sizeInWords);
    if (stringlen >= sizeInWords)
    {
        _RESET_STRING(wsrc, sizeInWords);
        _RETURN_DEST_NOT_NULL_TERMINATED(wsrc, sizeInWords);
    }
    _FILL_STRING(wsrc, sizeInWords, stringlen + 1);

    if ( plocinfo->locinfo->lc_handle[LC_CTYPE] == _CLOCALEHANDLE)
    {
        for ( p = wsrc ; *p ; p++ )
        {
            if ( (*p >= (wchar_t)L'A') && (*p <= (wchar_t)L'Z') )
            {
                *p -= L'A' - L'a';
            }
        }

        return 0;
    }   /* C locale */

    /* Inquire size of wdst string */
    if ( (dstsize = __crtLCMapStringW(
                    plocinfo->locinfo->lc_handle[LC_CTYPE],
                    LCMAP_LOWERCASE,
                    wsrc,
                    -1,
                    NULL,
                    0
                    )) == 0 )
    {
        errno = EILSEQ;
        return errno;
    }

    if (sizeInWords < (size_t)dstsize)
    {
        _RESET_STRING(wsrc, sizeInWords);
        _RETURN_BUFFER_TOO_SMALL(wsrc, sizeInWords);
    }

    /* Allocate space for wdst */
    wdst = (wchar_t *)_calloca(dstsize, sizeof(wchar_t));
    if (wdst == NULL)
    {
        errno = ENOMEM;
        return errno;
    }

    /* Map wrc string to wide-character wdst string in alternate case */
    if (__crtLCMapStringW(
                plocinfo->locinfo->lc_handle[LC_CTYPE],
                LCMAP_LOWERCASE,
                wsrc,
                -1,
                wdst,
                dstsize
                ) != 0)
    {
        /* Copy wdst string to user string */
        e = wcscpy_s(wsrc, sizeInWords, wdst);
    }
    else
    {
        e = errno = EILSEQ;
    }

    _freea(wdst);

    return e;
}

extern "C" errno_t __cdecl _wcslwr_s_l (
        wchar_t * wsrc,
        size_t sizeInWords,
        _locale_t plocinfo
        )
{
    _LocaleUpdate _loc_update(plocinfo);

    return _wcslwr_s_l_stat(wsrc, sizeInWords, _loc_update.GetLocaleT());
}


extern "C" errno_t __cdecl _wcslwr_s (
        wchar_t * wsrc,
        size_t sizeInWords
        )
{
    return _wcslwr_s_l(wsrc, sizeInWords, NULL);
}
