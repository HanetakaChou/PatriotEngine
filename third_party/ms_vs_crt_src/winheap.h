/***
*winheap.h - Private include file for winheap directory.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Contains information needed by the C library heap code.
*
*       [Internal]
*
****/

#pragma once

#ifndef _INC_WINHEAP
#define _INC_WINHEAP

#include <crtdefs.h>

#pragma pack(push,_CRT_PACKING)

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

#include <sal.h>
#include <windows.h>


#define BYTES_PER_PARA      16
#define PARAS_PER_PAGE      256     /*  tunable value */
#define BYTES_PER_PAGE      (BYTES_PER_PARA * PARAS_PER_PAGE)

extern  HANDLE _crtheap;

_Check_return_  _Ret_opt_bytecap_(_Size) void * __cdecl _nh_malloc(_In_ size_t _Size, _In_ int _NhFlag);
_Check_return_  _Ret_opt_bytecap_(_Size) void * __cdecl _heap_alloc(_In_ size_t _Size);

extern int     __cdecl _heap_init(void);
extern void    __cdecl _heap_term(void);

_Check_return_ _Ret_opt_bytecap_(_Size) extern _CRTIMP void *  __cdecl _malloc_base(_In_ size_t _Size);

extern _CRTIMP void    __cdecl _free_base(_Post_ptr_invalid_ void * _Memory);
_Success_(return!=0)
_Check_return_ _Ret_opt_bytecap_(_NewSize) extern void *  __cdecl _realloc_base(_Post_ptr_invalid_  void * _Memory, _In_ size_t _NewSize);
_Success_(return!=0)
_Check_return_ _Ret_opt_bytecap_x_(_Count*_Size) extern void *  __cdecl _recalloc_base(_Post_ptr_invalid_ void * _Memory, _In_ size_t _Count, _In_ size_t _Size);

_Check_return_ _Ret_opt_bytecap_(_NewSize) extern void *  __cdecl _expand_base(_Pre_notnull_ void * _Memory, _In_ size_t _NewSize);
_Check_return_ _Ret_opt_bytecap_x_(_Count*_Size) extern void *  __cdecl _calloc_base(_In_ size_t _Count, _In_ size_t _Size);

_Check_return_ extern size_t  __cdecl _msize_base(_Pre_notnull_ void * _Memory);
_Check_return_ extern size_t  __cdecl _aligned_msize_base(_Pre_notnull_ void * _Memory, _In_ size_t _Alignment, _In_ size_t _Offset);


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#pragma pack(pop)

#endif  /* _INC_WINHEAP */
