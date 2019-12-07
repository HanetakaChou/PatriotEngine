#include "../../../../Public/McRT/PTSImportTableHook.h"

#include <mach-o/loader.h>
#include <mach-o/nlist.h>

#if defined(__x86_64__)||defined(__aarch64__)
	typedef mach_header_64 PT_MACH_HEADER;
	#define PT_MH_MAGIC MH_MAGIC_64
	typedef load_command PT_LOAD_COMMAND;
	#define PT_LC_SEGMENT LC_SEGMENT_64
	typedef segment_command_64 PT_SEGMENT_COMMAND;
	typedef section_64 PT_SECTION;
	#define PT_LC_SYMTAB LC_SYMTAB
	typedef symtab_command PT_SYMTAB_COMMAND;
	typedef nlist_64 PT_NLIST;
	#define PT_LC_DYSYMTAB LC_DYSYMTAB
	typedef dysymtab_command PT_DYSYMTAB_COMMAND;
#elif defined(__i386__)||defined(__arm__)
	typedef mach_header PT_MACH_HEADER;
	#define PT_MH_MAGIC MH_MAGIC
	typedef load_command PT_LOAD_COMMAND;
	#define PT_LC_SEGMENT LC_SEGMENT
	typedef segment_command PT_SEGMENT_COMMAND;
	typedef section PT_SECTION;
	#define PT_LC_SYMTAB LC_SYMTAB_64
	typedef symtab_command PT_SYMTAB_COMMAND;
	typedef nlist PT_NLIST;
	#define PT_LC_DYSYMTAB LC_DYSYMTAB
	typedef dysymtab_command PT_DYSYMTAB_COMMAND;
#else
	#error Unknown Architecture
#endif

#include <string.h>
#include <assert.h>

