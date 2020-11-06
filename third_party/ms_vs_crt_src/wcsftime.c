/***
*wcsftime.c - String Format Time
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*
*******************************************************************************/


#include <cruntime.h>
#include <internal.h>
#include <mtdll.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <time.h>
#include <dbgint.h>
#include <malloc.h>
#include <errno.h>
#include <locale.h>
#include <setlocal.h>

/* Prototypes for local routines */
static BOOL __cdecl _W_expandtime(
        _locale_t plocinfo,
        wchar_t specifier,
        const struct tm *tmptr,
        wchar_t **out,
        size_t *count,
        struct __lc_time_data *lc_time,
        unsigned alternate_form);

static void __cdecl _W_store_str (wchar_t *in, wchar_t **out, size_t *count);

static void __cdecl _W_store_num (int num, int digits, wchar_t **out, size_t *count,
        unsigned no_lead_zeros);

static void __cdecl _W_store_number (int num, wchar_t **out, size_t *count);

static BOOL __cdecl _W_store_winword (
        _locale_t plocinfo,
        int field_code,
        const struct tm *tmptr,
        wchar_t **out,
        size_t *count,
        struct __lc_time_data *lc_time);

/* Codes for __lc_time_data ww_* fields for _W_store_winword */

#define WW_SDATEFMT     0
#define WW_LDATEFMT     1
#define WW_TIMEFMT      2

#define TIME_SEP        L':'

/*      get a copy of the current day names */
extern "C" wchar_t * __cdecl _W_Getdays_l (
        _locale_t plocinfo
        )
{
    const struct __lc_time_data *pt;
    size_t n, len = 0;
    wchar_t *p;
    _LocaleUpdate _loc_update(plocinfo);

    pt = __LC_TIME_CURR(_loc_update.GetLocaleT()->locinfo);

    for (n = 0; n < 7; ++n)
        len += wcslen(pt->_W_wday_abbr[n]) + wcslen(pt->_W_wday[n]) + 2;
    p = (wchar_t *)_malloc_crt((len + 1) * sizeof(wchar_t));

    if (p != 0) {
        wchar_t *s = p;

        for (n = 0; n < 7; ++n) {
            *s++ = TIME_SEP;
            _ERRCHECK(wcscpy_s(s, (len + 1) - (s - p), pt->_W_wday_abbr[n]));
            s += wcslen(s);
            *s++ = TIME_SEP;
            _ERRCHECK(wcscpy_s(s, (len + 1) - (s - p), pt->_W_wday[n]));
            s += wcslen(s);
        }
        *s++ = L'\0';
    }

    return (p);
}
extern "C" wchar_t * __cdecl _W_Getdays (
        void
        )
{
    return _W_Getdays_l(NULL);
}

/*      get a copy of the current month names */
extern "C" wchar_t * __cdecl _W_Getmonths_l (
        _locale_t plocinfo
        )
{
    const struct __lc_time_data *pt;
    size_t n, len = 0;
    wchar_t *p;
    _LocaleUpdate _loc_update(plocinfo);

    pt = __LC_TIME_CURR(_loc_update.GetLocaleT()->locinfo);

    for (n = 0; n < 12; ++n)
        len += wcslen(pt->_W_month_abbr[n]) + wcslen(pt->_W_month[n]) + 2;
    p = (wchar_t *)_malloc_crt((len + 1) * sizeof(wchar_t));

    if (p != 0) {
        wchar_t *s = p;

        for (n = 0; n < 12; ++n) {
            *s++ = TIME_SEP;
            _ERRCHECK(wcscpy_s(s, (len + 1) - (s - p), pt->_W_month_abbr[n]));
            s += wcslen(s);
            *s++ = TIME_SEP;
            _ERRCHECK(wcscpy_s(s, (len + 1) - (s - p), pt->_W_month[n]));
            s += wcslen(s);
        }
        *s++ = L'\0';
    }

    return (p);
}
extern "C" wchar_t * __cdecl _W_Getmonths (
        void
        )
{
    return _W_Getmonths_l(NULL);
}

