// locale0 -- class locale basic member functions
#include <climits>
#include <locale>
#include <xdebug>
#include <stdlib.h>
#include <sect_attribs.h>

 /* This should probably go to a compiler section just after the locks - unfortunately we have per-appdomain
    and per-process variables to initialize */
 #pragma warning(disable: 4073)
 #pragma init_seg(lib)

_STD_BEGIN
struct _Fac_node
	{	// node for lazy facet recording
	__CLR_OR_THIS_CALL _Fac_node(_Fac_node *_Nextarg, std::locale::facet *_Facptrarg)
		: _Next(_Nextarg), _Facptr(_Facptrarg)
		{	// construct a node with value
		}

	__CLR_OR_THIS_CALL ~_Fac_node()
		{	// destroy a facet
		_DELETE_CRT(_Facptr->_Decref());
		}

	_Fac_node *_Next;
	std::locale::facet *_Facptr;
	};

__PURE_APPDOMAIN_GLOBAL static _Fac_node *_Fac_head = 0;

_STD_END

static void __CLRCALL_OR_CDECL _Fac_tidy()
	{	// destroy lazy facets
	_BEGIN_LOCK(_LOCK_LOCALE)	// prevent double delete
		for (; std::_Fac_head != 0; )
			{	// destroy a lazy facet node
			std::_Fac_node *nodeptr = std::_Fac_head;
			std::_Fac_head = nodeptr->_Next;
			_DELETE_CRT(nodeptr);
			}
	_END_LOCK()
	}

_STD_BEGIN

struct _Fac_tidy_reg_t { ~_Fac_tidy_reg_t() { ::_Fac_tidy(); } };
_AGLOBAL const _Fac_tidy_reg_t _Fac_tidy_reg;

#if defined(_M_CEE) 
void __CLRCALL_OR_CDECL locale::facet::_Facet_Register_m(locale::facet *_This)
#else
void __CLRCALL_OR_CDECL locale::facet::_Facet_Register(locale::facet *_This)
#endif
	{	// queue up lazy facet for destruction
	_Fac_head = _NEW_CRT _Fac_node(_Fac_head, _This);
	}
_STD_END

#if !STDCPP_IMPLIB || defined(_M_CEE_PURE)
_C_STD_BEGIN
_EXTERN_C

void __CLRCALL_OR_CDECL _Deletegloballocale(void *ptr)
	{	// delete a global locale reference
	std::locale::_Locimp *locptr = *(std::locale::_Locimp **)ptr;
	if (locptr != 0)
		_DELETE_CRT(locptr->_Decref());
	}

__PURE_APPDOMAIN_GLOBAL static std::locale::_Locimp *global_locale = 0;	// pointer to current locale

static void __CLRCALL_PURE_OR_CDECL tidy_global()
	{	// delete static global locale reference
	_BEGIN_LOCK(_LOCK_LOCALE)	// prevent double delete
		_Deletegloballocale(&global_locale);
		global_locale=NULL;
	_END_LOCK()
	}

_END_EXTERN_C
_C_STD_END

_STD_BEGIN

_MRTIMP2_PURE std::locale::_Locimp *__CLRCALL_PURE_OR_CDECL std::locale::_Getgloballocale()
	{	// return pointer to current locale
	return (global_locale);
	}

_MRTIMP2_PURE void std::locale::_Setgloballocale(void *ptr)
	{	// alter pointer to current locale
	__PURE_APPDOMAIN_GLOBAL static bool registered = false;

	if (!registered)
		{	// register cleanup first time
		registered = true;
#if !defined(_M_CEE_PURE)
		::_Atexit(&tidy_global);
#else
        _atexit_m_appdomain(tidy_global);
#endif
		}
	global_locale = (std::locale::_Locimp *)ptr;
	}

 #pragma warning(disable: 4786)

__PURE_APPDOMAIN_GLOBAL static locale classic_locale(_Noinit);	// "C" locale object, uninitialized

