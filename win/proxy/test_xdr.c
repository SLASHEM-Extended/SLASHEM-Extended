/* $Id: test_xdr.c,v 1.3 2002-11-02 15:47:04 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001 */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * This module tests the NhExt implementation of XDR by using it to write
 * and read data to and from the equivalent code produced by rpcgen.
 *
 * Note: You need rpcgen to build this test program.
 *	 It should be built once with RPCGEN defined to get a version which
 *	 uses Sun Microsystems's rpc library and once without to get a
 *	 version which uses our implementation. The two can then be tested
 *	 for compatibility.
 */

#include <stdio.h>

#ifdef RPCGEN
#include <rpc/rpc.h>
#else
#include "hack.h"
#include "nhxdr.h"
#endif	/* RPCGEN */

#ifdef RPCGEN
#define SIZE(array)		(sizeof(array)/sizeof(*(array)))
#define nhext_xdr_bool_t	bool_t

#define NHEXT_XDR_DECODE	XDR_DECODE
#define NHEXT_XDR_ENCODE	XDR_ENCODE
#define NHEXT_XDR_FREE		XDR_FREE

#define nhext_xdr_op		xdr_op
typedef XDR NhExtXdr;
typedef xdrproc_t nhext_xdrproc_t;

#define nhext_xdrmem_create	xdrmem_create
#define nhext_xdr_getpos	xdr_getpos
#define nhext_xdr_setpos	xdr_setpos
#define nhext_xdr_destroy	xdr_destroy
#define nhext_xdr_short		xdr_short
#define nhext_xdr_long		xdr_long
#define nhext_xdr_int		xdr_int
#define nhext_xdr_char		xdr_char
#define nhext_xdr_u_short	xdr_u_short
#define nhext_xdr_u_long	xdr_u_long
#define nhext_xdr_u_int		xdr_u_int
#define nhext_xdr_u_char	xdr_u_char
#define nhext_xdr_enum		xdr_enum
#define nhext_xdr_bool		xdr_bool
#define nhext_xdr_vector	xdr_vector
#define nhext_xdr_array		xdr_array
#define nhext_xdr_string	xdr_string
#else
typedef nhext_xdr_bool_t (*nhext_xdrproc_t) ();
#endif	/* RPCGEN */

int
test_long(xdrs, op)
NhExtXdr *xdrs;
enum nhext_xdr_op op;
{
    int i;
    long l;
    for(i = 0; i < 8; i++) {
	l = i;
	if (!nhext_xdr_long(xdrs, &l)) {
	    fprintf(stderr,"%s", "nhext_xdr_long failed\n");
	    return FALSE;
	}
	if (op == NHEXT_XDR_DECODE)
	    printf("%ld ", l);
    }
    if (op == NHEXT_XDR_DECODE)
	putchar('\n');
    return TRUE;
}

int
test_vector(xdrs, op)
NhExtXdr *xdrs;
enum nhext_xdr_op op;
{
    int i;
    short vector[8];
    for(i = 0; i < 8; i++)
	vector[i] = i;
    if (!nhext_xdr_vector(xdrs, (char *)vector, SIZE(vector), sizeof(short),
      (nhext_xdrproc_t)nhext_xdr_short)) {
	fprintf(stderr,"%s", "nhext_xdr_vector failed\n");
	return FALSE;
    }
    if (op == NHEXT_XDR_DECODE) {
	printf("[ ");
	for(i = 0; i < 8; i++)
	    printf("%d ", vector[i]);
	putchar(']');
	putchar('\n');
    }
    return TRUE;
}

int
test_array(xdrs, op)
NhExtXdr *xdrs;
enum nhext_xdr_op op;
{
    int i;
    unsigned int length;
    unsigned char *array;
    if (op == NHEXT_XDR_ENCODE)
    {
	array = malloc(5);
	if (!array) {
	    fprintf(stderr,"%s", "malloc failed\n");
	    return FALSE;
	}
	array[0] = 'h'; array[1] = 'e'; array[2] = array[3] = 'l'; array[4] = 'o';
	length = 5;
    }
    else
	array = NULL;
    if (!nhext_xdr_array(xdrs, (char **)&array, &length, (unsigned int)-1,
      sizeof(unsigned char), (nhext_xdrproc_t)nhext_xdr_u_char)) {
	fprintf(stderr,"%s", "nhext_xdr_array failed\n");
	return FALSE;
    }
    if (op == NHEXT_XDR_DECODE) {
	printf("< ");
	for(i = 0; i < length; i++)
	    printf("'%c' ", array[i]);
	putchar('>');
	putchar('\n');
    }
    free(array);
    array = NULL;
    length = 0;
    if (!nhext_xdr_array(xdrs, (char **)&array, &length, (unsigned int)-1,
      sizeof(unsigned char), (nhext_xdrproc_t)nhext_xdr_u_char)) {
	fprintf(stderr,"%s", "nhext_xdr_array failed\n");
	return FALSE;
    }
    if (op == NHEXT_XDR_DECODE) {
	printf("< ");
	for(i = 0; i < length; i++)
	    printf("'%c' ", array[i]);
	putchar('>');
	putchar('\n');
    }
    if (array) {
	fprintf(stderr,"%s", "nhext_xdr_array allocated on empty array\n");
	return FALSE;
    }
    return TRUE;
}

