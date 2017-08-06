/*	SCCS Id: @(#)alloc.c	3.4	1995/10/04	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* to get the malloc() prototype from system.h */
#define ALLOC_C		/* comment line for pre-compiled headers */
/* since this file is also used in auxiliary programs, don't include all the
 * function declarations for all of nethack
 */
#define EXTERN_H	/* comment line for pre-compiled headers */
#include "config.h"

/* [Ali]
 * There are a number of preprocessor symbols which affect how this
 * file is compiled:
 *
 * NHALLOC_DLL	When set this causes the alloc module to build as a stand-alone
 *		allocator (which can either be used in object form for use
 *		with utility programs or combined with panic.o to form a DLL
 *		for dynamic linking with the game - this last allows for the
 *		possibility of 3rd party libraries to allocate memory via the
 *		module and thus be represented in the heap monitoring).
 *
 *		Global symbols defined: malloc, calloc etc.
 *		                        monitor_heap_push, monitor_heap_pop etc.
 *
 *		Warning: The version of panic defined in panic.o does _not_
 *		attempt to save the game. nhalloc.dll should therefore only be
 *		used for testing.
 *
 * WIZARD	When set this causes the definition of the fmt_ptr function.
 *
 * MONITOR_HEAP	When set this causes the alloc module to define the nhalloc and
 *		nhfree functions (which can then be used by the alloc and free
 *		macros). This allows some basic monitoring of the heap for
 *		memory leaks by dumping the heap to a log file on exit.
 *
 *		Note: This symbol also causes fmt_ptr to be defined.
 *
 *		Note: If the INTERNAL_MALLOC symbol is also defined, then
 *		nhalloc and nhfree will use the monitor_heap_ functions to
 *		store information about where the memory was allocated from.
 *		Unless compiling on the WIN32 platform, this will also cause
 *		the definition of the monitor_heap_ functions, the trivial
 *		allocator and a standard front end (malloc and friends).
 *		On the WIN32 platform, the monitor_heap_ and standard allocator
 *		functions are left as external references.
 */

#ifdef WIZARD
char *fmt_ptr(const void *,char *);
#endif

extern void panic(const char *,...) PRINTF_F(1,2);

void * alloc(size_t lth) {
	void * ptr;

	ptr = calloc(1, lth);

	if (!ptr) panic("Memory allocation failure; cannot get %zu bytes", lth);

	return ptr;
}


#ifdef WIZARD

# if defined(MICRO) || defined(WIN32)
/* we actually want to know which systems have an ANSI run-time library
 * to know which support the new %p format for printing pointers.
 * due to the presence of things like gcc, NHSTDC is not a good test.
 * so we assume microcomputers have all converted to ANSI and bigger
 * computers which may have older libraries give reasonable results with
 * the cast.
 */
#  define MONITOR_PTR_FMT
# endif

# ifdef MONITOR_PTR_FMT
#  define PTR_FMT "%p"
#  define PTR_TYP void *
# else
#  define PTR_FMT "%06lx"
#  define PTR_TYP unsigned long
# endif

/* format a pointer for display purposes; caller supplies the result buffer */
char *
fmt_ptr(ptr, buf)
const void * ptr;
char *buf;
{
	sprintf(buf, PTR_FMT, (PTR_TYP)ptr);
	return buf;
}

#endif	/* WIZARD */
/*alloc.c*/
