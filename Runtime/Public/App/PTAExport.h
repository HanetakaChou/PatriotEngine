#ifndef PT_APPLICATION_EXPORT_H
#define PT_APPLICATION_EXPORT_H

#include "../PTCommon.h"
#include "PTACommon.h"

#include "../Window/PTWWindow.h"
#include "../System/PTSFile.h"

extern "C" PTAPPAPI int PTCALL PTAMain(IPTWWindow *pWindow, int argc, char *argv[]);

extern "C" PTAPPAPI PTBOOL PTCALL PTAFileSystem_Initialize_Hook();
extern "C" PTAPPAPI IPTSFileSystem * PTCALL PTAFileSystem_ForProcess_Hook();

#endif