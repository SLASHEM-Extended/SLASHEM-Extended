/* $Id: nhxdr.h,v 1.3 2002-07-10 16:31:23 j_ali Exp $ */
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

#define NHEXT_XDR_RNDUP(x)	(((x) - 1 | 3) + 1)

enum nhext_xdr_op {
    NHEXT_XDR_ENCODE, NHEXT_XDR_DECODE, NHEXT_XDR_FREE
};

typedef struct nhext_xdr {
    unsigned char *x_buf;
    enum nhext_xdr_op x_op;
    boolean (*x_write)(struct nhext_xdr *xdrs, const void *addr, int size);
    boolean (*x_read)(struct nhext_xdr *xdrs, void *addr, int size);
    void (*x_destroy)(struct nhext_xdr *xdrs);
    unsigned int x_pos, x_size;
} NhExtXdr;

#define E extern

E unsigned int FDECL(nhext_xdr_getpos, (NhExtXdr *));
E boolean FDECL(nhext_xdr_setpos, (NhExtXdr *, unsigned int));
E void FDECL(nhext_xdr_free, (boolean (*)(), char *));
E void FDECL(nhext_xdrmem_create, (NhExtXdr *, char *, unsigned int,
			enum nhext_xdr_op));

#define nhext_xdr_destroy(xdrs)	(*(xdrs)->x_destroy)(xdrs)

E boolean FDECL(nhext_xdr_long, (NhExtXdr *, long *));
E boolean FDECL(nhext_xdr_u_long, (NhExtXdr *, unsigned long *));
E boolean FDECL(nhext_xdr_short, (NhExtXdr *, short *));
E boolean FDECL(nhext_xdr_u_short, (NhExtXdr *, unsigned short *));
E boolean FDECL(nhext_xdr_int, (NhExtXdr *, int *));
E boolean FDECL(nhext_xdr_u_int, (NhExtXdr *, unsigned int *));
E boolean FDECL(nhext_xdr_char, (NhExtXdr *, char *));
E boolean FDECL(nhext_xdr_u_char, (NhExtXdr *, unsigned char *));
E boolean FDECL(nhext_xdr_enum, (NhExtXdr *, int *));
E boolean FDECL(nhext_xdr_bool, (NhExtXdr *, boolean *));
E boolean FDECL(nhext_xdr_string, (NhExtXdr *, char **, unsigned int));
E boolean FDECL(nhext_xdr_bytes, (NhExtXdr *, char **, unsigned int *,
  unsigned int));
E boolean FDECL(nhext_xdr_wrapstring, (NhExtXdr *, char **));
E boolean FDECL(nhext_xdr_vector, (NhExtXdr *, char *, unsigned int,
  unsigned int, boolean (*)()));
E boolean FDECL(nhext_xdr_array, (NhExtXdr *, char **, unsigned int *,
  unsigned int, unsigned int, boolean (*)()));

#endif /* NHXDR_H */
