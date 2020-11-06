/***
*inittime.c - contains __init_time
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Contains the locale-category initialization function: __init_time().
*
*       Each initialization function sets up locale-specific information
*       for their category, for use by functions which are affected by
*       their locale category.
*
*       *** For internal use by setlocale() only ***
*
*******************************************************************************/

#include <stdlib.h>
#include <windows.h>
#include <locale.h>
#include <setlocal.h>
#include <malloc.h>
#include <dbgint.h>
#include <mtdll.h>

static int __cdecl _get_lc_time(struct __lc_time_data *lc_time, pthreadlocinfo);
void __cdecl __free_lc_time(struct __lc_time_data *lc_time);

/* C locale time strings */
extern struct __lc_time_data __lc_time_c;

/***
*int __init_time() - initialization for LC_TIME locale category.
*
*Purpose:
*       In non-C locales, read the localized time/date strings into
*       __lc_time_intl.  The old __lc_time_intl is not freed until
*       the new one is fully established.
*
*       Any allocated __lc_time_intl structures are freed.
*
*Entry:
*       None.
*
*Exit:
*       0 success
*       1 fail
*
*Exceptions:
*
*******************************************************************************/

int __cdecl __init_time (
        pthreadlocinfo ploci
        )
{
    /* Temporary date/time strings */
    struct __lc_time_data *lc_time;

    if ( ploci->lc_handle[LC_TIME] != _CLOCALEHANDLE )
    {
            /* Allocate structure filled with NULL pointers */
            if ( (lc_time = (struct __lc_time_data *)
                 _calloc_crt(1, sizeof(struct __lc_time_data))) == NULL )
                    return 1;

            if (_get_lc_time(lc_time, ploci))
            {
                    __free_lc_time (lc_time);
                    _free_crt (lc_time);
                    return 1;
            }
            lc_time->refcount = 1;
    } else {
            lc_time = &__lc_time_c;      /* point to new one */
    }
    if (ploci->lc_time_curr != &__lc_time_c &&
        InterlockedDecrement(&(ploci->lc_time_curr->refcount)) == 0)
    {
        _ASSERTE(ploci->lc_time_curr->refcount > 0);
    }
    ploci->lc_time_curr = lc_time;           /* point to new one */
    return 0;
}

/*
 *  Get the localized time strings.
 *  Of course, this can be beautified with some loops!
 */
static int __cdecl _get_lc_time (
        struct __lc_time_data *lc_time,
        pthreadlocinfo ploci
        )
{
        int ret = 0;
        _locale_tstruct locinfo;

        /* Some things are language-dependent and some are country-dependent.
        This works around an NT limitation and lets us distinguish the two. */

        LCID langid = MAKELCID(ploci->lc_id[LC_TIME].wLanguage, SORT_DEFAULT);
        LCID ctryid = MAKELCID(ploci->lc_id[LC_TIME].wCountry, SORT_DEFAULT);

        if (lc_time == NULL)
                return -1;

        locinfo.locinfo = ploci;
        locinfo.mbcinfo = 0;

        /* All the text-strings are Language-dependent: */

        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SABBREVDAYNAME1, (void *)&lc_time->wday_abbr[1]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SABBREVDAYNAME2, (void *)&lc_time->wday_abbr[2]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SABBREVDAYNAME3, (void *)&lc_time->wday_abbr[3]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SABBREVDAYNAME4, (void *)&lc_time->wday_abbr[4]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SABBREVDAYNAME5, (void *)&lc_time->wday_abbr[5]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SABBREVDAYNAME6, (void *)&lc_time->wday_abbr[6]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SABBREVDAYNAME7, (void *)&lc_time->wday_abbr[0]);

        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SDAYNAME1, (void *)&lc_time->wday[1]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SDAYNAME2, (void *)&lc_time->wday[2]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SDAYNAME3, (void *)&lc_time->wday[3]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SDAYNAME4, (void *)&lc_time->wday[4]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SDAYNAME5, (void *)&lc_time->wday[5]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SDAYNAME6, (void *)&lc_time->wday[6]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SDAYNAME7, (void *)&lc_time->wday[0]);

        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SABBREVMONTHNAME1, (void *)&lc_time->month_abbr[0]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SABBREVMONTHNAME2, (void *)&lc_time->month_abbr[1]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SABBREVMONTHNAME3, (void *)&lc_time->month_abbr[2]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SABBREVMONTHNAME4, (void *)&lc_time->month_abbr[3]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SABBREVMONTHNAME5, (void *)&lc_time->month_abbr[4]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SABBREVMONTHNAME6, (void *)&lc_time->month_abbr[5]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SABBREVMONTHNAME7, (void *)&lc_time->month_abbr[6]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SABBREVMONTHNAME8, (void *)&lc_time->month_abbr[7]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SABBREVMONTHNAME9, (void *)&lc_time->month_abbr[8]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SABBREVMONTHNAME10, (void *)&lc_time->month_abbr[9]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SABBREVMONTHNAME11, (void *)&lc_time->month_abbr[10]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SABBREVMONTHNAME12, (void *)&lc_time->month_abbr[11]);

        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SMONTHNAME1, (void *)&lc_time->month[0]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SMONTHNAME2, (void *)&lc_time->month[1]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SMONTHNAME3, (void *)&lc_time->month[2]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SMONTHNAME4, (void *)&lc_time->month[3]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SMONTHNAME5, (void *)&lc_time->month[4]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SMONTHNAME6, (void *)&lc_time->month[5]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SMONTHNAME7, (void *)&lc_time->month[6]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SMONTHNAME8, (void *)&lc_time->month[7]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SMONTHNAME9, (void *)&lc_time->month[8]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SMONTHNAME10, (void *)&lc_time->month[9]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SMONTHNAME11, (void *)&lc_time->month[10]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_SMONTHNAME12, (void *)&lc_time->month[11]);

        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_S1159, (void *)&lc_time->ampm[0]);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, langid, LOCALE_S2359, (void *)&lc_time->ampm[1]);


