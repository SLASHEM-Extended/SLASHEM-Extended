/* $Id: nhextnb.c,v 1.1 2003-10-25 18:06:01 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2003 */
/* NetHack may be freely redistributed.  See license for details. */

/* NhExt: Non-blocking support using threads & mutexes */

/* #define DEBUG */

#include <stdlib.h>
#ifdef DEBUG
#include <stdio.h>
#endif
#include "nhxdr.h"

#ifdef WIN32
#include <windows.h>
#include <process.h>

static int check_res(int res)
{
    return res == WAIT_ABANDONED || res == WAIT_OBJECT_0 ? 1 :
      res == WAIT_TIMEOUT ? 0 : -1;
}
#define DEFINE_LOCK(mutex)	HANDLE mutex
#define INIT_LOCK(mutex)	(mutex = CreateMutex(NULL, FALSE, NULL))
#define AQUIRE_LOCK_(mutex)	check_res(WaitForSingleObject(mutex, INFINITE))
#define TRY_LOCK_(mutex)	check_res(WaitForSingleObject(mutex, 0))
#define RELEASE_LOCK_(mutex)	ReleaseMutex(mutex)
#define FREE_LOCK(mutex)	CloseHandle(mutex)
#else	/* WIN32 */
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#define DEFINE_LOCK(mutex)	pthread_mutex_t mutex
#define INIT_LOCK(mutex)	(!pthread_mutex_init(&(mutex), NULL))
#define AQUIRE_LOCK_(mutex)	(!pthread_mutex_lock(&(mutex)))
#define TRY_LOCK_(mutex)	(!pthread_mutex_trylock(&(mutex)))
#define RELEASE_LOCK_(mutex)	(!pthread_mutex_unlock(&(mutex)))
#define FREE_LOCK(mutex)	(!pthread_mutex_destroy(&(mutex)))
#endif

#ifdef DEBUG
#ifdef WIN32
#define debug_line(str)		fprintf(stderr, "[%X] %s\n", \
				  GetCurrentThreadId(), str)