/*      get a copy of the current time locale information */
extern "C" void * __cdecl _W_Gettnames_l (
        _locale_t plocinfo
        )
{
    const struct __lc_time_data *pt;
    size_t n, len = 0;
    size_t footprint =
        sizeof(*pt) % sizeof(wchar_t) == 0
        ? sizeof(*pt)
        : (sizeof(*pt) / sizeof(wchar_t) + 1) * sizeof(wchar_t);
    unsigned char *p;
    _LocaleUpdate _loc_update(plocinfo);

    pt = __LC_TIME_CURR(_loc_update.GetLocaleT()->locinfo);

    for (n = 0; n < 7; ++n)
        len += wcslen(pt->_W_wday_abbr[n]) + wcslen(pt->_W_wday[n]) + 2;
    for (n = 0; n < 12; ++n)
        len += wcslen(pt->_W_month_abbr[n]) + wcslen(pt->_W_month[n]) + 2;
    len += wcslen(pt->_W_ampm[0]) + wcslen(pt->_W_ampm[1]) + 2;
    len += wcslen(pt->_W_ww_sdatefmt) + 1;
    len += wcslen(pt->_W_ww_ldatefmt) + 1;
    len += wcslen(pt->_W_ww_timefmt) + 1;

    p = (unsigned char *)_malloc_crt(len * sizeof(wchar_t) + footprint);

    if (p != 0) {
        struct __lc_time_data *pn = (struct __lc_time_data *)p;
        wchar_t *s = (wchar_t *)(p + footprint);
        const wchar_t *olds = s;

        memcpy(pn, pt, sizeof (*pt));
        for (n = 0; n < 7; ++n) {
            pn->_W_wday_abbr[n] = s;
            _ERRCHECK(wcscpy_s(s, len - (s - olds), pt->_W_wday_abbr[n]));
            s += wcslen(s) + 1;
            pn->_W_wday[n] = s;
            _ERRCHECK(wcscpy_s(s, len - (s - olds), pt->_W_wday[n]));
            s += wcslen(s) + 1;
        }
        for (n = 0; n < 12; ++n) {
            pn->_W_month_abbr[n] = s;
            _ERRCHECK(wcscpy_s(s, len - (s - olds), pt->_W_month_abbr[n]));
            s += wcslen(s) + 1;
            pn->_W_month[n] = s;
            _ERRCHECK(wcscpy_s(s, len - (s - olds), pt->_W_month[n]));
            s += wcslen(s) + 1;
        }
        pn->_W_ampm[0] = s;
        _ERRCHECK(wcscpy_s(s, len - (s - olds), pt->_W_ampm[0]));
        s += wcslen(s) + 1;
        pn->_W_ampm[1] = s;
        _ERRCHECK(wcscpy_s(s, len - (s - olds), pt->_W_ampm[1]));
        s += wcslen(s) + 1;
        pn->_W_ww_sdatefmt = s;
        _ERRCHECK(wcscpy_s(s, len - (s - olds), pt->_W_ww_sdatefmt));
        s += wcslen(s) + 1;
        pn->_W_ww_ldatefmt = s;
        _ERRCHECK(wcscpy_s(s, len - (s - olds), pt->_W_ww_ldatefmt));
        s += wcslen(s) + 1;
        pn->_W_ww_timefmt = s;
        _ERRCHECK(wcscpy_s(s, len - (s - olds), pt->_W_ww_timefmt));
    }

    return (p);
}
extern "C" void * __cdecl _W_Gettnames (
        void
        )
{
    return _W_Gettnames_l(NULL);
}

/***
*size_t _Wcsftime(string, maxsize, format,
*       timeptr, lc_time) - Format a time string for a given locale
*
*Purpose:
*       Place characters into the user's output buffer expanding time
*       format directives as described in the user's control string.
*       Use the supplied 'tm' structure for time data when expanding
*       the format directives. Use the locale information at lc_time.
*       [ANSI]
*
*Entry:
*       wchar_t *string = pointer to output string
*       size_t maxsize = max length of string
*       const wchar_t *format = format control string
*       const struct tm *timeptr = pointer to tb data structure
*       struct __lc_time_data *lc_time = pointer to locale-specific info
*           (passed as void * to avoid type mismatch with C++)
*
*Exit:
*       !0 = If the total number of resulting characters including the
*       terminating null is not more than 'maxsize', then return the
*       number of chars placed in the 'string' array (not including the
*       null terminator).
*
*       0 = Otherwise, return 0 and the contents of the string are
*       indeterminate.
*
*Exceptions:
*
*******************************************************************************/