/* The following relate to time format and are Country-dependent: */

        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, ctryid, LOCALE_SSHORTDATE, (void *)&lc_time->ww_sdatefmt);
        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, ctryid, LOCALE_SLONGDATE, (void *)&lc_time->ww_ldatefmt);

        ret |= __getlocaleinfo(&locinfo, LC_STR_TYPE, ctryid, LOCALE_STIMEFORMAT, (void *)&lc_time->ww_timefmt);

        ret |= __getlocaleinfo(&locinfo, LC_INT_TYPE, ctryid, LOCALE_ICALENDARTYPE, (void *)&lc_time->ww_caltype);

        lc_time->ww_lcid = ctryid;


        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SABBREVDAYNAME1, &lc_time->_W_wday_abbr[1]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SABBREVDAYNAME2, &lc_time->_W_wday_abbr[2]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SABBREVDAYNAME3, &lc_time->_W_wday_abbr[3]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SABBREVDAYNAME4, &lc_time->_W_wday_abbr[4]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SABBREVDAYNAME5, &lc_time->_W_wday_abbr[5]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SABBREVDAYNAME6, &lc_time->_W_wday_abbr[6]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SABBREVDAYNAME7, &lc_time->_W_wday_abbr[0]);

        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SDAYNAME1, &lc_time->_W_wday[1]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SDAYNAME2, &lc_time->_W_wday[2]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SDAYNAME3, &lc_time->_W_wday[3]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SDAYNAME4, &lc_time->_W_wday[4]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SDAYNAME5, &lc_time->_W_wday[5]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SDAYNAME6, &lc_time->_W_wday[6]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SDAYNAME7, &lc_time->_W_wday[0]);

        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SABBREVMONTHNAME1, &lc_time->_W_month_abbr[0]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SABBREVMONTHNAME2, &lc_time->_W_month_abbr[1]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SABBREVMONTHNAME3, &lc_time->_W_month_abbr[2]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SABBREVMONTHNAME4, &lc_time->_W_month_abbr[3]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SABBREVMONTHNAME5, &lc_time->_W_month_abbr[4]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SABBREVMONTHNAME6, &lc_time->_W_month_abbr[5]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SABBREVMONTHNAME7, &lc_time->_W_month_abbr[6]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SABBREVMONTHNAME8, &lc_time->_W_month_abbr[7]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SABBREVMONTHNAME9, &lc_time->_W_month_abbr[8]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SABBREVMONTHNAME10, &lc_time->_W_month_abbr[9]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SABBREVMONTHNAME11, &lc_time->_W_month_abbr[10]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SABBREVMONTHNAME12, &lc_time->_W_month_abbr[11]);

        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SMONTHNAME1, &lc_time->_W_month[0]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SMONTHNAME2, &lc_time->_W_month[1]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SMONTHNAME3, &lc_time->_W_month[2]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SMONTHNAME4, &lc_time->_W_month[3]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SMONTHNAME5, &lc_time->_W_month[4]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SMONTHNAME6, &lc_time->_W_month[5]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SMONTHNAME7, &lc_time->_W_month[6]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SMONTHNAME8, &lc_time->_W_month[7]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SMONTHNAME9, &lc_time->_W_month[8]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SMONTHNAME10, &lc_time->_W_month[9]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SMONTHNAME11, &lc_time->_W_month[10]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_SMONTHNAME12, &lc_time->_W_month[11]);

        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_S1159, &lc_time->_W_ampm[0]);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, langid, LOCALE_S2359, &lc_time->_W_ampm[1]);

        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, ctryid, LOCALE_SSHORTDATE, &lc_time->_W_ww_sdatefmt);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, ctryid, LOCALE_SLONGDATE, &lc_time->_W_ww_ldatefmt);
        ret |= __getlocaleinfo(&locinfo, LC_WSTR_TYPE, ctryid, LOCALE_STIMEFORMAT, &lc_time->_W_ww_timefmt);

        return ret;
}

