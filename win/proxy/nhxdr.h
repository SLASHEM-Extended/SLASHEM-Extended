/* $ Id: $ */
/* Copyright (c) Slash'EM Development Team 2001 */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef NHXDR_H
#define NHXDR_H

typedef struct nhext_xdr {
} NhExtXdr;

#define E extern

E unsigned int FDECL(nhext_xdr_getpos, (NhExtXdr *));
E unsigned int FDECL(nhext_xdr_setpos, (NhExtXdr *, unsigned int));
E void FDECL(nhext_xdrmem_create, (NhExtXdr *, char *, unsigned int,
			enum nhext_xdr_op));
E void FDECL(nhext_xdr_destroy, (NhExtXdr *));

#endif /* NHXDR_H */
