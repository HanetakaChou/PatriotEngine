/***
*dbgint.h - Supports debugging features of the C runtime library.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Support CRT debugging features.
*
*       [Internal]
*
****/

#pragma once

#ifndef _INC_DBGINT
#define _INC_DBGINT

#ifndef _CRTBLD
/*
 * This is an internal C runtime header file. It is used when building
 * the C runtimes only. It is not to be used as a public header file.
 */
#error ERROR: Use of C runtime library internal header file.
#endif  /* _CRTBLD */

#include <crtdbg.h>

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

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* __crt_debugger_hook */
#define _CRT_DEBUGGER_IGNORE            -1
#define _CRT_DEBUGGER_GSFAILURE         1
#define _CRT_DEBUGGER_INVALIDPARAMETER  2
#define _CRT_DEBUGGER_ABORT             3

#ifdef _M_IX86
#define _CRT_DEBUGGER_HOOK _crt_debugger_hook
#elif defined (_M_IA64)
#define _CRT_DEBUGGER_HOOK __crt_debugger_hook_ex
_CRTIMP void __cdecl __crt_debugger_hook(int);
#else  /* defined (_M_IA64) */
#define _CRT_DEBUGGER_HOOK __crt_debugger_hook
#endif  /* defined (_M_IA64) */

_CRTIMP void __cdecl _CRT_DEBUGGER_HOOK(int);

#ifndef _DEBUG

 /****************************************************************************
 *
 * Debug OFF
 * Debug OFF
 * Debug OFF
 *
 ***************************************************************************/

#ifdef __cplusplus

#define _new_crt                        new

#endif  /* __cplusplus */

#define _expand_crt                     _expand
#define _free_crt                       free
#define _msize_crt                      _msize
#define _dupenv_s_crt(ps1, size, s2)    _dupenv_s(ps1, size, s2)
#define _wdupenv_s_crt(ps1, size, s2)   _wdupenv_s(ps1, size, s2)
#if defined (_SYSCRT) || defined (MRTDLL) || defined (_M_CEE_PURE)
#define _malloc_crt                     malloc
#define _calloc_crt                     calloc
#define _realloc_crt                    realloc
#define _recalloc_crt                   _recalloc
#else  /* defined (_SYSCRT) || defined (MRTDLL) || defined (_M_CEE_PURE) */
_Check_return_ _Ret_opt_bytecap_(_Size) _CRTIMP void * __cdecl _malloc_crt(_In_ size_t _Size);
_Check_return_ _Ret_opt_bytecap_x_(_Count*_Size) _CRTIMP void * __cdecl _calloc_crt(_In_ size_t _Count, _In_ size_t _Size);
_Success_(return!=0)
_Check_return_ _Ret_opt_bytecap_(_NewSize) _CRTIMP void * __cdecl _realloc_crt(_In_opt_ void *_Ptr, _In_ size_t _NewSize);
_Success_(return!=0)
_Check_return_ _Ret_opt_bytecap_x_(_Count*_Size) _CRTIMP void * __cdecl _recalloc_crt(_In_opt_ void *_Ptr, _In_ size_t _Count, _In_ size_t _Size);
#endif  /* defined (_SYSCRT) || defined (MRTDLL) || defined (_M_CEE_PURE) */

#define _malloc_base                    malloc
#define _nh_malloc_base                 _nh_malloc
#define _nh_malloc_dbg(s, n, t, f, l)   _nh_malloc(s, n)
#define _heap_alloc_dbg(s, t, f, l)     _heap_alloc(s)
#define _calloc_base                    calloc
#define _realloc_base                   realloc
#define _recalloc_base                  _recalloc
#define _expand_base                    _expand
#define _free_base                      free
#define _msize_base                     _msize
#define _aligned_msize_base             _aligned_msize
#define _aligned_malloc_base            _aligned_malloc
#define _aligned_realloc_base           _aligned_realloc
#define _aligned_recalloc_base          _aligned_recalloc
#define _aligned_offset_malloc_base     _aligned_offset_malloc
#define _aligned_offset_realloc_base    _aligned_offset_realloc
#define _aligned_offset_recalloc_base   _aligned_offset_recalloc
#define _aligned_free_base              _aligned_free

#define _calloc_dbg_nolock(c, s, t, f, l)       _calloc_nolock(c, s)
#define _realloc_dbg_nolock(p, s, t, f, l)      _realloc_nolock(p, s)
#define _recalloc_dbg_nolock(p, c, s, t, f, l)  _recalloc_nolock(p, c, s)
#define _free_base_nolock                       _free_nolock
#define _free_dbg_nolock(p, t)                  _free_nolock(p)

#else  /* _DEBUG */


 /****************************************************************************
 *
 * Debug ON
 * Debug ON
 * Debug ON
 *
 ***************************************************************************/

#define _THISFILE   __FILE__

