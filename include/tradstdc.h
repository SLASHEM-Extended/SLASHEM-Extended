/*	SCCS Id: @(#)tradstdc.h 3.4	1993/05/30	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef TRADSTDC_H
#define TRADSTDC_H

#if defined(DUMB) && !defined(NOVOID)
#define NOVOID
#endif

#ifdef NOVOID
#define void int
#endif

/*
 * Borland C provides enough ANSI C compatibility in its Borland C++
 * mode to warrant this.  But it does not set __STDC__ unless it compiles
 * in its ANSI keywords only mode, which prevents use of <dos.h> and
 * far pointer use.
 */
#if (defined(__STDC__) || defined(__TURBOC__)) && !defined(NOTSTDC)
#define NHSTDC
#endif

#if defined(ultrix) && defined(__STDC__) && !defined(__LANGUAGE_C)
/* Ultrix seems to be in a constant state of flux.  This check attempts to
 * set up ansi compatibility if it wasn't set up correctly by the compiler.
 */
#ifdef mips
#define __mips mips
#endif

#ifdef LANGUAGE_C
#define __LANGUAGE_C LANGUAGE_C
#endif

#endif

/*
 * ANSI X3J11 detection.
 * Makes substitutes for compatibility with the old C standard.
 */

/* Decide how to handle variable parameter lists:
 * USE_STDARG means use the ANSI <stdarg.h> facilities (only ANSI compilers
 * should do this, and only if the library supports it).
 * USE_VARARGS means use the <varargs.h> facilities.  Again, this should only
 * be done if the library supports it.  ANSI is *not* required for this.
 * Otherwise, the kludgy old methods are used.
 * The defaults are USE_STDARG for ANSI compilers, and USE_OLDARGS for
 * others.
 */

/* #define USE_VARARGS */	/* use <varargs.h> instead of <stdarg.h> */
/* #define USE_OLDARGS */	/* don't use any variable argument facilites */

#if defined(apollo)             /* Apollos have stdarg(3) but not stdarg.h */
# define USE_VARARGS
#endif

#if defined(NHSTDC) || defined(ULTRIX_PROTO) || defined(MAC)
# if !defined(USE_VARARGS) && !defined(USE_OLDARGS) && !defined(USE_STDARG)
#   define USE_STDARG
# endif
#endif

#ifdef NEED_VARARGS		/* only define these if necessary */
#ifdef USE_STDARG
#include <stdarg.h>
# define VA_DECL(typ1,var1)	(typ1 var1, ...) { va_list the_args;
# define VA_DECL2(typ1,var1,typ2,var2)	\
	(typ1 var1, typ2 var2, ...) { va_list the_args;
# define VA_INIT(var1,typ1)
# define VA_NEXT(var1,typ1)	var1 = va_arg(the_args, typ1)
# define VA_ARGS		the_args
# define VA_START(x)		va_start(the_args, x)
# define VA_END()		va_end(the_args)
# if defined(ULTRIX_PROTO) && !defined(_VA_LIST_)
#  define _VA_LIST_	/* prevents multiple def in stdio.h */
# endif
#else
# ifdef USE_VARARGS
#include <varargs.h>
#  define VA_DECL(typ1,var1)	(va_alist) va_dcl {\
		va_list the_args; typ1 var1;
#  define VA_DECL2(typ1,var1,typ2,var2)	(va_alist) va_dcl {\
		va_list the_args; typ1 var1; typ2 var2;
#  define VA_ARGS		the_args
#  define VA_START(x)		va_start(the_args)
#  define VA_INIT(var1,typ1)	var1 = va_arg(the_args, typ1)
#  define VA_NEXT(var1,typ1)	var1 = va_arg(the_args,typ1)
#  define VA_END()		va_end(the_args)
# else
#   define VA_ARGS	arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9
#   define VA_DECL(typ1,var1)  (var1,VA_ARGS) typ1 var1; \
	char *arg1,*arg2,*arg3,*arg4,*arg5,*arg6,*arg7,*arg8,*arg9; {
#   define VA_DECL2(typ1,var1,typ2,var2)  (var1,var2,VA_ARGS) \
	typ1 var1; typ2 var2;\
	char *arg1,*arg2,*arg3,*arg4,*arg5,*arg6,*arg7,*arg8,*arg9; {
#   define VA_START(x)
#   define VA_INIT(var1,typ1)
#   define VA_END()
# endif
#endif
#endif /* NEED_VARARGS */

#if defined(NHSTDC) || defined(MSDOS) || defined(MAC) || defined(ULTRIX_PROTO) || defined(__BEOS__)

# if !((defined(ULTRIX_PROTO) && !defined(__GNUC__)) || defined(OS2_CSET2))
#  if !defined(NHSTDC) && !defined(MAC)
#   define const
#   define signed
#   define volatile
#  endif
# endif

/*
 * Suppress `const' if necessary and not handled elsewhere.
 * Don't use `#if defined(xxx) && !defined(const)'
 * because some compilers choke on `defined(const)'.
 * This has been observed with Lattice, MPW, and High C.
 */
# if (defined(ULTRIX_PROTO) && !defined(NHSTDC)) || defined(apollo)
	/* the system header files don't use `const' properly */
#  ifndef const
#   define const
#  endif
# endif

#else /* NHSTDC */	/* a "traditional" C  compiler */

/*
 * Traditional C compilers don't have "signed", "const", or "volatile".
 */
# define signed
# define const
# define volatile

#endif /* NHSTDC */



/*
 * According to ANSI, prototypes for old-style declarations must widen the
 * arguments to int.  However, the MSDOS compilers accept shorter arguments
 * (char, short, etc.) in prototypes and do typechecking with them.  Therefore
 * this mess to allow the better typechecking while also allowing some
 * prototypes for the ANSI compilers so people quit trying to fix the
 * prototypes to match the standard and thus lose the typechecking.
 */
#if defined(MSDOS) && !defined(__GO32__)
#define UNWIDENED_PROTOTYPES
#endif
#if defined(AMIGA) && !defined(AZTEC_50)
#define UNWIDENED_PROTOTYPES
#endif
#if defined(MAC_MPW)
#define WIDENED_PROTOTYPES
#endif
#if defined(__MWERKS__) && defined(__BEOS__)
#define UNWIDENED_PROTOTYPES
#endif
#if defined(WIN32)
#define UNWIDENED_PROTOTYPES
#endif

#if defined(ULTRIX_PROTO) && defined(ULTRIX_CC20)
#define UNWIDENED_PROTOTYPES
#endif
#if defined(apollo)
#define UNWIDENED_PROTOTYPES
#endif

#ifndef UNWIDENED_PROTOTYPES
# if defined(NHSTDC) || defined(ULTRIX_PROTO) || defined(THINK_C)
# define WIDENED_PROTOTYPES
# endif
#endif


	/* MetaWare High-C defaults to unsigned chars */
	/* AIX 3.2 needs this also */
#if defined(__HC__) || defined(_AIX32)
# undef signed
#endif


/*
 * Allow gcc2 to check parameters of printf-like calls with -Wformat;
 * append this to a prototype declaration (see pline() in extern.h).
 */
#ifdef __GNUC__
# if __GNUC__ >= 2
#define PRINTF_F(f,v) __attribute__ ((format (printf, f, v)))
# endif
#endif
#ifndef PRINTF_F
#define PRINTF_F(f,v)
#endif

#endif /* TRADSTDC_H */
