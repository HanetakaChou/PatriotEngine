/***
*nlsdata2.c - globals for international library - locale handles and code page
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This module defines the locale handles and code page.  The handles are
*       required by almost all locale dependent functions.  This module is
*       separated from nlsdatax.c for granularity.
*
*******************************************************************************/

#include <locale.h>
#include <setlocal.h>
#include <mbctype.h>

__declspec(selectany) char __clocalestr[] = "C";

__declspec(selectany) struct __lc_time_data __lc_time_c = {
    {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"},

    {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
     "Friday", "Saturday"},

    {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug",
     "Sep", "Oct", "Nov", "Dec"},

    {"January", "February", "March", "April", "May", "June",
     "July", "August", "September", "October",
     "November", "December"},

    {"AM", "PM"},

    "MM/dd/yy",
    "dddd, MMMM dd, yyyy",
    "HH:mm:ss",

    0x0409,
    1,
    0,

    { L"Sun", L"Mon", L"Tue", L"Wed", L"Thu", L"Fri", L"Sat" },

    { L"Sunday", L"Monday", L"Tuesday", L"Wednesday",
      L"Thursday", L"Friday", L"Saturday" },

    { L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun", L"Jul",
      L"Aug", L"Sep", L"Oct", L"Nov", L"Dec" },

    { L"January", L"February", L"March", L"April", L"May",
      L"June", L"July", L"August", L"September", L"October",
      L"November", L"December" },

    { L"AM", L"PM" },

    L"MM/dd/yy",
    L"dddd, MMMM dd, yyyy",
    L"HH:mm:ss"
};


/*
 * initial locale information struct, set to the C locale. Used only until the
 * first call to setlocale()
 */
__declspec(selectany) threadlocinfo __initiallocinfo = {
    1,                                        /* refcount                 */
    _CLOCALECP,                               /* lc_codepage              */
    _CLOCALECP,                               /* lc_collate_cp            */
    {   _CLOCALEHANDLE,                       /* lc_handle[_ALL]          */
        _CLOCALEHANDLE,                       /* lc_handle[_COLLATE]      */
        _CLOCALEHANDLE,                       /* lc_handle[_CTYPE]        */
        _CLOCALEHANDLE,                       /* lc_handle[_MONETARY]     */
        _CLOCALEHANDLE,                       /* lc_handle[_NUMERIC]      */
        _CLOCALEHANDLE                        /* lc_handle[_TIME]         */
    },
    {   {0, 0, 0},                            /* lc_id[LC_ALL]            */
        {0, 0, 0},                            /* lc_id[LC_COLLATE]        */
        {0, 0, 0},                            /* lc_id[LC_CTYPE]          */
        {0, 0, 0},                            /* lc_id[LC_MONETARY]       */
        {0, 0, 0},                            /* lc_id[LC_NUMERIC]        */
        {0, 0, 0}                             /* lc_id[LC_TIME]           */
    },
    {   {NULL, NULL, NULL, NULL},             /* lc_category[LC_ALL]      */
        {__clocalestr, NULL, NULL, NULL},     /* lc_category[LC_COLLATE]  */
        {__clocalestr, NULL, NULL, NULL},     /* lc_category[LC_CTYPE]    */
        {__clocalestr, NULL, NULL, NULL},     /* lc_category[LC_MONETARY] */
        {__clocalestr, NULL, NULL, NULL},     /* lc_category[LC_NUMERIC]  */
        {__clocalestr, NULL, NULL, NULL}      /* lc_category[LC_TIME]     */
    },
    1,                                        /* lc_clike                 */
    1,                                        /* mb_cur_max               */
    NULL,                                     /* lconv_intl_refcount      */
    NULL,                                     /* lconv_num_refcount       */
    NULL,                                     /* lconv_mon_refcount       */
    &__lconv_c,                               /* lconv                    */
    NULL,                                     /* ctype1_refcount          */
    NULL,                                     /* ctype1                   */
    __newctype + 128,                         /* pctype                   */
    __newclmap + 128,                         /* pclmap                   */
    __newcumap + 128,                         /* pcumap                   */
    &__lc_time_c,                             /* lc_time_curr             */
};

/*
 * global pointer to the current per-thread locale information structure.
 */
__declspec(selectany) pthreadlocinfo __ptlocinfo = &__initiallocinfo;

__declspec(selectany) _locale_tstruct __initiallocalestructinfo =
{
    &__initiallocinfo,
    &__initialmbcinfo
};
