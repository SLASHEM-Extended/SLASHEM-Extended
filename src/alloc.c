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

/* to get the definitons of ENOMEM and errno */
#if defined(NHALLOC_DLL) || defined(MONITOR_HEAP) && defined(INTERNAL_MALLOC)
#include <stdlib.h>
#include <errno.h>
#ifdef WIN32
#include <win32api.h>
#endif
#endif

#ifdef NHALLOC_DLL
#undef free
#else	/* NHALLOC_DLL */
#if defined(MONITOR_HEAP) || defined(WIZARD)
char *FDECL(fmt_ptr, (const genericptr,char *));
#endif

#ifdef MONITOR_HEAP
#undef alloc
#undef free
extern void FDECL(free,(genericptr_t));
static void NDECL(heapmon_init);

static FILE *heaplog = 0;
static boolean tried_heaplog = FALSE;
#endif

long *FDECL(alloc,(unsigned int));
extern void VDECL(panic, (const char *,...)) PRINTF_F(1,2);


long *
alloc(lth)
register unsigned int lth;
{
#ifdef LINT
/*
 * a ridiculous definition, suppressing
 *	"possible pointer alignment problem" for (long *) malloc()
 * from lint
 */
	long dummy = ftell(stderr);

	if(lth) dummy = 0;	/* make sure arg is used */
	return(&dummy);
#else
	register genericptr_t ptr;

	ptr = malloc(lth);
#ifndef MONITOR_HEAP
	if (!ptr) panic("Memory allocation failure; cannot get %u bytes", lth);
#endif
	return((long *) ptr);
#endif
}


#if defined(MONITOR_HEAP) || defined(WIZARD)

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
#  define PTR_TYP genericptr_t
# else
#  define PTR_FMT "%06lx"
#  define PTR_TYP unsigned long
# endif

/* format a pointer for display purposes; caller supplies the result buffer */
char *
fmt_ptr(ptr, buf)
const genericptr ptr;
char *buf;
{
	Sprintf(buf, PTR_FMT, (PTR_TYP)ptr);
	return buf;
}

#endif	/* MONITOR_HEAP || WIZARD */
#endif	/* !NHALLOC_DLL */

#if defined(MONITOR_HEAP) && !defined(NHALLOC_DLL)

/* If ${NH_HEAPLOG} is defined and we can create a file by that name,
   then we'll log the allocation and release information to that file. */
static void
heapmon_init()
{
	char *logname = getenv("NH_HEAPLOG");

	if (logname && *logname)
		heaplog = fopen(logname, "w");
	tried_heaplog = TRUE;
}

long *
nhalloc(lth, file, line)
unsigned int lth;
const char *file;
int line;
{
	long *ptr;
	char ptr_address[20];

#ifdef INTERNAL_MALLOC
	monitor_heap_push(file, line);
#endif
	ptr = alloc(lth);
#ifdef INTERNAL_MALLOC
	(void)monitor_heap_pop(file, line, 0);
#endif
	if (!tried_heaplog) heapmon_init();
	if (heaplog)
		(void) fprintf(heaplog, "+%5u %s %4d %s\n", lth,
				fmt_ptr((genericptr_t)ptr, ptr_address),
				line, file);
	/* potential panic in alloc() was deferred til here */
	if (!ptr) panic("Cannot get %u bytes, line %d of %s",
			lth, line, file);

	return ptr;
}

void
nhfree(ptr, file, line)
genericptr_t ptr;
const char *file;
int line;
{
	char ptr_address[20];

	if (!tried_heaplog) heapmon_init();
	if (heaplog)
		(void) fprintf(heaplog, "-      %s %4d %s\n",
				fmt_ptr((genericptr_t)ptr, ptr_address),
				line, file);

	free(ptr);
}
#endif	/* MONITOR_HEAP && !NHALLOC_DLL */

/*
 * Under Win32, the trivial allocator and monitor front end are
 * placed in nhalloc.dll rather than in the main executable.
 */

#if defined(NHALLOC_DLL) || (defined(INTERNAL_MALLOC) && !defined(WIN32))
/*
 * [Ali] A trivial malloc implementation.
 *
 * Notes:
 *
 * If attempting to port to a non-UNIX environment, you will need
 * a replacement for the sbrk() call. Under UNIX, sbrk() returns
 * contiguous blocks. If there is no equivalent call in the target
 * environment, you will either need to make this allocator rather
 * more intelligent, or just increase PAGESIZE to something huge like
 * 1Mb to keep the resultant fragmentation under control.
 *
 * The alignment implementation is advisory at best; we assume that
 * if the target enviroment is that bothered by alignment, sbrk()
 * will return aligned blocks and the compiler will pad the triv_head
 * structure appropriately. If either of these assumptions is not
 * correct, the alignment will be relaxed, which is presumably okay.
 *
 * This implementation is _very_ vulnerable to memory allocation bugs
 * in the main code. Don't enable it unless you are confident that
 * no such bugs exist.
 */