#ifdef __cplusplus

#define _new_crt        new(_CRT_BLOCK, _THISFILE, __LINE__)

#endif  /* __cplusplus */

#define _malloc_crt(s)          _malloc_dbg(s, _CRT_BLOCK, _THISFILE, __LINE__)
#define _calloc_crt(c, s)       _calloc_dbg(c, s, _CRT_BLOCK, _THISFILE, __LINE__)
#define _realloc_crt(p, s)      _realloc_dbg(p, s, _CRT_BLOCK, _THISFILE, __LINE__)
#define _recalloc_crt(p, c, s)  _recalloc_dbg(p, c, s, _CRT_BLOCK, _THISFILE, __LINE__)
#define _expand_crt(p, s)       _expand_dbg(p, s, _CRT_BLOCK)
#define _free_crt(p)            _free_dbg(p, _CRT_BLOCK)
#define _msize_crt(p)           _msize_dbg(p, _CRT_BLOCK)
#define _dupenv_s_crt(ps1, size, s2)    _dupenv_s_dbg(ps1, size, s2, _CRT_BLOCK, _THISFILE, __LINE__)
#define _wdupenv_s_crt(ps1, size, s2)   _wdupenv_s_dbg(ps1, size, s2, _CRT_BLOCK, _THISFILE, __LINE__)

/*
 * Prototypes for malloc, free, realloc, etc are in malloc.h
 */

_Check_return_ _Ret_opt_bytecap_(_Size) _CRTIMP void * __cdecl _malloc_base(
        _In_ size_t _Size
        );

_Check_return_ _Ret_opt_bytecap_(_Size) void * __cdecl _nh_malloc_base (
        _In_ size_t _Size,
        _In_ int _NhFlag
        );

_Check_return_ _Ret_opt_bytecap_(_Size) void * __cdecl _nh_malloc_dbg (
        _In_ size_t _Size,
        _In_ int _NhFlag,
        _In_ int _BlockType,
        _In_opt_z_ const char * _Filename,
        _In_ int _LineNumber
        );

_Check_return_ _Ret_opt_bytecap_(_Size) void * __cdecl _heap_alloc_dbg(
        _In_ size_t _Size,
        _In_ int _BlockType,
        _In_opt_z_ const char * _Filename,
        _In_ int _LineNumber
        );

_Check_return_ _Ret_opt_bytecap_x_(_Count*_Size) void * __cdecl _calloc_base(
        _In_ size_t _Count,
        _In_ size_t _Size
        );

_Success_(return!=0)
_Check_return_ _Ret_opt_bytecap_(_NewSize) void * __cdecl _realloc_base(
        _Post_ptr_invalid_ void * _Memory,
        _In_ size_t _NewSize
        );

_Success_(return!=0)
_Check_return_ _Ret_opt_bytecap_x_(_Count*_Size) void * __cdecl _recalloc_base
(
        _Post_ptr_invalid_ void * _Memory,
        _In_ size_t _Count,
        _In_ size_t _Size
);

_Check_return_ _Ret_opt_bytecap_(_NewSize) void * __cdecl _expand_base(
        _Pre_notnull_ void * _Memory,
        _In_ size_t _NewSize
        );

_CRTIMP void __cdecl _free_base(
        _Post_ptr_invalid_ void * _Memory
        );

_Check_return_ size_t __cdecl _msize_base (
        _Pre_notnull_ void * _Memory
        );

_Check_return_ size_t __cdecl _aligned_msize_base (
        _Pre_notnull_ void * _Memory,
        _In_ size_t _Alignment,
        _In_ size_t _Offset
        );

void    __cdecl _aligned_free_base(
        _Post_ptr_invalid_ void * _Memory
        );

_Check_return_ _Ret_opt_bytecap_(_Size) void *  __cdecl _aligned_malloc_base(
        _In_ size_t _Size,
        _In_ size_t _Alignment
        );

_Check_return_ _Ret_opt_bytecap_(_Size) void *  __cdecl _aligned_offset_malloc_base(
        _In_ size_t _Size,
        _In_ size_t _Alignment,
        _In_ size_t _Offset
        );

_Success_(return!=0)
_Check_return_ _Ret_opt_bytecap_(_NewSize) void *  __cdecl _aligned_realloc_base(
        _Post_ptr_invalid_ void * _Memory,
        _In_ size_t _NewSize,
        _In_ size_t _Alignment
        );

_Success_(return!=0)
_Check_return_ _Ret_opt_bytecap_(_Size) void *  __cdecl _aligned_recalloc_base
(
        _Post_ptr_invalid_ void * _Memory,
        _In_ size_t _Count,
        _In_ size_t _Size,
        _In_ size_t _Alignment
);

_Success_(return!=0)
_Check_return_ _Ret_opt_bytecap_(_NewSize) void *  __cdecl _aligned_offset_realloc_base(
        _Post_ptr_invalid_ void * _Memory,
        _In_ size_t _NewSize,
        _In_ size_t _Alignment,
        _In_ size_t _Offset
        );

