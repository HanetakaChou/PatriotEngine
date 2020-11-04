/***
*generics.cpp - STL/CLR Generics Definition and Assembly attributes
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*
*******************************************************************************/
#include "generics.h"

#using <mscorlib.dll>
#using <System.dll>

using namespace System::Reflection;
using namespace System::Security::Permissions;

[assembly: AssemblyTitle("Microsoft.VisualC.STLCLR")];
[assembly: AssemblyDescription("STLCLR cross assembly library")];
[assembly: AssemblyConfiguration("")];
[assembly: AssemblyCompany("Microsoft")];
[assembly: AssemblyProduct("STLCLR")];
[assembly: AssemblyCopyright("")];
[assembly: AssemblyTrademark("")];
[assembly: AssemblyCulture("")];
[assembly: AssemblyVersion("2.0.0.0")];
[assembly: AssemblyDelaySign(true)];
[assembly: AssemblyKeyName("")];

[assembly: PermissionSet(SecurityAction::RequestOptional,
        Name = "Nothing")];
[assembly: System::CLSCompliant(true)];
[assembly: System::Runtime::InteropServices::ComVisible(false)];

[module:
        System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.MSInternal",
        "CA904",
        Scope = "namespace",
        Target = "Microsoft.VisualC.StlClr")];
[module:
        System::Diagnostics::CodeAnalysis::SuppressMessage("Microsoft.MSInternal",
        "CA904",
        Scope = "namespace",
        Target = "Microsoft.VisualC.StlClr.Generic")];
