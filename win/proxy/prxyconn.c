/* $Id: prxyconn.c,v 1.9 2003-12-01 17:44:04 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2002-2003 */
/* NetHack may be freely redistributed.  See license for details. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef WIN32
#include <windows.h>
#include <process.h>
#include <fcntl.h>
#include <errno.h>
#else
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif
#include "nhxdr.h"
#include "proxycom.h"
#include "prxyclnt.h"

#ifdef WIN32

/* Mingw uses slightly different names to the platform SDK */

#if defined(WSAEINVALIDPROCTABLE) && !defined(WSAINVALIDPROCTABLE)
#define WSAINVALIDPROCTABLE WSAEINVALIDPROCTABLE
#endif
#if defined(WSAEINVALIDPROVIDER) && !defined(WSAINVALIDPROVIDER)
#define WSAINVALIDPROVIDER WSAEINVALIDPROVIDER
#endif
#if defined(WSAEPROVIDERFAILEDINIT) && !defined(WSAPROVIDERFAILEDINIT)
#define WSAPROVIDERFAILEDINIT WSAEPROVIDERFAILEDINIT
#endif

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
	proxy_clnt_error("standard input: %s", strerror(errno));
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
	proxy_clnt_error("Failed to connect pipe to game and standard I/O");
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

static int
client_read_file(void *handle, void *buf, unsigned int len)
{
    DWORD d;
    if (!ReadFile((HANDLE)handle, buf, len, &d, NULL)) {
	d = GetLastError();
	return d == ERROR_HANDLE_EOF || d == ERROR_BROKEN_PIPE ? 0 : -1;
    } else
	return d;
}

static int
client_write_file(void *handle, void *buf, unsigned int len)
{
    DWORD nb;
    if (!WriteFile((HANDLE)handle, buf, len, &nb, NULL))
	return -1;
    else
	return nb;
}

static int
client_read_skt(void *handle, void *buf, unsigned int len)
{
    int nb;
    nb = recv((SOCKET)handle, buf, len, 0);
    if (nb == SOCKET_ERROR)
	return -1;
    else
	return nb;
}

static int
client_write_skt(void *handle, void *buf, unsigned int len)
{
    int nb;
    nb = send((SOCKET)handle, buf, len, 0);
    if (nb == SOCKET_ERROR)
	return -1;
    else
	return nb;
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
	    proxy_clnt_error("select: %s", strerror(errno));
	    return 0;
	}
	if (FD_ISSET(0, &readfds)) {
	    nb = read(0, buffer, sizeof(buffer));
	    if (nb <= 0) {
		if (nb < 0)
		    proxy_clnt_error("standard in: %s", strerror(errno));
		return nb == 0;
	    } else
		write((int)wfd, buffer, nb);
	}
	if (FD_ISSET((int)rfd, &readfds)) {
	    nb = read((int)rfd, buffer, sizeof(buffer));
	    if (nb <= 0) {
		if (nb < 0)
		    proxy_clnt_error("pipe from game: %s", strerror(errno));
		return nb == 0;
	    } else
		write(1, buffer, nb);
	}
    }
}

static int
client_read(void *handle, void *buf, unsigned int len)
{
    int nb;
    nb = read((int)handle, buf, len);
    return nb >= 0 ? nb : -1;
}

static int
client_write(void *handle, void *buf, unsigned int len)
{
    int nb;
    nb = write((int)handle, buf, len);
    return nb >= 0 ? nb : -1;
}

#endif /* WIN32 */

static int exit_client_services;

void
proxy_exit_client_services()
{
    exit_client_services = 1;
}

int
proxy_init_client_services(nhext_io_func read_f, void *read_h,
  nhext_io_func write_f, void *write_h)
{
    int nb;
    char *s;
    if (!win_proxy_clnt_init(read_f, read_h, write_f, write_h)) {
	proxy_clnt_error("Proxy interface failed; switching to text mode");
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
	s = win_proxy_clnt_get_failed_packet(&nb);
	if (s)
	    write(1, s, nb);
	connect_pipe(read_h, write_h);
	return 1;
    }
    return 0;
}

void
proxy_start_client_services()
{
    exit_client_services = 0;
    while(win_proxy_clnt_iteration() >= 0 && !exit_client_services)
	;
}

