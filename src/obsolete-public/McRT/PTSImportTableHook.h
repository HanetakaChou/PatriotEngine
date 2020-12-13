#ifndef PT_SYSTEM_IMPORTTABLEHOOK_H
#define PT_SYSTEM_IMPORTTABLEHOOK_H

#include "../PTCommon.h"
#include "PTSCommon.h"

#include <stddef.h>
#include <stdint.h>

//Import Table Hook (Also Called "Function Redirection")
extern "C" PTMCRTAPI void PTCALL PTS_ImportTableHook(
	void const * const pLibraryImportBaseAddress
	, void const * const pLibraryImportSlideAddress //Only Used By Mach To Support Shared Cache
	, char const * const pLibraryExportName //Only Used By Win32 To Distinguish Different (Export)Library
	, char const * const * const pFuntionToHookNameVector //[In] //SOA(Structure Of Array)
	, void(PTPTR *const * const pFuntionToHookNewAddressVector)(void) //[In] //SOA(Structure Of Array)
	, void(PTPTR * *const * const pFuntionToHookOldAddressVector)(void) //[Out,Optional] //SOA(Structure Of Array)
	, size_t const FuntionToHookVectorCount
);

//Usage:
//Win32:

//Mach:
//pLibraryImportBaseAddress: Use _dyld_get_image_header/dladdr To Get It.
//pLibraryImportSlideAddress: Use _dyld_get_image_vmaddr_slide To Get It.
//pLibraryExportName: Ignored By Mach.
//pFuntionToHookNameVector: Because Of The Name Decoration Of The Compiler, The Actual Name Of The Function In The Mach-O File Has "_" Prefix.
#endif
