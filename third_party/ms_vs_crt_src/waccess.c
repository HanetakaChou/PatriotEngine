/***
*waccess.c - access function (wchar_t version)
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file has the _waccess() function which checks on file accessability.
*
*******************************************************************************/


#define WPRFLAG 1

#ifndef _UNICODE
#define _UNICODE 1
#endif  /* _UNICODE */

#ifndef UNICODE
#define UNICODE 1
#endif  /* UNICODE */

#undef _MBCS /* UNICODE not _MBCS */

#include "access.c"

