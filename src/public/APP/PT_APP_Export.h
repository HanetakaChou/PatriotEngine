#ifndef _PT_APP_EXPORT_H_
#define _PT_APP_EXPORT_H_

#include "../PTCommon.h"
#include "PT_APP_Common.h"

#include "../WSI/PT_WSI_Window.h"
#include "../McRT/PTSFile.h"

extern "C" PTAPPAPI int PTCALL PTAMain(PT_WSI_IWindow *pWindow, int argc, char *argv[]);

extern "C" PTAPPAPI bool PTCALL PTAFileSystem_Initialize_Hook();
extern "C" PTAPPAPI IPTSFileSystem * PTCALL PTAFileSystem_ForProcess_Hook();

#endif