extern "C" size_t __cdecl _Wcsftime_l (
        wchar_t *string,
        size_t maxsize,
        const wchar_t *format,
        const struct tm *timeptr,
        void *lc_time_arg,
        _locale_t plocinfo
        )
{
        unsigned alternate_form;
        struct __lc_time_data *lc_time;
        BOOL failed=FALSE;              /* true if a failure was reported to us */
        size_t left;                    /* space left in output string */
        wchar_t* strstart = string;
        _LocaleUpdate _loc_update(plocinfo);

        _VALIDATE_RETURN( ( string != NULL ), EINVAL, 0)
        _VALIDATE_RETURN( ( maxsize != 0 ), EINVAL, 0)
        *string = L'\0';

        _VALIDATE_RETURN( ( format != NULL ), EINVAL, 0)

// Validated below
//        _VALIDATE_RETURN( ( timeptr != NULL ), EINVAL, 0)


        lc_time = lc_time_arg == 0 ? _loc_update.GetLocaleT()->locinfo->lc_time_curr :
                  (struct __lc_time_data *)lc_time_arg;

        /* Copy maxsize into temp. */
        left = maxsize;

        /* Copy the input string to the output string expanding the format
        designations appropriately.  Stop copying when one of the following
        is true: (1) we hit a null char in the input stream, or (2) there's
        no room left in the output stream. */

        while (left > 0)
        {
            switch(*format)
            {

            case(L'\0'):

                /* end of format input string */
                goto done;

            case(L'%'):

                /* Format directive.  Take appropriate action based
                on format control character. */

                /* validation section */
                _VALIDATE_RETURN(timeptr != NULL, EINVAL, 0);

                format++;                       /* skip over % char */

                /* process flags */
                alternate_form = 0;
                if (*format == L'#')
                {
                    alternate_form = 1;
                    format++;
                }
                if(!_W_expandtime (_loc_update.GetLocaleT(), *format, timeptr, &string,
                             &left,lc_time, alternate_form))
                {
                    /* if we don't have any space left, do not set the failure flag:
                     * we will simply return ERANGE and do not call _invalid_parameter_handler
                     * (see below)
                     */
                    if (left > 0)
                    {
                        failed=TRUE;
                    }
                    goto done;
                }

                format++;                       /* skip format char */
                break;


            default:
                /* store character, bump pointers, dec the char count */
                *string++ = *format++;
                left--;
                break;
            }
        }


        /* All done.  See if we terminated because we hit a null char or because
        we ran out of space */

        done:

        if (!failed && left > 0) {

            /* Store a terminating null char and return the number of chars
            we stored in the output string. */

            *string = L'\0';
            return(maxsize-left);
        }
        else
        {
            /* error - return an empty string */
            *(strstart) = L'\0';

            /* now return our error/insufficient buffer indication */
            if ( !failed && left <= 0 )
            {
                /* do not report this as an error to allow the caller to resize */
                errno=ERANGE;
            }
            else
            {
                _VALIDATE_RETURN( FALSE, EINVAL, 0);
            }
            /* unused but compiler can't tell */
            return 0;
        }
}

extern "C" size_t __cdecl _Wcsftime (
        wchar_t *string,
        size_t maxsize,
        const wchar_t *format,
        const struct tm *timeptr,
        void *lc_time_arg
        )
{
    return _Wcsftime_l(string, maxsize, format, timeptr,
                        lc_time_arg, NULL);
}

/* Some comments on the valid range of tm_year.

   The check which ensures tm_year >= 0 should not be removed from:
     asctime_s
     asctime
     _wasctime_s
     _wasctime
   these function did not handle well negative years in VS 2003 either;
   17 Mar 1861 would be output as "Sun Mar 17 00:00:00 19-'".

   The check which ensures tm_year >= 69 in the mktime family is correct;
   we had the same check in VS 2003; we do not call _invalid_parameter in
   this case, we simply return (-1) to indicate that mktime could not
   transform from struct tm to time_t.

   The check which ensures tm_year >= 0 in _W_expandtime("%y")
   should not be removed (wcsftime calls _W_expandtime);
   _W_expandtime("%y") did not handle well negative years in Everett either;
   1861 would be printed out as "-'";
   with _W_expandtime("%Y"), everything works well if tm_year >= -1900 && tm_year <= 8099.
*/

/***
*_W_expandtime() - Expand the conversion specifier
*
*Purpose:
*       Expand the given wcsftime conversion specifier using the time struct
*       and store it in the supplied buffer.
*
*       The expansion is locale-dependent.
*
*       *** For internal use with wcsftime() only ***
*
*Entry:
*       wchar_t specifier = wcsftime conversion specifier to expand
*       const struct tm *tmptr = pointer to time/date structure
*       wchar_t **string = address of pointer to output string
*       size_t *count = address of char count (space in output area)
*       struct __lc_time_data *lc_time = pointer to locale-specific info
*
*Exit:
*       BOOL true for success, false for failure
*
*Exceptions:
*
*******************************************************************************/

