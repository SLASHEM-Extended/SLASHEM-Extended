/* $Id: nhxdr.h,v 1.6 2002-11-30 19:15:17 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001-2002 */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef NHXDR_H
#define NHXDR_H

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef MIN
#define MIN(a, b)	((a) <= (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b)	((a) >= (b) ? (a) : (b))
#endif

#ifndef NDECL
/*
 * Assume an ANSI X3.159 compatible compiler
 */
#define NDECL(f)		(f)(void)
#define FDECL(f, p)		(f)p
#define VDECL(f, p)		(f)p
#define CHAR_P			int
#define SHORT_P			int
#define BOOLEAN_P		int
#endif

#define NHEXT_XDR_RNDUP(x)	(((x) - 1 | 3) + 1)

typedef enum { nhext_xdr_false = 0, nhext_xdr_true = 1 } nhext_xdr_bool_t;

enum nhext_xdr_op {
    NHEXT_XDR_ENCODE, NHEXT_XDR_DECODE, NHEXT_XDR_FREE, NHEXT_XDR_COUNT
};

typedef struct nhext_xdr {
    void *x_data;
    enum nhext_xdr_op x_op;
    nhext_xdr_bool_t (*x_write)(struct nhext_xdr *xdrs, const void *addr,
      int size);
    nhext_xdr_bool_t (*x_read)(struct nhext_xdr *xdrs, void *addr, int size);
    void (*x_destroy)(struct nhext_xdr *xdrs);
    unsigned int x_pos, x_size;
} NhExtXdr;

#define E extern

E unsigned int FDECL(nhext_xdr_getpos, (NhExtXdr *));
E nhext_xdr_bool_t FDECL(nhext_xdr_setpos, (NhExtXdr *, unsigned int));
E void FDECL(nhext_xdr_free, (nhext_xdr_bool_t (*)(), char *));
E void FDECL(nhext_xdrmem_create, (NhExtXdr *, char *, unsigned int,
			enum nhext_xdr_op));

#define nhext_xdr_destroy(xdrs)	(*(xdrs)->x_destroy)(xdrs)

E nhext_xdr_bool_t FDECL(nhext_xdr_long, (NhExtXdr *, long *));
E nhext_xdr_bool_t FDECL(nhext_xdr_u_long, (NhExtXdr *, unsigned long *));
E nhext_xdr_bool_t FDECL(nhext_xdr_short, (NhExtXdr *, short *));
E nhext_xdr_bool_t FDECL(nhext_xdr_u_short, (NhExtXdr *, unsigned short *));
E nhext_xdr_bool_t FDECL(nhext_xdr_int, (NhExtXdr *, int *));
E nhext_xdr_bool_t FDECL(nhext_xdr_u_int, (NhExtXdr *, unsigned int *));
E nhext_xdr_bool_t FDECL(nhext_xdr_char, (NhExtXdr *, char *));
E nhext_xdr_bool_t FDECL(nhext_xdr_u_char, (NhExtXdr *, unsigned char *));
E nhext_xdr_bool_t FDECL(nhext_xdr_enum, (NhExtXdr *, int *));
E nhext_xdr_bool_t FDECL(nhext_xdr_bool, (NhExtXdr *, nhext_xdr_bool_t *));
E nhext_xdr_bool_t FDECL(nhext_xdr_string, (NhExtXdr *, char **, unsigned int));
E nhext_xdr_bool_t FDECL(nhext_xdr_bytes, (NhExtXdr *, char **, unsigned int *,
  unsigned int));
E nhext_xdr_bool_t FDECL(nhext_xdr_wrapstring, (NhExtXdr *, char **));
E nhext_xdr_bool_t FDECL(nhext_xdr_vector, (NhExtXdr *, char *, unsigned int,
  unsigned int, nhext_xdr_bool_t (*)()));
E nhext_xdr_bool_t FDECL(nhext_xdr_array, (NhExtXdr *, char **, unsigned int *,
  unsigned int, unsigned int, nhext_xdr_bool_t (*)()));

typedef int (*nhext_io_func)(void *handle, void *buf, unsigned int len);

typedef struct NhExtIO_ NhExtIO;

#define NHEXT_IO_RDONLY		1
#define NHEXT_IO_WRONLY		2
#define NHEXT_IO_NOAUTOFILL	4
#define NHEXT_IO_SIMPLEBUFFER	8

E NhExtIO *FDECL(nhext_io_open, (nhext_io_func, void *, unsigned int));
E int FDECL(nhext_io_close, (NhExtIO *));
E unsigned int FDECL(nhext_io_getmode, (NhExtIO *));
E void FDECL(nhext_io_setmode, (NhExtIO *, unsigned int));
E void FDECL(nhext_io_setautofill_limit, (NhExtIO *, unsigned int));
E int FDECL(nhext_io_filbuf, (NhExtIO *));
E int FDECL(nhext_io_getc, (NhExtIO *));
E int FDECL(nhext_io_read, (NhExtIO *, char *, int));
E char *FDECL(nhext_io_getpacket, (NhExtIO *, int *));
E int FDECL(nhext_io_flush, (NhExtIO *));
E int FDECL(nhext_io_fputc, (int, NhExtIO *));
E int FDECL(nhext_io_write, (NhExtIO *, char *, int));

#endif /* NHXDR_H */
