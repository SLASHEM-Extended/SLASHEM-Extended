/* $Id: test_ext.c,v 1.5 2002-11-30 19:15:18 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001-2002 */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * This module tests the NhExt support routines for sub-protocol 1.
 * These include nhext_init(), nhext_end() nhext_rpc() and nhext_svc(),
 * all of which can be found in nhext.c.
 *
 * Note: This module does not test the implementation of sub-protocol 1
 * itself (which is found in winproxy.c).
 */

#include <stdio.h>
#include <stdarg.h>

#include "hack.h"
#include "nhxdr.h"
#include "proxycom.h"

static int is_child=0;
static int connection, server_connection;

long *alloc(unsigned int nb)
{
    return malloc(nb);
}

void impossible(const char *fmt,...)
{
    int i;
    va_list args;
    va_start(args, fmt);
    fputs("impossible: ", stderr);
    if (is_child)
	fputs("C: ", stderr);
    vfprintf(stderr, fmt, args);
    putc('\n', stderr);
    fflush(stderr);
    va_end(args);
    if (!is_child) {
	for(i=0;i<10 && !child_wait0();i++)
	    sleep(1);
    }
    exit(126);
}

#ifdef WIN32
#include <windows.h>

static PROCESS_INFORMATION pi;
static HANDLE to_parent[2], to_child[2];

/* Create an anonymous pipe with one end inheritable. */

static int pipe_create(HANDLE *handles, int non_inherit)
{
    HANDLE h;
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    if (!CreatePipe(&handles[0], &handles[1], &sa, 0))
	return FALSE;
    if (!DuplicateHandle(GetCurrentProcess(), handles[non_inherit],
      GetCurrentProcess(), &h, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
	CloseHandle(handles[0]);
	CloseHandle(handles[1]);
	return FALSE;
    }
    CloseHandle(handles[non_inherit]);
    handles[non_inherit] = h;
    return TRUE;
}

static void pipe_close(HANDLE *handles)
{
    CloseHandle(handles[0]);
    CloseHandle(handles[1]);
}

int child_start()
{
    int fd, retval;
    HANDLE save_stdin, save_stdout;
    STARTUPINFO si;
    if (!pipe_create(to_parent, 0))
	return FALSE;
    if (!pipe_create(to_child, 1)) {
	pipe_close(to_parent);
	return FALSE;
    }
    save_stdin = GetStdHandle(STD_INPUT_HANDLE);
    save_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!SetStdHandle(STD_INPUT_HANDLE, to_child[0])) {
	pipe_close(to_parent);
	pipe_close(to_child);
	return FALSE;
    }
    if (!SetStdHandle(STD_OUTPUT_HANDLE, to_parent[1])) {
	SetStdHandle(STD_INPUT_HANDLE, save_stdin);
	pipe_close(to_parent);
	pipe_close(to_child);
	return FALSE;
    }
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    retval = CreateProcess(NULL,		/* No module name */
      "test_ext -c",			/* Command line */
      NULL,				/* Process handle not inheritable */
      NULL,				/* Thread handle not inheritable */
      TRUE,				/* Inherits parent's handles */
#if 0
      DETACHED_PROCESS,			/* Creation flags */
#else
      0,				/* Creation flags */
#endif
      NULL,				/* Use parent's environment */
      NULL,				/* Use parent's starting dir */
      &si, &pi);
    SetStdHandle(STD_INPUT_HANDLE, save_stdin);
    SetStdHandle(STD_OUTPUT_HANDLE, save_stdout);
    if (!retval) {
	pipe_close(to_parent);
	pipe_close(to_child);
    }
    else {
	CloseHandle(to_parent[1]);
	CloseHandle(to_child[0]);
    }
    return retval;
}

int child_read(handle, buf, len)
void *handle;
void *buf;
int len;
{
    DWORD nb;
    if (!ReadFile((HANDLE)handle, buf, len, &nb, NULL))
	return -1;
    else
	return nb;
}

int child_write(handle, buf, len)
void *handle;
void *buf;
int len;
{
    DWORD nb;
    if (!WriteFile((HANDLE)handle, buf, len, &nb, NULL))
	return -1;
    else
	return nb;
}

int child_wait0()
{
    return WaitForSingleObject(pi.hProcess, 0) == WAIT_OBJECT_0;
}