static BOOL __cdecl _W_expandtime (
        _locale_t plocinfo,
        wchar_t specifier,
        const struct tm *timeptr,
        wchar_t **string,
        size_t *left,
        struct __lc_time_data *lc_time,
        unsigned alternate_form
        )
{
        unsigned temp;                  /* temps */
        int wdaytemp;

        /* Use a copy of the appropriate __lc_time_data pointer.  This
        should prevent the necessity of locking/unlocking in mthread
        code (if we can guarantee that the various __lc_time data
        structures are always in the same segment). contents of time
        strings structure can now change, so thus we do use locking */

        switch(specifier) {             /* switch on specifier */

        case(L'a'):              /* abbreviated weekday name */
        {
            _VALIDATE_RETURN( ( ( timeptr->tm_wday >=0 ) && ( timeptr->tm_wday <= 6 ) ), EINVAL, FALSE)
            _W_store_str((wchar_t *)(lc_time->_W_wday_abbr[timeptr->tm_wday]),
                     string, left);
            break;
        }


        case(L'A'):              /* full weekday name */
        {
            _VALIDATE_RETURN( ( ( timeptr->tm_wday >=0 ) && ( timeptr->tm_wday <= 6 ) ), EINVAL, FALSE)
            _W_store_str((wchar_t *)(lc_time->_W_wday[timeptr->tm_wday]),
                     string, left);
            break;
        }

        case(L'b'):              /* abbreviated month name */
        {
            _VALIDATE_RETURN( ( ( timeptr->tm_mon >=0 ) && ( timeptr->tm_mon <= 11 ) ), EINVAL, FALSE)
            _W_store_str((wchar_t *)(lc_time->_W_month_abbr[timeptr->tm_mon]),
                     string, left);
            break;
        }

        case(L'B'):              /* full month name */
        {
            _VALIDATE_RETURN( ( ( timeptr->tm_mon >=0 ) && ( timeptr->tm_mon <= 11 ) ), EINVAL, FALSE)
            _W_store_str((wchar_t *)(lc_time->_W_month[timeptr->tm_mon]),
                     string, left);
            break;
        }

        case(L'c'):              /* date and time display */
            if (alternate_form)
            {
                if(!_W_store_winword( plocinfo,
                                WW_LDATEFMT,
                                timeptr,
                                string,
                                left,
                                lc_time))
                {
                    return FALSE;
                }

                if (*left == 0)
                    return FALSE;
                *(*string)++ = L' ';
                (*left)--;
                if(!_W_store_winword( plocinfo,
                                WW_TIMEFMT,
                                timeptr,
                                string,
                                left,
                                lc_time))
                {
                    return FALSE;
                }

            }
            else {
                if(!_W_store_winword( plocinfo,
                                WW_SDATEFMT,
                                timeptr,
                                string,
                                left,
                                lc_time))
                {
                    return FALSE;
                }
                if (*left == 0)
                    return FALSE;
                *(*string)++ = L' ';
                (*left)--;
                if(!_W_store_winword( plocinfo,
                                WW_TIMEFMT,
                                timeptr,
                                string,
                                left,
                                lc_time))
                {
                    return FALSE;
                }
            }
            break;

        case(L'd'):              /* mday in decimal (01-31) */
            /* pass alternate_form as the no leading zeros flag */
        {
            _VALIDATE_RETURN( ( ( timeptr->tm_mday >=1 ) && ( timeptr->tm_mday <= 31 ) ), EINVAL, FALSE)
            _W_store_num(timeptr->tm_mday, 2, string, left,
                       alternate_form);
            break;
        }

        case(L'H'):              /* 24-hour decimal (00-23) */
            /* pass alternate_form as the no leading zeros flag */
        {
            _VALIDATE_RETURN( ( ( timeptr->tm_hour >=0 ) && ( timeptr->tm_hour <= 23 ) ), EINVAL, FALSE)
            _W_store_num(timeptr->tm_hour, 2, string, left,
                       alternate_form);
            break;
        }

        case(L'I'):              /* 12-hour decimal (01-12) */
        {
            _VALIDATE_RETURN( ( ( timeptr->tm_hour >=0 ) && ( timeptr->tm_hour <= 23 ) ), EINVAL, FALSE)
            if (!(temp = timeptr->tm_hour%12))
                temp=12;
            /* pass alternate_form as the no leading zeros flag */
            _W_store_num(temp, 2, string, left, alternate_form);
            break;
        }

        case(L'j'):              /* yday in decimal (001-366) */
            /* pass alternate_form as the no leading zeros flag */
        {
            _VALIDATE_RETURN( ( ( timeptr->tm_yday >=0 ) && ( timeptr->tm_yday <= 365 ) ), EINVAL, FALSE)
            _W_store_num(timeptr->tm_yday+1, 3, string, left,
                       alternate_form);
            break;
        }

        case(L'm'):              /* month in decimal (01-12) */
            /* pass alternate_form as the no leading zeros flag */
        {
            _VALIDATE_RETURN( ( ( timeptr->tm_mon >=0 ) && ( timeptr->tm_mon <= 11 ) ), EINVAL, FALSE)
            _W_store_num(timeptr->tm_mon+1, 2, string, left,
                       alternate_form);
            break;
        }

        case(L'M'):              /* minute in decimal (00-59) */
            /* pass alternate_form as the no leading zeros flag */
        {
            _VALIDATE_RETURN( ( ( timeptr->tm_min >=0 ) && ( timeptr->tm_min <= 59 ) ), EINVAL, FALSE)
            _W_store_num(timeptr->tm_min, 2, string, left,
                       alternate_form);
            break;
        }

        case(L'p'):              /* AM/PM designation */
        {
            _VALIDATE_RETURN( ( ( timeptr->tm_hour >=0 ) && ( timeptr->tm_hour <= 23 ) ), EINVAL, FALSE)
            if (timeptr->tm_hour <= 11)
                _W_store_str((wchar_t *)(lc_time->_W_ampm[0]), string, left);
            else
                _W_store_str((wchar_t *)(lc_time->_W_ampm[1]), string, left);
            break;
        }

        case(L'S'):              /* secs in decimal (00-59) */
            /* pass alternate_form as the no leading zeros flag */
        {
            _VALIDATE_RETURN( ( ( timeptr->tm_sec >=0 ) && ( timeptr->tm_sec <= 59 ) ), EINVAL, FALSE)
            _W_store_num(timeptr->tm_sec, 2, string, left,
                       alternate_form);
            break;
        }

        case(L'U'):              /* sunday week number (00-53) */
            _VALIDATE_RETURN( ( ( timeptr->tm_wday >=0 ) && ( timeptr->tm_wday <= 6 ) ), EINVAL, FALSE)
            wdaytemp = timeptr->tm_wday;
            goto weeknum;   /* join common code */

        case(L'w'):              /* week day in decimal (0-6) */
            /* pass alternate_form as the no leading zeros flag */
        {
            _VALIDATE_RETURN( ( ( timeptr->tm_wday >=0 ) && ( timeptr->tm_wday <= 6 ) ), EINVAL, FALSE)
            _W_store_num(timeptr->tm_wday, 1, string, left,
                       alternate_form);
            break;
        }

        case(L'W'):              /* monday week number (00-53) */
            _VALIDATE_RETURN( ( ( timeptr->tm_wday >=0 ) && ( timeptr->tm_wday <= 6 ) ), EINVAL, FALSE)
            if (timeptr->tm_wday == 0)  /* monday based */
                wdaytemp = 6;
            else
                wdaytemp = timeptr->tm_wday-1;
        weeknum:
            _VALIDATE_RETURN( ( ( timeptr->tm_yday >=0 ) && ( timeptr->tm_yday <= 365 ) ), EINVAL, FALSE)
            if (timeptr->tm_yday < wdaytemp)
                temp = 0;
            else {
                temp = timeptr->tm_yday/7;
                if ((timeptr->tm_yday%7) >= wdaytemp)
                    temp++;
            }
            /* pass alternate_form as the no leading zeros flag */
            _W_store_num(temp, 2, string, left, alternate_form);
            break;

        case(L'x'):              /* date display */
            if (alternate_form)
            {
                if(!_W_store_winword( plocinfo,
                                WW_LDATEFMT,
                                timeptr,
                                string,
                                left,
                                lc_time))
                {
                    return FALSE;
                }
            }
            else
            {
                if(!_W_store_winword( plocinfo,
                                WW_SDATEFMT,
                                timeptr,
                                string,
                                left,
                                lc_time))
                {
                    return FALSE;
                }
            }
            break;

        case(L'X'):              /* time display */
            if(!_W_store_winword( plocinfo,
                            WW_TIMEFMT,
                            timeptr,
                            string,
                            left,
                            lc_time))
                {
                    return FALSE;
                }
            break;

        case(L'y'):              /* year w/o century (00-99) */
        {
            _VALIDATE_RETURN( ( timeptr->tm_year >=0 ), EINVAL, FALSE)
            temp = timeptr->tm_year%100;
            /* pass alternate_form as the no leading zeros flag */
            _W_store_num(temp, 2, string, left, alternate_form);
            break;
        }

        case(L'Y'):              /* year w/ century */
        {
            _VALIDATE_RETURN( ( timeptr->tm_year >= -1900 ) && ( timeptr->tm_year <= 8099 ), EINVAL, FALSE)
            temp = (((timeptr->tm_year/100)+19)*100) +
                   (timeptr->tm_year%100);
            /* pass alternate_form as the no leading zeros flag */
            _W_store_num(temp, 4, string, left, alternate_form);
            break;
        }

        case(L'Z'):              /* time zone name, if any */
        case(L'z'):              /* time zone name, if any */
            __tzset();      /* Set time zone info */
_BEGIN_SECURE_CRT_DEPRECATION_DISABLE
            {
                size_t wnum = 0;

                _ERRCHECK(_mbstowcs_s_l(&wnum, *string, *left,
                    _tzname[((timeptr->tm_isdst)?1:0)], _TRUNCATE, plocinfo));

                *string += wnum - 1;
                *left -= wnum - 1;
            }
_END_SECURE_CRT_DEPRECATION_DISABLE
            break;

        case(L'%'):              /* percent sign */
            *(*string)++ = L'%';
            (*left)--;
            break;

        case(L'\004'):           /* Workaround issue in older RogueWave libraries */
        case(L'\015'):
            break;

        default:                /* unknown format directive */
            /* ignore the directive and continue */
            /* [ANSI: Behavior is undefined.]    */
            _ASSERTE( ( "Invalid format directive" , 0 ) );
            return FALSE;
            break;

        }       /* end % switch */

        return TRUE;
}


