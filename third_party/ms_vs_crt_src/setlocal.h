/***
*setlocal.h - internal definitions used by locale-dependent functions.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Contains internal definitions/declarations for locale-dependent
*       functions, in particular those required by setlocale().
*
*       [Internal]
*
****/

#pragma once

#ifndef _INC_SETLOCAL
#define _INC_SETLOCAL

#ifndef _CRTBLD
/*
 * This is an internal C runtime header file. It is used when building
 * the C runtimes only. It is not to be used as a public header file.
 */
#error ERROR: Use of C runtime library internal header file.
#endif  /* _CRTBLD */

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include <crtdefs.h>
#include <cruntime.h>
#include <oscalls.h>
#include <limits.h>

#pragma pack(push,_CRT_PACKING)

/* Define _CRTIMP */

#ifndef _CRTIMP
#ifdef CRTDLL
#define _CRTIMP __declspec(dllexport)
#else  /* CRTDLL */
#ifdef _DLL
#define _CRTIMP __declspec(dllimport)
#else  /* _DLL */
#define _CRTIMP
#endif  /* _DLL */
#endif  /* CRTDLL */
#endif  /* _CRTIMP */

#define _CLOCALEHANDLE  0       /* "C" locale handle */
#define _CLOCALECP      CP_ACP  /* "C" locale Code page */
#define _COFFSET    127     /* offset to where ctype will point,
                               look in initctype on how it is being
                               used
                               introduced so that pctype can work with unsigned
                               char types and EOF, used only in initctype and
                               setloc.c
                               */

#define _PER_THREAD_LOCALE_BIT  0x2
#define _GLOBAL_LOCALE_BIT      0x1


/* LC_TIME localization structure */

#ifndef __LC_TIME_DATA
struct __lc_time_data {
        char *wday_abbr[7];
        char *wday[7];
        char *month_abbr[12];
        char *month[12];
        char *ampm[2];
        char *ww_sdatefmt;
        char *ww_ldatefmt;
        char *ww_timefmt;
        LCID ww_lcid;
        int  ww_caltype;
        int  refcount;
        wchar_t *_W_wday_abbr[7];
        wchar_t *_W_wday[7];
        wchar_t *_W_month_abbr[12];
        wchar_t *_W_month[12];
        wchar_t *_W_ampm[2];
        wchar_t *_W_ww_sdatefmt;
        wchar_t *_W_ww_ldatefmt;
        wchar_t *_W_ww_timefmt;
};
#define __LC_TIME_DATA
#endif  /* __LC_TIME_DATA */


#define MAX_LANG_LEN        64  /* max language name length */
#define MAX_CTRY_LEN        64  /* max country name length */
#define MAX_MODIFIER_LEN    0   /* max modifier name length - n/a */
#define MAX_LC_LEN          (MAX_LANG_LEN+MAX_CTRY_LEN+MAX_MODIFIER_LEN+3)
                                /* max entire locale string length */
#define MAX_CP_LEN          16  /* max code page name length */
#define CATNAMES_LEN        57  /* "LC_COLLATE=;LC_CTYPE=;..." length */

#define LC_INT_TYPE         0
#define LC_STR_TYPE         1
#define LC_WSTR_TYPE        2

#ifndef _TAGLC_ID_DEFINED
typedef struct tagLC_ID {
        unsigned short wLanguage;
        unsigned short wCountry;
        unsigned short wCodePage;
} LC_ID, *LPLC_ID;
#define _TAGLC_ID_DEFINED
#endif  /* _TAGLC_ID_DEFINED */

#ifndef _SETLOC_STRUCT_DEFINED
struct _is_ctype_compatible {
        unsigned long id;
        int is_clike;
};

typedef struct setloc_struct {
    /* getqloc static variables */
    char *pchLanguage;
    char *pchCountry;
    int iLcidState;
    int iPrimaryLen;
    BOOL bAbbrevLanguage;
    BOOL bAbbrevCountry;
    LCID lcidLanguage;
    LCID lcidCountry;
    /* expand_locale static variables */
    LC_ID       _cacheid;
    UINT        _cachecp;
    char        _cachein[MAX_LC_LEN];
    char        _cacheout[MAX_LC_LEN];
    /* _setlocale_set_cat (LC_CTYPE) static variable */
    struct _is_ctype_compatible _Lcid_c[5];
} _setloc_struct, *_psetloc_struct;
#define _SETLOC_STRUCT_DEFINED
#endif  /* _SETLOC_STRUCT_DEFINED */