__PURE_APPDOMAIN_GLOBAL locale::_Locimp *locale::_Locimp::_Clocptr = 0;	// pointer to classic_locale

__PURE_APPDOMAIN_GLOBAL int locale::id::_Id_cnt = 0;	// unique id counter for facets

__PURE_APPDOMAIN_GLOBAL locale::id ctype<char>::id(0);

__PURE_APPDOMAIN_GLOBAL locale::id ctype<wchar_t>::id(0);

__PURE_APPDOMAIN_GLOBAL locale::id codecvt<wchar_t, char, mbstate_t>::id(0);

 #ifdef _NATIVE_WCHAR_T_DEFINED
__PURE_APPDOMAIN_GLOBAL locale::id ctype<unsigned short>::id(0);

__PURE_APPDOMAIN_GLOBAL locale::id codecvt<unsigned short, char, mbstate_t>::id(0);

 #endif /* _NATIVE_WCHAR_T_DEFINED */

_MRTIMP2_PURE const locale& __CLRCALL_PURE_OR_CDECL locale::classic()
	{	// get reference to "C" locale
	_Init();
	return (classic_locale);
	}

_MRTIMP2_PURE locale __CLRCALL_PURE_OR_CDECL locale::empty()
	{	// make empty transparent locale
	_Init();
	return (locale(_NEW_CRT _Locimp(true)));
	}

_MRTIMP2_PURE locale::_Locimp *__CLRCALL_PURE_OR_CDECL locale::_Init()
    {	// setup global and "C" locales
	locale::_Locimp *ptr = _Getgloballocale();
	if (ptr == 0)
		{	// lock and test again
		_BEGIN_LOCK(_LOCK_LOCALE)	// prevent double initialization

			ptr = _Getgloballocale();
			if (ptr == 0)
				{	// create new locales
				_Setgloballocale(ptr = _NEW_CRT _Locimp);
				ptr->_Catmask = all;	// set current locale to "C"
				ptr->_Name = "C";

				_Locimp::_Clocptr = ptr;	// set classic to match
				_Locimp::_Clocptr->_Incref();
				new (&classic_locale) locale(_Locimp::_Clocptr);
				}
		_END_LOCK()
		}
	return (ptr);
	}

_MRTIMP2_PURE_NPURE void __CLRCALL_PURE_OR_CDECL locale::_Locimp::_Locimp_dtor(_Locimp* _This)
	{	// destruct a _Locimp
	_BEGIN_LOCK(_LOCK_LOCALE)	// prevent double delete
		for (size_t count = _This->_Facetcount; 0 < count; )
			if (_This->_Facetvec[--count] != 0)
				_DELETE_CRT(_This->_Facetvec[count]->_Decref());
		free(_This->_Facetvec);
	_END_LOCK()
	}

_MRTIMP2_PURE_NPURE void __CLRCALL_PURE_OR_CDECL _Locinfo::_Locinfo_ctor(_Locinfo *pLocinfo, const char *locname)
	{	// switch to a named locale
	const char *oldlocname = setlocale(LC_ALL, 0);

	pLocinfo->_Oldlocname = oldlocname == 0 ? "" : oldlocname;
	if (locname != 0)
		locname = setlocale(LC_ALL, locname);
	pLocinfo->_Newlocname = locname == 0 ? "*" : locname;
	}

_MRTIMP2_PURE_NPURE void __CLRCALL_PURE_OR_CDECL _Locinfo::_Locinfo_dtor(_Locinfo *pLocinfo)
	{	// destroy a _Locinfo object, revert locale
	if (!pLocinfo->_Oldlocname._Empty())
		setlocale(LC_ALL, pLocinfo->_Oldlocname._C_str());
	}
_STD_END

#endif// !STDCPP_IMPLIB 

/*
 * Copyright (c) 1992-2007 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
 V5.03:0009 */
