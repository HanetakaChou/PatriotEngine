/***
*cinitexe.c - C Run-Time Startup Initialization
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Do C++ initialization segment declarations for the EXE in CRT DLL
*       model
*
*Notes:
*       The C++ initializers will exist in the user EXE's data segment
*       so the special segments to contain them must be in the user EXE.
*
*******************************************************************************/

#include <stdio.h>
#include <internal.h>
#include <sect_attribs.h>

_CRTALLOC(".CRT$XIA") _PVFV __xi_a[] = { NULL };

_CRTALLOC(".CRT$XIZ") _PVFV __xi_z[] = { NULL };

_CRTALLOC(".CRT$XCA") _PVFV __xc_a[] = { NULL };

_CRTALLOC(".CRT$XCZ") _PVFV __xc_z[] = { NULL };


#pragma comment(linker, "/merge:.CRT=.rdata")

#pragma comment(linker, "/defaultlib:kernel32.lib")

#pragma comment(linker, "/disallowlib:libcmt.lib")
#pragma comment(linker, "/disallowlib:libcmtd.lib")
#ifdef _DEBUG
#pragma comment(linker, "/disallowlib:msvcrt.lib")
#else  /* _DEBUG */
#pragma comment(linker, "/disallowlib:msvcrtd.lib")
#endif  /* _DEBUG */
