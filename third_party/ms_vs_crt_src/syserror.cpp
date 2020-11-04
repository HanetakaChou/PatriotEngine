// syserror.cpp -- system_error static objects
#include <cstdlib>

 #if _HAS_CPP0X
 #include <system_error>
_STD_BEGIN

 #pragma warning(disable: 4996)	/* quiet strerror warning */

		// CLASS _Generic_error_category
class _Generic_error_category
	: public error_category
	{	// categorize a generic error
public:
	typedef error_category::value_type value_type;

	_Generic_error_category()
		:	_Mystr("generic")
		{	// default constructor
		}

	virtual const char *name() const
		{	// get name of category
		return (_Mystr);
		}

	virtual string message(value_type _Errcode) const
		{	// convert to name of error
		return (_CSTD strerror(_Errcode));
		}

private:
	const char *_Mystr;	// the category name
	};

		// CLASS _Iostream_error_category
class _Iostream_error_category
	: public error_category
	{	// categorize a Posix error
public:
	typedef error_category::value_type value_type;

	_Iostream_error_category()
		:	_Mystr("iostream")
		{	// default constructor
		}

	virtual const char *name() const
		{	// get name of category
		return (_Mystr);
		}

	virtual string message(value_type _Errcode) const
		{	// convert to name of error
		if (io_errc::stream == _Errcode)
			return ("iostream stream error");
		else
			return (_CSTD strerror(_Errcode));
		}

private:
	const char *_Mystr;	// the category name
	};

		// CLASS _System_error_category
class _System_error_category
	: public _Generic_error_category
	{	// categorize a generic error
public:
	typedef error_category::value_type value_type;

	_System_error_category()
		:	_Mystr("system")
		{	// default constructor
		}

	virtual const char *name() const
		{	// get name of category
		return (_Mystr);
		}

	virtual error_condition
		default_error_condition(value_type _Errval) const
		{	// make error_condition for error code (generic if possible)
		return (error_condition(_Errval, *this));
		}

private:
	const char *_Mystr;	// the category name
	};

const _Generic_error_category _Generic_object;
const _Iostream_error_category _Iostream_object;
const _System_error_category _System_object;

_CRTIMP2_PURE const error_category& __CLRCALL_PURE_OR_CDECL generic_category()
	{	// get generic_category
	return (_Generic_object);
	}

_CRTIMP2_PURE const error_category& __CLRCALL_PURE_OR_CDECL iostream_category()
	{	// get iostream_category
	return (_Iostream_object);
	}

_CRTIMP2_PURE const error_category& __CLRCALL_PURE_OR_CDECL system_category()
	{	// get system_category
	return (_System_object);
	}

_STD_END
 #else /* _HAS_CPP0X */
 #endif /* _HAS_CPP0X */

/*
 * Copyright (c) 1992-2009 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
V5.20:0009 */
