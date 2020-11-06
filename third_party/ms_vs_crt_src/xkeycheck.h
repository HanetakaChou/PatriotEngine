// xkeycheck internal header
#pragma once
#ifndef _XKEYCHECK
#define _XKEYCHECK
#ifndef RC_INVOKED

 #if !defined(__cplusplus)

  #if defined(auto)	/* check C keywords */  \
	|| defined(break) \
	|| defined(case) \
	|| defined(char) \
	|| defined(const) \
	|| defined(continue) \
	|| defined(default) \
	|| defined(do) \
	|| defined(double) \
	|| defined(else) \
	|| defined(enum) \
	|| defined(extern) \
	|| defined(float) \
	|| defined(for) \
	|| defined(goto) \
	|| defined(if) \
	|| defined(inline) \
	|| defined(int) \
	|| defined(long) \
	|| defined(register) \
	|| defined(restrict) \
	|| defined(return) \
	|| defined(short) \
	|| defined(signed) \
	|| defined(sizeof) \
	|| defined(static) \
	|| defined(struct) \
	|| defined(switch) \
	|| defined(typedef) \
	|| defined(union) \
	|| defined(unsigned) \
	|| defined(using) \
	|| defined(void) \
	|| defined(volatile) \
	|| defined(while) \
	|| defined(_Bool) \
	|| defined(_Complex) \
	|| defined(_Imaginary)
   #error keyword defined before including C standard header
  #endif /* defined... */

 #else 	/* __cplusplus is defined */

  #if defined(alignas)	/* check C++ keywords */ \
	|| defined(alignof) \
	|| defined(asm) \
	|| defined(auto) \
	|| defined(bool) \
	|| defined(break) \
	|| defined(case) \
	|| defined(catch) \
	|| defined(char) \
	|| defined(char16_t) \
	|| defined(char32_t) \
	|| defined(class) \
	|| defined(const) \
	|| defined(constexpr) \
	|| defined(const_cast) \
	|| defined(continue) \
	|| defined(decltype) \
	|| defined(default) \
	|| defined(delete) \
	|| defined(do) \
	|| defined(double) \
	|| defined(dynamic_cast) \
	|| defined(else) \
	|| defined(enum) \
	|| defined(explicit) \
	|| defined(export) \
	|| defined(extern) \
	|| defined(false) \
	|| defined(float) \
	|| defined(for) \
	|| defined(friend) \
	|| defined(goto) \
	|| defined(if) \
	|| defined(inline) \
	|| defined(int) \
	|| defined(long) \
	|| defined(mutable) \
	|| defined(namespace) \
	|| defined(new) \
	|| defined(nullptr) \
	|| defined(operator) \
	|| defined(private) \
	|| defined(protected) \
	|| defined(public) \
	|| defined(register) \
	|| defined(reinterpret_cast) \
	|| defined(return) \
	|| defined(short) \
	|| defined(signed) \
	|| defined(sizeof) \
	|| defined(static_assert) \
	|| defined(static_cast) \
	|| defined(static) \
	|| defined(struct) \
	|| defined(switch) \
	|| defined(template) \
	|| defined(this) \
	|| defined(throw) \
	|| defined(true) \
	|| defined(try) \
	|| defined(typedef) \
	|| defined(typeid) \
	|| defined(typename) \
	|| defined(union) \
	|| defined(unsigned) \
	|| defined(using) \
	|| defined(virtual) \
	|| defined(void) \
	|| defined(volatile) \
	|| defined(wchar_t) \
	|| defined(while)
   #error keyword defined before including C++ standard header
  #endif /* defined... */

 #endif /* defined(__cplusplus) */
#endif /* RC_INVOKED */
#endif /* _XKEYCHECK */

/*
 * Copyright (c) 1992-2009 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
V5.20:0009 */