int
test_string(xdrs, op)
NhExtXdr *xdrs;
enum nhext_xdr_op op;
{
    int i;
    char *string;
    if (op == NHEXT_XDR_ENCODE)
	string = strdup("Hello world!");	/* Multiple of 4 bytes */
    else
	string = NULL;
    if (!nhext_xdr_string(xdrs, &string, (unsigned int)-1)) {
	fprintf(stderr,"%s", "nhext_xdr_string failed\n");
	return FALSE;
    }
    if (op == NHEXT_XDR_DECODE)
	printf("\"%s\"\n", string);
    free(string);
    if (op == NHEXT_XDR_ENCODE)
	string = strdup("Goodbye");	/* Not a multiple of 4 bytes */
    else
	string = NULL;
    if (!nhext_xdr_string(xdrs, &string, (unsigned int)-1)) {
	fprintf(stderr,"%s", "nhext_xdr_string failed\n");
	return FALSE;
    }
    if (op == NHEXT_XDR_DECODE)
	printf("\"%s\"\n", string);
    free(string);
    if (op == NHEXT_XDR_ENCODE)
	string = strdup("");
    else
	string = NULL;
    if (!nhext_xdr_string(xdrs, &string, (unsigned int)-1)) {
	fprintf(stderr,"%s", "nhext_xdr_string failed\n");
	return FALSE;
    }
    if (op == NHEXT_XDR_DECODE)
	printf("\"%s\"\n", string);
    free(string);
    return TRUE;
}

int
test_bool(xdrs, op)
NhExtXdr *xdrs;
enum nhext_xdr_op op;
{
    nhext_xdr_bool_t bool = TRUE;
    if (!nhext_xdr_bool(xdrs, &bool)) {
	fprintf(stderr,"%s", "nhext_xdr_bool failed\n");
	return FALSE;
    }
    if (op == NHEXT_XDR_DECODE)
	printf("%s ", bool == TRUE ? "TRUE" : bool == FALSE ? "FALSE" : "???");
    bool = FALSE;
    if (!nhext_xdr_bool(xdrs, &bool)) {
	fprintf(stderr,"%s", "nhext_xdr_bool failed\n");
	return FALSE;
    }
    if (op == NHEXT_XDR_DECODE)
	printf("%s\n", bool == TRUE ? "TRUE" : bool == FALSE ? "FALSE" : "???");
    return TRUE;
}

int
test_enum(xdrs, op)
NhExtXdr *xdrs;
enum nhext_xdr_op op;
{
    enum nhext_xdr_op e;
    e = NHEXT_XDR_DECODE;
    if (!nhext_xdr_enum(xdrs, (int *)&e)) {
	fprintf(stderr,"%s", "nhext_xdr_enum failed\n");
	return FALSE;
    }
    if (op == NHEXT_XDR_DECODE)
	switch(e)
	{
	    case NHEXT_XDR_DECODE:	printf("NHEXT_XDR_DECODE "); break;
	    case NHEXT_XDR_ENCODE:	printf("NHEXT_XDR_ENCODE "); break;
	    case NHEXT_XDR_FREE:	printf("NHEXT_XDR_FREE "); break;
	    default:			printf("??? "); break;
	}
    e = NHEXT_XDR_ENCODE;
    if (!nhext_xdr_enum(xdrs, (int *)&e)) {
	fprintf(stderr,"%s", "nhext_xdr_enum failed\n");
	return FALSE;
    }
    if (op == NHEXT_XDR_DECODE)
	switch(e)
	{
	    case NHEXT_XDR_DECODE:	printf("NHEXT_XDR_DECODE\n"); break;
	    case NHEXT_XDR_ENCODE:	printf("NHEXT_XDR_ENCODE\n"); break;
	    case NHEXT_XDR_FREE:	printf("NHEXT_XDR_FREE\n"); break;
	    default:			printf("???\n"); break;
	}
    return TRUE;
}

int
codec(buf, len, maxlen, op)
char *buf;
unsigned int *len;
unsigned int maxlen;
enum nhext_xdr_op op;
{
    int retval;
    NhExtXdr xdrs;
    unsigned int pos;
    nhext_xdrmem_create(&xdrs, buf, maxlen, op);
    if (op == NHEXT_XDR_ENCODE)
	pos = nhext_xdr_getpos(&xdrs);
    retval = test_long(&xdrs, op) &&
      test_vector(&xdrs, op) &&
      test_array(&xdrs, op) &&
      test_string(&xdrs, op) &&
      test_bool(&xdrs, op) &&
      test_enum(&xdrs, op);
    if (op == NHEXT_XDR_ENCODE)
	*len = nhext_xdr_getpos(&xdrs) - pos;
    nhext_xdr_destroy(&xdrs);
    return retval;
}

reader()
{
    unsigned int nb;
    char buf[200];
    nb = fread(buf, 1, sizeof(buf), stdin);
    return codec(buf, &nb, nb, NHEXT_XDR_DECODE);
}

writer()
{
    unsigned int nb;
    char buf[200];
    if (!codec(buf, &nb, sizeof(buf), NHEXT_XDR_ENCODE))
	return FALSE;
    fwrite(buf, 1, nb, stdout);
    return TRUE;
}

main(argc, argv)
int argc;
char **argv;
{
    int retval;
    if (argc != 2 || argv[1][0]!='-' || argv[1][1]!='r' && argv[1][1]!='w' ||
      argv[1][2]) {
	fprintf(stderr, "Usage: %s -r | -w\n", argv[0]);
	exit(1);
    }
    if (argv[1][1]=='r')
	retval = reader();
    else
	retval = writer();
    exit(!retval);
}