//Import Table Hook (Also Called "Function Redirection")
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
    uintptr_t pLibraryFilePointer = reinterpret_cast<uintptr_t>(pLibraryImportBaseAddress);
    
    //Read MachHeader
    PT_MACH_HEADER const *const pMachHeader = reinterpret_cast<PT_MACH_HEADER *>(pLibraryFilePointer);
    pLibraryFilePointer += sizeof(PT_MACH_HEADER);
    
    //Even If The Mach-O Has FAT_HEADER, The dyld Will Only Load The Part That Matches The Current Architecture.
    assert(pMachHeader->magic == PT_MH_MAGIC);
    
    
    //Read LoadCommand
    
    //To Support Shared Cache
    //Reference The Source Code: Function"LinkEditOptimizer<A>::updateLoadCommands" In SourceFile"update_dyld_shared_cache.cpp" In Project"dyld"(https://github.com/opensource-apple/dyld
    uintptr_t fLinkEditStartAddress = ~0U;
    uintptr_t fLinkEditFileOffset = ~0U;
    uintptr_t fDataStartAddress = ~0U;
    uintptr_t fDataFileOffset = ~0U;
    
    //We Borrow The Name From The GUI Of "MachOView".
    
    //SymbolTable/Symbols
    uintptr_t SymbolTableFileOffset = ~0U;
    
    //StringTable
    uintptr_t StringTableFileOffset = ~0U;
    
    //
    uint32_t iDynamicSymbolBeginInSymbolTable = ~0U;
    uint32_t iDynamicSymbolEndInSymbolTable = ~0U;
    
    //DynamicSymbolTable/IndirectSymbols
    uintptr_t IndirectSymbolTableFileOffset = ~0U;
    uint32_t cIndirectSymbolTable = ~0U;
    
    //In Fact IndirectSymbol Include NonLazySymbol And LazySymbol.
    //But MachOView Only Show The Part Start From "pIndirectSymbolTable+iLazySymbolPointersBegin" In The "DynamicSymbolTable/IndirectSymbols".
    uint32_t iLazySymbolPointersBegin = ~0U;
    
    //Section(__DATA,__la_symbol_ptr)/LazySymbolPointers
    uintptr_t LazySymbolPointersFileOffset = ~0U;
    uint32_t cLazySymbolPointers = ~0U;
    
    
    for (uint32_t icmd = 0; icmd < pMachHeader->ncmds; ++icmd)
    {
        PT_LOAD_COMMAND const *const pLoadCommand = reinterpret_cast<PT_LOAD_COMMAND *>(pLibraryFilePointer);
        pLibraryFilePointer += pLoadCommand->cmdsize;
        
        switch (pLoadCommand->cmd)
        {
            case PT_LC_SEGMENT:
            {
                assert(pLoadCommand->cmd == PT_LC_SEGMENT);
                PT_SEGMENT_COMMAND const *const pSegmentCommand = reinterpret_cast<PT_SEGMENT_COMMAND const*>(pLoadCommand);
                if (::strcmp(pSegmentCommand->segname, SEG_DATA) == 0)
                {
                    PT_SECTION const *const pSectionVector = reinterpret_cast<PT_SECTION *>(reinterpret_cast<uintptr_t>(pSegmentCommand) + sizeof(PT_SEGMENT_COMMAND));
                    
                    for (uint32_t isect = 0; isect <= pSegmentCommand->nsects; ++isect)
                    {
                        PT_SECTION const *const pSection = pSectionVector + isect;
                        if (::strcmp(pSection->sectname, "__la_symbol_ptr") == 0)
                        {
                            fDataStartAddress = reinterpret_cast<uintptr_t>(pLibraryImportSlideAddress) + pSegmentCommand->vmaddr;
                            fDataFileOffset = pSegmentCommand->fileoff;
                            LazySymbolPointersFileOffset = pSection->offset;
                            iLazySymbolPointersBegin = pSection->reserved1;
                            assert((pSection->size % sizeof(uintptr_t)) == 0U);
                            cLazySymbolPointers = pSection->size / sizeof(uintptr_t);
                            break;
                        }
                    }
                }
                else if (::strcmp(pSegmentCommand->segname, SEG_LINKEDIT) == 0)
                {
                    fLinkEditStartAddress = reinterpret_cast<uintptr_t>(pLibraryImportSlideAddress) + pSegmentCommand->vmaddr;
                    fLinkEditFileOffset = pSegmentCommand->fileoff;
                    assert(pSegmentCommand->nsects == 0U);
                }
            }
                break;
            case PT_LC_SYMTAB:
            {
                assert(pLoadCommand->cmd == PT_LC_SYMTAB);
                PT_SYMTAB_COMMAND const *const pSymTabCommand = reinterpret_cast<PT_SYMTAB_COMMAND const*>(pLoadCommand);
                SymbolTableFileOffset = pSymTabCommand->symoff;
                StringTableFileOffset = pSymTabCommand->stroff;
            }
                break;
            case PT_LC_DYSYMTAB:
            {
                assert(pLoadCommand->cmd == PT_LC_DYSYMTAB);
                PT_DYSYMTAB_COMMAND const *const pDySymTabCommand = reinterpret_cast<PT_DYSYMTAB_COMMAND const*>(pLoadCommand);
                iDynamicSymbolBeginInSymbolTable = pDySymTabCommand->iundefsym;
                iDynamicSymbolEndInSymbolTable = pDySymTabCommand->iundefsym + pDySymTabCommand->nundefsym;
                IndirectSymbolTableFileOffset = pDySymTabCommand->indirectsymoff;
                cIndirectSymbolTable = pDySymTabCommand->nindirectsyms;
            }
                break;
        }
    }
    
    //To Support Shared Cache
    //Reference The Code: Function"LinkEditOptimizer<A>::updateLoadCommands" In SourceFile"update_dyld_shared_cache.cpp" In Project"dyld"(https://github.com/opensource-apple/dyld)
    bool bIsMachOFileFormatValid = (fLinkEditStartAddress != (~0U))
    && (fLinkEditFileOffset != (~0U))
    && (fLinkEditStartAddress > fLinkEditFileOffset)
    && (SymbolTableFileOffset != (~0U))
    && (StringTableFileOffset != (~0U))
    && (IndirectSymbolTableFileOffset != (~0U))
    && (fDataStartAddress != (~0U))
    && (fDataFileOffset != (~0U))
    && (fDataStartAddress > fDataFileOffset)
    && (LazySymbolPointersFileOffset != (~0U));
    assert(bIsMachOFileFormatValid);
    
    if (bIsMachOFileFormatValid)
    {
        uintptr_t const pLinkEditFileOffsetBaseAddress = fLinkEditStartAddress - fLinkEditFileOffset;
        PT_NLIST const * const pSymbolTable = reinterpret_cast<PT_NLIST const *>(pLinkEditFileOffsetBaseAddress + SymbolTableFileOffset);
        char const * const pStringTable = reinterpret_cast<char const *>(pLinkEditFileOffsetBaseAddress + StringTableFileOffset);
        uint32_t const * const pIndirectSymbolTable = reinterpret_cast<uint32_t const *>(pLinkEditFileOffsetBaseAddress + IndirectSymbolTableFileOffset);
        uintptr_t const pDataFileOffsetBaseAddress = fDataStartAddress - fDataFileOffset;
        void(PTPTR * * const pLazySymbolPointers)(void) = reinterpret_cast<void(PTPTR * * const)(void)>(pDataFileOffsetBaseAddress + LazySymbolPointersFileOffset);
        
        for (size_t iFuntionToHook = 0U; iFuntionToHook < FuntionToHookVectorCount; ++iFuntionToHook)
        {
            uint32_t iFuntionToHookInSymbolTable = ~0U;
            {
                for (uint32_t iDynamicSymbol = iDynamicSymbolBeginInSymbolTable; iDynamicSymbol < iDynamicSymbolEndInSymbolTable; ++iDynamicSymbol)
                {
                    PT_NLIST const *const pDynamicSymbol = pSymbolTable + iDynamicSymbol;
                    char const *pDynamicSymbolName = (pDynamicSymbol->n_un.n_strx != 0) ? (pStringTable + pDynamicSymbol->n_un.n_strx) : "";
                    if (::strcmp(pDynamicSymbolName, pFuntionToHookNameVector[iFuntionToHook]) == 0)
                    {
                        iFuntionToHookInSymbolTable = iDynamicSymbol;
                        break;
                    }
                }
            }
            
            if (iFuntionToHookInSymbolTable != (~0U))
            {
                uint32_t iFuntionToHookInLazySymbolPointers = ~0U;
                {
                    uint32_t const *const pLazySymbolTable = pIndirectSymbolTable + iLazySymbolPointersBegin;
                    for (uint32_t iLazySymbolPointer = 0; iLazySymbolPointer < cLazySymbolPointers; ++iLazySymbolPointer)
                    {
                        if (pLazySymbolTable[iLazySymbolPointer] == iFuntionToHookInSymbolTable)
                        {
                            iFuntionToHookInLazySymbolPointers = iLazySymbolPointer;
                            break;
                        }
                    }
                }
                
                if (iFuntionToHookInLazySymbolPointers != (~0U))
                {
					if (pFuntionToHookOldAddressVector[iFuntionToHook] != NULL)
					{
						(*pFuntionToHookOldAddressVector[iFuntionToHook]) = pLazySymbolPointers[iFuntionToHookInLazySymbolPointers];
					}

                    pLazySymbolPointers[iFuntionToHookInLazySymbolPointers] = pFuntionToHookNewAddressVector[iFuntionToHook];
                }
            }
        }
    }
}

