#ifndef PT_GFX_HAL_H
#define PT_GFX_HAL_H 1

#include "../PTCommon.h"
#include "PTGFXCommon.h"

#include <stddef.h>
#include <stdint.h>

struct PT_GFX_HAL_IDevice;
struct PT_GFX_HAL_ITexture;

struct PT_GFX_HAL_IDevice
{

};

extern "C" PTGFXAPI PT_GFX_HAL_IDevice  * PTCALL PT_GFX_HAL_Device_Init();

extern "C" PTGFXAPI PT_GFX_HAL_ITexture * PTCALL PT_GFX_HAL_Texture_Asset_LoadDDS(
	PT_GFX_HAL_IDevice *pDevice,
	void *fd_pUserData, intptr_t(PTPTR *fd_read)(void *fd_pUserData, void *buf, uintptr_t count), int64_t(PTPTR *fd_lseek64)(void *fd_pUserData, int64_t offset, int32_t whence), int32_t(PTPTR *fd_stat64)(void *fd_pUserData, int64_t *st_size)
);

#endif