#else
#define debug_line(str)		fprintf(stderr, "%s\n", str)
#endif
static int debug_line_res(int res, const char *mutex, const char *verb)
{
    char buf[100];
    if (res == 1)
	sprintf(buf, "Lock %s %sd", mutex, verb);
    else if (res == 0)
	sprintf(buf, "Lock %s not %sd", mutex, verb);
    else {
	sprintf(buf, "%s of lock %s produced error", verb, mutex);
	if (buf[0] >= 'a' && buf[0] <= 'z') {
	    buf[0] -= 'a';
	    buf[0] += 'A';
	}
    }
    debug_line(buf);
    return res;
}
#define AQUIRE_LOCK(mutex)	(debug_line("Aquiring lock " #mutex), \
				  debug_line_res(AQUIRE_LOCK_(mutex), \
				  #mutex, "aquire"))
#define TRY_LOCK(mutex)		debug_line_res(TRY_LOCK_(mutex), \
				  #mutex, "aquire")
#define RELEASE_LOCK(mutex)	debug_line_res(RELEASE_LOCK_(mutex), \
				  #mutex, "release")
#else
#define AQUIRE_LOCK(mutex)	AQUIRE_LOCK_(mutex)
#define TRY_LOCK(mutex)		TRY_LOCK_(mutex)
#define RELEASE_LOCK(mutex)	RELEASE_LOCK_(mutex)
#endif

#define NHEXT_NB_PENDING	1
#define NHEXT_NB_CLOSED		2
#define NHEXT_NB_ERROR		4

struct NhExtNB_ {
    unsigned int flags;
    nhext_io_func func;
    void *handle;
#ifdef WIN32
    HANDLE thread;
#else
    pthread_t thread;
#endif
    DEFINE_LOCK(m_A);
    DEFINE_LOCK(m_B);
    DEFINE_LOCK(m_C);
    struct {
	void *buffer;
	int bytes;
    } cmd;
    int res;
};

#ifdef WIN32
static unsigned  __stdcall read_thread(void *data)
{
    NhExtNB *nb = (NhExtNB *)data;
#else
static void *read_thread(void *data)
{
    NhExtNB *nb = (NhExtNB *)data;
#endif
    void *buffer = data;		/* Any non-zero value */
    int bytes;
#ifdef DEBUG
    debug_line("read_thread starts");
#endif
    if (AQUIRE_LOCK(nb->m_B)) {
	for(;;) {
	    if (!AQUIRE_LOCK(nb->m_C))
		break;
	    buffer = nb->cmd.buffer;
	    bytes = nb->cmd.bytes;
	    if (!RELEASE_LOCK(nb->m_B))
		break;
	    if (!AQUIRE_LOCK(nb->m_A))
		break;
	    if (!RELEASE_LOCK(nb->m_C))
		break;
	    if (!buffer)
		break;
	    if (!AQUIRE_LOCK(nb->m_B))
		break;
#ifdef DEBUG
	    debug_line("Issuing read call");
#endif
	    nb->res = (*nb->func)(nb->handle, buffer, bytes);
#ifdef DEBUG
	    debug_line("Read call returns");
#endif
	    if (!RELEASE_LOCK(nb->m_A))
		break;
	}
    }
    (void)RELEASE_LOCK(nb->m_A);
    if (!buffer) {
	/* Controlled exit - we're responsible for cleaning up */
	(void)FREE_LOCK(nb->m_A);
	(void)FREE_LOCK(nb->m_B);
	(void)FREE_LOCK(nb->m_C);
#ifdef WIN32
	CloseHandle(nb->thread);
#endif
#ifdef DEBUG
	debug_line("read_thread terminates");
#endif
	free(nb);
	return 0;
    } else {
	(void)RELEASE_LOCK(nb->m_B);
	(void)RELEASE_LOCK(nb->m_C);
#ifdef DEBUG
	debug_line("read_thread aborts");
#endif
#ifdef WIN32
	return 1;
#else
	return (void *)1;
#endif
    }
}

/*
 * Sequence of events for a non-blocking read:
 *
 *	Read thread			Main thread
 *	-----------			-----------
 *	B  C Waiting for cmd		AC - nhext_nb_read called
 *	B  C Waiting for cmd		AC - Writes cmd
 *	BC - Copying cmd		A  B Issues cmd
 *	C  A				AB -
 *	AC -				B  C Waiting for cmd to be actioned
 *	A  B Ready to action cmd	BC -
 *	AB - In underlying system	C  - Returns to caller
 *	AB - In underlying system	C  A nhext_nb_read re-called
 *	B  - Result recorded		AC - Result available to caller
 *
 * (The first column for each thread is the locks currently aquired. The second
 * column is the locks currently being waited for.)
 *
 * Sequence of events for a blocking read:
 *
 *	Read thread			Main thread
 *	-----------			-----------
 *	B  C Waiting for cmd		AC - nhext_nb_read called
 *	B  C Waiting for cmd		AC - Writes cmd
 *	BC - Copying cmd		A  B Issues cmd
 *	C  A				AB -
 *	AC -				B  C Waiting for cmd to be actioned
 *	A  B Ready to action cmd	BC -
 *	AB - In underlying system	C  A Waiting for results
 *	B  - Result recorded		AC - Returns to caller
 *
 * The mutexes also protect certain fields as follows:
 *	A - res
 *	B -
 *	C - cmd
 */

NhExtNB *nhext_nb_open(nhext_io_func func, void *handle)
{
    NhExtNB *nb;
    int retval;
    nb = malloc(sizeof(*nb));
    if (!nb) {
#ifdef DEBUG
	debug_line("nhext_nb_open failing (not enough memory)");
#endif
	return NULL;
    }
    nb->func = func;
    nb->handle = handle;
    if (!INIT_LOCK(nb->m_A)) {
	free(nb);
#ifdef DEBUG
	debug_line("nhext_nb_open failing (can't init lock A)");
#endif
	return NULL;
    }
    if (!INIT_LOCK(nb->m_B)) {
	FREE_LOCK(nb->m_A);
#ifdef DEBUG
	debug_line("nhext_nb_open failing (can't init lock B)");
#endif
	free(nb);
	return NULL;
    }
    if (!INIT_LOCK(nb->m_C)) {
	FREE_LOCK(nb->m_A);
	FREE_LOCK(nb->m_B);
#ifdef DEBUG
	debug_line("nhext_nb_open failing (can't init lock C)");
#endif
	free(nb);
	return NULL;
    }
    if (!AQUIRE_LOCK(nb->m_A) || !AQUIRE_LOCK(nb->m_C)) {
#ifdef DEBUG
	debug_line("nhext_nb_open failing (can't aquire locks A & C)");
#endif
	goto out;
    }
    nb->flags = 0;
#ifdef WIN32
    nb->thread = (HANDLE)_beginthreadex(NULL, 0, read_thread, nb, 0, NULL);
    if (!nb->thread) {
#else
    if (pthread_create(&nb->thread, NULL, read_thread, nb) == EAGAIN) {
#endif
#ifdef DEBUG
	debug_line("nhext_nb_open failing (can't create read thread)");
#endif
out:	RELEASE_LOCK(nb->m_A);
	RELEASE_LOCK(nb->m_C);
	FREE_LOCK(nb->m_A);
	FREE_LOCK(nb->m_B);
	FREE_LOCK(nb->m_C);
	free(nb);
	return NULL;
    }
    /*
     * We must wait for the read thread to start and aquire the B mutex
     * or the synchronization will fail.
     */
    while ((retval = TRY_LOCK(nb->m_B)) > 0) {
	if (!RELEASE_LOCK(nb->m_B)) {
#ifdef WIN32
	    TerminateThread(nb->thread, -1);
	    CloseHandle(nb->thread);
#else
	    pthread_cancel(nb->thread);
#endif
#ifdef DEBUG
	    debug_line("nhext_nb_open failing (can't release lock B)");
#endif
	    goto out;
	}
#ifdef WIN32
	Sleep(0);			/* Relinquish time slice */
#else
# ifdef _POSIX_PRIORITY_SCHEDULING
	sched_yield();			/* Relinquish time slice */
# else
	sleep(0);
# endif
#endif
    }
    if (retval < 0) {
	nhext_nb_close(nb);
#ifdef DEBUG
	debug_line("nhext_nb_open failing (error in trying to aquire lock B)");
#endif
	return NULL;
    }
    return nb;
}

int nhext_nb_close(NhExtNB *nb)
{
    int retval;
    if (nb->flags & NHEXT_NB_CLOSED)
	return -1;
    nb->flags |= NHEXT_NB_CLOSED;
    /*
     * Mutex B should always be owned by the read thread unless we have aborted
     * in the middle of a call to nhext_nb_read.
     */
    retval = TRY_LOCK(nb->m_B);
    if (retval == 1) {
#ifdef WIN32
	DWORD code;
	if (GetExitCodeThread(nb->thread, &code) == STILL_ACTIVE) {
	    /*
	     * Something has gone drastically wrong. Clean up as best we can.
	     */
	    TerminateThread(nb->thread, 1);
	    code = 1;
	}
#else
	void *code;
	if (pthread_join(nb->thread, &code)) {
	    /*
	     * Something has gone drastically wrong. Clean up as best we can.
	     */
	    pthread_cancel(nb->thread);
	    code = (void *)1;
	}
#endif
	if (code) {
	    /* Read thread has aborted. Clean up */
#ifdef WIN32
	    CloseHandle(nb->thread);
#endif
	    RELEASE_LOCK(nb->m_A);
	    RELEASE_LOCK(nb->m_B);
	    RELEASE_LOCK(nb->m_C);
	    FREE_LOCK(nb->m_A);
	    FREE_LOCK(nb->m_B);
	    FREE_LOCK(nb->m_C);
	    free(nb);
	} else {
	    /* read thread has terminated and cleaned up (shouldn't happen) */
	    RELEASE_LOCK(nb->m_B);
	}
	return -1;
    }
    nb->cmd.buffer = NULL;
    nb->cmd.bytes = 0;
    if (!(nb->flags & NHEXT_NB_PENDING))
	RELEASE_LOCK(nb->m_A);
    RELEASE_LOCK(nb->m_C);
    /* The read thread is responsible for cleaning up - if a read is pending
     * this will be after it finishes.
     */
    return 0;
}

int nhext_nb_read(NhExtNB *nb, char *buf, int bytes, int blocking)
{
    int retval;
#ifdef DEBUG
    debug_line("nhext_nb_read called");
#endif
    if (nb->flags & NHEXT_NB_ERROR)
	return -1;
    if (!(nb->flags & NHEXT_NB_PENDING)) {
	nb->cmd.buffer = buf;
	nb->cmd.bytes = bytes;
	if (!RELEASE_LOCK(nb->m_C) || !AQUIRE_LOCK(nb->m_B) ||
	  !RELEASE_LOCK(nb->m_A) || !AQUIRE_LOCK(nb->m_C) ||
	  !RELEASE_LOCK(nb->m_B)) {
	    nb->flags |= NHEXT_NB_ERROR;
#ifdef DEBUG
	    debug_line("nhext_nb_read failing with hard error");
#endif
	    return -1;
	}
	if (!blocking) {
	    nb->flags |= NHEXT_NB_PENDING;
#ifdef DEBUG
	    debug_line("nhext_nb_read returning PENDING");
#endif
	    return -2;
	} else {
	    if (!AQUIRE_LOCK(nb->m_A)) {
		nb->flags |= NHEXT_NB_ERROR;
#ifdef DEBUG
		debug_line("nhext_nb_read failing with hard error");
#endif
		return -1;
	    }
#ifdef DEBUG
	    debug_line("nhext_nb_read returns result");
#endif
	    return nb->res >= 0 ? nb->res : -1;
	}
    } else {
	if (buf != nb->cmd.buffer || bytes < nb->cmd.bytes) {
#ifdef DEBUG
	    debug_line("nhext_nb_read failing with soft error (INVALID)");
#endif
	    return -1;
	}
	if (!blocking) {
	    retval = TRY_LOCK(nb->m_A);
	    if (retval == 1) {
		nb->flags &= ~NHEXT_NB_PENDING;
#ifdef DEBUG
		debug_line("nhext_nb_read returns result");
#endif
		return nb->res >= 0 ? nb->res : -1;
	    } else if (retval) {
		nb->flags |= NHEXT_NB_ERROR;
#ifdef DEBUG
		debug_line("nhext_nb_read failing with hard error");
#endif
		return -1;
	    }
	    else {
#ifdef DEBUG
		debug_line("nhext_nb_read returning PENDING");
#endif
		return -2;
	    }
	} else {
	    if (!AQUIRE_LOCK(nb->m_A)) {
		nb->flags |= NHEXT_NB_ERROR;
#ifdef DEBUG
		debug_line("nhext_nb_read failing with hard error");
#endif
		return -1;
	    }
	    nb->flags &= ~NHEXT_NB_PENDING;
#ifdef DEBUG
	    debug_line("nhext_nb_read returns result");
#endif
	    return nb->res >= 0 ? nb->res : -1;
	}
    }
}
