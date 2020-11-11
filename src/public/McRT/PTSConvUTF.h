#ifndef PT_SYSTEM_CONVUTF_H
#define PT_SYSTEM_CONVUTF_H

#include "../PTCommon.h"
#include "PTSCommon.h"

#include <stdint.h>

extern "C" PTMCRTAPI bool PTCALL PTSConv_UTF8ToUTF16(char const *pInBuf, uint32_t *pInCharsLeft, char16_t *pOutbuf, uint32_t *pOutCharsLeft);
extern "C" PTMCRTAPI bool PTCALL PTSConv_UTF16ToUTF8(char16_t const *pInBuf, uint32_t *pInCharsLeft, char *pOutbuf, uint32_t *pOutCharsLeft);

#endif