/***
*_W_store_str() - Copy a time string
*
*Purpose:
*       Copy the supplied time string into the output string until
*       (1) we hit a null in the time string, or (2) the given count
*       goes to 0.
*
*       *** For internal use with wcsftime() only ***
*
*Entry:
*       wchar_t *in = pointer to null terminated time string
*       wchar_t **out = address of pointer to output string
*       size_t *count = address of char count (space in output area)
*
*Exit:
*       none
*Exceptions:
*
*******************************************************************************/

static void __cdecl _W_store_str (
        wchar_t *in,
        wchar_t **out,
        size_t *count
        )
{

        while ((*count != 0) && (*in != L'\0')) {
            *(*out)++ = *in++;
            (*count)--;
        }
}


/***
*_W_store_num() - Convert a number to ascii and copy it
*
*Purpose:
*       Convert the supplied number to decimal and store
*       in the output buffer.  Update both the count and
*       buffer pointers.
*
*       *** For internal use with wcsftime() only ***
*
*Entry:
*       int num                 = pointer to integer value
*       int digits              = # of ascii digits to put into string
*       wchar_t **out              = address of pointer to output string
*       size_t *count           = address of char count (space in output area)
*       unsigned no_lead_zeros  = flag indicating that padding by leading
*                                 zeros is not necessary
*
*Exit:
*       none
*Exceptions:
*
*******************************************************************************/

