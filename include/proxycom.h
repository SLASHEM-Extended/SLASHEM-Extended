/* $Id: proxycom.h,v 1.1 2002-10-09 16:20:34 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2002 */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef PROXYCOM_H
#define PROXYCOM_H

typedef int (*nhext_io_func)(void *handle, void *buf, unsigned int len);

typedef struct NhExtIO_ NhExtIO;

#define NHEXT_IO_RDONLY		1
#define NHEXT_IO_WRONLY		2
#define NHEXT_IO_NOAUTOFILL	4

NhExtIO *nhext_io_open(nhext_io_func func, void *handle, unsigned int flags);
int nhext_io_close(NhExtIO *io);
unsigned int nhext_io_getmode(NhExtIO *io);
void nhext_io_setmode(NhExtIO *io, unsigned int flags);
int nhext_io_filbuf(NhExtIO *io);
int nhext_io_getc(NhExtIO *io);
int nhext_io_read(NhExtIO *io, char *buf, int nb);
int nhext_io_flush(NhExtIO *io);
int nhext_io_fputc(int c, NhExtIO *io);
int nhext_io_write(NhExtIO *io, char *buf, int nb);

#endif /* PROXYCOM_H */