int child_wait()
{
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    pipe_close(to_parent);
    pipe_close(to_child);
    return TRUE;
}
#else	/* WIN32 */
static int pid;
static int to_parent[2], to_child[2];

int child_start()
{
    if (pipe(to_child))
	return FALSE;
    if (pipe(to_parent)) {
	close(to_child[0]);
	close(to_child[1]);
	return FALSE;
    }
    pid = fork();
    if (pid < 0) {
	close(to_child[0]);
	close(to_child[1]);
	close(to_parent[0]);
	close(to_parent[1]);
	return FALSE;
    }
    else if (!pid) {
	close(to_child[1]);
	close(to_parent[0]);
	dup2(to_child[0],0);
	dup2(to_parent[1],1);
	execl("test_ext", "test_ext", "-c", NULL);
	_exit(127);
    }
    else {
	close(to_child[0]);
	close(to_parent[1]);
    }
    return TRUE;
}

int child_read(handle, buf, len)
void *handle;
void *buf;
int len;
{
    return read((int)handle, buf, len);
}

int child_write(handle, buf, len)
void *handle;
void *buf;
int len;
{
    return write((int)handle, buf, len);
}

static int child__wait(options)
{
    int status;
    if (waitpid(pid,&status,options) == pid) {
	if (!WIFEXITED(status)) {
	    if (WIFSIGNALED(status))
		fprintf(stderr, "Child died due to signal %d\n",
		  WTERMSIG(status));
	    else
		fprintf(stderr, "Child died for unknown reason\n");
	}
	else if (WEXITSTATUS(status))
	    fprintf(stderr, "Child exited with code %d\n", WEXITSTATUS(status));
	return TRUE;
    } else
	return FALSE;
}

int child_wait0()
{
    return child__wait(WNOHANG);
}

int child_wait()
{
    close(to_child[1]);
    close(to_parent[0]);
    return child__wait(0);
}
#endif	/* WIN32 */

int debug_read(handle, buf, len)
void *handle;
void *buf;
unsigned int len;
{
    int i, retval;
    long l;
    unsigned char *bp = buf;
    retval = child_read(handle, buf, len);
    if (retval < 0)
	fputs("<- ERROR\n", stderr);
    else {
	for(i = 0; i < retval; ) {
	    if ((i & 7) == 0) {
		if (!i)
		    fputs("<-", stderr);
		else
		    fputs("\n  ", stderr);
	    }
	    if (retval - i >= 4) {
		l = (long)bp[i] << 24 | (long)bp[i+1] << 16 |
		  (long)bp[i+2] << 8 | bp[i+3];
		fprintf(stderr, " %08X", l);
		i += 4;
	    }
	    else {
		fprintf(stderr, " %02X", bp[i]);
		i++;
	    }
	}
	fputc('\n', stderr);
    }
    return retval;
}

int debug_write(handle, buf, len)
void *handle;
void *buf;
unsigned int len;
{
    int i, retval;
    long l;
    unsigned char *bp = buf;
    retval = child_write(handle, buf, len);
    if (retval < 0)
	fputs("-> ERROR\n", stderr);
    else {
	for(i = 0; i < retval; ) {
	    if ((i & 7) == 0) {
		if (!i)
		    fputs("->", stderr);
		else
		    fputs("\n  ", stderr);
	    }
	    if (retval - i >= 4) {
		l = (long)bp[i] << 24 | (long)bp[i+1] << 16 |
		  (long)bp[i+2] << 8 | bp[i+3];
		fprintf(stderr, " %08X", l);
		i += 4;
	    }
	    else {
		fprintf(stderr, " %02X", bp[i]);
		i++;
	    }
	}
	fputc('\n', stderr);
    }
    return retval;
}

volatile server_exit = 0;

#define EXT_FID_EXIT	0xFFFF
#define EXT_FID_TEST1	1
#define EXT_FID_TEST2	2
#define EXT_FID_TEST3	3
#define EXT_FID_TEST4	4
#define EXT_FID_TEST5	5
#define EXT_FID_TEST6	6
#define EXT_FID_TEST7	7

#define EXT_CID_TEST7	0x1007

void test7_callback(unsigned short id, NhExtXdr *request, NhExtXdr *reply)
{
    int i;
    nhext_rpc_params(request, 1, EXT_INT_P(i));
    nhext_rpc_params(reply, 1, EXT_INT(i * 5 + 3));
}

struct nhext_svc callbacks[] = {
    EXT_CID_TEST7,	test7_callback,
    0, NULL,
};

