/***
*muser.cpp - user handlers for managed scenarios
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Handling functions only called by the client programs,
*       not by the C/C++ runtime itself.
*
*******************************************************************************/
#include <internal.h>
#include <mtdll.h>
#include <new.h>
#include <signal.h>
#include <unknwn.h>
#include <minternal.h>

#include "crtversion.h"
#define ASSEMBLY_VERSION_HELPER(v) #v
#define ASSEMBLY_VERSION(rmj, rmm, rup, rbld) ASSEMBLY_VERSION_HELPER(rmj.rmm.rup.rbld)

using namespace System::Reflection;

[assembly: AssemblyTitle("msvcm100")];
[assembly: AssemblyDescription("Microsoft Visual C++ Managed Runtime 10.0")];
[assembly: AssemblyConfiguration("")];
[assembly: AssemblyCompany("Microsoft")];
[assembly: AssemblyProduct("Visual Studio 10.0")];
[assembly: AssemblyCopyright("Microsoft Corporation")];
[assembly: AssemblyTrademark("")];
[assembly: AssemblyCulture("")];

// [GMileka Review] Need to place the right macro here.
[assembly: AssemblyVersion(ASSEMBLY_VERSION(_VC_CRT_MAJOR_VERSION,_VC_CRT_MINOR_VERSION,_VC_CRT_BUILD_VERSION,_VC_CRT_RBUILD_VERSION))];

[assembly: AssemblyDelaySign(true)];
[assembly: AssemblyKeyName("")];
// the AssemblyKeyFile is added in the linking phase