_Success_(return!=0)
_Check_return_ _Ret_opt_bytecap_(_Size) void *  __cdecl _aligned_offset_recalloc_base
(
        _Post_ptr_invalid_ void * _Memory,
        _In_ size_t _Count,
        _In_ size_t _Size,
        _In_ size_t _Alignment,
        _In_ size_t _Offset
);


void __cdecl __freeCrtMemory();

/*
 * Prototypes and macros for multi-thread support
 */


_Check_return_ _Ret_opt_bytecap_x_(_Count*_Size) void * __cdecl _calloc_dbg_nolock(
        _In_ size_t _Count,
        _In_ size_t _Size,
        _In_ int _BlockType,
        _In_opt_z_ char * _Filename,
        _In_ int _LineNumber
        );

_Success_(return!=0)
_Check_return_ _Ret_opt_bytecap_(_NewSize) void * __cdecl _realloc_dbg_nolock(
        _Post_ptr_invalid_ void * _Memory,
        _In_ size_t _NewSize,
        _In_ int _BlockType,
        _In_opt_z_ const char * _Filename,
        _In_ int _LineNumber
        );

_Success_(return!=0)
_Check_return_ _Ret_opt_bytecap_(_Size) void * __cdecl _recalloc_dbg_nolock
(
         _Post_ptr_invalid_ void * _Memory,
        _In_ size_t _Count,
        _In_ size_t _Size,
        _In_ int _BlockType,
        _In_opt_z_ const char * _Filename,
        _In_ int _LineNumber
);

void __cdecl _free_base_nolock(
        _Post_ptr_invalid_ void * _Memory
        );

void __cdecl _free_dbg_nolock(
        _Post_ptr_invalid_ void *_Memory,
        _In_ int _BlockType
        );

/*
 * For diagnostic purpose, blocks are allocated with extra information and
 * stored in a doubly-linked list.  This makes all blocks registered with
 * how big they are, when they were allocated, and what they are used for.
 */

#define nNoMansLandSize 4

typedef struct _CrtMemBlockHeader
{
        struct _CrtMemBlockHeader * pBlockHeaderNext;
        struct _CrtMemBlockHeader * pBlockHeaderPrev;
        char *                      szFileName;
        int                         nLine;
#ifdef _WIN64
        /* These items are reversed on Win64 to eliminate gaps in the struct
         * and ensure that sizeof(struct)%16 == 0, so 16-byte alignment is
         * maintained in the debug heap.
         */
        int                         nBlockUse;
        size_t                      nDataSize;
#else  /* _WIN64 */
        size_t                      nDataSize;
        int                         nBlockUse;
#endif  /* _WIN64 */
        long                        lRequest;
        unsigned char               gap[nNoMansLandSize];
        /* followed by:
         *  unsigned char           data[nDataSize];
         *  unsigned char           anotherGap[nNoMansLandSize];
         */
} _CrtMemBlockHeader;

#define pbData(pblock) ((unsigned char *)((_CrtMemBlockHeader *)pblock + 1))
#define pHdr(pbData) (((_CrtMemBlockHeader *)pbData)-1)


_CRTIMP void __cdecl _CrtSetDbgBlockType(
        _In_opt_ void * _Memory,
        _In_ int _BlockType
        );

#define _BLOCK_TYPE_IS_VALID(use) (_BLOCK_TYPE(use) == _CLIENT_BLOCK || \
                                              (use) == _NORMAL_BLOCK || \
                                   _BLOCK_TYPE(use) == _CRT_BLOCK    || \
                                              (use) == _IGNORE_BLOCK)

extern _CRT_ALLOC_HOOK _pfnAllocHook; /* defined in dbghook.c */

int __cdecl _CrtDefaultAllocHook(
        int,
        void *,
        size_t,
        int,
        long,
        const unsigned char *,
        int
        );


/*    Adding some defines which are used in dbgrpt.c */

#define DBGRPT_MAX_MSG 4096
#define DBGRPT_TOOLONGMSG "_CrtDbgReport: String too long or IO Error"
#define DBGRPT_INVALIDMSG "_CrtDbgReport: String too long or Invalid characters in String"

typedef struct ReportHookNode {
        struct ReportHookNode *prev;
        struct ReportHookNode *next;
        unsigned refcount;
        _CRT_REPORT_HOOK pfnHookFunc;
} ReportHookNode;

typedef struct ReportHookNodeW {
        struct ReportHookNodeW *prev;
        struct ReportHookNodeW *next;
        unsigned refcount;
        _CRT_REPORT_HOOKW pfnHookFunc;
} ReportHookNodeW;

#endif  /* _DEBUG */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#pragma pack(pop)

#endif  /* _INC_DBGINT */
