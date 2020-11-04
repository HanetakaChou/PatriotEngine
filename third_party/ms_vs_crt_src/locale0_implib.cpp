#if defined(_DLL)
#ifdef CRTDLL2
#undef CRTDLL2
#endif

#ifdef MRTDLL
#undef MRTDLL
#endif

#define STDCPP_IMPLIB 1
#include "locale0.cpp"

#endif // defined(_DLL)