/*
 *  Free the localized time strings.
 *  Of course, this can be beautified with some loops!
 */
void __cdecl __free_lc_time (
        struct __lc_time_data *lc_time
        )
{
        if (lc_time == NULL)
                return;

        _free_crt (lc_time->wday_abbr[1]);
        _free_crt (lc_time->wday_abbr[2]);
        _free_crt (lc_time->wday_abbr[3]);
        _free_crt (lc_time->wday_abbr[4]);
        _free_crt (lc_time->wday_abbr[5]);
        _free_crt (lc_time->wday_abbr[6]);
        _free_crt (lc_time->wday_abbr[0]);

        _free_crt (lc_time->wday[1]);
        _free_crt (lc_time->wday[2]);
        _free_crt (lc_time->wday[3]);
        _free_crt (lc_time->wday[4]);
        _free_crt (lc_time->wday[5]);
        _free_crt (lc_time->wday[6]);
        _free_crt (lc_time->wday[0]);

        _free_crt (lc_time->month_abbr[0]);
        _free_crt (lc_time->month_abbr[1]);
        _free_crt (lc_time->month_abbr[2]);
        _free_crt (lc_time->month_abbr[3]);
        _free_crt (lc_time->month_abbr[4]);
        _free_crt (lc_time->month_abbr[5]);
        _free_crt (lc_time->month_abbr[6]);
        _free_crt (lc_time->month_abbr[7]);
        _free_crt (lc_time->month_abbr[8]);
        _free_crt (lc_time->month_abbr[9]);
        _free_crt (lc_time->month_abbr[10]);
        _free_crt (lc_time->month_abbr[11]);

        _free_crt (lc_time->month[0]);
        _free_crt (lc_time->month[1]);
        _free_crt (lc_time->month[2]);
        _free_crt (lc_time->month[3]);
        _free_crt (lc_time->month[4]);
        _free_crt (lc_time->month[5]);
        _free_crt (lc_time->month[6]);
        _free_crt (lc_time->month[7]);
        _free_crt (lc_time->month[8]);
        _free_crt (lc_time->month[9]);
        _free_crt (lc_time->month[10]);
        _free_crt (lc_time->month[11]);

        _free_crt (lc_time->ampm[0]);
        _free_crt (lc_time->ampm[1]);

        _free_crt (lc_time->ww_sdatefmt);
        _free_crt (lc_time->ww_ldatefmt);
        _free_crt (lc_time->ww_timefmt);


        _free_crt (lc_time->_W_wday_abbr[1]);
        _free_crt (lc_time->_W_wday_abbr[2]);
        _free_crt (lc_time->_W_wday_abbr[3]);
        _free_crt (lc_time->_W_wday_abbr[4]);
        _free_crt (lc_time->_W_wday_abbr[5]);
        _free_crt (lc_time->_W_wday_abbr[6]);
        _free_crt (lc_time->_W_wday_abbr[0]);

        _free_crt (lc_time->_W_wday[1]);
        _free_crt (lc_time->_W_wday[2]);
        _free_crt (lc_time->_W_wday[3]);
        _free_crt (lc_time->_W_wday[4]);
        _free_crt (lc_time->_W_wday[5]);
        _free_crt (lc_time->_W_wday[6]);
        _free_crt (lc_time->_W_wday[0]);

        _free_crt (lc_time->_W_month_abbr[0]);
        _free_crt (lc_time->_W_month_abbr[1]);
        _free_crt (lc_time->_W_month_abbr[2]);
        _free_crt (lc_time->_W_month_abbr[3]);
        _free_crt (lc_time->_W_month_abbr[4]);
        _free_crt (lc_time->_W_month_abbr[5]);
        _free_crt (lc_time->_W_month_abbr[6]);
        _free_crt (lc_time->_W_month_abbr[7]);
        _free_crt (lc_time->_W_month_abbr[8]);
        _free_crt (lc_time->_W_month_abbr[9]);
        _free_crt (lc_time->_W_month_abbr[10]);
        _free_crt (lc_time->_W_month_abbr[11]);

        _free_crt (lc_time->_W_month[0]);
        _free_crt (lc_time->_W_month[1]);
        _free_crt (lc_time->_W_month[2]);
        _free_crt (lc_time->_W_month[3]);
        _free_crt (lc_time->_W_month[4]);
        _free_crt (lc_time->_W_month[5]);
        _free_crt (lc_time->_W_month[6]);
        _free_crt (lc_time->_W_month[7]);
        _free_crt (lc_time->_W_month[8]);
        _free_crt (lc_time->_W_month[9]);
        _free_crt (lc_time->_W_month[10]);
        _free_crt (lc_time->_W_month[11]);

        _free_crt (lc_time->_W_ampm[0]);
        _free_crt (lc_time->_W_ampm[1]);

        _free_crt (lc_time->_W_ww_sdatefmt);
        _free_crt (lc_time->_W_ww_ldatefmt);
        _free_crt (lc_time->_W_ww_timefmt);

/* Don't need to make these pointers NULL */
}
