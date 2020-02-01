#include "../../../Public/McRT/PTSImportTableHook.h"
#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ImageHlp.h>
#include <assert.h>

void PTCALL PTS_ImportTableHook(
	void const * const pLibraryImportBaseAddress
	, void const * const pLibraryImportSlideAddress //Only Used By Mach To Support Shared Cache
	, char const * const pLibraryExportName //Only Used By Win32 To Distinguish Different (Export)Library
	, char const * const * const pFuntionToHookNameVector //[In] //SOA(Structure Of Array)
	, void(PTPTR *const * const pFuntionToHookNewAddressVector)(void) //[In] //SOA(Structure Of Array)
	, void(PTPTR * *const * const pFuntionToHookOldAddressVector)(void) //[Out,Optional] //SOA(Structure Of Array)
	, size_t const FuntionToHookVectorCount
)
{
	//On Win32, Functions Imported From Different (Export)Library Are Grouped Into Different ImportTable.
	//We Find The ImportTable That Matches The "pLibraryExportName" Argument .

	IMAGE_IMPORT_DESCRIPTOR *pImportDescriptor = NULL;
	{
		ULONG ImportDescriptorVectorSize;
		IMAGE_IMPORT_DESCRIPTOR *ImportDescriptorVector = static_cast<IMAGE_IMPORT_DESCRIPTOR *>(::ImageDirectoryEntryToData(const_cast<void *>(pLibraryImportBaseAddress), TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ImportDescriptorVectorSize));
		assert((ImportDescriptorVectorSize % sizeof(IMAGE_IMPORT_DESCRIPTOR)) == 0U);
		ULONG ImportDescriptorVectorCount = (ImportDescriptorVectorSize / sizeof(IMAGE_IMPORT_DESCRIPTOR));
		for (ULONG iImportDescriptor = 0U; iImportDescriptor < ImportDescriptorVectorCount; ++iImportDescriptor)
		{
			char *NameDLL = reinterpret_cast<char *>(reinterpret_cast<ULONG_PTR>(pLibraryImportBaseAddress) + ImportDescriptorVector[iImportDescriptor].Name);//RVA
			if (::_stricmp(NameDLL, pLibraryExportName) == 0)
			{
				pImportDescriptor = ImportDescriptorVector + iImportDescriptor;
				break;
			}
		}
	}

	//The ImportTable Can Be Considered As A SOA(Structure Of Array), Which Constists Of Two Arrays - ImportNameTable And ImportAddressTable.
	//We Use The ImportNameTable To Find The Index Of The Function In The ImportTable And Write The New Address To The ImportAddressTable.

	if (pImportDescriptor != NULL)
	{
		for (size_t iFuntionToHook = 0U; iFuntionToHook < FuntionToHookVectorCount; ++iFuntionToHook)
		{
			void(PTPTR * *pFunctionAddressToWrite)(void) = NULL;
			{
				//OriginalFirstThunk Is The RelativeVirtualAddress From LibraryBaseAddress
				IMAGE_THUNK_DATA *ImportNameTable = reinterpret_cast<IMAGE_THUNK_DATA *>(reinterpret_cast<ULONG_PTR>(pLibraryImportBaseAddress) + pImportDescriptor->OriginalFirstThunk);

				//FirstThunk Is The RelativeVirtualAddress From LibraryBaseAddress
				IMAGE_THUNK_DATA *ImportAddressTable = reinterpret_cast<IMAGE_THUNK_DATA *>(reinterpret_cast<ULONG_PTR>(pLibraryImportBaseAddress) + pImportDescriptor->FirstThunk);

				for (size_t iImportTable = 0U; ImportNameTable[iImportTable].u1.AddressOfData != 0U; ++iImportTable) //��RVA = 0�� Means End Of Table
				{
					//Name Is The RelativeVirtualAddress From LibraryBaseAddress
					char *FunctionName = reinterpret_cast<char *>(reinterpret_cast<IMAGE_IMPORT_BY_NAME *>(reinterpret_cast<ULONG_PTR>(pLibraryImportBaseAddress) + ImportNameTable[iImportTable].u1.AddressOfData)->Name);
					if (::strcmp(FunctionName, pFuntionToHookNameVector[iFuntionToHook]) == 0)
					{
						pFunctionAddressToWrite = reinterpret_cast<void(PTPTR * *)(void)>(&ImportAddressTable[iImportTable].u1.Function);
						break;
					}
				}
			}

			if (pFunctionAddressToWrite != NULL)
			{
				//We Use Write-Copy To Ensure The Write Will Success!
				DWORD flOldProtect1;
				::VirtualProtect(pFunctionAddressToWrite, sizeof(ULONG_PTR), PAGE_WRITECOPY, &flOldProtect1);

				if (pFuntionToHookOldAddressVector[iFuntionToHook] != NULL)
				{
					(*pFuntionToHookOldAddressVector[iFuntionToHook]) = (*pFunctionAddressToWrite);
				}

				*pFunctionAddressToWrite = pFuntionToHookNewAddressVector[iFuntionToHook];

				DWORD flOldProtect2;
				::VirtualProtect(pFunctionAddressToWrite, sizeof(ULONG_PTR), flOldProtect1, &flOldProtect2);
			}
		}
	}
}