#define TRIV_ALIGN	sizeof(double)
#define TRIV_PAGESIZE	4096

struct triv_head {
    size_t triv_size;		/* Total block size (excluding header) */
    union {
	struct {
	    size_t nb;		/* Number of bytes allocated by user */
	    void *userdata;
	} alloc;
	struct triv_head *next;	/* Next free block */
	double d;		/* Dummy for alignment */
    } u;
#define triv_nb		u.alloc.nb
#define triv_userdata	u.alloc.userdata
#define triv_next	u.next	/* Free blocks only */
};

static struct triv_head *triv_freelist = NULL;
#ifdef WIN32
/*
 * MS-Windows rounds size request up to a system page, so we need to
 * ensure we always request an exact number of system pages. This
 * variable holds TRIV_PAGESIZE rounded up accordingly.
 */
static unsigned int triv_pagesize = 0;
#endif

#define ROUNDUP(nbytes, align) (((nbytes) + (align) - 1) / (align) * (align))

void *triv_get_userdata(void *p)
{
    struct triv_head *h;
    if (!p)
	return p;
    h = (struct triv_head *)p - 1;
    return h->triv_userdata;
}

void triv_set_userdata(void *p, void *d)
{
    struct triv_head *h;
    if (p) {
	h = (struct triv_head *)p - 1;
	h->triv_userdata = d;
    }
}

#define IS_CONTIGUOUS(h1,h2) ((h2) == \
	(struct triv_head *)((unsigned char *)((h1) + 1) + (h1)->triv_size))

void triv_free(void *p)
{
    struct triv_head *f, *lf, *h;
    if (!p)
	return;
    h = (struct triv_head *)p - 1;
    /* Find f, the first free block _after_ h in memory */
    for (lf = NULL, f = triv_freelist; f; lf = f, f = f->triv_next)
	if (f > h)
	    break;
    if (IS_CONTIGUOUS(h, f)) {
	/* f is contiguous with h; merge them */
	h->triv_size += sizeof(struct triv_head) + f->triv_size;
	h->triv_next = f->triv_next;
    }
    else
	/* f is not contiguous; insert new block */
	h->triv_next = f;
    /* Update pointer from previous block */
    if (lf)
	lf->triv_next = h;
    else
	triv_freelist = h;
    /* Then check last free block _below_ h in memory */
    if (lf && IS_CONTIGUOUS(lf, h)) {
	/* h is contiguous with lf; merge them */
	lf->triv_size += sizeof(struct triv_head) + h->triv_size;
	lf->triv_next = h->triv_next;
    }
}

void *triv_malloc(size_t nb)
{
    struct triv_head *f, *lf, *nf;
    size_t size = ROUNDUP(nb, TRIV_ALIGN);
    size_t pagesize;
    if (!nb)
	return NULL;			/* _Not_ an error; don't set errno */
    for (lf = NULL, f = triv_freelist; f; lf = f, f = f->triv_next)
	/* Use first block found which is large enough */
	if (f->triv_size >= size) {
	    /* Split the free block if there's enough space left over */
	    if (f->triv_size - size > sizeof(struct triv_head)) {
		nf = (struct triv_head *)((unsigned char *)(f + 1) + size);
		nf->triv_size = f->triv_size - size - sizeof(struct triv_head);
		nf->triv_next = f->triv_next;
		f->triv_next = nf;
		f->triv_size = size;
	    }
	    /* Remove block from freelist */
	    if (lf)
		lf->triv_next = f->triv_next;
	    else
		triv_freelist = f->triv_next;
	    /* Initialise header and return */
	    f->triv_nb = nb;
	    f->triv_userdata = NULL;
	    return f + 1;
	}
    /* Get new block */
#ifdef WIN32
    if (triv_pagesize == 0) {
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	triv_pagesize = ROUNDUP(TRIV_PAGESIZE, si.dwPageSize);
    }
    pagesize = ROUNDUP(sizeof(struct triv_head) + size, triv_pagesize);
    f = VirtualAlloc(NULL, pagesize, MEM_COMMIT, PAGE_READWRITE);
#else
    pagesize = ROUNDUP(sizeof(struct triv_head) + size, TRIV_PAGESIZE);
    f = sbrk(pagesize);
#endif
    if (!f) {
	errno = ENOMEM;
	return f;
    }
    /* Initialise header */
    f->triv_size = pagesize - sizeof(struct triv_head);
    f->triv_nb = nb;
    f->triv_userdata = NULL;
    /* Split the new block if there's enough space left over */
    if (f->triv_size - size > sizeof(struct triv_head)) {
	nf = (struct triv_head *)((unsigned char *)(f + 1) + size);
	nf->triv_size = f->triv_size - size - sizeof(struct triv_head);
	f->triv_size = size;
	/* And contribute it to the free list */
	triv_free(nf + 1);
    }
    return f + 1;
}

