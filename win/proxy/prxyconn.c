/* $Id: prxyconn.c,v 1.1 2002-12-23 22:59:03 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2002 */
/* NetHack may be freely redistributed.  See license for details. */

#include <stdlib.h>
#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#include <process.h>
#include <fcntl.h>
#include <errno.h>
#endif
#include "nhxdr.h"
#include "proxycom.h"
#include "proxysvr.h"

#ifdef WIN32

/* Create an anonymous pipe with one end inheritable. */
static int
pipe_create(HANDLE *handles, int non_inherit)
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

/* The connect pipe thread deals with copying data from standard input
 * to the pipe, leaving the main thread to deal with reading from the
 * pipe and writing the result to standard output.
 *
 * Note that earlier versions of NetHack/Slash'EM assume that standard
 * input is a console. These will enter a tight loop if they attempt
 * to read input from our pipe. The user will need to use Ctrl-C to
 * interrupt both processes if this happens.
 */

static void
connect_pipe_thread(LPVOID lpv_param)
{
    int nb;
    DWORD nbw;
    char buffer[1024];
    HANDLE h = (HANDLE)lpv_param;
    while((nb = read(0, buffer, sizeof(buffer))) > 0)
	WriteFile(h, buffer, nb, &nbw, NULL);
    /* We wouldn't normally expect to get either EOF or ERROR from
     * standard input, but if we do then report any error and exit.
     */
    if (nb)
	perror("standard input");
    CloseHandle(h);
}

static int
connect_pipe(void *rh, void *wh)
{
    HANDLE thread;
    DWORD thread_id;
    DWORD nbr;
    char buffer[1024];
    thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)connect_pipe_thread,
      (LPVOID)wh, 0, &thread_id);
    if (thread == INVALID_HANDLE_VALUE) {
	fprintf(stderr, "Failed to connect pipe to game and standard I/O\n");
	return 0;
    }
    while (ReadFile((HANDLE)rh, buffer, sizeof(buffer), &nbr, NULL) && nbr)
	write(1, buffer, (int)nbr);
    /* After EOF/ERROR from the pipe, we no longer need to forward standard
     * input to the game so terminate the thread.
     */
    TerminateThread(thread, 0);
    CloseHandle(thread);
    return 1;
}

#else /* WIN32 */

static int
connect_pipe(void *rfd, void *wfd)
{
    int nb;
    fd_set readfds;
    char buffer[1024];
    for(;;) {
	FD_ZERO(&readfds);
	FD_SET(0, &readfds);
	FD_SET((int)rfd, &readfds);
	if (select((int)rfd + 1, &readfds, NULL, NULL, NULL) < 0) {
	    perror("select");
	    return 0;
	}
	if (FD_ISSET(0, &readfds)) {
	    nb = read(0, buffer, sizeof(buffer));
	    if (nb <= 0) {
		if (nb < 0)
		    perror("standard in");
		return nb == 0;
	    } else
		write((int)wfd, buffer, nb);
	}
	if (FD_ISSET((int)rfd, &readfds)) {
	    nb = read((int)rfd, buffer, sizeof(buffer));
	    if (nb <= 0) {
		if (nb < 0)
		    perror("pipe from game");
		return nb == 0;
	    } else
		write(1, buffer, nb);
	}
    }
}

#endif /* WIN32 */

void
proxy_start_server(char *prgname, void *read_h, void *write_h)
{
    int nb;
    char *s;
    if (!win_proxy_svr_init(read_h, write_h))
	panic("%s: Failed to start sub-protocol1", prgname);
    while(win_proxy_svr_iteration() >= 0)
	;
    fprintf(stderr, "%s: Proxy interface failed; switching to text mode\n",
      prgname);
    /*
     * Most likely cause of failing to start the proxy interface is that
     * either the game executable we called doesn't support proxy interfaces
     * or that an error occured during initialisation. The assumption is
     * that the output from the game is actually a message intended for the
     * user. In either case, we are best to retrieve the failed packet from
     * the nhext module and display it. Then we can connect the games's
     * standard I/O to the user for them to interpret and take appropriate
     * action.
     */
    s = win_proxy_svr_get_failed_packet(&nb);
    if (s)
	write(1, s, nb);
    connect_pipe(read_h, write_h);
}