#ifdef WIN32

static int
proxy_connect_file(char *address, int *argcp, char **argv)
{
    char *filename, *dirname, *save_wd;
    char **nargv;
    int i, pid;
    HANDLE to_game_h[2], from_game_h[2], save_stdin, save_stdout;
    nargv = (char **)alloc((*argcp + 2) * sizeof(char *));
    filename = strrchr(address, '\\');
    if (filename) {
	i = 1024;
	save_wd = malloc(i);
	*save_wd = '\0';
	while(!getcwd(save_wd, i) && errno == ERANGE) {
	    free(save_wd);
	    i *= 2;
	    save_wd = malloc(i);
	    *save_wd = '\0';
	}
	filename++;
	i = filename - address;
	dirname = malloc(i);
	if (!dirname) {
	    proxy_clnt_error("Not enough memory");
	    return 1;
	}
	strncpy(dirname, address, i - 1);
	dirname[i - 1] = '\0';
	if (chdir(dirname)) {
	    proxy_clnt_error("Can't change to directory %s", dirname);
	    return 1;
	}
	free(dirname);
	nargv[0] = filename;
    } else {
	nargv[0] = address;
	save_wd = NULL;
    }
    nargv[1] = "--proxy";
    for(i = 1; i <= *argcp; i++)
	nargv[i + 1] = argv[i];
    if (!pipe_create(to_game_h, 1) || !pipe_create(from_game_h, 0)) {
	proxy_clnt_error("Failed to create pipes");
	return 1;
    }
    save_stdin = (HANDLE)_get_osfhandle(0);
    if (save_stdin != INVALID_HANDLE_VALUE) {
	save_stdin = dup_osf(0);
	if (save_stdin == INVALID_HANDLE_VALUE) {
	    proxy_clnt_error("Failed to save stdin");
	    return 1;
	}
    }
    if (!redirect_to_osf(STD_INPUT_HANDLE, to_game_h[0])) {
	proxy_clnt_error("Failed to redirect stdin");
	return 1;
    }
    save_stdout = (HANDLE)_get_osfhandle(1);
    if (save_stdout != INVALID_HANDLE_VALUE) {
	save_stdout = dup_osf(1);
	if (save_stdout == INVALID_HANDLE_VALUE) {
	    proxy_clnt_error("Failed to save stdout");
	    return 1;
	}
    }
    if (!redirect_to_osf(STD_OUTPUT_HANDLE, from_game_h[1])) {
	proxy_clnt_error("Failed to redirect stdout");
	return 1;
    }
    pid = spawnv(P_NOWAIT, nargv[0], nargv);
    if (pid < 0) {
	proxy_clnt_error("%s: %s", nargv[0], strerror(errno));
	return 1;
    }
    free(nargv);
    if (save_stdin == INVALID_HANDLE_VALUE) {
	SetStdHandle(STD_INPUT_HANDLE, INVALID_HANDLE_VALUE);
	close(0);
    } else if (!redirect_to_osf(STD_INPUT_HANDLE, save_stdin)) {
	proxy_clnt_error("Failed to restore stdin");
	return 1;
    }
    if (save_stdout == INVALID_HANDLE_VALUE) {
	SetStdHandle(STD_OUTPUT_HANDLE, INVALID_HANDLE_VALUE);
	close(1);
    } else if (!redirect_to_osf(STD_OUTPUT_HANDLE, save_stdout)) {
	proxy_clnt_error("Failed to restore stdout");
	return 1;
    }
    if (save_wd && *save_wd)
	chdir(save_wd);
    if (save_wd)
	free(save_wd);
    return proxy_init_client_services(client_read_file, (void *)from_game_h[0],
      client_write_file, (void *)to_game_h[1]);
}