static void __cdecl _W_store_num (
        int num,
        int digits,
        wchar_t **out,
        size_t *count,
        unsigned no_lead_zeros
        )
{
        int temp = 0;

        if (no_lead_zeros) {
            _W_store_number (num, out, count);
            return;
        }

        if ((size_t)digits < *count)  {
            for (digits--; (digits+1); digits--) {
                (*out)[digits] = (wchar_t)(L'0' + num % 10);
                num /= 10;
                temp++;
            }
            *out += temp;
            *count -= temp;
        }
        else
            *count = 0;
}

/***
*_W_store_number() - Convert positive integer to string
*
*Purpose:
*       Convert positive integer to a string and store it in the output
*       buffer with no null terminator.  Update both the count and
*       buffer pointers.
*
*       Differs from _W_store_num in that the precision is not specified,
*       and no leading zeros are added.
*
*       *** For internal use with wcsftime() only ***
*
*       Created from xtoi.c
*
*Entry:
*       int num = pointer to integer value
*       wchar_t **out = address of pointer to output string
*       size_t *count = address of char count (space in output area)
*
*Exit:
*       none
*
*Exceptions:
*       The buffer is filled until it is out of space.  There is no
*       way to tell beforehand (as in _W_store_num) if the buffer will
*       run out of space.
*
*******************************************************************************/

