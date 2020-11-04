// exception handling support functions
#include <stdexcept>

_STD_BEGIN

_CRTIMP2_PURE __declspec(noreturn) void __CLRCALL_PURE_OR_CDECL _Xinvalid_argument(_In_z_ const char * _Message)
	{ // report an invalid_argument error
	_THROW_NCEE(invalid_argument, _Message);
	}

_CRTIMP2_PURE __declspec(noreturn) void __CLRCALL_PURE_OR_CDECL _Xlength_error(_In_z_ const char * _Message)
	{ // report a length_error
	_THROW_NCEE(length_error, _Message);
	}

_CRTIMP2_PURE __declspec(noreturn) void __CLRCALL_PURE_OR_CDECL _Xout_of_range(_In_z_ const char * _Message)
	{ // report an out_of_range error
	_THROW_NCEE(out_of_range, _Message);
	}

_CRTIMP2_PURE __declspec(noreturn) void __CLRCALL_PURE_OR_CDECL _Xoverflow_error(_In_z_ const char * _Message)
	{ // report an overflow_error
	_THROW_NCEE(overflow_error, _Message);
	}

_CRTIMP2_PURE __declspec(noreturn) void __CLRCALL_PURE_OR_CDECL _Xruntime_error(_In_z_ const char * _Message)
	{ // report a runtime_error
	_THROW_NCEE(runtime_error, _Message);
	}

_STD_END

/*
 * Copyright (c) 1995-2009 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
V5.20:0009 */