#ifndef _THREADLOCALEINFO
typedef struct threadlocaleinfostruct {
        int refcount;
        UINT lc_codepage;
        UINT lc_collate_cp;
        LCID lc_handle[6];
        LC_ID lc_id[6];
        struct {
            char *locale;
            wchar_t *wlocale;
            int *refcount;
            int *wrefcount;
        } lc_category[6];
        int lc_clike;
        int mb_cur_max;
        int * lconv_intl_refcount;
        int * lconv_num_refcount;
        int * lconv_mon_refcount;
        struct lconv * lconv;
        int * ctype1_refcount;
        unsigned short * ctype1;
        const unsigned short * pctype;
        const unsigned char * pclmap;
        const unsigned char * pcumap;
        struct __lc_time_data * lc_time_curr;
} threadlocinfo;
#define _THREADLOCALEINFO
#endif  /* _THREADLOCALEINFO */

#ifndef _THREADMBCINFO
typedef struct threadmbcinfostruct {
        int refcount;
        int mbcodepage;
        int ismbcodepage;
        int mblcid;
        unsigned short mbulinfo[6];
        unsigned char mbctype[257];
        unsigned char mbcasemap[256];
} threadmbcinfo;
#define _THREADMBCINFO
#endif  /* _THREADMBCINFO */

typedef struct tagLC_STRINGS {
        char szLanguage[MAX_LANG_LEN];
        char szCountry[MAX_CTRY_LEN];
        char szCodePage[MAX_CP_LEN];
} LC_STRINGS, *LPLC_STRINGS;

extern pthreadlocinfo __ptlocinfo;
pthreadlocinfo __cdecl __updatetlocinfo(void);
void __cdecl _setptlocale(int);

/* We have these as globals only for single threaded model. to improve performance */
#ifndef _M_CEE_PURE
_CRTIMP extern struct lconv * __lconv;  /* pointer to current lconv structure */
#endif  /* _M_CEE_PURE */

#define __LC_HANDLE(ptloci)     (ptloci)->lc_handle
#define __LC_CODEPAGE(ptloci)   (ptloci)->lc_codepage
#define __LC_COLLATE_CP(ptloci) (ptloci)->lc_collate_cp
#define __LC_CLIKE(ptloci)      (ptloci)->lc_clike
#define __LC_TIME_CURR(ptloci)  (ptloci)->lc_time_curr
#define __LCONV(ptloci)         (ptloci)->lconv

/* These functions are for enabling STATIC_CPPLIB functionality */
_CRTIMP LCID* __cdecl ___lc_handle_func(void);
_CRTIMP UINT __cdecl ___lc_codepage_func(void);
_CRTIMP UINT __cdecl ___lc_collate_cp_func(void);

BOOL __cdecl __get_qualified_locale(_In_opt_ const LPLC_STRINGS _LpInStr, _Out_opt_ LPLC_ID _LpOutId, _Out_opt_ LPLC_STRINGS _LpOutStr);

int __cdecl __getlocaleinfo (_In_opt_ _locale_t _Locale, _In_ int _Lc_type, LCID _LocaleId, LCTYPE _FieldType, _Out_ void * _Address);

/* lconv structure for the "C" locale */
extern struct lconv __lconv_c;

/* Initialization functions for locale categories */

int __cdecl __init_collate(_In_opt_ threadlocinfo *);
int __cdecl __init_ctype(_Inout_ threadlocinfo *_LocInfo);
int __cdecl __init_monetary(_Inout_ threadlocinfo *_LocInfo);
int __cdecl __init_numeric(_Inout_ threadlocinfo *_LocInfo);
int __cdecl __init_time(_Inout_ threadlocinfo *_LocInfo);
int __cdecl __init_dummy(_In_opt_ threadlocinfo *_LocInfo);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#ifdef __cplusplus
class _LocaleUpdate
{
    _locale_tstruct localeinfo;
    _ptiddata ptd;
    bool updated;
    public:
    _LocaleUpdate(_locale_t plocinfo)
        : updated(false)
    {
        if (plocinfo == NULL)
        {
            ptd = _getptd();
            localeinfo.locinfo = ptd->ptlocinfo;
            localeinfo.mbcinfo = ptd->ptmbcinfo;

            __UPDATE_LOCALE(ptd, localeinfo.locinfo);
            __UPDATE_MBCP(ptd, localeinfo.mbcinfo);
            if (!(ptd->_ownlocale & _PER_THREAD_LOCALE_BIT))
            {
                ptd->_ownlocale |= _PER_THREAD_LOCALE_BIT;
                updated = true;
            }
        }
        else
        {
            localeinfo=*plocinfo;
        }
    }
    ~_LocaleUpdate()
    {
        if (updated)
            ptd->_ownlocale = ptd->_ownlocale & ~_PER_THREAD_LOCALE_BIT;
    }
    _locale_t GetLocaleT()
    {
        return &localeinfo;
    }
};
#endif  /* __cplusplus */

#pragma pack(pop)

#endif  /* _INC_SETLOCAL */