/*
 * Currently supported protocols:
 *
 *	Protocol	Description			Address
 *	-------------------------------------------------------
 *	file		Run program on this computer	filename
 */

#ifdef WIN32

void
proxy_connect(char *protocol, char *address, int *argcp, char **argv)
{
    char *filename;
    char **nargv;
    int i, pid;
    HANDLE to_game_h[2], from_game_h[2], save_stdin, save_stdout;
    nargv = (char **)alloc((*argcp + 2) * sizeof(char *));
    if (!strcmp(protocol, "file"))
	nargv[0] = address;
    else
	panic("proxy_connect: Unsupported protocol: %s", protocol);
    nargv[1] = "--proxy";
    for(i = 1; i <= *argcp; i++)
	nargv[i + 1] = argv[i];
    if (!pipe_create(to_game_h, 1) || !pipe_create(from_game_h, 0)) {
	fprintf(stderr, "%s: Failed to create pipes\n", argv[0]);
	return;
    }
    save_stdin = dup_osf(0);
    if (save_stdin == INVALID_HANDLE_VALUE) {
	fprintf(stderr, "%s: Failed to save stdin\n", argv[0]);
	return;
    }
    if (!redirect_to_osf(STD_INPUT_HANDLE, to_game_h[0])) {
	fprintf(stderr, "%s: Failed to redirect stdin\n", argv[0]);
	return;
    }
    save_stdout = dup_osf(1);
    if (save_stdout == INVALID_HANDLE_VALUE) {
	fprintf(stderr, "%s: Failed to save stdout\n", argv[0]);
	return;
    }
    if (!redirect_to_osf(STD_OUTPUT_HANDLE, from_game_h[1])) {
	fprintf(stderr, "%s: Failed to redirect stdout\n", argv[0]);
	return;
    }
    pid = spawnv(P_NOWAIT, nargv[0], nargv);
    if (pid < 0) {
	perror(nargv[0]);
	return;
    }
    free(nargv);
    if (!redirect_to_osf(STD_INPUT_HANDLE, save_stdin)) {
	fprintf(stderr, "%s: Failed to restore stdin\n", argv[0]);
	return;
    }
    if (!redirect_to_osf(STD_OUTPUT_HANDLE, save_stdout)) {
	fprintf(stderr, "%s: Failed to restore stdout\n", argv[0]);
	return;
    }
    proxy_start_server(argv[0], (void *)from_game_h[0], (void *)to_game_h[1]);
}

#else /* WIN32 */

void
proxy_connect(char *protocol, char *address, int *argcp, char **argv)
{
    int i;
    char **nargv = NULL;
    int to_game[2],from_game[2];
    if (pipe(to_game) || pipe(from_game))
	panic("%s: Can't create NhExt stream", argv[0]);
    if (!strcmp(protocol, "file")) {
	nargv = (char **)alloc((*argcp + 2) * sizeof(char *));
	nargv[0] = address;
	nargv[1] = "--proxy";
	for(i = 1; i <= *argcp; i++)
	    nargv[i + 1] = argv[i];
    }
    else
	panic("proxy_connect: Unsupported protocol: %s", protocol);
    if (!fork()) {
	dup2(to_game[0],0);
	dup2(from_game[1],1);
	close(to_game[1]);
	close(from_game[0]);
	execvp(nargv[0], nargv);
	perror(nargv[0]);
	_exit(127);
    } else {
	free(nargv);
	close(to_game[0]);
	close(from_game[1]);
	proxy_start_server(argv[0], (void *)from_game[0], (void *)to_game[1]);
    }
}

#endif	/* WIN32 */