static void __cdecl _W_store_number (
        int num,
        wchar_t **out,
        size_t *count
        )
{
        wchar_t *p;                /* pointer to traverse string */
        wchar_t *firstdig;         /* pointer to first digit */
        wchar_t temp;              /* temp char */

        p = *out;

        /* put the digits in the buffer in reverse order */
        if (*count > 1)
        {
            do {
                *p++ = (wchar_t) (num % 10 + L'0');
                (*count)--;
            } while ((num/=10) > 0 && *count > 1);
        }

        firstdig = *out;                /* firstdig points to first digit */
        *out = p;                       /* return pointer to next space */
        p--;                            /* p points to last digit */

        /* reverse the buffer */
        do {
            temp = *p;
            *p-- = *firstdig;
            *firstdig++ = temp;     /* swap *p and *firstdig */
        } while (firstdig < p);         /* repeat until halfway */
}


/***
*_W_store_winword() - Store date/time in WinWord format
*
*Purpose:
*       Format the date/time in the supplied WinWord format
*       and store it in the supplied buffer.
*
*       *** For internal use with wcsftime() only ***
*
*       For simple localized Gregorian calendars (calendar type 1), the WinWord
*       format is converted token by token to wcsftime conversion specifiers.
*       _W_expandtime is then called to do the work.  The WinWord format is
*       expected to be a wide character string.
*
*       For other calendar types, the Win32 APIs GetDateFormatW/GetTimeFormatW
*       are instead used to do all formatting, so that this routine doesn't
*       have to know about era/period strings, year offsets, etc.
*
*
*Entry:
*       int field_code = code for ww_* field with format
*       const struct tm *tmptr = pointer to time/date structure
*       wchar_t **out = address of pointer to output string
*       size_t *count = address of char count (space in output area)
*       struct __lc_time_data *lc_time = pointer to locale-specific info
*
*Exit:
*       BOOL true for success, false for failure
*
*Exceptions:
*
*******************************************************************************/

