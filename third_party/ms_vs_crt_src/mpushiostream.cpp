#ifdef MRTDLL
#undef MRTDLL
#endif

// This file is built into msvcurt.lib
#if !defined(_M_CEE_PURE)
#error This file must be built with /clr:pure.
#endif

#include <fstream>
#include <iostream>

#pragma warning(push)
#pragma warning(disable:4074) // warning C4074: initializers put in compiler reserved initialization area
#pragma init_seg(compiler)
#pragma warning(pop)

_STD_BEGIN
_STD_END