void *triv_calloc(size_t nobj, size_t size)
{
    void *p;
    size_t nb = nobj * size;
    if (nb / nobj != size) {		/* Check overflow */
	errno = ENOMEM;
	return NULL;
    }
    p = triv_malloc(nb);
    if (p)
	memset(p, 0, nb);
    return p;
}

void *triv_realloc(void *p, size_t size)
{
    struct triv_head *h;
    void *np;
    if (!size) {
	triv_free(p);
	return NULL;
    }
    if (!p)
	return triv_malloc(size);
    h = (struct triv_head *)p - 1;
    if (size <= h->triv_size) {
	h->triv_nb = size;
	return p;
    }
    np = triv_malloc(size);
    if (np) {
	memcpy(np, p, h->triv_nb);
	triv_set_userdata(np, triv_get_userdata(p));
	triv_free(p);
    }
    return np;
}

/* Monitoring front-end */

struct monitor_id {
    const char *id;			/* Typically file name */
    int subid;				/* Typically line number */
};
    
static char *monitor_id_stack_default_id = "<none>";
static int monitor_id_stack_depth = 0;
static struct monitor_id *monitor_id_stack = NULL;
static size_t monitor_mem_in_use = 0;
static boolean monitor_trace = 0;
static FILE *monitor_fp = NULL;

#define MHF_MARKED	1

struct monitor_head {
    void *p;
    size_t size;
    struct monitor_id id;
    unsigned long flags;
    struct monitor_head *next, *prev;
};

static struct monitor_head *monitor_heap = NULL;

void monitor_heap_push(const char *id, int subid)
{
    ++monitor_id_stack_depth;
    monitor_id_stack = triv_realloc(monitor_id_stack,
      monitor_id_stack_depth * sizeof (*monitor_id_stack));
    if (!monitor_id_stack)
	panic("monitor_heap_push: not enough memory");
    monitor_id_stack[monitor_id_stack_depth - 1].id = id;
    monitor_id_stack[monitor_id_stack_depth - 1].subid = subid;
}

/* retval is to make writing macros which use monitor_heap_push/pop easier */

unsigned long monitor_heap_pop(const char *id, int subid, unsigned long retval)
{
    if (!monitor_id_stack_depth)
	panic("monitor_heap_pop: empty stack");
    if (monitor_id_stack[monitor_id_stack_depth - 1].id != id ||
      monitor_id_stack[monitor_id_stack_depth - 1].subid != subid)
	panic("monitor_heap_pop: mismatch: (%s, %d) != (%s, %d)",
	  monitor_id_stack[monitor_id_stack_depth - 1].id,
	  monitor_id_stack[monitor_id_stack_depth - 1].subid,
	  id, subid);
    --monitor_id_stack_depth;
    monitor_id_stack = triv_realloc(monitor_id_stack,
      monitor_id_stack_depth * sizeof (*monitor_id_stack));
    return retval;
}

void monitor_heap_set_subid(const char *id, int subid)
{
    if (!monitor_id_stack_depth)
	panic("monitor_heap_set_subid: empty stack");
    if (monitor_id_stack[monitor_id_stack_depth - 1].id != id)
	panic("monitor_heap_set_subid: mismatch: %s != %s",
	  monitor_id_stack[monitor_id_stack_depth - 1].id, id);
    monitor_id_stack[monitor_id_stack_depth - 1].subid = subid;
}

size_t monitor_heap_getmem(void)
{
    return monitor_mem_in_use;
}

boolean monitor_heap_trace(boolean flag)
{
    boolean retval = !!monitor_trace;
    if (flag)
	monitor_trace++;
    else
	monitor_trace--;
    return retval;
}