static BOOL __cdecl _W_store_winword (
        _locale_t plocinfo,
        int field_code,
        const struct tm *tmptr,
        wchar_t **out,
        size_t *count,
        struct __lc_time_data *lc_time
        )
{
        const wchar_t *format;
        wchar_t specifier;
        const wchar_t *p;
        int repeat;
        wchar_t *ampmstr;
        unsigned no_lead_zeros;

        switch (field_code)
        {
        case WW_SDATEFMT:
            format = lc_time->_W_ww_sdatefmt;
            break;
        case WW_LDATEFMT:
            format = lc_time->_W_ww_ldatefmt;
            break;
        case WW_TIMEFMT:
        default:
            format = lc_time->_W_ww_timefmt;
            break;
        }

        if (lc_time->ww_caltype != 1)
        {
            /* We have something other than the basic Gregorian calendar */

            SYSTEMTIME SystemTime;
            int cch;
            int (WINAPI * FormatFuncW)(LCID, DWORD, const SYSTEMTIME *,
                                      LPCWSTR, LPWSTR, int);

            if (field_code != WW_TIMEFMT)
                FormatFuncW = GetDateFormatW;
            else
                FormatFuncW = GetTimeFormatW;

            /* We leave the verification of SystemTime up to GetDateFormatW or GetTimeFormatW;
               if one of those function returns 0 to indicate error, we will fall through and
               call _W_expandtime() again.
             */
            SystemTime.wYear   = (WORD)(tmptr->tm_year + 1900);
            SystemTime.wMonth  = (WORD)(tmptr->tm_mon + 1);
            SystemTime.wDay    = (WORD)(tmptr->tm_mday);
            SystemTime.wHour   = (WORD)(tmptr->tm_hour);
            SystemTime.wMinute = (WORD)(tmptr->tm_min);
            SystemTime.wSecond = (WORD)(tmptr->tm_sec);
            SystemTime.wMilliseconds = 0;

            /* Find buffer size required */
            cch = FormatFuncW(lc_time->ww_lcid, 0, &SystemTime,
                             format, NULL, 0);

            if (cch != 0)
            {
                wchar_t *buffer;

                /* Allocate buffer, first try stack, then heap */
                buffer = (wchar_t *)_malloca(cch * sizeof(wchar_t));
                if (buffer != NULL)
                {
                    /* Do actual date/time formatting */
                    cch = FormatFuncW(lc_time->ww_lcid, 0, &SystemTime,
                                     format, buffer, cch);

                    /* Copy to output buffer */
                    p = buffer;
                    while (--cch > 0 && *count > 0) {
                        *(*out)++ = *p++;
                        (*count)--;
                    }

                    _freea(buffer);
                    return TRUE;
                }
            }

            /* In case of error, just fall through to localized Gregorian */
        }

        while (*format && *count != 0)
        {
            specifier = 0;          /* indicate no match */
            no_lead_zeros = 0;      /* default is print leading zeros */

            /* count the number of repetitions of this character */
            for (repeat=0, p=format; *p++ == *format; repeat++);
            /* leave p pointing to the beginning of the next token */
            p--;

            /* switch on ascii format character and determine specifier */
            switch (*format)
            {
            case L'M':
                switch (repeat)
                {
                case 1: no_lead_zeros = 1;  /* fall thru */
                case 2: specifier = L'm'; break;
                case 3: specifier = L'b'; break;
                case 4: specifier = L'B'; break;
                } break;
            case L'd':
                switch (repeat)
                {
                case 1: no_lead_zeros = 1;  /* fall thru */
                case 2: specifier = L'd'; break;
                case 3: specifier = L'a'; break;
                case 4: specifier = L'A'; break;
                } break;
            case L'y':
                switch (repeat)
                {
                case 2: specifier = L'y'; break;
                case 4: specifier = L'Y'; break;
                } break;
            case L'h':
                switch (repeat)
                {
                case 1: no_lead_zeros = 1;  /* fall thru */
                case 2: specifier = L'I'; break;
                } break;
            case L'H':
                switch (repeat)
                {
                case 1: no_lead_zeros = 1;  /* fall thru */
                case 2: specifier = L'H'; break;
                } break;
            case L'm':
                switch (repeat)
                {
                case 1: no_lead_zeros = 1;  /* fall thru */
                case 2: specifier = L'M'; break;
                } break;
            case L's': /* for compatibility; not strictly WinWord */
                switch (repeat)
                {
                case 1: no_lead_zeros = 1;  /* fall thru */
                case 2: specifier = L'S'; break;
                } break;
            case L'A':
            case L'a':
                if (!_wcsicmp(format, L"am/pm"))
                    p = format + 5;
                else if (!_wcsicmp(format, L"a/p"))
                    p = format + 3;
                specifier = L'p';
                break;
            case L't': /* t or tt time marker suffix */
                if ( tmptr->tm_hour <= 11 )
                    ampmstr = lc_time->_W_ampm[0];
                else
                    ampmstr = lc_time->_W_ampm[1];

                if ( (repeat == 1) && (*count > 0) ) {
                    *(*out)++ = *ampmstr++;
                    (*count)--;
                } else {
                    while (*ampmstr != 0 && *count > 0) {
                        *(*out)++ = *ampmstr++;
                        (*count)--;
                    }
                }
                format = p;
                continue;

            case L'\'': /* literal string */
                if (repeat & 1) /* odd number */
                {
                    format += repeat;
                    while (*format && *count != 0)
                    {
                        if (*format == L'\'')
                        {
                            format++;
                            break;
                        }

                        *(*out)++ = *format++;
                        (*count)--;
                    }
                }
                else { /* even number */
                    format += repeat;
                }
                continue;

            default: /* non-control char, print it */
                break;
            } /* switch */

            /* expand specifier, or copy literal if specifier not found */
            if (specifier)
            {
                if (!_W_expandtime( plocinfo,
                             specifier,
                             tmptr,
                             out,
                             count,
                             lc_time,
                             no_lead_zeros))
                {
                    return FALSE;
                }
                format = p; /* bump format up to the next token */
            } else {
                *(*out)++ = *format++;
                (*count)--;
            }
        } /* while */

        return TRUE;
}

/***
*size_t wcsftime(wstring, maxsize, wformat, timeptr) - Format a time string
*
*Purpose:
*       The wcsftime function is equivalent to the strftime function, except
*       that the argument 'wstring' specifies an array of a wide string into
*       which the generated output is to be placed.
*       [ISO]
*
*Entry:
*       wchar_t *wstring = pointer to output string
*       size_t maxsize = max length of string
*       const wchar_t *wformat = format control string
*       const struct tm *timeptr = pointer to tb data structure
*
*Exit:
*       !0 = If the total number of resulting characters including the
*       terminating null is not more than 'maxsize', then return the
*       number of wide chars placed in the 'wstring' array (not including the
*       null terminator).
*
*       0 = Otherwise, return 0 and the contents of the string are
*       indeterminate.
*
*Exceptions:
*
*******************************************************************************/

extern "C" size_t __cdecl _wcsftime_l (
        wchar_t *wstring,
        size_t maxsize,
        const wchar_t *wformat,
        const struct tm *timeptr,
        _locale_t plocinfo
        )
{
    return (_Wcsftime_l(wstring, maxsize, wformat, timeptr, 0, plocinfo));
}

extern "C" size_t __cdecl wcsftime (
        wchar_t *wstring,
        size_t maxsize,
        const wchar_t *wformat,
        const struct tm *timeptr
        )
{
    return (_Wcsftime_l(wstring, maxsize, wformat, timeptr, 0, NULL));
}