static const char *strerror_WSA(int errnum)
{
    int i;
    static char buf[30];
    struct {
	int errnum;
	const char *err;
    } errors[] = {
	WSAEACCES, "Perminssion denied",
	WSAEADDRINUSE, "Address already in use",
	WSAEADDRNOTAVAIL, "Cannot assign requested address",
	WSAEAFNOSUPPORT, "Address family not supported by protocol family",
	WSAEALREADY, "Operation already in progress",
	WSAECONNABORTED, "Software caused connection abort",
	WSAECONNREFUSED, "Connection refused",
	WSAECONNRESET, "Connection reser by peer",
	WSAEDESTADDRREQ, "Destination address required",
	WSAEFAULT, "Bad address",
	WSAEHOSTDOWN, "Host is down",
	WSAEHOSTUNREACH, "No route to host",
	WSAEINPROGRESS, "Operation now in progress",
	WSAEINTR, "Interrupted function call",
	WSAEINVAL, "Invalid argument",
	WSAEISCONN, "Socket is already connected",
	WSAEMFILE, "Too many open files",
	WSAEMSGSIZE, "Message too long",
	WSAENETDOWN, "Network is down",
	WSAENETRESET, "Network dropped connection on reset",
	WSAENETUNREACH, "Network is unreachable",
	WSAENOBUFS, "No buffer space available",
	WSAENOPROTOOPT, "Bad protocol option",
	WSAENOTCONN, "Socket is not connected",
	WSAENOTSOCK, "Socket operation on nonsocket",
	WSAEOPNOTSUPP, "Operation not supported",
	WSAEPFNOSUPPORT, "Protocol family not supported",
	WSAEPROCLIM, "Too many processes",
	WSAEPROTONOSUPPORT, "Protocol not supported",
	WSAEPROTOTYPE, "Protocol wrong for socket",
	WSAESHUTDOWN, "Cannot send after socket shutdown",
	WSAESOCKTNOSUPPORT, "Socket type not supported",
	WSAETIMEDOUT, "Connection timed out",
	WSATYPE_NOT_FOUND, "Class type not found",
	WSAEWOULDBLOCK, "Resource temporarily unavailable",
	WSAHOST_NOT_FOUND, "Host not found",
	WSA_INVALID_HANDLE, "Specified event object handle is invalid",
	WSA_INVALID_PARAMETER, "One or more parameters are invalid",
	WSAINVALIDPROCTABLE, "Invalid procedure table from service provider",
	WSAINVALIDPROVIDER, "Invalid service provider version number",
	WSA_IO_INCOMPLETE, "Overlapped I/O event object not in signaled state",
	WSA_IO_PENDING, "Overlapped operations will complete later",
	WSA_NOT_ENOUGH_MEMORY, "Insufficient memory available",
	WSANOTINITIALISED, "Successful WSAStartup not yet performed",
	WSANO_DATA, "Valid name, no data record of requested type",
	WSANO_RECOVERY, "This is a nonrecoverable error",
	WSAPROVIDERFAILEDINIT, "Unable to initialize a service provider",
	WSASYSCALLFAILURE, "System call failure",
	WSASYSNOTREADY, "Network subsystem is unavailable",
	WSATRY_AGAIN, "Nonauthoritive host found",
	WSAVERNOTSUPPORTED, "Winsock.dll version out of range",
	WSAEDISCON, "Graceful shutdown in progress",
	WSA_OPERATION_ABORTED, "Overlapped operation aborted",
    };
    for(i = 0; i < sizeof(errors)/sizeof(*errors); i++)
	if (errors[i].errnum == errnum)
	    return errors[i].err;
    sprintf(buf, "Unknown error %d", errnum);
    return buf;
}