void monitor_heap_mark(void)
{
    struct monitor_head *m;
    for(m = monitor_heap; m; m = m->next)
	m->flags |= MHF_MARKED;
}

void monitor_heap_release(void)
{
    int first = 1;
    struct monitor_head *m;
    if (!monitor_fp)
	return;
    for(m = monitor_heap; m; m = m->next)
	m->flags ^= MHF_MARKED;
    for(m = monitor_heap; m; m = m->next)
	if (m->flags & MHF_MARKED) {
	    if (first) {
		fprintf(monitor_fp,
		  "Blocks allocated but not freed during mark/release:\n");
		fprintf(monitor_fp, "Address         Size    Sub-ID  ID\n");
		first = 0;
	    }
	    fprintf(monitor_fp,
	      "%-16p%-8lu%-8d%s\n", m->p, m->size, m->id.subid, m->id.id);
	}
}

static void monitor_dump(void)
{
    int first = 1;
    struct monitor_head *m;
    FILE *fp;
    size_t used = 0;
    if (!monitor_fp)
	return;
    monitor_heap_mark();
    fprintf(monitor_fp, "Dump of all monitored blocks in heap:\n");
    for(m = monitor_heap; m; m = m->next) {
	if (m->flags & MHF_MARKED) {
	    used += m->size;
	    if (m->id.id == monitor_id_stack_default_id)
		continue;
	    if (first) {
		fprintf(monitor_fp, "Address         Size    Sub-ID  ID\n");
		first = 0;
	    }
	    fprintf(monitor_fp,
	      "%-16p%-8lu%-8d%s\n", m->p, m->size, m->id.subid, m->id.id);
	}
    }
    if (first)
	fprintf(monitor_fp,
	  "No monitored blocks in heap with non-default ID.\n");
    fprintf(monitor_fp, "Total used space: %lu bytes\n", used);
}

void *malloc(size_t nb)
{
    static int busy = 0;
    static int inited = 0;
    struct monitor_head *m;
    if (!busy && !(inited&1) && !monitor_id_stack_depth) {
	inited|=1;
	monitor_heap_push(monitor_id_stack_default_id, 0);
    }
    if (!busy && !(inited&2)) {
	char *s;
	inited|=2;
	atexit(monitor_dump);
	s = getenv("NH_HEAPDUMP");
	monitor_fp = s ? fopen(s, "w"): NULL;
    }
    if (!nb)
	return NULL;
    busy++;
    m = triv_malloc(sizeof(*m));
    if (!m) {
	busy--;
	return m;
    }
    m->size = nb;
    m->flags = 0;
    m->p = triv_malloc(nb);
    if (monitor_trace)
	fprintf(stderr, "malloc(%d) = %p\n", nb, m->p);
    if (!m->p) {
	triv_free(m);
	busy--;
	return NULL;
    }
    monitor_mem_in_use += nb;
    m->prev = NULL;
    m->next = monitor_heap;
    m->id = monitor_id_stack[monitor_id_stack_depth - 1];
    if (monitor_heap)
	monitor_heap->prev = m;
    monitor_heap = m;
    triv_set_userdata(m->p, m);
    busy--;
    return m->p;
}

void *calloc(size_t nobj, size_t size)
{
    void *p;
    size_t nb = nobj * size;
    if (nb / nobj != size) {		/* Check overflow */
	errno = ENOMEM;
	return NULL;
    }
    p = malloc(nb);
    if (p)
	memset(p, 0, nb);
    return p;
}

void *realloc(void *p, size_t size)
{
    struct monitor_head *m;
    void *np;
    if (!size) {
	free(p);
	return NULL;
    }
    if (!p)
	return malloc(size);
    m = triv_get_userdata(p);
    np = triv_realloc(p, size);
    if (monitor_trace)
	fprintf(stderr, "realloc(%p, %d) = %p\n", p, size, np);
    if (np) {
	monitor_mem_in_use += size - m->size;
	m->size = size;
	m->p = np;
    }
    return np;
}

void free(void *p)
{
    struct monitor_head *m;
    if (p)
    {
	if (monitor_trace)
	    fprintf(stderr, "free(%p)\n", p);
	m = triv_get_userdata(p);
	monitor_mem_in_use -= m->size;
	triv_free(p);
	if (m->prev)
	    m->prev->next = m->next;
	else
	    monitor_heap = m->next;
	if (m->next)
	    m->next->prev = m->prev;
	triv_free(m);
    }
}

#endif /* NHALLOC_DLL || INTERNAL_MALLOC && !WIN32 */

/*alloc.c*/