void svc_exit(unsigned short id, NhExtXdr *request, NhExtXdr *reply)
{
    server_exit = 1;
    nhext_rpc_params(reply, 0);
}

void svc_test1(unsigned short id, NhExtXdr *request, NhExtXdr *reply)
{
    nhext_rpc_params(reply, 0);
}

void svc_test2(unsigned short id, NhExtXdr *request, NhExtXdr *reply)
{
    int i;
    nhext_rpc_params(request, 1, EXT_INT_P(i));
    nhext_rpc_params(reply, 0);
}

void svc_test3(unsigned short id, NhExtXdr *request, NhExtXdr *reply)
{
    int i;
    nhext_rpc_params(request, 1, EXT_INT_P(i));
    nhext_rpc_params(reply, 1, EXT_INT(i + 1));
}

void svc_test4(unsigned short id, NhExtXdr *request, NhExtXdr *reply)
{
    char *s;
    char *buf;
    nhext_rpc_params(request, 1, EXT_STRING_P(s));
    buf = (char *)alloc(strlen(s) + 3);
    sprintf(buf, "<%s>", s);
    free(s);
    nhext_rpc_params(reply, 1, EXT_STRING(buf));
    free(buf);
}

struct test5_request {
    unsigned int n;
    long *array;
};

int svc_xdr_test5_request(xdrs, datum)
NhExtXdr *xdrs;
struct test5_request *datum;
{
    return nhext_xdr_array(xdrs, (char **)&datum->array, &datum->n,
      (unsigned int)-1, sizeof(long), nhext_xdr_long);
}

void svc_test5(unsigned short id, NhExtXdr *request, NhExtXdr *reply)
{
    int i;
    int total = 0;
    struct test5_request req = { 0, (long *)0 };
    nhext_rpc_params(request, 1, EXT_XDRF(svc_xdr_test5_request, &req));
    for(i = 0; i < req.n; i++)
	total += req.array[i];
    nhext_rpc_params(reply, 1, EXT_INT(total));
}

void svc_test6(unsigned short id, NhExtXdr *request, NhExtXdr *reply)
{
    int i;
    char *s;
    winid w;
    nhext_xdr_bool_t b;
    char c;
    nhext_rpc_params(request, 5, EXT_INT_P(i), EXT_INT_P(w), EXT_CHAR_P(c),
      EXT_STRING_P(s), EXT_BOOLEAN_P(b));
    nhext_rpc_params(reply, 5, EXT_INT(w), EXT_STRING(s), EXT_CHAR(c),
      EXT_INT(i), EXT_BOOLEAN(b));
    free(s);
}

void svc_test7(unsigned short id, NhExtXdr *request, NhExtXdr *reply)
{
    int i;
    nhext_rpc_params(request, 1, EXT_INT_P(i));
    nhext_rpc_c(server_connection,
      EXT_CID_TEST7, 1, EXT_INT(i), 1, EXT_INT_P(i));
    nhext_rpc_params(reply, 1, EXT_INT(i));
}

struct nhext_svc services[] = {
    EXT_FID_EXIT,	svc_exit,
    EXT_FID_TEST1,	svc_test1,
    EXT_FID_TEST2,	svc_test2,
    EXT_FID_TEST3,	svc_test3,
    EXT_FID_TEST4,	svc_test4,
    EXT_FID_TEST5,	svc_test5,
    EXT_FID_TEST6,	svc_test6,
    EXT_FID_TEST7,	svc_test7,
    0,			NULL,
};

int server_read(void *handle, void *buf, unsigned int len)
{
    return read((int)handle, buf, len);
}

int server_write(void *handle, void *buf, unsigned int len)
{
    return write((int)handle, buf, len);
}

void server(void)
{
    int i;
    NhExtIO *rd, *wr;
    rd = nhext_io_open(server_read, (void *)0, NHEXT_IO_RDONLY);
    wr = nhext_io_open(server_write, (void *)1, NHEXT_IO_WRONLY);
    if (!rd || !wr) {
	fprintf(stderr, "C Failed to open I/O streams.\n");
	exit(1);
    }
    server_connection = nhext_init(rd, wr, callbacks);
    if (server_connection < 0) {
	fprintf(stderr, "C Failed to initialize sub-protocol1.\n");
	exit(1);
    }
    do {
	i = nhext_svc_c(server_connection, services);
	if (!i)
	    impossible("Ignoring packet with zero ID");
    } while (!server_exit);
    nhext_end_c(server_connection);
    nhext_io_close(rd);
    nhext_io_close(wr);
}