static int
proxy_connect_tcp(char *address)
{
    SOCKET skt;
    WSADATA wsa_data;
    int i, port;
    long iaddr;
    char *s, *err;
    struct hostent *he;
    struct sockaddr_in sa;
    s = strrchr(address, ':');
    if (!s) {
	proxy_clnt_error("Missing port number in tcp scheme");
	return 1;
    }
    port=atoi(s + 1);
    if (!port) {
	proxy_clnt_error("Illegal port number in tcp scheme");
	return 1;
    }
    i = s - address;
    s = (char *)alloc(i + 1);
    strncpy(s, address, i);
    s[i] = '\0';
    switch (WSAStartup(MAKEWORD(2, 0), &wsa_data))
    {
	case 0:
	    err = NULL;
	    break;
	case WSASYSNOTREADY:
	    err = "Network subsystem not ready";
	    break;
	case WSAVERNOTSUPPORTED:
	    err = "WinSock API: Version 2.0 not supported";
	    break;
	case WSAEINPROGRESS:
	    err = "WinSock API: Blocking operation in progress";
	    break;
	case WSAEPROCLIM:
	    err = "WinSock API: Too many tasks";
	    break;
	case WSAEFAULT:
	    err = "WinSock API: Application error";
	    break;
	default:
	    err = "WinSock API: Unknown error";
	    break;
    }
    if (err) {
	proxy_clnt_error(err);
	return 1;
    }
    skt = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (skt == INVALID_SOCKET) {
	proxy_clnt_error("Failed to create socket: %s",
	  strerror_WSA(WSAGetLastError()));
	return 1;
    }
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    iaddr = inet_addr(s);
    if (iaddr == INADDR_NONE) {
	he = gethostbyname(s);
	if (!he) {
	    proxy_clnt_error("Can't resolve hostname: %s",
	      strerror_WSA(WSAGetLastError()));
	    return 1;
	}
	iaddr = *((long *)he->h_addr);
    }
    sa.sin_addr.S_un.S_addr = iaddr;
    if (connect(skt, (struct sockaddr *)&sa, sizeof(sa))) {
	proxy_clnt_error("Failed to connect to remote machine: %s",
	  strerror_WSA(WSAGetLastError()));
	return 1;
    }
    free(s);
    return proxy_init_client_services(client_read_skt, (void *)skt,
      client_write_skt, (void *)skt);
}

#else /* WIN32 */

static int
proxy_connect_file(char *address, int *argcp, char **argv)
{
    int i;
    char **nargv = NULL;
    int to_game[2],from_game[2];
    if (pipe(to_game) || pipe(from_game)) {
	proxy_clnt_error("Can't create NhExt stream");
	return 1;
    }
    nargv = (char **)alloc((*argcp + 2) * sizeof(char *));
    nargv[0] = address;
    nargv[1] = "--proxy";
    for(i = 1; i <= *argcp; i++)
	nargv[i + 1] = argv[i];
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
	return proxy_init_client_services(client_read, (void *)from_game[0],
	  client_write, (void *)to_game[1]);
    }
    /*NOTREACHED*/
    return 1;
}

static int
proxy_connect_tcp(char *address)
{
    int skt;
    int i, port;
    long iaddr;
    char *s, *err;
    struct hostent *he;
    struct sockaddr_in sa;
    s = strrchr(address, ':');
    if (!s) {
	proxy_clnt_error("Missing port number in tcp scheme");
	return 1;
    }
    port=atoi(s + 1);
    if (!port) {
	proxy_clnt_error("Illegal port number in tcp scheme");
	return 1;
    }
    i = s - address;
    s = (char *)alloc(i + 1);
    strncpy(s, address, i);
    s[i] = '\0';
    skt = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (skt < 0) {
	proxy_clnt_error("Failed to create socket: %s", strerror(errno));
	return 1;
    }
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    iaddr = inet_addr(s);
    if (iaddr == INADDR_NONE) {
	he = gethostbyname(s);
	if (!he) {
	    extern int h_errno;
	    proxy_clnt_error("Can't resolve hostname: %s",
	      hstrerror(h_errno));
	    return 1;
	}
	iaddr = *((long *)he->h_addr);
    }
    sa.sin_addr.s_addr = iaddr;
    if (connect(skt, (struct sockaddr *)&sa, sizeof(sa))) {
	proxy_clnt_error("Failed to connect to remote machine: %s",
	  strerror(errno));
	return 1;
    }
    free(s);
    return proxy_init_client_services(client_read, (void *)skt,
      client_write, (void *)skt);
}

#endif	/* WIN32 */

/*
 * Currently supported schemes:
 *
 *	Scheme		Description			Address
 *	-------------------------------------------------------
 *	file		Run program on this computer	filename
 *	tcp		Connect to remote computer	host:port
 */

int
proxy_connect(char *scheme, char *address, int *argcp, char **argv)
{
    if (!strcmp(scheme, "file"))
	return proxy_connect_file(address, argcp, argv);
    else if (!strcmp(scheme, "tcp"))
	return proxy_connect_tcp(address);
    else
	proxy_clnt_error("Unsupported scheme: %s", scheme);
    return 1;
}
