/* $Id: test_com.c,v 1.1.2.1 2004-11-18 22:38:10 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001-2004 */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * This module provides common routines used by the test programs.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "test_com.h"

int is_child = 0;

long *alloc(unsigned int nb)
{
    return malloc(nb);
}

void impossible(const char *fmt, ...)
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

void nhext_error_handler(int class, const char *error)
{
    int i;
    fputs("NhExt error: ", stderr);
    if (is_child)
	fputs("C: ", stderr);
    fputs(error, stderr);
    putc('\n', stderr);
    fflush(stderr);
    if (!is_child) {
	for(i=0;i<10 && !child_wait0();i++)
	    sleep(1);
    }
    exit(126);
}

#ifdef WIN32
#include <windows.h>
#include <fcntl.h>
#include <errno.h>

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
    if (!CreatePipe(&handles[0], &handles[1], &sa, 8192))
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

static HANDLE
dup_osf(int fd)
{
    HANDLE h, dh;
    h = (HANDLE)_get_osfhandle(fd);
    if (h == INVALID_HANDLE_VALUE)
	return h;
    if (!DuplicateHandle(GetCurrentProcess(), h, GetCurrentProcess(), &dh,
      0, FALSE, DUPLICATE_SAME_ACCESS))
	return INVALID_HANDLE_VALUE;
    return dh;
}

static int
redirect_to_osf(DWORD std_handle, HANDLE osf)
{
    int i, retval, fd, mode;
    switch(std_handle) {
	case STD_INPUT_HANDLE:
	    fd = 0;
	    mode = _O_RDONLY | _O_BINARY;
	    break;
	case STD_OUTPUT_HANDLE:
	    fd = 1;
	    mode = _O_WRONLY | _O_BINARY;
	    break;
	case STD_ERROR_HANDLE:
	    fd = 2;
	    mode = _O_WRONLY | _O_BINARY;
	    break;
	default:
	    errno = EINVAL;
	    return 0;
    }
    if (!SetStdHandle(std_handle, osf))
	return 0;
    i = _open_osfhandle((long)osf, mode);
    if (i < 0)
	return 0;
    retval = (dup2(i, fd) >= 0);
    close(i);
    return retval;
}

static void pipe_close(HANDLE *handles)
{
    CloseHandle(handles[0]);
    CloseHandle(handles[1]);
}

int child_start(const char *argv0)
{
    int fd, retval;
    char *cmdline;
    HANDLE save_stdin, save_stdout;
    STARTUPINFO si;
    cmdline = malloc(strlen(argv0) + 4);
    if (!cmdline)
	return FALSE;
    sprintf(cmdline, "%s -c", argv0);
    if (!pipe_create(to_parent, 0)) {
	free(cmdline);
	return FALSE;
    }
    if (!pipe_create(to_child, 1)) {
	pipe_close(to_parent);
	free(cmdline);
	return FALSE;
    }
    save_stdin = dup_osf(0);
    save_stdout = dup_osf(1);
    if (save_stdin == INVALID_HANDLE_VALUE ||
      save_stdout == INVALID_HANDLE_VALUE) {
	pipe_close(to_parent);
	pipe_close(to_child);
	free(cmdline);
	return FALSE;
    }
    if (!redirect_to_osf(STD_INPUT_HANDLE, to_child[0])) {
	pipe_close(to_parent);
	pipe_close(to_child);
	free(cmdline);
	return FALSE;
    }
    if (!redirect_to_osf(STD_OUTPUT_HANDLE, to_parent[1])) {
	redirect_to_osf(STD_INPUT_HANDLE, save_stdin);
	pipe_close(to_parent);
	pipe_close(to_child);
	free(cmdline);
	return FALSE;
    }
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    retval = CreateProcess(NULL,		/* No module name */
      cmdline,				/* Command line */
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
    free(cmdline);
    redirect_to_osf(STD_INPUT_HANDLE, save_stdin);
    redirect_to_osf(STD_OUTPUT_HANDLE, save_stdout);
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
unsigned int len;
{
    DWORD d;
    if (!ReadFile((HANDLE)handle, buf, len, &d, NULL)) {
	d = GetLastError();
	return d == ERROR_HANDLE_EOF || d == ERROR_BROKEN_PIPE ? 0 : -1;
    } else
	return d;
}

int child_write(handle, buf, len)
void *handle;
void *buf;
unsigned int len;
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

int parent_read(void *handle, void *buf, unsigned int len)
{
    DWORD d;
    if (!ReadFile((HANDLE)handle, buf, len, &d, NULL)) {
	d = GetLastError();
	return d == ERROR_HANDLE_EOF || d == ERROR_BROKEN_PIPE ? 0 : -1;
    } else
	return d;
}

int parent_write(void *handle, void *buf, unsigned int len)
{
    DWORD nb;
    if (!WriteFile((HANDLE)handle, buf, len, &nb, NULL))
	return -1;
    else
	return nb;
}

void *get_parent_readh()
{
    return (void *)_get_osfhandle(0);
}

void *get_parent_writeh()
{
    return (void *)_get_osfhandle(1);
}
#else	/* WIN32 */
#include <sys/types.h>
#include <sys/wait.h>

static int pid;
static int to_parent[2], to_child[2];

int child_start(const char *argv0)
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
	execl(argv0, argv0, "-c", NULL);
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
unsigned int len;
{
    int retval;
    retval = read((int)handle, buf, len);
    return retval >= 0 ? retval : -1;
}

int child_write(handle, buf, len)
void *handle;
void *buf;
unsigned int len;
{
    int retval;
    retval = write((int)handle, buf, len);
    return retval >= 0 ? retval : -1;
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
		fprintf(stderr,"%s", "Child died for unknown reason\n");
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

int parent_read(void *handle, void *buf, unsigned int len)
{
    int retval;
    retval = read((int)handle, buf, len);
    return retval >= 0 ? retval : -1;
}

int parent_write(void *handle, void *buf, unsigned int len)
{
    int retval;
    retval = write((int)handle, buf, len);
    return retval >= 0 ? retval : -1;
}

void *get_parent_readh()
{
    return (void *)0;
}

void *get_parent_writeh()
{
    return (void *)1;
}
#endif	/* WIN32 */

void *get_child_readh()
{
    return (void *)to_parent[0];
}

void *get_child_writeh()
{
    return (void *)to_child[1];
}

int debug_read(handle, buf, len)
void *handle;
void *buf;
unsigned int len;
{
    int i, retval;
    long l;
    unsigned char *bp = buf;
    retval = child_read(handle, buf, len);
    if (retval == -2)
	fputs("<- PENDING\n", stderr);
    else if (retval < 0)
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