void run_tests(void)
{
    int i, retval;
    char c, *s;
    nhext_xdr_bool_t b;
    winid w;
    int total;
    struct test5_request req;
    fprintf(stderr, "Test 1...\n");
    retval = nhext_rpc_c(connection, EXT_FID_TEST1, 0, 0);
    fprintf(stderr, "Test 1 %s.\n", retval ? "passed" : "failed");
    fprintf(stderr, "Test 2...\n");
    retval = nhext_rpc_c(connection, EXT_FID_TEST2, 1, EXT_INT(0), 0);
    fprintf(stderr, "Test 2 %s.\n", retval ? "passed" : "failed");
    fprintf(stderr, "Test 3...\n");
    retval = nhext_rpc_c(connection, EXT_FID_TEST3, 1, EXT_INT(67), 1, EXT_INT_P(i));
    if (i != 68)
	retval = FALSE;
    fprintf(stderr, "Test 3 %s.\n", retval ? "passed" : "failed");
    fprintf(stderr, "Test 4...\n");
    retval = nhext_rpc_c(connection, EXT_FID_TEST4, 1, EXT_STRING("Hello"),
      1, EXT_STRING_P(s));
    if (strcmp(s, "<Hello>"))
	retval = FALSE;
    fprintf(stderr, "Test 4 %s.\n", retval ? "passed" : "failed");
    fprintf(stderr, "Test 5...\n");
    req.n = 5;
    req.array = (long *)alloc(req.n * sizeof(long));
    for(i = 0; i < 5; i++)
	req.array[i] = 7 * i + 3;
    retval = nhext_rpc_c(connection, EXT_FID_TEST5,
      1, EXT_XDRF(svc_xdr_test5_request, &req), 1, EXT_INT_P(total));
    for(i = 0; i < 5; i++)
	total -= req.array[i];
    if (total)
	retval = FALSE;
    free(req.array);
    fprintf(stderr, "Test 5 %s.\n", retval ? "passed" : "failed");
    fprintf(stderr, "Test 6...\n");
    retval = nhext_rpc_c(connection, EXT_FID_TEST6,
      5, EXT_INT(37), EXT_INT(2), EXT_CHAR('l'), EXT_STRING("Shalom"),
         EXT_BOOLEAN(TRUE),
      5, EXT_INT_P(w), EXT_STRING_P(s), EXT_CHAR_P(c), EXT_INT_P(i),
         EXT_BOOLEAN_P(b));
    if (w != 2 || strcmp(s, "Shalom") || c != 'l' || i != 37 || !b)
	retval = FALSE;
    free(s);
    fprintf(stderr, "Test 6 %s.\n", retval ? "passed" : "failed");
    fprintf(stderr, "Test 7...\n");
    retval = nhext_rpc_c(connection, EXT_FID_TEST7, 1, EXT_INT(11), 1, EXT_INT_P(i));
    if (i != 58)
	retval = FALSE;
    fprintf(stderr, "Test 7 %s.\n", retval ? "passed" : "failed");
}

main(argc, argv)
int argc;
char **argv;
{
    NhExtIO *rd, *wr;
    if (argc > 1 && !strcmp(argv[1], "-c")) {
	is_child++;
	server();
	exit(0);
    }
    if (!child_start()) {
	fprintf(stderr, "Failed to start child.\n");
	exit(1);
    }
    rd = nhext_io_open(debug_read, (void *)to_parent[0], NHEXT_IO_RDONLY);
    wr = nhext_io_open(debug_write, (void *)to_child[1], NHEXT_IO_WRONLY);
    if (!rd || !wr) {
	fprintf(stderr, "Failed to open I/O streams.\n");
	exit(1);
    }
    connection = nhext_init(rd, wr, callbacks);
    if (connection < 0) {
	fprintf(stderr, "Failed to initialize sub-protocol1.\n");
	exit(1);
    }
    run_tests();
    nhext_rpc_c(connection, EXT_FID_EXIT, 0, 0);
    nhext_end_c(connection);
    nhext_io_close(rd);
    nhext_io_close(wr);
    if (!child_wait()) {
	fprintf(stderr, "Error while waiting for child.\n");
	exit(1);
    }
    exit(0);
}
